/************************************************************************************
* This module contains the Embedded Bootloader interface description.
*
* Note! This module contains function pointers to function accessable in Embedded Bootloader
*       This file contains absolute address (See system memory map) - BE CAREFULL.
*
*       This version of the Embedded Bootloader interface file MUST be used with the
*       Embedded Bootloader firmware version 5.00 and above.
*       Use the Freeloader_inf.h for Embedded Bootloader (Freeloader) versions below 5.00.
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

#ifndef _EMBEDDED_BOOTLOADER_H_
#define _EMBEDDED_BOOTLOADER_H_
#include "app_config.h"
#if (EMBEDDED_BOOTLOADER == TRUE) // Make sure that no defines etc. is available without define.

#include "DigiType.h"

// **************************************************************************

// Embedded BootLoader control flags:
#define NO_BIT_ENABLED                ((uint8_t)0x00) // Note! The option description values is inverted.
#define ERASE_FLASH                    ((uint8_t)0x01) // Option: 'Skip flash erase', Enabled = ‘0’, disabled = ‘1’
#define BOOT_AFTER_DOWNLOAD            ((uint8_t)0x02) // Option: 'Do not reset after upload', Enabled = ‘0’, disabled = ‘1’
#define KEEP_NV_RAM                    ((uint8_t)0x04) // Option: 'Erase production data', Enabled = ‘0’, disabled = ‘1’
#define PERFORM_FLASH_VERIFICATION    ((uint8_t)0x08) // Option: 'Skip firmware checksum verification', Enabled = ‘0’, disabled = ‘1’

#define DOWNLOAD_MSG_RECEIVED        ((uint8_t)0x40)
#define RESET_MSG_RECEIVED            ((uint8_t)0x80)

#define ALL_BIT_ENABLED                (ERASE_FLASH | BOOT_AFTER_DOWNLOAD | KEEP_NV_RAM | PERFORM_FLASH_VERIFICATION)

#define ERASED_STATE                ((uint8_t)0xFF)

// Embedded BootLoader boot flag:
#define EXECUTE_APPLICATION            ((uint8_t)0x55) // Application present - execute application
#define DO_UPDATE_FIRMWARE          ((uint8_t)0x00) // Application was present - download new application
#define FLASH_EMPTY                 ((uint8_t)0xFF) // Flash is in erased state - No application

#define FLASH_ERROR                    ((uint8_t)0xAA) // Flash error

// **************************************************************************

#ifndef FOR_BOOTLOADER_ONLY

// Define some function pointer, which points to function in Embedded BootLoader code space
typedef uint8_t (*Enable_Download_Firmware_ptr_t)(uint8_t interface_state, uint8_t firmware_state);
typedef void(*Hard_Reset_ptr_t)(void);
typedef void (*NV_Flash_Setup_ptr_t)(void);
typedef bool_t (*Update_NV_RAM_ptr_t)(uint8_t *NV_RAM_Distination_ptr, uint8_t *Source_ptr, uint16_t Source_Length);
typedef void (*ICG_Setup_ptr_t)(void);
typedef void (*UART_Port_Select_ptr_t)(uint8_t data_reg_address, uint8_t status_reg_address);


#define BOOTLOADER_CODE_ADDRESS 0xF000 // Only change this address

#define Enable_Download_Firmware_Address  (BOOTLOADER_CODE_ADDRESS + 0)
#define Hard_Reset_Address                (BOOTLOADER_CODE_ADDRESS + 2)
#define NV_Flash_Setup_Address            (BOOTLOADER_CODE_ADDRESS + 4)
#define Update_NV_RAM_Address             (BOOTLOADER_CODE_ADDRESS + 6)
#define ICG_Setup_Address                 (BOOTLOADER_CODE_ADDRESS + 8)
#define UART_Port_Select_Address          (BOOTLOADER_CODE_ADDRESS + 10)

extern const uint8_t boot_loader_control;
extern const uint8_t boot_loader_flag;

// Same name as the real function in BootLoader
extern Enable_Download_Firmware_ptr_t Enable_Download_Firmware;
extern Hard_Reset_ptr_t Hard_Reset;
extern NV_Flash_Setup_ptr_t NV_Flash_Setup;
extern Update_NV_RAM_ptr_t Update_NV_RAM;
extern ICG_Setup_ptr_t FL_ICG_Setup;
extern UART_Port_Select_ptr_t UART_Port_Select;

extern void BootLoader_Interface_Init(void);

#endif FOR_BOOTLOADER_ONLY

// **************************************************************************

#endif BOOTLOADER_ENABLED

#endif _EMBEDDED_BOOTLOADER_H_
