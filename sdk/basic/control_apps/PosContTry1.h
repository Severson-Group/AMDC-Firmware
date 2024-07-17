#ifndef __POS_CONT_TRY1_H
#define __POS_CONT_TRY1_H

//#define USE_H_BRIDGE_CC			//uncomment this for HBridge CC (Bridge config)
//#define USE_H_BRIDGE_DQ_CC		//uncomment this for using Hbridges controlled by a dq0 controller
#define USE_VSI_CC					//uncomment this for VSI CC (Parallel config)
#define VSI_ISOLATED				//uncomment this if using isolated VSI's in parallel config

#define VSI_BOT_INDEX 0
#define VSI_TOP_INDEX 1


#define POS_BOT_INDEX 0
#define POS_TOP_INDEX 1

#define POS_XY0_INDEX 0
#define POS_THXY_INDEX 1

typedef struct pid_house
{
	pid_controller pid;
	controller_context io;
	controller_holder ctxt;
	double input;
	double output;
	double reference;
	u8 updated;	//set to TRUE everytime this controller output is updated, cleared everytime the output is used
	//1 to 1 double adc_scale;		//3 amps per volt, wound around twice gives me a factor of 3/2
} pid_house;

typedef struct PosCntVSI_context
{
	pid_house x[2];
	pid_house y[2];
#if defined(USE_VSI_CC) || defined(USE_H_BRIDGE_DQ_CC)
	struct VSI_context *VSItop;
	struct VSI_context *VSIbot;
#endif
#if defined(USE_H_BRIDGE_CC)
	struct HB_context *HBtop[3];
	struct HB_context *HBbot[3];
	double Itop_uvw_sus[3];	//reference suspension uvw currents
	double Ibot_uvw_sus[3]; //reference suspension uvw currents
#endif
	u32 InputAtTickCount;		//the tick count of the last time the inputs were updated (to avoid unnecessary dq transformations)
	double x_thx[2];
	double y_thy[2];
	double xs[2];
	double ys[2];
	double Itop_xy[2];
	double Ibot_xy[2];
	double MaxPhaseCurrent;
	double TorqueVSI_topIxy[2];
	double TorqueVSI_botIxy[2];
} PosCnt_context;

extern PosCnt_context PosControllers;

extern int Init_PosContTry1(u8 StartAllControllers);
extern int Stop_PosContTry1_Controller(u32 poscnt);
extern int PosContTry1_StopCommand(const char *szCmd, char *szResponse, void *CommDevice);
extern int PosContTry1_StartCommand(const char *szCmd, char *szResponse, void *CommDevice);
extern int PosContTry1_Command(const char * szCmd, char *szResponse, void *CommDevice);
#endif //__POS_CONT_TRY1_H

