/*!
\mainpage 


*/
/****************************************************************************************/
/*!
Copyright (c) 2005 Freescale Semiconductor
Freescale Confidential Proprietary
\file GB60_InternalFlash_api.c
\brief Interface file to access the Flash for an OTAP application
\author Freescale Semiconductor
\author $AUTHOR = $
\author RTAC Americas - CWO
\version 0v1
\date $DATE = $
\warning This file shall work only for GB60/GT60 devices

*/

#include "OTAP_Mod_GB60_InternalFlash_api.h"
#include "freescale_radio_hardware.h"

#pragma MESSAGE DISABLE C1805
#pragma MESSAGE DISABLE C4200
#pragma MESSAGE DISABLE C20000

//volatile const UINT8 NVOPT @0xFFBF = 0x02;

/* Static variables */
static UINT16 su16CurrAddr = F_START_ADDR;
static UINT16 su16CurrSize = 0;
extern UINT8 temp=0;
//Array of opcode instructions of the Erase/Program function in the HCS08 family
volatile unsigned char PGMOtapAcc[59] = {  
0x87,0xC6,0x18,0x25,0xA5,0x10,0x27,0x08,0xC6,0x18,0x25,0xAA,0x10,0xC7,0x18,0x25,
0x9E,0xE6,0x01,0xF7,0xA6,0x20,0xC7,0x18,0x26,0x45,0x18,0x25,0xF6,0xAA,0x80,0xF7,
0x9D,0x9D,0x9D,0x9D,0x45,0x18,0x25,0xF6,0xF7,0xF6,0xA5,0x30,0x27,0x04,0xA6,0xFF,
0x20,0x07,0xC6,0x18,0x25,0xA5,0x40,0x27,0xF9,0x8A,0x81};

UINT8 u8FData @F_START_ADDR;

#define OTAPPage_Erase   OTAPPGM[21]=0x40; temp = ((unsigned char(*)(unsigned int))(OTAPPGM))
#define OTAPProgram_Byte OTAPPGM[21]=0x20; temp = ((unsigned char(*)(unsigned int, unsigned char))(OTAPPGM))

//Array of opcode instructions of the Erase/Program function in the HCS08 family
volatile unsigned char OTAPPGM[59] = {  
0x87,0xC6,0x18,0x25,0xA5,0x10,0x27,0x08,0xC6,0x18,0x25,0xAA,0x10,0xC7,0x18,0x25,
0x9E,0xE6,0x01,0xF7,0xA6,0x20,0xC7,0x18,0x26,0x45,0x18,0x25,0xF6,0xAA,0x80,0xF7,
0x9D,0x9D,0x9D,0x9D,0x45,0x18,0x25,0xF6,0xF7,0xF6,0xA5,0x30,0x27,0x04,0xA6,0xFF,
0x20,0x07,0xC6,0x18,0x25,0xA5,0x40,0x27,0xF9,0x8A,0x81};
/*  The opcode above represents this set of instructions  
    if (FSTAT&0x10){                     //Check to see if FACCERR is set
        FSTAT = FSTAT | 0x10;            //write a 1 to FACCERR to clear
    }
    (*((volatile unsigned char *)(Address))) = data;  //write to somewhere in flash
    FSTAT = 0x80;                        //Put FCBEF at 1.
    _asm NOP;                            //Wait 4 cycles
    _asm NOP;
    _asm NOP;
    _asm NOP;
    if (FSTAT&0x30){                     //check to see if FACCERR or FVIOL are set
    return 0xFF;                         //if so, error.
    }
    while ((FSTAT&0x40)==0){             //else wait for command to complete
        ;
    }*/



UINT8 FInit(UINT16 u8BusClk)
{
    
    switch (u8BusClk){
    
      case F8MHZ:
          FCDIV = 39;      
      break;
      
      case F4MHZ:
          FCDIV = 20;    
      break;
      
      default:
          FCDIV = 39; 
      break;
    }

    FSTAT = 0x80;   //Put FCBEF at 1.

    su16CurrAddr = F_START_ADDR;
    su16CurrSize = 0;
    
    CopyInRAM();
    return F_OK;
 }
                                    
UINT8 FWriteByte(UINT8 u8Data)
{
    
    /* Validation */
    if (su16CurrSize <= (su16CurrAddr - F_START_ADDR))
    {
        return F_EOF;
    }
    temp = Flash_Program(su16CurrAddr, u8Data);
    if (temp == 0x00)  /* If correctly programmed word */
    {
        su16CurrAddr++;
        return F_OK;
    } else 
    {
        return F_ERR;
    }
    
}

UINT8 FWriteName(UINT8 u8Data){
static UINT16 su16NameAddr = F_NAME_ADDR;

    temp = OTAPProgram_Byte(su16NameAddr, u8Data);
    if (temp != 0xff)  /* If correctly programmed word */
    {
        su16NameAddr++;
        return F_OK;
    } else 
    {
        return F_ERR;
    }

}


UINT8 FReadWord(UINT16 *pu16Word)
{
    if (su16CurrSize <= (su16CurrAddr + 1 - F_START_ADDR))
    {
        return F_EOF;
    }
    *pu16Word = *((UINT16 *) su16CurrAddr);
    su16CurrAddr += 2;
    return F_OK;
}


UINT8 FSetSize(UINT16 u16Size)
{
    if (u16Size >= F_MAX_SIZE)
    {
        return F_ERR;
    }
    su16CurrSize = (u16Size) + (u16Size & 0x0001);
    return F_OK;   
}

UINT8 FGetSize(UINT16 *pu16Size)
{
    *pu16Size = su16CurrSize;
    return F_OK;
}
UINT8 FEraseImage(void)
{
    UINT16 tmpAddr = F_START_ADDR;
    if (su16CurrSize > 0)
    {
        while ((tmpAddr - F_START_ADDR) <= su16CurrSize)
        {
         temp = Flash_Erase(tmpAddr); /* Erase current page */
            tmpAddr += 0x0200; /* Add a page size */
        }
    }
    return F_OK;
}


/*#pragma CODE_SEG MYFLASH
void FEraseApp(void)
{    

    volatile UINT16 tmpAddr = 0x9200;
    volatile UINT8 *u8ImageData ;
    u8ImageData = &u8FData;
      //Erases the Application

        while ((tmpAddr - 0x9200) < 0x6E00)
        {
         OTAPPage_Erase(tmpAddr); // Erase current page 
            tmpAddr += 0x0200; // Add a page size
        }

      //Switch Context
          _asm jmp 0x2200;
                  
            while ((tmpAddr - 0x2200) < 0x8E00)
        {
         OTAPProgram_Byte(tmpAddr,*u8ImageData); // Erase current page
            tmpAddr += 1; // Add a page size
        }
        
 //Reset board
_asm JMP 0xFFFE

}     

#pragma CODE_SEG DEFAULT */
void FLASHMoveImage(void) 
{

UINT16 curradr = F_DEST_ADDR;

UINT8 i;

UINT8 *data ;
data = &u8FData;

temp = OTAPPage_Erase(0x3200);
if(temp==0xFF){
asm nop;
} 
                  
for (i=0; i< 255; i++){
temp = OTAPProgram_Byte(0x3200 + i, i);
if(temp==0xFF){
asm nop;
}
}

temp = OTAPPage_Erase(0x3400);
if(temp==0xFF){
asm nop;
} 

while(curradr < 0x3600){

temp = OTAPProgram_Byte(curradr,*data );
if(temp==0xFF){
asm nop;
}
data++;
curradr++;
}

}

#define __SEG_START_REF(a)  __SEG_START_ ## a
#define __SEG_END_REF(a)    __SEG_END_   ## a
#define __SEG_SIZE_REF(a)   __SEG_SIZE_  ## a
#define __SEG_START_DEF(a)  extern char __SEG_START_REF(a) []
#define __SEG_END_DEF(a)    extern char __SEG_END_REF(  a) []
#define __SEG_SIZE_DEF(a)   extern char __SEG_SIZE_REF( a) []

__SEG_START_DEF(FLASH_TO_RAM);
__SEG_SIZE_DEF(FLASH_TO_RAM);
#define Start_Copy_In_RAM __SEG_START_REF(FLASH_TO_RAM)
#define Size_Copy_In_RAM  __SEG_SIZE_REF(FLASH_TO_RAM)
  
void CopyInRAM(void) 
{
  char *srcPtr, *dstPtr;
  int count;
  srcPtr = (char *)Start_Copy_In_RAM;
  dstPtr = (char *)&FContextChange;
  for (count = 0; count < (int) Size_Copy_In_RAM;  count++, dstPtr++, srcPtr++) 
  {
    *dstPtr = *srcPtr;
  }
}




UINT8 FImageValidation(void)
{
  UINT8 u8ByteCount = 0;
  UINT16 u16Addr = 0x0000;
  UINT8 *pu8Data = (UINT8 *)F_START_ADDR;
  UINT16 u16Index = 0;  
  while (u16Index <= su16CurrSize)
  {
    /*Find addr*/
    u16Addr  = ((UINT16)pu8Data[u16Index])<<8;
    u16Addr |= (UINT16)pu8Data[++u16Index];
    u8ByteCount = pu8Data[++u16Index];
    u16Index += u8ByteCount;
    if (u16Index > su16CurrSize)
      break;
    if(pu8Data[++u16Index] != 0x55)
    {
      return FALSE;
    }
    u16Index++;
  }
  return TRUE;
}







#pragma CODE_SEG FLASH_TO_RAM
void FContextChange()
{
  
  UINT8 u8ByteCount = 0;
  UINT16 u16Addr = 0x0000;
  UINT8 *pu8srcData = (UINT8 *)F_START_ADDR;
  UINT8 *pu8dstData = (UINT8 *) 0;
  UINT16 u16Index = 0;
  UINT8 i=0;  
  /* Let's first erase all the pages to which the image references */
  while (u16Index <= su16CurrSize)
  {
    /*Find addr*/
    u16Addr  = (UINT16)pu8srcData[u16Index]<<8;
    u16Addr |= (UINT16)pu8srcData[++u16Index];
    pu8dstData = (UINT8 *) u16Addr;
    u8ByteCount = pu8srcData[++u16Index];
    if (u8ByteCount == 0xFF)
      break;  // End of file reached
    for (i=0;i<u8ByteCount;i++)
    {
      if (*(pu8dstData) != 0xFF)
      {
        (void)Flash_Erase(pu8dstData);
        break;
      }
      pu8dstData++;
    }
    pu8dstData = (UINT8 *) u16Addr;
    
    
    u16Index += u8ByteCount;
    u16Index++;
    u16Index++;
  }
  u16Index = 0;
  i=0;  
  
  /* Now let's program the addresses referenced by the image */
  while (u16Index <= su16CurrSize)
  {
    /*Find addr*/
    u16Addr  = (UINT16)pu8srcData[u16Index]<<8;
    u16Addr |= (UINT16)pu8srcData[++u16Index];
    pu8dstData = (UINT8 *) u16Addr;
    u8ByteCount = pu8srcData[++u16Index];
    if (u8ByteCount == 0xFF)
      break;  // End of file reached
    pu8dstData = (UINT8 *) u16Addr;
    
    for (i=0;i<u8ByteCount;i++, pu8dstData++)
    {
      (void)Flash_Program(pu8dstData, pu8srcData[++u16Index]);
    }
    //Flash_Burst(pu8dstData, u8ByteCount, &pu8srcData[++u16Index]);
    
    //u16Index += u8ByteCount;
    u16Index++;
    u16Index++;
  }

  __asm SWI;
}

UINT8 Flash_Cmd(unsigned int FlashAddress, unsigned int FlashDataCounter, unsigned char* pFlashDataPtr, unsigned char FlashCommand)
{
    /* Check to see if FACCERR or PVIOL is set */
    if (FSTAT&0x30)  
    {         
        /* Clear Flags if set*/
        FSTAT = FSTAT | 0x30;  
    }

    if (FlashDataCounter)
    {
      do
      {
          /* Wait for the Last Busrt Command to complete */
          while(!(FSTAT&0x80));
          
          /* Write Data into Flash*/
          (*((volatile unsigned char *)(FlashAddress++))) = *pFlashDataPtr;
          pFlashDataPtr++;
          /* Write Command */
          FCMD = FlashCommand;
          _asm NOP;
          _asm NOP;
          _asm NOP;
          _asm NOP;
          
          /* Put FCBEF at 1 */
          FSTAT = 0x80;
          /* Check if Flash Access Error or Protection Violation Error are Set */
          if (FSTAT&0x30)
          {     
            /* If so, finish the function returning 1 to indicate error */
            return (1);
          }
      }while (--FlashDataCounter);
    }
    /* wait for the last command to complete */
    while ((FSTAT&0x40)==0);
    /* Return zero to indicate that the function executed Ok */
    return (0);
}



        
#pragma CODE_SEG ROM_DEFAULT