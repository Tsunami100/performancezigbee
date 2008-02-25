/****************************************************************************
application smac_per_tx.c.
This is a basic PER test.  The TX portion of the application
sends out 1000 packets.

Author: Freescale

Application Note: 
\****************************************************************************/

#include <hidef.h> /* for EnableInterrupts macro */
#include "pub_def.h"
#include "APP_SMAC_API.h"
#include "freescale_radio_hardware.h"
#include "SCI.h"
#include "smac_per_tx.h"
#include "eventDefs.h"
#include "timer.h"
#include "ledFunctions.h"
#include "kbi.h"
#include "ascii_utilities.h"
#if SMAC_FEATURE_OTAP == TRUE
#include "APP_OTAP_API.h"
#endif // SMAC_FEATURE_OTAP == TRUE
#if SMAC_FEATURE_SECURITY == TRUE
#include "APP_security_API.h"
#endif // SMAC_FEATURE_SECURITY == TRUE
#if (EMBEDDED_BOOTLOADER == TRUE)  
#include "bootloader user api.h"
#endif

#if SMAC_FEATURE_OTAP == TRUE
#include "APP_OTAP_API.h"
#endif // SMAC_FEATURE_OTAP == TRUE

#if SMAC_FEATURE_SECURITY == TRUE
#include "APP_security_API.h"
#endif // SMAC_FEATURE_SECURITY == TRUE

/* Global Variables */
byte gu8RTxMode; /* needed for s-mac, application can read this variable */
				/* rtx_mode is the software state variable of the radio */
				/* This variable reflects the current state of MC13192 */
int app_status = 0; 
byte led_status = 0;
UINT8 gu8Channel = CHANNEL_NUMBER;
UINT16 gu16Events = 0;
extern UINT16 gu16msTimer;
extern UINT16 gu16timerEvent[2];

UINT32 packet_count = 0;
UINT32 packet_number = 0;
UINT32 packet_size = 0;
UINT32 packet_interval = 0;

void main(void) {

	tRxPacket rx_packet;  //SMAC structure for RX packets
	tTxPacket tx_packet;	//SMAC structure for TX packets
	byte rx_data_buffer[20];	//Data buffer to hold RX data, if you want larger packets change 20 to what you need.
	byte tx_data_buffer[20];	//Data buffer to hold TX data, if you want larger packets change 20 to what you need.
	UINT16 loop;
	UINT8 i;

   char App_String[10];
	
	//Initialize the packet.
  tx_packet.u8DataLength = 0;				//Set TX default to 0
  tx_packet.pu8Data = &tx_data_buffer[0];	//Load the address of our txbuffer into tx structure.
  rx_packet.u8DataLength = 0;				//Set RX default to 0
  rx_packet.pu8Data = &rx_data_buffer[0];	//Load the address of our rxbuffer into rx structure
  rx_packet.u8MaxDataLength = 100;		//Define the max buffer we are interested in.
  rx_packet.u8Status = 0;					//initialize the status packet to 0.
  

  
   MCUInit();		//Initialize the mcu, setup GPIOs, SPI, clock scheme, etc.
   RadioInit();
   KBIInit();
   
   KBI_ENABLEINTERRUPTS;  
   app_init();
	SCIInitGeneric(8000000, 38400, DEFAULT_SCI_PORT);	
  		
	(void)MLMESetMC13192ClockRate(0);  /* Set initial Clk speed from MC13192s CLKO*/
	UseExternalClock();	/* switch clock sources to EXTERNAL clock */
	
	/* include your start up code here */	
  EnableInterrupts; /* Turn on system interrupts */
	(void)MLMESetChannelRequest(CHANNEL_NUMBER);	//Set channel zero, can be changed to users preference
	
#if (EMBEDDED_BOOTLOADER == TRUE)
  boot_init();  //Initialize the bootloader...in bootloader user api.c
#endif 
	
  /******************************************************************
  To adjust output power call the MLME_MC13192_PA_output_adjust() with:
  
  MAX_POWER     (+3 to +5dBm)
  NOMINAL_POWER (0 dBm)
  MIN_POWER     ~(-16dBm)
  
  or somewhere custom ? (0-15, 11 (NOMINAL_POWER) being Default power)

	******************************************************************/
  //MLMEMC13192PAOutputAdjust(MAX_POWER);	//Set MAX power setting
  //MLMEMC13192PAOutputAdjust(MIN_POWER);	//Set MIN power setting
  (void)MLMEMC13192PAOutputAdjust(OUTPUT_POWER);	//Set Nominal power setting	
  
  
  
  #if (EMBEDDED_BOOTLOADER == TRUE)
      boot_call();  //Checks for user request for bootloader.
                    //Runs bootloader if requested.  Code will not return.
  #endif 
  
  app_status = INITIAL_STATE;		//Set the initial app_status of the application state variable to INITIAL_STATE
		
	SCITransmitStr("\n\n\n\r\rFirmware para testes de PER.\n\r");
	SCITransmitStr("\rOperando no canal ");
  (void)int2string((UINT32) gu8Channel, App_String);
	(void)trimWhiteSpace(App_String);
	SCITransmitStr(App_String);	
	SCITransmitStr("\n\r");        	      
		
  //Loop forever
	for (;;) {

		switch (app_status) {
			
			case INITIAL_STATE:		//Walk the LEDs
			
				//For TX
				LED1 = LED_OFF;		//Turn off all LEDs
				LED2 = LED_OFF;
				LED3 = LED_OFF;
				LED4 = LED_OFF;
				
				LED1 = LED_ON;									//Lights LED1
				for (loop = 0; loop < LED_DELAY; loop++);		
				LED1 = LED_OFF; LED2 = LED_ON;					//Lights LED2, Turns off LED1
				for (loop = 0; loop < LED_DELAY; loop++);
				LED2 = LED_OFF; LED3 = LED_ON;					//Lights LED3, Turns off LED2
				for (loop = 0; loop < LED_DELAY; loop++);
				LED3 = LED_OFF; LED4 = LED_ON;					//Lights LED4, Turns off LED3
				for (loop = 0; loop < LED_DELAY; loop++);
				LED4 = LED_OFF;									//Turns off LED4
				LED1 = LED_ON; 

				packet_count=0;
		    
		    SCITransmitStr("\n\r\n\rIniciando Transmissor. Aguardando pacote de START...");		     		      
		    
				app_status = RX_STATE;	//Switch app status to TX_STATE
				
        break;
			case RX_STATE:
			
				MLMERXEnableRequest(&rx_packet,0); //Zero means wait forever with RX ON.
				
				app_status = RECEIVER_ALWAYS_ON;
				
				break;
			case RECEIVER_ALWAYS_ON:
			
				break;
		  case START_RECEIVED:
		  
			  SCITransmitStr("\n\rPacote de START recebido.");
			  
			  SCITransmitStr("\n\rNumero de pacotes:");
			  (void)int2string((UINT32) packet_number, App_String);
			  (void)trimWhiteSpace(App_String);
			  SCITransmitStr(App_String);

			  SCITransmitStr("\n\rTamanho dos pacotes:");
			  (void)int2string((UINT32) packet_size, App_String);
			  (void)trimWhiteSpace(App_String);
			  SCITransmitStr(App_String);

			  SCITransmitStr("\n\rIntervalo entre os pacotes:");
			  (void)int2string((UINT32) packet_interval, App_String);
			  (void)trimWhiteSpace(App_String);
			  SCITransmitStr(App_String);

				for (i=0; i<(packet_size-2); i++) {
				   tx_data_buffer[i] = 0xAA; // 10101010
				}
				tx_packet.u8DataLength = (packet_size-2);
				
        app_status = TX_STATE;
        
        break;
			case TX_STATE:		 
				//Load the tx buffer with the ZigBee packet.
				
				RAMDrvWriteTx(&tx_packet);
        
			  SCITransmitStr("\n\ri\n\r");
			  
			  for (loop=0;loop<packet_number;loop++) {
			    
			  
				  if (MCPSDataRequestWithoutLoad() == SUCCESS)	//TX Packet, Blocking will stay here until the packet is in the air
  				  LED1 ^=1;
				
          //packet_count++;
				
				  //MLME_RX_enable_request(&rx_packet,DELAY_COUNT);	//Now turn on the RX with a timeout of DELAY_COUNT
          delaySymbols(packet_interval);
          
			  }
		   
			  SCITransmitStr("\n\rf\n\r");
			  
				//if (packet_count < packet_number)
				//  app_status = DELAY_STATE ; 	//Set app_status as waiting for ack. 
			  //else
				  app_status = FINAL_STATE;				
				
				break;
				
			case RESET_STATE:
				//MC13192 Reset, reinitialize and return to default state (TX_STATE).
				RadioInit();
				app_status = INITIAL_STATE;
				break;
				
			case SET_LEDS:
				
				/* This state takes the power value from the ack and turns on the appropriate LEDs */
				
				LED1 ^= 1;	/* Toggle LED1 whenever an ack is received. (blinks LED1) */
				switch (led_status) {
					case 0:
						LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_OFF; //At spec (-88)
						break;
					case 1:
						LED2 = LED_ON; LED3 = LED_OFF; LED4 = LED_OFF; //spec < reading < -80
						break;
					case 2:
						LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_OFF; //spec < reading < -60
						break;
					case 3:
						LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_ON; //reading < -40
						break;
					default:
					break;
				}
				//app_status = DELAY_STATE;	//Take the app state and wait for some time to retransmit.
				app_status = TX_STATE;	//After the delay, retransmit
				break;
				
			case DELAY_STATE:
				//Sets the TX rate, small DELAY_COUNT means TX rate is fast while larger values slow it down.
        delaySymbols(packet_interval/2);
				app_status = TX_STATE;	//After the delay, retransmit
 				break;
 				
 		   case IDLE_STATE:
		      break;
		      
 			
		   case FINAL_STATE:

		      LED3 = LED_ON;
		      
		      tx_packet.pu8Data[0] = 'D';
		      tx_packet.pu8Data[1] = 'O';
		      tx_packet.pu8Data[2] = 'N';
		      tx_packet.pu8Data[3] = 'E';
		      tx_packet.pu8Data[4] = 'D';
		      tx_packet.pu8Data[5] = 'O';
		      tx_packet.pu8Data[6] = 'N';
		      tx_packet.pu8Data[7] = 'E';
		      tx_packet.pu8Data[8] = '\0';
		      tx_packet.u8DataLength = 9;
  		      
          (void)MLMEMC13192PAOutputAdjust(14);	//Set Nominal power setting	
          
		      for (i=0; i<20; i++) {
		         for (loop = 0; loop < DELAY_COUNT; loop++);
		         (void)MCPSDataRequest(&tx_packet);
		         app_status = FINAL_STATE;
		      }
		      
		      LED1 = LED_OFF;
		      
		      app_status = INITIAL_STATE;
		      
			default:
				break;
			
		}
		
		/* Check for events */
		  
		  /* Check for debounce timer (TIMER1) */
        if ((gu16Events & TIMER_EVENT1) != 0) {
            gu16Events &= ~TIMER_EVENT1; /* Clear the event */
            gu16timerEvent[1] = 0; /* Turn off the timer */
        }
       
        /* See if a RX ON Request has been made */	      
        if ((gu16Events & KBI2_EVENT) != 0) {
            #if BUZZER_ENABLED
               BUZZER = BUZZER_ON;
            #endif
               delay(10);
            #if BUZZER_ENABLED
               BUZZER = BUZZER_OFF;
            #endif
               gu16Events &= ~KBI2_EVENT; /* Clear the event */
        	      
        	      KBI1SC |= 0x02; /* Enable Interrupts */
           
           // Enviar o pacote para o TX com DONEDONE na potencia maxima
           MLMERXDisableRequest();
           
          app_status = FINAL_STATE;
        }       
	}
}


void MCPSDataIndication(tRxPacket *rx_packet) {
		       
	if (rx_packet->u8Status == TIMEOUT) {

		/* Put timeout condition code here */
		
		app_status = TX_STATE;	//If timeout occurs, simply set back to TX_STATE to transmit again.

	}

	if (rx_packet->u8Status == SUCCESS) {	//Good packet received.

		//Check to see if it is an "ACKx" packet....
		if (rx_packet->pu8Data[0] == 'A' &&
			rx_packet->pu8Data[1] == 'C' &&
			rx_packet->pu8Data[2] == 'K') {
			
			//Good ACK packet read the power level byte
			led_status = rx_packet->pu8Data[3];	//Store power level into global led_status
			app_status = SET_LEDS;				//Set app_status to process the LEDs
			
		}
		else {
		if (rx_packet->pu8Data[0] == 'S' &&
			rx_packet->pu8Data[1] == 'T' &&
			rx_packet->pu8Data[2] == 'A' &&
			rx_packet->pu8Data[3] == 'R' &&
			rx_packet->pu8Data[4] == 'T') {
			// Recebeu o pacote de START

        UINT8 quantidadePkt[6];
        UINT8 tamanhoPkt[4];
        UINT8 intervaloPkt[4];
        UINT8 loop;
        
 		    for (loop = 0; loop < 6; loop++)
 		      quantidadePkt[loop] = 0;
 		    for (loop = 0; loop < 4; loop++)
 		      tamanhoPkt[loop] = 0;
 		    for (loop = 0; loop < 4; loop++)
 		      intervaloPkt[loop] = 0;
			
        quantidadePkt[0] = rx_packet->pu8Data[5];
        quantidadePkt[1] = rx_packet->pu8Data[6];
        quantidadePkt[2] = rx_packet->pu8Data[7];
        quantidadePkt[3] = rx_packet->pu8Data[8];
        quantidadePkt[4] = rx_packet->pu8Data[9];

        tamanhoPkt[0] = rx_packet->pu8Data[10];
        tamanhoPkt[1] = rx_packet->pu8Data[11];
        tamanhoPkt[2] = rx_packet->pu8Data[12];
        
        intervaloPkt[0] = rx_packet->pu8Data[13];
        intervaloPkt[1] = rx_packet->pu8Data[14];
        intervaloPkt[2] = rx_packet->pu8Data[15];
        
			  packet_number = String2Uint32(quantidadePkt);
			  packet_size = String2Uint32(tamanhoPkt);
			  packet_interval = String2Uint32(intervaloPkt);

        app_status = START_RECEIVED;	
				LED4 = LED_ON;
			} else {
			  
			//A good packet was received but it is not the ack packet we want.
			//Whenever a good packet is received the radio will default back to idle, so
			//retransmit by setting state variable to TX_STATE.  This resets the machine.
			app_status = TX_STATE;
			}
		}
	}		

}

void MLMEMC13192ResetIndication() {
	//Notifies you that the MC13192 has been reset.
	//Application must handle this here.

	app_status = RESET_STATE;	//MC13192 reset, re-initialize.

}

void app_init() {
  
  
  /* Init LED's 1-OFF 0-ON */
	LED1 = 1; /* Default is off */
	LED2 = 1;
	LED3 = 1;
	LED4 = 1;
	
	LED1DIR = 1; /*Set the DDIR register as Outputs for the LED pins*/
	LED2DIR = 1;
	LED3DIR = 1;
	LED4DIR = 1;
	
	/* Set Push button pull ups */
	PB0PU = 1;
	PB1PU = 1;
	PB2PU = 1;
	PB3PU = 1;

  /* OTAP entry */
  #if SMAC_FEATURE_OTAP == TRUE
  if (PB0 == 0){
    KBI1SC = 0x00; /* Turn On Interrupts */
    OTAPActivate();
  }
  #endif // SMAC_FEATURE_OTAP == TRUE
  
   /* Setup Timer 1 */
     /* Setup Timer 1 */
  TPM1MOD = 1; /* Set the timer for a 26ms timer rate (1/8E6) * 128 * 625 = 10msec */
                 /* Timer rate = (1/BUSCLOCK) * TIMER_PRESCALER * TIMER MOD */
  TPM1SC = 0x4F; /* Timer divide by 128. (16uS timebase for 8MHz bus clock). interrupt on*/
	
}