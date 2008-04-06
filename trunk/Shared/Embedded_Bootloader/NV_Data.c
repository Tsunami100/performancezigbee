/************************************************************************************
* This module contains the NV Data module
*
* Note!! The specified link sequence is very important for the allocation of NV RAM
*        A changed link sequence can change the NV RAM layout and the pointer cannot
*        read the correct NV RAM data.
*
*       Note! This file covers version 0003 of the Freescale NVM layout. Version 0003 is backward
*             compatible with version 0001 and 0002
*
* Author(s):  Michael V. Christensen
*
* (c) Copyright 2005, Freescale Semiconductor, Inc. All rights reserved.
*
* Freescale Confidential Proprietary
* Digianswer Confidential
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected: 04-11-05
* Last Tested:
*
************************************************************************************/

//#include "LCD.h"
#include "DigiType.h"
#include "NV_Data.h"
#include "FunctionalityDefines.h"
//#include "App_Target.h"
//#include "AllTargets.h"

// **************************************************************************

#pragma MESSAGE DISABLE C3303 // Warning C3303: Implicit concatenation of strings
#pragma DISABLE_WARNING(C4200) // Warning C4200: Other segment than in previous declaration

// **************************************************************************

#pragma PLACE_CONST_SEG(BOOTLOADER_MCU_NV_DATA0)
// MCU manufacture Number
  _CONST uint8_t  MCU_Manufacture = MCU_MANUFACTURE;
// MCU ID Number
  _CONST uint8_t  MCU_Version = MCU_VERSION;
#pragma RESTORE_CONST_SEG

// **************************************************************************

#pragma PLACE_CONST_SEG(BOOTLOADER_APP_NV_DATA0)

// Initialized with default values
#define PTC_VerComplete        "PTC Demo  Ver 2.02a Build: "__DATE__" "__TIME__

volatile const Init_NV_RAM_Struct_t NV_RAM0 =
{
// Application Name
#if (!defined FLASH_LIB_ENABLED && !defined DYN_FLASH_LIB_ENABLED && !defined BOOTLOADER_ENABLED &&\
	 !defined DYN_BOOTLOADER_ENABLED && !defined  BOOTLOADER_LITE_ENABLED)
	PTC_VerComplete,
#endif (!defined FLASH_LIB_ENABLED && !defined DYN_FLASH_LIB_ENABLED && !defined BOOTLOADER_ENABLED &&\
	 !defined DYN_BOOTLOADER_ENABLED && !defined  BOOTLOADER_LITE_ENABLED)

#ifdef FLASH_LIB_ENABLED
	"PTC Demo  Ver 2.02b Build: "__DATE__" "__TIME__,
#endif FLASH_LIB_ENABLED

#ifdef DYN_FLASH_LIB_ENABLED
	"PTC Demo  Ver 2.02c Build: "__DATE__" "__TIME__,
#endif DYN_FLASH_LIB_ENABLED

#ifdef BOOTLOADER_ENABLED
	"SMAC 4_1c: "__DATE__" "__TIME__,
#endif BOOTLOADER_ENABLED

#ifdef DYN_BOOTLOADER_ENABLED
	"PTC Demo  Ver 2.02e Build: "__DATE__" "__TIME__,
#endif DYN_BOOTLOADER_ENABLED

#ifdef BOOTLOADER_LITE_ENABLED
	"PTC Demo  Ver 2.02f Build: "__DATE__" "__TIME__,
#endif BOOTLOADER_LITE_ENABLED
					  
// Embedded Bootloader version - if present
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,
// NVM Layout Version
	NVM_LAYOUT_VERSION,
// PA Level	
	PA_LEVEL,
// Channel Select
	CHANNEL_SELECT,
#ifdef SYSTEM_CLOCK_8MHZ
// System/bus frequency	
	0x08, // 8 MHz
// RF Clock Setting	
	CLOCK_OUT_SETTING,
// RF calibration Setting
	HF_CALIBRATION,
// ICG
	0x18,
	0x00, // 0x00 => CPU clk=16 MHz, Buc clk = 8 MHz
// ICG Filter
	0x02,
	0x40,
// SCI
	0x00,
	0x1A, // 1A => 19200 @ 8 MHz
#endif SYSTEM_CLOCK_8MHZ

#ifdef SYSTEM_CLOCK_12MHZ
	0x0C, // 12 MHz
	CLOCK_OUT_SETTING,
	HF_CALIBRATION,
// ICG clock
	0x18,
	0x41, // 0x41 => CPU clk=24 MHz, Buc clk = 12 MHz
// ICG Filter
	0x02,
	0x40,
// SCI
	0x00,
	0x27, // 0x27 => 19200 @ 12 MHz
#endif SYSTEM_CLOCK_12MHZ

#ifdef SYSTEM_CLOCK_16MHZ
	0x10, // 16 MHz
	CLOCK_OUT_SETTING,
	HF_CALIBRATION,
// ICG clock
	0x18,
	0x20, // 0x20 => CPU clk=32 MHz, Buc clk = 16 MHz
// ICG Filter
	0x02,
	0x40,
// SCI
	0x00,
	0x34, // 0x34 => 19200 @ 16 MHz
#endif SYSTEM_CLOCK_16MHZ

#ifdef SYSTEM_CLOCK_16_78MHZ
	0x10, // 16 MHz -> ~16,78 MHz
	CLOCK_OUT_SETTING,
	HF_CALIBRATION,
// ICG clock
	0x18,
	0x60, // 0x60 => CPU clk=32,78 MHz, Buc clk = 16,39 MHz
// ICG Filter
	0x02,
	0x40,
// SCI
	0x00,
	0x09, // 0x09 => 115200 @ 16,78 MHz
#endif SYSTEM_CLOCK_16_78MHZ

#ifdef SYSTEM_CLOCK_14_24MHZ
	0x0E, // 14,24 MHz
	CLOCK_OUT_SETTING,
	HF_CALIBRATION,
// ICG clock
	0x18,
	0x50, // 0x50 => CPU clk=28,48 MHz, Buc clk = 14,24 MHz
// ICG Filter
	0x02,
	0x40,
// SCI
	0x00,
	0x04, // 0x04 => 230400 @ 14,24 MHz
#endif SYSTEM_CLOCK_14_24MHZ

// MAC Address
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
// AntennaSelect
    0x01,
// Sleep Mode Enable
	0x00,
// HW Name	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
// Serial Number	
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
// Production Site
	0xFFFF,
// Country Code
	0xFF,
// Productin Week
	0xFF,
// Production Year	
	0xFF,
// Reserved
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
// Crystal Trim and Bias Setting	
	CRYSTAL_TRIM_AND_BIAS_SETTING,
// Channel PA
	CHANNEL_PA,
// Application_Section
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	NV_SYSTEM_FLAG
};

// **************************************************************************

#pragma PLACE_CONST_SEG(BOOTLOADER_APP_NV_DATA1)
  // This is the empty "copy" sector - contains all 0xFF's
  volatile const NV_RAM_Struct_t NV_RAM1;
#pragma RESTORE_CONST_SEG

#pragma PLACE_DATA_SEG(NV_RAM_POINTER)
  volatile NV_RAM_Struct_t *NV_RAM_ptr; // A pointer to NV Data will be setup in init code.
#pragma RESTORE_DATA_SEG

// **************************************************************************
// Place NVM holder, so that Bootloader can find them

#if defined DYN_BOOTLOADER_ENABLED || defined DYN_FLASH_LIB_ENABLED

    #pragma PLACE_CONST_SEG(APP_BOOTLOADER_NVM0_STORED)
        NV_RAM_Struct_t *const NVM0_ptr = (NV_RAM_Struct_t*)Freescale_Copyright;
    #pragma RESTORE_CONST_SEG
  
    #pragma PLACE_CONST_SEG(APP_BOOTLOADER_NVM1_STORED)
        NV_RAM_Struct_t *const NVM1_ptr = (NV_RAM_Struct_t*)&NV_RAM1;
    #pragma RESTORE_CONST_SEG

#endif defined DYN_BOOTLOADER_ENABLED || defined DYN_FLASH_LIB_ENABLED

void NV_Data_Init(void)
{
  // Addition to Include_Symboles in Main.c but put here for easy rebuild
	if ((MCU_Manufacture != MCU_MANUFACTURE) || (MCU_Version != MCU_VERSION))
	  __asm nop;
			
	// Setup NV RAM pointer after update.
}
