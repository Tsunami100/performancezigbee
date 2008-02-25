/* otap_destination.h 
 * Mains application header

 * $Author: rgd04c $
 * $Date: 2006/08/31 20:58:30 $
 * $Name: BeeKitLaunch $
 */
 
//includes
//#include "device_header.h"
#include "pub_def.h"


//Prototypes
void OTAPInit(void);
UINT8 ACKResponse (UINT8 u8state);

//Defines
 /* LED Definitions */
 #define LED_ON                  0
 #define LED_OFF                 1

 #define DELAY_COUNT             0xB000
 #define LED_DELAY               0x0800

/* EVB */


/* ARD or EVB low power STOP3*/
#define KBDWAIT() _asm stop

#define MC13192_CLKO_FREQ    0x01 /* Sets the MC13192 CLKO frequency */
#define LOW_POWER_WHILE() _asm wait



/* Prototypes */
void OTAPFindReqProcess(void);
UINT8 OTAPDataRequestOK(void);
UINT8 OTAPIsValidCmd(UINT8 * Buf);
UINT8 OTAPIsPacketForMe(UINT8 * Buffer);
void OTAPDecodeCMD(UINT8 *Buf);
void OTAPRxOn(void);
void OTAPRxOff(void);
void OTAPTxOn(UINT8 *Response);
void OTAPClearEvents(UINT8 *pu8Data);
void OTAPReset(void);
UINT8 OTAPContextChange(void);
UINT8 OTAPValidateImage_OK(void);
extern void FLASHEraseImage(void);
extern UINT8 FLASHWrite(UINT8 *pData, UINT8 len);
UINT8 OTAPStartProcessEvent(UINT8 *Event);
UINT8 OTAPValidateStartRequestOK(void);
void OTAPScheduleFlashWrite(UINT8 * pData, UINT8 len);
void OTAPFlashExecute(void);
void OTAPScheduleFlashEraseImage(void);
void OTAPScheduleFlashContextChange(void);

/* The following defines should have different values */
#define OTAP_CMD_WREQ    0
#define OTAP_ACK_WACK    1
#define OTAP_ACK_WNAK    2
#define OTAP_CMD_SREQ    3
#define OTAP_ACK_SACK    4
#define OTAP_ACK_SNAK    5
#define OTAP_CMD_DREQ    6
#define OTAP_ACK_DACK    7
#define OTAP_ACK_DNAK    8
#define OTAP_CMD_EREQ    9
#define OTAP_ACK_EACK    10
#define OTAP_ACK_ENAK    11
#define OTAP_CMD_CREQ    12
#define OTAP_ACK_CACK    13
#define OTAP_ACK_CNAK    14

#define OTAP_RX_TIMEOUT  30
#define OTAP_NORESPONSE  31
#define OTAP_NOEVENT     32

#define OTAP_IDLE        50
#define OTAP_DATA        51
#define OTAP_END_OK      52
#define OTAP_CANCEL      53

#define OTAP_FLASH_IDLE        60
#define OTAP_FLASH_WRITE       61
#define OTAP_FLASH_ERASE_IMAGE 62
#define OTAP_FLASH_CONTEXT_CHANGE 63



//#define OTAP_DEBUG 1      
#define OFF FALSE
#define ON  TRUE
#define OTAP_BLOCKSIZE   64
#define OTAP_DELAY     250000
#define OTAP_TIMEOUT     0
#define MAX_TX_SIZE      125

#define OTAP_DEFAULT_ID "PERTTX01"

typedef struct 
{
  struct
  {
    int u7undef1 :7;
    int u1NVMOverwrite :1;
    UINT8 u8undef2;
  }Flags;
  UINT16 u16Size;
  UINT8 au8ImageName[16];
}tSREQ;

typedef struct 
{
  UINT8 au8Timeout[4];
  UINT8 au8OtapID[4];
  UINT8 au8AppID[16];
  UINT8 u8BlockSize;
}tSACK;

typedef struct 
{
  UINT8 u8NVMOverwrite;
  UINT16 u16ImageSize;
  UINT8 au8ImageName[16];
  UINT8 u8LRC;
  UINT16 u16CurrDSN;
  UINT8 u8NextACK;
  UINT16 u16BytesWriten;
}tOTAPTransaction;

