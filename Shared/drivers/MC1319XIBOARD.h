/* toro_gen_header.h 
 * toro header for test verification

 * $Author: a20639 $
 * $Date: 2006/08/25 14:55:00 $
 * $Name:  $
 */
 
//includes
#include "pub_def.h"
#include "board_config.h"

//Prototypes


#define HCS08G																	  /*!<HCS08G family member*/
#define MC13192_CE                  PTED_PTED2    /*!<Enables SPI transfers */     
#define MC13192_CE_PORT             PTEDD_PTEDD2  /*!<SPI enable pin data direction */              
#define MC13192_ATTN                PTAD_PTAD3		/*!<Transitions IC from either Hibernate 
                                                      or Doze Modes to Idle */ 
#define MC13192_ATTN_PORT           PTADD_PTADD3  /*!<ATTN pin data direction */
#define MC13192_RTXEN               PTCD_PTCD2		/*!<Enables Transmission or Reception from SPI */
#define MC13192_RTXEN_PORT          PTCDD_PTCDD2  /*!<SPI Transmission pin direction */
#define MC13192_RESET               PTAD_PTAD2		/*!<Transciever Reset */
#define MC13192_RESET_PORT          PTADD_PTADD2  /*!<Transciever Reset pin data direction  */
#define MC13192_RESET_PULLUP        PTAPE_PTAPE0  /*!<Enables pull up for Reset */
#define MC13192_IRQ_SOURCE          IRQSC					/*!<IRQ register */
#define MC13192_IRQ_IE_BIT          IRQSC_IRQIE		/*!<Enables IRQ pin */
#define MC13192_ANT_CTRL            PTCD_PTCD5		/*!<Control line 1 for antenna */
#define MC13192_ANT_CTRL2           PTCD_PTCD4		/*!<Control line 2 for antenna */
#define MC13192_ANT_CTRL_PORT       PTCDD_PTCDD5  /*!<Control line 1 pin direction */
#define MC13192_ANT_CTRL2_PORT      PTCDD_PTCDD4  /*!<Control line 2 pin direction */
#define ANT_CTRL_OFF                0							/*!<Antenna Control Logic low is off  */
#define ANT_CTRL_ON                 1							/*!<Antenna Control Logic low is on  */
  
#if defined (PA)
    #define MC13192_PA_CTRL_PORT    PTEDD_PTEDD1  /*!<PA control pin direction*/
    #define MC13192_PA_CTRL         PTED_PTED1		/*!<PA control */
    #define PA_ON                   1							/*!<PA control on */
    #define PA_OFF                  0							/*!<PA control off */
#endif 

//Defines
/* EVB */

/**************************************************************
Enable peripherials that are on the Board
**************************************************************/

#define SCI_PORT              1
#define ACCEL_ENABLED         FALSE
#define TEMP_SENSOR_ENABLED   FALSE
#define BUZZER_ENABLED        FALSE
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

#define LED1                    PTCD_PTCD6
#define LED1DIR                 PTCDD_PTCDD6
/* Fifth LED2-5 is not defined for I_BOARD2, set to Port F pin 6 for all */

#define LED2                    PTFD_PTFD6
#define LED2DIR                 PTFDD_PTFDD6
#define LED3                    PTFD_PTFD6
#define LED3DIR                 PTFDD_PTFDD6
#define LED4                    PTFD_PTFD6
#define LED4DIR                 PTFDD_PTFDD6
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
#define PB0IE KBI1PE_KBIPE4
  
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
    
                              
                              
#define ENTER_LOW_POWER _asm stop

/***********************************************
Duplicate PB0 to PUSH_BUTTON1 etc for different
formats.
***********************************************/

#define PUSH_BUTTON1 PB0
#define PUSH_BUTTON2 PB1
#define PUSH_BUTTON3 PB2
#define PUSH_BUTTON4 PB3
