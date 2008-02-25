/*!
 * Copyright (c) 2005, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 * \file    mcu_spi_config.h
 * \brief   Header used to select the desired core.
 * \author  Larry Roshak
 * \version 4.1a
 * \date    2005/08/11 20:44:37 
 * 
 * \b Description:
 *
 * This is the SMAC C source driver header file for SPI
 * Driver in the MCU.  The header defines all externals, prototypes and 
 * mask bits used by the actual C driver.
 *
 * \b Department: Freescale Radio Products Division 
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 17/04/2004 : Initial Development. (FLR005)
 * - 07/01/2005 : Code Standarization. (A19259)
 * - 11/16/2005 Doc. update to Doxygen compliant by Gonzalo Delgado 
 * Huitron rgd04c
 */ 


#ifndef _SPI_DRIVERS_H
#define _SPI_DRIVERS_H

#include "derivative.h"
#include "pub_def.h"
#include "mcu_hw_config.h"
#include "app_config.h"

/*  SPI Function Defines */

#if defined (HCS08G) | defined (HCS08R)

void SPIInit(void);
 
void SPIWaitTransferDone(void);       
void SPIClearRecieveStatReg(UINT8 * u8TempValue);    
void SPIClearRecieveDataReg(UINT8 * u8TempValue);    
void SPISendChar(UINT8 u8Char);         
UINT8 SPIRead(void);                   
void SPIReadMSB(UINT16 * u16Data);                
void SPIReadLSB(UINT16 * u16Data);                

#define SPIClkInvert    SPI1C1 |= 0x04; /*!< Set CPHA bit of SPCR (clk polarity) */
#define SPIClkNormal    SPI1C1 &= 0xFB; /*!< Clr CPHA bit of SPCR (clk polarity) */



#endif /*! (HCS08G) | defined (HCS08R) */
#endif /*! _SPI_DRIVERS_H */