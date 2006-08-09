/*********************************************************************
 *
 *                  PHY for the Chipcon 2420
 *
 *********************************************************************
 * FileName:        zPHY_CC2420.c
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

#include "ZigbeeTasks.h"
#include "zigbee.def"
#include "generic.h"
#include "zPHY.h"
//#include "zPHY_CC2420.h"
#include "zMAC.h"
//#include "zMAC_CC2420.h"
#include "MSPI.h"
#include "sralloc.h"

#include "console.h"

#if(RF_CHIP == CC2420)

void UserInterruptHandler(void);

PHY_PIB phyPIB;


extern MAC_TASKS_PENDING macTasksPending;
extern CURRENT_PACKET currentPacket;

typedef union _PHYPendingTasks
{
    struct
    {
        unsigned int PLME_SET_TRX_STATE_request_task :1;
        unsigned int PHY_RX :1;
    } bits;
    BYTE Val;
} PHY_PENDING_TASKS;

typedef struct _RX_DATA
{
    unsigned int size :7;
    unsigned int inUse :1;
} RX_DATA;

volatile RX_DATA RxData;
BYTE TRXCurrentState;
volatile PHY_PENDING_TASKS PHYTasksPending;
SAVED_BITS savedBits;
volatile MAC_FRAME_CONTROL pendingAckFrameControl;
//volatile BYTE OldACKSeqNum;

#if (RX_BUFFER_SIZE > 256)
    extern volatile WORD RxWrite;
    extern WORD RxRead;
#else
    extern volatile BYTE RxWrite;
    extern BYTE RxRead;
#endif

extern BYTE RxBuffer[RX_BUFFER_SIZE];

static unsigned char GIEH_backup;

BYTE PHYGet(void)
{
    BYTE toReturn;

    toReturn = RxBuffer[RxRead++];

    if(RxRead == RX_BUFFER_SIZE)
    {
        RxRead = 0;
    }

    return toReturn;
}

BYTE PHYHasBackgroundTasks(void)
{
    //PHY might want to check to verify that we are not in the middle of
    // a transmission before allowing the user to turn off the TRX
    return PHYTasksPending.Val;
}


void PHYInit(void)
{
    WORD i;
    CC2420_STATUS d;

    RxData.inUse = 0;
    PHYTasksPending.Val = 0;

    PHY_RESETn = 0;

    CLRWDT();
    i = 0xE000;
    PHY_VREG_EN = 1;
    while(++i){}
    CLRWDT();

    PHY_RESETn = 1;
    i = 500;
    while(i--);
    CLRWDT();

    i=0;

    while(1)
    {
        PHY_CSn_0();
        SPIPut(STROBE_SXOSCON);
        PHY_CSn_1();

        d.Val = SSPBUF;

        if(d.bits.XOSC16M_STABLE == 1)
        {
            break;
        }
        i++;
    }

    PHY_CSn_0();

    /* set up REG_MDMCTRL0 */
    // Set AUTOACK bit in MDMCTRL0 register.
    // 15:14 = '00' = Reserved
    // 13    = '0'  = Reserved frames are rejected
    // 12    = '?'  = '1' if this is coordinator, '0' if otherwise
    // 11    = '1'  = Address decoding is enabled
    // 10:8  = '010'    = CCA Hysteresis in DB - default value
    // 7:6   = '11' = CCA = 1, when RSSI_VAL < CCA_THR - CCA_HYST and not receiving valid IEEE 802.15.4 data
    // 5     = '1'  = Auto CRC
    // 4     = '1'  = Auto ACK
    // 3:0   = '0010' = 3 leading zero bytes - IEEE compliant.


    SPIPut(REG_MDMCTRL0);
#if defined(I_AM_COORDINATOR)
    // MSB = 0b0001 1010
    SPIPut(0x1A);
    // LSB = 0b1111 0010
    SPIPut(0xF2);
#else
    // MSB = 0b0000 1010
    SPIPut(0x0A);
    // LSB = 0b1111 0010
    SPIPut(0xF2);
#endif

    SPIPut(REG_MDMCTRL1);
    SPIPut(0x05);            // MSB
    SPIPut(0x00);            // LSB

    /* disable the MAC level security */
    SPIPut(REG_SECCTRL0);
    SPIPut(0x01);            // MSB
    SPIPut(0xC4);            // LSB

/*
 *  PA_LEVEL determiens output power of transciever
 *          According to Table 9 of CC2420 datasheet
 *
 *          PA_LEVEL (TXCTRL.LSB)       Output Power (dBm)  Current Consumtiion
 *          ====================================================================
 *          0xFF                        0                   17.4 mA
 *          0xFB                        -1                  16.5 mA
 *          0xF7                        -3                  15.2 mA
 *          0xF3                        -5                  13.9 mA
 *          0xEF                        -7                  12.5 mA
 *          0xEB                        -10                 11.2 mA
 *          0xE7                        -15                 9.9 mA
 *          0xE3                        -25                 8.5 mA
 */

    // Select desired TX output power level
    SPIPut(REG_TXCTRL);
    // As defined by Table 9 of CC2420 datasheet.
    SPIPut(0xA0);             // MSB
    SPIPut(PA_LEVEL);        // LSB

    // Set FIFOP threshold to full RXFIFO buffer
    SPIPut(REG_IOCFG0);
    SPIPut(0x00);
    SPIPut(0x7F);
    PHY_CSn_1();

    // Flush the TX FIFO
    PHY_CSn_0();
    SPIPut(STROBE_SFLUSHTX);
    PHY_CSn_1();

#if !defined(I_AM_END_DEVICE)
    // set the data pending bit
    PHY_CSn_0();
    SPIPut(STROBE_SACKPEND);
    PHY_CSn_1();
#endif

    // Flush the RX FIFO
    PHY_CSn_0();
    SPIPut(REG_RXFIFO | CMD_READ);
    SPIGet();
    PHY_CSn_1();
    PHY_CSn_0();
    SPIPut(STROBE_SFLUSHRX);
    SPIPut(STROBE_SFLUSHRX);
    PHY_CSn_1();

    return;
}

ZIGBEE_PRIMITIVE PHYTasks(ZIGBEE_PRIMITIVE inputPrimitive)
{
    if(inputPrimitive == NO_PRIMITIVE)
    {

        if(ZigBeeStatus.flags.bits.bRxBufferOverflow == 1)
        {
            ConsolePutROMString((rom char*)"RxBufferOverflow!\r\n");
        }

        /* manage background tasks here */
        /* phy background tasks */
        if(ZigBeeTxUnblocked)   //(TxFIFOControl.bFIFOInUse==0)
        {
            if(PHYTasksPending.bits.PHY_RX)
            {
                BYTE packetSize;

                if(CurrentRxPacket != NULL)
                {
                    return NO_PRIMITIVE;
                }

                packetSize = RxBuffer[RxRead];

                params.PD_DATA_indication.psdu = SRAMalloc(packetSize);

                if(params.PD_DATA_indication.psdu == NULL)
                {
                    return NO_PRIMITIVE;
                }

                /* save the packet head somewhere so that it can be freed later */
                if(CurrentRxPacket == NULL)
                {
                    CurrentRxPacket = params.PD_DATA_indication.psdu;

                    params.PD_DATA_indication.psduLength = packetSize;
                    RxRead++;
                    if(RxRead == RX_BUFFER_SIZE)
                    {
                        RxRead = 0;
                    }

                    while(packetSize--)
                    {
                        *params.PD_DATA_indication.psdu++ = PHYGet();
                    }

                    /* reset the psdu to the head of the alloc-ed RAM, just happens to me CurrentRxPacket */
                    params.PD_DATA_indication.psdu = CurrentRxPacket;

                    /* disable interrupts before checking to see if this was the
                        last packet in the FIFO so that if we get a packet(interrupt) after the check,
                        but before the clearing of the bit then the new indication will not
                        get cleared */

                    savedBits.bGIEH = GIEH;
                    GIEH = 0;

                    if(RxRead == RxWrite)
                    {
                        PHYTasksPending.bits.PHY_RX = 0;
                    }

                    GIEH = savedBits.bGIEH;

                    return PD_DATA_indication;
                }
            }

        }
        else
        {
//if (PHYTasksPending.bits.PHY_RX)
//{
//ConsolePut('!');
//}
            // if tx is already busy sending a packet then we can't RX a packet because of resource control issues
            return NO_PRIMITIVE;
        }
    }
    else
    {
        /* handle primitive here */
        switch(inputPrimitive)
        {
//            case PD_DATA_request:
//                  This is handled by CC2420 hardware
//                break;
//            case PLME_CCA_request:
//                  This is handled by CC2420 hardware
//                break;
//            case PLME_ED_request:
//                    This is handled by CC2420 hardware, just read variable out of CC2420
//                break;
//            case PLME_SET_request:
//                  User will access variables directly
//                break;
//            case PLME_GET_request:
//                  User will access variables directly
//                break;
            case PLME_SET_TRX_STATE_request:
                /* input_state RX_ON, TRX_OFF, FORCE_TRX_OFF, or TX_ON */
                /* returns PLME_SET_TRX_STATE_confirm with SUCCESS, RX_ON, TRX_OFF, TX_ON, BUSY_RX, or BUSY_TX */

                if(params.PLME_SET_TRX_STATE_request.state == TRXCurrentState)
                {
                    params.PLME_SET_TRX_STATE_confirm.status = params.PLME_SET_TRX_STATE_request.state;
                    break;
                }

                if(params.PLME_SET_TRX_STATE_request.state == phyRX_ON || params.PLME_SET_TRX_STATE_request.state == phyTRX_OFF)
                {
                    BYTE_VAL status;
                    /* get status of CC2420 */
                    status.Val = CC2420GetStatus();

                    if(status.bits.b3 == 1)
                    {
                        params.PLME_SET_TRX_STATE_confirm.status = phyBUSY_TX;
                        break;
                    }
                }

                /* if we are in the middle of an RX then don't allow switch to TX */
                if((params.PLME_SET_TRX_STATE_request.state == phyTX_ON || params.PLME_SET_TRX_STATE_request.state == phyTRX_OFF) && (TRXCurrentState == phyRX_ON) )
                {
                    if(PHY_SFD == 1)
                    {
                        params.PLME_SET_TRX_STATE_confirm.status = phyBUSY_RX;
                        PHYTasksPending.bits.PLME_SET_TRX_STATE_request_task = 1;
                        return PLME_SET_TRX_STATE_confirm;
                    }
                }

                if(params.PLME_SET_TRX_STATE_request.state == phyFORCE_TRX_OFF || params.PLME_SET_TRX_STATE_request.state == phyTRX_OFF)
                {
                    /* force it off here */
                    PHY_CSn_0();
                    SPIPut(STROBE_SRFOFF);
                    PHY_CSn_1();
                    TRXCurrentState = phyTRX_OFF;
                    /* SUCCESS */
                    params.PLME_SET_TRX_STATE_confirm.status = phySUCCESS;
                    break;
                }
                else if(params.PLME_SET_TRX_STATE_request.state == phyRX_ON)
                {
                    PHY_CSn_0();
                    SPIPut(STROBE_SRXON);
                    PHY_CSn_1();
                }
//don't use this one because it is automatically handled in the CC2420 when we use the STXONCCA command strobe
//                else if(params.PLME_SET_TRX_STATE_request.state == phyTX_ON)
//                {
//                    PHY_CSn_0();
//                    SPIPut(STROBE_STXON);
//                    PHY_CSn_1();
//                }

                /* passed all of the tests to change the state of the TRX */
                TRXCurrentState = params.PLME_SET_TRX_STATE_request.state;
                /* SUCCESS */
                params.PLME_SET_TRX_STATE_confirm.status = phySUCCESS;
                break;
        }
    }
    return NO_PRIMITIVE;
}

BYTE CC2420GetStatus(void)
{
    BYTE GIEHsave;
    BYTE status;

    GIEHsave = GIEH;
    GIEH = 0;
    PHY_CSn_0();
    status = SPIGet();
    PHY_CSn_1();
    GIEH = GIEHsave;
    return status;
}


#if defined(MCHP_C18)
    #pragma interruptlow HighISR
    void HighISR(void)
#elif defined(HITECH_C18)
    void interrupt HighISR(void)
#else
    void HighISR(void)
#endif
{
    if(CCP2IF)
    {
        if(CCP2IE)
        {
            BYTE_VAL ack_status;
            BYTE count;
            BYTE_VAL w;

    #if (RX_BUFFER_SIZE > 256)
            #error "Rx buffer must be <= 256"
    #else
            #define BUFFER_CAST BYTE
            BYTE RxBytesRemaining;
            BYTE OldRxWrite;
    #endif

            CCP2IF = 0;
            ack_status.Val = 0;
            count = 0;

            if(RxData.inUse == 0)
            {
                if(PHY_FIFO == 0)
                {
                    // Flush the RX FIFO
                    iPHY_CSn_0();
                    SPIPut(REG_RXFIFO | CMD_READ);
                    SPIGet();
                    iPHY_CSn_1();
                    iPHY_CSn_0();
                    SPIPut(STROBE_SFLUSHRX);
                    SPIPut(STROBE_SFLUSHRX);
                    iPHY_CSn_1();

                    return;
                }

                iPHY_CSn_0();
                SPIPut(REG_RXFIFO | CMD_READ);
                RxData.size=SPIGet();
                RxData.inUse=1;
            }
            else
            {
                // We are recovering from an overflow.  We already have the size.
                iPHY_CSn_0();
                SPIPut(REG_RXFIFO | CMD_READ);
            }

            /* save the old write so that we can restore it if there isn't enough room left */
            OldRxWrite = RxWrite;

            if(RxWrite < RxRead)
            {
                RxBytesRemaining = (BUFFER_CAST)(RxRead - RxWrite - 1);
            }
            else
            {
                RxBytesRemaining = (BUFFER_CAST)(RX_BUFFER_SIZE - 1 - RxWrite + RxRead);
            }

            w.Val = RxData.size;

            /* this is less then because we need one extra byte for the length (which worst case would make it equivent to less then or equal to )*/
            if(w.Val < RxBytesRemaining)
            {
                MAC_FRAME_CONTROL mfc;

                /* there is room in the buffer */
                RxData.inUse = 0;

                /* add the packet */
                RxBuffer[RxWrite++]=w.Val;

                if(RxWrite==RX_BUFFER_SIZE)
                {
                    RxWrite = 0;
                }

                while(w.Val--)
                {
                    //Note: I am counting on the fact that RxWrite doesn't get incremented here anymore such that the ACK packet doesn't get written into the Buffer and the RxWrite doesn't get modified.
                    RxBuffer[RxWrite]=SPIGet();

                    if(count==0)
                    {
                        //if the frame control indicates that this packet is an ack

                        mfc.word.byte.LSB=RxBuffer[RxWrite];

//only enable this code if you don't want to use autoAck
//                        if(mfc.bits.ACKRequest == 1)
//                        {
//                            iPHY_CSn_1();
//                            iPHY_CSn_0();
//                            SPIPut(STROBE_SACKPEND);
//                            iPHY_CSn_1();
//
//                            iPHY_CSn_0();
//                            SPIPut(REG_RXFIFO | CMD_READ);
//                        }

                        if(mfc.bits.FrameType == 0b010)
                        {
                            //it was an ack then set the ack_status.bits.b0 to 1 showing it was an ack
                            ack_status.bits.b0 = 1;
                            //ConsolePut('@');
                        }
                    }
                    else if(count==2)
                    {
                        //if we are reading the sequence number and the packet was an ack
                        if(ack_status.bits.b0)
                        {

                              if ((macTasksPending.bits.packetPendingAck) &&
                                  (RxBuffer[RxWrite] == currentPacket.sequenceNumber))
                              {
                                  // If this is the ACK we've been waiting for, set the flag to
                                  // send up the confirm and save the Frame Control.
                                  macTasksPending.bits.bSendUpMACConfirm = 1;
                                  pendingAckFrameControl = mfc;
                              }

                              // Throw away the ACK
                              while(w.Val--)
                              {
                                  SPIGet();
                              }
                              RxWrite = OldRxWrite;
                              goto DoneReceivingPacket;

                        }
                    }
                    count++;
                    RxWrite++;
                    //roll buffer if required
                    if(RxWrite==RX_BUFFER_SIZE)
                    {
                        RxWrite = 0;
                    }
                }

                if(RxWrite==0)
                {
                    w.Val = RxBuffer[RX_BUFFER_SIZE-1];
                }
                else
                {
                    w.Val = RxBuffer[RxWrite - 1];
                }

                if(w.bits.b7 == 0)
                {
                    /* crc failed.  Erase packet from the array */
                    RxWrite = OldRxWrite;
                    // Flush the RX FIFO
                    iPHY_CSn_0();
                    SPIPut(REG_RXFIFO | CMD_READ);
                    SPIGet();
                    iPHY_CSn_1();
                    iPHY_CSn_0();
                    SPIPut(STROBE_SFLUSHRX);
                    SPIPut(STROBE_SFLUSHRX);
                    iPHY_CSn_1();
                }
                else
                {
                    PHYTasksPending.bits.PHY_RX = 1;
                }
            }
            else
            {
                RxWrite = OldRxWrite;
                ZigBeeStatus.flags.bits.bRxBufferOverflow = 1;
            }

DoneReceivingPacket:

            iPHY_CSn_1();
        }
    }
    if(TMR0IF)
    {
        if(TMR0IE)
        {
            /* there was a timer overflow */
            TMR0IF = 0;
            timerExtension++;
        }
    }

    UserInterruptHandler();
}

//******************************************************************************
// Interrupt Vectors
//******************************************************************************

#if defined(MCHP_C18)
#pragma code highVector=0x08
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif

#if defined(MCHP_C18)
#pragma code lowhVector=0x18
void LowVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif


#endif

