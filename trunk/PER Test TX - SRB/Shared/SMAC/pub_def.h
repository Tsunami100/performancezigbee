/*!
 * Copyright (c) 2005, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 * \file    pub_def.h
 * \brief   Definitions for variable types, transciever modes and 
 * PHY status.
 * \author  a19259
 * \version 4.1a
 * \date    2005/07/29 02:27:50 
 *
 * \b Department: Freescale Radio Products Division 
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 16/11/2005 Doc. update to Doxygen compliant by Mario Rodriguez r12369c
 */ 

#ifndef __PUB_DEFINE__
#define __PUB_DEFINE__

/*
 * Standard Definitions:
 * These defines allow for easier porting to other compilers. if porting change
 * these defines to the required values for the chosen compiler.
 */

typedef unsigned char   UINT8;      //!<unsigned 8 bit definition 
typedef unsigned int    UINT16;     //!<unsigned 16 bit definition
typedef unsigned long   UINT32;     //!<unsigned 32 bit definition
typedef signed char     INT8;       //!<signed 8 bit definition 
typedef int             INT16;      //!<signed 16 bit definition
typedef long int        INT32;      //!<signed 32 bit definition


/* Modes defined for the transciever */
#define IDLE_MODE               0x00		/*!< Idle mode */
#define RX_MODE                 0x02		/*!< Receive mode */
#define TX_MODE                 0x03		/*!< Trasmit mode */
#define PULSE_TX_MODE           0x43		/*!< Pulse transmit mode */
#define CCA_MODE                0x01		/*!< CCA mode */
#define RX_MODE_WTO             0x80		/*!< Receive mode wto */
#define HIBERNATE_MODE          0x81		/*!< Hibernate mode */
#define DOZE_MODE               0x82		/*!< Doze mode */
#define IDLE_MODE_DOZE          0x83		/*!< Idle mode doze */
#define IDLE_MODE_ATTN          0x84		/*!< Idle mode attenuation */
#define TIMEOUT                 0x85		/*!< Timeout */
#define SYSTEM_RESET_MODE       0x86    /*!< Not implemented */
#define MC13192_RESET_MODE      0x87    /*!< Not implemented */
#define MC13192_CONFIG_MODE     0x88    /*!< Not implemented */
#define RESET_DELAY             0x89    /*!< Set to 100ms    */

/* Test modes defined for the transciever */
#define PULSE_TX_PRBS9            0x01  //!< Continuously transmit a PRBS9 pattern
#define FORCE_IDLE                0x02  //!< Back to idle mode
#define CONTINUOUS_RX             0x03  //!< continuous receive mode
#define CONTINUOUS_TX_MOD         0x04  //!< continuously transmit a 10101010 pattern
#define CONTINUOUS_TX_NOMOD       0x05  //!< continuously transmit an unmodulated CW 

#define TRUE    1		/*!< True */
#define FALSE   0		/*!< False */

#define MAXPACKETSIZE   126		/*!< Max packet size */

#define MAX_POWER       100     /*!< Numbers chosen arbitrarily but > 16 */
#define NOMINAL_POWER   0x0B		/*!< Nominal power */
#define MIN_POWER       50      /*!< Numbers chosen arbitrarily but > 16 */

#define SCAN_MODE_ED   0x0020
#define SCAN_MODE_CCA  0x0010

#define SUCCESS         0x77		/*!< Success */
#define INITIAL_VALUE   0x0			/*!< Initial value */


/* Status enumations for the PHY. */
enum PD_DATA_STATUS {RX_ON = 1, TRX_OFF};
enum MC13192_POWER_MODES {RF_POWER_ON = 1, RF_POWER_HIBERNATE, RF_POWER_DOZE};
enum PLME_SET_TRX_STATE_REQUEST {ERROR = 1};
#define OVERFLOW    1		/*!< Overflow */

/*! defines the variable to be received by the SMAC */
typedef struct {
    UINT8 u8MaxDataLength; //!< maximum packet size to accept If greater packet is dropped 
    UINT8 u8DataLength;		 //!< length in bytes of the received packet 
    UINT8 *pu8Data;				 //!< where the application would like to have the received data copied into 
    UINT8 u8Status;				 //!< determine if the data indication is due to a received packet or a timeout.
} tRxPacket;

/*! variable to be transmitted by the SMAC.*/
typedef struct {
    UINT8 u8DataLength; //!< number of bytes to transmit 
    UINT8 *pu8Data;			//!< pointer to the data buffer to transmit 
} tTxPacket;

/*! CCA measurement storage */
typedef struct {    
    UINT8 u8CCAMode; //!< CCA Mode 
    UINT8 u8InstantValue; //!< Instant mode 
    UINT8 u8FinalValue; //!< Final mode    
} tCCAMeasurement;


/* SMAC Phy Options */
/*typedef union {
  byte Byte;
  struct {
    byte PTDSE0      :1;                                    
    byte PTDSE1      :1;                                    
    byte PTDSE2      :1;                                    
    byte PTDSE3      :1;                                    
    byte PTDSE4      :1;                                    
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpPTDSE :5;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} PTDSESTR;      */


typedef union {
	
	UINT16 PhyOptions[2];
   struct {
      /* Header Processing Byte*/
      UINT16 u1PromiscuousMode   :1;
      UINT16 u15Undef1           :15;
      UINT16 u15Undef2;
   } Bits;
   struct {
      UINT16 u8HeaderProcessing;
      UINT16 u16ScanChannels;
   } Byte;
   
      
} tPhyOptions;


#ifndef NULL
    #define NULL    ((void *)0)	 /*!< NULL */
#endif

#endif