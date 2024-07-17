#ifndef __HBRIDGE_CC_H
#define  __HBRIDGE_CC_H

extern int Init_HBridge_CC(u8 StartAllControllers);
extern int HBridge_CC_StartCommand(const char *szCmd, char *szResponse, void *CommDevice);
extern int Start_HBridge_CC_Controller(u32 HB);
extern int Stop_HBridge_CC_Controller(u32 HB);
extern int HBridge_CC_Command(const char * szCmd, char *szResponse, void *CommDevice);
extern int HBridge_CC_SetCurrentRef(u32 hbridge, double reference);
extern int HBridge_CC_StopCommand(const char *szCmd, char *szResponse, void *CommDevice);

typedef u8 (*HBridge_input)(void *arg, void *VSIcontext);
typedef void (*Hbridge_output)(void *arg, void *VSIcontext);

#define NUM_HBRIDGE 6

typedef struct HB_context
{
	pi_house c;
	u32 InputAtTickCount;		//the tick count of the last time the inputs were updated (to avoid unnecessary computation)
	double I;
	u8 d[2];				//[0] for leg 1, [1] for leg 2
	HBridge_input get_input;
	void * input_arg;
	Hbridge_output write_output;
	void * output_arg;
} HB_context;

extern HB_context HB_Controllers[NUM_HBRIDGE];
#endif 
