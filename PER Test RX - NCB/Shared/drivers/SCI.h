/**
 * Copyright (c) 2004, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 * File name :  SCI.h
 * Project name: SMAC (Simple Media Access Controller)
 *
 * Department : Freescale Radio Products Division
 *
 * Description : Header for SCI module.
 *
 * $Author: a20639 $
 * $Date: 2006/08/23 05:17:21 $
 * $Name:  $
 */

/* Constants Initialization*/
#define INIT_SCI2C2    0b00101100
#define BAUD_38400    0x0D    //for busclk=8Mhz (clk rate=16Mhz)



/* SCI functions */
void SCIInit(void);
void SCIStartTransmit(UINT8 cData);
void SCITransmitStr(UINT8 *pStr);
interrupt void Vscirx();
void SCITransmitArray(UINT8 *pStr, UINT8 length);
UINT8 SCIgets(char *pPrompt, char *pBuffer, UINT8 u8Size);
void SCIInitGeneric(UINT32 u32BusClock, UINT32 u32BaudRate, UINT8 u8ComNumber);
void SCITransmitFormattedArray(char *pStr, UINT8 length);
