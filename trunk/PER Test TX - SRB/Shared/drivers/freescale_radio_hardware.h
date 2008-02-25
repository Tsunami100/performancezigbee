/* wireless_uart.h 
 * Mains application header

 * $Author: a20639 $
 * $Date: 2006/08/22 23:44:59 $
 * $Name:  $
 */
 
//includes
#include "app_config.h"
#include "derivative.h"

/* Define the correct pin interface to the RFIC */
#include "pub_def.h"
#if (TARGET_BOARD == MC1319XAXMC)
   #include "MC1319XAXMC.h"
   
#elif (TARGET_BOARD == MC1319XARD)
   #include "MC1319XARD.h"
   
#elif (TARGET_BOARD == MC1319XSARD)
   #include "MC1319XSARD.h"
#elif (TARGET_BOARD == MC1319XEVB)
   #include "MC1319XEVB.h"
   
#elif (TARGET_BOARD == MC1319XIBOARD)
   #include "MC1319XIBOARD.h"   
      
#elif (TARGET_BOARD == MC1321XNCB)
   #include "MC1321XNCB.h"

#elif (TARGET_BOARD == MC1321XSRB)
   #include "MC1321XSRB.h"
   
#else (TARGET_BOARD == MC1320XAXMC)
   #include "MC1320XAXMC.h"   
#endif
