/*!
 * Copyright (c) 2005, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 * \file    mcu_hw_config.c
 * \brief   MCU Hardware configuration routines.
 * \author  a20639
 * \version 4.1a
 * \date    2005/08/11 20:44:37 
 * 
 * \b Description:
 *
 * This file contains the functions to manage the clock, 
 * COP, GPIO, SPI and IRQ. 
 *
 * \b Department: Freescale Radio Products Division 
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 11/16/2005 Doc. update to Doxygen compliant by Gonzalo Delgado 
 * Huitron rgd04c
 */ 
 


#include "mcu_hw_config.h"
#include "simple_phy.h"
#include "mcu_spi_config.h"
#include "drivers.h"

/*! Global Variables */
extern UINT8 gu8RTxMode;
 

#if defined (HCS08G) | defined (HCS08R) | defined (MC1321X) 
/*!
  \fn void UseExternalClock() 
  \brief	This function configures the use of the external clock.
   Switch the MCU from internal to MC13192 supplied clock.
     The MCU FLL is not engaged.
  \warning This function is executed if the core is a member of the
           HCS08G family       
  
*/
void UseExternalClock(void) //use_external_clock()
{
    ICGC1 = 0x50;    /* 
                      *  0b01010000
                      *    ||||||||__ Unimplemented
                      *    |||||||___ Unimplemented
                      *    ||||||____ Oscillator disabled when ICG is in 
                      *    |||||      off mode
                      *    |||||
                      *    ||||+_____ FLL bypassed, external reference
                      *    |||_______ External Clock Requested
                      *    ||________ Osc configured for High Frequency
                      *    |_________ Unimplemented
                      */
    while (!ICGS1_ERCS)    /* Wait for external reference to be stable. */
        ;
    ICGC2_LOLRE = 1;
    ICGC2_MFD = 0x00;       /* Mult factor of 4. */
}


/*!
  \fn void UseMcuClock()
  \brief	This function configures the use of the internal clock.
   Switch the MCU from MC13192 external to internal clock.
  \warning This function is executed if the core is a member of the
           HCS08G family 
*/
void UseMcuClock() //use_mcu_clock()
{
    ICGC1_RANGE = 1;        /* Range High. */
    ICGC1_OSCSTEN = 1;      /* Osc enabled in off mode. */
    ICGC1_CLKS = 0x00;      /* Self clocked. */            
}
#endif


/*
 * MC13192Restart: Restart the MC13192.
 *
 * Parameters: None
 *
 * Return : None
 */
void MC13192Restart()
{
//UINT8 u8AttnIrq =0; //mod
//UINT16 u8AttnIrq = 0; //mod

    gu8RTxMode = SYSTEM_RESET_MODE;
    IRQInit();                  /* Turn on the IRQ pin. */
    MC13192_RESET = 1;          /* Take MC13192 out of reset */
    while (IRQSC_IRQF == 0)     /* Poll waiting for MC13192 to assert the irq */
        ; /* Empty Body */      /* (i.e. ATTN). */
    (void)SPIDrvRead(0x24);            /* Clear MC13192 interrupts */
    IRQACK();                   /* ACK the pending IRQ interrupt */
    IRQPinEnable();             /* Pin Enable, IE, IRQ CLR, negative edge. */    
}


/*
 * MC13192ContReset: Reset (continuous) the MC13192.
 *
 * Parameters: None
 *
 * Return : None
 */
void MC13192ContReset()
{
    gu8RTxMode = SYSTEM_RESET_MODE;
    IRQSC = 0x00;                   /* Set for negative edge. */
    MC13192_RESET = 0;              /* Place the MC13192 into reset */
}



/*!
  \fn void GPIOInit()
  \brief Initialize the MCU-to-MC13192 GPIO direction and data.
*/
void GPIOInit()
{
    MC13192_RESET_PULLUP = 0;
    MC13192_CE = 1;                    
    MC13192_ATTN = 1;
    MC13192_RTXEN = 0;
    MC13192_RESET = 0;                  /* Do not initially reset MC13192 */
    MC13192_CE_PORT = 1;
    MC13192_ATTN_PORT = 1;
    MC13192_RTXEN_PORT = 1;
    MC13192_RESET_PORT = 1;
    MC13192_CE = 1;                    
    MC13192_ATTN = 1;
    MC13192_RTXEN = 0;
    MC13192_RESET = 0;                  /* Do not initially reset MC13192 */
 
    #if defined (ANTENNA_SWITCH)     
        MC13192_ANT_CTRL2_PORT = 1;        /* Output for antenna port RX */
        MC13192_ANT_CTRL_PORT = 1;        /* Output for antenna port TX */
        MC13192_ANT_CTRL2 = 1;          /* Signal to turn on RX antenna */
        MC13192_ANT_CTRL = 1;           /* Signal to turn on TX antenna */
    #endif
 
    #if defined (LNA)
        MC13192_LNA_CTRL = LNA_OFF;     /* Turn off the LNA out of reset */
        MC13192_LNA_CTRL_PORT  = 1;     /* Enable the port for OUTPUT */
    #endif
 
    #if defined (PA)
        MC13192_PA_CTRL = PA_OFF;       /* Turn off the PA out of Reset */
        MC13192_PA_CTRL_PORT = 1;       /* Enable the port for OUTPUT */
    #endif
 
}

 /*!
  \fn void MCUInit() 
  \brief	Initialize the MCU COP, GPIO, SPI and IRQ.
   Set the desired MC13192 clock frequency here.
  \sa GPIOInit()
  \sa SPIInit()
  \sa IRQInit()
  \sa IRQACK()             
  \sa SPIDrvRead()
  \sa IRQPinEnable() 
*/

void MCUInit(void)
{
    UINT16 u16IrqReg =0;
    UINT8 u8AttnIrq = FALSE;
    UINT8 u8TimerHi, u8TimerLo;
    
    SOPT = 0x73;                /* Turn off the watchdog. */    
    
    gu8RTxMode = RESET_DELAY;
    
    /* Add a delay to debouce the reset switch on development boards ~200ms */
    TPM1SC = 0x0D;                /* 
                                 * Set the Timer module to use BUSCLK as 
                                 * reference with Prescaler at / 32 
                                 */    
    do {
        u8TimerHi = TPM1CNTH;   /* Get value of timer register (hi byte) */
        u8TimerLo = TPM1CNTL;   /* Get value of timer register (lo byte) */
    } while (u8TimerLo <= 0x80);/* 
                                 * Poll for TIMER LO to be greater than 
                                 * 0x80 at 4MHz/32
                                 */    
    TPM1SC = 0x00;              /* Return to reset values */

    gu8RTxMode = SYSTEM_RESET_MODE;
    GPIOInit();
    SPIInit();
    IRQInit();                   /* Turn on the IRQ pin. */
    gu8RTxMode = MC13192_RESET_MODE;
    MC13192_RESET = 1;          /* Take MC13192 out of reset */

    while (u8AttnIrq == FALSE) {
    if (IRQFLAG == 1) {      /* Check to see if IRQ is asserted */
            u16IrqReg = SPIDrvRead(0x24);   /* 
                                             * Clear MC13192 interrupts and  
                                             * check for ATTN IRQ from 13192
                                             */
            u16IrqReg &= 0x400;
            if (u16IrqReg == 0) {
                u8AttnIrq = FALSE;
            }
            else {
                u8AttnIrq = TRUE;
            }
        }
    }
 

    (void)PLMEPhyReset();        /* Reset the phy to its default settings */
    IRQACK();              /* ACK the pending IRQ interrupt */
    IRQPinEnable();        /* Pin Enable, IE, IRQ CLR, negative edge. */
    gu8RTxMode = MC13192_CONFIG_MODE;
}

 /*!
  \fn UINT8 IRQPinLow() 
  \brief	Checks IRQ Pin to see if is low.
  \return	1 if IRQ is Low   
   
*/
UINT8 IRQPinLow(void){

    asm  BIL irq    
    return 0;    
    asm irq:    
    return 1;
}

#pragma INLINE
void AssertCE(void){
  MC13192_CE = 0; /*!< Asserts the MC13192 CE pin */
}
#pragma INLINE
void DeAssertCE(void){
  MC13192_CE = 1; /*!< Deasserts the MC13192 CE pin */
}
#pragma INLINE
void RTXENDeAssert(void){
  MC13192_RTXEN = 0; /*!< Deassert RTXEN */
}
#pragma INLINE
void RTXENAssert(void){
  MC13192_RTXEN = 1; /*!< Assert RTXEN */
}
#pragma INLINE
void MC13192Wake(void){
  MC13192_ATTN = 0; /*!< Assert ATTN */ 
  MC13192_ATTN = 1;  /*!< Deassert ATTN */
}