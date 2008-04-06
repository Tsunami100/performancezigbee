#include "drivers.h"
#include "MC1320XAXMC.h"

void RadioInit(void) {
    SPIDrvWrite(0x11,0x80FF);   /* Eliminate Unlock Conditions due to L01 */
    SPIDrvWrite(0x1B,0x8000);   /* Disable TC1. */
    SPIDrvWrite(0x1D,0x8000);   /* Disable TC2. */
    SPIDrvWrite(0x1F,0x8000);   /* Disable TC3. */
    SPIDrvWrite(0x21,0x8000);   /* Disable TC4. */
    SPIDrvWrite(0x07,0x5C00);   /* Enable CLKo in Doze */
    SPIDrvWrite(0x0C,0x0381);   /* IRQ pull-up disable. */
    SPIDrvWrite(0x04,0xA08D);   /* New cal value */
    SPIDrvWrite(0x05,0x8351);   /* Acoma, TC1, Doze, ATTN masks, LO1, CRC */
    SPIDrvWrite(0x06,0x4720);   /* CCA, TX, RX, energy detect */
    SPIDrvWrite(0x0B,0xC0FE);  
    (void)SPIDrvRead(0x25);
    
    /* Advance the state variable to Idle */
    gu8RTxMode = IDLE_MODE;
}