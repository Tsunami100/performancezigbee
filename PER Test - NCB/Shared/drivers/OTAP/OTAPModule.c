/****************************************************************************
otap_destination.c


$Author: rgd04c $
$Date: 2006/10/03 23:05:56 $
$Name:  $
****************************************************************************/
#include <hidef.h> /* for EnableInterrupts macro */
#include "pub_def.h"
#include "simple_mac.h"
#include "LCD.h"
#include "SCI.h"
#include "mc13192_hw_config.h"
#include "bootloader user api.h"
#include "OTAPModule.h"
#include "freescale_radio_hardware.h"
#include "mcu_hw_config.h"
#include "OTAP_Mod_GB60_InternalFlash_api.h"


UINT8 gu8OTAPLastEvent = OTAP_NOEVENT;
UINT8 gu8OTAPRxState = OFF;
UINT8 gu8OTAPStatus = 0;
UINT8 gu8OTAPStatusAPI = 0;
UINT8 gu8Response = OTAP_NORESPONSE;

/*Global Variables*/
UINT8 *gpu8OTAPData = (UINT8 *) 0;
UINT8 gu8FlashReq = OTAP_FLASH_IDLE;
UINT8 gu8OTAPDataLen = 0;

/* OTAP Related information */
UINT8 MY_MAC_ADDR[] = OTAP_DEFAULT_ID;
UINT8 OTAP_ID[] = "r050";
UINT8 OTAP_APP_ID[] = "OTAP Demo App 01";
tOTAPTransaction gsOTAP;
#define OTAP_IMAGE_MAX_SIZE  (0x6E00/OTAP_BLOCKSIZE) // in packages
#define OTAP_DREQ_DELAY      25000

tRxPacket gsRxOtapPacket;
tTxPacket gsTxOtapPacket;
UINT8 gau8RxOtapDataBuffer[128]={0};
UINT8 gau8TxOtapDataBuffer[128]={0};
UINT16 gu16PacketNum = 0; 
UINT8 rxflag;
UINT16 gu16ByteNum = 0;
UINT8 gu8OtapStatus = 0;

#ifdef BOOTLOADER_ENABLED
    #define TIMEOUT_PERIOD 0x2000 /*Changed to 0x2000 due to lower UART baud rate. 28.09.04 MVC*/
#else
    #define TIMEOUT_PERIOD 0x1000
#endif BOOTLOADER_ENABLED

#define RETRY_COUNT 4

void OTAPActivate(void)
{
    UINT8 u8RetryNo = 0;
    UINT16 u16Count = 0;

  OTAPInit();
  #ifdef OTAP_DEBUG
  SCITransmitStr("\rOTAP Module debug\r");
  #endif
  gu8OTAPStatusAPI = TRUE;
     
  for(;;) {
#ifdef OTAP_DEBUG
    if (!(gu8OTAPLastEvent == OTAP_NOEVENT))
    {
      SCITransmitStr("\rState:");
      SCIStartTransmit(gu8OtapStatus + 0x30);
      SCITransmitStr("\rEvent:");
      SCIStartTransmit(gu8OTAPLastEvent + 0x30);
      gu8OTAPLastEvent = OTAP_NOEVENT;
    }
#endif
    
        /* TX/RX */
    OTAPTxOn(&gu8Response); /* Transmit last generated response */
    OTAPFlashExecute();
    if (gu8Response == OTAP_NORESPONSE){     
      OTAPRxOn();/* Enable Reception of packets */
    }
    switch (gu8OtapStatus) {
    case OTAP_IDLE:
      /* Execute actions according to the CMD received */
      if (OTAPStartProcessEvent(&gu8OTAPLastEvent))/* Set  event processor */
      {                                           /* semaphore */
        if (gu8OTAPLastEvent == OTAP_CMD_WREQ) /* Ping request */
        {
          OTAPFindReqProcess();
        } else if (gu8OTAPLastEvent == OTAP_CMD_SREQ) /* Start request for this device */
        {
          if (OTAPValidateStartRequestOK())
          {

            OTAPScheduleFlashEraseImage();
            gu8OtapStatus = OTAP_DATA;
            gu8Response = OTAP_ACK_SACK;
          } else
          {
            gu8Response = OTAP_ACK_SNAK;
            gu8OtapStatus = OTAP_CANCEL;
          }
        } else /* Any other command on this state will be ignored */
          ;
      }
      OTAPClearEvents(&gu8OTAPLastEvent); /* Clear the last event */
      break;

    case OTAP_DATA:
      /* Execute actions according to the CMD received */
      if (OTAPStartProcessEvent(&gu8OTAPLastEvent))/* Set  event processor */
      {                                           /* semaphore */
        if (gu8OTAPLastEvent == OTAP_CMD_DREQ) /* Data request */
        {
          if (OTAPDataRequestOK()) /* Validate Data */
          {
            gu8Response = OTAP_ACK_DACK; /* Next response to OTAP Programmer will be DACK -- good reception */
          } else
          {
            gu8OtapStatus = OTAP_CANCEL;  /* Move state machine to state CANCEL */
            gu8Response = OTAP_ACK_DNAK; /* Data not valid: Next response is DNAK */
          }
        } else if (gu8OTAPLastEvent == OTAP_CMD_EREQ) /* End of Data request for this device */
        {
          if (OTAPValidateImage_OK())
          {
            gu8OtapStatus = OTAP_END_OK;  /* Image successfully received, Move state machine to state END_OK */
            gu8Response = OTAP_ACK_EACK; /* Image successfully received: respond with ack */
            
          } else
          {
            gu8OtapStatus = OTAP_CANCEL;  /* Image invalid: Move state machine to state CANCEL */
            gu8Response = OTAP_ACK_ENAK; /* Image invalid: Next response is ENAK */
          }
        } else if (gu8OTAPLastEvent == OTAP_CMD_CREQ) /* Cancel request for this device */
        {
          gu8OtapStatus = OTAP_CANCEL;  /* Move state machine to state CANCEL */
          gu8Response = OTAP_ACK_CACK; /* Next response is CACK */
        } else if (gu8OTAPLastEvent == OTAP_CMD_SREQ) /* Start request for this device */
        {
          gu8OtapStatus = OTAP_DATA;
          gu8Response = OTAP_ACK_SACK;
        } else /* Any other command on this state will be ignored */
          ;
      }
      OTAPClearEvents(&gu8OTAPLastEvent); /* Clear the last event */
      break;
    
    case OTAP_END_OK:
      OTAPRxOff();/* Disable Reception of packets */
      //gu8Response = OTAP_NORESPONSE;
      OTAPScheduleFlashContextChange(); /* Perform the context change and boot from new app */
      break;
    
    case OTAP_CANCEL:
      OTAPRxOff();/* Disable Reception of packets */
      //gu8Response = OTAP_NORESPONSE;
      (void) OTAPReset(); /* Reset application and restart original application */
      break;
    }
    
  }
}

void OTAPInit(void) {
  UINT8 i;
  gsOTAP.u8NVMOverwrite = 0x00;
  gsOTAP.u16ImageSize = 0x0000;
  for (i=0; i < 16; i++)
    gsOTAP.au8ImageName[i] = ' ';
  gsOTAP.u8LRC = 0;
  gsOTAP.u16CurrDSN = 0x0000;
  gsOTAP.u8NextACK = 0;
  

  #pragma MESSAGE DISABLE C2705
  (void) FInit((UINT16) F8MHZ);        //Init flash register values
  #pragma MESSAGE WARNING C2705


    /* Initialize the packet.*/
  gsTxOtapPacket.u8DataLength = 0;
  gsTxOtapPacket.pu8Data = &gau8TxOtapDataBuffer[0];
  gsRxOtapPacket.u8DataLength = 0;
  gsRxOtapPacket.pu8Data = &gau8RxOtapDataBuffer[0];
  gsRxOtapPacket.u8MaxDataLength = 128;
  gsRxOtapPacket.u8Status = 0;

  RadioInit();
   
  (void) MLMEMC13192PAOutputAdjust(NOMINAL_POWER);    //Set MAX power setting

  (void) MLMESetMC13192ClockRate(0);  /* Set initial Clk speed */
  UseExternalClock();    /* switch clock sources */

  
  EnableInterrupts;
        
  (void) MLMESetChannelRequest(15);
  gu8OtapStatus = OTAP_IDLE;


                               
}

/*OTAP Callback function for packet reception */
void OTAPDataIndication(tRxPacket *sRxOtapPacket) 
{
  gu8OTAPRxState = OFF; /* Indicate the OTAP module that RX is now off */
  if (sRxOtapPacket->u8Status == SUCCESS) 
  {
    /* Packet received */
    if (!OTAPIsValidCmd(&sRxOtapPacket->pu8Data[0]))
      return;
    if (!OTAPIsPacketForMe(&sRxOtapPacket->pu8Data[0]))
      return;
    OTAPDecodeCMD(&sRxOtapPacket->pu8Data[0]);

  } else if (sRxOtapPacket->u8Status == TIMEOUT) 
  {
    /* Received TIMEOUT */
    gu8OTAPLastEvent = OTAP_RX_TIMEOUT;
  }        
}

/*Name: OTAPIsValidCmd
  Desc: This function evaluates the payload of the received packet
        and determines whether it is a valid OTAP command (for devices)
  Para: Pointer to the data buffer of the packet payload
  Returns: UINT8:
          - TRUE:  Valid OTAP command detected
          - FALSE: Invalid OTAP command detected*/
UINT8 OTAPIsValidCmd(UINT8 * Buf)
{
  if (Buf[0] == 'O' &&
      Buf[1] == 'T' &&
      Buf[2] == 'A' &&
      Buf[3] == 'P' &&
      Buf[4] == '-' &&
      Buf[9] == '-')
  {
    return TRUE;
  } else
  {
    return FALSE;
  }
}


/*Name: OTAPIsPacketForMe
  Desc: This function evaluates the payload of the received packet
        and determines whether it was a packet for this device
  Para: Pointer to the data buffer of the packet payload
  Returns: UINT8:
          - TRUE:  The packet is intented to be processed by this device
          - FALSE: This packet is not supposed to be processed here*/
UINT8 OTAPIsPacketForMe(UINT8 * Buf)
{
  /* If WREQ, then return true, since it is a broadcast */
  if (Buf[5] == 'W' &&
      Buf[6] == 'R' &&
      Buf[7] == 'E' &&
      Buf[8] == 'Q' )
  {
    return TRUE;
  }
  
  /* If any other CMD, validate Address */
  if (Buf[10] == MY_MAC_ADDR[0] &&
      Buf[11] == MY_MAC_ADDR[1] &&
      Buf[12] == MY_MAC_ADDR[2] &&
      Buf[13] == MY_MAC_ADDR[3] &&
      Buf[14] == MY_MAC_ADDR[4] &&
      Buf[15] == MY_MAC_ADDR[5] &&
      Buf[16] == MY_MAC_ADDR[6] &&
      Buf[17] == MY_MAC_ADDR[7] )
  {
    return TRUE;
  }
  
  return FALSE;
}

/*Name: OTAPDecodeCMD
  Desc: This function decodes the OTAP command and updates the global 
        variables required for the execution of this command.
  Para: Pointer to the data buffer of the packet payload
  Returns: void*/
void OTAPDecodeCMD(UINT8 *Buff)
{
  if (Buff[6] == 'R' &&
      Buff[7] == 'E' &&
      Buff[8] == 'Q' )
  {
    if(Buff[5] == 'S') /* Is it a START cmd? */
    {
      gu8OTAPLastEvent = OTAP_CMD_SREQ;
      
    } else if (Buff[5] == 'D') /* Is it a DATA cmd? */
    {
      gu8OTAPLastEvent = OTAP_CMD_DREQ;
      
      
    } else if (Buff[5] == 'E') /* Is it a END OF IMAGE cmd? */
    {
      gu8OTAPLastEvent = OTAP_CMD_EREQ;
      
      
    } else if (Buff[5] == 'C') /* Is it a CANCEL cmd? */
    {
      gu8OTAPLastEvent = OTAP_CMD_CREQ;
      
    } else if (Buff[5] == 'W') /* Is it a PING (WHO) cmd? */
    {
      gu8OTAPLastEvent = OTAP_CMD_WREQ;
      
    } else /* No standard Request found: ignore it */
    {
      gu8OTAPLastEvent = OTAP_NOEVENT;
    }
    
  } else /* No standard OTAP Packet found: ignore it */
  {
      gu8OTAPLastEvent = OTAP_NOEVENT;
  }

  return;

}


/*Name: OTAPRxOn
  Desc: Turns the receiver on (only if it is not currently on)
  Para: void
  returns: void*/
void OTAPRxOn(void)
{
#ifdef OTAP_DEBUG
  UINT8 Array[10];
  SCIgets("\r:", &Array[0],2);
  gu8OTAPLastEvent = ((Array[0] - 0x30)*10) + Array[1] - 0x30;
  if (gu8OTAPLastEvent == OTAP_CMD_SREQ)
  {
    gau8RxOtapDataBuffer[0]  = 'O';
    gau8RxOtapDataBuffer[1]  = 'T';
    gau8RxOtapDataBuffer[2]  = 'A';
    gau8RxOtapDataBuffer[3]  = 'P';
    gau8RxOtapDataBuffer[4]  = '-';
    gau8RxOtapDataBuffer[5]  = 'S';
    gau8RxOtapDataBuffer[6]  = 'R';
    gau8RxOtapDataBuffer[7]  = 'E';
    gau8RxOtapDataBuffer[8]  = 'Q';
    gau8RxOtapDataBuffer[9]  = '-';
    gau8RxOtapDataBuffer[10] = ' ';
    gau8RxOtapDataBuffer[11] = ' ';
    gau8RxOtapDataBuffer[12] = ' ';
    gau8RxOtapDataBuffer[13] = ' ';
    gau8RxOtapDataBuffer[14] = ' ';
    gau8RxOtapDataBuffer[15] = ' ';
    gau8RxOtapDataBuffer[16] = ' ';
    gau8RxOtapDataBuffer[17] = ' ';
    gau8RxOtapDataBuffer[18] = '0';
    gau8RxOtapDataBuffer[19] = '0';
    gau8RxOtapDataBuffer[20] = 0x0a;
    gau8RxOtapDataBuffer[21] = 0x00;
    gau8RxOtapDataBuffer[22] = ' ';
    gau8RxOtapDataBuffer[23] = ' ';
    gau8RxOtapDataBuffer[24] = ' ';
    gau8RxOtapDataBuffer[25] = ' ';
  } else   if (gu8OTAPLastEvent == OTAP_CMD_DREQ)
  {
    gau8RxOtapDataBuffer[0]  = 'O';
    gau8RxOtapDataBuffer[1]  = 'T';
    gau8RxOtapDataBuffer[2]  = 'A';
    gau8RxOtapDataBuffer[3]  = 'P';
    gau8RxOtapDataBuffer[4]  = '-';
    gau8RxOtapDataBuffer[5]  = 'D';
    gau8RxOtapDataBuffer[6]  = 'R';
    gau8RxOtapDataBuffer[7]  = 'E';
    gau8RxOtapDataBuffer[8]  = 'Q';
    gau8RxOtapDataBuffer[9]  = '-';
    gau8RxOtapDataBuffer[10] = (UINT8)(gsOTAP.u16CurrDSN >> 8);
    gau8RxOtapDataBuffer[11] = (gsOTAP.u16CurrDSN & 0x00FF);
    gau8RxOtapDataBuffer[12] = ' ';
    gau8RxOtapDataBuffer[13] = ' ';
    gau8RxOtapDataBuffer[14] = ' ';
    gau8RxOtapDataBuffer[15] = ' ';
    gau8RxOtapDataBuffer[16] = ' ';
    gau8RxOtapDataBuffer[17] = ' ';
    gau8RxOtapDataBuffer[18] = '0';
    gau8RxOtapDataBuffer[19] = '0';
    gau8RxOtapDataBuffer[20] = ' ';
    gau8RxOtapDataBuffer[21] = ' ';
    gau8RxOtapDataBuffer[22] = ' ';
    gau8RxOtapDataBuffer[23] = ' ';
    gau8RxOtapDataBuffer[24] = ' ';
    gau8RxOtapDataBuffer[25] = ' ';
  }
  if(Array[0] == 'C' && Array[1] == 'C')
  {
     OTAPScheduleFlashContextChange();
     OTAPFlashExecute();
  }
  return;
#endif
  if (gu8OTAPRxState == OFF)
  {
    (void) MLMERXEnableRequest(&gsRxOtapPacket, OTAP_TIMEOUT); /* Enable RX only if it is off */
    gu8OTAPRxState = ON;
  }
}

/*Name: OTAPRxOff
  Desc: Turns the receiver off
  Para: void
  returns: void*/
void OTAPRxOff(void)
{
  (void) MLMERXDisableRequest(); /* Disable RX */
  gu8OTAPRxState = OFF;
}

/*Name: OTAPTxOn
  Desc: Transmits the last configured packet with the packet type requested
  Para: PacketType: Type of packet to be transmitted
  returns: void*/
void OTAPTxOn(UINT8 *Response)
{
  UINT8 i = 0, pt = 0, a = 0, n = 0;
  if (*Response == OTAP_NORESPONSE)
    return;
  if (gu8OTAPRxState == OFF)
  {
    
    if (*Response == OTAP_ACK_WACK)
    {
      pt = 'W';
      a  = 'A';
      n  = 'C';
    } else if (*Response == OTAP_ACK_WNAK)
    {
      pt = 'W';
      a  = 'N';
      n  = 'A';
    } else if (*Response == OTAP_ACK_SACK)
    {
      pt = 'S';
      a  = 'A';
      n  = 'C';
    } else if (*Response == OTAP_ACK_SNAK)
    {
      pt = 'S';
      a  = 'N';
      n  = 'A';
    } else if (*Response == OTAP_ACK_DACK)
    {
      pt = 'D';
      a  = 'A';
      n  = 'C';
    } else if (*Response == OTAP_ACK_DNAK)
    {
      pt = 'D';
      a  = 'N';
      n  = 'A';
    } else if (*Response == OTAP_ACK_EACK)
    {
      pt = 'E';
      a  = 'A';
      n  = 'C';
    } else if (*Response == OTAP_ACK_ENAK)
    {
      pt = 'E';
      a  = 'N';
      n  = 'A';
    } else if (*Response == OTAP_ACK_CACK)
    {
      pt = 'C';
      a  = 'A';
      n  = 'C';
    } else if (*Response == OTAP_ACK_CNAK)
    {
      pt = 'C';
      a  = 'N';
      n  = 'A';
    } else 
    {
      return;
    }

    gsTxOtapPacket.pu8Data[0]  = 'O';
    gsTxOtapPacket.pu8Data[1]  = 'T';
    gsTxOtapPacket.pu8Data[2]  = 'A';
    gsTxOtapPacket.pu8Data[3]  = 'P';
    gsTxOtapPacket.pu8Data[4]  = '-';
    gsTxOtapPacket.pu8Data[5]  = pt;
    gsTxOtapPacket.pu8Data[6]  = a;
    gsTxOtapPacket.pu8Data[7]  = n;
    gsTxOtapPacket.pu8Data[8]  = 'K';
    gsTxOtapPacket.pu8Data[9]  = '-';

    (void) MCPSDataRequest(&gsTxOtapPacket); /* Transmit packet only if RX is off */
    for(i = 0; i < MAX_TX_SIZE; i++)
      gsTxOtapPacket.pu8Data[i] = 0;
    *Response = OTAP_NORESPONSE;
  }
}

/*Name: OTAPFindReqProcess
  Desc: Determines if WREQ included this device's address in the 
        shut-up list.  If not, it creates the TX packet to be transmitted
        in the next state of the main state machine.
  Para: void
  returns: void*/
void OTAPFindReqProcess(void){
  UINT8 i = 0;
  UINT8 flag = 0;
  UINT8 *pData = &gau8RxOtapDataBuffer[10]; /* RX buffer, payload section */
  
  while (i < (gsRxOtapPacket.u8DataLength-10))
  {
    if (MY_MAC_ADDR[i%8] == pData[i])
      flag++;
    else 
      flag = 0;
    
    if (flag == 0){
      i = ((i>>3) + 1)<<3;
      flag = 0;
    } else
      i++;
    if (flag == 8)
      break;
  }
  
  if (flag != 8)
  {
    gau8TxOtapDataBuffer[0]  = 'O';
    gau8TxOtapDataBuffer[1]  = 'T';
    gau8TxOtapDataBuffer[2]  = 'A';
    gau8TxOtapDataBuffer[3]  = 'P';
    gau8TxOtapDataBuffer[4]  = '-';
    gau8TxOtapDataBuffer[5]  = 'W';
    gau8TxOtapDataBuffer[6]  = 'A';
    gau8TxOtapDataBuffer[7]  = 'C';
    gau8TxOtapDataBuffer[8]  = 'K';
    gau8TxOtapDataBuffer[9]  = '-';
    gau8TxOtapDataBuffer[10] = MY_MAC_ADDR[0];
    gau8TxOtapDataBuffer[11] = MY_MAC_ADDR[1];
    gau8TxOtapDataBuffer[12] = MY_MAC_ADDR[2];
    gau8TxOtapDataBuffer[13] = MY_MAC_ADDR[3];
    gau8TxOtapDataBuffer[14] = MY_MAC_ADDR[4];
    gau8TxOtapDataBuffer[15] = MY_MAC_ADDR[5];
    gau8TxOtapDataBuffer[16] = MY_MAC_ADDR[6];
    gau8TxOtapDataBuffer[17] = MY_MAC_ADDR[7];
    gau8TxOtapDataBuffer[18] = 0;
    gsTxOtapPacket.u8DataLength = 18;
    gu8Response = OTAP_ACK_WACK;
    return;
  }
    gu8Response = OTAP_NORESPONSE;
}


/*Name: OTAPClearEvents
  Desc: Clears the Events in the variable.
        This version can only have one event active.
  Para: Pointer to the Variable that holds an event.
  returns: void*/
void OTAPClearEvents(UINT8 *pu8Data)
{
#ifndef OTAP_DEBUG
  *pu8Data = OTAP_NOEVENT;
#endif  
  return;
}


/* INCOMPLETE
Name: OTAPDataRequestOK
  Desc: Validates the data in the global data pointer
  Para: void
  returns: UINT8:
          TRUE:  Data Valid
          FALSE: Data Invalid*/
UINT8 OTAPDataRequestOK(void)
{
  UINT16 dsn = 0x0000;
  dsn = (UINT16) (gau8RxOtapDataBuffer[18]<<8) | (gau8RxOtapDataBuffer[19]);
  if (gsOTAP.u8NextACK)
  {
    if ((gsOTAP.u16CurrDSN-1) == dsn) 
    {
      /* Data Valid: Write received data to flash */
      OTAPScheduleFlashWrite(&gau8RxOtapDataBuffer[20], OTAP_BLOCKSIZE);
      gsOTAP.u16CurrDSN--;
    }
  }
  return gsOTAP.u8NextACK;
}

/*Name: OTAPValidateImage_OK
  Desc: Validates the data in the image region of flash
  Para: void
  returns: UINT8:
          TRUE:  Data Valid
          FALSE: Data Invalid*/
UINT8 OTAPValidateImage_OK(void)
{
  
  return FImageValidation();
}

/*Name: OTAPContextChange
  Desc: Schedules a context change in the flash library
  Para: void
  returns: void*/
void OTAPScheduleFlashContextChange(void)
{
  if (gu8FlashReq == OTAP_FLASH_IDLE)
    gu8FlashReq = OTAP_FLASH_CONTEXT_CHANGE;
  return;
}

/* INCOMPLETE
Name: OTAPReset
  Desc: Validates the data in the global data pointer
  Para: void
  returns: UINT8:
          TRUE:  Data Valid
          FALSE: Data Invalid*/
void OTAPReset(void)
{
  (void)gpu8OTAPData;
  for(;;)
    ;
  //return TRUE;
}

/*Name: OTAPStartProcessEvent
  Desc: Determines if a new event is available.
  Para: Pointer to the Event pointer.
  returns: UINT8:
          TRUE:  There is an event waiting, process can start
          FALSE: There is not an event waiting, process cannot start*/
UINT8 OTAPStartProcessEvent(UINT8 *Event)
{
  if (*Event == OTAP_NOEVENT ||
      *Event == OTAP_RX_TIMEOUT)
    return FALSE;/* Do not start processing events, there is none*/
  else
    return TRUE;  /* Start processing of event */
}

/*Name: OTAPValidateStartRequestOK
  Desc: Determines if the SREQ command received is valid for this device
  Para: void
  returns: UINT8:
          TRUE:  Valid SREQ
          FALSE: Invalid SREQ*/
UINT8 OTAPValidateStartRequestOK(void)
{
  tSREQ *sSREQ; /* Pointer used for formating of incoming SREQ */
  UINT8 ret = TRUE;
  UINT8 i = 0;
  /* Format received packet for further processing*/
  sSREQ = (tSREQ *) &gau8RxOtapDataBuffer[18];
  
  /* Evaluation of the specific parameters*/
  if (sSREQ->u16Size > OTAP_IMAGE_MAX_SIZE)
  {
    ret = FALSE;
  } else
  {
    if(sSREQ->Flags.u1NVMOverwrite)
      gsOTAP.u8NVMOverwrite = TRUE;
    else
      gsOTAP.u8NVMOverwrite = FALSE;
    
    gsOTAP.u16ImageSize = (sSREQ->u16Size)*OTAP_BLOCKSIZE;
    gsOTAP.u16CurrDSN = sSREQ->u16Size;

    for (i = 0; i < 16; i++)
      gsOTAP.au8ImageName[i] = sSREQ->au8ImageName[i];
    
    gsOTAP.u8LRC = 0;
    
    #pragma MESSAGE DISABLE C2705
    #pragma MESSAGE DISABLE C5900
        
    gau8TxOtapDataBuffer[10] = (UINT8)((UINT32)OTAP_DELAY >> 24);
    gau8TxOtapDataBuffer[11] = (UINT8)((UINT32)OTAP_DELAY >> 16);
    gau8TxOtapDataBuffer[12] = (UINT8)((UINT32)OTAP_DELAY >> 8);
    gau8TxOtapDataBuffer[13] = (UINT8)((UINT32)OTAP_DELAY);
    
    gau8TxOtapDataBuffer[14] = (UINT8)((UINT32)OTAP_ID >> 24);
    gau8TxOtapDataBuffer[15] = (UINT8)((UINT32)OTAP_ID >> 16);
    gau8TxOtapDataBuffer[16] = (UINT8)((UINT32)OTAP_ID >> 8);
    gau8TxOtapDataBuffer[17] = (UINT8)((UINT32)OTAP_ID);

    #pragma MESSAGE WARNING C2705


    
    gau8TxOtapDataBuffer[24] = (UINT8)((UINT32)OTAP_BLOCKSIZE);
    
    gsTxOtapPacket.u8DataLength = 25;
  }
  
  return ret;
}

/*Name: OTAPScheduleFlashWrite
  Desc: Schedules a FlashWrite
  Para: pData: pointer to the data buffer to write
        len: length of the data buffer to write
  returns: void*/
void OTAPScheduleFlashWrite(UINT8 * pData, UINT8 len)
{
  UINT8 i;
  if(gu8FlashReq != OTAP_FLASH_IDLE)
  {
    return;
  } else 
  {
    gpu8OTAPData = &pData[0];
    gu8OTAPDataLen = len;
    /* Get new LRC value */
    for (i=0; i<len; i++)
      gsOTAP.u8LRC ^= gpu8OTAPData[i];
      gu8FlashReq = OTAP_FLASH_WRITE;

  }
  
}

/*Name: OTAPScheduleFlashErase
  Desc: Schedules a FlashErase
  Para: void
  returns: void*/
void OTAPScheduleFlashEraseImage(void)
{
  //UINT8 i;
  if(gu8FlashReq != OTAP_FLASH_IDLE)
  {
    return;
  } else 
  {
    gu8FlashReq = OTAP_FLASH_ERASE_IMAGE;
  }
  
}


void OTAPFlashExecute(void)
{
  UINT8 i=0,o=0, ret=F_OK;
  if(gu8FlashReq == OTAP_FLASH_WRITE)
  {  DisableInterrupts;
    /* Programming */
//    SCITransmitStr("\rFlash write");

    while(i < gu8OTAPDataLen &&
          ret == F_OK        &&
          gsOTAP.u16BytesWriten <= (gsOTAP.u16ImageSize+1))
    {
      ret = FWriteByte(gpu8OTAPData[i]);
//      SCIStartTransmit(gpu8OTAPData[i]);
      gsOTAP.u16BytesWriten++;
      i++;
    }
    if (gsOTAP.u16BytesWriten > (gsOTAP.u16ImageSize+1))
      ret = F_ERR;
     EnableInterrupts;
  } else   if(gu8FlashReq == OTAP_FLASH_ERASE_IMAGE)
  { DisableInterrupts;
    /* Erasing Image */
    (void) FSetSize(gsOTAP.u16ImageSize);
    ret = FEraseImage();
//    SCITransmitStr("\rFlash erase");
    EnableInterrupts;
  } else   if(gu8FlashReq == OTAP_FLASH_CONTEXT_CHANGE)
  {
    /* Context Change */
    DisableInterrupts;
    for(i=0;i++;i<100)
      for(o=0;o++;o<100);
    FContextChange();
    (void) OTAPReset();
  }


  if (ret == F_OK)
    gsOTAP.u8NextACK = TRUE;
  else
    gsOTAP.u8NextACK = FALSE;    
  
  gu8FlashReq = OTAP_FLASH_IDLE;
}




