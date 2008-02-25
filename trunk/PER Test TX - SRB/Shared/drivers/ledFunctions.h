#include "freescale_radio_hardware.h"


void setLedsMode(UINT8 u8mode, UINT16 u16value, UINT16 u16ledDelay, UINT8 flags);
void setLed(UINT8 u8value, UINT8 flags);

/* Led FLAGS */
#define LED_NO_FLAGS 0x00
#define LED_INVERT   0x01

/* Led MODES */
#define LED_BINARY_MODE    0x00
#define LED_HEX_MODE       0x01
#define LED_COUNT_MODE     0x02
#define LED_DIGIT_MODE     0x04

#define LED_COUNT 4