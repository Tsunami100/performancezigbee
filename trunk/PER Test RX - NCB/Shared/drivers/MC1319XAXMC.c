#include "drivers.h"
#include "MC1319XAXMC.h"

void RadioInit(void) {
    (void)SPIDrvWrite(0x11,0x80FF);   /* Eliminate Unlock Conditions due to L01 */
    (void)SPIDrvWrite(0x1B,0x8000);   /* Disable TC1. */
    (void)SPIDrvWrite(0x1D,0x8000);   /* Disable TC2. */
    (void)SPIDrvWrite(0x1F,0x8000);   /* Disable TC3. */
    (void)SPIDrvWrite(0x21,0x8000);   /* Disable TC4. */
    (void)SPIDrvWrite(0x07,0x5E00);   /* Enable CLKo in Doze */
    (void)SPIDrvWrite(0x0C,0x0300);   /* IRQ pull-up disable. */
    (void)SPIDrvWrite(0x04,0xA08D);   /* New cal value */
    (void)SPIDrvWrite(0x08,0xFFF7);   /* Preferred injection */
    (void)SPIDrvWrite(0x05,0x8351);   /* Acoma, TC1, Doze, ATTN masks, LO1, CRC */
    (void)SPIDrvWrite(0x06,0x4720);   /* CCA, TX, RX, energy detect */
    (void)SPIDrvRead(0x25);
    
    /* Advance the state variable to Idle */
    gu8RTxMode = IDLE_MODE;
}