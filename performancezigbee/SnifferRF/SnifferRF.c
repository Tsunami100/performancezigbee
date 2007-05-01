/*
    Microchip ZigBee Stack

    Demo RFD

    This demonstration shows how a ZigBee RFD can be set up.  This demo allows
    the PICDEM Z Demostration Board to act as either a "Switching Load Controller"
    (e.g. a light) or a "Switching Remote Control" (e.g. a switch) as defined by
    the Home Controls, Lighting profile.  It is designed to interact with a
    second PICDEM Z programmed with the Demo Coordinator project.

    To give the PICDEM Z "switch" capability, uncomment the I_AM_SWITCH definition
    below.  To give the PICDEM Z "light" capability, uncomment the I_AM_LIGHT
    definition below.  The PICDEM Z may have both capabilities enabled.  Be sure
    that the corresponding Demo Coordinator device is programmed with complementary
    capabilities.  NOTE - for simplicity, the ZigBee simple descriptors for this
    demonstration are fixed.

    If this node is configured as a "switch", it can discover the network address
    of the "light" using two methods.  If the USE_BINDINGS definition is
    uncommented below, then End Device Binding must be performed between the
    "switch" and the "light" before messages can be sent and received successfully.
    If USE_BINDINGS is commented out, then the node will default to the probable
    network address of the other node, and messages may be able to be sent
    immediately.  However, the node will also be capable of performing Device
    Discovery to discover the actual network address of the other node, in case
    the network was formed with alternate short address assignments.  NOTE: The
    USE_BINDINGS definition must be the same in both the RFD and the ZigBee
    Coordinator nodes.

    Switch functionality is as follows:
        RB4, I_AM_SWITCH defined, sends a "toggle" message to the other node's "light"
        RB4, I_AM_SWITCH not defined, no effect
        RB5, USE_BINDINGS defined, sends an End Device Bind request
        RB5, USE_BINDINGS undefined, sends a NWK_ADDR_req for the MAC address specified

    End Device Binding
    ------------------
    If the USE_BINDINGS definition is uncommented, the "switch" will send an
    APS indirect message to toggle the "light".  In order for the message to
    reach its final destination, a binding must be created between the "switch"
    and the "light".  To do this, press RB5 on one PICDEM Z, and then press RB5
    on the other PICDEM Z within 5 seconds.  A message will be displayed indicating
    if binding was successful or not.  Note that End Device Binding is a toggle
    function.  Performing the operation again will unbind the nodes, and messages
    will not reach their final destination.

    Device Discovery
    ----------------
    If the USE_BINDINGS definition is not uncommented, pressing RB5 will send a
    broadcast NWK_ADDR_req message.  The NWK_ADDR_req message contains the MAC
    address of the desired node.  Be sure this address matches the address
    contained in the other node's zigbee.def file.

    NOTE: To speed network formation, ALLOWED_CHANNELS has been set to
    channel 12 only.

 *********************************************************************
 * FileName:        RFD.c
 * Dependencies:
 * Processor:       PIC18F
 * Complier:        MCC18 v3.00 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
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


//******************************************************************************
// Header Files
//******************************************************************************

// Include the main ZigBee header file.
#include "zAPL.h"

// If you are going to send data to a terminal, include this file.
#include "console.h"

#include "zPHY_CC2420.h"

//******************************************************************************
// Configuration Bits
//******************************************************************************

#if defined(MCHP_C18) && defined(__18F4620)
    #pragma romdata CONFIG1H = 0x300001
    const rom unsigned char config1H = 0b00000110;      // HSPLL oscillator

    #pragma romdata CONFIG2L = 0x300002
    const rom unsigned char config2L = 0b00011111;      // Brown-out Reset Enabled in hardware @ 2.0V, PWRTEN disabled

    #pragma romdata CONFIG2H = 0x300003
    const rom unsigned char config2H = 0b00010010;      // HW WD disabled, 1:512 prescaler

    #pragma romdata CONFIG3H = 0x300005
    const rom unsigned char config3H = 0b10000000;      // PORTB digital on RESET

    #pragma romdata CONFIG4L = 0x300006
    const rom unsigned char config4L = 0b10000001;      // DEBUG disabled,
                                                        // XINST disabled
                                                        // LVP disabled
                                                        // STVREN enabled

    #pragma romdata

#elif defined(HITECH_C18) && defined(_18F4620)
    // Set configuration fuses for HITECH compiler.
    __CONFIG(1, 0x0600);    // HSPLL oscillator
    __CONFIG(2, 0x101F);    // PWRTEN disabled, BOR enabled @ 2.0V, HW WD disabled, 1:128 prescaler
    __CONFIG(3, 0x8000);    // PORTB digital on RESET
    __CONFIG(4, 0x0081);    // DEBUG disabled,
                            // XINST disabled
                            // LVP disabled
                            // STVREN enabled
#endif

//******************************************************************************
// Compilation Configuration
//******************************************************************************

#define _USE_BINDINGS
#define I_AM_LIGHT
#define I_AM_SWITCH

//******************************************************************************
// Constants
//******************************************************************************

// Switches and LEDs locations.
#define BROADCAST_SWITCH            RB5
#define LIGHT_SWITCH                RB4

#define BIND_INDICATION             LATA0
#define MESSAGE_INDICATION          LATA1

#define BIND_STATE_BOUND            0
#define BIND_STATE_TOGGLE           1
#define BIND_STATE_UNBOUND          1
#define BIND_WAIT_DURATION          (6*ONE_SECOND)

#define LIGHT_OFF                   0x00
#define LIGHT_ON                    0xFF
#define LIGHT_TOGGLE                0xF0


//******************************************************************************
// Application Variables
//******************************************************************************

static union
{
    struct
    {
        BYTE    bBroadcastSwitchToggled    : 1;
        BYTE    bLightSwitchToggled        : 1;
        BYTE    bTryingToBind              : 1;
        BYTE    bIsBound                   : 1;
        BYTE    bDestinationAddressKnown   : 1;
		BYTE    bTestePerformance          : 1;
    } bits;
    BYTE Val;
} myStatusFlags;
#define STATUS_FLAGS_INIT       0x00
#define TOGGLE_BOUND_FLAG       0x08

NETWORK_DESCRIPTOR  *currentNetworkDescriptor;
ZIGBEE_PRIMITIVE    currentPrimitive;
SHORT_ADDR          destinationAddress;
NETWORK_DESCRIPTOR  *NetworkDescriptor;

TICK                lastTestePerformance;

BYTE sinal;

unsigned int packetsNumber = 0;
unsigned int maxPacketsNumber = 1000;

//******************************************************************************
// Function Prototypes
//******************************************************************************

void HardwareInit( void );
BOOL myProcessesAreDone( void );

//******************************************************************************
//******************************************************************************
// Main
//******************************************************************************
//******************************************************************************

void main(void)
{
    CLRWDT();
    ENABLE_WDT();

    currentPrimitive = NO_PRIMITIVE;
    NetworkDescriptor = NULL;

    // If you are going to send data to a terminal, initialize the UART.
    ConsoleInit();

    ConsolePutROMString( (ROM char *)"\r\n\r\n\r\n*************************************\r\n" );
    ConsolePutROMString( (ROM char *)"Microchip ZigBee(TM) Stack - v1.0-3.5\r\n\r\n" );
    ConsolePutROMString( (ROM char *)"ZigBee RFD\r\n\r\n" );
    ConsolePutROMString( (ROM char *)"Programa Sniffer RF para rede ZigBee.\r\n\r\n" );

    // Initialize the hardware - must be done before initializing ZigBee.
    HardwareInit();

    // Initialize the ZigBee Stack.
    ZigBeeInit();

    // *************************************************************************
    // Perform any other initialization here
    // *************************************************************************

    myStatusFlags.Val = STATUS_FLAGS_INIT;

    // Default the destination address to the ZigBee Coordinator
    destinationAddress.Val = 0x0000;

    // Initialize the LED's.
    BIND_INDICATION = !myStatusFlags.bits.bIsBound;
    MESSAGE_INDICATION = 0;

    // Enable interrupts to get everything going.
    RBIE = 1;
    IPEN = 1;
    GIEH = 1;

	phyPIB.phyCurrentChannel = 12;


		ConsolePutROMString( (ROM char *)"Iniciando no canal: " );
		PrintChar( phyPIB.phyCurrentChannel  );
		ConsolePutROMString( (ROM char *)"\r\n" );
	

	MACEnable();

                    PHY_CSn_0();
                    SPIPut(REG_FSCTRL);
                    SPIPut(0x41);            // LOCK_THR = 1 as recommended, with bit 8 = 1 for MSb of FREQ value
                    SPIPut((phyPIB.phyCurrentChannel-11)*5+101);      // Create raw LSB for given channel
                    PHY_CSn_1();
    // Turn on RX
    PHY_CSn_0();
    SPIPut(STROBE_SRXON);
        //sinal = SPIGet();
		//PrintChar( sinal );
    PHY_CSn_1();

    /* wait for the part to enter RX mode */
    {
        TICK t,T2;
        t = TickGet();
        while(1)
        {
            T2 = TickGet();
            if(TickGetDiff(T2,t) > SYMBOLS_TO_TICKS(12))
            {
                break;
            }
        }
    }

    while (1)
    {
        CLRWDT();

		BIND_INDICATION ^= 1;

//		ConsolePutROMString( (ROM char *)"." );

//		sinal = CC2420GetStatus();
//		PrintChar( sinal );


        CLRWDT();

        PHY_CSn_0();
        SPIPut(STROBE_STXONCCA);
        sinal = SPIGet();

		//PrintChar( sinal );

        SPIPut(STROBE_SNOP);
        sinal = SPIGet();

//		sinal = CC2420GetStatus();
		//PrintChar( sinal );
        // If transmission was started, break out of this loop.
        if ( sinal & 0x08 )
        {
			ConsolePutROMString( (ROM char *)"_" );
        }
		else
		{
			MESSAGE_INDICATION ^= 1;

			ConsolePutROMString( (ROM char *)"||" );


		}

		if ( sinal & 0x02 )
		{
			BYTE RSSI;

        	PHY_CSn_1();
            PHY_CSn_0();
            SPIPut(CMD_READ | REG_RSSI);
            SPIGet();   //don't care about this value
            RSSI = SPIGet() +128;   //this is what we are looking for
            PHY_CSn_1();

			PrintChar( RSSI );
            PHY_CSn_0();
		}

//		ConsolePutROMString( (ROM char *)"" );

        PHY_CSn_1();

    /* wait for the part to enter RX mode */
    {
        TICK t,T2;
        t = TickGet();
        while(1)
        {
            T2 = TickGet();
            if(TickGetDiff(T2,t) > SYMBOLS_TO_TICKS(1))
            {
                break;
            }
        }
    }

        // *********************************************************************
        // Place any non-ZigBee related processing here.  Be sure that the code
        // will loop back and execute ZigBeeTasks() in a timely manner.
        // *********************************************************************

    }
}

/*******************************************************************************
myProcessesAreDone

This routine should contain any tests that are required by the application to
confirm that it can go to sleep.  If the application can go to sleep, this
routine should return TRUE.  If the application is still busy, this routine
should return FALSE.
*******************************************************************************/

BOOL myProcessesAreDone( void )
{
    return (myStatusFlags.bits.bBroadcastSwitchToggled==FALSE) && (myStatusFlags.bits.bLightSwitchToggled==FALSE);
}

/*******************************************************************************
HardwareInit

All port directioning and SPI must be initialized before calling ZigBeeInit().

For demonstration purposes, required signals are configured individually.
*******************************************************************************/
void HardwareInit(void)
{

    //-------------------------------------------------------------------------
    // This section is required to initialize the PICDEM Z for the CC2420
    // and the ZigBee Stack.
    //-------------------------------------------------------------------------

    SPIInit();

    // CC2420 I/O assignments with respect to PIC:
    //NOTE: User must make sure that pin is capable of correct digital operation.
    //      This may require modificaiton of which pins are digital and analog.
    //NOTE: The stack requires that the SPI interface be located on LATC3 (SCK),
    //      RC4 (SO), and LATC5 (SI).
    //NOTE: The appropriate config bit must be set such that FIFOP is the CCP2
    //      input pin. The stack uses the CCP2 interrupt.

    // Start with CC2420 disabled and not selected
    PHY_CSn             = 1;
    PHY_VREG_EN         = 0;
    PHY_RESETn          = 1;

    // Set the directioning for the CC2420 pin connections.
    PHY_FIFO_TRIS       = 1;    // FIFO      (Input)
    PHY_SFD_TRIS        = 1;    // SFD       (Input - Generates interrupt on falling edge)
    PHY_FIFOP_TRIS      = 1;    // FIFOP     (Input - Used to detect overflow, CCP2 interrupt)
    PHY_CSn_TRIS        = 0;    // CSn       (Output - to select CC2420 SPI slave)
    PHY_VREG_EN_TRIS    = 0;    // VREG_EN   (Output - to enable CC2420 voltage regulator)
    PHY_RESETn_TRIS     = 0;    // RESETn    (Output - to reset CC2420)

    // Initialize the SPI pins and directions
    LATC3               = 1;    // SCK
    LATC5               = 1;    // SDO
    TRISC3              = 0;    // SCK
    TRISC4              = 1;    // SDI
    TRISC5              = 0;    // SDO

    // Initialize the SPI module
    SSPSTAT = 0xC0;
    SSPCON1 = 0x20;

    //-------------------------------------------------------------------------
    // This section is required for application-specific hardware
    // initialization.
    //-------------------------------------------------------------------------

    // D1 and D2 are on RA0 and RA1 respectively, and CS of the TC77 is on RA2.
    // Make PORTA digital I/O.
    ADCON1 = 0x0F;

    // Deselect the TC77 temperature sensor (RA2)
    LATA = 0x04;

    // Make RA0, RA1, RA2 and RA4 outputs.
    TRISA = 0xE0;

    // Clear the RBIF flag (INTCONbits.RBIF)
    RBIF = 0;

    // Enable PORTB pull-ups (INTCON2bits.RBPU)
    RBPU = 0;

    // Make the PORTB switch connections inputs.
    TRISB4 = 1;
    TRISB5 = 1;
}

/*******************************************************************************
User Interrupt Handler

The stack uses some interrupts for its internal processing.  Once it is done
checking for its interrupts, the stack calls this function to allow for any
additional interrupt processing.
*******************************************************************************/

void UserInterruptHandler(void)
{

    // *************************************************************************
    // Place any application-specific interrupt processing here
    // *************************************************************************

    // Is this a interrupt-on-change interrupt?
    if ( RBIF == 1 )
    {
        // Record which button was pressed so the main() loop can
        // handle it
        if (BROADCAST_SWITCH == 0)
            myStatusFlags.bits.bBroadcastSwitchToggled = TRUE;

        //if (LIGHT_SWITCH == 0)
        //    myStatusFlags.bits.bLightSwitchToggled = TRUE;
		if (LIGHT_SWITCH == 0)
		{
			if (myStatusFlags.bits.bTestePerformance)
			{
            	ConsolePutROMString( (ROM char *)"Parando teste de performance...\r\n" );
				myStatusFlags.bits.bTestePerformance = FALSE;
										BIND_INDICATION = 0;
			}
			else
			{
            	ConsolePutROMString( (ROM char *)"Iniciando teste de performance...\r\n" );
				myStatusFlags.bits.bTestePerformance = TRUE;
				lastTestePerformance = TickGet();
										BIND_INDICATION = 1;
			}
		}



        // Disable further RBIF until we process it
        RBIE = 0;

        // Clear mis-match condition and reset the interrupt flag
        LATB = PORTB;

        RBIF = 0;
    }

}
