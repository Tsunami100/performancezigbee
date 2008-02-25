 /*!
 * Copyright (c) 2005, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 * \file    MC13192_hw_config.c
 * \brief   Here are the routines that are used by the MCU to initialize
 *          the MC13192.
 * \author  a19259
 * \version 4.1a
 * \date    2005/07/29 02:27:50 
 * 
 * \b Description:
 *
 * The main routine is the one that initializes the MC13192.There are
 * also included the Disable/Restore MC13192 interrupts commands.
 *
 * \b Department: Freescale Radio Products Division 
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 16/11/2005 Doc. update to Doxygen compliant by Mario Rodriguez r12369c
 */ 

#include "pub_def.h"
#include "mcu_hw_config.h"
#include "drivers.h"
#include "MC13192_hw_config.h"

/* Globals */
extern UINT8 gu8RTxMode;		//!< Global mode mirror. 
extern UINT8 gu8IRQValue;		//!< IRQ value. 

/*!
 * \brief MC13192Init: Initialize the MC13192 register map.
 *
 */
void MC13192Init(void)
{

/* Please refer to document MC13192RM for hidden register initialization */
    SPIDrvWrite(0x11,0x20FF);   /* Eliminate Unlock Conditions due to L01 */
    SPIDrvWrite(0x1B,0x8000);   /* Disable TC1. */
    SPIDrvWrite(0x1D,0x8000);   /* Disable TC2. */
    SPIDrvWrite(0x1F,0x8000);   /* Disable TC3. */
    SPIDrvWrite(0x21,0x8000);   /* Disable TC4. */
    SPIDrvWrite(0x07,0x0E00);   /* Enable CLKo in Doze */
    SPIDrvWrite(0x0C,0x0300);   /* IRQ pull-up disable. */
    (void)SPIDrvRead(0x25);           /* Sets the reset indicator bit */
    SPIDrvWrite(0x04,0xA08D);   /* New cal value */
    SPIDrvWrite(0x08,0xFFF7);   /* Preferred injection */
    SPIDrvWrite(0x05,0x8351);   /* Acoma, TC1, Doze, ATTN masks, LO1, CRC */
    SPIDrvWrite(0x06,0x4720);   /* CCA, TX, RX, energy detect */
 
    /* Read the status register to clear any undesired IRQs. */
    (void)SPIDrvRead(0x24);           /* Clear the status register, if set */
    gu8RTxMode = IDLE_MODE;     /* Update global to reflect MC13192 status */

}


/*!
 * \brief MC13192DisableInterrupts : Disable MC13192 interrupts
 *
 */
void MC13192DisableInterrupts(void)
{
    
    MC13192_IRQ_Disable();  /* Save context & disable the MC13192 interrupt source */
}


/*!
 * \brief MC13192RestoreInterrupts : Restore MC13192 interrupts to previous condition
 *
 */
void MC13192RestoreInterrupts(void)
{
    MC13192_IRQ_Enable();   /* Restore the context of the IRQ register from global */
}


