
/*!
 * Copyright (c) 2005, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 * \file    drivers.c
 * \brief   This is the SMAC C source driver file for the HC(S)08
 * MCU and MC13192 transceiver.
 * \author  r01160
 * \version 4.1a
 * \date    2005/07/29 03:30:40 
 * 
 * \b Description:
 *
 * The driver controls all interfaces to/from the MC13192 by the SPI,
 * selected interrupt source and MCU GPIO's assigned to other MC13192
 * pins.The SMAC driver is the lowest layer of C code.
 * Actual hardware interconnects from the MCU to the MC13192 are 
 * defined in the MC13192_hw_config.h header file.
 *
 * \b Department: Freescale Radio Products Division 
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 16/11/2005 Doc. update to Doxygen compliant by Mario Rodriguez r12369c
 */ 
 
#include "mcu_hw_config.h"
#include "pub_def.h"
#include "drivers.h"
#include "simple_phy.h"
#include "MC13192_hw_config.h"
#include "MC13192_regs.h"

/* Globals */
tRxPacket *psDrvRxPacket;           //!< Where the packet received will be stored 
tCCAMeasurement sDrvCCAReading;     //!< NOT USED  
extern UINT8 gu8RTxMode;						//!< Global mode mirror. MC13192 Status. 
UINT8 gu8IRQValue = 0;							//!< IRQ value  
extern tPhyOptions gsPhyOptions;
volatile UINT16 u16StatusContent;

/*!
 * \brief  MC13192 initiated IRQ interrupt handler.
 * The vector is defined in vectortable.c
 *
 * \return The interrupt will RTI unless valid data is recvd.
 * In this case a PDDataIndication function call-back will be 
 * executed first, followed by a RTI.
 *
 */
interrupt void  IRQIsr(void)   
{
  
    /* Result of the status register read */
    volatile UINT16 u16StatusContent, u16Reg, u16Temp;   
    /* Data length for the RX packet */
    UINT8 u8DataLength = 0;     
    UINT8 u8MinLen = 0;
    #if defined (HCS08G) | defined (HCS08R)
    /* Acknowledge the interrupt. MC13192 IRQ pin still low.*/
    CLEAR_IRQ_FLAG();   
    #endif
    u16StatusContent = SPIDrvRead(STATUS_ADDR); /* 
                                                 * Read the MC13192 status 
                                                 * register.
                                                 */
    /* Test for IRQ glitch or possible fast double IRQ. */

    if(IRQPinLow())       //__isflag_int_enabled
    {        
   
        CLEAR_IRQ_FLAG();   /* Acknowledge the interrupt. MC13192 IRQ pin still low. */     
        u16Reg = SPIDrvRead(STATUS_ADDR);   /* Read the MC13192 status register. */
        u16StatusContent = u16StatusContent | u16Reg;   /* 
                                                         * Actual status is the OR 
                                                         * of both.
                                                         */
    }   
    
    if (gsPhyOptions.Bits.u1PromiscuousMode)
      u8MinLen = 3;
    else
      u8MinLen = 5;

    u16StatusContent &= TX_IRQ_MASK | RX_IRQ_MASK | ATTN_IRQ_MASK | \
                        CRC_VALID_MASK | TIMER1_IRQ_MASK | CCA_IRQ_MASK | \
                        LO_LOCK_IRQ_MASK | DOZE_IRQ_MASK | CCA_BIT_MASK;
                        
    if (gu8RTxMode != RX_MODE_WTO) {
        /* If timeout not being used, mask out timer. */
        u16StatusContent &= ~(TIMER1_IRQ_MASK);
    }
    
    if ((u16StatusContent & ~(CRC_VALID_MASK)) == 0) {
        /* If no status bits are set just return. */
        return;
    }
    
    /* DOZE Complete Interrupt */
    if ((u16StatusContent & DOZE_IRQ_MASK) != 0) {
        SPIDrvWrite(T2_HI_ADDR, 0x8000); /* disable the timer2 */
        u16Temp= SPIDrvRead(MODE2_ADDR);
        u16Temp &= 0xFFFC;
        SPIDrvWrite(MODE2_ADDR, u16Temp);/* disable doze/hibernate modes */      
        gu8RTxMode = IDLE_MODE;        
    }  
    
    /* ATTN IRQ Handler*/
    if ((u16StatusContent & ATTN_IRQ_MASK) != 0) {
       /* 
        * If attn interrupt, set the rtx_state mirror.
        * For MC13192 V2.x devices, read the reset indication in R25/7 first. 
        * If a reset is indicated, call back to a reset handler. 
        */
        u16StatusContent = SPIDrvRead(RESIND_ADDR); /* 
                                                     * Read the MC13192 reset 
                                                     * indicator register.
                                                     */
        u16StatusContent &= RESET_BIT_MASK;
        if (u16StatusContent == 0) {            /* Reset */
            gu8RTxMode = MC13192_RESET_MODE;    /* 
                                                 * Set the rtx_state mirror to 
                                                 * idle with attn.
                                                 */
            PLMEMC13192ResetIndication();
            return;
        }
        else {
            gu8RTxMode = IDLE_MODE_ATTN;    /* 
                                             * Set the rtx_state mirror to idle
                                             * with attn.
                                             */
            return;
        }
    }
    /* TIMER1 IRQ Handler (Used for receiver timeout notification) */
    if ((u16StatusContent & TIMER1_IRQ_MASK) != 0) {
        /* If timeout ocurs (and timeout enabled), update mode mirror state. */  
        if (gu8RTxMode == RX_MODE_WTO) {
            /* Clear Timer1 if in RX_MODE_WTO */
            SPIDrvWrite(T1_HI_ADDR, 0x8000); /* Disables TC1 and clears IRQ */
            SPIDrvWrite(T1_LO_ADDR, 0x0000);
            psDrvRxPacket->u8Status = TIMEOUT;
            RTXENDeAssert();                 /* Forces the MC13192 to idle. */
            gu8RTxMode = IDLE_MODE;
            PDDataIndication();              /* TIMEOUT notification */
            return;
        }
    }
    /* LO LOCK IRQ - Occurs when MC13192 loses LOCK */
    /* For receive cycles, re-enable. For transmit/CCA, abort. */
    if ((u16StatusContent & LO_LOCK_IRQ_MASK) != 0) {
        RTXENDeAssert(); /* Forces the MC13192 to idle. */
        /* Unlock from receive cycles */
        if ((gu8RTxMode == RX_MODE) || (gu8RTxMode == RX_MODE_WTO) || \
            (gu8RTxMode == CCA_MODE) || (gu8RTxMode == PULSE_TX_MODE)) {
            /* Read the MC13192 trx register. Timer trigger off. */
            u16StatusContent = (SPIDrvRead(MODE_ADDR) & 0xFF7F); 
            SPIDrvWrite(MODE_ADDR, u16StatusContent);   /* 
                                                         * Re-write the trx 
                                                         * register.
                                                         */  
            RTXENAssert();          /* Re-start the sequence. */
        }
        else {
            gu8RTxMode = IDLE_MODE;
        }
        return;
    }
    if ((gu8RTxMode == IDLE_MODE) || ((u16StatusContent & CCA_IRQ_MASK) != 0) || \
       ((u16StatusContent & TX_IRQ_MASK) != 0)) {
        /* If in idle mode already or if CCA or TX is done, just return. */
        RTXENDeAssert();            /* Forces the MC13192 to idle. */
        
        
        /* Check for Pulse TX Mode for PRBS9 Test mode sequence */
        if (gu8RTxMode == PULSE_TX_MODE) {
                u16StatusContent = (SPIDrvRead(MODE_ADDR) & 0xFF7F); /* Read the MC13192 trx register. Timer trigger off. */
                SPIDrvWrite(MODE_ADDR, u16StatusContent); /* Re-write the trx register. */  
                  RTXENAssert(); /* Re-start the sequence. */
            
                #if defined (PA)
            MC13192_PA_CTRL = PA_OFF;       //Turn off the PA when TX is complete
            #endif
            return;
        }
        
        gu8RTxMode = IDLE_MODE;
        return;
    }
    /* If rx is done */
    if ((u16StatusContent & RX_IRQ_MASK) != 0) 
    {  
        RTXENDeAssert();            /* Forces the MC13192 to idle. */
        if (((u16StatusContent & CRC_VALID_MASK) == 0) && ((gsPhyOptions.Bits.u1PromiscuousMode == FALSE))) 
        {
            /* If an invalid CRC, restart receiver. */
            /* Read the MC13192 trx register. Timer trigger off. */
            u16StatusContent = (SPIDrvRead(MODE_ADDR) & 0xFF7F); 
            /* Update the trx register */          
            SPIDrvWrite(MODE_ADDR, u16StatusContent); 
            RTXENAssert(); /* Forces the MC13192 to enter the receive mode. */
//            psDrvRxPacket->u8Status = CRC_ERROR;
//            PDDataIndication(); /* Notify PHY that there is data available. */            
            return;
        }
        else {
            /* Read received packet length register and mask off length bits */
            u8DataLength = (UINT8) (SPIDrvRead(RX_PKT_LEN) & 0x7F);
if(TRUE) {
  
            if (u8DataLength < u8MinLen) /* Rx_pkt_length is bad when <5 because of CRC and byte codes. */
            {
                /* Read the MC13192 trx register. Timer trigger off. */                
                u16StatusContent = (SPIDrvRead(MODE_ADDR) & 0xFF7F); 
                /* Update the trx register. */ 
                SPIDrvWrite(MODE_ADDR, u16StatusContent);
                /* Forces the MC13192 to enter the receive mode. */
                RTXENAssert();  
                return;
            }
            /* Test the Byte Codes */
            if (gsPhyOptions.Bits.u1PromiscuousMode == 0)
            {
              u16Temp = SPIDrvRead2(RX_PKT); /* Get the code bytes */
              
              if ((u16Temp != 0xFF7E) && ((gsPhyOptions.Bits.u1PromiscuousMode == FALSE)))
              {
                  /* Read the MC13192 trx register. Timer trigger off. */                
                  u16StatusContent = (SPIDrvRead(MODE_ADDR) & 0xFF7F); 
                  /* Update the trx register. */ 
                  SPIDrvWrite(MODE_ADDR, u16StatusContent);
                  /* Forces the MC13192 to enter the receive mode. */
                  RTXENAssert();  
                  return;
              }
              
            }
}
            /* A valid packet has been received. */
            gu8RTxMode = IDLE_MODE;             /* Set the rtx_state to idle */
            SPIDrvWrite(T1_HI_ADDR, 0x8000);    /* 
                                                 * Disables TC1 and clears the 
                                                 * IRQ. 
                                                 */
            SPIDrvWrite(T1_LO_ADDR, 0x0000);
            psDrvRxPacket->u8DataLength = u8DataLength;
            psDrvRxPacket->u8Status = SUCCESS;
            PDDataIndication(); /* Notify PHY that there is data available. */
            return;
        }
    }
}


/*!
 * \brief SPIDrvWrite : Write 1 word to SPI
 *
 * \param u8Addr - SPI address
 * \param u16Content - Data to send
 *
 */
void SPIDrvWrite(UINT8 u8Addr, UINT16 u16Content)
{
  UINT8 u8TempValue=0;

  SPIClearRecieveStatReg(&u8TempValue); /* Clear status register (possible SPRF, SPTEF) */  
  SPIClearRecieveDataReg(&u8TempValue); /* 
                             * Clear receive data register. SPI entirely ready 
                             * for read or write 
                             */                       
  MC13192DisableInterrupts();   /* Necessary to prevent double SPI access */
  AssertCE();                   /* Enables MC13192 SPI */
  SPISendChar(u8Addr & 0x3F);   /*
                                 * Mask address, 6bit addr. 
                                 * Set write bit (i.e. 0). 
                                 */
  SPIWaitTransferDone();        /* 
                                 * For this bit to be set, SPTED MUST be set.
                                 * Now write content MSB
                                 */
  SPIClearRecieveDataReg(&u8TempValue);     /* 
                                 * Clear receive data register. SPI entirely 
                                 * ready for read or write 
                                 */
  SPISendChar((UINT8)(u16Content >> 8));    /* Write MSB */       
  SPIWaitTransferDone();        /* 
                                 * For this bit to be set, SPTED MUST be set. 
                                 * Now write content LSB 
                                 */
  SPIClearRecieveDataReg(&u8TempValue);     /* 
                                 * Clear receive data register. SPI entirely 
                                 * ready for read or write
                                 */
  SPISendChar((UINT8)(u16Content & 0x00FF));    /* Write LSB */
  SPIWaitTransferDone();        /* 
                                 * For this bit to be set, SPTED MUST be set.
                                 * Now read last of garbage
                                 */
  SPIClearRecieveDataReg(&u8TempValue);     /* 
                                 * Clear receive data register. SPI entirely 
                                 * ready for read or write 
                                 */
  DeAssertCE();                 /* Disables MC13192 SPI */
  MC13192RestoreInterrupts();   /* Restore MC13192 interrupt status */
}


/*!
 * \brief SPIDrvRead : Read 1 word from SPI
 *
 * \param u8Addr - SPI address
 *
 * \return u16Data -  u16Data[0] is the MSB, u16Data[1] is the LSB
 */
UINT16 SPIDrvRead(UINT8 u8Addr)
{
    UINT8 u8TempValue=0;
    UINT16  u16Data=0;            /* u16Data[0] is MSB, u16Data[1] is LSB */

    SPIClearRecieveStatReg(&u8TempValue);  /* Clear status register (possible SPRF, SPTEF) */  
    SPIClearRecieveDataReg(&u8TempValue);  /* 
                                 * Clear receive data register. SPI entirely 
                                 * ready for read or write
                                 */                       
    MC13192DisableInterrupts(); /* Necessary to prevent double SPI access */
    AssertCE();                 /* Enables MC13192 SPI */
    SPISendChar((u8Addr & 0x3f) | 0x80);   /* Mask address, 6bit addr, Set 
                                             * read bit. 
                                             */
    SPIWaitTransferDone();      /* For this bit to be set, SPTED MUST be set */
    SPIClearRecieveDataReg(&u8TempValue);   /* 
                                 * Clear receive data register. SPI 
                                 * entirely ready for read or write 
                                 */
    SPISendChar(u8Addr );       /*
                                 * Dummy write. Receive register of SPI 
                                 * will contain MSB
                                 */
    SPIWaitTransferDone();      /* 
                                 * For this bit to be set, SPTED MUST be 
                                 * set. Get MSB
                                 */
    SPIReadMSB(&u16Data);               /* MSB */
    SPISendChar(u8Addr);             /*
                                       * Dummy write. Waiting until after 
                                       * reading received data insures no 
                                       * overrun 
                                       */
    SPIWaitTransferDone();            /* 
                                       * For this bit to be set, SPTED MUST be 
                                       * set. Get LSB
                                       */
    ((UINT8*)&u16Data)[1] = SPIRead();    /* LSB */
    DeAssertCE();                     /* Disables MC13192 SPI */
    MC13192RestoreInterrupts();       /* Restore MC13192 interrupt status */
    return u16Data;
}

/*!
 * \brief SPIDrvRead2 : Read the second word from SPI
 *
 * \param u8Addr - SPI address
 *
 * \return u16Data -  u16Data[0] is the MSB, u16Data[1] is the LSB
 */
UINT16 SPIDrvRead2(UINT8 u8Addr)
{
    UINT8 u8TempValue=0;
    UINT16  u16Data=0;            /* u16Data[0] is MSB, u16Data[1] is LSB */

    SPIClearRecieveStatReg(&u8TempValue);  /* Clear status register (possible SPRF, SPTEF) */  
    SPIClearRecieveDataReg(&u8TempValue);  /* 
                                 * Clear receive data register. SPI entirely 
                                 * ready for read or write
                                 */                       
    MC13192DisableInterrupts(); /* Necessary to prevent double SPI access */
    AssertCE();                 /* Enables MC13192 SPI */
    SPISendChar((u8Addr & 0x3f) | 0x80);   /* Mask address, 6bit addr, Set 
                                             * read bit. 
                                             */
    SPIWaitTransferDone();      /* For this bit to be set, SPTED MUST be set */
    SPIClearRecieveDataReg(&u8TempValue);   /* 
                                 * Clear receive data register. SPI 
                                 * entirely ready for read or write 
                                 */
    SPISendChar(u8Addr );       /*
                                 * Dummy write. Receive register of SPI 
                                 * will contain MSB
                                 */
    SPIWaitTransferDone();      /* 
                                 * For this bit to be set, SPTED MUST be 
                                 * set. Get MSB
                                 */
    SPIReadMSB(&u16Data);               /* MSB */
    SPISendChar(u8Addr);             /*
                                       * Dummy write. Waiting until after 
                                       * reading received data insures no 
                                       * overrun 
                                       */
    SPIWaitTransferDone();            /* 
                                       * For this bit to be set, SPTED MUST be 
                                       * set. Get LSB
                                       */
    SPIReadLSB(&u16Data);               /* LSB */
    SPISendChar(u8Addr);             /*
                                       * Dummy write. Waiting until after 
                                       * reading received data insures no 
                                       * overrun 
                                       */
    SPIWaitTransferDone();            /* 
                                       * For this bit to be set, SPTED MUST be 
                                       * set. Get LSB
                                       */
    SPIReadMSB(&u16Data);               /* MSB */
    SPISendChar(u8Addr);             /*
                                       * Dummy write. Waiting until after 
                                       * reading received data insures no 
                                       * overrun 
                                       */
    SPIWaitTransferDone();            /* 
                                       * For this bit to be set, SPTED MUST be 
                                       * set. Get LSB
                                       */
    SPIReadLSB(&u16Data);               /* LSB */
    DeAssertCE();                     /* Disables MC13192 SPI */
    MC13192RestoreInterrupts();       /* Restore MC13192 interrupt status */
    return u16Data;
}


/*!
 * \brief RAMDrvWriteTx : Write a block of data to TX packet RAM, whichever is selected
 *
 * \param *psTxPkt - Packet to write
 *
 */
void RAMDrvWriteTx(tTxPacket *psTxPkt)
{
    UINT8 i, u8TempByte=0, u8TempValue=0;   /* Temporary counters */ 
    UINT8 u8MinLen=0;

    UINT16  u16Reg=0;                     /* TX packet length register value */

    if (gsPhyOptions.Bits.u1PromiscuousMode)
      u8MinLen = 3;
    else
      u8MinLen = 5;

    u16Reg = SPIDrvRead(TX_PKT_LEN);    /* 
                                         * Read the TX packet length register 
                                         * contents 
                                         */
    u16Reg = (0xFF80 & u16Reg) | (psTxPkt->u8DataLength + (u8MinLen - 1)); /* 
                                                            * Mask out old 
                                                            * length setting and
                                                            * update. Add 2 for
                                                            * CRC and 2 for code bytes
                                                            */
    SPIDrvWrite(TX_PKT_LEN, u16Reg);    /* Update the TX packet length field */
    SPIClearRecieveStatReg(&u8TempValue);/* Clear status register 
                                         * (possible SPRF, SPTEF) 
                                         */
    SPIClearRecieveDataReg(&u8TempValue);   /* 
                                 * Clear receive data register. SPI entirely
                                 * ready for read or write
                                 */                       
    MC13192DisableInterrupts(); /* Necessary to prevent double SPI access */
    AssertCE();                 /* Enables MC13192 SPI */
    SPISendChar(TX_PKT);        /* SPI TX ram data register */
    SPIWaitTransferDone();      /* 
                                 * For this bit to be set, SPTED MUST be set. 
                                 * Now write content MSB 
                                 */
    SPIClearRecieveDataReg(&u8TempValue);   /* 
                                 * Clear receive data register. SPI entirely 
                                 * ready for read or write
                                 */
    if (!gsPhyOptions.Bits.u1PromiscuousMode)
    {
      
      SPISendChar(0xFF);          /* Send 1st Code Byte */
      SPIWaitTransferDone();      /* 
                                   * For this bit to be set, SPTED MUST be set. 
                                   * Now write content MSB 
                                   */
      SPISendChar(0x7E);          /* Send 2nd Code Byte */
      SPIWaitTransferDone();      /* 
                                   * For this bit to be set, SPTED MUST be set. 
                                   * Now write content MSB 
                                   */
      SPIClearRecieveDataReg(&u8TempValue);   /* 
                                   * Clear receive data register. SPI entirely 
                                   * ready for read or write
                                   */
    }
    u8TempByte = 0;             /* Byte counter for *contents */
    /* Word loop. Round up. */ 
    for (i=0; i<((psTxPkt->u8DataLength+1) >> 1); i++) { 
        SPISendChar(psTxPkt->pu8Data[u8TempByte + 1]);  /* Write MSB */
        SPIWaitTransferDone();  /* 
                                 * For this bit to be set, SPTED MUST be set. 
                                 * Now write content LSB 
                                 */
        SPIClearRecieveDataReg(&u8TempValue);/* 
                                 * Clear receive data register. SPI entirely
                                 * ready for read or write
                                 */
        SPISendChar(psTxPkt->pu8Data[u8TempByte]);     /* Write LSB */          
        u8TempByte = (UINT8)(u8TempByte + 2);           /* Increment byte counter */
        SPIWaitTransferDone();  /* For this bit to be set, SPTED MUST be set.*/
        SPIClearRecieveDataReg(&u8TempValue);/* 
                                 * Clear receive data register. SPI entirely 
                                 * ready for read or write
                                 */
    }
    DeAssertCE();                   /* Disables MC13192 SPI */
    MC13192RestoreInterrupts();     /* Restore MC13192 interrupt status */
}


/*!
 * \brief RAMDrvReadRx : Read a block of data from RX packet RAM, whichever is selected
 *
 * \param *psRxPkt - Packet received
 *
 * \return u8Status
 */
UINT8 RAMDrvReadRx(tRxPacket *psRxPkt)
{
    UINT8 i, u8TempByte=0;        /* Temporary counters. */
    UINT8 u8MinLen=0;
    UINT8 u8TempValue=0;          /* 
                                 * Used by SPIClearRecieveDataReg to 
                                 * flush SPI1D register during read
                                 */
    UINT8  u8Status = 0;        /* Holder for the return value */
    UINT16 u16RxLength;
    
    if (gsPhyOptions.Bits.u1PromiscuousMode)
      u8MinLen = 3;
    else
      u8MinLen = 5;
  
    u16RxLength = SPIDrvRead(RX_PKT_LEN);   /* 
                                             * Read the RX packet length 
                                             * register contents 
                                             */
    u16RxLength &= 0x007F;          /* Mask out all but the RX packet length */

    /* MC13192 reports length with 2 CRC bytes and 2 Code bytes,
     * remove them. 
     */
    if (u16RxLength >= u8MinLen) {
        psRxPkt->u8DataLength = u16RxLength - (u8MinLen - 1); /*
                                                  * Less byte code
                                                  * and CRC
                                                  */
    }
    else {
        psRxPkt->u8DataLength = 0;
    }
        
    if ((psRxPkt->u8DataLength >= 1) && \
        (psRxPkt->u8DataLength <= psRxPkt->u8MaxDataLength)) { /* 
                                                          * If <5, the packet 
                                                          * is garbage
                                                          */
        SPIClearRecieveStatReg(&u8TempValue);   /* Clear status register (SPRF, SPTEF) */  
        SPIClearRecieveDataReg(&u8TempValue);   /* 
                                     * Clear receive data register. 
                                     * SPI entirely ready for read or write
                                     */                       
        MC13192DisableInterrupts(); /* Necessary to prevent double SPI access */
        AssertCE();                 /* Enables MC13192 SPI */
        SPISendChar(RX_PKT | 0x80); /* SPI RX ram data register */
        SPIWaitTransferDone();      /* For this bit to be set, SPTED 
                                     * MUST be set.
                                     */
        SPIClearRecieveDataReg(&u8TempValue);   /* 
                                     * Clear receive data register. 
                                     * SPI entirely ready for read or write 
                                     */
        SPISendChar(u8TempValue);   /* 
                                     * Dummy write. Receive register of SPI will 
                                     * contain MSB garbage for first read 
                                     */
        SPIWaitTransferDone();      /* For this bit to be set, SPTED MUST be set.*/
        SPIClearRecieveDataReg(&u8TempValue);   /* 
                                     * Clear receive data register. 
                                     * SPI entirely ready for read or write 
                                     */
        SPISendChar(u8TempValue);   /* 
                                     * Dummy write. Receive register of SPI will 
                                     * contain LSB garbage for first read 
                                     */
        SPIWaitTransferDone();      /* For this bit to be set, SPTED MUST be set.*/
        SPIClearRecieveDataReg(&u8TempValue);   /* 
                                     * Clear receive data register. 
                                     * SPI entirely ready for read or write 
                                     */
        if (gsPhyOptions.Bits.u1PromiscuousMode == 0) 
        {
          
                                      /* Byte codes */
          SPISendChar(u8TempValue);   /* 
                                       * Dummy write. Receive register of SPI will 
                                       * contain LSB garbage for first read 
                                       */
          SPIWaitTransferDone();      /* For this bit to be set, SPTED MUST be set.*/
          u8TempValue = SPIRead();    /* Read first code byte */
          SPISendChar(u8TempValue);   /* 
                                       * Dummy write. Receive register of SPI will 
                                       * contain LSB garbage for first read 
                                       */
          SPIWaitTransferDone();      /* For this bit to be set, SPTED MUST be set.*/
          u8TempValue = SPIRead();    /* Read second code byte */

        }
        u8TempByte = 0;             /* Byte counter for *contents */
        /* Word loop. Round up. Deduct CRC. */
        for (i=0; i<((u16RxLength-1)>>1); i++) { 
            SPISendChar(u8TempValue);/* 
                                     * Dummy write. Receive register of SPI 
                                     * will contain MSB
                                     */
            SPIWaitTransferDone();  /* 
                                     * For this bit to be set, SPTED MUST be 
                                     * set. Get MSB
                                     */
            /* For a trailing garbage byte, just read and discard */
            if ((u8TempByte + 3) == u16RxLength) { 
                SPIClearRecieveDataReg(&u8TempValue);                  /* Discard */
            }
            else {
                psRxPkt->pu8Data[u8TempByte + 1] = SPIRead();   /* Read MSB */
            }
            SPISendChar(u8TempValue);/*
                                     * Dummy write. Receive register of SPI 
                                     * will contain LSB
                                     */
            SPIWaitTransferDone();  /* 
                                     * For this bit to be set, SPTED MUST be
                                     * set. Get LSB
                                     */
            psRxPkt->pu8Data[u8TempByte] = SPIRead();   /* Read LSB */
            u8TempByte = (UINT8)(u8TempByte + 2);       /* Increment byte counter */
        }
        DeAssertCE();               /* Disables MC13192 SPI */
        psRxPkt->u8Status = SUCCESS;
        MC13192RestoreInterrupts(); /* Restore MC13192 interrupt status */
    }
    /* Check to see if a larger packet than desired is received. */  
    if (psRxPkt->u8DataLength > psRxPkt->u8MaxDataLength) {
        psRxPkt->u8Status = OVERFLOW;
    }
    return u8Status;  
}

