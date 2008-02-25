/*!
 * Copyright (c) 2005, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 * \file    MC13192_hw_config.h
 * \brief   Here is the header file for the routines that are used 
 *          by the MCU to initialize the MC13192.
 * \author  a20309
 * \version 4.1a
 * \date    2005/07/29 02:54:07 
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

#ifndef _MC13192_HW_CONFIG_H
#define _MC13192_HW_CONFIG_H

#include "pub_def.h"

/* Globals */
extern UINT8 gu8RTxMode;    //!< Global mode mirror 

/* Function Prototypes */
void MC13192Init(void);
void MC13192DisableInterrupts(void);
void MC13192RestoreInterrupts(void);


#endif /* _MC13192_HW_CONFIG_H */