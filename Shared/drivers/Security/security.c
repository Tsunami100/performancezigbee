/****************************************************************************
Security.c

$Author: rgd04c $
$Date: 2006/09/01 22:33:08 $
$Name: BeeKitLaunch $
****************************************************************************/

#include "pub_def.h"
#include "security.h"
/*#include <hidef.h> 
#include "simple_mac.h"
#include "mc13192_hw_config.h"
#include "freescale_radio_hardware.h"*/

UINT8 *pu8SECKey;
UINT8 u8SECKeyLength = 0;

void SECSetKey(UINT8 *pBuffer, UINT8 u8Length){
  pu8SECKey = pBuffer;
  u8SECKeyLength = u8Length;
}

void SECSecure(UINT8 *pu8Buffer, UINT8 u8Length){
#if SEC_CONFIG_MODE == SEC_MODE_NOSECURITY
  return;
#endif /* SEC_CONFIG_MODE == SEC_MODE_NOSECURITY */
#if SEC_CONFIG_MODE == SEC_MODE_CCM
  #error "Security Mode CCM not supported in the current version"
  return;
#endif /* SEC_CONFIG_MODE == SEC_MODE_CCM */

#if SEC_CONFIG_MODE == SEC_MODE_CTR
  #error "Security Mode CTR not supported in the current version"
  return;
#endif /* SEC_CONFIG_MODE == SEC_MODE_CTR */

#if SEC_CONFIG_MODE == SEC_MODE_CTS
  #error "Security Mode CTS not supported in the current version"
  return;
#endif /* SEC_CONFIG_MODE == SEC_MODE_CTS */

#if SEC_CONFIG_MODE == SEC_MODE_SIMPLEXOR
  UINT8 i = 0;

  if (u8SECKeyLength==0) {
    SECSetKey(SEC_DEFAULT_KEY, (UINT8) SEC_DEFAULT_KEY_SIZE);
  }

  if (u8SECKeyLength > 0) {
    while (i < u8Length){
      pu8Buffer[i++] ^= pu8SECKey[i % u8SECKeyLength];
    }
  }
#endif /* SEC_CONFIG_MODE == SEC_MODE_SIMPLEXOR */

  
}

void CCM_XorN (UINT8 *pDst, UINT8 *pSrc, UINT8 n){
UINT8 i = 0;

for(i = 0; i<n; i++) {

pDst[i] ^= pSrc[i];

}

}


UINT8 SecLib_CcmStar() {
    
  return Not_Implemented;
  
}


UINT8 SecLib_Aes(){
    
  return Not_Implemented;
  
}

/*void main(void)
{


    
    MCUInit();
    RadioInit();
   
    MLMEMC13192PAOutputAdjust(MAX_POWER);    
    MLMESetMC13192ClockRate(0);  
    UseExternalClock();   
    MLMESetChannelRequest(15);
    
    EnableInterrupts;
        
    for(;;) {    

    
    value = CCM_XorN (0xAB);
    value = CCM_XorN (value);

    
    
    }
}

void MCPSDataIndication(tRxPacket *gsRxPacket) 
{
      
}

void MLMEMC13192ResetIndication(void) 
{

}*/ 