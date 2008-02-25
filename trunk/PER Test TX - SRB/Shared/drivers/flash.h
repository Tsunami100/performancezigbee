/* flash.h
 * Header for the flash programming and erasing routines.
 * This file should be included only in one file: the main file.
 *
 * $Author: a20639 $
 * $Date: 2006/08/22 23:44:58 $
 */
 
#define Page_Erase   PGMAcc[21]=0x40; temp = ((unsigned char(*)(unsigned int))(PGMAcc))
#define Program_Byte PGMAcc[21]=0x20; temp = ((unsigned char(*)(unsigned int, unsigned char))(PGMAcc))

//extern volatile unsigned char *PGMAcc;
//Array of opcode instructions of the Erase/Program function in the HCS08 family
volatile unsigned char PGMAcc[59] = {  
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
