/*!
 * \mainpage SMAC 4.1a
 * 
 * Copyright (c) 2004, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b Department: Freescale Radio Products Division
 *
 

 */
 
/*!
 * \file simple_mac.c
 *
 * \brief  This is the SMAC C code for the MAC layer  
 * 
 * \author a19259
 * 
 * \version 4.1a
 *
 * \date 2005/07/29 02:27:50 
 *
 * \b Description: This is the SMAC C source media (i.e. MAC) layer file for
 *                  the HC(S)08 MCU and MC13192 transceiver.
 *                  The SMAC MAC is the highest layer of C code for the SMAC.*
 *

 * \b Department: Freescale Radio Products Division 
 *
 * \b Project: SMAC (Simple Media Access Controller) 
 *
 * \b History: 
 * - 16/11/2005 Doc. update to Doxygen compliant by Laura Delgado r12370c
 * 
 */

#include "pub_def.h"
#include "drivers.h"       
#include "simple_phy.h"
#include "simple_mac.h"
#include "MC13192_regs.h"

/* Externals */ 
extern tRxPacket *psDrvRxPacket;//!< Where the packet received will be stored 
extern UINT8 gu8RTxMode;        //!< Global mode mirror. MC13192 Status.  
extern tPhyOptions *psPhyOptions;
/* Version string to put in NVM. Note! size limits */
/* Normally it shoud be enough to change the version numbers. */
#define Database_Label_Version  "1.00"
#define MAC_Version             "4.20"
#define MAC_Label               "SMAC "
#define SMAC_VerComplete        "MAC " MAC_Label " Ver " MAC_Version " Build: " __DATE__ " " __TIME__
#define SMAC_VerSize            sizeof(SMAC_VerComplete)


#if defined (HCS08G) | defined (HCS08R)
#pragma MESSAGE DISABLE C3303   /* 
                                 * Warning C3303: Implicit concatenation of 
                                 * strings 
                                 */
#pragma MESSAGE DISABLE C4200   /* 
                                 * Warning C4200: Other segment than in 
                                 * previous declaration
                                 */

#pragma CONST_SEG BOOTLOADER_MAC_NV_DATA0
#endif



/* DO NOT CHANGE OR REMOVE */
/* These strings will be located in the NV RAM0 section.*/
/* Note!!Check that items are location in the same sequence as specified. */
const unsigned char Freescale_Copyright[54] = "(c) Copyright 2004 Freescale Inc. All rights reserved";
const unsigned char Firmware_Database_Label[40] = {"DB Label: SMAC4.1c 1321x 1319xVer " Database_Label_Version};
const unsigned char SMAC_Version[SMAC_VerSize] =  SMAC_VerComplete;

#if defined (HCS08G) | defined (HCS08R)
#pragma CONST_SEG DEFAULT
#endif

/*!
 * \fn UINT8 MCPSDataRequest (tTxPacket *psTxPacket)
 *
 * \brief Transmit data packet
 *
 * \param *psTxPacket packet pointer
 *
 * \return Status
 */
UINT8 MCPSDataRequest(tTxPacket *psTxPacket)
{
    UINT8 u8Status;
    
    /* Send it to the phy for processing */
    u8Status = PDDataRequest(psTxPacket);
    return u8Status;
}

/*
 * MCPSDataIndication : Receive data packet indication
 *
 * Parameters : Data packet pointer
 *
 * Return : None
 *
 * Notes : This function return should be located in the application 
 */


/*!
 * \fn UINT8 MLMEHibernateRequest(void)
 *
 * \brief Hibernate the MC13192 (very low current, no CLKO)
 *
 * \return Status
 */

UINT8 MLMEHibernateRequest(void)
{
    UINT8 u8Status = 0;
    
    u8Status = PLMEHibernateRequest();
    return u8Status;
}



/*!
 * \fn UINT8 MLMEDozeRequest(UINT32 u32Timeout)
 *
 * \brief Doze the MC13192 (Low current, CLKO <= 1MHz)
 *
 * \param u32Timeout 32-bit time period
 * 
 * \return Status
 */

UINT8 MLMEDozeRequest(UINT32 u32Timeout)
{
    UINT8 u8Status = 0;
    
    u8Status = PLMEDozeRequest(u32Timeout);
    return u8Status;
}

/*!
 * \fn UINT8 MLMEWakeRequest(void)
 *
 * \brief Wake the MC13192 from Hibernate or Doze
 *
 * \return Status
 */

UINT8 MLMEWakeRequest(void)
{
    UINT8 u8Status = 0;
    if (gu8RTxMode == DOZE_MODE || gu8RTxMode == HIBERNATE_MODE)
    {
        u8Status = PLMEWakeRequest();
        return u8Status;
    }
    return ERROR;
}

 
 /*!
 * \fn UINT8 MLMESetChannelRequest(UINT8 u8Channel)
 *
 * \brief Set the MC13192 operating channel
 *
 * \param u8Channel Channel number (0-15)   \n
 * Channel frequencies :                    \n
 *  0 : 2.405GHz                            \n
 *  1 : 2.410GHz                            \n
 *  2 : 2.415GHz                            \n
 *  3 : 2.420GHz                            \n
 *  4 : 2.425GHz                            \n
 *  5 : 2.430GHz                            \n
 *  6 : 2.435GHz                            \n
 *  7 : 2.440GHz                            \n
 *  8 : 2.445GHz                            \n
 *  9 : 2.450GHz                            \n
 *  10: 2.455GHz                            \n
 *  11: 2.460GHz                            \n
 *  12: 2.465GHz                            \n
 *  13: 2.470GHz                            \n
 *  14: 2.475GHz                            \n
 *  15: 2.480GHz                            \n
 *                                          
 * \return Status                           
 */                                         
UINT8 MLMESetChannelRequest(UINT8 u8Channel)
{
    UINT8 u8Status = 0;
    
    u8Status = PLMESetChannelRequest(u8Channel);
    return u8Status;
}


/*!
 * \fn UINT8 MLMERXEnableRequest(tRxPacket *psRxPacket, UINT32 u32Timeout)
 *
 * \brief Set the MC13192 receiver ON (with optional timeout)
 *
 * \param *psRxPacket Packet pointer for received data
 * \param u32Timeout timeout
 * 
 * \note Timeout of 0 disables the timeout.
 * The actual timeout period is the timeout value times the MC13192 timer rate
 * from MLMESetMC13192TmrPrescale.
 * \return Status
 */

UINT8 MLMERXEnableRequest(tRxPacket *psRxPacket, UINT32 u32Timeout)
{
    UINT8 u8Status = 0;
    
    psDrvRxPacket = psRxPacket;      /* Assign the rx_packet to SMAC global. */
    if (u32Timeout == 0) {          /* Timeout disabled */
        /* Just enable the receiver */
        u8Status = PLMESetTrxStateRequest(RX_MODE);
    }
    else {
        /* Timeout requested. Get the current time and add the timeout value. */
        /* Erratum: In order to avoid the missing Irq, we reset the timer1 */
        PLMESetTimeRequest(0x00000000);
        u8Status = PLMEEnableMC13192Timer1(u32Timeout);        /* 
                                                                * Set the 
                                                                * timeout in TC1
                                                                */
        u8Status = PLMESetTrxStateRequest(RX_MODE_WTO);
    }
    return u8Status;
}

/*!
 * \fn UINT8 MLMERXDisableRequest(void)
 *
 * \brief Set the MC13192 receiver OFF
 *
 * \return Status
 */

UINT8 MLMERXDisableRequest(void)
{
    (void)PLMEDisableMC13192Timer1();     /* 
                                     * In case the timeout is being used, 
                                     * disable it also 
                                     */
    if (PLMESetTrxStateRequest(IDLE_MODE) == SUCCESS) {     /* 
                                                             * Attempt to 
                                                             * disable the 
                                                             * timer 
                                                             */
        return SUCCESS;
    }
    else {
        return ERROR;
    }
}



 /*!
 * \fn UINT8 MLMESetMC13192ClockRate(UINT8 u8Freq)
 *
 * \brief Set MC13192 CLKo frequency
 *
 * \param u8Freq Frequency value enumeration (0-7)  \n
 *                                                  \n
 *  Freq    Output Frequency                        \n
 *  0       16MHz (Recommended default)             \n
 *  1       8MHz                                    \n
 *  2       4MHz                                    \n
 *  3       2MHz                                    \n
 *  4       1MHz                                    \n
 *  5       62.5kHz                                 \n
 *  6       32.786kHz                               \n
 *  7       16.393kHz                               \n
 * 
 * \return Status
 */
UINT8 MLMESetMC13192ClockRate(UINT8 u8Freq)
{
    UINT8 u8Status;
    
    u8Status = PLMESetMC13192ClockRate(u8Freq);
    return u8Status;
}

/*!
 * \fn UINT8 MLMESetMC13192TmrPrescale (UINT8 u8Freq)
 *
 * \brief Set MC13192 timer frequency
 *
 * \param u8Freq Frequency value enumeration (0-7)  \n
 *                                                  \n
 *  Freq    Output Frequency                        \n
 *  0       16MHz (Recommended default)             \n
 *  1       8MHz                                    \n
 *  2       4MHz                                    \n
 *  3       2MHz                                    \n
 *  4       1MHz                                    \n
 *  5       62.5kHz                                 \n
 *  6       32.786kHz                               \n
 *  7       16.393kHz                               \n
 * 
 * \return Status
 */
UINT8 MLMESetMC13192TmrPrescale (UINT8 u8Freq)
{
    UINT8 u8Status;
    
    u8Status = PLMESetMC13192TmrPrescale (u8Freq);
    return u8Status;
}

/*!
 * \fn UINT8 MLMEEnergyDetect (void)
 *
 * \brief Measure channel energy
 * 
 * \return Energy
 * 
 * \note                                \n
 * Actual power returned is: -(power/2) \n
 * Global calibration required for accuracy (from MLMEMC13192FEGainAdjust).
*/
UINT8 MLMEEnergyDetect (void)
{
    UINT8 u8Power;
    
    u8Power = PLMEEnergyDetect();
    return u8Power;
}

/*!
 * \fn  UINT8 MLMEMC13192SoftReset(void)
 *
 * \brief Force the MC13192 into a soft reset condition
 * 
 * \return Status
 * 
*/
UINT8 MLMEMC13192SoftReset(void)
{
    UINT8 u8Status;

    /* Performs a soft reset of MC13192 via writing to register 0*/ 
    u8Status = PLMEMC13192SoftReset();
    return u8Status;
}

/*!
 * \fn UINT8 MLMEMC13192XtalAdjust(UINT8 u8ReqValue)
 *
 * \brief Adjust the MC13192s crystal trim value
 *
 * \param u8ReqValue Trim Value (0-255)
 * 
 * \return Status
 * 
*/

UINT8 MLMEMC13192XtalAdjust(UINT8 u8ReqValue)
{
    UINT8 u8Status;
    
    u8Status = PLMEMC13192XtalAdjust(u8ReqValue);
    return u8Status;
}


/*!
 * \fn UINT8 MLMELinkQuality (void)
 *
 * \brief Report energy from last successful RX packet
 * 
 * \return Energy
 *
 * \note                                   \n
 * Actual power returned is: -(power/2)    \n
 * Global calibration required for accuracy (from MLMEMC13192FEGainAdjust).
 * 
*/
UINT8 MLMELinkQuality (void)
{
    UINT8 u8Power;
  
    u8Power = PLMELinkQuality ();
    return u8Power;
}

/*!
 * \fn UINT8 MLMEMC13192FEGainAdjust(UINT8 u8GainValue)
 *
 * \brief Adjust the MC13192s gain compensator
 *
 * \param u8GainValue Gain compensation value (0 to 255. 128 is center point)
 * 
 * \return Status
 */
UINT8 MLMEMC13192FEGainAdjust(UINT8 u8GainValue)
{
    UINT8 u8Status;
    
    u8Status = PLMEMC13192FEGainAdjust(u8GainValue);
    return u8Status;
}


/*
 * MLMEScanRequest : Scan the enabled channels for energy
 *
 * Parameters : FLAGS Option (UINT8)
 *              SCAN_MODE_CCA
 *              SCAN_MODE_ED
 *
 *              (UINT8 *)
 *              The results of the scan [16 bytes long]
 *
 * Return : Status
 */
UINT8 MLMEScanRequest(UINT8 flags, UINT8 *pu8ChannelScan) {
   
   UINT16 u16ChannelReg1;
   UINT16 u16ChannelReg2;
   
   UINT8 u8ClearestChLevel = 0; /* Default to very busy */
   UINT8 u8ClearestCh = 0;
   UINT8 u8i;
   UINT16 u16ChannelMask = 0;
   UINT8 u8Status;
   
   /* Turn off the Radio if it is on */
   (void)MLMERXDisableRequest();
   
   /* Save current channel registers to set it back */
   u16ChannelReg1 = SPIDrvRead(LO1_IDIV_ADDR);
   u16ChannelReg2 = SPIDrvRead(LO1_NUM_ADDR);
   
   for (u8i=0; u8i < 16; u8i++) {
      u16ChannelMask = (1 << u8i);   
         u8Status = MLMESetChannelRequest(u8i);
         if (flags & SCAN_MODE_CCA) {
            pu8ChannelScan[u8i] = PLMECCARequest();
         }
         
         if (flags & SCAN_MODE_ED) {
            pu8ChannelScan[u8i] = PLMEEnergyDetect();
         }
      
      /* Find the clearest Channel (Largest Number)*/       
      if (pu8ChannelScan[u8i] > u8ClearestChLevel) {
         u8ClearestChLevel = pu8ChannelScan[u8i];
         u8ClearestCh = u8i;
      }
         
   }

   /* Restore Channel Value */
   SPIDrvWrite(LO1_IDIV_ADDR, u16ChannelReg1);
   SPIDrvWrite(LO1_IDIV_ADDR, u16ChannelReg1);
   
   return u8ClearestCh;
}


/*!
 * \fn UINT8 MLMEMC13192PAOutputAdjust(UINT8 u8PaValue)
 *
 * \brief Adjust the Output power of the transmitter
 *
 * \param u8PaValue Course Valueacket
 * 
 * \return Status
 */
UINT8 MLMEMC13192PAOutputAdjust(UINT8 u8PaValue)
{
    UINT8 u8Status;
    
    u8Status = PLMEMC13192PAOutputAdjust(u8PaValue);
    return u8Status;
}

/*!
 * \fn UINT8 MLMEGetRficVersion(void)
 *
 * \brief Reads the version number of the IC
 * 
 * \return The version number of the IC
 */
UINT8 MLMEGetRficVersion(void)
{
    UINT8 u8Version;
    
    u8Version = PLMEGetRficVersion();
    return u8Version;
}
/*!
 * \fn void MLMETestMode (tTxPacket *psPacket, UINT8 u8mode)
 *
 * \brief Execute a test mode
 *
 * \param *psPacket Packet pointer
 * \param u8mode desired test mode
 * 
 */
void MLMETestMode (tTxPacket *psPacket, UINT8 u8mode)
{
    UINT16 u16reg;
    UINT32 i;
    switch (u8mode)
    {
        /* Continuously transmit a PRBS9 pattern. */
        case PULSE_TX_PRBS9:            
            PLMELoadPRBS9 (psPacket);   /* Load the TX RAM */
            (void)PLMESetTrxStateRequest(PULSE_TX_MODE); /* transmit it. Interrupt 
                                                    * routine will retransmit
                                                    * after completion 
                                                    */
        break;
        /* Sets the device back to original IDLE mode. */
        case FORCE_IDLE:
        
           MC13192DisableInterrupts();  /* Mask off interrupts from MC13192 */
           RTXENDeAssert();             /* Force MC13192 to IDLE */
           gu8RTxMode = IDLE_MODE;      /* set the SW mode to IDLE */
           MC13192RestoreInterrupts();
           
           for (i=0; i < 200; i++)
                ; /* Empty Loop */
            (void)PLMESetTrxStateRequest(IDLE_MODE);  /* Set to IDLE */
            u16reg = SPIDrvRead(BER_REG);       /* Turn off BER mode */
            u16reg = (u16reg & ~(BER_MASK));
            SPIDrvWrite(BER_REG,u16reg);
            u16reg = SPIDrvRead(PSM_REG);       /* Turn off PSM test mode */
            u16reg = (u16reg & ~(PSM_MASK));
            SPIDrvWrite(PSM_REG,u16reg);
            
            u16reg = SPIDrvRead(PAEN_REG);      /* dis-enable the PA */
            u16reg = (u16reg & ~PAEN_MASK);
            SPIDrvWrite(PAEN_REG,u16reg);
            
            (void)PLMESetTrxStateRequest(TX_MODE);    /* Realign TX */
            
            u16reg = SPIDrvRead(PAEN_REG);      /* enable the PA */
            u16reg = (u16reg | PAEN_MASK);
            SPIDrvWrite(PAEN_REG,u16reg);
            
        break;
        /* Sets the device into continuous RX mode */
        case CONTINUOUS_RX:
            u16reg = SPIDrvRead(BER_REG);       /* Turn on BER mode */
            u16reg = (u16reg | BER_MASK);
            SPIDrvWrite(BER_REG,u16reg);
            (void)PLMESetTrxStateRequest(RX_MODE);    /* Turn the receiver on */
        break;
        /* Sets the device to continuously transmit a 10101010 pattern */
        case CONTINUOUS_TX_MOD:
            SPIDrvWrite(TX_PKT,0xAAAA);         /* Load the test pattern */
            u16reg = SPIDrvRead(PAEN_REG);      /* Disable the PA */
            u16reg = (u16reg & ~(PAEN_MASK));
            SPIDrvWrite(PAEN_REG,u16reg);
            u16reg = SPIDrvRead(TX_PKT_LEN);    /* Set the length field */
            u16reg = ((u16reg & ~(TX_PKT_LEN_MASK)) | 0x0004);
            SPIDrvWrite(TX_PKT_LEN,u16reg);
            (void)PLMESetTrxStateRequest(TX_MODE);    /* Do a transmit to initialize */
            while (gu8RTxMode != IDLE_MODE)     /* Wait till done */
                ; /* Empty Loop */
            u16reg = SPIDrvRead(PAEN_REG);      /* Re-enable the PA */
            u16reg = (u16reg | PAEN_MASK);
            SPIDrvWrite(PAEN_REG,u16reg);
            u16reg = SPIDrvRead(BER_REG);       /* Turn on BER mode */
            u16reg = (u16reg | BER_MASK);
            SPIDrvWrite(BER_REG,u16reg);
            (void)PLMESetTrxStateRequest(TX_MODE);    /* Turn the transmitter on */
        break;
        /* Sets the device to continuously transmit an unmodulated CW */
        case CONTINUOUS_TX_NOMOD:
            u16reg = SPIDrvRead(PAEN_REG);      /* Disable the PA */
            u16reg = (u16reg & ~(PAEN_MASK));
            SPIDrvWrite(PAEN_REG,u16reg);
            u16reg = SPIDrvRead(TX_PKT_LEN);    /* Set the length field */
            u16reg = ((u16reg & ~(TX_PKT_LEN_MASK)) | 0x0004);
            SPIDrvWrite(TX_PKT_LEN,u16reg);
            (void)PLMESetTrxStateRequest(TX_MODE);    /* Do a transmit to initialize */
            while (gu8RTxMode != IDLE_MODE)     /* Wait till done */
                ; /* Empty Loop */
            u16reg = SPIDrvRead(PAEN_REG);      /* Re-enable the PA */
            u16reg = (u16reg | PAEN_MASK);
            SPIDrvWrite(PAEN_REG,u16reg);
            u16reg = SPIDrvRead(BER_REG);       /* Turn on BER mode */
            u16reg = (u16reg | BER_MASK);
            SPIDrvWrite(BER_REG,u16reg);
            u16reg = SPIDrvRead(PSM_REG);       /* Turn off PSM */
            u16reg = (u16reg | PSM_MASK);
            SPIDrvWrite(PSM_REG,u16reg);
            (void)PLMESetTrxStateRequest(TX_MODE);    /* Turn the transmitter on */
        break;
    }
}