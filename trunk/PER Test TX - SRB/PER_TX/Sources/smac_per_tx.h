//includes
#include "pub_def.h"

//Prototypes
void app_init(void);
void delaySymbols(UINT16 count);
void delaySeconds(UINT16 count);

/* Application state definitions */

#define INITIAL_STATE				1
#define RESET_STATE					2
#define RECEIVER_ALWAYS_ON 		3
#define WAITING_FOR_ACK 			4
#define TX_STATE 					   5
#define RX_STATE 					   6
#define SET_LEDS 					   7
#define DELAY_STATE					8
#define FINAL_STATE              9
#define IDLE_STATE               10

#define START_RECEIVED 11

#define DELAY_COUNT			   0x8000
#define LED_DELAY             0xB000

#define CHANNEL_NUMBER      15  
#define OUTPUT_POWER        11
#define TEST_NUM            999