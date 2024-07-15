/*
 * PosContTry1.c
 *
 *  Created on: Jul 10, 2014
 *      Author: sever212
 */
#include "../src/project_include.h"

#if defined (USE_VSI_CC)
	//(these are all scaled so that we can take sensor inputs in units of mm)
	#define KKpf_xo_GAIN 47.905474024799268 //38.9693  //Kxo_forcenorm*Kpf_xo
	#define KKiTs_xo_GAIN 0.011041196186847 //0.0075708324	//Kxo_forcenorm*Kif_xo*Ts
	//derivative gain for current error: derivative difference EQ: D = kdn*E[n] + kdn1*E[n-1] + kdm1*D[n-1]
	#define KKdn_xo_GAIN 398.2600565348860 //349.43217 //Kxo_forcenorm*er_coefs_xo(1)
	#define KKdn1_xo_GAIN -398.2600565348860 //-349.43217 //Kxo_forcenorm*er_coefs_xo(2)
	#define KKdm1_xo_GAIN 0.909993668824279 //0.905664 //Dcoefs_xo(2)

	#define KKpf_th_GAIN 4.70 //3.45  //Kth_forcenorm*Kpf_th
	#define KKiTs_th_GAIN 0.001060151124033 //.000517474016	//Kth_forcenorm*Kif_th*Ts
	//derivative gain for current error: derivative difference EQ: D = kdn*E[n] + kdn1*E[n-1] + kdm1*D[n-1]
	#define KKdn_th_GAIN 76.670981745634862 //76.2908 //Kth_forcenorm*er_coefs_th(1)
	#define KKdn1_th_GAIN -76.670981745634862 //-76.2908 //Kth_forcenorm*er_coefs_th(2)
	#define KKdm1_th_GAIN 0.909993668824279 //0.857308 //Dcoefs_th(2)


	#define CUR_KP_GAIN  0.441786 //Old val (works): 0.9990 //WORKS: 0.499513231920777 // 0.16650441064	//Kpv
	#define CUR_KiTs_GAIN 0.014726 //Old val (works): 0.0251 //WORKS: 0.012566370614359 // 0.006283185307180	//Kiv*Ts


	#define BOT_US_ADC 1
	#define BOT_VS_ADC 2
	#define BOT_WS_ADC 3
	#define TOP_US_ADC 4
	#define TOP_VS_ADC 5
	#define TOP_WS_ADC 6

	#define BOT_US_LEG 1
	#define BOT_VS_LEG 2
	#define BOT_WS_LEG 3
	#define TOP_US_LEG 4
	#define TOP_VS_LEG 5
	#define TOP_WS_LEG 6


	#define BOTX_ADC 7
	#define BOTY_ADC 8
	#define TOPX_ADC 9
	#define TOPY_ADC 10

	#define BOT_ADC_MM_V 0.201005		//mm/V (1/4.975) for SN1309172-01-01
	#define TOP_ADC_MM_V 0.201491		//mm/V (1/4.963) for SN1207054-01-01

	#define INV_2RX 2.617801047120	//1/(2*rx)  //rx should be in units of m (even though sensors are in units of mm -- because gains were divided by 1000)

	#define MAX_PHASE_CURRENT 3.6 //max that the sensor can read (8 turns --> 0.375 A/V, so max it can read is 3.75A)

	#define CS_ADC_AMP_PER_VOLT 0.375

	static u8 POS_VSI_Input(void *arg, void *VSIContext);
	static void POS_VSI_Output(void *arg, void *VSIContext);
#endif

#if defined (USE_H_BRIDGE_CC) || defined(USE_H_BRIDGE_DQ_CC)
	//(these are all scaled so that we can take sensor inputs in units of mm)
	#define KKpf_xo_GAIN 61.669050462043472 //WORKS: 77.938586884587338  //Kxo_forcenorm*Kpf_xo
	#define KKiTs_xo_GAIN 0.010458260654729 //0.015141664815228	//Kxo_forcenorm*Kif_xo*Ts
	//derivative gain for current error: derivative difference EQ: D = kdn*E[n] + kdn1*E[n-1] + kdm1*D[n-1]
	#define KKdn_xo_GAIN 546.4352960041329 //698.86434078013 //Kxo_forcenorm*er_coefs_xo(1)
	#define KKdn1_xo_GAIN -546.4352960041329 //-698.8643407801309 //Kxo_forcenorm*er_coefs_xo(2)
	#define KKdm1_xo_GAIN  0.917183613124594 //0.905664 //Dcoefs_xo(2)

	#define KKpf_th_GAIN 7.183817503137941 //6.90  //Kth_forcenorm*Kpf_th
	#define KKiTs_th_GAIN 0.001101464122195 //0.001034948	//Kth_forcenorm*Kif_th*Ts
	//derivative gain for current error: derivative difference EQ: D = kdn*E[n] + kdn1*E[n-1] + kdm1*D[n-1]
	#define KKdn_th_GAIN 159.0181780942346 //152.581543975 //Kth_forcenorm*er_coefs_th(1)
	#define KKdn1_th_GAIN -159.0181780942346 //-152.581543975 //Kth_forcenorm*er_coefs_th(2)
	#define KKdm1_th_GAIN 0.854358985825323 //0.857308 //Dcoefs_th(2)


	#define CUR_KP_GAIN  0.161 //Works (Lext): 0.3338 //Works (IAS): 0.1047 //0.1665 //Kpv
	#define CUR_KiTs_GAIN 0.0210 //Works (Lext): 0.0105 //Works (IAS): 0.02094 //0.006283 //Kiv*Ts


	#define BOT_US_ADC 1
	#define BOT_VS_ADC 2
	#define BOT_WS_ADC 3
	#define TOP_US_ADC 4
	#define TOP_VS_ADC 5
	#define TOP_WS_ADC 6

	#define BOT_US_L1 11
	#define BOT_US_L2 12
	#define BOT_VS_L1 15
	#define BOT_VS_L2 16
	#define BOT_WS_L1 19
	#define BOT_WS_L2 20
	#define TOP_US_L1 5
	#define TOP_US_L2 6
	#define TOP_VS_L1 13
	#define TOP_VS_L2 14
	#define TOP_WS_L1 17
	#define TOP_WS_L2 18

	#define BOTX_ADC 7
	#define BOTY_ADC 8
	#define TOPX_ADC 9
	#define TOPY_ADC 10

	#define BOT_ADC_MM_V 0.201005		//mm/V (1/4.975) for SN1309172-01-01
	#define TOP_ADC_MM_V 0.201491		//mm/V (1/4.963) for SN1207054-01-01

	#define INV_2RX 2.617801047120	//1/(2*rx)  //rx should be in units of m (even though sensors are in units of mm -- because gains were divided by 1000)

	#define MAX_PHASE_CURRENT 5.8 //max that the sensor can read (5 turns --> 0.6 A/V, so max it can read is 6A)

	#define CS_ADC_AMP_PER_VOLT 0.6

#if defined(USE_H_BRIDGE_CC)
	static u8 POS_HB_Input(void *arg, void *HBContext);
	static void POS_HB_Output(void *arg, void *HBContext);
#endif
#if defined(USE_H_BRIDGE_DQ_CC)
	static u8 POS_VSI_Input(void *arg, void *VSIContext);
	static void POS_VSI_Output(void *arg, void *VSIContext);
#endif
#endif



static void POS_Input(void *arg, struct controller_context *cnt);
static void POS_Output(void *arg, struct controller_context *cnt);



//Logging:
s32 x0_meas, y0_meas, x0Error, y0Error, thx, thy, thxError, thyError;
s32 Ixt_meas, Iyt_meas, Ixb_meas, Iyb_meas, Ixt_ref, Iyt_ref, Ixb_ref, Iyb_ref;
s32 Ixb_ref_usat, Iyb_ref_usat, Ixt_ref_usat, Iyt_ref_usat;

#define DEFAULT_X_TOP_REF 0.251
#define DEFAULT_Y_TOP_REF -0.053
#define DEFAULT_X_BOT_REF 0.089
#define DEFAULT_Y_BOT_REF -0.140

//double default_ref_pos[4] = {DEFAULT_X_TOP_REF, DEFAULT_Y_TOP_REF, DEFAULT_X_BOT_REF, DEFAULT_Y_BOT_REF};

//non-control -- allows x and y, top and bottom currents to be forced to a certain value
double force_ixy_top[2] = {0.0, 0.0};
double force_ixy_bot[2] = {0.0, 0.0};
u8 bForceXY_Currents = 0;

int SaturateDQCurrents(double MaxPhaseCurrent, double *inputDQ, double *outputDQ)
{
	double mc = sqrt(0.6667*(inputDQ[0]*inputDQ[0] + inputDQ[1]*inputDQ[1]));
	if (mc > MaxPhaseCurrent)
	{
		outputDQ[0] = inputDQ[0]/mc*MaxPhaseCurrent;
		outputDQ[1] = inputDQ[1]/mc*MaxPhaseCurrent;
	}
	else
	{
		outputDQ[0] = inputDQ[0];
		outputDQ[1] = inputDQ[1];
	}
	return SUCCESS;
}

int SaturatePhaseCurrents(double MaxPhaseCurrent, double *inputUVW, double *outputUVW)
{
	int i = 0;
	double max_c = inputUVW[0];
	double scale = 1.0;
	for (i = 1; i < 3; i++)
	{
		if (fabs(inputUVW[i]) > fabs(max_c))
			max_c = inputUVW[i];
	}
	if (fabs(max_c) > MaxPhaseCurrent)
	{
		scale = fabs(MaxPhaseCurrent/max_c);
		for (i = 0; i < 3; i++)
			outputUVW[i] = scale*inputUVW[i];
	}
	else
	{
		for (i = 0; i < 3; i++)
			outputUVW[i] = inputUVW[i];
	}

	return SUCCESS;
}

PosCnt_context PosControllers;
static controller_linked_list_item pos_cont_items [4];

int ConvertFromX1X2(double *x1x2, double *x0th)
{
	x0th[0] = 0.5*(x1x2[0] + x1x2[1]);
	x0th[1] = INV_2RX*(x1x2[0] - x1x2[1]);
	return SUCCESS;
}

int ConvertFromSumDiff(double *SumDiff, double *OneTwo)
{
	OneTwo[0] = 0.5*(SumDiff[0] + SumDiff[1]);
	OneTwo[1] = 0.5*(SumDiff[0] - SumDiff[1]);
	return SUCCESS;
}

/*****************************
 * Init_PosContTry1()
 * Sets up the controllers memory to default values
 * and optionally starts the controllers running.
 * The default settings can be overridden by manually
 * editing the VSI_DQ_Cur_Controllers structure.
 *
 *StartAllControllers: this is ignored
 *returns a success code
 */
int Init_PosContTry1(u8 StartAllControllers)
{
	struct pid_house *pid;
	int i;

	PosControllers.MaxPhaseCurrent = MAX_PHASE_CURRENT;
#if defined(USE_VSI_CC) || defined(USE_H_BRIDGE_DQ_CC)
	PosControllers.VSItop = &VSI_DQ_Cur_Controllers[VSI_TOP_INDEX];
	PosControllers.VSIbot = &VSI_DQ_Cur_Controllers[VSI_BOT_INDEX];
#endif


	//setup and register PID controllers
	for (i = 0; i < 4; i++)
	{
		if (i < 2)
			pid = &PosControllers.x[i];
		else
			pid = &PosControllers.y[i-2];

		//position controller
		if ((i == 0) || (i == 2))
		{
			pid->pid.kp = KKpf_xo_GAIN;
			pid->pid.kits = KKiTs_xo_GAIN;
			pid->pid.kdn = KKdn_xo_GAIN;
			pid->pid.kdn1 = KKdn1_xo_GAIN;
			pid->pid.kdm1 = KKdm1_xo_GAIN;
		}
		else
		{
			pid->pid.kp = KKpf_th_GAIN;
			pid->pid.kits = KKiTs_th_GAIN;
			pid->pid.kdn = KKdn_th_GAIN;
			pid->pid.kdn1 = KKdn1_th_GAIN;
			pid->pid.kdm1 = KKdm1_th_GAIN;
		}
		pid->pid.lastInt = 0;
		pid->pid.lastErr = 0;
		pid->pid.lastD = 0;
		pid->pid.plusSat = 2.4495*MAX_PHASE_CURRENT;  //2*sqrt(3/2)*Kf*Iph_max
		pid->pid.negSat = -2.4495*MAX_PHASE_CURRENT;  //-2*sqrt(3/2)*Kf*Iph_max

		//house
		pid->reference = 0;
		pid->updated = FALSE;
		pid->input = 0;
		pid->output = 0;

		//IO (controller_context)
		pid->io.inputs = &pid->input;
		pid->io.outputs = &pid->output;
		pid->io.NumInputs = 1;
		pid->io.NumOutputs = 1;
		pid->io.ClockTime = 5;	//uS
		pid->io.clockCount = 0;
		pid->io.NumClocks = 1;
		pid->io.Type = C_PID;
		pid->io.Status = C_RUNNING;

		//CTXT (controller_holder)
		pid->ctxt.arg_in = (void *)i;
		pid->ctxt.arg_out = (void *)i;
		pid->ctxt.control_io = &pid->io;
		pid->ctxt.controller_info = &pid->pid;
		pid->ctxt.input_callback = POS_Input;
		pid->ctxt.output_callback = POS_Output;

		//register the controller
		pos_cont_items[i].context = &pid->ctxt;
		RegisterController(0, &pos_cont_items[i]);
	}

	//setup VSI's:
#if defined(USE_VSI_CC) || defined(USE_H_BRIDGE_DQ_CC)
	Init_VSI_DQ_VSPWM(FALSE);		//set to defaults (but don't register controllers)
	for (i = 0; i < 2; i++)
	{
		u32 vsi_index;
		struct VSI_context *VSI;
		if (i == 0)
		{
			VSI = PosControllers.VSIbot;
			vsi_index = VSI_BOT_INDEX;
		}
		else
		{
			VSI = PosControllers.VSItop;
			vsi_index = VSI_TOP_INDEX;
#if defined(VSI_ISOLATED)
			VSI->bUseZeroSequence = 0;
#else
			VSI->bUseZeroSequence = 1;
			VSI->zero.pi.kits = CUR_KiTs_GAIN/2.0;
			VSI->zero.pi.kp = CUR_KP_GAIN/2.0;
#endif
		}

		VSI->input_arg = (void*) vsi_index;
		VSI->output_arg = (void*) vsi_index;

		VSI->d.pi.kits = CUR_KiTs_GAIN;
		VSI->d.pi.kp = CUR_KP_GAIN;
		VSI->q.pi.kits = CUR_KiTs_GAIN;
		VSI->q.pi.kp = CUR_KP_GAIN;

		//callbacks
		VSI->get_input = POS_VSI_Input;
		VSI->write_output = POS_VSI_Output;

		//start it up
		Start_VSI_DQ_SVPWM_Controller(vsi_index+1);
	}
#endif
#if defined(USE_H_BRIDGE_CC)

	Init_HBridge_CC(FALSE);		//set to defaults (but don't register controllers)
	for (i = 0; i < 6; i++)
	{
		u32 hb_index;
		struct HB_context *HB;

		if (i < 3)
		{
			PosControllers.HBtop[i] = &HB_Controllers[i];
			HB = PosControllers.HBtop[i]; //i = 0: u, i = 1: v, i = 2: w
			hb_index = i;
		}
		else
		{
			PosControllers.HBbot[i-3] = &HB_Controllers[i];
			HB = PosControllers.HBbot[i-3]; //i = 3: u, i = 4: v, i = 5: w
			hb_index = i;
		}

		HB->input_arg = (void*) hb_index;
		HB->output_arg = (void*) hb_index;

		HB->c.pi.kits = CUR_KiTs_GAIN;
		HB->c.pi.kp = CUR_KP_GAIN;

		//callbacks
		HB->get_input = POS_HB_Input;
		HB->write_output = POS_HB_Output;

		//start it up
		Start_HBridge_CC_Controller(hb_index+1);
	}
#endif

	//adjust references of position controllers
	//x1 is top, x2 is bot
	double xs[2] = {DEFAULT_X_TOP_REF, DEFAULT_X_BOT_REF};
	double ys[2] = {DEFAULT_Y_TOP_REF, DEFAULT_Y_BOT_REF};
	double xthx[2];
	double ythy[2];
	ConvertFromX1X2(xs, xthx);
	ConvertFromX1X2(ys, ythy);

	PosControllers.x[0].reference = xthx[0];
	PosControllers.x[1].reference = xthx[1];

	PosControllers.y[0].reference = ythy[0];
	PosControllers.y[1].reference = ythy[1];
	return SUCCESS;
}


/*****************************
 * Stop_PosContTry1_Controller()
 * Unregisters the position controller. Note that this does
 * not stop any corresponding current controller
 *
 *vsi: the VSI number to unregister (1 - VSI_DQ_SVPWM_NUM_VSI)
 *returns a success code
 */
int Stop_PosContTry1_Controller(u32 poscnt)
{
	u32 i = poscnt-1;
	if (i >= 4)
		return INVALID_ARGUMENT;
	if (pos_cont_items[2*i].bRegistered == TRUE)
		UnregisterController(0, &pos_cont_items[2*i]);
	if (pos_cont_items[2*i+1].bRegistered == TRUE)
		UnregisterController(0, &pos_cont_items[2*i+1]);
	return SUCCESS;
}

/*****************************
 * PosContTry1_StopCommand()
 * Process an ascii command for stopping the position controllers
 * and any associated current controllers
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int PosContTry1_StopCommand(const char *szCmd, char *szResponse, void *CommDevice)
{
	int i;
	for (i=0; i< 4; i++)
	{
		Stop_PosContTry1_Controller(i+1);
	}
#if defined(USE_VSI_CC) || defined(USE_H_BRIDGE_DQ_CC)
	Stop_VSI_DQ_SVPWM_Controller(VSI_BOT_INDEX);
	Stop_VSI_DQ_SVPWM_Controller(VSI_TOP_INDEX);
#endif
#if defined(USE_H_BRIDGE_CC)
	for (i=0; i < 6; i++)
		Stop_HBridge_CC_Controller(i);
#endif

	strcpy(szResponse, "OK");
	return strlen(szResponse);
}


/*****************************
 * PosContTry1_StartCommand()
 * Process an ascii command for starting the position controllers
 * Init_PosContTry1 is called
 *
 * Example: CST=1,FORCEI=1
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int PosContTry1_StartCommand(const char *szCmd, char *szResponse, void *CommDevice)
{

	if (strstr(szCmd, "FORCEI"))
	{
		strcpy(szResponse, "FORCEI=1");
		bForceXY_Currents = TRUE;
	}
	else
	{
		bForceXY_Currents = FALSE;
		strcpy(szResponse, "OK");
	}


	Init_PosContTry1(TRUE);

	return strlen(szResponse);
}

/*****************************
 * PosContTry1_Command()
 * Process an ascii command for setting a reference position (or current)
 * Syntax: n,tb,val
 *  n is the controller axis (x for x-axis, y for y-axis),
 *  tb is t for top, b for bottom,
 *	val is the mm reference (*128)
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int PosContTry1_Command(const char * szCmd, char *szResponse, void *CommDevice)
{
	u8 tb = szCmd[0];
	u8 bError = FALSE;
	double ref;
	if ((tb != 'b')  && (tb != 't') )
		bError = TRUE;
	else if (szCmd[1] != ',')
		bError = TRUE;
	else
	{
		u8 xy = szCmd[2];
		if ((xy != 'x')  && (xy != 'y') )
			bError = TRUE;
		else if (szCmd[3] != ',')
			bError = TRUE;
		else if (szCmd[4] == 0)
			bError = TRUE;
		else
		{
			ref = ((double) atoi(&szCmd[4]))/128;
			if (bForceXY_Currents)
			{
				if (tb == 't')
				{
					if (xy == 'x')
						force_ixy_top[0] = ref;
					else
						force_ixy_top[1] = ref;
				}
				else
				{
					if (xy == 'x')
						force_ixy_bot[0] = ref;
					else
						force_ixy_bot[1] = ref;
				}
				strcpy(szResponse, "OK");
			}
			else //not supported yet
				bError = TRUE;
		}

	}
	if (bError)
		strcpy(szResponse, "ERROR");

	return strlen(szResponse);
}




//Internal function called by control.c before executing a controller
static void POS_Input(void *arg, struct controller_context *cnt)
{
//	u32 i= (u32) arg;

	//calculate the error and store it
	if (TimeElapsed10us(PosControllers.InputAtTickCount) > 5)
	{

		PosControllers.InputAtTickCount = MainTimerTimerTick;
		PosControllers.xs[POS_BOT_INDEX] = BOT_ADC_MM_V*ReadADC(BOTX_ADC);
		PosControllers.xs[POS_TOP_INDEX] = TOP_ADC_MM_V*ReadADC(TOPX_ADC);
		PosControllers.ys[POS_BOT_INDEX] = BOT_ADC_MM_V*ReadADC(BOTY_ADC);
		PosControllers.ys[POS_TOP_INDEX] = TOP_ADC_MM_V*ReadADC(TOPY_ADC);
		double x1x2[2] = {PosControllers.xs[POS_TOP_INDEX], PosControllers.xs[POS_BOT_INDEX]};
		double y1y2[2] = {PosControllers.ys[POS_TOP_INDEX], PosControllers.ys[POS_BOT_INDEX]};
		ConvertFromX1X2(x1x2, PosControllers.x_thx);
		ConvertFromX1X2(y1y2, PosControllers.y_thy);

		PosControllers.x[0].input = PosControllers.x[0].reference - PosControllers.x_thx[0];
		PosControllers.x[1].input = PosControllers.x[1].reference - PosControllers.x_thx[1];
		PosControllers.y[0].input = PosControllers.y[0].reference - PosControllers.y_thy[0];
		PosControllers.y[1].input = PosControllers.y[1].reference - PosControllers.y_thy[1];

		//logging
		x0_meas = (s32) 128.0*PosControllers.x_thx[0];
		y0_meas = (s32) 128.0*PosControllers.y_thy[0];
		thx = (s32) 128.0*PosControllers.x_thx[1];
		thy = (s32) 128.0*PosControllers.y_thy[1];
		x0Error = (s32) 128.0*PosControllers.x[0].input;
		y0Error = (s32) 128.0*PosControllers.y[0].input;
		thxError = (s32) 128.0*PosControllers.x[1].input;
		thyError = (s32) 128.0*PosControllers.y[1].input;

	}
}

//Internal function called by control.c when an output is ready
static void POS_Output(void *arg, struct controller_context *cnt)
{
	u32 i= (u32) arg;

	switch (i)
	{
		case 0:
			PosControllers.x[0].updated = TRUE;
			break;

		case 1:
			PosControllers.x[1].updated = TRUE;
			break;

		case 2:
			PosControllers.y[0].updated = TRUE;
			break;

		case 3:
			PosControllers.y[1].updated = TRUE;
			break;

		default:
		break;
	}


	if ( (PosControllers.x[0].updated == TRUE) && (PosControllers.x[1].updated==TRUE)
			&& (PosControllers.y[0].updated == TRUE) && (PosControllers.y[1].updated==TRUE))
	{
		double current_xy_top[2], current_xy_bot[2], current_12x[2], current_12y[2], current_sumdiff[2];
		//all controller output current references have been updated,
		//update the xy current holder taking into account maximum current values
		PosControllers.x[0].updated = FALSE;
		PosControllers.x[1].updated = FALSE;
		PosControllers.y[0].updated = FALSE;
		PosControllers.y[1].updated = FALSE;

		//get top bot x currents
		current_sumdiff[0] = PosControllers.x[POS_XY0_INDEX].output;
		current_sumdiff[1] = PosControllers.x[POS_THXY_INDEX].output;
		ConvertFromSumDiff(current_sumdiff, current_12x);
		current_xy_top[0] = current_12x[0];
		current_xy_bot[0] = current_12x[1];	//NOT negative because the bottom force is NOT created in the opposite direction

		//get top bot y currents
		current_sumdiff[0] = PosControllers.y[POS_XY0_INDEX].output;
		current_sumdiff[1] = PosControllers.y[POS_THXY_INDEX].output;
		ConvertFromSumDiff(current_sumdiff, current_12y);
		current_xy_top[1] = current_12y[0];
		current_xy_bot[1] = current_12y[1]; //NOT negative because the bottom force is NOT created in the opposite direction

		//if we are forcing the currents, override the above calculations
		if (bForceXY_Currents)
		{
			current_xy_top[0] = force_ixy_top[0];
			current_xy_top[1] = force_ixy_top[1];
			current_xy_bot[0] = force_ixy_bot[0];
			current_xy_bot[1] = force_ixy_bot[1];
		}

		//saturate the top bot xy currents
#if defined(USE_VSI_CC) || defined(USE_H_BRIDGE_DQ_CC)
		SaturateDQCurrents(PosControllers.MaxPhaseCurrent, current_xy_top, PosControllers.Itop_xy);
		SaturateDQCurrents(PosControllers.MaxPhaseCurrent, current_xy_bot, PosControllers.Ibot_xy);
#else
		PosControllers.Itop_xy[0] = current_xy_top[0]; PosControllers.Itop_xy[1] = current_xy_top[1];
		PosControllers.Ibot_xy[0] = current_xy_bot[0]; PosControllers.Ibot_xy[1] = current_xy_bot[1];
#endif

#if defined(USE_H_BRIDGE_CC)
		{
			double i_xy0[3], i_uvw[3];
			i_xy0[0] = PosControllers.Itop_xy[0];
			i_xy0[1] = PosControllers.Itop_xy[1];
			i_xy0[2] = 0;
			InverseDQ0_Transform(0, i_uvw, i_xy0);
			SaturatePhaseCurrents(PosControllers.MaxPhaseCurrent, i_uvw, PosControllers.Itop_uvw_sus);
			i_xy0[0] = PosControllers.Ibot_xy[0]; i_xy0[1] = PosControllers.Ibot_xy[1]; i_xy0[2] = 0;
			InverseDQ0_Transform(0, i_uvw, i_xy0);
			SaturatePhaseCurrents(PosControllers.MaxPhaseCurrent, i_uvw, PosControllers.Ibot_uvw_sus);
		}
#endif

		//logging
		Ixb_ref_usat = (s32) 128.0*current_xy_bot[0];
		Iyb_ref_usat = (s32) 128.0*current_xy_bot[1];
		Ixt_ref_usat = (s32) 128.0*current_xy_top[0];
		Iyt_ref_usat = (s32) 128.0*current_xy_top[1];
	}
}


#if defined(USE_VSI_CC) || defined(USE_H_BRIDGE_DQ_CC)
//update reference and measured currents
//return false if ABC values set, true if DQ values set
static u8 POS_VSI_Input(void *arg, void *VSIContext)
{
	u32 vsi_index = (u32) arg;
	u32 adc_num[3];
	struct VSI_context *cnt = VSIContext;
	//double meas_abc[3], meas_dq0[3];
	double adc_gain = CS_ADC_AMP_PER_VOLT;

	//MEASURED CURRENTS
	if (vsi_index == VSI_BOT_INDEX)
	{
		adc_num[0] = BOT_US_ADC; adc_num[1] = BOT_VS_ADC; adc_num[2] = BOT_WS_ADC;
	}
	else
	{
		adc_num[0] = TOP_US_ADC; adc_num[1] = TOP_VS_ADC; adc_num[2] = TOP_WS_ADC;
	}

	cnt->Iabc[0] = adc_gain*ReadADC(adc_num[0]);
	cnt->Iabc[1] = adc_gain*ReadADC(adc_num[1]);
	cnt->Iabc[2] = adc_gain*ReadADC(adc_num[2]);
	cnt->theta_da = 0;

	//REFERENCE DQ CURRENTS
	if (vsi_index == VSI_BOT_INDEX)
	{
#if defined(USE_H_BRIDGE_DQ_CC)
		cnt->d.reference = PosControllers.Ibot_xy[0];
		cnt->q.reference = PosControllers.Ibot_xy[1];
#endif

#if defined(USE_VSI_CC)
		cnt->d.reference = PosControllers.Ibot_xy[0] - 0.5*PosControllers.TorqueVSI_botIxy[0];
		cnt->q.reference = PosControllers.Ibot_xy[1] - 0.5*PosControllers.TorqueVSI_botIxy[1];
#endif
		//logging
		Ixb_ref = (s32) 128.0*PosControllers.Ibot_xy[0]; //PosControllers.Ibot_xy[0] - 0.5*PosControllers.TorqueVSI_topIxy[0];
		Iyb_ref = (s32) 128.0*PosControllers.Ibot_xy[1]; //PosControllers.Ibot_xy[1] - 0.5*PosControllers.TorqueVSI_topIxy[1];
	}
	else
	{
#if defined(USE_H_BRIDGE_DQ_CC)
		cnt->d.reference = PosControllers.Itop_xy[0];
		cnt->q.reference = PosControllers.Itop_xy[1];
#endif
#if defined(USE_VSI_CC)
		cnt->d.reference = PosControllers.Itop_xy[0] - 0.5*PosControllers.TorqueVSI_topIxy[0];
		cnt->q.reference = PosControllers.Itop_xy[1] - 0.5*PosControllers.TorqueVSI_topIxy[1];
#endif
		//logging
		Ixt_ref = (s32) 128.0*PosControllers.Itop_xy[0];
		Iyt_ref = (s32) 128.0*PosControllers.Itop_xy[1];
	}
	return FALSE;
}

//write the duty ratio out to the legs
static void POS_VSI_Output(void *arg, void *VSIContext)
{
	u32 vsi_index = (u32) arg;
	u32 legs[6];
	u8 duty1, duty2, duty3;
	struct VSI_context *cnt = VSIContext;
	legs[0] = 0; legs[1] = 0; legs[2] = 0;
	int i;

#if defined(USE_H_BRIDGE_DQ_CC)
	if (vsi_index == VSI_BOT_INDEX)
	{
		legs[0] = BOT_US_L1; legs[1] = BOT_US_L2;
		legs[2] = BOT_VS_L1; legs[3] = BOT_VS_L2;
		legs[4] = BOT_WS_L1; legs[5] = BOT_WS_L2;
	}
	else
	{
		legs[0] = TOP_US_L1; legs[1] = TOP_US_L2;
		legs[2] = TOP_VS_L1; legs[3] = TOP_VS_L2;
		legs[4] = TOP_WS_L1; legs[5] = TOP_WS_L2;
	}

	//convert the modulation indices to duty ratios
	for (i = 0; i < 3; i++)
	{
		double m;
		u8 d1, d2;
		//safety to avoid roll-over:
		if (cnt->mabc[i] > 1.0)
			m = 1.0;
		else if (cnt->mabc[i] < -1.0)
			m = -1.0;
		else
			m = cnt->mabc[i];


		//calculate duty ratios
		d1 = (unsigned char) 127*(1 + m);
		d2 = (unsigned char) 127*(1 - m);

		//write it out
		WriteDutyRatio(legs[i*2], d1);
		WriteDutyRatio(legs[i*2+1], d2);

	}
#endif

#if defined(USE_VSI_CC)
	if (vsi_index == VSI_BOT_INDEX)
	{
		legs[0] = BOT_US_LEG; legs[1] = BOT_VS_LEG; legs[2] = BOT_WS_LEG;
	}
	else
	{
		legs[0] = TOP_US_LEG; legs[1] = TOP_VS_LEG; legs[2] = TOP_WS_LEG;
	}
	//convert the duty ratios
	//dtest = 256*(d_com + m_abc[0]);
	duty1 = (unsigned char) 256.0*(cnt->dabc[0]);
	duty2 = (unsigned char) 256.0*(cnt->dabc[1]);
	duty3 = (unsigned char) 256.0*(cnt->dabc[2]);

	//write them out:
	WriteDutyRatio(legs[0], duty1);
	WriteDutyRatio(legs[1], duty2);
	WriteDutyRatio(legs[2], duty3);
#endif
}
#endif

#if defined(USE_H_BRIDGE_CC)
//update reference and measured currents
//return false if ABC values set, true if DQ values set
static u8 POS_HB_Input(void *arg, void *HBContext)
{
	u32 hb_index = (u32) arg;
	u32 adc_num = 0;
	struct HB_context *cnt = HBContext;
	double adc_gain = CS_ADC_AMP_PER_VOLT;

	//MEASURED CURRENTS
	switch(hb_index)
	{
	case 0:
		adc_num = TOP_US_ADC;
		break;
	case 1:
		adc_num = TOP_VS_ADC;
		break;
	case 2:
		adc_num = TOP_WS_ADC;
		break;
	case 3:
		adc_num = BOT_US_ADC;
		break;
	case 4:
		adc_num = BOT_VS_ADC;
		break;
	case 5:
		adc_num = BOT_WS_ADC;
		break;
	default:
		//should not get here!
		break;
	}

	//update the measured current
	cnt->I = adc_gain*ReadADC(adc_num);

	//update reference currents
	if (hb_index < 3)
		HBridge_CC_SetCurrentRef(hb_index+1, PosControllers.Itop_uvw_sus[hb_index]);
	else
		HBridge_CC_SetCurrentRef(hb_index+1, PosControllers.Ibot_uvw_sus[hb_index-3]);

	//logging
		Ixt_ref = (s32) 128.0*PosControllers.Itop_xy[0];
		Iyt_ref = (s32) 128.0*PosControllers.Itop_xy[1];
		Ixb_ref = (s32) 128.0*PosControllers.Ibot_xy[0];
		Iyb_ref = (s32) 128.0*PosControllers.Ibot_xy[1];

	return FALSE;
}

//write the duty ratio out to the legs
static void POS_HB_Output(void *arg, void *HBContext)
{
	u32 hb_index = (u32) arg;
	u32 legs[2];

	struct HB_context *cnt = HBContext;
	legs[0] = 0; legs[1] = 0;

	switch(hb_index)
	{
	case 0:
		legs[0] = TOP_US_L1;
		legs[1] = TOP_US_L2;
		break;
	case 1:
		legs[0] = TOP_VS_L1;
		legs[1] = TOP_VS_L2;
		break;
	case 2:
		legs[0] = TOP_WS_L1;
		legs[1] = TOP_WS_L2;
		break;
	case 3:
		legs[0] = BOT_US_L1;
		legs[1] = BOT_US_L2;
		break;
	case 4:
		legs[0] = BOT_VS_L1;
		legs[1] = BOT_VS_L2;
		break;
	case 5:
		legs[0] = BOT_WS_L1;
		legs[1] = BOT_WS_L2;
		break;
	default:
		//should not get here!
		break;
	}

	//write them out:
	WriteDutyRatio(legs[0], cnt->d[0]);
	WriteDutyRatio(legs[1], cnt->d[1]);
}
#endif
