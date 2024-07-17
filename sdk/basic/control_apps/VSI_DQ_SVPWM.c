/*
 * VSI_DQ_SVPWM.c
 *
 *  Created on: Jun 30, 2014
 *      Author: sever212
 */

#include "../src/project_include.h"
//#include <tgmath.h>


#define KiTs_Q_GAIN 0.005
#define KiTs_D_GAIN 0.005

#define KP_Q_GAIN 0.452
#define KP_D_GAIN 0.452

#define DEFAULT_SINE_TRIANGLE 1		//0 for SVPWM, 1 for SINE TRIANGLE


//assumes the dq voltages are a fraction of the dc bus voltage
int SaturateDQ0Voltages(double *inputDQ, double *outputDQ, u8 bSineTriangle)
{
	double ml, ml_0, a;
	if (bSineTriangle)
	{
		ml = 1.632993161855452*sqrt(inputDQ[0]*inputDQ[0] + inputDQ[1]*inputDQ[1]);
		ml_0 = 0.816496580927726*fabs(inputDQ[2]);
		a = 0.816496580927726;
	}
	else
	{
		ml = sqrt(2.0*(inputDQ[0]*inputDQ[0] + inputDQ[1]*inputDQ[1]));
		ml_0 = 0.707106781186548*fabs(inputDQ[2]);
		a = 0.707106781186548;
	}

	if (ml > 1.0) //case 1: limit d and q, no zero sequence
	{
		outputDQ[0] = inputDQ[0]/ml;
		outputDQ[1] = inputDQ[1]/ml;
		outputDQ[2] = 0; //Zero seq reference set to 0
	}
	else if (ml + ml_0 > 1.0) //case 2: no limit on d and q, limit zero sequence
	{
		outputDQ[0] = inputDQ[0];
		outputDQ[1] = inputDQ[1];
		if (inputDQ[2] > 0.0)
			outputDQ[2] = (1.0-ml)/a;
		else
			outputDQ[2] = -(1.0-ml)/a;
	}
	else //case 3: no limits.
	{
		outputDQ[0] = inputDQ[0];
		outputDQ[1] = inputDQ[1];
		outputDQ[2] = inputDQ[2];

	}
	return SUCCESS;
}

//assumes the dq voltages are a fraction of the dc bus voltage
int SaturateDQVoltages(double *inputDQ, double *outputDQ, u8 bSineTriangle)
{
	double ml;
	if (bSineTriangle)
		ml = 1.632993161855452*sqrt(inputDQ[0]*inputDQ[0] + inputDQ[1]*inputDQ[1]);
	else
		ml = sqrt(2.0*(inputDQ[0]*inputDQ[0] + inputDQ[1]*inputDQ[1]));

	if (ml > 1.0)
	{
		outputDQ[0] = inputDQ[0]/ml;
		outputDQ[1] = inputDQ[1]/ml;
	}
	else
	{
		outputDQ[0] = inputDQ[0];
		outputDQ[1] = inputDQ[1];
	}
	return SUCCESS;
}

int DQ0_Transform(double theta_da, double *abc, double *dq0)
{
	dq0[0] = 	DQK*(cos(theta_da)*abc[0] 	+cos(theta_da - PI23)*abc[1] 	+cos(theta_da + PI23)*abc[2]);
	dq0[1] =   -DQK*(sin(theta_da)*abc[0] 	+sin(theta_da - PI23)*abc[1] 	+sin(theta_da + PI23)*abc[2]);
	dq0[2] = 	DQK*(			abc[0] 		+abc[1] 						+abc[2]);
	return SUCCESS;
}

int InverseDQ0_Transform(double theta_da, double *abc, double *dq0)
{
	abc[0] = DQK*(cos(theta_da)*dq0[0] 			-sin(theta_da)*dq0[1] 			+0.5*dq0[2]);
	abc[1] = DQK*(cos(theta_da - PI23)*dq0[0] 	-sin(theta_da - PI23)*dq0[1] 	+0.5*dq0[2]);
	abc[2] = DQK*(cos(theta_da + PI23)*dq0[0] 	-sin(theta_da + PI23)*dq0[1] 	+0.5*dq0[2]);
	return SUCCESS;
}

double GetMid(double m1, double m2, double m3)
{
	if ( (m2 >= m1) && (m1 >= m3) )
		return m1;
	if ( (m3 >= m1) && (m1 >= m2) )
		return m1;
	if ( (m1 >= m2) && (m2 >= m3))
		return m2;
	if ( (m3 >= m2) && (m2 >= m1))
		return m2;
	if ( (m1 >= m3) && (m3 >= m2) )
		return m3;
	if ( (m2 >= m3) && (m3 >= m1))
		return m3;
	return m1;		//should never get here!
}

//change the reference frame of a dq0 value.
int ChangeDQ_ReferenceFrame(double *new_dq0, double new_theta_da, double *old_dq0, double old_theta_da)
{
	double theta = old_theta_da - new_theta_da;

	new_dq0[0] = old_dq0[0]*cos(theta) - old_dq0[1]*sin(theta);
	new_dq0[1] = old_dq0[0]*sin(theta) + old_dq0[1]*cos(theta);
	new_dq0[2] = old_dq0[2];

	return SUCCESS;
}

#define OUTPUT_FREQ 120.0		//Hz
//#define THETA_DA  GetTheta_DA() /*0.0 //212 //*/ //((double) 2.0*PI*OUTPUT_FREQ*((double)MainTimerTimerTick)/((double)100000.0))


double GetTheta_DA()
{
	static double theta_da = 0;
	double update_da = 0;
	static u32 LastTickCNT = 0;
	u32 elapsed;
	elapsed = TimeElapsed10us(LastTickCNT);
	LastTickCNT = MainTimerTimerTick;
	update_da = ((double) 2.0*PI*OUTPUT_FREQ*((double)elapsed)/((double)100000.0));
	theta_da += update_da;
	while (theta_da > PI2)
		theta_da = theta_da - PI2;

	return theta_da;
}

u32 vsi_theta_da;


static u8 ExampleVSI_NeedsInput(void *arg, void *VSIContext);
static void ExampleVSI_WriteOutput(void *arg, void *VSIContext);

VSI_context VSI_DQ_Cur_Controllers[VSI_DQ_SVPWM_NUM_VSI];
static controller_linked_list_item cont_items [3*VSI_DQ_SVPWM_NUM_VSI];

//callbacks:
static void VSI_Input(void *arg, struct controller_context *cnt);
static void VSI_Output(void *arg, struct controller_context *cnt);

//logging:
u32 VSI_V1_da, VSI_V1_db, VSI_V1_dc;
s32 VSI_V1_d_error, VSI_V1_q_error, VSI_V1_d_current, VSI_V1_q_current;


//non-control -- allows d and q voltages to be forced to a certain value (fraction of dc bus)
double force_vd[VSI_DQ_SVPWM_NUM_VSI];
double force_vq[VSI_DQ_SVPWM_NUM_VSI];
u8 bForceDQ_Voltages = 0;

/*****************************
 * Init_VSI_DQ_VSPWM()
 * Sets up the controllers memory to default values
 * and optionally starts the controllers running.
 * The default settings can be overridden by manually
 * editing the VSI_DQ_Cur_Controllers structure.
 *
 *StartAllControllers: if this is set to TRUE, all the
 * controllers are registered in control.c and started.
 *returns a success code
 */
int Init_VSI_DQ_VSPWM(u8 StartAllControllers)
{
	//pi_house *pi_ptr = HB_Controllers;
	int i;
	for (i = 0; i<VSI_DQ_SVPWM_NUM_VSI; i++)
	{
		force_vd[i] = 0;
		force_vq[i] = 0;

		VSI_DQ_Cur_Controllers[i].bSineTriangle = DEFAULT_SINE_TRIANGLE;

		//PI controllers
		VSI_DQ_Cur_Controllers[i].d.pi.kits = KiTs_D_GAIN;
		VSI_DQ_Cur_Controllers[i].q.pi.kits = KiTs_Q_GAIN;
		VSI_DQ_Cur_Controllers[i].zero.pi.kits = 0.0;
		VSI_DQ_Cur_Controllers[i].d.pi.kp = KP_D_GAIN;
		VSI_DQ_Cur_Controllers[i].q.pi.kp = KP_Q_GAIN;
		VSI_DQ_Cur_Controllers[i].zero.pi.kp = 0.0;



		VSI_DQ_Cur_Controllers[i].d.pi.lastInt = 0;
		VSI_DQ_Cur_Controllers[i].d.pi.plusSat = 0.5;		//limit integral to +- 0.5Vdcbus because this is the max value Vd and Vq can both be (max modulation index)
		VSI_DQ_Cur_Controllers[i].d.pi.negSat = -0.5;
		VSI_DQ_Cur_Controllers[i].q.pi.lastInt = 0;
		VSI_DQ_Cur_Controllers[i].q.pi.plusSat = 0.5;
		VSI_DQ_Cur_Controllers[i].q.pi.negSat = -0.5;
		VSI_DQ_Cur_Controllers[i].zero.pi.lastInt = 0;
		VSI_DQ_Cur_Controllers[i].zero.pi.plusSat = 0.5;
		VSI_DQ_Cur_Controllers[i].zero.pi.negSat = -0.5;

		//basic
		VSI_DQ_Cur_Controllers[i].d.io.inputs = &VSI_DQ_Cur_Controllers[i].d.input;
		VSI_DQ_Cur_Controllers[i].q.io.inputs = &VSI_DQ_Cur_Controllers[i].q.input;
		VSI_DQ_Cur_Controllers[i].zero.io.inputs = &VSI_DQ_Cur_Controllers[i].zero.input;
		VSI_DQ_Cur_Controllers[i].d.io.outputs = &VSI_DQ_Cur_Controllers[i].d.output;
		VSI_DQ_Cur_Controllers[i].q.io.outputs = &VSI_DQ_Cur_Controllers[i].q.output;
		VSI_DQ_Cur_Controllers[i].zero.io.outputs = &VSI_DQ_Cur_Controllers[i].zero.output;
		VSI_DQ_Cur_Controllers[i].d.io.NumInputs = 1;
		VSI_DQ_Cur_Controllers[i].q.io.NumInputs = 1;
		VSI_DQ_Cur_Controllers[i].zero.io.NumInputs = 1;
		VSI_DQ_Cur_Controllers[i].d.io.NumOutputs = 1;
		VSI_DQ_Cur_Controllers[i].q.io.NumOutputs = 1;
		VSI_DQ_Cur_Controllers[i].zero.io.NumOutputs = 1;
		VSI_DQ_Cur_Controllers[i].d.io.NumClocks = 1;
		VSI_DQ_Cur_Controllers[i].q.io.NumClocks = 1;
		VSI_DQ_Cur_Controllers[i].zero.io.NumClocks = 1;
		VSI_DQ_Cur_Controllers[i].d.io.ClockTime = 5; //5us
		VSI_DQ_Cur_Controllers[i].q.io.ClockTime = 5;
		VSI_DQ_Cur_Controllers[i].zero.io.ClockTime = 5;
		VSI_DQ_Cur_Controllers[i].d.io.clockCount = 0;
		VSI_DQ_Cur_Controllers[i].q.io.clockCount = 0;
		VSI_DQ_Cur_Controllers[i].zero.io.clockCount = 0;
		VSI_DQ_Cur_Controllers[i].d.io.Type = C_PI;
		VSI_DQ_Cur_Controllers[i].q.io.Type = C_PI;
		VSI_DQ_Cur_Controllers[i].zero.io.Type = C_PI;
		VSI_DQ_Cur_Controllers[i].d.io.Status = C_RUNNING;
		VSI_DQ_Cur_Controllers[i].q.io.Status = C_RUNNING;
		VSI_DQ_Cur_Controllers[i].zero.io.Status = C_RUNNING;

		//context
		VSI_DQ_Cur_Controllers[i].d.ctxt.arg_in = 0;
		VSI_DQ_Cur_Controllers[i].q.ctxt.arg_in = 0;
		VSI_DQ_Cur_Controllers[i].zero.ctxt.arg_in = 0;
		VSI_DQ_Cur_Controllers[i].d.ctxt.arg_out = 0;
		VSI_DQ_Cur_Controllers[i].q.ctxt.arg_out = 0;
		VSI_DQ_Cur_Controllers[i].zero.ctxt.arg_out = 0;
		VSI_DQ_Cur_Controllers[i].d.ctxt.control_io = &VSI_DQ_Cur_Controllers[i].d.io;
		VSI_DQ_Cur_Controllers[i].q.ctxt.control_io = &VSI_DQ_Cur_Controllers[i].q.io;
		VSI_DQ_Cur_Controllers[i].zero.ctxt.control_io = &VSI_DQ_Cur_Controllers[i].zero.io;
		VSI_DQ_Cur_Controllers[i].d.ctxt.controller_info = &VSI_DQ_Cur_Controllers[i].d.pi;
		VSI_DQ_Cur_Controllers[i].q.ctxt.controller_info = &VSI_DQ_Cur_Controllers[i].q.pi;
		VSI_DQ_Cur_Controllers[i].zero.ctxt.controller_info = &VSI_DQ_Cur_Controllers[i].zero.pi;

		VSI_DQ_Cur_Controllers[i].d.ctxt.input_callback = VSI_Input;
		VSI_DQ_Cur_Controllers[i].q.ctxt.input_callback = VSI_Input;
		VSI_DQ_Cur_Controllers[i].zero.ctxt.input_callback = VSI_Input;

		VSI_DQ_Cur_Controllers[i].d.ctxt.output_callback = VSI_Output;
		VSI_DQ_Cur_Controllers[i].q.ctxt.output_callback = VSI_Output;
		VSI_DQ_Cur_Controllers[i].zero.ctxt.output_callback = VSI_Output;


		VSI_DQ_Cur_Controllers[i].d.reference = 0;
		VSI_DQ_Cur_Controllers[i].q.reference = 0;
		VSI_DQ_Cur_Controllers[i].zero.reference = 0;
		VSI_DQ_Cur_Controllers[i].dqComp[0] = 0;
		VSI_DQ_Cur_Controllers[i].dqComp[1] = 0;

		VSI_DQ_Cur_Controllers[i].bUseZeroSequence = 0;

		//callbacks
		VSI_DQ_Cur_Controllers[i].get_input = ExampleVSI_NeedsInput;
		VSI_DQ_Cur_Controllers[i].write_output = ExampleVSI_WriteOutput;

		//load it:
		cont_items[3*i].context = &VSI_DQ_Cur_Controllers[i].d.ctxt;
		cont_items[3*i+1].context = &VSI_DQ_Cur_Controllers[i].q.ctxt;
		cont_items[3*i+2].context = &VSI_DQ_Cur_Controllers[i].zero.ctxt;
		if (StartAllControllers)
		{
			Start_VSI_DQ_SVPWM_Controller(i+1);
		}
	}
	return SUCCESS;
}

/*****************************
 * Start_VSI_DQ_SVPWM_Controller()
 * Registers the VSI d and q current controllers with control.c.
 *
 *vsi: the VSI number to register (1 - VSI_DQ_SVPWM_NUM_VSI)
 *returns a success code
 */
int Start_VSI_DQ_SVPWM_Controller(u32 vsi)
{
	u32 i = vsi-1;
	if (i >= VSI_DQ_SVPWM_NUM_VSI)
		return INVALID_ARGUMENT;
	if (cont_items[3*i].bRegistered == TRUE)
		return INVALID_OPERATION;
	if (cont_items[3*i+1].bRegistered == TRUE)
		return INVALID_OPERATION;
	if (VSI_DQ_Cur_Controllers[i].bUseZeroSequence)
		if (cont_items[3*i+2].bRegistered == TRUE)
			return INVALID_OPERATION;

	RegisterController(0, &cont_items[3*i]);
	RegisterController(0, &cont_items[3*i+1]);

	if (VSI_DQ_Cur_Controllers[i].bUseZeroSequence)
		RegisterController(0, &cont_items[3*i+2]);

	return SUCCESS;
}

/*****************************
 * Stop_VSI_DQ_SVPWM_Controller()
 * Unregisters the VSI d and q current controllers from control.c.
 *
 *vsi: the VSI number to unregister (1 - VSI_DQ_SVPWM_NUM_VSI)
 *returns a success code
 */
int Stop_VSI_DQ_SVPWM_Controller(u32 vsi)
{
	u32 i = vsi-1;
	if (i >= VSI_DQ_SVPWM_NUM_VSI)
		return INVALID_ARGUMENT;
	if (cont_items[2*i].bRegistered == TRUE)
		UnregisterController(0, &cont_items[3*i]);
	if (cont_items[2*i+1].bRegistered == TRUE)
		UnregisterController(0, &cont_items[3*i+1]);
	if (cont_items[3*i+2].bRegistered == TRUE)
		UnregisterController(0, &cont_items[3*i+2]);

	return SUCCESS;
}

/*****************************
 * VSI_DQ_VSPW_StopCommand()
 * Process an ascii command for starting VSI controllers
 * Init_VSI_DQ_VSPWM is called
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int VSI_DQ_VSPW_StopCommand(const char *szCmd, char *szResponse, void *CommDevice)
{
	int i;
	for (i=0; i< VSI_DQ_SVPWM_NUM_VSI; i++)
	{
		Stop_VSI_DQ_SVPWM_Controller(i+1);
	}

	strcpy(szResponse, "OK");
	return strlen(szResponse);
}

/*****************************
 * VSI_DQ_VSPW_StartCommand()
 * Process an ascii command for starting VSI controllers
 * Init_VSI_DQ_VSPWM is called
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int VSI_DQ_VSPW_StartCommand(const char *szCmd, char *szResponse, void *CommDevice)
{
	if (strstr(szCmd, "FORCEVDQ"))
	{
		strcpy(szResponse, "FORCEVDQ=1");
		bForceDQ_Voltages = TRUE;
	}
	else
	{
		bForceDQ_Voltages = FALSE;
		strcpy(szResponse, "OK");
	}

	Init_VSI_DQ_VSPWM(TRUE);

	return strlen(szResponse);
}

/*****************************
 * VSI_DQ_VSPW_Command()
 * Process an ascii command for setting a reference current
 * Syntax: n,r,val
 *  n is the Vbridge number (1 - VSI_DQ_SVPWM_NUM_VSI),
 *  r is d for direct axis or q for quadrature axis,
 *	val is the current reference value * 128
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int VSI_DQ_VSPW_Command(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *ptr;
	u32 vsi = atoi(szCmd);
	u8 bError = FALSE, bD = FALSE;
	if (vsi < 1 || vsi > VSI_DQ_SVPWM_NUM_VSI)
		bError = TRUE;
	else
	{
		for (ptr = szCmd; ptr != &szCmd[strlen(szCmd)-1]; ptr++)
			if (*ptr == ',')
				break;
		ptr++;	//advance past the comma
		if ( (ptr >= &szCmd[strlen(szCmd)]) || (ptr[1] != ',') || ((*ptr != 'd') && (*ptr != 'q')) )
			bError = TRUE;
		else
		{
			double current_ref;
			if (ptr[0] == 'd')
				bD = TRUE;

			ptr = &ptr[2];
			current_ref = ((double) atoi(ptr))/128;
			if (bForceDQ_Voltages == TRUE)
			{
				if (bD)
					force_vd[vsi-1] = current_ref;
				else
					force_vq[vsi-1] = current_ref;

				strcpy(szResponse, "OK");
			}
			else
			{
				if (VSI_DQ_VSPW_SetCurrentRef(vsi, bD, current_ref) == SUCCESS)
					strcpy(szResponse, "OK");
				else
					bError = TRUE;
			}
		}

	}
	if (bError)
		strcpy(szResponse, "ERROR");

	return strlen(szResponse);
}


/*****************************
 * VSI_DQ_VSPW_SetCurrentRef()
 * Set a d or q axis reference current
 *
 *
 *vsi: index of the VSI to set the reference current of
 *bDaxis: 1 if setting a d-axis quantity, otherwise 0 for q
 *reference: the value of the new reference current
 */
int VSI_DQ_VSPW_SetCurrentRef(u32 vsi, u8 bDaxis, double reference)
{
	if (vsi < 1 || vsi > VSI_DQ_SVPWM_NUM_VSI)
		return INVALID_ARGUMENT;
	if (bDaxis)
		VSI_DQ_Cur_Controllers[vsi-1].d.reference = reference;
	else
		VSI_DQ_Cur_Controllers[vsi-1].q.reference = reference;

	return SUCCESS;
}

//Internal function called by control.c before executing a controller
static void VSI_Input(void *arg, struct controller_context *cnt)
{
	u32 i=0;

	for (i= 0; i< VSI_DQ_SVPWM_NUM_VSI; i++)
	{
		if (&VSI_DQ_Cur_Controllers[i].d.io == cnt)
			break;
		else if (&VSI_DQ_Cur_Controllers[i].q.io == cnt)
			break;
		else if (&VSI_DQ_Cur_Controllers[i].zero.io == cnt)
			break;

	}

	//calculate the error and store it
	if (TimeElapsed10us(VSI_DQ_Cur_Controllers[i].InputAtTickCount) > 3)
	{

		VSI_DQ_Cur_Controllers[i].InputAtTickCount = MainTimerTimerTick;

		if (!VSI_DQ_Cur_Controllers[i].get_input(VSI_DQ_Cur_Controllers[i].input_arg, &VSI_DQ_Cur_Controllers[i]))
			DQ0_Transform(VSI_DQ_Cur_Controllers[i].theta_da, VSI_DQ_Cur_Controllers[i].Iabc, VSI_DQ_Cur_Controllers[i].Idq0);

		VSI_DQ_Cur_Controllers[i].d.input = VSI_DQ_Cur_Controllers[i].d.reference - VSI_DQ_Cur_Controllers[i].Idq0[0];
		VSI_DQ_Cur_Controllers[i].q.input = VSI_DQ_Cur_Controllers[i].q.reference - VSI_DQ_Cur_Controllers[i].Idq0[1];
		//if a zero sequence current controller:
		if (VSI_DQ_Cur_Controllers[i].bUseZeroSequence)
			VSI_DQ_Cur_Controllers[i].zero.input = VSI_DQ_Cur_Controllers[i].zero.reference - VSI_DQ_Cur_Controllers[i].Idq0[2];

		//logging:
		if (i == 0)
		{
			VSI_V1_d_error = (s32) (VSI_DQ_Cur_Controllers[i].d.input*128.0);
			VSI_V1_q_error = (s32) (VSI_DQ_Cur_Controllers[i].q.input*128.0);
			VSI_V1_d_current = (s32) (VSI_DQ_Cur_Controllers[i].Idq0[0]*128.0);
			VSI_V1_q_current = (s32) (VSI_DQ_Cur_Controllers[i].Idq0[1]*128.0);
			vsi_theta_da = (u32) VSI_DQ_Cur_Controllers[i].theta_da*128.0;
		}

	}
}

//Internal function called by control.c when an output is ready
static void VSI_Output(void *arg, struct controller_context *cnt)
{
	u32 i = 0, n = 0;
	double m_dq0[3], mid_m, d_com;
	for (i= 0; i< VSI_DQ_SVPWM_NUM_VSI; i++)
	{
		if (&VSI_DQ_Cur_Controllers[i].d.io == cnt)
		{
			VSI_DQ_Cur_Controllers[i].d.updated = TRUE;
			break;
		}
		else if (&VSI_DQ_Cur_Controllers[i].q.io == cnt)
		{
			VSI_DQ_Cur_Controllers[i].q.updated = TRUE;
			break;
		}
		else if (&VSI_DQ_Cur_Controllers[i].zero.io == cnt)
		{
			VSI_DQ_Cur_Controllers[i].zero.updated = TRUE;
			break;
		}
	}

	if ( (VSI_DQ_Cur_Controllers[i].d.updated == TRUE) && (VSI_DQ_Cur_Controllers[i].q.updated==TRUE) && (VSI_DQ_Cur_Controllers[i].zero.updated==TRUE || (!VSI_DQ_Cur_Controllers[i].bUseZeroSequence)))
	{
		double dq_raw[3], dq_saturated[3];
		//both d and q have been updated, do the inverse dq transform and write out the duty ratios

		VSI_DQ_Cur_Controllers[i].d.updated = FALSE;
		VSI_DQ_Cur_Controllers[i].q.updated = FALSE;
		VSI_DQ_Cur_Controllers[i].zero.updated = FALSE;

		//first saturate the outputs:
		if (bForceDQ_Voltages == FALSE)
		{
			dq_raw[0] = VSI_DQ_Cur_Controllers[i].d.output + VSI_DQ_Cur_Controllers[i].dqComp[0];
			dq_raw[1] = VSI_DQ_Cur_Controllers[i].q.output + VSI_DQ_Cur_Controllers[i].dqComp[1];
			if (VSI_DQ_Cur_Controllers[i].bUseZeroSequence)
				dq_raw[2] = VSI_DQ_Cur_Controllers[i].zero.output;
		}
		else
		{
			//allow to operate VSI open loop
			dq_raw[0] = force_vd[i];
			dq_raw[1] = force_vq[i];
			dq_raw[2] = 0;
		}

		if (VSI_DQ_Cur_Controllers[i].bUseZeroSequence)
			SaturateDQ0Voltages(dq_raw, dq_saturated, VSI_DQ_Cur_Controllers[i].bSineTriangle);
		else
			SaturateDQVoltages(dq_raw, dq_saturated, VSI_DQ_Cur_Controllers[i].bSineTriangle);

		//dq modulation indices
		m_dq0[0] = dq_saturated[0];
		m_dq0[1] = dq_saturated[1];
		if (VSI_DQ_Cur_Controllers[i].bUseZeroSequence)
			m_dq0[2] = dq_saturated[2];
		else
			m_dq0[2] = 0;

		//abc modulation indices
		InverseDQ0_Transform(VSI_DQ_Cur_Controllers[i].theta_da, VSI_DQ_Cur_Controllers[i].mabc, m_dq0);

		//mid modulation indices (for SVPWM)
		if (VSI_DQ_Cur_Controllers[i].bSineTriangle)
			d_com = 0.5;
		else
		{
			mid_m = GetMid(VSI_DQ_Cur_Controllers[i].mabc[0], VSI_DQ_Cur_Controllers[i].mabc[1], VSI_DQ_Cur_Controllers[i].mabc[2]);
			d_com = 0.5*(1+mid_m);
		}

		//calculate duty ratios
		//dtest = 256*(d_com + m_abc[0]);
		VSI_DQ_Cur_Controllers[i].dabc[0] = d_com + VSI_DQ_Cur_Controllers[i].mabc[0];
		VSI_DQ_Cur_Controllers[i].dabc[1] = d_com + VSI_DQ_Cur_Controllers[i].mabc[1];
		VSI_DQ_Cur_Controllers[i].dabc[2] = d_com + VSI_DQ_Cur_Controllers[i].mabc[2];

		for (n = 0; n < 3; n++) //SAFETY make sure we don't wrap around
		{
			if (VSI_DQ_Cur_Controllers[i].dabc[n] > 1.0)
				VSI_DQ_Cur_Controllers[i].dabc[n] = 1.0;
			else if (VSI_DQ_Cur_Controllers[i].dabc[n] < -1.0)
				VSI_DQ_Cur_Controllers[i].dabc[n] = -1.0;
		}

		//write them out:
		VSI_DQ_Cur_Controllers[i].write_output(VSI_DQ_Cur_Controllers[i].output_arg, &VSI_DQ_Cur_Controllers[i]);

	}
}

//return false if ABC values set, true if DQ values set
static u8 ExampleVSI_NeedsInput(void *arg, void *VSIContext)
{
	u32 i=0;
	u32 adc_num[3];
	struct VSI_context *cnt = VSIContext;
	//double meas_abc[3], meas_dq0[3];
	adc_num[0] = 4; adc_num[1] = 5; adc_num[2] = 6;

	for (i= 0; i< VSI_DQ_SVPWM_NUM_VSI; i++)
	{
		if (&VSI_DQ_Cur_Controllers[i] == cnt)
		{
			if (i == 0)
			{
				adc_num[0] = 1; adc_num[1] = 2; adc_num[2] = 3;
				/*Verror = &VSI_V1_d_error;
				Vcurrent = &VSI_V1_d_current;*/
				//bD = TRUE;
			}
			break;
		}
	}

	cnt->Iabc[0] = ReadADC(adc_num[0]);
	cnt->Iabc[1] = ReadADC(adc_num[1]);
	cnt->Iabc[2] = ReadADC(adc_num[2]);
	cnt->theta_da = GetTheta_DA();

	return FALSE;
}

//return false if ABC values set, true if DQ values set
static void ExampleVSI_WriteOutput(void *arg, void *VSIContext)
{
	u32 legs[3];
	u8 duty1, duty2, duty3;
	struct VSI_context *cnt = VSIContext;
	int i;
	legs[0] = 0; legs[1] = 0; legs[2] = 0;
	for (i= 0; i< VSI_DQ_SVPWM_NUM_VSI; i++)
	{
		if (&VSI_DQ_Cur_Controllers[i] == cnt)
		{
			if (i == 0)
				legs[0] = 1; legs[1] = 2; legs[2] = 3;
			break;
		}
	}

	if (i==0)
	{
		//convert the duty ratios
		//dtest = 256*(d_com + m_abc[0]);
		duty1 = (unsigned char) 256.0*(cnt->dabc[0]);
		duty2 = (unsigned char) 256.0*(cnt->dabc[1]);
		duty3 = (unsigned char) 256.0*(cnt->dabc[2]);

		//write them out:
		WriteDutyRatio(legs[0], duty1);
		WriteDutyRatio(legs[1], duty2);
		WriteDutyRatio(legs[2], duty3);

		//logging
		VSI_V1_da = duty1;
		VSI_V1_db = duty2;
		VSI_V1_dc = duty3;

	}
}

