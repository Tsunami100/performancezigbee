/* toro_gen_header.h 
 * toro header for test verification

 * $Author: a20639 $
 * $Date: 2006/08/25 14:55:00 $
 * $Name:  $
 */
 
//includes
#include "pub_def.h"
#include "board_config.h"
/* Prototypes */
void RadioInit(void);



#define HCS08G																/*!<HCS08G family member*/
#define MC13192_CE              PTED_PTED2    /*!<Enables SPI transfers*/              
#define MC13192_CE_PORT         PTEDD_PTEDD2  /*!<SPI enable pin data direction */              
#define MC13192_ATTN            PTDD_PTDD5		/*!<Transitions IC from either Hibernate 
                                                  or Doze Modes to Idle.  */
#define MC13192_ATTN_PORT       PTDDD_PTDDD5  /*!<ATTN pin data direction  */
#define MC13192_RTXEN           PTDD_PTDD6		/*!<Enables Transmission or Reception from SPI  */
#define MC13192_RTXEN_PORT      PTDDD_PTDDD6  /*!<SPI Transmission pin direction */
#define MC13192_RESET           PTDD_PTDD7		/*!<Transciever Reset */
#define MC13192_RESET_PORT      PTDDD_PTDDD7  /*!<Transciever Reset pin data direction */
#define MC13192_RESET_PULLUP    PTDPE_PTDPE7  /*!<Enables pull up for Reset */
#define MC13192_IRQ_SOURCE      IRQSC					/*!<IRQ register */
#define MC13192_IRQ_IE_BIT 	    IRQSC_IRQIE			 /*!<Enables IRQ pin */


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
#if (DEFAULT_SCI_PORT == 1)
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
   
   #define  RTS			   PTFD_PTFD7
   #define  RTSDIR		 PTFDD_PTFDD7
   
#else 

   #define  SCIBDH     SCI2BDH
   #define  SCIBDL     SCI2BDL
   #define  SCIC1      SCI2C1 
   #define  SCIC2      SCI2C2 
   #define  SCIS1      SCI2S1 
   #define  SCIS2      SCI2S2 
   #define  SCIC3      SCI2C3 
   #define  SCID       SCI2D
   
   #define  RTS			PTFD_PTFD7
   #define  RTSDIR		PTFDD_PTFDD7
   
   #define  SCIS1_TDRE  SCI2S1_TDRE
   #define  SCIS1_TC    SCI2S1_TC

#endif 
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
    
#define LED_ON                  0
#define LED_OFF					1
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
    
    
#define BUZZER         PTDD_PTDD0
#define BUZZERDIR      PTDDD_PTDDD0
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
#define PB1IE KBI1PE_KBIPE5
   
/* SW3 on schematic */
#define PB2 PTAD_PTAD6
#define PB2PU PTAPE_PTAPE6
#define PB2DIR PTADD_PTADD6
#define PB2IE KBI1PE_KBIPE6
  
/* SW4 on schematic */
#define PB3 PTAD_PTAD7
#define PB3PU PTAPE_PTAPE7
#define PB3DIR PTADD_PTADD7
#define PB3IE KBI1PE_KBIPE7

/* Define the KBI switch bit positions */
#define KBI_SW1 0x10
#define KBI_SW2 0x20
#define KBI_SW3 0x40
#define KBI_SW4 0x80

#define SWITCH_INIT_MACRO    PB0PU = TRUE; \
                             PB1PU = TRUE; \
                             PB2PU = TRUE; \
                             PB3PU = TRUE; \
                             PB0DIR = FALSE;\
                             PB1DIR = FALSE;\
                             PB2DIR = FALSE;\
                             PB3DIR = FALSE;
/***********************************************
LCD Settings
***********************************************/
                             
#define LCD_ENABLE        /* None Dip Switch */
#define LCD_REG_SEL       PTED_PTED6
#define LCD_EN            PTED_PTED7
#define LCD_RW            PTGD_PTGD3
    /* Data bits are 4-7 */
#define LCD_DATA1         PTGD_PTGD4
#define LCD_DATA2         PTGD_PTGD5
#define LCD_DATA3         PTGD_PTGD6
#define LCD_DATA4         PTGD_PTGD7
#define LCD_INIT_MACRO    PTGDD = 0xF8; \
                          PTEDD |= 0xC0;
                                           
#define ENTER_LOW_POWER _asm stop

/***********************************************
Duplicate PB0 to PUSH_BUTTON1 etc for different
formats.
***********************************************/

#define PUSH_BUTTON1 PB0
#define PUSH_BUTTON2 PB1
#define PUSH_BUTTON3 PB2
#define PUSH_BUTTON4 PB3

#define PB_PRESSED 0
