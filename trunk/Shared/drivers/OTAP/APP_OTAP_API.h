/* otap_destination.h 
 * Mains application header

 * $Author: rgd04c $
 * $Date: 2006/08/31 14:54:28 $
 * $Name: BeeKitLaunch $
 */
 
#ifndef __APP_OTAP_API_H__
#define __APP_OTAP_API_H__

#include "pub_def.h"
#include "app_config.h"

#if SMAC_FEATURE_OTAP == FALSE
  #error {"You have added the OTAP module to this application. " 
         "This requires that the SMAC is compiled with the OTAP feature " 
         "enabled (smac-config.h)"}
#endif

//Prototypes
extern void OTAPActivate(void);


#endif /* __APP_OTAP_API_H__ */