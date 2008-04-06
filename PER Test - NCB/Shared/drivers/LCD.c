/**
 * Copyright (c) 2004, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 * File name :  LCD.c
 * Project name: SMAC (Simple Media Access Controller)
 *
 * Department : Freescale Radio Products Division
 *
 * Description : LCD functions
 *
 * $Author: rgd04c $
 * $Date: 2006/09/25 14:47:08 $
 * $Name:  $
 */
/* File: LCD.c */



#include "LCD.h"
#include <string.h>
#include "freescale_radio_hardware.h"

#if (LCD_ENABLED)
void LCDFirstLine( void )
{
    /*  Setup the R/W for writing (PTGD), Initialize EN and RS to 0   */

    LCD_RW = 0;     
    LCD_REG_SEL = 0;     
 

    /* DD Ram Address Set - 1st Digit */
    LCDWrite4bits(0x80);
    LCDToggleEN();
    LCDWaitShort(1);
    LCDWrite4bits(0x00);
    LCDToggleEN();
    LCDWaitShort(4);
}

void LCDSecondLine(void)
{
    /* Setup the R/W for writing (PTGD), Initialize EN and RS to 0  */    

    LCD_RW = 0;
    LCD_REG_SEL = 0;     

    /* DD Ram Address Set - 1st Digit */
    LCDWrite4bits(0xC0);
    LCDToggleEN();
    LCDWaitShort(1);
    LCDWrite4bits(0x00);
    LCDToggleEN();
    LCDWaitShort(4);
}

void LCDClearDisplay(void)
{
    /* Setup the R/W for writing (PTGD), Initialize EN and RS to 0 */
    LCD_REG_SEL = 0;
    LCD_RW = 0; 
  
    LCDWrite4bits(0x00);
    LCDToggleEN();
    LCDWaitShort(1);
    LCDWrite4bits(0x10);
    LCDToggleEN();
    LCDWaitLong(3);
}

void LCDWaitLong(UINT16 w)
{
    UINT16 x;
    UINT16 y;
      
    for ( y = 0; y < w; y++ )
    {
        for ( x = 0; x < 0x7FF; x++ );
    }
}

void LCDWaitShort(UINT8 w )
{
    UINT8 x;
    UINT8 y;
      
    for ( y = 0; y < w; y++ )
    {
        for ( x = 0; x < 0xFF; x++ );
    }
}

void LCDToggleEN( void )
{

LCD_EN = 1;
LCD_EN = 0; 

}

void LCDInit(void)
{
    LCDWaitLong(5);
  
  
    LCD_INIT_MACRO;
    
  
    /* Initialize data port */
    
    LCD_DATA1 = 0;
    LCD_DATA2 = 0;
    LCD_DATA3 = 0;
    LCD_DATA4 = 0;// PTBD = PTBD & 0x0F;    

    /* Setup the R/W for writing (PTBD) */
    
    LCD_RW = 0; 

    /* Initialize EN and RS to 0 */
    
    LCD_REG_SEL = 0;
    LCD_EN = 0;
    
    /* Send the reset sequence */
    LCDWrite4bits(0x30);
    LCDToggleEN();
      
    LCDWaitLong(5);
  
    LCDWrite4bits(0x30);
    LCDToggleEN();
  
    LCDWaitShort(2);
  
    LCDWrite4bits(0x30);
    LCDToggleEN();
  
    LCDWaitShort(2);

    LCDWrite4bits(0x20);
    LCDToggleEN();
  
    LCDWaitShort(1);
  
    /* Function, 4 bit data length */
    LCDWrite4bits(0x20);
    LCDToggleEN();
  
    LCDWaitShort(1);
  
    /*  2 lines, 5x7 dot format  */
    LCDWrite4bits(0x80);
    LCDToggleEN();
  
    LCDWaitShort(4);
  
    // Entry Mode Inc, No Shift
    LCDWrite4bits(0x00);
    LCDToggleEN();
    LCDWaitShort(1);
    LCDWrite4bits(0x60);
    LCDToggleEN();
    LCDWaitShort(5);
  
    // Display ON/OFF Control - Display On, Cursor Off, Blink Off
    LCDWrite4bits(0x00);
    LCDToggleEN();
    LCDWaitShort(1);
    LCDWrite4bits(0xC0);
    LCDToggleEN();
    LCDWaitShort(5);
  
    // Display Clear
    LCDClearDisplay();
  
    //LCDFirstLine();
}

void LCDWrite4bits(UINT8 bdata)
{
    if (bdata & 0x80)
    {
        LCD_DATA4 = 1;//PTBD = PTBD | bdata; 
    }
    else
    {
        LCD_DATA4 = 0;//PTBD = PTBD & 0x7F; 
    }
        
    if (bdata & 0x40)
    {
        LCD_DATA3 = 1;//PTBD = PTBD | bdata; 
    }
      else
    {
        LCD_DATA3 = 0;//PTBD = PTBD & 0xBF; 
    }
        
    if (bdata & 0x20)
    {
        LCD_DATA2 = 1;//PTBD = PTBD | bdata; 
    }
    else
    {
        LCD_DATA2 = 0;//PTBD = PTBD & 0xDF; 

    }
        
    if (bdata & 0x10)
    {
        LCD_DATA1 = 1;//PTBD = PTBD | bdata; 
    }
    else
    {
        LCD_DATA1 = 0;//PTBD = PTBD & 0xEF; 
    }
}

void LCDWriteCommand(UINT8 cmd)
{
    
    LCD_RW = 0;
    LCD_REG_SEL = 0;
    LCD_EN = 0; 

    LCDWrite4bits(cmd);  /* cmd bits 4-7 */
  
    LCDToggleEN();

    LCDWrite4bits(cmd << 4);  /* cmd bits 0-3 */
    
    LCDToggleEN();
  
    LCDWaitLong(5);
}


void LCDWriteString(UINT8 line, UINT8 *str)
{
    UINT8 len;
    UINT8 x;
  
    if (line == 2)
        LCDSecondLine();
    else
        LCDFirstLine();
  
    len = (UINT8)strlen(str);

    /* Check boundries  */
    if (len > MAX_LCD_CHARS)
        len = MAX_LCD_CHARS;
      
    for (x = 0; x < len; x++)
    {
        LCDWriteChar(str[x]);
    }
  
    /* Clear the rest of the line */
    for (; x < 16; x++)
    {
        LCDWriteChar(' ');
    }
}

void LCDWriteChar(UINT8 ch)
{
    
    LCD_RW = 0;
    LCD_EN = 0;
    LCD_REG_SEL = 1;
      
    LCDWrite4bits(ch);
    
    LCDToggleEN();
      
    LCDWrite4bits(ch << 4);
      
    LCDToggleEN();
      
    LCDWaitShort(2);
}

#else
void LCDInit(void)
{
  
}

void LCDWrite4bits(UINT8 bdata)
{
  (void) bdata;
}

void LCDFirstLine(void)
{
  
}

void LCDSecondLine(void)
{
  
}

void LCDClearDisplay(void)
{
  
}

void LCDWriteString(UINT8 line, UINT8 *str)
{
  (void) line;
  (void) str[0];
}

void LCDWriteChar(UINT8 ch)
{
  (void) ch;
}
#endif