#ifndef __TORQUE_VSI_H
#define __TORQUE_VSI_H

#define NUM_TVSI 1

#if defined (USE_VSI_CC)
//#define TVSI_STATIONARY_FRAME		//comment this out to turn off the torque stationary frame controller
#endif

typedef struct TVSI_context
{
	struct VSI_context *VSI;
#if defined (TVSI_STATIONARY_FRAME)
	struct VSI_context *VSI_stationary;
#endif
	u32 InputAtTickCount;		//the tick count of the last time the inputs were updated (to avoid unnecessary dq transformations)
	double OmegaDA_Cmd;
	double OmegaDA_Current;
	double MaxPhaseCurrent;
	double Idq_ref[2];	
} TVSI_context;

extern int Init_TorqueVSI(u8 StartAllControllers);
extern int Stop_TorqueVSI_Controller(u32 tvsi);
extern int TorqueVSI_StopCommand(const char *szCmd, char *szResponse, void *CommDevice);
extern int TorqueVSI_StartCommand(const char *szCmd, char *szResponse, void *CommDevice);
extern int TorqueVSI_Command(const char * szCmd, char *szResponse, void *CommDevice);
#endif //__TORQUE_VSI_H
