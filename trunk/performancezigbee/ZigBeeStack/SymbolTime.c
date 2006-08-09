/*********************************************************************
 *
 *                  ZigBee Symbol Timer
 *
 *********************************************************************
 * FileName:        SymbolTime.c
 * Dependencies:
 * Processor:       PIC18F
 * Complier:        MCC18 v3.00 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC® microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC microcontroller products.
 *
 * You may not modify or create derivatives works of the Software.
 *
 * If you intend to use the software in a product for sale, then you must
 * be a member of the ZigBee Alliance and obtain any licenses required by
 * them.  For more information, go to www.zigbee.org.
 *
 * The software is owned by the Company and/or its licensor, and is
 * protected under applicable copyright laws. All rights are reserved.
 *
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * DF/KO                01/09/06 Microchip ZigBee Stack v1.0-3.5
********************************************************************/

#include "zigbee.def"
#include "SymbolTime.h"
#include "Compiler.h"
#include "generic.h"

volatile BYTE timerExtension;

void InitSymbolTimer()
{
    T0CON = 0b00000000 | CLOCK_DIVIDER_SETTING;
    TMR0IP = 1;
    TMR0IF = 0;
    TMR0IE = 1;
    TMR0ON = 1;

    timerExtension = 0;
}


/* caution: this function should never be called when interrupts are disabled */
/* if interrupts are disabled when this is called then the timer might rollover */
/* and the byte extension would not get updated. */
TICK TickGet(void)
{
    TICK currentTime;

    /* disable the timer to prevent roll over of the lower 16 bits while before/after reading of the extension */
    TMR0ON = 0;

    /* Zigbee and IEEE802.15.4 use 3 byte timers.
    We have used a 4 byte timer because of compiler
    reasons thus we need to zero out the MSB of the timestamp */
    currentTime.byte.b3 = 0;

    /* copy the byte extension */
    currentTime.byte.b2 = timerExtension;

    /* read the timer value */
    currentTime.byte.b0 = TMR0L;
    currentTime.byte.b1 = TMR0H;

    /* enable the timer*/
    TMR0ON = 1;

    return currentTime;
}
