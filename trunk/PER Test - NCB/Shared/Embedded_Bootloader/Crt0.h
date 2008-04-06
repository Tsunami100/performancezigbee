/************************************************************************************
* This module contains some basic init code.
* Note! Basic code is adapted (start08.h and hidef.h) from some code developed by Metrowerks. 
* 
* Note! The init code in this file can be used with a D18/Application with or without the
*       BootLoader. #define FOR_BOOTLOADER_ONLY MUST be specificed for use with BootLoader
*
* Author(s):  Michael V. Christensen
*
* (c) Copyright 2004, Freescale, Inc.  All rights reserved.
*
* Freescale Confidential Proprietary
* Digianswer Confidential
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected: 29-03-01
* Last Tested:
************************************************************************************/

#ifndef _CRT0_H
#define _CRT0_H

/**********************************************************************/
// Includes

#include "DigiType.h"

#ifdef FOR_BOOTLOADER_ONLY
    #include "NV_Flash.h"
#endif FOR_BOOTLOADER_ONLY

/**********************************************************************/
// Defines

#define SYSTEMS_OPTION_REG_VALUE    0x33 // Cop disable, short timeout, STOP mode enabled, background debug enabled

// Table with some filter values and corresponding bus frequency (measure with scope).
// MSB  LSB  => Bus frequency in MHz:

// 0x00 0x00 => ~1,9 MHz minimum SCM frequency

// 0x00 0xC0 => ~4,2 MHz default out of reset
// 0x01 0x20 => ~5,2 MHz
// 0x01 0x80 => ~6,2 MHz
// 0x01 0xE0 => ~7,1 MHz
// 0x02 0x40 => ~7,9 MHz
// 0x02 0xA0 => ~8,7 MHz
// 0x03 0x00 => ~9,5 MHz

// 0x0F 0xFF => ~30  MHz maximum SCM frequency

// Note! Do not use minimum and maximum values.

// Default filter values => ~8 MHz bus frequency
#define ICG_FILTER_MSB 0x02;
#define ICG_FILTER_LSB 0x40; // LSB value must be written first

// Definitions for none-volatile registers.
#define NV_BACKKEY_LENGTH   8
#define NV_RESERVED_LENGTH  5

// ONLY CHANGE THIS SECTION
#define NV_BACKKEY_VALUE    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
#define NV_RESERVED_VALUE   0xFF,0xFF,0xFF,0xFF,0xFF

#ifdef FOR_BOOTLOADER_ONLY
    // Use values defined in flash module
    #define NV_PROT_VALUE       NVFPROT_VALUE
    #define NV_OPT_VALUE        NVFOPT_VALUE
#else
    // Use some default values - flash programming functions not available
    #define NV_PROT_VALUE       0x98 // Any flash location (not protected) can be erased or programmed, flash is protected as
                                     // specified in FPS2:FPS1:FPS0 (0xF000-0xFFFF)(=4KB).
    #define NV_OPT_VALUE        0x02 // No backdoor key allowed, vectors redirect enabled, unsecure.
#endif FOR_BOOTLOADER_ONLY

// Default value
#define NV_ICG_TRIM_VALUE   0x80 // This value must/should be calibrated by production test system.

#define FAR  // Empty - for tiny FAR is used as segment modifier
#define _FAR __far

#define toCopyDownBegOffs          0

// Only 2 flags are define
#define STARTUP_FLAGS_NONE         0
#define STARTUP_FLAGS_ROM_LIB      (1<<0)  // If module is a ROM library
#define STARTUP_FLAGS_NOT_INIT_SP  (1<<1)  // If stack pointer has not to be initialized

// LibInits is only used for ROM libraries
#define INCLUDE_ROM_LIBRARIES      0 // Switch on to implement ROM libraries

/**********************************************************************/
// Types

// HCS08 NV register type
typedef struct NV_REG
{
    uint8_t    nv_backkey[NV_BACKKEY_LENGTH];
    uint8_t    nv_reserved[NV_RESERVED_LENGTH];
    uint8_t    nv_prot;
    uint8_t    nv_icg_trim; // This value must/should be calibrated by production test system.
    uint8_t    nv_opt;
} NV_REG_t;

//  the following datastructures contain the data needed to
//  initialize the processor and memory 

typedef struct
{
    unsigned char *_FAR beg;
    int size; // [beg..beg+size]
} _Range;

typedef struct _Copy
{
    int size;
    unsigned char *_FAR dest;
} _Copy;

typedef void (*_PFunc)(void);

#ifdef INCLUDE_ROM_LIBRARIES
    typedef struct _LibInit
    {
        _PFunc  *startup; // Address of startup desc
    } _LibInit;
#endif INCLUDE_ROM_LIBRARIES

// attention: the linker scans the debug information for this structures
// to obtain the available fields and their sizes.
// So dont change the names in this file.

typedef struct _tagStartup
{
    unsigned char   flags;                     // STARTUP_FLAGS_xxx
    _PFunc          main;                      // Top level procedure of user program
    unsigned short  stackOffset;             // Initial value of the stack pointer
    unsigned short  nofZeroOuts;            // Number of zero out ranges
    _Range          *_FAR pZeroOut;            // Vector of ranges with nofZeroOuts elements
    _Copy           *_FAR toCopyDownBeg;    // ROM-address where copydown-data begins

#if INCLUDE_ROM_LIBRARIES
    unsigned short  nofLibInits;             // number of library startup descriptors
    _LibInit        *_FAR libInits;         // Vector of pointers to library startup descriptors
#endif INCLUDE_ROM_LIBRARIES

} _startupData_t;    


/**********************************************************************/
// Macros

#define DisableInterrupts          __asm SEI;
#define EnableInterrupts           __asm CLI;

#ifdef FOR_BOOTLOADER_ONLY
    #define STARTUP_DATA        _startupData_var
    #define CALL_MAIN_INTERFACE        main_interface(); // Call main interface function
#else
    #define STARTUP_DATA        _startupData
    #define CALL_MAIN_INTERFACE        Init();\
                                    (*_startupData.main)();    // Call user "main" function
#endif FOR_BOOTLOADER_ONLY

/**********************************************************************/
// Prototypes data

extern const volatile NV_REG_t none_volatile_struct;

/**********************************************************************/
// Prototypes

#ifdef FOR_BOOTLOADER_ONLY
    extern void _Startup (void); // DO NOT CHANGE FUNCTION NAME
    extern void Copy_startup_structure(_startupData_t * copy_structure);
    extern _startupData_t _startupData; // Stored in flash
    extern _startupData_t _startupData_var;
#endif FOR_BOOTLOADER_ONLY

extern void Find_NV_RAM(void);

/**********************************************************************/

#endif _CRT0_H

