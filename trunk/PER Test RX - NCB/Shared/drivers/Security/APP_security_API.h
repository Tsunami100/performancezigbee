/* APP_security_API.h 
 * Mains application header

 * $Author: rgd04c $
 * $Date: 2006/09/01 22:33:08 $
 * $Name: BeeKitLaunch $
 */
 
#ifndef __APP_SECURITY_API_H__
#define __APP_SECURITY_API_H__

#include "pub_def.h"
#include "app_config.h"

/* Prototypes */
extern void SECSecure(UINT8 *pBuffer, UINT8 u8Length);
extern void SECSetKey(UINT8 *pBuffer, UINT8 u8Length);





#endif /* __APP_SECURITY_API_H__ */