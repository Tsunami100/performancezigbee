/*
 * Copyright (c) 2006, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 */
/*!
 * \file app_config.h
 *
 * \brief Application and SMAC configuration file. This file permits select the profile
 *        of smac to be used
 *
 * \author $Author: rgd04c $
 * 
 * \version 4.2
 *
 * \date    $DATE:$
 *  
 * \b Department : Freescale Commercial Wireless Operation
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 18/JULY/2006 File Creation
 */ 
 
 
 /* Select the features you want to include within the SMAC */
 
 #ifndef __SMAC_CONFIG_H__
 #define __SMAC_CONFIG_H__

#include "pub_def.h"
#include "board_config.h"

/* SMAC features: 
 * To Enable a feature, just define it.
 * To disbale a feature, just undef it */  
  #define EMBEDDED_BOOTLOADER FALSE 
 #define SMAC_FEATURE_OTAP FALSE
 #define SMAC_FEATURE_SECURITY FALSE
 
 
 #if (SMAC_FEATURE_OTAP == TRUE)
 #include "SMAC_OTAP_API.h"
 #endif /* SMAC_FEATURE_OTAP */
 
 #if (SMAC_FEATURE_SECURITY == TRUE)
 #include "SMAC_SECURITY_API.h"
 #endif /* SMAC_FEATURE_SECURITY */
 
 #define SMAC_PROMISCUOUS_MODE FALSE
 
 #endif /* __SMAC_CONFIG_H__ */