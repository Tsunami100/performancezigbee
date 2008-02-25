/****************************************************************************
application rx_per.c

Author: MOTOROLA

Application Note: 
RX_per waits for a ZigBee packet to be received.
Then the link quality of the ZigBee packet is measured by reading the link quality register.
A comparison is made between SPEC_POWER L1_POWER, L2_POWER, and L3_POWER.
From this comparison a ACK packet is formed {'A','C','K', x = {0,1,2,3}} where x represents the respective power settings.
After the ACK is sent the RADIO returns to RX forever mode.
****************************************************************************/

#include <hidef.h> /* for EnableInterrupts macro */
#include "pub_def.h"
#include "LCD.h"
#include "SCI.h"
#include "APP_SMAC_API.h"
#include "freescale_radio_hardware.h"
#include "smac_per_rx.h"
#include "eventDefs.h"
#include "timer.h"
#include "ledFunctions.h"
#include <string.h>
#include "kbi.h"
#include "ascii_utilities.h"
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
int app_status = 0;	//State machine variable for the application	
byte led_status = 0;	//led status reflects which power setting should be set.
UINT8 link_quality;	//Holds the link quality of the last received ZigBee Packet.
extern char SCIdata[2];
extern int SCIdata_flag;
UINT8 gu8Channel = CHANNEL_NUMBER;
UINT16 gu16Events = 0;
UINT8 expectedString[30];
extern UINT16 gu16msTimer;
extern UINT16 gu16timerEvent[2];

UINT32 packet_count = 0;
UINT32 packet_number = 0;
UINT32 packet_size = 0;
UINT32 packet_interval = 0;
UINT32 packet_repeat = 0;

UINT32 packet_histogramy[10];


	
void main(void) {

   tRxPacket rx_packet;	//SMAC structure for RX packets
   tTxPacket tx_packet;	//SMAC structure for TX packets
   byte rx_data_buffer[20];	//Data buffer to hold RX data, if you want larger packets change 20 to what you need.
   byte tx_data_buffer[20];	//Data buffer to hold TX data, if you want larger packets change 20 to what you need.
   UINT32 loop, i;
   UINT8 quantidadePkt[6];
   UINT8 tamanhoPkt[4];
   UINT8 intervaloPkt[4];
   UINT8 repetirPkt[4];
   

   char App_String[10];
	
	//EnableInterrupts; /* enable interrupts */
	/* include your code here */
	/* Init LED's */



	//Initialize the packet.
  tx_packet.u8DataLength = 0;		//Set TX default length to 0
  tx_packet.pu8Data = &tx_data_buffer[0];	//Load the address of our txbuffer into the tx structure
  rx_packet.u8DataLength = 0;		//Set RX default length to 0
  rx_packet.pu8Data = &rx_data_buffer[0];	//Load the address of our rxbuffer into the rx structure
  rx_packet.u8MaxDataLength = 128;		//Define the max buffer we are interested in.
  rx_packet.u8Status = 0;					//initial status variable for rx to 0.
  MCUInit();			//Initialize the mcu, setup GPIOs, SPI, clock scheme, etc.
  RadioInit();
  
  app_init();
	SCIInitGeneric(8000000, 38400, DEFAULT_SCI_PORT);	
   KBIInit();
   
   KBI_ENABLEINTERRUPTS;
   
	(void)MLMESetMC13192ClockRate(0);  /* Set initial Clk speed */
	UseExternalClock();	/* switch clock sources */
	
	
	/* include your start up code here */	
  EnableInterrupts; /* Turn on system interrupts */
	(void)MLMESetChannelRequest(CHANNEL_NUMBER);	//Set channel to 0 (2.405GHz)		

#if (EMBEDDED_BOOTLOADER == TRUE)
  boot_init();  //Initialize the bootloader...in bootloader user api.c
#endif

  /******************************************************************
  To adjust output power call the MLMEMC13192PAOutputAdjust() with:
  
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

  #if LCD_ENABLED
    LCDInit(); /* Optional LCD display initialization */
    LCDClearDisplay();
  #endif
   /* Indicate RX radio out of reset */
   setLedsMode(LED_HEX_MODE, 0xF, 30, LED_NO_FLAGS);
   setLedsMode(LED_HEX_MODE, 0xF, 30, LED_NO_FLAGS);
   
   /* Set up the expected string */
   for (loop=0; loop<20; loop++) {
      expectedString[loop] = (UINT8)loop;
   }
   expectedString[20] = '\0'; /* Terminate the string */
   
	
	app_status = INITIAL_STATE;
	
	SCITransmitStr("\n\n\n\r\rFirmware para testes de PER.\n\r");
	SCITransmitStr("\rOperando no canal ");
  (void)int2string((UINT32) gu8Channel, App_String);
	(void)trimWhiteSpace(App_String);
	SCITransmitStr(App_String);	
	SCITransmitStr("\n\r");        	      
		
  #if LCD_ENABLED
    LCDWriteString( 1, "Firmware TESTE" );
    LCDWriteString( 2, "802.15.4" );
  #endif
	//Loop forever
	for (;;) {
	
	   
	   

		/* include your regular code here */
		switch (app_status) {
			case INITIAL_STATE:
			
				  // Estado inicial
				
				  LED1 = LED_ON; LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_ON;		//All LEDs On
				  for (loop = 0; loop < LED_DELAY; loop++);

				  LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_OFF; //All LEDs Off
				  for (loop = 0; loop < LED_DELAY; loop++);

				  LED1 = LED_ON; LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_ON;		//All LEDs On
				  for (loop = 0; loop < LED_DELAY; loop++);	

				  LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_OFF;	//All LEDs Off
				  for (loop = 0; loop < LED_DELAY; loop++);

				  app_status = INPUT_VALUE;

				break;
								
 		  case INPUT_VALUE:
 		  
 		      for (loop = 0; loop < 6; loop++)
 		        quantidadePkt[loop] = 0;
 		      for (loop = 0; loop < 4; loop++)
 		        tamanhoPkt[loop] = 0;
 		      for (loop = 0; loop < 4; loop++)
 		        intervaloPkt[loop] = 0;
 		      for (loop = 0; loop < 4; loop++)
 		        repetirPkt[loop] = 0;
 		  
 		      LED1 = LED_ON;
 		      
          (void)SCIgets("\r\nEntre com o numero de pacotes:\r\n", &quantidadePkt[0],5);

 		      LED2 = LED_ON;
 		      
          (void)SCIgets("\r\nEntre com o tamanho dos pacotes(bytes):\r\n", &tamanhoPkt[0],3);
		  
 		      LED3 = LED_ON;
 		      
          (void)SCIgets("\r\nEntre com o intervalo entre os pacotes(simbolos):\r\n", &intervaloPkt[0],3);
		  
 		      LED4 = LED_ON;
 		      
 		      (void)SCIgets("\r\nEntre com quantas vezes o teste vai ser executado:\r\n", &repetirPkt[0],3);
 		      
 		      
			    SCITransmitStr("\r\nNumero de pacotes:");
			    SCITransmitStr(quantidadePkt);
			    
			    packet_number = String2Uint32(quantidadePkt);

			    SCITransmitStr("\r\nTamanho dos pacotes:");
			    SCITransmitStr(tamanhoPkt);
			    
			    packet_size = String2Uint32(tamanhoPkt);

			    SCITransmitStr("\r\nIntervalo entre os pacotes:");
			    SCITransmitStr(intervaloPkt);
			    
			    packet_interval = String2Uint32(intervaloPkt);
			    
			    SCITransmitStr("\r\nExecutar o testes:");
			    SCITransmitStr(repetirPkt);
			    
			    packet_repeat = String2Uint32(repetirPkt);
			    
			    if (packet_repeat < 1) {
			      packet_repeat = 1;
			    }
			    
				  app_status = WAITING_START;

		    break;
		  case WAITING_START:
		  
          (void)SCIgets("\r\nTecle <ENTER> para iniciar o teste:", &App_String,1);
        
				  app_status = SEND_START;

		    break;
		  case SEND_START:
          SCITransmitStr("\r\nIniciando o teste numero ");
          (void)int2string((UINT32) packet_repeat, App_String);
          (void)trimWhiteSpace(App_String);
          SCITransmitStr(App_String);	
          SCITransmitStr(".");        	      
          
		      SCITransmitStr("\r\nEnviando pacote de START...");		     

    	    tx_data_buffer[0] = 'S';
				  tx_data_buffer[1] = 'T';
				  tx_data_buffer[2] = 'A';
				  tx_data_buffer[3] = 'R';
				  tx_data_buffer[4] = 'T';
				  
				  tx_data_buffer[5] = (char)quantidadePkt[0];
				  tx_data_buffer[6] = (char)quantidadePkt[1];
				  tx_data_buffer[7] = (char)quantidadePkt[2];
				  tx_data_buffer[8] = (char)quantidadePkt[3];
				  tx_data_buffer[9] = (char)quantidadePkt[4];

				  tx_data_buffer[10] = (char)tamanhoPkt[0];
				  tx_data_buffer[11] = (char)tamanhoPkt[1];
				  tx_data_buffer[12] = (char)tamanhoPkt[2];

				  tx_data_buffer[13] = (char)intervaloPkt[0];
				  tx_data_buffer[14] = (char)intervaloPkt[1];
				  tx_data_buffer[15] = (char)intervaloPkt[2];
				  
				  tx_packet.u8DataLength = 16;
			    packet_count = 0;

			    for(loop=0;loop<10;loop++) {
			      packet_histogramy[loop] = 0;
			    }

				  if (MCPSDataRequest(&tx_packet) == SUCCESS) {
				    SCITransmitStr("\r\nPacote de START enviado com sucesso.");
					  LED1 = LED_OFF;
					  LED2 = LED_OFF;
					  LED3 = LED_OFF;
					  LED4 = LED_ON;
				  } else {
				    SCITransmitStr("\r\nErro ao enviar pacote de START.");
				  }
        		  
				  app_status = RX_STATE;
        
				break;
				
			case RX_STATE:
			  
				MLMERXEnableRequest(&rx_packet,0); //Zero means wait forever with RX ON.
  				
				app_status = WAITING_FOR_ACK;
	  			
				break;
			case WAITING_FOR_ACK:
			
				break;
			
			case GOOD_PACKET:
			   LED1 ^= 1;
			   
         link_quality = (UINT8)(MLMELinkQuality()/2);	//Read the link quality of the last received packet. 
         
         // Histograma do RSSI
         // 0 : < 40 db
         // 1 : 40 db - 45 db
         // 2 : 45 db - 50 db
         // 3 : 50 db - 55 db
         // 4 : 55 db - 60 db
         // 5 : 60 db - 65 db
         // 6 : 65 db - 70 db
         // 7 : 70 db - 75 db
         // 8 : 75 db - 80 db
         // 9 : 80 db - 85 db
         if (link_quality < 40) {
          packet_histogramy[0] ++;
         } else if (link_quality < 45) {
          packet_histogramy[1] ++;
         } else if (link_quality < 50) {
          packet_histogramy[2] ++;
         } else if (link_quality < 55) {
          packet_histogramy[3] ++;
         } else if (link_quality < 60) {
          packet_histogramy[4] ++;
         } else if (link_quality < 65) {
          packet_histogramy[5] ++;
         } else if (link_quality < 70) {
          packet_histogramy[6] ++;
         } else if (link_quality < 75) {
          packet_histogramy[7] ++;
         } else if (link_quality < 80) {
          packet_histogramy[8] ++;
         } else {
          packet_histogramy[9] ++;
         }
/*			   

          // Nao imprimir dados do pacotes pois deixa lento 
          // e alguns pacotes nao sao processados.

			    SCITransmitStr("\n\rN:");
			   (void)int2string((UINT32) packet_count, App_String);
			   (void)trimWhiteSpace(App_String);
			   SCITransmitStr(App_String);
				
				SCITransmitStr(" L:");
			   (void)int2string((UINT32) rx_packet.u8DataLength, App_String);
			   (void)trimWhiteSpace(App_String);
			   SCITransmitStr(App_String);

            link_quality = (UINT8)(MLMELinkQuality()/2);	//Read the link quality of the last received packet.  
            SCITransmitStr(" LQI=-");
			   (void)int2string((UINT32) link_quality, App_String);
			   (void)trimWhiteSpace(App_String);
			   SCITransmitStr(App_String);				
			   	
          //SCITransmitStr(" CRC=1 Data=");
          //SCITransmitFormattedArray(rx_packet.pu8Data, rx_packet.u8DataLength);
          //SCITransmitStr("\r");
*/          
            packet_count++;
          
            app_status = RX_STATE;
            
            break;
            
		   case ECHO_STATE:
			
					LED4 = LED_OFF;
					
          link_quality = (UINT8)(MLMELinkQuality()/2);	//Read the link quality of the last received packet.  
          
		      SCITransmitStr("\r\nGood:");
			    (void)int2string((UINT32) (packet_count), App_String);
			    (void)trimWhiteSpace(App_String);
			    SCITransmitStr(App_String);
			    SCITransmitStr("/");
				  (void)int2string((UINT32) (packet_number), App_String);//SCITransmitStr("/TEST_NUM\r\r");
				  (void)trimWhiteSpace(App_String);
			    SCITransmitStr(App_String);
			    
		      //SCITransmitStr("\n\rUltimo LQI recebido:-");
			    //(void)int2string((UINT32) link_quality, App_String);
			    //(void)trimWhiteSpace(App_String);
			    //SCITransmitStr(App_String);				

		      SCITransmitStr("\r\nHistograma RSSI");
			    for(loop=0;loop<10;loop++) {
  		      SCITransmitStr("\r\n[-");
	  		    (void)int2string((UINT32) (40+(loop*5)), App_String);
		  	    (void)trimWhiteSpace(App_String);
			      SCITransmitStr(App_String);				
  		      SCITransmitStr("]:");
	  		    (void)int2string((UINT32) packet_histogramy[loop], App_String);
		  	    (void)trimWhiteSpace(App_String);
			      SCITransmitStr(App_String);				
			    }

				  
            app_status = FINAL_STATE;
          
				break;
				
		   case FINAL_STATE:
		   
		      packet_repeat --;
		      
		      if (packet_repeat > 0) {
		        for(i=0;i<5;i++)
		          delaySeconds(1);
		        app_status = SEND_START;
		        break;
		      }
		      
		         for (i=0; i<strlen(App_String); i++) {
		            switch (App_String[i]) {
		              case '0':
		                  LED1 = LED_ON; LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_ON;
		                  break;
		              case '1':
		                  LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_ON;
		                  break;
		              case '2':
		                  LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_ON; LED4 = LED_OFF;
		                  break;
		              case '3':
		                  LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_ON; LED4 = LED_ON;
		                  break;
		              case '4':
		                  LED1 = LED_OFF; LED2 = LED_ON; LED3 = LED_OFF; LED4 = LED_OFF;
		                  break;
		              case '5':
		                  LED1 = LED_OFF; LED2 = LED_ON; LED3 = LED_OFF; LED4 = LED_ON;
		                  break;
		              case '6':
		                 LED1 = LED_OFF; LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_OFF;
		                  break;
		              case '7':
		                  LED1 = LED_OFF; LED2 = LED_ON; LED3 = LED_ON; LED4 = LED_ON;
		                  break;
		              case '8':
		                  LED1 = LED_ON; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_OFF;
		                  break;   
		              case '9':
		                  LED1 = LED_ON; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_ON;
		                  break;   
		              
		              default:
		               break;     
		            }
		            
		            delay(30); /* 30 msecs */
		           LED1 = LED_OFF; LED2 = LED_OFF; LED3 = LED_OFF; LED4 = LED_OFF;
		            delay(30); /* 30 msecs */ 
		         }
		         
		         //Larger delay
		         delay(50);
		      
	  			app_status = INITIAL_STATE;
		      
          break;				
			case RESET_STATE:
				RadioInit();
				app_status = RECEIVER_ALWAYS_ON;
				break;
				
			case RECEIVER_ALWAYS_ON:
				break;
				
		}
	
	
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
           
           // Enviar o pacote para o TX com a quantidade de pacotes a
           // ser enviados e o tamanho desses pacotes
           
           MLMERXDisableRequest();
        
          app_status = FINAL_STATE;
        }       
       
	}
}

void MCPSDataIndication(tRxPacket *rx_packet2) {

	if (rx_packet2->u8Status == TIMEOUT) {

		/* Put timeout condition code here */
		app_status = RX_STATE;	//Should not get here.

	}
   if (rx_packet2->u8Status == SUCCESS) {      
   
   
	   if (strcmp(&rx_packet2->pu8Data[0], "DONEDONE") == 0)
	      app_status = ECHO_STATE;
	   else if (rx_packet2->u8DataLength == (packet_size-2)){
	      app_status = GOOD_PACKET;
	   } else{
	      app_status = RX_STATE;
	   }
	} else {
	   app_status = RX_STATE;
	}
}

void MLMEMC13192ResetIndication() {
	//Notifies you that the MC13192 has been reset.
	//Application must handle this here.
	app_status = RESET_STATE;
}

void app_init(void) {
  
  /* Init LED's */
	LED1 = 1; /* Default is off */
	LED2 = 1;
	LED3 = 1;
	LED4 = 1;

	LED1DIR = 1; /*Set the DDIR register as Outputs for the LED pins*/
	LED2DIR = 1;
	LED3DIR = 1;
	LED4DIR = 1;
	
	//Pull up the pushbuttons.
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
