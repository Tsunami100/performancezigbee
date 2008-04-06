/************************************************************************************
* The global header file containing platform specific definitions for
* the embedded platform
*
* Author(s): Thomas O. Jensen
*
* (c) Copyright 2004, Freescale, Inc.  All rights reserved.
*
* Freescale Confidential Proprietary
* Digianswer Confidential
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
* Last Inspected:
* Last Tested:
************************************************************************************/

#ifndef _TARGET_SPECIFIC_H_
#define _TARGET_SPECIFIC_H_

//LR #include "gb60_io.h"
#include "FunctionalityDefines.h"

//**********************************************************************************
// Compiler warning removal pragmas
//**********************************************************************************

#define PLACE_CONST_SEG(segment) CONST_SEG segment
#define RESTORE_CONST_SEG CONST_SEG DEFAULT

#define PLACE_DATA_SEG(segment) DATA_SEG segment
#define RESTORE_DATA_SEG DATA_SEG DEFAULT

#define INLINE_NEXT_FUNCTION INLINE

#define Unreferenced_c            C5703
#define ConditionAlwaysTrue_c     C4000
#define ConditionAlwaysFalse_c    C4001
#define AssignmentInCondition_c   C5909
#define UnusedResultOfFunction_c  C1420
#define NonStandardConversion_c   C1805
#define InlineFunctionCall_c      C4301
#define RemovedDeadCode_c         C5660
#define ResultNotUsed_c           C4002
#define RecursiveFunctionCall_c   C1855

#define DISABLE_WARNING(msg) MESSAGE DISABLE msg
#define RESTORE_WARNING(msg) MESSAGE DEFAULT msg


//**********************************************************************************
// Define target Hardware platform:
//**********************************************************************************

#if !(defined(TARGET_DIG527_1) || defined(TARGET_DIG528_1) || defined(TARGET_DIG536_1) || defined(TARGET_DIG536_2) || defined(TARGET_DIG534_1) || defined(TARGET_DIG538_1) || defined(TARGET_AXIOM_GB60) )
// Enable target platform
//#define TARGET_DIG527_1
//#define TARGET_DIG528_1
#define TARGET_DIG528_2
//#define TARGET_DIG536_1 // SARD
//#define TARGET_DIG536_2 // SARD
//#define TARGET_DIG534_1
//#define TARGET_DIG538_1 // with module DIG537
//#define TARGET_AXIOM_GB60
#endif


//**********************************************************************************

#ifdef TARGET_AXIOM_GB60
  #define HWAssertAbelReset        PTDD &= ~0x80;    // Reset = 0;
  #define HWDeAssertAbelReset      PTDD |=  0x80;    // Reset = 1;

  #define RxTxDisable              PTDD &= ~0x40;    // RxTxEnable = 0;
  #define RxTxEnable               PTDD |=  0x40;    // RxTxEnable = 1;

  #define AttEnable                PTDD |=  0x20;    // PTD5
  #define AttDisable               PTDD &= ~0x20;

  #define LED1ON  PTFD &= ~0x01;                      // PTF0
  #define LED1OFF PTFD |= 0x01;
  #define LED1TOGGLE PTFD ^= 0x01;

  #define LED2ON  PTFD &= ~0x02;                      // PTF1
  #define LED2OFF PTFD |= 0x02;
  #define LED2TOGGLE PTFD ^= 0x02;

  #define LED3ON  PTFD &= ~0x04;                      // PTF2
  #define LED3OFF PTFD |= 0x04;
  #define LED3TOGGLE PTFD ^= 0x04;

  #define LED4ON  PTFD &= ~0x08;                      // PTF3
  #define LED4OFF PTFD |= 0x08;
  #define LED4TOGGLE PTFD ^= 0x08;

#define IsAbelActive()   (PTBD & (1<<4))
#ifdef I_AM_A_SNIFFER
  #define IsAbelCrcOk()    (0)
#else
  #define IsAbelCrcOk()    (PTBD & (1<<5))
#endif
#define IsAbelCcaBusy()  (PTBD & (1<<5))

#ifdef ANTENNE_SWITCH_ENABLED
  #define ENABLE_ANTENNESWITCH_PINS PTBDD |= 0xC0;
  #define RX_ANTENNE_ENABLED PTBD |= 0x40; PTBD &= ~0x80;
  #define TX_ANTENNE_ENABLED PTBD |= 0x80; PTBD &= ~0x40;
#else
  #define ENABLE_ANTENNESWITCH_PINS
  #define RX_ANTENNE_ENABLED
  #define TX_ANTENNE_ENABLED
#endif

// UART interface select SCI1
#define SELECT_SCI1

  // Setup Port B for ABEL GPIO 1+2
#define  ABEL_GPIO1    (1<<4)
#define  ABEL_GPIO2    (1<<5)

#define  mSETUP_PORT_A  PTAPE = 0x00;\
                        PTASE = 0x00;\
                        PTADD = 0x00;        //All Port A input

#define  mSETUP_PORT_B  PTBDD &= ~(ABEL_GPIO1 | ABEL_GPIO2); // Clear GPIO1 and GPIO2

#define  mSETUP_PORT_C    PTCD = 0x00;\
                        PTCPE = 0x00;\
                        PTCDD = 0x63;

#define  mSETUP_PORT_D  PTDPE = 0x00;\
                        PTDSE = 0x00;\
                        PTDDD = 0x01 | 0x20 | 0x40 | 0x80;


#endif TARGET_AXIOM_GB60

//**********************************************************************************

#ifdef TARGET_DIG527_1
  #define HWAssertAbelReset        PTDD &= ~0x80;    // Reset = 0;
  #define HWDeAssertAbelReset      PTDD |=  0x80;    // Reset = 1;

  #define RxTxDisable              PTDD &= ~0x40;    // RxTxEnable = 0;
  #define RxTxEnable               PTDD |=  0x40;    // RxTxEnable = 1;

 //  #define LED1ON  PTDD &= 0xFE;
//  #define LED1OFF PTDD |= 0x01;
//  #define LED1TOGGLE PTDD ^= 0x01;

  #define LED1ON  PTFD &= 0xFE;
  #define LED1OFF PTFD |= 0x01;
  #define LED1TOGGLE PTFD ^= 0x01;

  #define LED2ON  PTDD &= ~0x08;
  #define LED2OFF PTDD |= 0x08;
  #define LED2TOGGLE PTDD ^= 0x08;

  #define LED3ON  
  #define LED3OFF 
  #define LED3TOGGLE 

  #define LED4ON  
  #define LED4OFF 
  #define LED4TOGGLE 
  
      // UART interface select SCI1
    #define SELECT_SCI1
//    #define SELECT_SCI2

      // Setup Port B for ABEL GPIO 1+2
    #define  ABEL_GPIO1    (1<<4)
    #define  ABEL_GPIO2    (1<<5)
  
    #define ANTENNE_SWITCH_ENABLED
    #ifdef ANTENNE_SWITCH_ENABLED
      #define ENABLE_ANTENNESWITCH_PINS PTBDD |= 0xC0;
      #define RX_ANTENNE_ENABLED PTBD |= 0x40; PTBD &= ~0x80;
      #define TX_ANTENNE_ENABLED PTBD |= 0x80; PTBD &= ~0x40;
    #else
      #define ENABLE_ANTENNESWITCH_PINS
      #define RX_ANTENNE_ENABLED
      #define TX_ANTENNE_ENABLED
    #endif

      
    #define IsAbelActive()   (PTBD & (1<<4))
    #ifdef I_AM_A_SNIFFER
      #define IsAbelCrcOk()    (0)
    #else
      #define IsAbelCrcOk()    (PTBD & (1<<5))
    #endif
    #define IsAbelCcaBusy()  (PTBD & (1<<5))
    
    #define USE_LOW_POWER
#define mENTER_LOWPOWER  SIMOPT |= 0x20;PMCSC2 &=  ~0x02;__asm STOP;

#ifdef FLASH_LIB_ENABLED
  #define USE_NVM_QUICK_START
#endif FLASH_LIB_ENABLED                 

  
#endif

/*

#ifdef TARGET_DIG527_1
  #define HWAssertAbelReset        PTDD &= ~0x80;    // Reset = 0;
  #define HWDeAssertAbelReset      PTDD |=  0x80;    // Reset = 1;

  #define RxTxDisable              PTDD &= ~0x40;    // RxTxEnable = 0;
  #define RxTxEnable               PTDD |=  0x40;    // RxTxEnable = 1;

  #define LED1ON  PTDD &= 0xFE;
  #define LED1OFF PTDD |= 0x01;
  #define LED1TOGGLE PTDD ^= 0x01;

  #define LED2ON  PTDD &= 0xFD;
  #define LED2OFF PTDD |= 0x02;
  #define LED2TOGGLE PTDD ^= 0x02;

  #define LED3ON  PTDD &= 0xF7;
  #define LED3OFF PTDD |= 0x08;
  #define LED3TOGGLE PTDD ^= 0x08;

  #define LED4ON  PTDD &= 0xEF;
  #define LED4OFF PTDD |= 0x10;
  #define LED4TOGGLE PTDD ^= 0x10;
#endif TARGET_DIG527_1

*/


//**********************************************************************************

#ifdef TARGET_DIG528_1
  #define HWAssertAbelReset        PTBD &= ~0x02;    // Reset = 0;
  #define HWDeAssertAbelReset      PTBD |=  0x02;    // Reset = 1;

  #define RxTxEnable               PTBD |=  0x08;    // RxTxEnable = 1 (PTB3);
  #define RxTxDisable              PTBD &= ~0x08;    // RxTxEnable = 0 (PTB3);

  #define AttEnable                PTBD |=  0x04;    // PTB2
  #define AttDisable               PTBD &= ~0x04;

  #define AntSwRx                  PTBD &= ~0x40;    // PTB6
  #define AntSwTx                  PTBD |=  0x40;

  #define LED1ON  PTDD &= 0xFE;
  #define LED1OFF PTDD |= 0x01;
  #define LED1TOGGLE PTDD ^= 0x01;

  #define LED2ON  PTDD &= 0xFD;
  #define LED2OFF PTDD |= 0x02;
  #define LED2TOGGLE PTDD ^= 0x02;

  #define LED3ON  PTDD &= 0xF7;
  #define LED3OFF PTDD |= 0x08;
  #define LED3TOGGLE PTDD ^= 0x08;

  #define LED4ON  PTDD &= 0xEF;
  #define LED4OFF PTDD |= 0x10;
  #define LED4TOGGLE PTDD ^= 0x10;

  #define cPTADD_PortAIOsetup 0x00;

#define IsAbelActive()   (PTBD & (1<<4))
#ifdef I_AM_A_SNIFFER
  #define IsAbelCrcOk()    (0)
#else
  #define IsAbelCrcOk()    (PTBD & (1<<5))
#endif
#define IsAbelCcaBusy()  (PTBD & (1<<5))

#define ANTENNE_SWITCH_ENABLED

#ifdef ANTENNE_SWITCH_ENABLED
  #define ENABLE_ANTENNESWITCH_PINS PTBDD |= 0xC0;
  #define RX_ANTENNE_ENABLED PTBD &= ~0x40;     // PTB6 = 0
  #define TX_ANTENNE_ENABLED PTBD |= 0x40;      // PTB6 = 1
#else
  #define ENABLE_ANTENNESWITCH_PINS
  #define RX_ANTENNE_ENABLED
  #define TX_ANTENNE_ENABLED
#endif

// UART interface select SCI1 or SCI2- enable one
#define SELECT_SCI1
#define SELECT_SCI2

  // Setup Port B for ABEL GPIO 1+2
#define ABEL_GPIO1    (1<<4)
#define ABEL_GPIO2    (1<<5)

#define ABEL_RESET_PIN        (1<<1)
#define ABEL_ATT_PIN        (1<<2)
#define ABEL_RxTx_PIN       (1<<3)
#define ABEL_ANT_SWITCH_PIN (1<<6)

// The switches
#define mSWITCH1_MASK 0x10
#define mSWITCH2_MASK 0x20
#define mSWITCH3_MASK 0x40
#define mSWITCH4_MASK 0x80
#define mSWITCH_MASK 0xF0
#define mSWITCH_PORT_GET ((PTAD & mSWITCH_MASK) ^ mSWITCH_MASK)

#define  mSETUP_PORT_A  PTAPE = 0xF0;\
                        PTADD = 0x00;        //All Port A input

#define  mSETUP_PORT_B  PTBDD = (ABEL_RESET_PIN | ABEL_ATT_PIN | ABEL_RxTx_PIN | ABEL_ANT_SWITCH_PIN);

#define  mSETUP_PORT_C    PTCD = 0x00;\
                        PTCPE = 0x00;\
                        PTCDD = 0x63;

#define  mSETUP_PORT_D  PTDPE = 0x00;\
                        PTDDD = (0x01 | 0x02| 0x08 | 0x10);


#define USE_LOW_POWER
#define mENTER_LOWPOWER  SIMOPT |= 0x20;PMCSC2 &=  ~0x02;__asm STOP;

#if defined FLASH_LIB_ENABLED || defined BOOTLOADER_ENABLED
  #define USE_NVM_QUICK_START
#endif defined FLASH_LIB_ENABLED || defined BOOTLOADER_ENABLED                


#endif TARGET_DIG528_1

//**********************************************************************************

#ifdef TARGET_DIG534_1

  #define FTDI_USB

  #define HWAssertAbelReset        PTBDD |=  0x02; PTBD &= ~0x02;    // Reset = 0;
  #define HWDeAssertAbelReset      PTBDD |=  0x02; PTBD |=  0x02;    // Reset = 1;
//  #define HWAssertAbelReset        PTBD &= ~0x02;    // Reset = 0;
//  #define HWDeAssertAbelReset      PTBD |=  0x02;    // Reset = 1;

  #define RxTxEnable               PTBD |=  0x08;    // RxTxEnable = 1 (PTB3);
  #define RxTxDisable              PTBD &= ~0x08;    // RxTxEnable = 0 (PTB3);

  #define AttEnable                PTBD |=  0x04;    // PTB2
  #define AttDisable               PTBD &= ~0x04;

  #define AntSwRx                  PTBD &= ~0x40;    // PTB6
  #define AntSwTx                  PTBD |=  0x40;

   #define cPTADD_PortAIOsetup 0x00;

  #define GREEN_LED_OFF PTDD |= 0x08;
  #define GREEN_LED_ON  PTDD &= ~0x08;
  #define GREEN_LED_TOGGLE PTDD ^= 0x08;

  #define RED_LED_OFF PTDD |= 0x10;
  #define RED_LED_ON  PTDD &= ~0x10;
  #define RED_LED_TOGGLE PTDD ^= 0x10;


#ifdef SYSTEM_CLOCK_16MHZ
#define CompareHighValue 0x09;
#define CompareLowValue 0x00;
#endif

#ifdef SYSTEM_CLOCK_8MHZ
#define CompareHighValue 0x04;
#define CompareLowValue 0x80;
#endif


#define  mRestartLockTimer  TPM1C0SC &= 0x3f; \
                            TPM1C0VH = CompareHighValue; \
                            TPM1C0VL = CompareLowValue; \
                            TPM1CNTL = 0; \
                            TPM1C0SC = 0x50;

#define IsAbelActive()   (PTBD & (1<<4))
#define IsAbelCrcOk()    (PTBD & (1<<5))
//#define IsAbelCrcOk()    (0)
#define IsAbelCcaBusy()  (PTBD & (1<<5))

#define ANTENNE_SWITCH_ENABLED

#ifdef ANTENNE_SWITCH_ENABLED
  #define ENABLE_ANTENNESWITCH_PINS PTBDD |= 0x40;
  #define RX_ANTENNE_ENABLED PTBD &= ~0x40;     // PTB6 = 0
  #define TX_ANTENNE_ENABLED PTBD |= 0x40;      // PTB6 = 1
#else
  #define ENABLE_ANTENNESWITCH_PINS
  #define RX_ANTENNE_ENABLED
  #define TX_ANTENNE_ENABLED
#endif

// UART interface select SCI1 or SCI2- enable one
#define SELECT_SCI1
//#define SELECT_SCI2

  // Setup Port B for ABEL GPIO 1+2
#define ABEL_GPIO1    (1<<4)
#define ABEL_GPIO2    (1<<5)

#define ABEL_RESET_PIN        (1<<1)
#define ABEL_ATT_PIN        (1<<2)
#define ABEL_RxTx_PIN       (1<<3)
#define ABEL_ANT_SWITCH_PIN (1<<6)


#define  mSETUP_PORT_A  PTAPE = 0x00;\
                        PTASE = 0x00;\
                        PTADD = 0x00;        //All Port A input

#define  mSETUP_PORT_B  PTBDD = (ABEL_RESET_PIN | ABEL_ATT_PIN | ABEL_RxTx_PIN | ABEL_ANT_SWITCH_PIN);
  // Setup port B
  //PTBDD = 0x08; //Bit 3 as ouput. Bit 3 is RxTxEnable. Obs bit 1 is Reset, but is controlled within HWAssertAbelReset/HWDeAssertAbelReset
  // Setup Port B for ABEL GPIO 1+2
  //  PTBDD &= ~((1<<4) | (1<<5)); // Clear bit 4+5 PTB4 = GPIO1, PTB5 = GPIO2


#define  mSETUP_PORT_C    PTCD = 0x00;\
                        PTCPE = 0x00;\
                        PTCDD = 0x63;

#define  mSETUP_PORT_D  PTDPE = 0x00;\
                        PTDSE = 0x00;\
                        PTDDD = 0x00 | 0x08 | 0x10;  // Port 3,4 as output
                        //PTDDD = (0x01 | 0x02| 0x08 | 0x10);


#endif TARGET_DIG534_1

//**********************************************************************************

#ifdef TARGET_DIG536_1
    // UART interface select SCI1
    #define SELECT_SCI1

    #define HWAssertAbelReset        PTCD &= ~0x10;    // Reset = 0;
    #define HWDeAssertAbelReset      PTCD |=  0x10;    // Reset = 1;

    #define RxTxEnable               PTCD |=  0x08;    // RxTxEnable = 1 (PTC3);
    #define RxTxDisable              PTCD &= ~0x08;    // RxTxDisable = 0 (PTC3);  

    #define AttEnable                PTCD |=  0x04;    // PTC2
    #define AttDisable               PTCD &= ~0x04;

    #define LED1_PIN  (1<<0)
    #define LED2_PIN  (1<<1)
    #define LED3_PIN  (1<<3)
    #define LED4_PIN  (1<<4)

    #define LED1ON  PTDD &= 0xFE;
    #define LED1OFF PTDD |= 0x01;
    #define LED1TOGGLE PTDD ^= 0x01;

    #define LED2ON  PTDD &= 0xFD;
    #define LED2OFF PTDD |= 0x02;
    #define LED2TOGGLE PTDD ^= 0x02;

    #define LED3ON  PTDD &= 0xF7;
    #define LED3OFF PTDD |= 0x08;
    #define LED3TOGGLE PTDD ^= 0x08;

    #define LED4ON  PTDD &= 0xEF;
    #define LED4OFF PTDD |= 0x10;
    #define LED4TOGGLE PTDD ^= 0x10;

    #define IsAbelActive()   (PTBD & (1<<4))
    #define IsAbelCrcOk()    (PTBD & (1<<5))
    #define IsAbelCcaBusy()  (PTBD & (1<<5))

    #define ENABLE_ANTENNESWITCH_PINS
    #define RX_ANTENNE_ENABLED
    #define TX_ANTENNE_ENABLED

      // Setup Port C for ABEL setup (GPIO 1+2)
    #define ABEL_GPIO1            (1<<4)
    #define ABEL_GPIO2            (1<<5)

    #define ABEL_ATT_PIN        (1<<2)
    #define ABEL_RxTx_PIN       (1<<3)
    #define ABEL_RESET_PIN        (1<<4)
    #define ABEL_ANT_SWITCH_PIN // No antenna switch

    // The switches
    #define mSWITCH1_MASK 0x04
    #define mSWITCH2_MASK 0x08
    #define mSWITCH3_MASK 0x10
    #define mSWITCH4_MASK 0x20
    #define mSWITCH_MASK 0x3C
    #define mSWITCH_PORT_GET ((PTAD & mSWITCH_MASK) ^ mSWITCH_MASK)


    #define  mSETUP_PORT_A  PTAPE = 0x3C;\
                            PTADD = 0x00;        //All Port A input

    #define  mSETUP_PORT_B    PTBD = 0x00;\
                            PTBPE = 0x00;\
                            PTBDD = 0x00;

    #define  mSETUP_PORT_C  PTCDD = (ABEL_RESET_PIN | ABEL_ATT_PIN | ABEL_RxTx_PIN ); // | ABEL_ANT_SWITCH_PIN);
                            

    #define  mSETUP_PORT_D  PTDPE = 0x00;\
                            PTDDD = (0x01 | 0x02| 0x08 | 0x10);

#define USE_LOW_POWER
#define mENTER_LOWPOWER  SIMOPT |= 0x20;PMCSC2 &=  ~0x02;__asm STOP;

#if defined FLASH_LIB_ENABLED || defined BOOTLOADER_ENABLED
  #define USE_NVM_QUICK_START
#endif defined FLASH_LIB_ENABLED || defined BOOTLOADER_ENABLED                

#endif TARGET_DIG536_1

//**********************************************************************************

#if defined TARGET_DIG536_2 || defined TARGET_DIG528_2
    // UART interface select SCI1
    #define SELECT_SCI1
    #define SELECT_SCI2

    #define HWAssertAbelReset        PTCD &= ~0x10;    // Reset = 0;
    #define HWDeAssertAbelReset      PTCD |=  0x10;    // Reset = 1;

    #define RxTxEnable               PTCD |=  0x08;    // RxTxEnable = 1 (PTC3);
    #define RxTxDisable              PTCD &= ~0x08;    // RxTxDisable = 0 (PTC3);  

    #define AttEnable                PTCD |=  0x04;    // PTC2
    #define AttDisable               PTCD &= ~0x04;

    #define LED1_PIN  (1<<0)
    #define LED2_PIN  (1<<1)
    #define LED3_PIN  (1<<3)
    #define LED4_PIN  (1<<4)

    #define LED1ON  PTDD &= 0xFE;
    #define LED1OFF PTDD |= 0x01;
    #define LED1TOGGLE PTDD ^= 0x01;

    #define LED2ON  PTDD &= 0xFD;
    #define LED2OFF PTDD |= 0x02;
    #define LED2TOGGLE PTDD ^= 0x02;

    #define LED3ON  PTDD &= 0xF7;
    #define LED3OFF PTDD |= 0x08;
    #define LED3TOGGLE PTDD ^= 0x08;

    #define LED4ON  PTDD &= 0xEF;
    #define LED4OFF PTDD |= 0x10;
    #define LED4TOGGLE PTDD ^= 0x10;

    #define IsAbelActive()   (PTBD & (1<<4))
    #define IsAbelCrcOk()    (PTBD & (1<<5))
    #define IsAbelCcaBusy()  (PTBD & (1<<5))

    #define ANTENNE_SWITCH_ENABLED

    #ifdef ANTENNE_SWITCH_ENABLED
      #define ENABLE_ANTENNESWITCH_PINS PTBDD |= 0x40;
      #define RX_ANTENNE_ENABLED PTBD &= ~0x40;     // PTB6 = 0
      #define TX_ANTENNE_ENABLED PTBD |= 0x40;      // PTB6 = 1
    #else
      #define ENABLE_ANTENNESWITCH_PINS
      #define RX_ANTENNE_ENABLED
      #define TX_ANTENNE_ENABLED
    #endif


      // Setup Port C for ABEL setup (GPIO 1+2)
    #define ABEL_GPIO1            (1<<4)
    #define ABEL_GPIO2            (1<<5)

    #define ABEL_ATT_PIN        (1<<2)
    #define ABEL_RxTx_PIN       (1<<3)
    #define ABEL_RESET_PIN        (1<<4)
    #define ABEL_ANT_SWITCH_PIN // No antenna switch

    // The switches
    #define mSWITCH1_MASK 0x04
    #define mSWITCH2_MASK 0x08
    #define mSWITCH3_MASK 0x10
    #define mSWITCH4_MASK 0x20
    #define mSWITCH_MASK 0x3C
    #define mSWITCH_PORT_GET ((PTAD & mSWITCH_MASK) ^ mSWITCH_MASK)


    #define  mSETUP_PORT_A  PTAPE = 0x3C;\
                            PTADD = 0x00;        //All Port A input

    #define  mSETUP_PORT_B    PTBD = 0x00;\
                            PTBPE = 0x00;\
                            PTBDD = 0x00;

    #define  mSETUP_PORT_C  PTCDD = (ABEL_RESET_PIN | ABEL_ATT_PIN | ABEL_RxTx_PIN ); // | ABEL_ANT_SWITCH_PIN);
                            

    #define  mSETUP_PORT_D  PTDPE = 0x00;\
                            PTDDD = (0x01 | 0x02| 0x08 | 0x10);

#define USE_LOW_POWER
#define mENTER_LOWPOWER  SIMOPT |= 0x20;PMCSC2 &=  ~0x02;__asm STOP;

#if defined FLASH_LIB_ENABLED || defined BOOTLOADER_ENABLED
  #define USE_NVM_QUICK_START
#endif defined FLASH_LIB_ENABLED || defined BOOTLOADER_ENABLED                

#endif TARGET_DIG536_2 || TARGET_DIG528_2

//**********************************************************************************

#ifdef TARGET_DIG538_1
    // UART interface select SCI1 or SCI2
    //#define SELECT_SCI1
    #define SELECT_SCI2

    #define HWAssertAbelReset        PTCD &= ~0x10;    // Reset = 0;
    #define HWDeAssertAbelReset      PTCD |=  0x10;    // Reset = 1;

    #define RxTxEnable               PTCD |=  0x08;    // RxTxEnable = 1 (PTC3);
    #define RxTxDisable              PTCD &= ~0x08;    // RxTxDisable = 0 (PTC3);  

    #define AttEnable                PTCD |=  0x04;    // PTC2
    #define AttDisable               PTCD &= ~0x04;

    #define LED1_PIN  (1<<0)
    #define LED2_PIN  (1<<1)
    #define LED3_PIN  (1<<3)
    #define LED4_PIN  (1<<4)

    #define LED1ON  PTDD &= 0xFE;
    #define LED1OFF PTDD |= 0x01;
    #define LED1TOGGLE PTDD ^= 0x01;

    #define LED2ON  PTDD &= 0xFD;
    #define LED2OFF PTDD |= 0x02;
    #define LED2TOGGLE PTDD ^= 0x02;

    #define LED3ON  PTDD &= 0xF7;
    #define LED3OFF PTDD |= 0x08;
    #define LED3TOGGLE PTDD ^= 0x08;

    #define LED4ON  PTDD &= 0xEF;
    #define LED4OFF PTDD |= 0x10;
    #define LED4TOGGLE PTDD ^= 0x10;
  
    #define IsAbelActive()   (PTBD & (1<<4))
    #define IsAbelCrcOk()    (PTBD & (1<<5))
    #define IsAbelCcaBusy()  (PTBD & (1<<5))

    #define ENABLE_ANTENNESWITCH_PINS PTBDD |= 0xC0;
    #define RX_ANTENNE_ENABLED PTBD &= ~0x40;     // PTB6 = 0
    #define TX_ANTENNE_ENABLED PTBD |= 0x40;      // PTB6 = 1

      // Setup Port B for ABEL setup (Antenna switch, GPIO 1 and GPIO 2)
    #define ABEL_GPIO1            (1<<4)
    #define ABEL_GPIO2            (1<<5)
    #define ABEL_ANT_SWITCH_PIN (1<<6)

      // Setup Port C for ABEL setup (ATT, TXTX and RESET)
    #define ABEL_ATT_PIN        (1<<2)
    #define ABEL_RxTx_PIN       (1<<3)
    #define ABEL_RESET_PIN        (1<<4)

    // The switches
    #define mSWITCH1_MASK 0x04
    #define mSWITCH2_MASK 0x08
    #define mSWITCH3_MASK 0x10
    #define mSWITCH4_MASK 0x20
    #define mSWITCH_MASK  0x3C
    #define mSWITCH_PORT_GET ((PTAD & mSWITCH_MASK) ^ mSWITCH_MASK)


    #define  mSETUP_PORT_A  PTAPE = mSWITCH_MASK;\
                            PTADD = 0x00;        //All Port A input

    #define  mSETUP_PORT_B  PTBDD = (ABEL_ANT_SWITCH_PIN); // Output


    #define  mSETUP_PORT_C  PTCDD = (ABEL_ATT_PIN | ABEL_RxTx_PIN | ABEL_RESET_PIN); // Output

    #define  mSETUP_PORT_D  PTDPE = 0x00;\
                            PTDDD = (LED1_PIN | LED2_PIN| LED3_PIN | LED4_PIN); // Output

#endif TARGET_DIG538_1

//**********************************************************************************


//#define USE_RX_RESTART_FIX

// GROUP: SELECT ONE
#define PLATFORM_GB60       /* Set uProcessor platform as target */
//#define PLATFORM_WINDOWS /* Set windows platform as target */
// ENDGROUP

#ifdef INCLUDE_802_15_4 // Defined by NWK build environment
 #define F8W_NWK_PRESENT 1 // Define if compiling for F8W target - excludes PTC PHY fnc-table init.
#else
 //#define F8W_NWK_PRESENT 1
#endif INCLUDE_802_15_4

//#define RX_TIMING_MEASUREMENT

//#define LO_LOCK_DETECT_HACK

//#define ENABLE_SIMULATOR 1 /* When defined, use HAL/MAL for BTLC comm. */

#define LITTLE_ENDIAN 0
#define BIG_ENDIAN    1

//#define TIME_INTERRUPT
//#define ACTION_SETUP_TIMING
//#define ENABLETIMING /* ensure TIME_INTERRUPT is not defined!!! */

#ifdef TIME_INTERRUPT
  #define ENABLE_ISR_TIMING_PINS PTADD |= 0x0f;

  #define CHECK_ABEL_ISR_RUNNING_1 PTAD & 0x02;
  #define ISR_DISABLED_0 PTAD |= 0x01;
  #define ABEL_ISR_RUNNING_1 PTAD |= 0x02;

  #define ISR_ENABLED_0 PTAD &= ~0x01;
  #define ABEL_ISR_DONE_1 PTAD &= ~0x02;

  #define ISR_TOGGLE_0 PTAD ^= 0x01;
  #define ABEL_ISR_TOGGLE_1 PTAD ^= 0x02;

  #define ISR_TIMING_SET_2 PTAD |= 0x04;
  #define ISR_TIMING_SET_3 PTAD |= 0x08;
  #define ISR_TIMING_CLR_2 PTAD &= ~0x04;
  #define ISR_TIMING_CLR_3 PTAD &= ~0x08;
  #define ISR_TIMING_TGL_2 PTAD ^= 0x04;
  #define ISR_TIMING_TGL_3 PTAD ^= 0x08;
#else
  #define ENABLE_ISR_TIMING_PINS
  #define ISR_DISABLED_0
  #define ABEL_ISR_RUNNING_1

  #define ISR_ENABLED_0
  #define ABEL_ISR_DONE_1

  #define ISR_TOGGLE_0
  #define ABEL_ISR_TOGGLE_1

  #define ISR_TIMING_SET_2;
  #define ISR_TIMING_SET_3;
  #define ISR_TIMING_CLR_2;
  #define ISR_TIMING_CLR_3;
  #define ISR_TIMING_TGL_2;
  #define ISR_TIMING_TGL_3;
#endif

#define ENABLE_ALL_INTERRUPTS   __asm CLI; ISR_ENABLED_0;  // Macros to enable/disable all processor interrupts
#define DISABLE_ALL_INTERRUPTS  __asm SEI; ISR_DISABLED_0;

// Macros to enable/disable Abel interrupt
#define ENABLE_ABEL_INTERRUPTS_FAST   IRQSC |= 0x02;       //Set IRQPE = IRQ pin from Abel enabled
#define DISABLE_ABEL_INTERRUPTS_FAST  IRQSC &= ~0x02;      //Clear IRQPE = IRQ pin from Abel disabled
#define DISABLE_ABEL_INTERRUPTS(var)   __asm LDA 0x14 ! {A}, {}; __asm STA var; __asm BCLR 1,0x14 ! {}, {A};
#define ENABLE_ABEL_INTERRUPTS(var) __asm LDA var ! {A}, {}; __asm STA 0x14 ! {}, {A};

#define CLEAR_IRQ_STATUSREG(retReg) {ABEL_READ(ABEL_reg24, retReg); _asm BSET 2,0x14;}

// Macros to store/restore processors status register on stack
#define STOREINTERRUPTS(var)   __asm TPA; __asm STA var;
#define RESTOREINTERRUPTS(var) __asm LDA var; __asm TAP; ISR_ENABLED_0;
// Alternate way of storing/restoring status register on stack.
// Push/Pull is used, and care must be taken not to mess up the
// stack when using these macros. Compared to STOREINTERRUPTS/
// RESTOREINTERRUPTS the push/pull method results in a small
// decrease in code size, and a small increase in execution
// speed (9 bytes/11 cycles compared to 5 bytes/8 cycles).
#define PUSH_IRQ_STATUS __asm TPA;  __asm PSHA;
#define PULL_IRQ_STATUS __asm PULA; __asm TAP; ISR_ENABLED_0;

//#define USE_INTERRUPT_EOF /* Used if EOF interrupt is enabled. Otherwise, GPIO polling is used */









#define SMAPI_MLME_SM_DEBUG 0
#define SMAPI_MEM_LOWER_DEBUG 0
#define SMAPI_MEM_CTRL_DEBUG 0
#define SMAPI_SEQ_SUB_COMP_DEBUG 0
#define SMAPI_SEQ_UPPER_DEBUG 0

//#define MM_DEBUG // Enables debug info in the Memory Manager.
//#define ACTION_FAILED_CNTR // Enables debug messages when an ActionFailInd occurs.


/****************************************/
/****** ASSERT Functionality begin ******/

  // Main switch for enabling asserts on target
//#define ENABLE_ASSERTS
  // Print line number, and file name in either LCD
  // display or on COM port
//#define ENABLE_ASSERT_TEXTUAL
  // If defined a CPU breakpoint instruction is created,
  // otherwise a while(1) loop is used to halt the code.
#define ENABLE_ASSERT_BREAKPOINT

  // Asserts that may produce output and will stop the code
#define ASSERT_NONE 0
#define ASSERT_STOP 1
#define ASSERT_INFO 2

  // Module specific assert switches. Undefine or set
  // to ASSERT_NONE to disable asserts for a module.
#define PHY_ASSERT    ASSERT_NONE
#define MAC_ASSERT    ASSERT_STOP
#define SEQ_ASSERT    ASSERT_STOP
#define MEM_ASSERT    ASSERT_STOP
#define ASP_ASSERT    ASSERT_STOP
#define SMAPI_ASSERT  ASSERT_STOP
  // Asserts that may produce output, but does not stop the code
  // Should be used in combination with audio/visual indications.
#define PHY_ASSERT_INFO   ASSERT_NONE
#define MAC_ASSERT_INFO   ASSERT_INFO
#define SEQ_ASSERT_INFO   ASSERT_INFO
#define MEM_ASSERT_INFO   ASSERT_INFO
#define ASP_ASSERT_INFO   ASSERT_INFO
#define SMAPI_ASSERT_INFO ASSERT_INFO

#ifdef ENABLE_ASSERTS
 #ifdef ENABLE_ASSERT_TEXTUAL
  void digiassert(unsigned char *file, unsigned short line);
  void digiassert_info(unsigned char *file, unsigned short line);
  #pragma DISABLE_WARNING(ConditionAlwaysTrue_c)
  #pragma DISABLE_WARNING(ConditionAlwaysFalse_c)
  #define DigiAssert(level, regexp) if((level==ASSERT_STOP) && ((regexp)==0)) digiassert(__FILE__, __LINE__);\
                                    if((level==ASSERT_INFO) && ((regexp)==0)) digiassert_info(__FILE__, __LINE__);
 #else
  void digiassert(void);
  void digiassert_info(void);
  #pragma DISABLE_WARNING(ConditionAlwaysTrue_c)
  #pragma DISABLE_WARNING(ConditionAlwaysFalse_c)
  #define DigiAssert(level, regexp) if((level==ASSERT_STOP) && ((regexp)==0)) digiassert();\
                                    if((level==ASSERT_INFO) && ((regexp)==0)) digiassert_info();
 #endif  // ENABLE_ASSERT_TEXTUAL
#else
 #define DigiAssert(level, regexp) /* No assert */
#endif
/******  ASSERT Functionality end  ******/
/****************************************/

#define MAXTXLENGTH (127) /* max length of data in TX buffer */
#define MAXRXLENGTH (127) /* max length of data in RX buffer */





#ifdef ENABLETIMING
  #define ENABLE_TIMING_PINS PTADD |= 0x0f;
  #define SETTIMINGPIN_0 PTAD |= 0x01;
  #define SETTIMINGPIN_1 PTAD |= 0x02;
  #define SETTIMINGPIN_2 PTAD |= 0x04;
  #define SETTIMINGPIN_3 PTAD |= 0x08;

  #define CLRTIMINGPIN_0 PTAD &= ~0x01;
  #define CLRTIMINGPIN_1 PTAD &= ~0x02;
  #define CLRTIMINGPIN_2 PTAD &= ~0x04;
  #define CLRTIMINGPIN_3 PTAD &= ~0x08;

  #define TOGGLETIMINGPIN_0 PTAD ^= 0x01;
  #define TOGGLETIMINGPIN_1 PTAD ^= 0x02;
  #define TOGGLETIMINGPIN_2 PTAD ^= 0x04;
  #define TOGGLETIMINGPIN_3 PTAD ^= 0x08;
#else
  #define ENABLE_TIMING_PINS
  #define SETTIMINGPIN_0
  #define SETTIMINGPIN_1
  #define SETTIMINGPIN_2
  #define SETTIMINGPIN_3

  #define CLRTIMINGPIN_0
  #define CLRTIMINGPIN_1
  #define CLRTIMINGPIN_2
  #define CLRTIMINGPIN_3

  #define TOGGLETIMINGPIN_0
  #define TOGGLETIMINGPIN_1
  #define TOGGLETIMINGPIN_2
  #define TOGGLETIMINGPIN_3
#endif ENABLETIMING


#ifdef ACTION_SETUP_TIMING
  #define ENABLE_ACT_TIMING_PINS PTADD |= 0x0f;
  #define ACT_SETTIMINGPIN_0 PTAD |= 0x01;
  #define ACT_SETTIMINGPIN_1 PTAD |= 0x02;
  #define ACT_SETTIMINGPIN_2 PTAD |= 0x04;
  #define ACT_SETTIMINGPIN_3 PTAD |= 0x08;

  #define ACT_CLRTIMINGPIN_0 PTAD &= ~0x01;
  #define ACT_CLRTIMINGPIN_1 PTAD &= ~0x02;
  #define ACT_CLRTIMINGPIN_2 PTAD &= ~0x04;
  #define ACT_CLRTIMINGPIN_3 PTAD &= ~0x08;

  #define ACT_TOGGLETIMINGPIN_0 PTAD ^= 0x01;
  #define ACT_TOGGLETIMINGPIN_1 PTAD ^= 0x02;
  #define ACT_TOGGLETIMINGPIN_2 PTAD ^= 0x04;
  #define ACT_TOGGLETIMINGPIN_3 PTAD ^= 0x08;
#else
  #define ENABLE_ACT_TIMING_PINS
  #define ACT_SETTIMINGPIN_0
  #define ACT_SETTIMINGPIN_1
  #define ACT_SETTIMINGPIN_2
  #define ACT_SETTIMINGPIN_3

  #define ACT_CLRTIMINGPIN_0
  #define ACT_CLRTIMINGPIN_1
  #define ACT_CLRTIMINGPIN_2
  #define ACT_CLRTIMINGPIN_3

  #define ACT_TOGGLETIMINGPIN_0
  #define ACT_TOGGLETIMINGPIN_1
  #define ACT_TOGGLETIMINGPIN_2
  #define ACT_TOGGLETIMINGPIN_3
#endif ACTION_SETUP_TIMING


#endif _TARGET_SPECIFIC_H_
