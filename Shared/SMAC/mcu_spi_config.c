/*!
 * Copyright (c) 2005, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 * \file    mcu_spi_config.c
 * \brief   This is the SMAC C source for the SPI driver
 * \author  Larry Roshak
 * \version 4.1a
 * \date    2005/08/11 20:44:37 
 * 
 * \b Description:
 *
 * This is the SMAC C source driver file for the 
 * The driver control interface to the SPI.
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

#include "pub_def.h"
#include "mcu_hw_config.h"
#include "derivative.h"
#include "drivers.h"
#include "MC13192_hw_config.h"
#include "MC13192_regs.h"
#include "mcu_spi_config.h"


/*!
  \fn void SPIInit(void)
  \brief Initialize the SPI Control and Baud Rate Registers
*/
void SPIInit(void){
    SPI1C1 = 0x50;   /*  
                      *  0b01010000
                      *    ||||||||__ SPI serial data transfers start with MSB
                      *    |||||||___ SS1 used as GPIO
                      *    ||||||____ First edge occurs at the middle of cycle
                      *    |||||_____ Active-high SPI clock
                      *    ||||______ SPI Configured as Master
                      *    |||_______ SPI Transmit Interrupt Disabled
                      *    ||________ SPI System Enabled
                      *    |_________ SPI Interrupt Disabled
                      */
    SPI1C2 = 0x00;   /*  
                      *  0b00000000
                      *    ||||||||__ Separate pins for data input and output.
                      *    |||||||___ SPI clocks operate in wait mode.
                      *    ||||||____ Unimplemented
                      *    |||||_____ SPI data I/O pin acts as an input.
                      *    ||||______ Mode Fault Disabled
                      *    |||_______ Unimplemented
                      *    ||________ Unimplemented
                      *    |_________ Unimplemented
                      */
    SPI1BR = 0x00;   /*  
                      *  0b00000000
                      *    |||||||| 
                      *    ||||||||
                      *    ||||||++-- Rate Divisor = 2
                      *    |||||_____ Unimplemented
                      *    |||| 
                      *    |||| 
                      *    ||++------ Prescaler Divisor = 0 
                      *    |_________ Unimplemented
                      */
}

/*!
 * \def SPIWaitTransferDone()
 * \brief Waits until the SPI1D has been transferred and received
 */
#pragma INLINE
void SPIWaitTransferDone(void){
  while (!(SPI1S_SPRF));
}

/*!
 * 
 * \def SPIClearRecieveStatReg()
 * \brief SPI_ClearRecieveStatReg : Clears any flags that may be activated in the
 * the SPI Data Register.
 */
#pragma INLINE
void SPIClearRecieveStatReg(UINT8 * u8TempValue){
  *u8TempValue = SPI1S;
}

/*! 
 * \def SPIClearRecieveDataReg()
 * \brief SPIClearRecieveDataReg : Clear receive data register. SPI entirely ready 
 * for read or write 
 */
#pragma INLINE
void SPIClearRecieveDataReg(UINT8 * u8TempValue){
  *u8TempValue = SPI1D;
}

/*! 
 * \def SPISendChar()
 * \brief SPI_SendChar : Sends a Char through the SPI port that is being used
 *
 * Parameters : u8Char - Char to be sent through the SPI
 */
#pragma INLINE
void SPISendChar(UINT8 u8Char){
  SPI1D = u8Char;
}

/*! 
 * \def SPIRead()
 * \brief SPIRead : Returns the data that is in the SPI port.
 * Return : UINT8 - Data that is in the SPI port being used.
 */
#pragma INLINE
UINT8 SPIRead(void){
  return SPI1D;	 /*!<SPI Data Register*/
}


#pragma INLINE
void SPIReadMSB(UINT16 * u16Data){
  ((UINT8*)u16Data)[0] = SPI1D; /*!<SPI More Significant Byte*/	
//  ((UINT8*)u16Data)[0] = SPIRead(); /*!<SPI More Significant Byte*/
}

#pragma INLINE
void SPIReadLSB(UINT16 * u16Data){
  ((UINT8*)u16Data)[1] = SPI1D; /*!<SPI Less Significant Byte*/
//  ((UINT8*)u16Data)[1] = SPIRead(); /*!<SPI Less Significant Byte*/
}


