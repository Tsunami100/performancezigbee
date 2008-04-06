/************* Includes ************/
#include "pub_def.h"

/************* Defines ************/
/* Constants */
#define Page_EraseAcc   PGMOtapAcc[21]=0x40; temp = ((unsigned char(*)(unsigned int))(PGMOtapAcc))
#define Program_ByteAcc PGMOtapAcc[21]=0x20; temp = ((unsigned char(*)(unsigned int, unsigned char))(PGMOtapAcc))

//Array of opcode instructions of the Erase/Program function in the HCS08 family
/*volatile unsigned char PGMOtapAcc[59] = {  
0x87,0xC6,0x18,0x25,0xA5,0x10,0x27,0x08,0xC6,0x18,0x25,0xAA,0x10,0xC7,0x18,0x25,
0x9E,0xE6,0x01,0xF7,0xA6,0x20,0xC7,0x18,0x26,0x45,0x18,0x25,0xF6,0xAA,0x80,0xF7,
0x9D,0x9D,0x9D,0x9D,0x45,0x18,0x25,0xF6,0xF7,0xF6,0xA5,0x30,0x27,0x04,0xA6,0xFF,
0x20,0x07,0xC6,0x18,0x25,0xA5,0x40,0x27,0xF9,0x8A,0x81}; */
#define F_MAX_SIZE (28160) /* Maximum size for image read/write (55 pages) */
#define F_START_ADDR (0x2200) /* Start Address */
#define F_DEST_ADDR (0x9200)
#define F_NAME_ADDR (0x2000) /* Locals page */
#define F8MHZ      8000000 
#define F4MHZ      4000000 

/* Return types */
#define F_OK (1)
#define F_ERR (-1)
#define F_NOT_IMPLEMENTED (2)
#define F_EOF (3)


/*********** Prototypes ***********/
UINT8 FInit(UINT16 u8BusClk);
UINT8 FWriteByte(UINT8 u8Data);
UINT8 FReadByte(UINT16 *pu8Data);
UINT8 FSetSize(UINT16 u16Size);
UINT8 FGetSize(UINT16 *pu16Size);
UINT8 FEraseImage(void);

UINT8 FWriteName(UINT8 u8Data);
  #define FLASH_MASS_ERASE_CMD  0x41
  #define FLASH_ERASE_CMD       0x40
  #define FLASH_PROGRAM_CMD     0x20
  #define FLASH_BURST_CMD       0x25

UINT8 FImageValidation(void);
void CopyInRAM(void); 
  #define Flash_Erase(Address) \
          Flash_Cmd((unsigned int)Address, (unsigned int)1, (unsigned char*) 0, FLASH_ERASE_CMD)
  
  #define Flash_Program(Address, Data) \
          Flash_Cmd((unsigned int)Address, (unsigned int)1, (unsigned char*)&Data, FLASH_PROGRAM_CMD)
  
  #define Flash_Burst(Address, Size, DataPtr) \
          Flash_Cmd((unsigned int)Address, (unsigned int)Size, (unsigned char*)DataPtr, FLASH_BURST_CMD)
          
#pragma CODE_SEG FLASH_TO_RAM
void FContextChange(void);
UINT8 Flash_Cmd(unsigned int FlashAddress, unsigned int FlashDataCounter, unsigned char* pFlashDataPtr, unsigned char FlashCommand);
#pragma CODE_SEG DEFAULT
