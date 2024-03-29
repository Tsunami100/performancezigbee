/************************************************************************************
* This module contains the FreeLoader interface description.
* 
*
* Author(s):  Michael V. Christensen
*
* (c) Copyright 2004, Freescale Semiconductor, Inc. All rights reserved.
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
*
* Source Safe revision history (Do not edit manually) 
*   $Date: 2006/09/06 06:07:53 $
*   $Author: rgd04c $
*   $Revision: 1.1.2.1 $
*   $Workfile: MotoLoader_inf.c $
************************************************************************************/
#include "app_config.h"
// Make sure that no defines etc. is available without define.
#if (EMBEDDED_BOOTLOADER == TRUE)

#include "DigiType.h"
//#include "FreeLoader_inf.h" // Enable this header file for version < 5.00 of Embedded Bootloader 
#include "Embedded_Bootloader.h" // Enable this header file for version >= 5.00 of Embedded Bootloader

// **************************************************************************

#pragma MESSAGE DISABLE C4200 // Warning C4200: Other segment than...

// **************************************************************************
#pragma CONST_SEG APP_BOOTLOADER_FLAGS

// Default setting for how BootLoader downloads firmware
const uint8_t boot_loader_control = ALL_BIT_ENABLED;
const uint8_t boot_loader_flag = EXECUTE_APPLICATION;

#pragma CONST_SEG DEFAULT

// Function pointers
Enable_Download_Firmware_ptr_t Enable_Download_Firmware;
Hard_Reset_ptr_t Hard_Reset;
NV_Flash_Setup_ptr_t NV_Flash_Setup;
Update_NV_RAM_ptr_t Update_NV_RAM;
ICG_Setup_ptr_t FL_ICG_Setup;
UART_Port_Select_ptr_t UART_Port_Select;

// **************************************************************************

void BootLoader_Interface_Init(void)
{
    // Setup function pointer - MUST POINT TO FUNCTION IN BOOTLOADER SPACE
    Enable_Download_Firmware = *((Enable_Download_Firmware_ptr_t*)Enable_Download_Firmware_Address);
    Hard_Reset = *((Hard_Reset_ptr_t*)Hard_Reset_Address);
    NV_Flash_Setup = *((NV_Flash_Setup_ptr_t*)NV_Flash_Setup_Address);
    Update_NV_RAM = *((Update_NV_RAM_ptr_t*)Update_NV_RAM_Address);
    FL_ICG_Setup = *((ICG_Setup_ptr_t*)ICG_Setup_Address);
    UART_Port_Select = *((UART_Port_Select_ptr_t*)UART_Port_Select_Address);
}

// **************************************************************************

// Make sure that no defines etc. is available without define.
#endif defined BOOTLOADER_ENABLED && !defined FOR_BOOTLOADER_ONLY
