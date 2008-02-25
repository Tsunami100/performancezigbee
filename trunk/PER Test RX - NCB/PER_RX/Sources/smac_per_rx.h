//Prototypes
void app_init(void);
void delay(UINT16 count); 

/* Application state definitions */

#define INITIAL_STATE				1
#define RESET_STATE					2
#define RECEIVER_ALWAYS_ON 		3
#define WAITING_FOR_ACK 			4
#define TX_STATE 						5
#define RX_STATE 						6
#define SET_LEDS 						7
#define GOOD_PACKET					8
#define BAD_PACKET               9
#define FALSE_PACKET             10
#define FINAL_STATE              11
#define IDLE_STATE               12
#define ECHO_STATE               13

#define SEND_START                14
#define INPUT_VALUE               15
#define WAITING_START             16

#define DELAY_COUNT					0xB000
#define LED_DELAY           0x2000

/* Power Level defines */
#define SPEC_POWER 	88		//LEDS 2-4 OFF
#define L1_POWER	80		//LED 2 ON
#define L2_POWER	60		//LED 2,3 ON
#define L3_POWER	40		//LED 2,3,4 ON

#define CHANNEL_NUMBER      15  
#define OUTPUT_POWER        11
#define TEST_NUM            999