/**
 * Copyright (c) 2004, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 * File name :  SCI.c
 * Project name: SMAC (Simple Media Access Controller)
 *
 * Department : Freescale Radio Products Division
 *
 * Description : SCI module functions.
 *
 * $Author: a20639 $
 * $Date: 2006/08/22 23:44:58 $
 * $Name:  $
 */

#include <string.h>
#include "derivative.h"
#include "freescale_radio_hardware.h"
#include "SCI.h"


UINT8 gu8SCIData[128] ;
UINT8 gu8SCIDataFlag; 


void SCIInit(void)
{
    SCIBDH = 0x00;
    SCIBDL = BAUD_38400;
    SCIC2 = INIT_SCI2C2;
}
/************************************************************
Function: SCIInitGeneric
arg types (u16, u16, u8)

Function initializing the SCI to the requested baud rate
based on the supplied busclock in kilohertz on the com port
that is selected

************************************************************/



void SCIInitGeneric(UINT32 u32BusClock, UINT32 u32BaudRate, UINT8 u8ComNumber) {
   
   UINT16 u16Sbr = 0;   
   u16Sbr = (UINT16) ((u32BusClock) / (u32BaudRate * 16));
   
   switch (u8ComNumber) {
      case 1:
         SCI1BDL = (UINT8) u16Sbr;
         u16Sbr = u16Sbr >> 8;
         SCI1BDH = (UINT8) u16Sbr;
         SCI1C2 = INIT_SCI2C2;   
         break;
      
      case 2:
         SCI2BDL = (UINT8) u16Sbr;
         u16Sbr = u16Sbr >> 8;
         SCI2BDH = (UINT8) u16Sbr;
         SCI2C2 = INIT_SCI2C2;
         break;
         
      default:
         SCI1BDL = (UINT8) u16Sbr;
         u16Sbr = u16Sbr >> 8;
         SCI1BDH = (UINT8) u16Sbr;
         SCI1C2 = INIT_SCI2C2;   
         break;
   }   
}

interrupt void Vscirx()
{    
    UINT8 u8status, u8dummy;
    /* ensure Rx data buffer is full */
    do 
    {
        u8status = SCIS1;
    } while ((u8status & 0x20) == 0);
    
    /* Check for Errors (Framing, Noise, Parity) */
    if ((u8status & 0x07) != 0) 
    {
        u8dummy = SCID;
        return;
    }

    /* Good Data. */
    gu8SCIData[0] = SCID; /* load SCI register to data */
    gu8SCIData[1] = 0; 
    gu8SCIDataFlag = 1;
}

void SCITransmitArray(UINT8 *pStr, UINT8 length)
{
    UINT8 i;
    for (i=0; i<length; i++)
    {
        SCIStartTransmit(pStr[i]);
    }
}

void SCIStartTransmit(UINT8 cData)
{
    while (!SCIS1_TDRE);  /* ensure Tx data buffer empty */
    SCID = cData; /* load data to SCI2 register */
    while (!SCIS1_TC); /* wait for Tx complete */
}

void SCITransmitStr(UINT8 *pStr)
{
    UINT16 i;
    UINT16 nStrLen=strlen(pStr);
    
    for (i=0; i<nStrLen; i++)
    {
        SCIStartTransmit(pStr[i]);
    }
}

UINT8 SCIgets(char *pPrompt, char *pBuffer, UINT8 u8Size) {

   
   UINT8 u8ExitFlag = FALSE;
   UINT8 u8Offset=0;
   
   //Echo the string for the command line
   SCITransmitStr(pPrompt);
   
   //Keep processing SCI inputs until Size is reached or terminating NULL
   //occurs.
   while (u8ExitFlag == FALSE) {
      if (gu8SCIDataFlag == 1) {
         gu8SCIDataFlag = 0;
      
         
         //Check the terminating string
         switch (gu8SCIData[0]) {
         
            //Check for Terminating string
            case 0x0D:
               u8ExitFlag = TRUE;
               pBuffer[u8Offset] = 0; //Terminate sting with null.
               break;
               
            case '\b':
               if (u8Offset != 0)
                  pBuffer[u8Offset--] = 0;
               else
                  pBuffer[u8Offset] = 0;
               break;
            
            case '\n':
            default:
               pBuffer[u8Offset++] = gu8SCIData[0];
               break;
         }
         
         
         
         
         //Check the buffersize not to overrun buffer.
         
         
         if (u8Offset == u8Size) {
            u8ExitFlag = TRUE;
         }
      }
   }
   
   return u8Offset;
   
}

void SCITransmitFormattedArray(char *pStr, UINT8 length)
{
	int i;
	UINT8 low_nibble=0, hi_nibble=0;
	UINT8 fourbit=0;
	
	for (i=0; i<length; i++) {
	
	   low_nibble = pStr[i] & 0x0F;
	   hi_nibble = (pStr[i] & 0xF0) >> 4;
	   
	   if (hi_nibble <= 9) {
	      SCIStartTransmit(hi_nibble + 0x30);
	   } else {
	      SCIStartTransmit(hi_nibble + 0x37);
	   }
	   
	   if (low_nibble <= 9) {
	      SCIStartTransmit(low_nibble + 0x30);
	   } else {
	      SCIStartTransmit(low_nibble + 0x37);
	   }
	}
}
