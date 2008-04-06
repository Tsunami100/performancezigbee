/************************************************************************************
* This is a global header file for setting up the functionalities that should be
* included in the build.
*
* Author(s): JRJEN1
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
*
* Source Safe revision history (Do not edit manually) 
*   $Date: 2006/08/22 23:44:59 $
*   $Author: a20639 $
*   $Revision: 1.1 $
*   $Workfile: FunctionalityDefines.h $
************************************************************************************/

#ifndef _FUNCTIONALITY_DEFINES_H_
#define _FUNCTIONALITY_DEFINES_H_

//#define ANTENNE_SWITCH_ENABLED  //Define ONLY used in Target.h
//#define I_AM_A_SNIFFER

//#define Type_MRFD

  // All device types: (are defined because of HC08 compiler problems only)
#define gDeviceTypeMRFD_d  1
#define gDeviceTypeRFD_d   2
#define gDeviceTypeFFDD_d  3
#define gDeviceTypeFFDDC_d 4
#define gDeviceTypeFFDP_d  5
#define gDeviceTypeFFDDP_d 6
#define gDeviceTypeFFDC_d  7

// Target specific compiler defines (no value) "Type_..."
#ifdef Type_MRFD
    #define gDeviceType_d gDeviceTypeMRFD_d
    #define DEVICE_TYPE "MRFD "
    #define SYSTEM_CLOCK_16MHZ
    //#define SYSTEM_CLOCK_12MHZ
    //#define SYSTEM_CLOCK_16MHZ
#endif Type_MRFD

#ifdef  Type_RFD
    #define gDeviceType_d gDeviceTypeRFD_d
    #define DEVICE_TYPE "RFD  "
    #define SYSTEM_CLOCK_16MHZ
    //#define SYSTEM_CLOCK_12MHZ
    //#define SYSTEM_CLOCK_16MHZ
#endif  Type_RFD

#ifdef Type_FFDD
    #define gDeviceType_d gDeviceTypeFFDD_d
    #define DEVICE_TYPE "FFDD "
    #define SYSTEM_CLOCK_16MHZ
    //#define SYSTEM_CLOCK_12MHZ
    //#define SYSTEM_CLOCK_16MHZ
#endif Type_FFDD

#ifdef Type_FFDDC
    #define gDeviceType_d gDeviceTypeFFDDC_d
    #define DEVICE_TYPE "FFDDC"
    #define SYSTEM_CLOCK_16MHZ
    //#define SYSTEM_CLOCK_12MHZ
    //#define SYSTEM_CLOCK_16MHZ
#endif Type_FFDDC

#ifdef Type_FFDP
    #define gDeviceType_d gDeviceTypeFFDP_d
    #define DEVICE_TYPE "FFDP "
    #define SYSTEM_CLOCK_16MHZ
    //#define SYSTEM_CLOCK_12MHZ
    //#define SYSTEM_CLOCK_16MHZ
#endif Type_FFDP

#ifdef Type_FFDDP
    #define gDeviceType_d gDeviceTypeFFDDP_d
    #define DEVICE_TYPE "FFDDP"
    #define SYSTEM_CLOCK_16MHZ
    //#define SYSTEM_CLOCK_12MHZ
    //#define SYSTEM_CLOCK_16MHZ
#endif Type_FFDDP

#ifdef Type_FFDC
    #define gDeviceType_d gDeviceTypeFFDC_d
    #define DEVICE_TYPE "FFDC "
    #define SYSTEM_CLOCK_16MHZ
    //#define SYSTEM_CLOCK_12MHZ
    //#define SYSTEM_CLOCK_16MHZ
#endif Type_FFDC

#ifndef gDeviceType_d // If device type is not predefined use the following definitions:

  #define DEVICE_TYPE "XXXXX"

// GROUP: SELECT ONE
    #define SYSTEM_CLOCK_8MHZ
    //#define SYSTEM_CLOCK_12MHZ
    //#define SYSTEM_CLOCK_16MHZ

  /**********************************************************************************
  ***********************************************************************************
  * Role capability definitions
  ***********************************************************************************
  **********************************************************************************/

    // Set if the device need to be able to take on the device role
  #define gDeviceCapability_d 1

    // Set if the device need to be able to take on the coordinator role
  #define gCoorCapability_d 1

  /**********************************************************************************
  ***********************************************************************************
  * Functionality inclusion definitions that depend on device type
  ***********************************************************************************
  **********************************************************************************/

    // Set if the device need to be able to take on the pan coordinator role
    // Is never set unless coordinator capability is set
  #define gPanCoorCapability_d (1 && gCoorCapability_d)

    // Set if device (in role of either device or ccordinator) supports beaconed
    // operation. Note that ...
  #define gBeaconedCapability_d 1

    // Set if device (in role of either device or ccordinator) supports GTS in
    // beaconed operation. Is never set unless beaconed operation is supported
  #define gGtsCapability_d (1 && gBeaconedCapability_d)

    // SCAN functionalities - optional for RFD and below (=spec. RFD)
  #define gEnergyScanCapability_d 1
  #define gActiveScanCapability_d 1

    // MCPS-PURGE.request capabilities
  #define gPurgeCapability_d 1

#else // If gDeviceType_d is defined use one of the following presets:

  #if gDeviceType_d == gDeviceTypeMRFD_d
    #define gDeviceCapability_d   1
    #define gCoorCapability_d     0
    #define gPanCoorCapability_d  0
    #define gBeaconedCapability_d 0
    #define gGtsCapability_d      0
    #define gOptionalPack_d       0
  #elif gDeviceType_d == gDeviceTypeRFD_d
    #define gDeviceCapability_d   1
    #define gCoorCapability_d     0
    #define gPanCoorCapability_d  0
    #define gBeaconedCapability_d 1
    #define gGtsCapability_d      0
    #define gOptionalPack_d       0
  #elif gDeviceType_d == gDeviceTypeFFDD_d
    #define gDeviceCapability_d   1
    #define gCoorCapability_d     0
    #define gPanCoorCapability_d  0
    #define gBeaconedCapability_d 1
    #define gGtsCapability_d      1
    #define gOptionalPack_d       1
  #elif gDeviceType_d == gDeviceTypeFFDDC_d
    #define gDeviceCapability_d   1
    #define gCoorCapability_d     1
    #define gPanCoorCapability_d  0
    #define gBeaconedCapability_d 1
    #define gGtsCapability_d      1
    #define gOptionalPack_d       1
  #elif gDeviceType_d == gDeviceTypeFFDP_d
    #define gDeviceCapability_d   0
    #define gCoorCapability_d     1
    #define gPanCoorCapability_d  1
    #define gBeaconedCapability_d 1
    #define gGtsCapability_d      1
    #define gOptionalPack_d       1
  #elif gDeviceType_d == gDeviceTypeFFDDP_d
    #define gDeviceCapability_d   1
    #define gCoorCapability_d     1
    #define gPanCoorCapability_d  1
    #define gBeaconedCapability_d 1
    #define gGtsCapability_d      1
    #define gOptionalPack_d       1
  #elif gDeviceType_d == gDeviceTypeFFDC_d
    #define gDeviceCapability_d   0
    #define gCoorCapability_d     1
    #define gPanCoorCapability_d  0
    #define gBeaconedCapability_d 1
    #define gGtsCapability_d      1
    #define gOptionalPack_d       1
  #else
    #error Invalid gDeviceType_d define value
  #endif // gDeviceType_d
  
    // SCAN capabilities
  #define gEnergyScanCapability_d gOptionalPack_d
  #define gActiveScanCapability_d gOptionalPack_d

    // MCPS-PURGE.request capabilities
  #define gPurgeCapability_d gOptionalPack_d

#endif // gDeviceType_d

/************************************************************************************
*************************************************************************************
* Functionality inclusion definitions that are not directly dependant on device type
*************************************************************************************
************************************************************************************/

  // Set if security is supported
#define gSecurityCapability_d 0

  // Set if ASP features are supported
#define gAspCapability_d 1

  // Set to 1 if new disassociate request primitive must be supported. Does not require additional code.
#define gNewDisassociateReq_d 0

  // Transaction expiration capability
#define gTransactionExpiredCapability_d (1 && gCoorCapability_d)

  // Set to one to support the RX Enable feature
#define gRxEnableCapability_d 1

#if gGtsCapability_d==1
#undef gGtsCapability_d
#define gGtsCapability_d 1   // Master switch for GTS feature
#endif // gGtsCapability_d

/************************************************************************************
*************************************************************************************
* Buffer control definitions (numbers of but not sizes of message buffers)
*************************************************************************************
************************************************************************************/

  // Number of "small" message buffers needed
#define gTotalSmallMsgs_d 5

  // Buffers needed for TX (should be 1 but is 2 because...)
#define gBigMsgsForMinimumFunctionality_d 2

  // Buffers needed for RX
#define gBigMsgsCoorCapability_d (1 * gCoorCapability_d)

  // Buffer needed for beaconed operation (needed for either RX or TX of beacon)
#define gBigMsgsBeaconedCapability_d (1 * gBeaconedCapability_d )

  // Buffers needed for GTS (This parameter heavily depend on the usage scenario)
#define gBigMsgsGtsCapability_d (2 * gGtsCapability_d)

  // Message buffers needed for:
  // 1) If NWK want to send more than one MCPS-Data.Request before getting the
  //    corresponding MCPS-Data.Confirm
  // 2) TBD
#define gBigMsgsExtraNwkPending_d 1

  // Here any extra message buffers other than required for normal operation may be
  // defined...
#define gBigMsgsExtraOverhead_d 0

  // The sum of buffers needed for "big" messages is calculated
#define gTotalBigMsgs_d gBigMsgsForMinimumFunctionality_d +  \
                        gBigMsgsCoorCapability_d +           \
                        gBigMsgsBeaconedCapability_d +       \
                        gBigMsgsGtsCapability_d +            \
                        gBigMsgsExtraNwkPending_d +          \
                        gBigMsgsExtraOverhead_d


/************************************************************************************
*************************************************************************************
* Proprietary stuff... 
*************************************************************************************
************************************************************************************/

#endif /* _FUNCTIONALITY_DEFINES_H_ */
