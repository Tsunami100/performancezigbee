/*
 * Copyright (c) 2004, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 */
/*!
 * \file simple_phy.c
 *
 * \brief  This is the SMAC C code for the PHY layer  
 *
 * \author a19259 
 *
 * \version 4.1a
 *
 * \date - 29/07/2005 03:30:28
 *
 * \b Description: This is the SMAC C source physical layer file for the HC(S)08
 *                 MCU and MC13192 transceiver.
 *                 The SMAC phy is the second lowest layer of C code.
 * 
 * \b Project name: SMAC (Simple Media Access Controller)
 *
 * \b Department : Freescale Radio Products Division
 *
 * \b History: 
 * - 16/11/2005 Doc. update to Doxygen compliant by Laura Delgado r12370c
 * 
 */

#include "MC13192_regs.h"
#include "MC13192_hw_config.h"
#include "pub_def.h"
#include "drivers.h"
#include "simple_phy.h"
#include "simple_mac.h"
#include "mcu_hw_config.h"
#include "app_config.h"

/* Globals */
extern tRxPacket *psDrvRxPacket; //!< Where the packet received will be stored 
extern UINT8 gu8RTxMode;         //!< Global mode mirror. MC13192 Status.  
tPhyOptions gsPhyOptions;

extern UINT16 u16StatusContent;


/* Version string to put in NVM. Note! size limits */
/* Normally it shoud be enough to change the version numbers. */
#define PHY_Version     "4.2 "
#define PHY_Label       "SMAC"
#define PHY_VerComplete "PHY " PHY_Label " Ver " PHY_Version " Build: " __DATE__ " " __TIME__
#define PHY_VerSize     sizeof(PHY_VerComplete)


#if defined (HCS08G) | defined (HCS08R)	| defined (_HCS12)
  #include <hidef.h>
#pragma MESSAGE DISABLE C3303  /* 
                                * Warning C3303: Implicit concatenation of 
                                * strings 
                                */
#pragma MESSAGE DISABLE C4200  /* 
                                * Warning C4200: Other segment than in previous 
                                * declaration
                                */
#pragma CONST_SEG BOOTLOADER_PHY_NV_DATA0
#endif

// DO NOT CHANGE OR REMOVE

// This string will be located in the NV RAM0 section.
// Note!! Check that item is location in the sequence as specified. 
const unsigned char SPHY_Version[PHY_VerSize] = PHY_VerComplete;

#if defined (HCS08G) | defined (HCS08R) | defined (_HCS12)
#pragma CONST_SEG DEFAULT
#endif




/*
 * PLMEPhyInit : Sets the default values for Phy
 *
 * Parameters : Uses the gsPhyOptions
 *
 * Return : Status
 */
 
 UINT8 PLMEPhyReset(void) {
 
   /* Sets the default Options */
   gsPhyOptions.Bits.u1PromiscuousMode = SMAC_PROMISCUOUS_MODE;
   gsPhyOptions.Byte.u16ScanChannels = 0xFFFF; /* Default to all channels */
   
   return SUCCESS;  
 }
/*!
 * \fn UINT8 PDDataRequest(tTxPacket *psPacket) 
 *
 * \brief Transmit data packet
 *
 * \param *psPacket packet pointer
 *
 * \return Status
 */
UINT8 PDDataRequest(tTxPacket *psPacket)
{
    if (gu8RTxMode == IDLE_MODE)
    { 
        #if (SMAC_FEATURE_SECURITY == TRUE)
          SECSecure(psPacket->pu8Data, psPacket->u8DataLength);
        #endif /* SMAC_FEATURE_SECURITY */
        
        RAMDrvWriteTx(psPacket);            /* Load the data into packet RAM */
        (void)PLMESetTrxStateRequest(TX_MODE);    /* transmit it */

        /* Wait for the state to return to idle. (finish transmitting) */
        while (gu8RTxMode != IDLE_MODE)
        {
            MCU_LOW_POWER_WHILE();
        }
        return SUCCESS;
    }
    else
    {
        return RX_ON;
    }
}

UINT8 PDDataRequestWithoutLoad(void)
{
    if (gu8RTxMode == IDLE_MODE)
    { 
        (void)PLMESetTrxStateRequest(TX_MODE);    /* transmit it */

        /* Wait for the state to return to idle. (finish transmitting) */
        while (gu8RTxMode != IDLE_MODE)
        {
            MCU_LOW_POWER_WHILE();
        }
        return SUCCESS;
    }
    else
    {
        return RX_ON;
    }
}

/*!
 * \fn void PDDataIndication()
 *
 * \brief Receive data packet indication.
 *
 */
void PDDataIndication()
{
    /* Read the Data only if it is a good packet. */
    if (psDrvRxPacket->u8Status == SUCCESS)
    {
        (void)RAMDrvReadRx(psDrvRxPacket);        /* 
                                             * Read data from MC13192, 
                                             * check status 
                                             */
    }
    EnableInterrupts;       /* Allow system interrupts within the IRQ handler */
    
    #if (SMAC_FEATURE_SECURITY == TRUE)
      SECSecure(psDrvRxPacket->pu8Data, psDrvRxPacket->u8DataLength);
    #endif /* SMAC_FEATURE_SECURITY */
    
    #if (SMAC_FEATURE_OTAP == TRUE)
      if (gu8OTAPStatusAPI == OTAP_ACTIVE){
        OTAPDataIndication(psDrvRxPacket);
      } else {
        MCPSDataIndication(psDrvRxPacket);
      }
    #else
      MCPSDataIndication(psDrvRxPacket);
    #endif /* SMAC_FEATURE_OTAP */

}

/*!
 * \fn UINT8 PLMEHibernateRequest(void) 
 *
 * \brief Hibernate the MC13192 (very low current, no CLKO).
 *
 * \return Status
 */
 UINT8 PLMEHibernateRequest(void)
{
    UINT16 u16CurrentValue;
    
    gu8RTxMode = HIBERNATE_MODE;
    u16CurrentValue = SPIDrvRead(MODE2_ADDR);       /* 
                                                     * Read MC13192 Hiberate 
                                                     * register. 
                                                     */
    u16CurrentValue &= 0xFFFC;
    u16CurrentValue |= 0x0002;                      /* Hiberate enable */
    SPIDrvWrite(MODE2_ADDR, u16CurrentValue);       /* 
                                                     * Write back to MC13192 to 
                                                     * enable hibernate mode. 
                                                     */
    return SUCCESS;         /* Call back to MAC layer indicating success. */
}

/*!
 * \fn UINT8 PLMEDozeRequest(UINT32 u32Timeout) 
 *
 * \brief Doze the MC13192 (Low current, CLKO <= 1MHz).
 *
 * \param u32Timeout - timeout
 *
 * \return Status
 */

UINT8 PLMEDozeRequest(UINT32 u32Timeout)
{
    UINT16 u16CurrentValue;
    UINT32 u32CurrentTime;
    
    gu8RTxMode = DOZE_MODE;
    u16CurrentValue = SPIDrvRead(MODE2_ADDR);       /* 
                                                     * Read MC13192 Doze 
                                                     * register. 
                                                     */
    if (u32Timeout == 0)        /* ACOMA mode, with clkout */
    {
        u16CurrentValue = SPIDrvRead(IRQ_MASK);
        u16CurrentValue &= 0xFE0D;   /* Change the IRQ_Mask to set Acoma en,*/
        u16CurrentValue |= 0x0100;   /* doze irq disabled and tmr2 disabled*/
        SPIDrvWrite(IRQ_MASK, u16CurrentValue);      
        
        u16CurrentValue = SPIDrvRead(MODE2_ADDR);       /* 
                                                         * Read MC13192 Doze 
                                                         * register. 
                                                         */
        u16CurrentValue &= 0xFDFC;
        u16CurrentValue |= 0x0201;                      /* Doze (acoma) & CLKOUT enable */
        SPIDrvWrite(MODE2_ADDR, u16CurrentValue);       /* 
                                                         * Write back to MC13192 to 
                                                         * enable hibernate mode. 
                                                         */
    } else						 /* DOZE mode with timeout, no clkout */
    {
        u16CurrentValue = SPIDrvRead(IRQ_MASK);
        u16CurrentValue &= 0xFE0D;   /* Change the IRQ_Mask to set Acoma dis,*/
        u16CurrentValue |= 0x0012;   /* doze enabled and tmr2 enabled*/
        SPIDrvWrite(IRQ_MASK, u16CurrentValue);      
        
        u32CurrentTime = PLMEGetTimeRequest();
        u32Timeout += u32CurrentTime;
        SPIDrvWrite( T2_HI_ADDR, ((UINT16)(u32Timeout>>16)&0x00FF) ); 
        SPIDrvWrite( T2_LO_ADDR, ((UINT16)u32Timeout)&0xFFFF );  /* Enable the Timer 2 
                                                         * and save the timeout value
                                                         */
        u16CurrentValue = SPIDrvRead(MODE2_ADDR);
        u16CurrentValue &= 0xFDFC;						/* Disable CLKOinDozeand */
        u16CurrentValue |= 0x0001;                      /* enter in Doze mode */
        SPIDrvWrite(MODE2_ADDR, u16CurrentValue);      
    }
    return SUCCESS;         /* Call back to MAC layer indicating success. */
}

 /*!
 * \fn UINT8 PLMEWakeRequest(void) 
 *
 * \brief Wake the MC13192 from Hibernate or Doze. 
 *
 * \return Status
 */
UINT8 PLMEWakeRequest(void)
{
    UINT16 u16CurrentValue;
    
    MC13192Wake();                              /* Wake up the device */
    while (gu8RTxMode != IDLE_MODE_ATTN)
    {
        MCU_LOW_POWER_WHILE();                    /* Wait until ATTN */
    }
    u16CurrentValue = SPIDrvRead(MODE2_ADDR);       /* 
                                                     * Read MC13192 
                                                     * Hibernate/Doze register. 
                                                     */
    u16CurrentValue &= 0xFFFC;                  /* Hiberate and Doze disable */
    SPIDrvWrite(MODE2_ADDR, u16CurrentValue);       /* 
                                                     * Write back to MC13192 to 
                                                     * disable hibernate and doze 
                                                     * mode. 
                                                     */
    SPIDrvWrite(T2_HI_ADDR, 0x0000);                /* Disable Timer2
                                                     * To avoid a T2 int because
                                                     * of doze w/timeout
                                                     */
    gu8RTxMode = IDLE_MODE;
    return SUCCESS;
}

 /*!
 * \fn UINT8 PLMESetChannelRequest(UINT8 u8Channel)
 *
 * \brief Set the MC13192 operating channel.
 *
 * \param u8Channel Channel number
 *
 * \return Status
 */
UINT8 PLMESetChannelRequest(UINT8 u8Channel)
{
    switch (u8Channel)
    {
    case 0x00:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F95);
        SPIDrvWrite(LO1_NUM_ADDR,0x5000);
    break;
    case 0x01:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F95);
        SPIDrvWrite(LO1_NUM_ADDR,0xA000); 
    break;
    case 0x02:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F95);
        SPIDrvWrite(LO1_NUM_ADDR,0xF000); 
    break;
    case 0x03:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F96);
        SPIDrvWrite(LO1_NUM_ADDR,0x4000); 
    break;
    case 0x04:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F96);
        SPIDrvWrite(LO1_NUM_ADDR,0x9000); 
    break;
    case 0x05:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F96);
        SPIDrvWrite(LO1_NUM_ADDR,0xE000); 
    break;
    case 0x06:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F97);
        SPIDrvWrite(LO1_NUM_ADDR,0x3000); 
    break;
    case 0x07:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F97);
        SPIDrvWrite(LO1_NUM_ADDR,0x8000); 
    break;
    case 0x08:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F97);
        SPIDrvWrite(LO1_NUM_ADDR,0xD000); 
    break;
    case 0x09:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F98);
        SPIDrvWrite(LO1_NUM_ADDR,0x2000); 
    break;
    case 0x0A:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F98);
        SPIDrvWrite(LO1_NUM_ADDR,0x7000); 
    break;
    case 0x0B:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F98);
        SPIDrvWrite(LO1_NUM_ADDR,0xC000); 
    break;
    case 0x0C:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F99);
        SPIDrvWrite(LO1_NUM_ADDR,0x1000); 
    break;
    case 0x0D:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F99);
        SPIDrvWrite(LO1_NUM_ADDR,0x6000); 
    break;
    case 0x0E:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F99);
        SPIDrvWrite(LO1_NUM_ADDR,0xB000); 
    break;
    case 0x0F:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F9A);
        SPIDrvWrite(LO1_NUM_ADDR,0x0000); 
    break;
    default:
        SPIDrvWrite(LO1_IDIV_ADDR,0x0F97);
        SPIDrvWrite(LO1_NUM_ADDR,0xD000); 
        return ERROR;
    }
    return SUCCESS;
}

/*!
 * \fn UINT8 PLMESetTrxStateRequest(UINT8 u8ReqMode) 
 *
 * \brief Set the MC13192 transceive operation.
 *
 * \param u8ReqMode Operational mode
 *
 * \return Status  
 */
UINT8 PLMESetTrxStateRequest(UINT8 u8ReqMode)
{
    UINT16 u16Reg;
    
    RTXENDeAssert(); 
    u16Reg = SPIDrvRead(MODE_ADDR);
    u16Reg &= 0xFFF8;                           /* Clear mode. */
    switch (u8ReqMode)
    {
        //u16Reg |= IDLE_MODE;
        case IDLE_MODE:                         /* Write Idle */
        #if defined (LNA)
            MC13192_LNA_CTRL = LNA_OFF;
        #endif
        #if defined (PA)
            MC13192_PA_CTRL = PA_OFF;
        #endif
        gu8RTxMode = IDLE_MODE;
        SPIDrvWrite(MODE_ADDR, u16Reg);
        break;

        case RX_MODE:                           /* Write RX */
        #if defined (LNA)
            MC13192_LNA_CTRL = LNA_ON;
        #endif
        #if defined (PA)
            MC13192_PA_CTRL = PA_OFF;
        #endif
        #if defined (ANTENNA_SWITCH)
            MC13192_ANT_CTRL2 = ANT_CTRL_ON;    /* Turn on the RX antenna */
            MC13192_ANT_CTRL = ANT_CTRL_OFF;    /* Turn off the TX antenna */
        #endif
        gu8RTxMode = RX_MODE;
        u16Reg |= RX_MODE;
        SPIDrvWrite(MODE_ADDR, u16Reg);
        RTXENAssert(); 
        break;
        
        case RX_MODE_WTO:           /* Write RX, but set gu8RTxMode to timeout */
        #if defined (LNA)
            MC13192_LNA_CTRL = LNA_ON;
        #endif
        #if defined (PA)
            MC13192_PA_CTRL = PA_OFF;
        #endif
        #if defined (ANTENNA_SWITCH)
            MC13192_ANT_CTRL2 = ANT_CTRL_ON;    /* Turn on the RX antenna */
            MC13192_ANT_CTRL = ANT_CTRL_OFF;    /* Turn off the TX antenna */
        #endif
        gu8RTxMode = RX_MODE_WTO;
        u16Reg |= RX_MODE;
        SPIDrvWrite(MODE_ADDR, u16Reg);
        RTXENAssert();
        break;
        
        case TX_MODE:               /* Write Tx. Note: force LO lock not used */
        #if defined (PA)
            MC13192_PA_CTRL = PA_ON;
        #endif
        #if defined (LNA)
            MC13192_LNA_CTRL = LNA_OFF;
        #endif
        #if defined (ANTENNA_SWITCH)
            MC13192_ANT_CTRL2 = ANT_CTRL_OFF;   /* Turn off the RX antenna */
            MC13192_ANT_CTRL = ANT_CTRL_ON;     /* Turn on the TX antenna */
        #endif
        u16Reg |= TX_MODE;
        gu8RTxMode = TX_MODE;
        SPIDrvWrite(MODE_ADDR, u16Reg);
        RTXENAssert();
        break;
        
        case PULSE_TX_MODE: /* Write Tx. Note: force LO lock not used */

		  #if defined (PA)
  	         MC13192_PA_CTRL = PA_ON;
		  #endif
		
		  #if defined (LNA)
		      MC13192_LNA_CTRL = LNA_OFF;
		  #endif
		
		  #if defined (ANTENNA_SWITCH)
				MC13192_ANT_CTRL2 = ANT_CTRL_OFF;	/* Turn off the RX antenna */
				MC13192_ANT_CTRL = ANT_CTRL_ON;	/* Turn on the TX antenna */
		  #endif
						
		   u16Reg |= TX_MODE;
			gu8RTxMode = PULSE_TX_MODE;
			SPIDrvWrite(MODE_ADDR, u16Reg);
			RTXENAssert();
			break;

        default:
        return ERROR;    
    }
    return SUCCESS;
}

/*!
 * \fn UINT8 PLMEEnergyDetect (void) 
 *
 * \brief Measure channel energy.
 *
 * \return Energy
 * \note Actual power returned is: -(power/2)   \n
 * Global calibration required for accuracy. 
 */
UINT8 PLMEEnergyDetect (void)
{
    UINT16 u16Reg;
    UINT8 u8Power;
    
    gu8RTxMode = CCA_MODE;                /* Write energy detect mode */
    u16Reg = SPIDrvRead(MODE_ADDR);
    
    u16Reg &= 0xFFC8; /* Mask off the CCA_type mode 5:4, xcvr_seq 2:0 */    
    u16Reg |= (CCA_MODE | SCAN_MODE_ED); /* Set the CCA mode and turn on CCA */
    SPIDrvWrite(MODE_ADDR, u16Reg);
    RTXENAssert();
    while (gu8RTxMode != IDLE_MODE)       /* Wait for energy detect to complete */
        {
        MCU_LOW_POWER_WHILE();
        }
    u16Reg = (SPIDrvRead(CCA_RESULT_ADDR) & 0xFF00);
    u8Power = (UINT8)(u16Reg >> 8);
    return u8Power;
}

UINT8 PLMECCARequest (void)
/* Note: Actual power returned is: -(power/2) */
/* Global calibration required for accuracy. */
{
    UINT16 u16Reg;
    //UINT8 u8Power;
    
    gu8RTxMode = CCA_MODE;                /* Write energy detect mode */
    u16Reg = SPIDrvRead(MODE_ADDR);
    
    u16Reg &= 0xFFC8; /* Mask off the CCA_type mode 5:4, xcvr_seq 2:0 */    
    u16Reg |= (CCA_MODE | SCAN_MODE_CCA); /* Set the CCA mode and turn on CCA */
    SPIDrvWrite(MODE_ADDR, u16Reg);
    RTXENAssert();
    while (gu8RTxMode != IDLE_MODE)       /* Wait for energy detect to complete */
        {
        MCU_LOW_POWER_WHILE();
        }
    
    if ((u16StatusContent & CCA_BIT_MASK) == 0) {
      return 0; //Clear  
    } else {
      return 1; //BUSY
    }
}

/*!
 * \fn UINT8 PLMELinkQuality (void)
 *
 * \brief Report energy from last successful RX packet.
 *
 * \return Energy
 *
 * \note Actual power returned is: -(power/2) \n
 *     Global calibration required for accuracy. 
 */
UINT8 PLMELinkQuality (void)
{
    UINT16 u16Reg;
    UINT8 u8Power;
    
    u16Reg = SPIDrvRead(CCA_RESULT_ADDR);
    u8Power = (UINT8)((u16Reg & 0xFF00) >> 8);
    return u8Power;
}

/*!
 * \fn UINT32 PLMEGetTimeRequest(void) 
 *
 * \brief Get MC13192 timer value. 
 *
 * \return Timer value
 */

UINT32 PLMEGetTimeRequest(void)
{
    UINT32 u32UpperWord, u32LowerWord;
    UINT32 u32CurrentTime;
    
    DisableInterrupts;
    u32UpperWord = SPIDrvRead(TIMESTAMP_HI_ADDR);
    u32LowerWord = SPIDrvRead(TIMESTAMP_LO_ADDR);
    u32UpperWord &= TIMESTAMP_HI_MASK;              /* Clears TS_HELD bit. */
    u32CurrentTime = (UINT32) (u32UpperWord << 16) | u32LowerWord;
    EnableInterrupts;
    return u32CurrentTime;
}

/*!
 * \fn UINT8 PLMESetMC13192ClockRate(UINT8 u8Freq)
 *
 * \brief Set MC13192 CLKo frequency.
 *
 * \param u8Freq Frequency value
 *
 * \return Status
 */

UINT8 PLMESetMC13192ClockRate(UINT8 u8Freq)
{
    volatile UINT16 u16CurrentValue;
    
    u16CurrentValue = SPIDrvRead(CLKS_ADDR); /* Read register and re-write */
    u16CurrentValue &= 0xFFF8;
    u16CurrentValue |= u8Freq;
    SPIDrvWrite(CLKS_ADDR, u16CurrentValue);
    return SUCCESS;
}

/*!
 * \fn UINT8 PLMESetMC13192TmrPrescale (UINT8 freq) 
 *
 * \brief Set MC13192 timer frequency.
 *
 * \param freq Frequency value
 *
 * \return Status
 */
UINT8 PLMESetMC13192TmrPrescale (UINT8 freq) 
{
    volatile UINT16 current_value;
    
    current_value = SPIDrvRead(PRESCALE_ADDR);
    current_value &= 0xFFF8;
    current_value |= freq;
    SPIDrvWrite(PRESCALE_ADDR, current_value);
    return SUCCESS;
}

/*!
 * \fn void PLMESetTimeRequest(UINT32 u32RequestedTime)
 *
 * \brief Set MC13192 timer value (i.e. initialize)
 *
 * \param u32RequestedTime Timer Value
 *
 */
void PLMESetTimeRequest(UINT32 u32RequestedTime)
{
    UINT16 u16UpperWord, u16LowerWord;
    UINT16 u16Mode2RegVal;
    
    /* Split 32 bit input into 2 16 bit values */
    u16UpperWord = (UINT16) (u32RequestedTime >> 16) & 0x000000FF;
    u16LowerWord = (UINT16) u32RequestedTime & 0x0000FFFF;
    /* Program Time1 comparator with the desired value */    
    SPIDrvWrite(T1_HI_ADDR, u16UpperWord);
    SPIDrvWrite(T1_LO_ADDR, u16LowerWord);
    /* Get current state of the MODE2 MC13192 register */
    u16Mode2RegVal = SPIDrvRead(MODE2_ADDR);
    /* Set the Tmr_load bit */
    u16Mode2RegVal |= 0x8000;
    /* Now write the value back to MC13192 register MODE2 */
    SPIDrvWrite(MODE2_ADDR, u16Mode2RegVal);
    /* Clear the tmr_load bit */
    u16Mode2RegVal &= 0x7FFF;
    /* Clr the tmr_load bit to prepare for next set_time_request. */
    SPIDrvWrite(MODE2_ADDR, u16Mode2RegVal);    
    return;
}

/*!
 * \fn UINT8 PLMEEnableMC13192Timer1(UINT32 u32CounterValue)
 *
 * \brief Set MC13192 timer compare value.
 *
 * \param u32CounterValue 
 *
 * \return Status
 */
UINT8 PLMEEnableMC13192Timer1(UINT32 u32CounterValue)
{
    /* Load the timeout value into T1 with Timer disabled. */
    SPIDrvWrite(T1_HI_ADDR, (UINT16) ((u32CounterValue >> 16) & 0x000000FF)|\
                                           0x000080FF);
    SPIDrvWrite(T1_LO_ADDR, (UINT16) (u32CounterValue & 0x0000FFFF));
    /* Turn Timer1 mask on. */
    SPIDrvWrite(T1_HI_ADDR, (UINT16) ((u32CounterValue >> 16) & 0x000000FF));
    SPIDrvWrite(T1_LO_ADDR, (UINT16) (u32CounterValue & 0x0000FFFF));
    return SUCCESS;
}

 /*!
 * \fn UINT8 PLMEDisableMC13192Timer1(void) 
 *
 * \brief Disable MC13192 timer comparator TC1.
 *
 * \return Status
 */
UINT8 PLMEDisableMC13192Timer1(void)
{
    /* Load the timeout value into T1 with Timer disabled. */
    /* Clear Timer1 if in RX_MODE_WTO */
    SPIDrvWrite(T1_HI_ADDR, 0x8000);
    SPIDrvWrite(T1_LO_ADDR, 0x0000);
    //irq_mask_reg = SPIDrvRead(IRQ_MASK);      /* NOT USED */
    //irq_mask_reg &= ~TIMER1_IRQMASK_BIT;
    //SPIDrvWrite(IRQ_MASK, irq_mask_reg);
    return SUCCESS;
}

/*!
 * \fn void PLMEMC13192ResetIndication (void) 
 *
 * \brief Indicate a MC13192 reset condition.
 *
 */
void PLMEMC13192ResetIndication (void)
{
    MLMEMC13192ResetIndication();
}

/*!
 * \brief Force the MC13192 into a soft reset condition.
 *
 * \return Status 
 */
UINT8 PLMEMC13192SoftReset(void)
{
    SPIDrvWrite(RESET, 0x00);
    return SUCCESS;
}

/*!
 * \fn UINT8 PLMEMC13192XtalAdjust(UINT8 u8TrimValue) 
 *
 * \brief Adjust the MC13192s crystal trim value.
 *
 * \param u8TrimValue Trim value
 *
 * \return Status
 */
UINT8 PLMEMC13192XtalAdjust(UINT8 u8TrimValue)
{
    UINT16 u16Reg;
    UINT16 u16RegValue;
    
    u16RegValue = (UINT16)(u8TrimValue << 8);/* 
                                              * Shift the req value into the 
                                              * higher half word 
                                              */
    u16Reg = SPIDrvRead(XTAL_ADJ_ADDR);      /* 
                                              * Read the current value of XTAL 
                                              * Reg 
                                              */
    u16Reg = ((u16Reg & 0x00FF) | u16RegValue);
    SPIDrvWrite(XTAL_ADJ_ADDR, u16Reg);
    return SUCCESS;
}

/*!
 * \fn UINT8 PLMEMC13192FEGainAdjust(UINT8 i8GainValue)
 *
 * \brief Adjust the MC13192s gain compensator.
 *
 * \param i8GainValue Gain Compensation
 *
 * \return Status
 */
UINT8 PLMEMC13192FEGainAdjust(UINT8 i8GainValue)
{
    UINT16 u16Reg;
    
    u16Reg = SPIDrvRead(FEGAIN_ADDR);       /* 
                                             * Read the current value of GAIN 
                                             * Reg 
                                             */
    u16Reg = ((u16Reg & 0xFF00) | i8GainValue);
    SPIDrvWrite(FEGAIN_ADDR, u16Reg);
    return SUCCESS;
}

 /*!
 * \fn UINT8 PLMEMC13192PAOutputAdjust(UINT8 u8RequestedPAValue)
 *
 * \brief Adjust the MC13192s Output power.
 *
 * \param u8RequestedPAValue PA output adjust
 *
 * \return Status
 */
UINT8 PLMEMC13192PAOutputAdjust(UINT8 u8RequestedPAValue)
{
    UINT16 u16Reg;
    UINT8 u8PAValue;
    
    switch (u8RequestedPAValue)
    {
        case MAX_POWER:         /* Sets the PA drive level and PA gain to MAX */
            u8PAValue = 0xFF;
        break;

        case MIN_POWER:
            u8PAValue = 0x00;   /* Sets the PA drive level and PA gain to MIN */
        break;

        default:
            if (u8RequestedPAValue > 15) {
                return OVERFLOW;
            }
            else {                
                u8PAValue = u8RequestedPAValue;
            }
        break;
    }
    
    u16Reg = SPIDrvRead(PA_ADJUST_ADDR);    /* 
                                             * Read the current value of GAIN 
                                             * Register 
                                             */
    u16Reg &= 0xFF00;
    
    if ((u8RequestedPAValue == MAX_POWER) || (u8RequestedPAValue == MIN_POWER)){
        u16Reg |= u8PAValue;
    }
    else {
        u16Reg |= ((u8PAValue << 4) | 0x000C);
    }
    SPIDrvWrite(PA_ADJUST_ADDR, u16Reg);
    return SUCCESS;
}

/*!
 * \fn UINT8 PLMEGetRficVersion(void)
 *
 * \brief Returns the RFIC version number.
 *
 * \return Version number 
 */
UINT8 PLMEGetRficVersion(void)
{
    UINT16 u16Reg;
    
    u16Reg = SPIDrvRead(VERSION_REG);   /* 
                                         * Read the version register 
                                         * version[12:10] 
                                         */
    u16Reg &= VERSION_MASK;             /* Shift to generate accurate number */
    
    u16Reg = u16Reg >> 10;              /* Hard coded to shift */
    return (UINT8) u16Reg;
}
/*!
 * \fn void PLMELoadPRBS9 (tTxPacket *psPacket)
 *
 * \brief Loads the transmit RAM with a PRBS9 data pattern.
 *
 * \param  *psPacket Transmit buffer pointer
 *
 */
void PLMELoadPRBS9 (tTxPacket *psPacket)
{
  UINT8 u8c1;                           /* Byte counter */
  UINT8 u8c2;                           /* Bit counter */
  UINT16 u16t1;                         /* LFSR */
  UINT16 u16t2;                         /* LFSR output */
  UINT16 u16t3;                         /* LFSR feedback tap */
  UINT8 u8t4;                           /* Assembled transmit byte */
  
    u16t1 = 0x01FF;                     /* Initialize the LFSR */
    for (u8c1=0; u8c1<64; u8c1++)       /* Byte counter */
    {
        u8t4 = 0x00;                    /* Initialize the byte */
    for (u8c2=0; u8c2<8; u8c2++)        /* Bit counter */
    {
        u16t2 = (u16t1 & 0x0001);       /* LFSR output */
        if (u16t2 == 0x0001)
        {
            u8t4 = (UINT8)(u8t4 | 0x80);         /* Set/Clear byte based on LFSR output */
        }
        if (u8c2 != 7)
        {
            u8t4 = (UINT8)(u8t4 >> 1);           /* LSBit will be first bit out of LFSR */
        }
        u16t3 = ((u16t1 & 0x0010) >> 4);/* LFSR tap */
        u16t1 = u16t1 >> 1;             /* Now shift the LFSR */
        if (u16t2 == u16t3)             /* Set/Clr the LFSR MSBit */
        {
            u16t1 = u16t1 & 0xFEFF;
        }
        else
        {
            u16t1 = u16t1 | 0x0100;
        }
    }
    psPacket->pu8Data[u8c1] = u8t4;
    }
    psPacket->u8DataLength = 64;
    RAMDrvWriteTx(psPacket);            /* Load the data into packet RAM */
}

UINT8  PLMEEnablePromiscuousMode(void)
{
//  gsPhyOptions.Bits.u1PromiscuousMode = TRUE;
  return SUCCESS;
}
UINT8  PLMEDisablePromiscuousMode(void)
{
//  gsPhyOptions.Bits.u1PromiscuousMode = FALSE;
  return SUCCESS;
}