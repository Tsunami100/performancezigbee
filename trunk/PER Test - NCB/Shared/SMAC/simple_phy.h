/*
 * Copyright (c) 2004, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 *
 */
/*!
 * \brief  Function prototypes. See simple_phy.c for a complete 
 * description.  
 *
 * \file simple_phy.h
 *
 * \author a19259 
 *
 * \version 4.1a
 *
 * \date    2005/07/29 02:27:50 
 *
 * \b Department : Freescale Radio Products Division
 *
 * \b Project name: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * 
 * - 16/11/2005 Doc. update to Doxygen compliant by Laura Delgado r12370c
 */

#ifndef _SIMPLE_PHY_H
#define _SIMPLE_PHY_H

#include "pub_def.h"
#include <hidef.h>

/* Prototypes */
UINT8  PDDataRequest(tTxPacket *psPacket);
UINT8  PDDataRequestWithoutLoad(void);
void   PDDataIndication(void);
UINT8  PLMEHibernateRequest(void);
UINT8  PLMEDozeRequest(UINT32 u32Timeout);
UINT8  PLMEWakeRequest(void);
UINT8  PLMESetChannelRequest(UINT8 u8Channel);
UINT8  PLMESetTrxStateRequest(UINT8 u8ReqMode);
UINT8  PLMEEnergyDetect(void);
UINT8  PLMECCARequest(void);
UINT8  PLMELinkQuality(void);
UINT32 PLMEGetTimeRequest(void);
UINT8  PLMESetMC13192ClockRate(UINT8 u8Freq);
UINT8  PLMESetMC13192TmrPrescale (UINT8 u8Freq); 
void   PLMESetTimeRequest(UINT32 u32RequestedTime);
UINT8  PLMEEnableMC13192Timer1(UINT32 u32CounterValue);
UINT8  PLMEDisableMC13192Timer1(void);
void   PLMEMC13192ResetIndication(void);
UINT8  PLMEMC13192SoftReset(void);
UINT8  PLMEMC13192XtalAdjust(UINT8 u8TrimValue);
UINT8  PLMEMC13192FEGainAdjust(UINT8 i8GainValue);
UINT8  PLMEMC13192PAOutputAdjust(UINT8 u8RequestedPAValue);   /* CHECK variable type mismatch */
UINT8  PLMEGetRficVersion(void);
void   PLMELoadPRBS9 (tTxPacket *psPacket);
UINT8  PLMEPhyReset(void);
UINT8  PLMEEnablePromiscuousMode(void);
UINT8  PLMEDisablePromiscuousMode(void);


#endif /* _SIMPLE_PHY_H */