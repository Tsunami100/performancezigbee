#include "app_config.h"
extern UINT8 MCPSDataRequest(tTxPacket *psTxPacket);
extern UINT8 MLMERXEnableRequest(tRxPacket *psRxPacket, UINT32 u32Timeout);
extern UINT8 MLMERXDisableRequest(void);
extern UINT8 MLMEHibernateRequest(void);
extern UINT8 MLMEDozeRequest(UINT32 u32Timeout);
extern UINT8 MLMEWakeRequest(void);
extern UINT8 MLMESetChannelRequest(UINT8 u8Channel);
extern UINT8 MLMESetMC13192ClockRate(UINT8 u8Freq);
extern UINT8 MLMESetMC13192TmrPrescale (UINT8 u8Freq);
extern UINT8 MLMEMC13192XtalAdjust(UINT8 u8ReqValue);
extern UINT8 MLMEEnergyDetect(void);
extern UINT8 MLMEMC13192SoftReset(void);
extern UINT8 MLMELinkQuality (void);
extern UINT8 MLMEMC13192FEGainAdjust(UINT8 u8GainValue);
extern UINT8 MLMEMC13192PAOutputAdjust(UINT8 u8PaValue);
extern UINT8 MLMEGetRficVersion(void);
extern void  MLMETestMode (tTxPacket *psPacket, UINT8 u8mode);
extern UINT8 MLMEScanRequest(UINT8 flags, UINT8 *pu8ChannelScan);
extern void MCUInit(void);
extern void UseExternalClock(void);
extern void UseMcuClock(void);


extern void MC13192DisableInterrupts(void);
extern void MC13192RestoreInterrupts(void);

extern void MC13192ContReset(void);
extern void MC13192Restart(void);
