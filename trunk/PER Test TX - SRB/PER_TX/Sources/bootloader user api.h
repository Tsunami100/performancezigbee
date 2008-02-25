/********************************************************
application calls for Bootloader.

Author: LTR
File: bootloader user api.h
********************************************************/
//Includes
#include "NV_data.h"

#pragma MESSAGE DISABLE C4200
//Prototypes
void boot_init(void);
void boot_call(void);

//Globals
#pragma PLACE_CONST_SEG(BOOTLOADER_MAC_NV_DATA0)
extern const unsigned char Freescale_Copyright[54];
extern const unsigned char Firmware_Database_Label[40];
extern const unsigned char SMAC_Version[47]; 
extern const unsigned char SPHY_Version[46];
extern volatile NV_RAM_Struct_t *NV_RAM_ptr;
extern const uint8_t boot_loader_control;
extern const uint8_t boot_loader_flag;
#pragma RESTORE_CONST_SEG
