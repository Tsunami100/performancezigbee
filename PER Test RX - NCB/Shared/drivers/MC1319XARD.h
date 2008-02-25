/* toro_gen_header.h 
 * toro header for test verification

 * $Author: a20639 $
 * $Date: 2006/08/27 03:08:22 $
 * $Name: BeeKitLaunch $
 */
 
//includes
#include "pub_def.h"
#include "board_config.h" 

//Prototypes




//Defines
/* EVB */

/**************************************************************
Enable peripherials that are on the Board
**************************************************************/

#define ACCEL_ENABLED         FALSE
#define TEMP_SENSOR_ENABLED   FALSE
#define BUZZER_ENABLED        TRUE
#define BUS_SPEED	            8000000

/**************************************************************
Define the SCI perameters
**************************************************************/
#define  SCIBDH     SCI1BDH
#define  SCIBDL     SCI1BDL
#define  SCIC1      SCI1C1 
#define  SCIC2      SCI1C2 
#define  SCIS1      SCI1S1 
#define  SCIS2      SCI1S2 
#define  SCIC3      SCI1C3 
#define  SCID       SCI1D

#define  SCIS1_TDRE  SCI1S1_TDRE
#define  SCIS1_TC    SCI1S1_TC
#define  SCI_DEFAULT_BAUD  38400

/**************************************************************
Define the LED perameters
**************************************************************/

#define LED1                    PTFD_PTFD0
#define LED1DIR                 PTFDD_PTFDD0
#define LED2                    PTFD_PTFD1
#define LED2DIR                 PTFDD_PTFDD1
#define LED3                    PTFD_PTFD2
#define LED3DIR                 PTFDD_PTFDD2
#define LED4                    PTFD_PTFD3
#define LED4DIR                 PTFDD_PTFDD3

/* Fifth LED is not defined for GB60EVB, set to Port F pin 4 */
#define LED5						  PTFD_PTFD4
#define LED5DIR                 PTFDD_PTFDD4
    
#define LED_ON                  1
#define LED_OFF					  0
#define DDIR_OUTPUT             1
#define DDIR_INPUT              0
    
#define LED_INIT_MACRO LED1     = LED_OFF; \
                       LED2     = LED_OFF; \
                       LED3     = LED_OFF; \
                       LED4     = LED_OFF; \
                       LED5     = LED_OFF; \
                       LED1DIR  = DDIR_OUTPUT; \
                       LED2DIR  = DDIR_OUTPUT; \
                       LED3DIR  = DDIR_OUTPUT; \
                       LED4DIR  = DDIR_OUTPUT; \
                       LED5DIR  = DDIR_OUTPUT;        
    
/***********************************************
Define Buzzer
***********************************************/
    
    
#define BUZZER         PTDD_PTDD2
#define BUZZERDIR      PTDDD_PTDDD2
#define BUZZER_ON      1
#define BUZZER_OFF     0
   
#define BUZZER_INIT_MACRO BUZZER      = BUZZER_OFF; \
                          BUZZERDIR   = DDIR_OUTPUT;
    
/***********************************************
Define Switches
***********************************************/
    
    
/* SW1 on schematic */
#define PB0 PTAD_PTAD4
#define PB0PU PTAPE_PTAPE4
#define PB0DIR PTADD_PTADD4
#define PB0IE KBIPE_KBIPE4
  
/* SW2 on schematic */
#define PB1 PTAD_PTAD5
#define PB1PU PTAPE_PTAPE5
#define PB1DIR PTADD_PTADD5
   
/* SW3 on schematic */
#define PB2 PTAD_PTAD6
#define PB2PU PTAPE_PTAPE6
#define PB2DIR PTADD_PTADD6
  
/* SW4 on schematic */
#define PB3 PTAD_PTAD7
#define PB3PU PTAPE_PTAPE7
#define PB3DIR PTADD_PTADD7

#define SWITCH_INIT_MACRO    PB0PU = TRUE; \
                             PB1PU = TRUE; \
                             PB2PU = TRUE; \
                             PB3PU = TRUE;


/***********************************************
LCD Settings
***********************************************/
#define LCD_INIT_MACRO    PTBDD = 0xFF;  /* Set whole port to output */
    
                              
                              
#define ENTER_LOW_POWER _asm stop

/***********************************************
Duplicate PB0 to PUSH_BUTTON1 etc for different
formats.
***********************************************/

#define PUSH_BUTTON1 PB0
#define PUSH_BUTTON2 PB1
#define PUSH_BUTTON3 PB2
#define PUSH_BUTTON4 PB3
