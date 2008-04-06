/*
 * vectortable.c
 * This file holds the interrupt vector table
 
 * $Author: rgd04c $
 * $Date: 2006/09/06 05:45:22 $
 * $Name:  $
 */
#include "pub_def.h"
#include "app_config.h"

typedef void(*tIsrFunc)(void);


extern interrupt void IRQIsr(void);
extern interrupt void KBIisr();
extern interrupt void IRQTimer1();
//*****************************
//Put your ISR routines here.
//extern interrupt void yourAppISR(void);
extern interrupt void Vscirx();

//Put the Function Name of your interrupt handler into this constant array.
//at the ISR location that you want.

interrupt void UnimplementedISR(void)
{
  for(;;);
}

// Added redirected ISR vectors when BootLoader is enabled.
// The application cannot have a reset vector (resides in BootLoader).
#if (EMBEDDED_BOOTLOADER == TRUE)
    // Redirected ISR vectors
    const tIsrFunc _vect[] @0xEFCC = {  /* Interrupt table */
#else
    const tIsrFunc _vect[] @0xFFCC = {  /* Interrupt table */
#endif
        UnimplementedISR,       /* vector 25: RT */
        UnimplementedISR,       /* vector 24: IIC */
        UnimplementedISR,       /* vector 23: ATD */
        KBIisr,          /* vector 22: KBI */
        UnimplementedISR,          /* vector 21: SCI2TX */
        Vscirx,                   /* vector 20: SCI2RX */
        UnimplementedISR,       /* vector 19: SCI2ER */
        UnimplementedISR,        /* vector 18: SCI1TX */
        Vscirx,                    /* vector 17: SCI1RX */
        UnimplementedISR,       /* vector 16: SCI1ER */
        UnimplementedISR,       /* vector 15: SPI */
        UnimplementedISR,          /* vector 14: TPM2OF */
        UnimplementedISR,       /* vector 13: TPM2C4 */
        UnimplementedISR,       /* vector 12: TPM2C3 */
        UnimplementedISR,       /* vector 11: TPM2C2 */
        UnimplementedISR,       /* vector 10: TPM2C1 */
        UnimplementedISR,       /* vector 09: TPM2C0 */
        IRQTimer1,       /* vector 08: TPM1OF */
        UnimplementedISR,       /* vector 07: TPM1C2 */
        UnimplementedISR,       /* vector 06: TPM1C1 */
        UnimplementedISR,          /* vector 05: TPM1C0 */
        UnimplementedISR,       /* vector 04: ICG */
        UnimplementedISR,       /* vector 03: Low Voltage Detect */
        IRQIsr,                /* vector 02: IRQ pin */
        UnimplementedISR        /* vector 01: SWI */
        /*_Startup, by default in library*/ /* Reset vector */
   };