/*!
 * Copyright (c) 2005, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 * \file    mcu_hw_config.h
 * \brief   Header used to select the desired core.
 * \author  a20639
 * \version 4.1a
 * \date    2005/08/11 20:44:37 
 * 
 * \b Description:
 *
 * Header that includes the declaration of the methods used to
 * configure the hardware
 *
 * \b Department: Freescale Radio Products Division 
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 11/16/2005 Doc. update to Doxygen compliant by Gonzalo Delgado 
 * Huitron rgd04c
 */ 

//#ifndef _MCU_HW_CONFIG_H
#define _MCU_HW_CONFIG_H
#include "freescale_radio_hardware.h"
#include "pub_def.h"


/* Defines */


#define MCU_LOW_POWER_WHILE() _asm wait			/*!<Low power Wait instruction*/

void AssertCE(void);            
void DeAssertCE(void);          
void RTXENDeAssert(void) ;      
void RTXENAssert(void);         
void MC13192Wake(void);         
void MC13192Restart(void);
void MC13192ContReset(void);
                               


#define IRQFLAG                     IRQSC_IRQF	 /*!< IRQ Flag*/
#define IRQACK()                    IRQSC |= 0x04 /*!< IRQ Acknowledge enabled*/
#define IRQInit()                   IRQSC = 0x14  /*!< Configures IRQ*/
#define IRQPinEnable()              IRQSC = 0x16  /*!<Enables IRQ pin */
#define IRQ_Disable()               IRQSC = 0x00;/*!< Set for negative edge. */
#define MC13192_IRQ_Disable()       MC13192_IRQ_SOURCE = MC13192_IRQ_SOURCE & ~(0x06) /*!<Disables MC13192 transceiver */
#define MC13192_IRQ_Enable()        MC13192_IRQ_SOURCE = MC13192_IRQ_SOURCE | (0x02)  /*!<Enables MC13192 transceiver */

//    MC13192_IRQ_SOURCE = gu8IRQValue;
#define CLEAR_IRQ_FLAG()            IRQACK()	 /*!< Clears IRQ flag*/


/* Declarations */
extern UINT8 gu8RTxMode;            //!< Global mode mirror.

/* Function Prototypes */
void GPIOInit(void);
#if defined (HCS08G) || defined (HCS08R) || defined (MC1321X)
void UseExternalClock(void);
void UseMcuClock(void);
#endif
void MCUInit(void);
UINT8 IRQPinLow(void);




//#endif /* _MCU_HW_CONFIG_H */