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
 * (the �Company�) for its PICmicro� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
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
    ConsolePutROMString( (ROM char *)"Teste de performance da rede ZigBee.\r\n\r\n" );

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

    while (1)
    {
        CLRWDT();
        ZigBeeTasks( &currentPrimitive );

        switch (currentPrimitive)
        {
            case NLME_NETWORK_DISCOVERY_confirm:
                currentPrimitive = NO_PRIMITIVE;
                if (!params.NLME_NETWORK_DISCOVERY_confirm.Status)
                {
                    if (!params.NLME_NETWORK_DISCOVERY_confirm.NetworkCount)
                    {
                        ConsolePutROMString( (ROM char *)"No networks found.  Trying again...\r\n" );
                    }
                    else
                    {
                        // Save the descriptor list pointer so we can destroy it later.
                        NetworkDescriptor = params.NLME_NETWORK_DISCOVERY_confirm.NetworkDescriptor;

                        // Select a network to try to join.  We're not going to be picky right now...
                        currentNetworkDescriptor = NetworkDescriptor;

                        // not needed for new join params.NLME_JOIN_request.ScanDuration = ;
                        // not needed for new join params.NLME_JOIN_request.ScanChannels = ;
                        params.NLME_JOIN_request.PANId          = currentNetworkDescriptor->PanID;
                        ConsolePutROMString( (ROM char *)"Network(s) found. Trying to join " );
                        PrintChar( params.NLME_JOIN_request.PANId.byte.MSB );
                        PrintChar( params.NLME_JOIN_request.PANId.byte.LSB );
                        ConsolePutROMString( (ROM char *)".\r\n" );
                        params.NLME_JOIN_request.JoinAsRouter   = FALSE;
                        params.NLME_JOIN_request.RejoinNetwork  = FALSE;
                        params.NLME_JOIN_request.PowerSource    = NOT_MAINS_POWERED;
                        params.NLME_JOIN_request.RxOnWhenIdle   = FALSE;
                        params.NLME_JOIN_request.MACSecurity    = FALSE;
                        currentPrimitive = NLME_JOIN_request;
                    }
                }
                else
                {
                    PrintChar( params.NLME_NETWORK_DISCOVERY_confirm.Status );
                    ConsolePutROMString( (ROM char *)" Error finding network.  Trying again...\r\n" );
                }
                break;

            case NLME_JOIN_confirm:
                if (!params.NLME_JOIN_confirm.Status)
                {
                    ConsolePutROMString( (ROM char *)"Join successful!\r\n" );
                    if (NetworkDescriptor)
                    {
                        // If we joined as an orphan, this will be NULL.
                        free( NetworkDescriptor );
                    }
                    currentPrimitive = NO_PRIMITIVE;
                }
                else
                {
                    PrintChar( params.NLME_JOIN_confirm.Status );
                    ConsolePutROMString( (ROM char *)" Could not join.\r\n" );
                    currentPrimitive = NO_PRIMITIVE;
                }
                break;

            case NLME_LEAVE_indication:
                if (!memcmppgm2ram( &params.NLME_LEAVE_indication.DeviceAddress, (ROM void *)&macLongAddr, 8 ))
                {
                    ConsolePutROMString( (ROM char *)"We have left the network.\r\n" );
                }
                else
                {
                    ConsolePutROMString( (ROM char *)"Another node has left the network.\r\n" );
                }
                currentPrimitive = NO_PRIMITIVE;
                break;

            case NLME_RESET_confirm:
                ConsolePutROMString( (ROM char *)"ZigBee Stack has been reset.\r\n" );
                currentPrimitive = NO_PRIMITIVE;
                break;

            case NLME_SYNC_confirm:
                switch (params.NLME_SYNC_confirm.Status)
                {
                    case SUCCESS:
                        // I have heard from my parent, but it has no data for me.  Note that
                        // if my parent has data for me, I will get an APSDE_DATA_indication.
                        ConsolePutROMString( (ROM char *)"No data available.\r\n" );
                        break;

                    case NWK_SYNC_FAILURE:
                        // I cannot communicate with my parent.
                        ConsolePutROMString( (ROM char *)"I cannot communicate with my parent.\r\n" );
                        break;

                    case NWK_INVALID_PARAMETER:
                        // If we call NLME_SYNC_request correctly, this doesn't occur.
                        ConsolePutROMString( (ROM char *)"Invalid sync parameter.\r\n" );
                        break;
                }
                currentPrimitive = NO_PRIMITIVE;
                break;

            case APSDE_DATA_indication:
                {
                    WORD_VAL    attributeId;
                    BYTE        command;
                    BYTE        data;
                    BYTE        dataLength;
                    //BYTE        dataType;
                    BYTE        frameHeader;
                    BYTE        sequenceNumber;
                    BYTE        transaction;
                    BYTE        transByte;

                    currentPrimitive = NO_PRIMITIVE;
                    frameHeader = APLGet();

                    switch (params.APSDE_DATA_indication.DstEndpoint)
                    {
                        case EP_ZDO:
                            ConsolePutROMString( (ROM char *)"  Receiving ZDO cluster " );
                            PrintChar( params.APSDE_DATA_indication.ClusterId );
                            ConsolePutROMString( (ROM char *)"\r\n" );

                            // Put code here to handle any ZDO responses that we requested
                            if ((frameHeader & APL_FRAME_TYPE_MASK) == APL_FRAME_TYPE_MSG)
                            {
                                frameHeader &= APL_FRAME_COUNT_MASK;
                                for (transaction=0; transaction<frameHeader; transaction++)
                                {
                                    sequenceNumber          = APLGet();
                                    dataLength              = APLGet();
                                    transByte               = 1;    // Account for status byte

                                    switch( params.APSDE_DATA_indication.ClusterId )
                                    {

                                        // ********************************************************
                                        // Put a case here to handle each ZDO response that we requested.
                                        // ********************************************************

                                        case NWK_ADDR_rsp:
                                            if (APLGet() == SUCCESS)
                                            {
                                                ConsolePutROMString( (ROM char *)"  Receiving NWK_ADDR_rsp.\r\n" );

                                                // Skip over the IEEE address of the responder.
                                                for (data=0; data<8; data++)
                                                {
                                                    APLGet();
                                                    transByte++;
                                                }
                                                destinationAddress.byte.LSB = APLGet();
                                                destinationAddress.byte.MSB = APLGet();
                                                transByte += 2;
                                                myStatusFlags.bits.bDestinationAddressKnown = 1;
                                            }
                                            break;

                                        #ifdef USE_BINDINGS
                                        case END_DEVICE_BIND_rsp:
                                            switch( APLGet() )
                                            {
                                                case SUCCESS:
                                                    ConsolePutROMString( (ROM char *)" End device bind/unbind successful!\r\n" );
                                                    myStatusFlags.bits.bIsBound ^= TOGGLE_BOUND_FLAG;
                                                    BIND_INDICATION = !myStatusFlags.bits.bIsBound;
                                                    break;
                                                case ZDO_NOT_SUPPORTED:
                                                    ConsolePutROMString( (ROM char *)" End device bind/unbind not supported.\r\n" );
                                                    break;
                                                case END_DEVICE_BIND_TIMEOUT:
                                                    ConsolePutROMString( (ROM char *)" End device bind/unbind time out.\r\n" );
                                                    break;
                                                case END_DEVICE_BIND_NO_MATCH:
                                                    ConsolePutROMString( (ROM char *)" End device bind/unbind failed - no match.\r\n" );
                                                    break;
                                                default:
                                                    ConsolePutROMString( (ROM char *)" End device bind/unbind invalid response.\r\n" );
                                                    break;
                                            }
                                            myStatusFlags.bits.bTryingToBind = 0;
                                            break;
                                        #endif

                                        default:
                                            break;
                                    }

                                    // Read out the rest of the MSG in case there is another transaction.
                                    for (; transByte<dataLength; transByte++)
                                    {
                                        APLGet();
                                    }
                                }
                            }
                            break;

                        // ************************************************************************
                        // Place a case for each user defined endpoint.
                        // ************************************************************************

                        case EP_LIGHT:
                            if ((frameHeader & APL_FRAME_TYPE_MASK) == APL_FRAME_TYPE_KVP)
                            {
                                frameHeader &= APL_FRAME_COUNT_MASK;
                                for (transaction=0; transaction<frameHeader; transaction++)
                                {
                                    sequenceNumber          = APLGet();
                                    command                 = APLGet();
                                    attributeId.byte.LSB    = APLGet();
                                    attributeId.byte.MSB    = APLGet();

                                    //dataType = command & APL_FRAME_DATA_TYPE_MASK;
                                    command &= APL_FRAME_COMMAND_MASK;

                                    if ((params.APSDE_DATA_indication.ClusterId == OnOffSRC_CLUSTER) &&
                                        (attributeId.Val == OnOffSRC_OnOff))
                                    {
                                        if ((command == APL_FRAME_COMMAND_SET) ||
                                            (command == APL_FRAME_COMMAND_SETACK))
                                        {
                                            // Prepare a response in case it is needed.
                                            TxBuffer[TxData++] = APL_FRAME_TYPE_KVP | 1;    // KVP, 1 transaction
                                            TxBuffer[TxData++] = sequenceNumber;
                                            TxBuffer[TxData++] = APL_FRAME_COMMAND_SET_RES | (APL_FRAME_DATA_TYPE_UINT8 << 4);
                                            TxBuffer[TxData++] = attributeId.byte.LSB;
                                            TxBuffer[TxData++] = attributeId.byte.MSB;

                                            // Data type for this attibute must be APL_FRAME_DATA_TYPE_UINT8
                                            data = APLGet();
                                            switch (data)
                                            {
                                                case LIGHT_OFF:
                                                    ConsolePutROMString( (ROM char *)" Turning light off.\r\n" );
                                                    MESSAGE_INDICATION = 0;
                                                    TxBuffer[TxData++] = SUCCESS;
                                                    break;
                                                case LIGHT_ON:
                                                    ConsolePutROMString( (ROM char *)" Turning light on.\r\n" );
                                                    MESSAGE_INDICATION = 1;
                                                    TxBuffer[TxData++] = SUCCESS;
                                                    break;
                                                case LIGHT_TOGGLE:
                                                    ConsolePutROMString( (ROM char *)" Toggling light.\r\n" );
                                                    MESSAGE_INDICATION ^= 1;
                                                    TxBuffer[TxData++] = SUCCESS;
                                                    break;
                                                default:
                                                    PrintChar( data );
                                                    ConsolePutROMString( (ROM char *)" Invalid light message.\r\n" );
                                                    TxBuffer[TxData++] = KVP_INVALID_ATTRIBUTE_DATA;
                                                    break;
                                            }
                                        }
                                        if (command == APL_FRAME_COMMAND_SETACK)
                                        {
                                            // Send back an application level acknowledge.
                                            ZigBeeBlockTx();

                                            // Take care here that parameters are not overwritten before they are used.
                                            // We can use the data byte as a temporary variable.
                                            params.APSDE_DATA_request.DstAddrMode = params.APSDE_DATA_indication.SrcAddrMode;
                                            params.APSDE_DATA_request.DstEndpoint = params.APSDE_DATA_indication.SrcEndpoint;
                                            params.APSDE_DATA_request.DstAddress.ShortAddr = params.APSDE_DATA_indication.SrcAddress.ShortAddr;

                                            //params.APSDE_DATA_request.asduLength; TxData
                                            //params.APSDE_DATA_request.ProfileId; unchanged
                                            params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;
                                            params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_ENABLE;
                                            params.APSDE_DATA_request.TxOptions.Val = 0;
                                            params.APSDE_DATA_request.SrcEndpoint = EP_LIGHT;
                                            //params.APSDE_DATA_request.ClusterId; unchanged

                                            currentPrimitive = APSDE_DATA_request;
                                        }
                                        else
                                        {
                                            // We are not sending an acknowledge, so reset the transmit message pointer.
                                            TxData = TX_DATA_START;
                                        }
                                    }
                                    // TODO read to the end of the transaction.
                                } // each transaction
                            } // frame type
                            break;
                        case EP_PERFORMANCE:
						            	ConsolePutROMString( (ROM char *)"Iniciando teste de performance...\r\n" );
										myStatusFlags.bits.bTestePerformance = TRUE;
										packetsNumber = 0;
										lastTestePerformance = TickGet();
										BIND_INDICATION = 1;

                            if ((frameHeader & APL_FRAME_TYPE_MASK) == APL_FRAME_TYPE_KVP)
                            {
                                frameHeader &= APL_FRAME_COUNT_MASK;
                                for (transaction=0; transaction<frameHeader; transaction++)
                                {
                                    sequenceNumber          = APLGet();
                                    command                 = APLGet();
                                    attributeId.byte.LSB    = APLGet();
                                    attributeId.byte.MSB    = APLGet();

                                    //dataType = command & APL_FRAME_DATA_TYPE_MASK;
                                    command &= APL_FRAME_COMMAND_MASK;


                                    if ((params.APSDE_DATA_indication.ClusterId ==Performance_CLUSTER) &&
                                        (attributeId.Val == Performance_PduOStr))
                                    {
/*
                                        if ((command == APL_FRAME_COMMAND_SET) ||
                                            (command == APL_FRAME_COMMAND_SETACK))
                                        {
                                            // Prepare a response in case it is needed.
                                            TxBuffer[TxData++] = APL_FRAME_TYPE_KVP | 1;    // KVP, 1 transaction
                                            TxBuffer[TxData++] = sequenceNumber;
                                            TxBuffer[TxData++] = APL_FRAME_COMMAND_SET_RES | (APL_FRAME_DATA_TYPE_UINT8 << 4);
                                            TxBuffer[TxData++] = attributeId.byte.LSB;
                                            TxBuffer[TxData++] = attributeId.byte.MSB;

                                            // Data type for this attibute must be APL_FRAME_DATA_TYPE_UINT8
                                            data = APLGet();
                                            switch (data)
                                            {
                                                case LIGHT_OFF:
                                                    ConsolePutROMString( (ROM char *)" Turning light off.\r\n" );
                                                    MESSAGE_INDICATION = 0;
                                                    TxBuffer[TxData++] = SUCCESS;
                                                    break;
                                                case LIGHT_ON:
                                                    ConsolePutROMString( (ROM char *)" Turning light on.\r\n" );
                                                    MESSAGE_INDICATION = 1;
                                                    TxBuffer[TxData++] = SUCCESS;
                                                    break;
                                                case LIGHT_TOGGLE:
                                                    ConsolePutROMString( (ROM char *)" Toggling light.\r\n" );
                                                    MESSAGE_INDICATION ^= 1;
                                                    TxBuffer[TxData++] = SUCCESS;
                                                    break;
                                                default:
                                                    PrintChar( data );
                                                    ConsolePutROMString( (ROM char *)" Invalid light message.\r\n" );
                                                    TxBuffer[TxData++] = KVP_INVALID_ATTRIBUTE_DATA;
                                                    break;
                                            }
                                        }
                                        if (command == APL_FRAME_COMMAND_SETACK)
                                        {
                                            // Send back an application level acknowledge.
                                            ZigBeeBlockTx();

                                            // Take care here that parameters are not overwritten before they are used.
                                            // We can use the data byte as a temporary variable.
                                            params.APSDE_DATA_request.DstAddrMode = params.APSDE_DATA_indication.SrcAddrMode;
                                            params.APSDE_DATA_request.DstEndpoint = params.APSDE_DATA_indication.SrcEndpoint;
                                            params.APSDE_DATA_request.DstAddress.ShortAddr = params.APSDE_DATA_indication.SrcAddress.ShortAddr;

                                            //params.APSDE_DATA_request.asduLength; TxData
                                            //params.APSDE_DATA_request.ProfileId; unchanged
                                            params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;
                                            params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_ENABLE;
                                            params.APSDE_DATA_request.TxOptions.Val = 0;
                                            params.APSDE_DATA_request.SrcEndpoint = EP_LIGHT;
                                            //params.APSDE_DATA_request.ClusterId; unchanged

                                            currentPrimitive = APSDE_DATA_request;
                                        }
                                        else
                                        {
                                            // We are not sending an acknowledge, so reset the transmit message pointer.
                                            TxData = TX_DATA_START;
                                        }
*/
                                    }
                                    // TODO read to the end of the transaction.
                                } // each transaction
                            } // frame type
                            break;
                        default:
                            // If the command type was something that requested an acknowledge, we could send back
                            // KVP_INVALID_ENDPOINT here.
                            break;
                    }

                    APLDiscardRx();
                }
                break;

            case APSDE_DATA_confirm:
                if (params.APSDE_DATA_confirm.Status)
                {
                    ConsolePutROMString( (ROM char *)"Error " );
                    PrintChar( params.APSDE_DATA_confirm.Status );
                    ConsolePutROMString( (ROM char *)" sending message.\r\n" );
                }
                else
                {
                    ConsolePutROMString( (ROM char *)" Message sent successfully.\r\n" );
					packetsNumber++;
                }
                currentPrimitive = NO_PRIMITIVE;
                break;

            case NO_PRIMITIVE:
                if (!ZigBeeStatus.flags.bits.bNetworkJoined)
                {
                    if (!ZigBeeStatus.flags.bits.bTryingToJoinNetwork)
                    {
                        if (ZigBeeStatus.flags.bits.bTryOrphanJoin)
                        {
                            ConsolePutROMString( (ROM char *)"Trying to join network as an orphan...\r\n" );
                            params.NLME_JOIN_request.JoinAsRouter           = FALSE;
                            params.NLME_JOIN_request.RejoinNetwork          = TRUE;
                            params.NLME_JOIN_request.PowerSource            = NOT_MAINS_POWERED;
                            params.NLME_JOIN_request.RxOnWhenIdle           = FALSE;
                            params.NLME_JOIN_request.MACSecurity            = FALSE;
                            params.NLME_JOIN_request.ScanDuration           = 8;
                            params.NLME_JOIN_request.ScanChannels.Val       = ALLOWED_CHANNELS;
                            currentPrimitive = NLME_JOIN_request;
                        }
                        else
                        {
                            ConsolePutROMString( (ROM char *)"Trying to join network as a new device...\r\n" );
                            params.NLME_NETWORK_DISCOVERY_request.ScanDuration          = 8;
                            params.NLME_NETWORK_DISCOVERY_request.ScanChannels.Val      = ALLOWED_CHANNELS;
                            currentPrimitive = NLME_NETWORK_DISCOVERY_request;
                        }
                    }
                }
                else
                {
                    // See if I can do my own internal tasks.  We don't want to try to send a message
                    // if we just asked for one.
                    if (ZigBeeStatus.flags.bits.bDataRequestComplete & ZigBeeReady())
                    {

                        // ************************************************************************
                        // Place all processes that can send messages here.  Be sure to call
                        // ZigBeeBlockTx() when currentPrimitive is set to APSDE_DATA_request.
                        // ************************************************************************

                        if ( myStatusFlags.bits.bLightSwitchToggled )
                        {
                            // Send a light toggle message to the other node.
                            myStatusFlags.bits.bLightSwitchToggled = FALSE;

                            TxBuffer[TxData++] = APL_FRAME_TYPE_KVP | 1;    // KVP, 1 transaction
                            TxBuffer[TxData++] = APLGetTransId();
                            TxBuffer[TxData++] = APL_FRAME_COMMAND_SET | (APL_FRAME_DATA_TYPE_UINT8 << 4);
                            TxBuffer[TxData++] = OnOffSRC_OnOff & 0xFF;         // Attribute ID LSB
                            TxBuffer[TxData++] = (OnOffSRC_OnOff >> 8) & 0xFF;  // Attribute ID MSB
                            TxBuffer[TxData++] = LIGHT_TOGGLE;

                            #ifdef USE_BINDINGS
                                params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_NOT_PRESENT;
                            #else
                                params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_16_BIT;
                                params.APSDE_DATA_request.DstEndpoint = EP_LIGHT;
                                params.APSDE_DATA_request.DstAddress.ShortAddr = destinationAddress;
                            #endif

                            //params.APSDE_DATA_request.asduLength; TxData
                            params.APSDE_DATA_request.ProfileId.Val = MY_PROFILE_ID;
                            params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;
                            params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_ENABLE;
                            params.APSDE_DATA_request.TxOptions.Val = 0;
                            params.APSDE_DATA_request.SrcEndpoint = EP_LIGHT;
                            params.APSDE_DATA_request.ClusterId = OnOffSRC_CLUSTER;

                            ConsolePutROMString( (ROM char *)" Trying to send light switch message.\r\n" );

                            currentPrimitive = APSDE_DATA_request;
                        }
                        else
                        #ifdef USE_BINDINGS
                            if (myStatusFlags.bits.bBroadcastSwitchToggled)
                            {
                                // Send END_DEVICE_BIND_req

                                myStatusFlags.bits.bBroadcastSwitchToggled = FALSE;

                                TxBuffer[TxData++] = APL_FRAME_TYPE_MSG | 1;    // KVP, 1 transaction
                                TxBuffer[TxData++] = APLGetTransId();
                                #if defined(I_AM_LIGHT) && defined(I_AM_SWITCH)
                                    TxBuffer[TxData++] = 9; // Transaction Length
                                #else
                                    TxBuffer[TxData++] = 8; // Transaction Length
                                #endif

                                // Binding Target
                                TxBuffer[TxData++] = 0x00;      // Binding Target
                                TxBuffer[TxData++] = 0x00;
                                TxBuffer[TxData++] = EP_LIGHT;
                                TxBuffer[TxData++] = MY_PROFILE_ID_LSB;
                                TxBuffer[TxData++] = MY_PROFILE_ID_MSB;

                                #ifdef I_AM_LIGHT
                                    TxBuffer[TxData++] = 1;     // Input clusters
                                    TxBuffer[TxData++] = OnOffSRC_CLUSTER;
                                #else
                                    TxBuffer[TxData++] = 0;
                                #endif

                                #ifdef I_AM_SWITCH
                                    TxBuffer[TxData++] = 1;     // Output clusters
                                    TxBuffer[TxData++] = OnOffSRC_CLUSTER;
                                #else
                                    TxBuffer[TxData++] = 0;
                                #endif

                                params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_16_BIT;
                                params.APSDE_DATA_request.DstEndpoint = EP_ZDO;
                                params.APSDE_DATA_request.DstAddress.ShortAddr.Val = 0x0000;

                                //params.APSDE_DATA_request.asduLength; TxData
                                params.APSDE_DATA_request.ProfileId.Val = MY_PROFILE_ID;
                                params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;
                                params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_ENABLE;
                                params.APSDE_DATA_request.TxOptions.Val = 0;
                                params.APSDE_DATA_request.SrcEndpoint = EP_ZDO;
                                params.APSDE_DATA_request.ClusterId = END_DEVICE_BIND_req;

                                ConsolePutROMString( (ROM char *)" Trying to send END_DEVICE_BIND_req.\r\n" );

                                currentPrimitive = APSDE_DATA_request;
                            }
                        #else
                            if (myStatusFlags.bits.bBroadcastSwitchToggled)
                            {
                                // Send NWK_ADDR_req

                                myStatusFlags.bits.bBroadcastSwitchToggled = FALSE;

                                TxBuffer[TxData++] = APL_FRAME_TYPE_MSG | 1;    // KVP, 1 transaction
                                TxBuffer[TxData++] = APLGetTransId();
                                TxBuffer[TxData++] = 10; // Transaction Length

                                // IEEEAddr of the node we want to find.  !!!Must match our other PICDEM Z!!!
                                TxBuffer[TxData++] = 0x54;
                                TxBuffer[TxData++] = 0x00;
                                TxBuffer[TxData++] = 0x00;
                                TxBuffer[TxData++] = 0x00;
                                TxBuffer[TxData++] = 0x00;
                                TxBuffer[TxData++] = 0xa3;
                                TxBuffer[TxData++] = 0x04;
                                TxBuffer[TxData++] = 0x00;

                                // RequestType
                                TxBuffer[TxData++] = 0x00;

                                // StartIndex
                                TxBuffer[TxData++] = 0x00;

                                params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_16_BIT;
                                params.APSDE_DATA_request.DstEndpoint = EP_ZDO;
                                params.APSDE_DATA_request.DstAddress.ShortAddr.Val = 0xFFFF;

                                //params.APSDE_DATA_request.asduLength; TxData
                                params.APSDE_DATA_request.ProfileId.Val = MY_PROFILE_ID;
                                params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;
                                params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_ENABLE;
                                params.APSDE_DATA_request.TxOptions.Val = 0;
                                params.APSDE_DATA_request.SrcEndpoint = EP_ZDO;
                                params.APSDE_DATA_request.ClusterId = NWK_ADDR_req;

                                ConsolePutROMString( (ROM char *)" Trying to send NWK_ADDR_req.\r\n" );

                                currentPrimitive = APSDE_DATA_request;
                            }
                        #endif

                        // We've processed any key press, so re-enable interrupts.
                        RBIE = 1;
                    }

                    // If we don't have to execute a primitive, see if we need to request data from
                    // our parent, or if we can go to sleep.
                    if (currentPrimitive == NO_PRIMITIVE)
                    {
                        if (!ZigBeeStatus.flags.bits.bDataRequestComplete)
                        {
                            // We have not received all data from our parent.  If we are not waiting
                            // for an answer from a data request, send a data request.
                            if (!ZigBeeStatus.flags.bits.bRequestingData)
                            {
                                if (ZigBeeReady())
                                {
                                    // Our parent still may have data for us.
                                    params.NLME_SYNC_request.Track = FALSE;
                                    currentPrimitive = NLME_SYNC_request;
                                    ConsolePutROMString( (ROM char *)"Requesting data...\r\n" );
                                }
                            }
                        }
                        else
                        {
							if (myStatusFlags.bits.bTestePerformance && packetsNumber < maxPacketsNumber)
							{
								TICK currentTime;
								currentTime = TickGet();

								if (TickGetDiff(currentTime, lastTestePerformance) >= (ONE_SECOND/40)  & ZigBeeReady())
								//if (TickGetDiff(currentTime, lastTestePerformance) >= (ONE_SECOND/100)  & ZigBeeReady())
								{
									int i;
									BYTE sizePacket = 70;

		                        	//ConsolePutROMString( (ROM char *)"Hora de enviar novo pdu...\r\n" );

                            		TxBuffer[TxData++] = APL_FRAME_TYPE_MSG | 1;    // KVP, 1 transaction

                            		TxBuffer[TxData++] = APLGetTransId();
	                                TxBuffer[TxData++] = sizePacket; // Transaction Length

									for(i=0;i<sizePacket;i++)
									{
										TxBuffer[TxData++] = 0xEF;
									}

	                                params.APSDE_DATA_request.DstAddrMode = APS_ADDRESS_16_BIT;
	                                params.APSDE_DATA_request.DstEndpoint = EP_PERFORMANCE;
	                                params.APSDE_DATA_request.DstAddress.ShortAddr.Val = 0x0000;

		                            //params.APSDE_DATA_request.asduLength; TxData
		                            params.APSDE_DATA_request.ProfileId.Val = MY_PROFILE_ID;
		                            params.APSDE_DATA_request.RadiusCounter = DEFAULT_RADIUS;
		                            params.APSDE_DATA_request.DiscoverRoute = ROUTE_DISCOVERY_ENABLE;
		                            params.APSDE_DATA_request.TxOptions.Val = 0;
									params.APSDE_DATA_request.TxOptions.bits.acknowledged = FALSE;
params.MCPS_DATA_request.TxOptions.bits.acknowledged_transmission = 0;
		                            params.APSDE_DATA_request.SrcEndpoint = EP_PERFORMANCE;
		                            params.APSDE_DATA_request.ClusterId = Performance_CLUSTER;

                            		ConsolePutROMString( (ROM char *)" Trying to send performance message.\r\n" );

                            		currentPrimitive = APSDE_DATA_request;

									lastTestePerformance = currentTime;
									MESSAGE_INDICATION ^= 1;
								}
							}
							else if (myStatusFlags.bits.bTestePerformance && packetsNumber >= maxPacketsNumber)
							{
								MESSAGE_INDICATION = 0;
								BIND_INDICATION = 0;
								myStatusFlags.bits.bTestePerformance = FALSE;
							}
							else if (!ZigBeeStatus.flags.bits.bHasBackgroundTasks && myProcessesAreDone())
                            {
                                // We do not have a primitive to execute, we've extracted all messages
                                // that our parent has for us, the stack has no background tasks,
                                // and all application-specific processes are complete.  Now we can
                                // go to sleep.  Make sure that the UART is finished, turn off the transceiver,
                                // and make sure that we wakeup from key press.
                                ConsolePutROMString( (ROM char *)"Going to sleep...\r\n" );
                                while (!ConsoleIsPutReady());
                                APLDisable();
                                RBIE = 1;
                                SLEEP();
                                NOP();


                                // We just woke up from sleep. Turn on the transceiver and
                                // request data from our parent.
                                APLEnable();
                                params.NLME_SYNC_request.Track = FALSE;
                                currentPrimitive = NLME_SYNC_request;
                                ConsolePutROMString( (ROM char *)"Requesting data...\r\n" );
                            }
                        }
                    }
                }
                break;

            default:
                PrintChar( currentPrimitive );
                ConsolePutROMString( (ROM char *)" Unhandled primitive.\r\n" );
                currentPrimitive = NO_PRIMITIVE;
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
