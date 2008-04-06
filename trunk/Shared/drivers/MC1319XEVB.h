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
void RadioInit(void);

//Defines


#define HCS08G																	 /*!<HCS08G family member*/
#define MC13192_CE              PTED_PTED2       /*!<Enables SPI transfers */         
#define MC13192_CE_PORT         PTEDD_PTEDD2     /*!<SPI enable pin data direction */           
#define MC13192_ATTN            PTCD_PTCD2			 /*!<Transitions IC from either Hibernate 
                                                     or Doze Modes to Idle. */
#define MC13192_ATTN_PORT       PTCDD_PTCDD2		 /*!<ATTN pin data direction */
#define MC13192_RTXEN           PTCD_PTCD3			 /*!<Enables Transmission or Reception from SPI  */
#define MC13192_RTXEN_PORT      PTCDD_PTCDD3		 /*!<SPI Transmission pin direction */
#define MC13192_RESET           PTCD_PTCD4			 /*!<Transciever Reset */
#define MC13192_RESET_PORT      PTCDD_PTCDD4		 /*!<Transciever Reset pin data direction */
#define MC13192_RESET_PULLUP    PTCPE_PTCPE4		 /*!<Enables pull up for Reset */
#define MC13192_IRQ_SOURCE      IRQSC						 /*!<IRQ register */
#define MC13192_IRQ_IE_BIT      IRQSC_IRQIE			 /*!<Enables IRQ pin */

#ifdef ANTENNA_SWITCH
    #define MC13192_ANT_CTRL        PTBD_PTBD6	 /*!<Control line 1 for antenna */
    #define MC13192_ANT_CTRL2       PTBD_PTBD6   /*!< 
                                                  * Second Control line not
                                                  * used in MC13192EVB
                                                  */
    #define MC13192_ANT_CTRL_PORT   PTBDD_PTBDD6 /*!<Control line 1 pin direction */
    #define MC13192_ANT_CTRL2_PORT  PTBDD_PTBDD6 /*!<Control line 1 pin direction */
    #define ANT_CTRL_OFF            0       /*!<Control line Logic low is off */
    #define ANT_CTRL_ON             1       /*!<Control line Logic high is on */
#endif ANTENNA_SWITCH

#ifdef LNA
    #define MC13192_LNA_CTRL        PTBD_PTBD0		/*!<LNA Control*/
    #define MC13192_LNA_CTRL_PORT   PTBDD_PTBDD0  /*!<LNA control pin direction*/
    #define LNA_ON                  1							/*!<Turns on LNA*/
    #define LNA_OFF                 0							/*!<Turns of LNA*/
#endif LNA  

/**************************************************************
Enable peripherials that are on the Board
**************************************************************/
#define ACCEL_ENABLED         FALSE
#define TEMP_SENSOR_ENABLED   FALSE
#define BUZZER_ENABLED        FALSE
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
   
   #define  RTS				PTAD_PTAD1
   #define  RTSDIR		  	PTADD_PTADD1

#else 

   #define  SCIBDH     SCI2BDH
   #define  SCIBDL     SCI2BDL
   #define  SCIC1      SCI2C1 
   #define  SCIC2      SCI2C2 
   #define  SCIS1      SCI2S1 
   #define  SCIS2      SCI2S2 
   #define  SCIC3      SCI2C3 
   #define  SCID       SCI2D

   #define  SCIS1_TDRE  SCI2S1_TDRE
   #define  SCIS1_TC    SCI2S1_TC

   #define  RTS				PTAD_PTAD7
   #define  RTSDIR		  	PTADD_PTADD7
#endif 
#define  SCI_DEFAULT_BAUD  38400

/**************************************************************
Define the LED perameters
**************************************************************/

#define LED1                    PTDD_PTDD0
#define LED1DIR                 PTDDD_PTDDD0
#define LED2                    PTDD_PTDD1
#define LED2DIR                 PTDDD_PTDDD1
#define LED3                    PTDD_PTDD3
#define LED3DIR                 PTDDD_PTDDD3
#define LED4                    PTDD_PTDD4
#define LED4DIR                 PTDDD_PTDDD4

/* LED is not implemented on the 13192EVB, using PTCD2 */

#define LED5						  PTCD_PTCD2
#define LED5DIR                 PTCDD_PTCDD2
    
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
Define Switches
***********************************************/
    
    
/* SW1 on schematic */
#define PB0 PTAD_PTAD2
#define PB0PU PTAPE_PTAPE2
#define PB0DIR PTADD_PTADD2
#define PB0IE KBI1PE_KBIPE2
  
/* SW2 on schematic */
#define PB1 PTAD_PTAD3
#define PB1PU PTAPE_PTAPE3
#define PB1DIR PTADD_PTADD3
#define PB1IE KBI1PE_KBIPE3
   
/* SW3 on schematic */
#define PB2 PTAD_PTAD4
#define PB2PU PTAPE_PTAPE4
#define PB2DIR PTADD_PTADD4
#define PB2IE KBI1PE_KBIPE4
  
/* SW4 on schematic */
#define PB3 PTAD_PTAD5
#define PB3PU PTAPE_PTAPE5
#define PB3DIR PTADD_PTADD5
#define PB3IE KBI1PE_KBIPE5

/* Define the KBI Pins */
#define KBI_SW1 0x04
#define KBI_SW2 0x08
#define KBI_SW3 0x10
#define KBI_SW4 0x20

#define SWITCH_INIT_MACRO    PB0PU = TRUE; \
                             PB1PU = TRUE; \
                             PB2PU = TRUE; \
                             PB3PU = TRUE; \
                             PB0DIR = FALSE;\
                             PB1DIR = FALSE;\
                             PB2DIR = FALSE;\
                             PB3DIR = FALSE;
                                  
#define ENTER_LOW_POWER _asm stop

/***********************************************
Duplicate PB0 to PUSH_BUTTON1 etc for different
formats.
***********************************************/

#define PUSH_BUTTON1 PB0
#define PUSH_BUTTON2 PB1
#define PUSH_BUTTON3 PB2
#define PUSH_BUTTON4 PB3

#define PRESSED 0
#define PB_PRESSED 0