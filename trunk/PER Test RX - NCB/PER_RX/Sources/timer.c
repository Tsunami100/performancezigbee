#include "timer.h"
#include "eventDefs.h"

/* Globals */
UINT16 gu16msTimer = 0;
UINT16 gu16timerEvent[2];
extern UINT16 gu16Events;

interrupt void IRQTimer1(void) {

   gu16msTimer++;
   if (gu16timerEvent[0] != 0) {
      if(gu16timerEvent[0] == gu16msTimer) {
         gu16Events |= TIMER_EVENT0;
         gu16timerEvent[0] = 0; /* Turn off the timer0 */
      }
   }
   
   if (gu16timerEvent[1] != 0) {
      if(gu16timerEvent[1] == gu16msTimer) {
         gu16Events |= TIMER_EVENT1;
         gu16timerEvent[1] = 0; /* Turn off the timer1 */
      }
   }   
   
   TPM1SC &= ~0x80;  /* clear the interrupt */
}

/**************************************************************
*    Function:     delay
*    Parameters: 16-bit delay value
*    Return:        none
*
*    Simple blocking delay loop using HW timer.
**************************************************************/

 
void delaySymbols(UINT16 count) {
  UINT16 u16Match;
  
  u16Match = gu16msTimer + count;
  while (gu16msTimer != u16Match);
}

void delaySeconds(UINT16 count) {
  delaySymbols(count*62500);
}

void delay(UINT16 count) {
  delaySymbols(count*625);
}

