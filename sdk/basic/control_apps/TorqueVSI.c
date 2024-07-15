/*
 * TorqueVSI.c
 *
 *  Created on: Jul 17, 2014
 *      Author: sever212
 */

#include "../src/project_include.h"


#define OMEGA_DA_RAMP_RATE 0.001

#define DEFAULT_T_SINE_TRIANGLE 1
#define MAX_PHASE_CURRENT 1
const u8 TVSI_INDEX[NUM_TVSI] = {2};



TVSI_context TVSIs[NUM_TVSI];

#define IField 1

#if defined (USE_VSI_CC)
	#define TVSI_KiTs_Q_GAIN 0.001675516 //0.0010053 //0.001675 //0.0032 //0.0063 //Old val (works): 0.0151 //0.0008 //0.005 //Ki_t*Ts/Vbus
	#define TVSI_KiTs_D_GAIN 0.001675516 //0.0010053 //0.001675 //0.0032 //0.0063 //0.0151 //0.0008 //0.005 //Ki_t*Ts/Vbus

	#define TVSI_KP_Q_GAIN 0.1590065 //0.09540388 //0.159006 //0.2981 //0.5963 //Old val (works): 1.431 //.076 //0.452 //Kp_t/Vbus
	#define TVSI_KP_D_GAIN 0.1590065 //0.09540388 //0.159006 //0.2981 //0.5963 //1.431 //.076 //0.452 //Kp_t/Vbus


	#define T0_U_ADC 11
	#define T0_V_ADC 12

	#define T0_U_LEG 11
	#define T0_V_LEG 12
	#define T0_W_LEG 13

	#define Lq_div_Vbus 0.00002531 //0.000126533 //Lq/Vbus
	#define Ld_div_Vbus 0.00002531 //0.000126533 //Ld/Vbus
	#define sqrt6_Lfg_div_Vbus 0.00009219 //0.0004609576 //sqrt(6)*Lfg_m/Vbus

#endif

#if defined(TVSI_STATIONARY_FRAME)
	const u8 TVSI_STATIONARY_INDEX[NUM_TVSI] = {3};

	#define TVSI_KiTs_X_GAIN TVSI_KiTs_D_GAIN
	#define TVSI_KiTs_Y_GAIN TVSI_KiTs_D_GAIN

	#define TVSI_KP_X_GAIN 0
	#define TVSI_KP_Y_GAIN 0

	static u8 T_VSI_Input_Stationary(void *arg, void *VSIContext);
	static void T_VSI_Output_Stationary(void *arg, void *VSIContext);

	//logging
	u32 Itx_measured, Ity_measured;
	u32 Itx_ref, Ity_ref;
	u32 x_out_ref, y_out_ref;
	u32 dxy_out_ref, qxy_out_ref;
#endif

#if defined (USE_H_BRIDGE_CC) || defined(USE_H_BRIDGE_DQ_CC)
	//wc = 2*pi*750
	#define TVSI_KiTs_Q_GAIN 0.00837758 //0.0047 //0.0151 //Ki_t*Ts/Vbus
	#define TVSI_KiTs_D_GAIN 0.00837758 //0.0047 //0.0151 //Ki_t*Ts/Vbus

	#define TVSI_KP_Q_GAIN 0.7950 //0.13499 //1.431 //Kp_t/Vbus
	#define TVSI_KP_D_GAIN 0.7950 //0.13499 //1.431 //Kp_t/Vbus

	#define T0_U_ADC 11
	#define T0_V_ADC 12

	#define T0_U_LEG 1
	#define T0_V_LEG 2
	#define T0_W_LEG 3

	#define Lq_div_Vbus 0.000126533 //Lq/Vbus
	#define Ld_div_Vbus 0.000126533 //Ld/Vbus
	#define sqrt6_Lfg_div_Vbus 0.0004609576 //sqrt(6)*Lfg_m/Vbus
#endif

s32 OmegaDA_Log = 0;

static u8 T_VSI_Input(void *arg, void *VSIContext);
static void T_VSI_Output(void *arg, void *VSIContext);

static double GetTheta_DA(double theta_da, double omega_da, u32 LastTickCNT)
{
	double update_da = 0;
	u32 elapsed;
	elapsed = TimeElapsed10us(LastTickCNT);
	update_da = ((double) omega_da*((double)elapsed)/((double)100000.0));
	theta_da += update_da;
	while (theta_da > PI2)
		theta_da = theta_da - PI2;

	return theta_da;
}
u32 t_vsi_theta_da;

/*****************************
 * Init_TorqueVSI()
 * Sets up the controllers memory to default values
 * and optionally starts the controllers running.
 * The default settings can be overridden by manually
 * editing the VSI_DQ_Cur_Controllers structure.
 *
 *StartAllControllers: this is ignored
 *returns a success code
 */
int Init_TorqueVSI(u8 StartAllControllers)
{
	int i;

#if defined(USE_H_BRIDGE_CC)
	Init_VSI_DQ_VSPWM(FALSE);		//We need to init the VSI controllers in the HBridge case because the position controller has not done that.
#endif

	for (i = 0; i < NUM_TVSI; i++)
	{
		TVSIs[i].MaxPhaseCurrent = MAX_PHASE_CURRENT;
		TVSIs[i].InputAtTickCount = MainTimerTimerTick;
		TVSIs[i].OmegaDA_Cmd = 0;
		TVSIs[i].OmegaDA_Current = 0;
		TVSIs[i].VSI = &VSI_DQ_Cur_Controllers[TVSI_INDEX[i]];
		TVSIs[i].VSI->theta_da = 0;
		TVSIs[i].Idq_ref[0] = 0;
		TVSIs[i].Idq_ref[1] = 0;

		//assume init_vsi_dq_VSPWM has already been called
		TVSIs[i].VSI->bSineTriangle = DEFAULT_T_SINE_TRIANGLE;

		//controller gains
		TVSIs[i].VSI->d.pi.kits = TVSI_KiTs_D_GAIN;
		TVSIs[i].VSI->d.pi.kp = TVSI_KP_D_GAIN;
		TVSIs[i].VSI->q.pi.kits = TVSI_KiTs_Q_GAIN;
		TVSIs[i].VSI->q.pi.kp = TVSI_KP_Q_GAIN;

		//callbacks
		TVSIs[i].VSI->input_arg = (void*) i;
		TVSIs[i].VSI->output_arg = (void*) i;
		TVSIs[i].VSI->get_input = T_VSI_Input;
		TVSIs[i].VSI->write_output = T_VSI_Output;

#if defined (TVSI_STATIONARY_FRAME)
		TVSIs[i].VSI_stationary = &VSI_DQ_Cur_Controllers[TVSI_STATIONARY_INDEX[i]];
		TVSIs[i].VSI_stationary->theta_da = 0;

		//assume init_vsi_dq_VSPWM has already been called
		TVSIs[i].VSI_stationary->bSineTriangle = DEFAULT_T_SINE_TRIANGLE;

		//controller gains
		TVSIs[i].VSI_stationary->d.pi.kits = TVSI_KiTs_X_GAIN;
		TVSIs[i].VSI_stationary->d.pi.kp = TVSI_KP_X_GAIN;
		TVSIs[i].VSI_stationary->q.pi.kits = TVSI_KiTs_Y_GAIN;
		TVSIs[i].VSI_stationary->q.pi.kp = TVSI_KP_Y_GAIN;

		//callbacks
		TVSIs[i].VSI_stationary->input_arg = (void*) i;
		TVSIs[i].VSI_stationary->output_arg = (void*) i;
		TVSIs[i].VSI_stationary->get_input = T_VSI_Input_Stationary;
		TVSIs[i].VSI_stationary->write_output = T_VSI_Output_Stationary;

		Start_VSI_DQ_SVPWM_Controller(TVSI_STATIONARY_INDEX[i]+1);
#endif

		//start up the current controller
		Start_VSI_DQ_SVPWM_Controller(TVSI_INDEX[i]+1);
	}
	return SUCCESS;
}


/*****************************
 * Stop_TorqueVSI_Controller()
 * Unregisters the VSI's belonging to the TVSI controller.
 *
 *tvsi: the TVSI number to unregister (1 - NUM_TVSI)
 *returns a success code
 */
int Stop_TorqueVSI_Controller(u32 tvsi)
{
	u32 i = tvsi-1;
	if (i >= NUM_TVSI)
		return INVALID_ARGUMENT;

#if defined(TVSI_STATIONARY_FRAME)
	Stop_VSI_DQ_SVPWM_Controller(TVSI_STATIONARY_INDEX[i]+1);
#endif
	return Stop_VSI_DQ_SVPWM_Controller(TVSI_INDEX[i]+1);
}


/*****************************
 * TorqueVSI_StopCommand()
 * Process an ascii command for stopping the torque VSI current controllers
 * and any associated current controllers
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int TorqueVSI_StopCommand(const char *szCmd, char *szResponse, void *CommDevice)
{
	int i;
	for (i=0; i< 4; i++)
	{
		Stop_TorqueVSI_Controller(i+1);
	}
	strcpy(szResponse, "OK");
	return strlen(szResponse);
}


/*****************************
 * TorqueVSI_StartCommand()
 * Process an ascii command for starting the torque VSI controllers
 * Init_PosContTry1 is called
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int TorqueVSI_StartCommand(const char *szCmd, char *szResponse, void *CommDevice)
{

	/*if (strstr(szCmd, "FORCEI"))
	{
		strcpy(szResponse, "FORCEI=1");
		bForceXY_Currents = TRUE;
	}
	else
	{
		bForceXY_Currents = FALSE;*/
		strcpy(szResponse, "OK");
	//}


	Init_TorqueVSI(TRUE);

	return strlen(szResponse);
}


/*****************************
 * PosContTry1_Command()
 * Process an ascii command for setting a reference position (or current)
 * Syntax: n,v,val
 *  n is the controller index (1 or 2),
 *  v is the variable to set (d for d axis current, q for q axis current, w for reference OmegaDa command)
 *	val is the value (*128)
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int TorqueVSI_Command(const char * szCmd, char *szResponse, void *CommDevice)
{
	u8 num = atoi(szCmd);
	u8 bError = FALSE;
	double ref;
	if ((num != 1) )
		bError = TRUE;
	else if (szCmd[1] != ',')
		bError = TRUE;
	else
	{
		u8 v = szCmd[2];
		if ((v != 'd')  && (v != 'q') && (v != 'w'))
			bError = TRUE;
		else if (szCmd[3] != ',')
			bError = TRUE;
		else if (szCmd[4] == 0)
			bError = TRUE;
		else
		{
			ref = ((double) atoi(&szCmd[4]))/128;
			if (v == 'd')
			{
				TVSIs[num-1].Idq_ref[0] = ref;
				strcpy(szResponse, "OK");
			}
			else if (v == 'q')
			{
				TVSIs[num-1].Idq_ref[1] = ref;
				strcpy(szResponse, "OK");
			}
			else if (v == 'w')
			{
				TVSIs[num-1].OmegaDA_Cmd = ref;
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

//update reference and measured currents
//return false if ABC values set, true if DQ values set
static u8 T_VSI_Input(void *arg, void *VSIContext)
{
	u32 t_index = (u32) arg;
	u32 adc_num[3];
	struct VSI_context *cnt = VSIContext;
	//double meas_abc[3], meas_dq0[3];

	//MEASURED CURRENTS
	if (t_index == 0)
	{
		adc_num[0] = T0_U_ADC; adc_num[1] = T0_V_ADC;
		adc_num[2] = 0; //use a+b+c =0
	}
	else
	{
		; //adc_num[0] = TOP_US_ADC; adc_num[1] = TOP_VS_ADC; adc_num[2] = TOP_WS_ADC; //should not get here!
	}

	cnt->Iabc[0] = 0.6*ReadADC(adc_num[0]);
	cnt->Iabc[1] = 0.6*ReadADC(adc_num[1]);
	cnt->Iabc[2] = -cnt->Iabc[0] -cnt->Iabc[1]; //ReadADC(adc_num[2]);
#if defined (USE_VSI_CC) && !defined(VSI_ISOLATED)
	cnt->Iabc[2] = cnt->Iabc[2] - (PosControllers.VSItop->Iabc[0] + PosControllers.VSItop->Iabc[1] + PosControllers.VSItop->Iabc[2]);
#endif

	cnt->theta_da = GetTheta_DA(cnt->theta_da, TVSIs[t_index].OmegaDA_Current, TVSIs[t_index].InputAtTickCount);
	DQ0_Transform(cnt->theta_da, cnt->Iabc, cnt->Idq0);

	//compensation terms:
	cnt->dqComp[0] = 0; //-TVSIs[t_index].OmegaDA_Current*Lq_div_Vbus*cnt->Idq0[1];
	cnt->dqComp[1] = 0; //TVSIs[t_index].OmegaDA_Current*(Ld_div_Vbus*cnt->Idq0[0] + sqrt6_Lfg_div_Vbus*IField);

#if defined (TVSI_STATIONARY_FRAME)
	//add the stationary frame terms as additional compensation
	{
		//cnt->dqComp[0] += TVSIs[t_index].VSI_stationary->d.output*cos(cnt->theta_da) + TVSIs[t_index].VSI_stationary->q.output*sin(cnt->theta_da);
		//cnt->dqComp[1] += TVSIs[t_index].VSI_stationary->q.output*cos(cnt->theta_da) - TVSIs[t_index].VSI_stationary->d.output*sin(cnt->theta_da);
		double new_dq0[3], old_dq0[3], test_uvw[3], alt_dq0[3];
		old_dq0[0] = TVSIs[t_index].VSI_stationary->d.output;
		old_dq0[1] = TVSIs[t_index].VSI_stationary->q.output;
		old_dq0[2] = 0;
		ChangeDQ_ReferenceFrame(new_dq0, cnt->theta_da, old_dq0, 0.0);
		cnt->dqComp[0] = cnt->dqComp[0] + new_dq0[0];
		cnt->dqComp[1] = cnt->dqComp[1] + new_dq0[1];

		//debugging
		//InverseDQ0_Transform(0.0, test_uvw, old_dq0);
		//DQ0_Transform(cnt->theta_da, test_uvw, alt_dq0);

		//logging
		/*x_out_ref = (s32)(128.0*old_dq0[0]);
		y_out_ref = (s32)(128.0*old_dq0[1]);
		dxy_out_ref = (s32)(128.0*(alt_dq0[0] - new_dq0[0])); //(s32)(128.0*new_dq0[0]);
		qxy_out_ref = (s32)(128.0*(alt_dq0[1] - new_dq0[1])); //(s32)(128.0*new_dq0[1]);*/

	}
#endif

	//acceleration
	if (TVSIs[t_index].OmegaDA_Cmd != TVSIs[t_index].OmegaDA_Current)
	{
		if (TVSIs[t_index].OmegaDA_Cmd > TVSIs[t_index].OmegaDA_Current)
		{
			TVSIs[t_index].OmegaDA_Current += OMEGA_DA_RAMP_RATE*TimeElapsed10us(TVSIs[t_index].InputAtTickCount);
			if (TVSIs[t_index].OmegaDA_Current > TVSIs[t_index].OmegaDA_Cmd)
				TVSIs[t_index].OmegaDA_Current = TVSIs[t_index].OmegaDA_Cmd;
		}
		else if (TVSIs[t_index].OmegaDA_Cmd < TVSIs[t_index].OmegaDA_Current)
		{
			TVSIs[t_index].OmegaDA_Current -= OMEGA_DA_RAMP_RATE*TimeElapsed10us(TVSIs[t_index].InputAtTickCount);
			if (TVSIs[t_index].OmegaDA_Current < TVSIs[t_index].OmegaDA_Cmd)
				TVSIs[t_index].OmegaDA_Current = TVSIs[t_index].OmegaDA_Cmd;
		}
	}

	//should limit the reference currents, but for now don't worry about it:
	cnt->d.reference = TVSIs[t_index].Idq_ref[0];
	cnt->q.reference = TVSIs[t_index].Idq_ref[1];

#if defined(USE_VSI_CC)
	//inform the position controller what we are up to
	{
		double new_dq0[3], old_dq0[3];
		old_dq0[0] = cnt->d.reference;
		old_dq0[1] = cnt->q.reference;
		old_dq0[2] = 0;
	//PosControllers.TorqueVSI_topIxy[0] = cnt->d.reference*cos(cnt->theta_da) - cnt->q.reference*sin(cnt->theta_da);
	//PosControllers.TorqueVSI_topIxy[1] = cnt->q.reference*cos(cnt->theta_da) + cnt->d.reference*sin(cnt->theta_da);
		ChangeDQ_ReferenceFrame(new_dq0, 0.0, old_dq0, cnt->theta_da);
		PosControllers.TorqueVSI_topIxy[0] = new_dq0[0];
		PosControllers.TorqueVSI_topIxy[1] = new_dq0[1];
	}
#endif

	//update the tick count:
	TVSIs[t_index].InputAtTickCount = MainTimerTimerTick;

	//logging
	if (t_index == 0)
		OmegaDA_Log = (s32)(128.0*TVSIs[t_index].OmegaDA_Current);
	return TRUE;
}

//write the duty ratio out to the legs
static void T_VSI_Output(void *arg, void *VSIContext)
{
	u32 tvsi_index = (u32) arg;
	u32 legs[3];
	u8 duty1, duty2, duty3;
	struct VSI_context *cnt = VSIContext;
	legs[0] = 0; legs[1] = 0; legs[2] = 0;

	if (tvsi_index == 0)
	{
		legs[0] = T0_U_LEG; legs[1] = T0_V_LEG; legs[2] = T0_W_LEG;
	}
	else
	{
		; //should not get here legs[0] = TOP_US_LEG; legs[1] = TOP_VS_LEG; legs[2] = TOP_WS_LEG;
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
}

#if defined(TVSI_STATIONARY_FRAME)
//update reference and measured currents
//return false if ABC values set, true if DQ values set
static u8 T_VSI_Input_Stationary(void *arg, void *VSIContext)
{
	u32 t_index = (u32) arg;
	struct VSI_context *cnt = VSIContext;
	double new_dq0[3], old_dq0[3];

	//use the measured VSI currents (transform them into the stationary reference frame)
	ChangeDQ_ReferenceFrame(TVSIs[t_index].VSI_stationary->Idq0, 0.0, TVSIs[t_index].VSI->Idq0, TVSIs[t_index].VSI->theta_da);

	//the reference currents also need to be brought over from the other frame
	old_dq0[0] = TVSIs[t_index].VSI->d.reference;
	old_dq0[1] = TVSIs[t_index].VSI->q.reference;
	old_dq0[2] = 0;
	ChangeDQ_ReferenceFrame(new_dq0, 0.0, old_dq0, TVSIs[t_index].VSI->theta_da);

	cnt->d.reference = new_dq0[0];
	cnt->q.reference = new_dq0[1];

	//logging:
	/*Itx_measured = (s32)(128.0*TVSIs[t_index].VSI_stationary->Idq0[0]);
	Ity_measured = (s32)(128.0*TVSIs[t_index].VSI_stationary->Idq0[1]);
	Itx_ref = (s32)(128.0*TVSIs[t_index].VSI_stationary->d.reference);
	Ity_ref = (s32)(128.0*TVSIs[t_index].VSI_stationary->q.reference);*/

	return TRUE;
}

//normally would write the duty ratio out to the legs, but don't do anything, let the other output function handle all of this.
static void T_VSI_Output_Stationary(void *arg, void *VSIContext)
{

}

#endif

