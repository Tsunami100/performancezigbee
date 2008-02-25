/*
 * Copyright (c) 2006, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 */
/*!
 * \file board_config.h
 *
 * \brief Board configuration
 *
 * \author $Author: a20639 $
 * 
 * \version 4.2
 *
 * \date    $Date: 2006/08/23 05:10:13 $
 *  
 * \b Department : Freescale Commercial Wireless Operation
 *
 * \b Project: SMAC (Simple Media Access Controller)
 *
 * \b History:
 * - 18/JULY/2006 File Creation
 */ 
 
 
 /* Select the features you want to include within the SMAC */
 
 #ifndef __BOARD_CONFIG_H__
 #define __BOARD_CONFIG_H__


#define MC1319XSARD   0
#define MC1319XEVB    1
#define MC1319XAXMC   2
#define MC1321XSRB    3
#define MC1321XNCB    4
#define MC1320XAXMC   5
#define MC1319XARD    10
#define MC1319XIBOARD 11


#define TARGET_BOARD  MC1321XSRB

/* LCD_ENABLED property*/
#define LCD_ENABLED FALSE

/* SCI related properties */
#define DEFAULT_SCI_PORT      2



 #endif /* __BOARD_CONFIG_H__ */