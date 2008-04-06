/* security.h 
 * Mains application header

 * $Author: rgd04c $
 * $Date: 2006/09/01 22:33:08 $
 * $Name: BeeKitLaunch $
 */
 
#ifndef __SECURITY_H__
#define __SECURITY_H__

#include "pub_def.h"
#include "app_config.h"

#if SMAC_FEATURE_SECURITY == FALSE
  #error {"SMAC Security Feature must be enabled.  Please " 
         "define it in file smac_config.h and recompile the " 
         "SMAC project"}
#endif /* SMAC_SECURITY_FEATURE */


/* Macro required */
#define SEC_MODE_NOSECURITY 0
#define SEC_MODE_CCM        1 /* Not implemented */
#define SEC_MODE_CTR        2 /* Not implemented */
#define SEC_MODE_CTS        3 /* Not implemented */
#define SEC_MODE_SIMPLEXOR  4
#define Not_Implemented -1

/* Module configuration */
#define SEC_CONFIG_MODE SEC_MODE_SIMPLEXOR

/* Default key used just in case the user doesn't specify none */
#define SEC_DEFAULT_KEY        "555555"
#define SEC_DEFAULT_KEY_SIZE   sizeof(SEC_DEFAULT_KEY)


//Prototypes
void SECSecure(UINT8 *pBuffer, UINT8 u8Length);
void SECSetKey(UINT8 *pBuffer, UINT8 u8Length);

void CCM_XorN (UINT8 *pDst, UINT8 *pSrc, UINT8 n);
UINT8 SecLib_CcmStar(void);
UINT8 SecLib_Aes(void);




#endif /* __SECURITY_H__ */