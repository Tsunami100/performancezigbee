/**
 * Copyright (c) 2004, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 * File name :  LCD.h
 * Project name: SMAC (Simple Media Access Controller)
 *
 * Department : Freescale Radio Products Division
 *
 * Description : Header for LCD functions.
 *
 * $Author: rgd04c $
 * $Date: 2006/09/25 14:47:08 $
 * $Name:  $
 */
/* File: LCD.h */
#include "pub_def.h"
#define MAX_LCD_CHARS 16

/* LCD Functions */

void LCDWaitLong(UINT16 w);

void LCDWaitShort(UINT8 w );

void LCDToggleEN( void );

void LCDInit(void);

void LCDWrite4bits(UINT8 bdata);

void LCDFirstLine(void);

void LCDSecondLine(void);

void LCDClearDisplay(void);

void LCDWriteString(UINT8 line, UINT8 *str);

void LCDWriteChar(UINT8 ch);

