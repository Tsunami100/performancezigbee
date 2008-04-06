 /*!
 * Copyright (c) 2005, Freescale Semiconductor
 *
 * Freescale Confidential Proprietary
 * \file    drivers.h
 * \brief   This is the SMAC C source driver header file for the HCS08 MCU
 * and MC13192 transceiver.  
 * \author  a19259
 * \version 4.1a
 * \date    2005/07/29 02:27:50 
 * 
 * \b Description:
 *
 * The header defines all externals, prototypes and 
 * MC13192 status mask bits used by the actual C driver.
 *
 * \b Department: Freescale Radio Products Division 
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 16/11/2005 Doc. update to Doxygen compliant by Mario Rodriguez r12369c
 */ 
 
#ifndef _DRIVERS_H
#define _DRIVERS_H

#include "derivative.h"
#include "pub_def.h"

/* Externals */
extern UINT8 gu8RTxMode;            //!< Global mode mirror. 

/* Defines */
#define TX_IRQ_MASK         0x0040  /*!< Transmit IRQ Mask */
#define RAMERR_IRQ_MASK     0x4000  /*!< RAM add error IRQ Mask */
#define RX_IRQ_MASK         0x0080  /*!< Receive IRQ Mask */
#define ATTN_IRQ_MASK       0x0400  /*!< Attention IRQ Mask */
#define CRC_VALID_MASK      0x0001  /*!< CRC valid Mask */
#define TIMER1_IRQ_MASK     0x0100  /*!< Timer1 IRQ Mask */
#define XCVR_SEQ_MASK       0xFFF8  /*!< Transceiver seq mask */
#define LO_LOCK_IRQ_MASK    0x8000  /*!< Lo lock IRQ Mask */
#define CCA_IRQ_MASK        0x0020  /*!< CCA IRQ Mask */
#define DOZE_IRQ_MASK       0x0200  /*!< Doze IRQ Mask */
#define RESET_BIT_MASK      0x0080  /*!< Reset bit Mask */
#define CCA_BIT_MASK        0x0002

/* Prototypes */
/* See drivers.c for a complete description. */
void SPIDrvWrite(UINT8 u8Addr, UINT16 u16Content);
UINT16 SPIDrvRead(UINT8 u8Addr);
UINT16 SPIDrvRead2(UINT8 u8Addr);
void RAMDrvWriteTx(tTxPacket *psTxPkt);
UINT8 RAMDrvReadRx(tRxPacket *psRxPkt);


extern void AssertCE(void);            
extern void DeAssertCE(void);          
extern void RTXENDeAssert(void) ;      
extern void RTXENAssert(void);         
extern void MC13192Wake(void);  
extern UINT8 IRQPinLow(void);

extern void SPIWaitTransferDone(void);       
extern void SPIClearRecieveStatReg(UINT8 * u8TempValue);    
extern void SPIClearRecieveDataReg(UINT8 * u8TempValue);    
extern void SPISendChar(UINT8 u8Char);         
extern UINT8 SPIRead(void);
extern void SPIReadMSB(UINT16 * u16Data);                
extern void SPIReadLSB(UINT16 * u16Data);      

#if defined (_56F800E)
void IRQIsr(void);
#endif

#endif /* _DRIVERS_H */