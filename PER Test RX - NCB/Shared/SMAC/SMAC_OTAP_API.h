/*
 * Copyright (c) 2006, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 */
/*!
 * \file OTAP_API.h
 *
 * \brief SMAC Over the Air Programming Module API.
 *
 * \author $Author=$
 * 
 * \version 4.2
 *
 * \date    $DATE=$
 *  
 * \b Department : Freescale Commercial Wireless Operation
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 18/JULY/2006 File Creation
 */ 

#ifndef __OTAP_API__
#define __OTAP_API__

#define OTAP_ACTIVE   1
#define OTAP_INACTIVE 0

extern UINT8 gu8OTAPStatusAPI;
extern void OTAPDataIndication(tRxPacket *gsRxPacket);

#endif /* __SECURITY_API__ */