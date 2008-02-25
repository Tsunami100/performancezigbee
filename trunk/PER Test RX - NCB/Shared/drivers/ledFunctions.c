#include "ledFunctions.h"
#include "ascii_utilities.h"
#include <string.h>

extern void delay(UINT16);
/**************************************************************
*    Function:     setLedsMode()
*    Parameters: UINT8 mode, UINT16 value, UINT16 delay
*    Return: none
*
*     Mode BINARY
*     setLeds() turns on the LEDs for delay time.
*     5 - lights LED2, 4
*     7 - lisgts LED2,3,4...
***************************************************************/

void setLedsMode(UINT8 u8mode, UINT16 u16value, UINT16 u16ledDelay, UINT8 flags) {

   char str[10];
   UINT8 i;
   
   str[0] = '\0';
   
   if (flags & LED_INVERT) {
      u16value ^= 0xFFFF;
   }
   
   switch (u8mode) {
      case LED_BINARY_MODE:
         setLed((UINT8)(1 << (u16value % LED_COUNT)), flags);
         
         if (u16ledDelay != 0) {
            delay(u16ledDelay);
            setLed(0, LED_NO_FLAGS);
         }   
         break;
         
      case LED_HEX_MODE:      
         setLed((UINT8) u16value, flags);
         if (u16ledDelay != 0) {
            delay(u16ledDelay);
            setLed(0, LED_NO_FLAGS);
         } 
         delay(u16ledDelay);        
         break;
         
      case LED_COUNT_MODE:
         for (i = 0; i<u16value; i++) {
            LED1 = LED_OFF;
            delay(u16ledDelay);
            LED1 = LED_ON;
            delay(u16ledDelay);
         }
         delay(u16ledDelay);
         break;
         
      case LED_DIGIT_MODE:      
            int2string((UINT32) u16value, str);
			   trimWhiteSpace(str);
      
            for (i=0; i<strlen(str); i++) {
		         switch (str[i]) {
		              case '0':
		                  setLed(0x0F, LED_NO_FLAGS);
		                  break;
		              case '1':
		                  setLed(0x01, LED_NO_FLAGS);
		                  break;
		              case '2':
		                  setLed(0x02, LED_NO_FLAGS);		                  
		                  break;
		              case '3':
		                  setLed(0x03, LED_NO_FLAGS);
		                  break;
		              case '4':
		                  setLed(0x04, LED_NO_FLAGS);
		                  break;
		              case '5':
		                  setLed(0x05, LED_NO_FLAGS);
		                  break;
		              case '6':
		                  setLed(0x06, LED_NO_FLAGS);
		                  break;
		              case '7':
		                  setLed(0x07, LED_NO_FLAGS);
		                  break;
		              case '8':
		                  setLed(0x08, LED_NO_FLAGS);
		                  break;   
		              case '9':
		                  setLed(0x09, LED_NO_FLAGS);
		                  break;   
		              
		              default:
		               break;     
		         }
		         delay(u16ledDelay);
		         setLed(0, LED_NO_FLAGS);
		         delay(u16ledDelay);   
            }
         break;
   }
}

/**************************************************************
*    Function:     setLed()
*    Parameters: UINT8 value, UINT8 flags
*    Return: none
*
*     value = 1 - lights LED4
*     value = 3 -> Lights LED3,4
*
*     flags bit 0 invert bit.
***************************************************************/

void setLed(UINT8 u8value, UINT8 flags) {

   if (flags & 0x01) {
      u8value ^= 1;   /* invert the values */
   }

   switch (u8value % 16) {
            case 0:
               LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_OFF;
               break;               
            case 1:
               LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_ON;
               break;               
            case 2:
               LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_ON; LED4 = LED_OFF;
               break;
            case 3:
               LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_ON; LED4 = LED_ON;
               break;
            case 4:
               LED1 = LED_OFF; LED2 = LED_ON; LED3 = LED_OFF; LED4 = LED_OFF;
               break;
            case 5:
               LED1 = LED_OFF; LED2 = LED_ON; LED3 = LED_OFF; LED4 = LED_ON;
               break;
            case 6:
               LED1 = LED_OFF; LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_OFF;
               break;
            case 7:
               LED1 = LED_OFF; LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_ON;
               break;
            case 8:
               LED1 = LED_ON; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_OFF;
               break;
            case 9:
               LED1 = LED_ON; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_ON;
               break;
            case 10:
               LED1 = LED_ON; LED2 = LED_OFF; LED3 = LED_ON; LED4 = LED_OFF;
               break;
            case 11:
               LED1 = LED_ON; LED2 = LED_OFF; LED3 = LED_ON; LED4 = LED_ON;
               break;
            case 12:
               LED1 = LED_ON; LED2 = LED_ON; LED3 = LED_OFF; LED4 = LED_OFF;
               break;
            case 13:
               LED1 = LED_ON; LED2 = LED_ON; LED3 = LED_OFF; LED4 = LED_ON;
               break;
            case 14:
               LED1 = LED_ON; LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_OFF;
               break;
            case 15:
               LED1 = LED_ON; LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_ON;
               break;
   }
   
}
