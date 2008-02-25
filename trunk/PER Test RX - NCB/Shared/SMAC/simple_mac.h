/*
 * Copyright (c) 2004, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 */
/*!
 * \file simple_mac.h
 *
 * \brief Function prototypes. See simple_mac.c for a complete 
 * description.
 *
 * \author a20309
 * 
 * \version 4.1a
 *
 * \date    2005/07/29 02:27:50 
 *  
 * \b Department : Freescale Radio Products Division
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 16/11/2005 Doc. update to Doxygen compliant by Laura Delgado r12370c
 * 
 */
#ifndef _SIMPLE_MAC_H
#define _SIMPLE_MAC_H

#include "pub_def.h"

/* Prototypes */
UINT8 MCPSDataRequest(tTxPacket *psTxPacket);
UINT8 MLMERXEnableRequest(tRxPacket *psRxPacket, UINT32 u32Timeout);
UINT8 MLMERXDisableRequest(void);
UINT8 MLMEHibernateRequest(void);
UINT8 MLMEDozeRequest(UINT32 u32Timeout);
UINT8 MLMEWakeRequest(void);
UINT8 MLMESetChannelRequest(UINT8 u8Channel);
UINT8 MLMESetMC13192ClockRate(UINT8 u8Freq);
UINT8 MLMESetMC13192TmrPrescale (UINT8 u8Freq);
UINT8 MLMEMC13192XtalAdjust(UINT8 u8ReqValue);
UINT8 MLMEEnergyDetect(void);
UINT8 MLMEMC13192SoftReset(void);
UINT8 MLMELinkQuality (void);
UINT8 MLMEMC13192FEGainAdjust(UINT8 u8GainValue);
UINT8 MLMEMC13192PAOutputAdjust(UINT8 u8PaValue);
UINT8 MLMEGetRficVersion(void);
void  MLMETestMode (tTxPacket *psPacket, UINT8 u8mode);
UINT8 MLMEScanRequest(UINT8 flags, UINT8 *pu8ChannelScan);


extern void MC13192DisableInterrupts(void);  /* Mask off interrupts from MC13192 */
//extern void RTXENDeAssert(void);             /* Force MC13192 to IDLE */
extern void MC13192RestoreInterrupts(void);

/* bugfix: 12338 */
extern void MLMEMC13192ResetIndication(void);          //!<This call back function deals with the soft reset condition on the MC1319x triggered by the MLMEMC13192SoftReset() function
extern void MCPSDataIndication(tRxPacket *gsRxPacket); //!<Processes data when recived by the MC1319X
/* end bugfix */
#endif /* _SIMPLE_MAC_H */