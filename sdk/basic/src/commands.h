#ifndef __COMMANDS_H
#define __COMMANDS_H

extern void *StreamFIFOComm;
extern u32 StreamFIFO_MaxPacket;
extern char bPrimeStreamFIFO;	//set this to true if the streaming FIFO needs to be loaded from the main loop (instead of the callback)

extern int MakeMostFullLFR(char *szResponse, u32 uiMaxLength, u32 *SQnum);
extern int ReloadStreamFIFO(void * comm);

extern int CommandParser(const char * szCmd, char *szResponse, void *CommDevice);
extern u32 HandleCommandsUart(char *szRcvCmd, u32 state);

extern u32 HandleCommandsGeneral(char *szOutData, char *szRcvCmd, char *szNewData, u32 NewDataLength, u32 *state, void *ComDevice);

extern void HBA_100usTick();
extern void VSI_100usTick(void);
#endif //__COMMANDS_H


