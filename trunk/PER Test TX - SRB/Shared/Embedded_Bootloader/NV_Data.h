/************************************************************************************
* This module contains the NV Data module
*
* Note!! The specified link sequence is very important for the allocation of NV RAM
*        A changed link sequence can change the NV RAM layout and the pointer cannot
*        read the correct NV RAM data.
*
*       Note! This file covers version 0004 of the Freescale NVM layout. Version 0004 is backward
*             compatible with version 0001 and 0002.
*
* Author(s):  Michael V. Christensen
*
* (c) Copyright 2005, Freescale, Inc.  All rights reserved.
*
* Freescale Confidential Proprietary
* Digianswer Confidential
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected: 19-12-05
* Last Tested:
*
************************************************************************************/

#ifndef _NV_DATA_H_
#define _NV_DATA_H_

#include "DigiType.h"
#include "FunctionalityDefines.h"

#ifdef PLATFORM_WINDOWS
	#define _CONST
#else
	#define _CONST   const
#endif PLATFORM_WINDOWS

// These addresses MUST be the same as the ones in Linker file
// The values are used by the Bootloader to find the NVM sectors
// Should not be changed - be carefull!!!
#ifdef DYN_FLASH_LIB 
    #define GET_NV_RAM0_ADDRESS_FROM 0xEFBA
    #define GET_NV_RAM1_ADDRESS_FROM 0xEFBC
#endif DYN_FLASH_LIB 

// These address MUST be the same as the ones in Linker file
// Be carefull!!!
#define NV_RAM0_ADDRESS 0x1400
#define NV_RAM1_ADDRESS 0x1600

#define HCS08_SECTOR_SIZE 9 // 512 bytes, 2^9=512 used instead of division

// Sector number must match address above
#define NV_RAM0_SECTOR 10
#define NV_RAM1_SECTOR 11

// Pointer to hold location of NVM
#ifdef DYN_FLASH_LIB 
    #define stored_NVM0_ptr  (*(NV_RAM_Struct_t**)GET_NV_RAM0_ADDRESS_FROM)
    #define stored_NVM1_ptr  (*(NV_RAM_Struct_t**)GET_NV_RAM1_ADDRESS_FROM)
#else
    #define stored_NVM0_ptr  ((NV_RAM_Struct_t*)NV_RAM0_ADDRESS)
    #define stored_NVM1_ptr  ((NV_RAM_Struct_t*)NV_RAM1_ADDRESS)
#endif DYN_FLASH_LIB 

// **************************************************************************
#define NV_RAM_SIZE (sizeof(NV_RAM_Struct_t))

// System flag to detect a full copied section
#define NV_SYSTEM_FLAG  ((uint8_t)0x55)
#define ERASED_BYTE     ((uint8_t)0xFF)
#define ERASED_WORD     ((uint16_t)0xFFFF)

// **************************************************************************
// Defines for NV DATA

// Values for Abel register 0x04
#define ABEL_CCA_ENERGY_DETECT_THRESHOLD 0x9600 //MSB=CCA Energy detect threshold: abs(power in dBm)*2, 0x96=-75dBm, 0x82=-65dBm
#define ABEL_POWER_COMPENSATION_OFFSET   0x0074 //LSB=Power compensation Offset added to RSSI. Typical 0x74 FFJ/JK 13/01/04 (Abel 013)

// Register 0x0A, bit (15:8)
// Xtal trim - crystal oscillator capacitor trim
// Setting       AbelX.X
// ---------    ------------
//   0x00         Default from reset
#define ABEL_XTAL_TRIM (0x36 << 8) // MSB

// Register 0x0A, bit (7:6)
// Xtal bias - crystal oscillator bias adjustment
// Setting       Abel2.0            Abel2.1
// ---------    ------------       ------------
//   00           5 (Doze)           5          (Default from reset)
//   01           8 (Idle)           8
//   10           11                 11 (Doze, Idle)
//   11           84 (Boost)         84 (Boost)
#define ABEL_XTAL_BIAS_CURRENT 0x80 // LSB

// Register 0x0A, bit (5:3)
// Chip rate
// Setting       AbelX.X
// ---------    ------------
//   0x00         Default from reset
#define ABEL_CHIP_RATE 0x00 // LSB

// Register 0x0A, bit (2:0)
// CLKO rate - selects the clock frequency of the CLKO out pin
// Setting       AbelX.X
// ---------    ------------
//   000          16MHz
//   001          8MHz
//   010          4MHz
//   011          2MHz
//   100          1MHz
//   101          62,5kHz
//   110          31,25kHz (default from reset)
//   111          15,625kHz
#define ABEL_CLKO_FREQ 0x05 // LSB


// **************************************************************************
// Some default NVM parameters

#define NVM_LAYOUT_VERSION      0x0004
#define MCU_MANUFACTURE         0x00 // = Freescale
#define MCU_VERSION             0x02

#ifdef TARGET_TOROWEAP
    // Using Toro RF
    #define CLOCK_OUT_SETTING   0x7E85
    #define HF_CALIBRATION      0x968D
#else
    // Using Abel RF
    #define CLOCK_OUT_SETTING   0x3645
    #define HF_CALIBRATION      (ABEL_CCA_ENERGY_DETECT_THRESHOLD | ABEL_POWER_COMPENSATION_OFFSET)
#endif TARGET_TOROWEAP

#define CRYSTAL_TRIM_AND_BIAS_SETTING CLOCK_OUT_SETTING // Default

#define CHANNEL_SELECT          12 // A number between [11;26]

#define PA_LEVEL                0x00BC

// Channel number                 11   12   13 	 14   15   16   17   18   19   20   21   22   23   24   25   26
#define CHANNEL_PA              {0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC,0xBC}


// **************************************************************************
// External Prototypes
extern const uint8_t Freescale_Copyright[54]; // Defined in MacMain.c
extern const uint8_t Firmware_Database_Label[40]; // Defined in MacMain.c
extern const uint8_t MAC_Version[47]; // Defined in MacMain.c
extern const uint8_t PHY_Version[47]; // Defined in PhyMain.c

// This is the NV RAM initializer layout.
//
// DO NOT USE THIS TYPE WHEN READING/WRITING DATA
// 
typedef struct Init_NV_RAM_Struct
{
	// Missing version strings
	_CONST uint8_t  Target_Version[48];
	_CONST uint8_t  Embedded_Bootloader_Firmware_Version[52];
	_CONST uint16_t NV_RAM_Version;
	_CONST uint8_t  PA_Level;
	_CONST uint8_t  ChannelSelect;
	_CONST uint8_t  Bus_Frequency_In_MHz;
    _CONST uint16_t Abel_Clock_Out_Setting; // Name kept for historical reasons
	_CONST uint16_t HF_Calibration;
	_CONST uint8_t  NV_ICGC1;
	_CONST uint8_t  NV_ICGC2;
	_CONST uint8_t  NV_ICGFLTU;
	_CONST uint8_t  NV_ICGFLTL;
	_CONST uint8_t  NV_SCI1BDH;
	_CONST uint8_t  NV_SCI1BDL;
	_CONST uint8_t  MAC_Address[8];
	_CONST uint8_t  AntennaSelect;
	_CONST uint8_t  SleepModeEnable;
	_CONST uint8_t  HWName_Revision[20];
	_CONST uint8_t  SerialNumber[10];
	_CONST uint16_t ProductionSite;
	_CONST uint8_t  CountryCode;
	_CONST uint8_t  ProductionWeekCode;
	_CONST uint8_t  ProductionYearCode;
	_CONST uint8_t  Reserved[8];
	_CONST uint16_t Crystal_Trim_and_Bias_Setting;
	_CONST uint8_t  ChannelPA[16];
	_CONST uint8_t  Application_Section[137];
	_CONST uint8_t  System_Flag; // Must not be changed
} Init_NV_RAM_Struct_t;

// **************************************************************************

// This is the NV RAM layout. The layout covers a whole physical flash sector
// (512 bytes) in HCS08. The NV RAM data while be copied to another physical sector
// when updated.
typedef struct NV_RAM_Struct
{
	_CONST uint8_t  Freescale_Copyright[54];
	_CONST uint8_t  Firmware_Database_Label[38];
	_CONST uint8_t  MCU_Manufacture;
	_CONST uint8_t  MCU_Version;
	_CONST uint8_t  MAC_Version[47];
	_CONST uint8_t  PHY_Version[47];
	_CONST uint8_t  Target_Version[48];
	_CONST uint8_t  Embedded_Bootloader_Firmware_Version[52];
	_CONST uint16_t NV_RAM_Version;
	_CONST uint8_t  PA_Level;
	_CONST uint8_t  ChannelSelect;
	_CONST uint8_t  Bus_Frequency_In_MHz;
	_CONST uint16_t Abel_Clock_Out_Setting; // Name kept for historical reasons
	_CONST uint16_t HF_Calibration;
	_CONST uint8_t  NV_ICGC1;
	_CONST uint8_t  NV_ICGC2;
	_CONST uint8_t  NV_ICGFLTU;
	_CONST uint8_t  NV_ICGFLTL;
	_CONST uint8_t  NV_SCI1BDH;
	_CONST uint8_t  NV_SCI1BDL;
	_CONST uint8_t  MAC_Address[8];
	_CONST uint8_t  AntennaSelect;
	_CONST uint8_t  SleepModeEnable;
	_CONST uint8_t  HWName_Revision[20];
	_CONST uint8_t  SerialNumber[10];
	_CONST uint16_t ProductionSite;
	_CONST uint8_t  CountryCode;
	_CONST uint8_t  ProductionWeekCode;
	_CONST uint8_t  ProductionYearCode;
	_CONST uint8_t  Reserved[8];
	_CONST uint16_t Crystal_Trim_and_Bias_Setting;
	_CONST uint8_t  ChannelPA[16];
	_CONST uint8_t  Application_Section[137];
	_CONST uint8_t  System_Flag; // Must not be changed
} NV_RAM_Struct_t;

// **************************************************************************

extern volatile const Init_NV_RAM_Struct_t NV_RAM0; // Initialized with default values
extern volatile const NV_RAM_Struct_t NV_RAM1; // This is the empty "copy" sector - contains all 0xFF's
extern volatile NV_RAM_Struct_t *NV_RAM_ptr; // A pointer to NV Data
extern void NV_Data_Init(void);

#if defined DYN_BOOTLOADER_ENABLED || defined DYN_FLASH_LIB_ENABLED
  extern NV_RAM_Struct_t *const NVM0_ptr;
  extern NV_RAM_Struct_t *const NVM1_ptr;
#endif defined DYN_BOOTLOADER_ENABLED || defined DYN_FLASH_LIB_ENABLED

// **************************************************************************

#endif _NV_DATA_H_
