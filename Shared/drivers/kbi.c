#include "freescale_radio_hardware.h"
#include "kbi.h"
#include "eventDefs.h"

extern UINT16 gu16Events;
extern UINT16 gu16msTimer;
extern UINT16 gu16timerEvent[2];

interrupt void KBIisr (void) {

   UINT8 u8keys;
   

   u8keys = ~PTAD;
   
   /* Still in debounce time - just return */
   if (gu16timerEvent[1] != 0) {
      KBI1SC_KBACK = 1;
      return;
   }
      
   if ((u8keys & KBI_SW1) == KBI_SW1) {
      gu16timerEvent[1] = gu16msTimer + KBI_DEBOUNCE;
      gu16Events |= KBI2_EVENT;
   }
   
   if ((u8keys & KBI_SW2) == KBI_SW2) {
      gu16timerEvent[1] = gu16msTimer + KBI_DEBOUNCE;    
	   gu16Events |= KBI3_EVENT;
   }
   
   if ((u8keys & KBI_SW3) == KBI_SW3) {
      gu16timerEvent[1] = gu16msTimer + KBI_DEBOUNCE;
      gu16Events |= KBI4_EVENT;
   }
   
   if ((u8keys & KBI_SW4) == KBI_SW4) {
      gu16timerEvent[1] = gu16msTimer + KBI_DEBOUNCE;    
	   gu16Events |= KBI5_EVENT;
   }
   
   KBI1SC_KBACK = 1; 
   
}
void KBIInit(void) {

     PB0IE = 1;  /* Enable KBI PB0 */
     PB1IE = 1;
     PB2IE = 1;
     PB3IE = 1;
     
           
     
}

