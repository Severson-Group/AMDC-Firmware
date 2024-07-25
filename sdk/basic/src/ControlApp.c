/*
 * ControlApp.c
 *
 * This file houses the control application
 *  Created on: Jun 24, 2014
 *      Author: sever212
 */

#include "project_include.h"

typedef struct c_pi_house
{
	pi_controller pi;
	controller_context io;
	controller_holder ctxt;
	double input;
	double output;
	double reference;
	double adc_scale;		//3 amps per volt, wound around twice gives me a factor of 3/2
} c_pi_house;

c_pi_house PI_Controllers[NUM_PI_CONTROLLERS];
controller_linked_list_item cont_items [NUM_PI_CONTROLLERS];

//Logging
u32 Leg1_d1;
u32 Leg1_d2;
s32 Leg1_error;
s32 Leg1_output;
s32 Leg1_reference = 0;
s32 Leg1_ADC;
s32 Leg1_Current;

static void HBridgeCurrent_Input(void *arg, struct controller_context *cnt);
static void HBridgeCurrent_Output(void *arg, struct controller_context *cnt);

//#define HB8L
//#define VSI_DQ_SVPWM
#define PARALLEL_SUSPENSION

int ControlStopAppCommand(const char *szCmd, char *szResponse, void *CommDevice)
{
#if defined(HB8L)
	return HBridge_CC_8Leg_StopCommand(szCmd, szResponse, CommDevice);
#elif defined(VSI_DQ_SVPWM)
	return VSI_DQ_VSPW_StopCommand(szCmd, szResponse, CommDevice);
#elif defined (PARALLEL_SUSPENSION)
	return PosContTry1_StopCommand(szCmd, szResponse, CommDevice);
#else
	int i;
	for (i = 0; i< NUM_PI_CONTROLLERS; i++)
		UnregisterPIController(0, &cont_items[i]);

	strcpy(szResponse, "OK");
	return strlen(szResponse);
#endif
}

int ControlStartAppCommand(const char *szCmd, char *szResponse, void *CommDevice)
{
#if defined(HB8L)
	return HBridge_CC_8Leg_StartCommand(szCmd, szResponse, CommDevice);
#elif defined(VSI_DQ_SVPWM)
	return VSI_DQ_VSPW_StartCommand(szCmd, szResponse, CommDevice);
#elif defined (PARALLEL_SUSPENSION)
	return PosContTry1_StartCommand(szCmd, szResponse, CommDevice);
#else
	InitControlApp();
	strcpy(szResponse, "OK");
	return strlen(szResponse);
#endif
}

int ControlAppCommand(const char * szCmd, char *szResponse, void *CommDevice)
{
#if defined(HB8L)
	return HBridge_CC_8Leg_Command(szCmd, szResponse, CommDevice);
#elif defined (VSI_DQ_SVPWM)
	return VSI_DQ_VSPW_Command(szCmd, szResponse, CommDevice);
#elif defined (PARALLEL_SUSPENSION)
	return PosContTry1_Command(szCmd, szResponse, CommDevice);
#else
	s32 current = atoi(szCmd);
	s32 *L1R = &Leg1_reference;
	PI_Controllers[0].reference = ((double) current)/128;
	strcpy(szResponse, "OK");
	*L1R = current;
	return strlen(szResponse);
#endif
}

#if !defined(HB8L) && !defined(VSI_DQ_SVPWM) && !defined(PARALLEL_SUSPENSION)
int InitControlApp()
{
	/*int i;
	for(i = 0; i<NUM_PI_CONTROLLERS; i++)
	{
		PI_Controllers[i].pi
	}*/

	//PI controller
	PI_Controllers[0].pi.kits = 0.005;
	PI_Controllers[0].pi.kp = 0.452;
	PI_Controllers[0].pi.lastInt = 0;
	PI_Controllers[0].pi.plusSat = 0.9;
	PI_Controllers[0].pi.negSat = -0.9;

	//basic
	PI_Controllers[0].io.inputs = &PI_Controllers[0].input;
	PI_Controllers[0].io.outputs = &PI_Controllers[0].output;
	PI_Controllers[0].io.NumInputs = 1;
	PI_Controllers[0].io.NumOutputs = 1;
	PI_Controllers[0].io.NumClocks = 1;
	PI_Controllers[0].io.ClockTime = 10; //10 us
	PI_Controllers[0].io.clockCount = 0;
	PI_Controllers[0].io.Type = C_PI;
	PI_Controllers[0].io.Status = C_RUNNING;

	//context
	PI_Controllers[0].ctxt.arg_in = 0;
	PI_Controllers[0].ctxt.arg_out = 0;
	PI_Controllers[0].ctxt.control_io = &PI_Controllers[0].io;
	PI_Controllers[0].ctxt.controller_info = &PI_Controllers[0].pi;
	PI_Controllers[0].ctxt.input_callback = HBridgeCurrent_Input;
	PI_Controllers[0].ctxt.output_callback = HBridgeCurrent_Output;

	PI_Controllers[0].adc_scale = 1;  //new: 3 amps per volt, wound around three times gives me a factor of 1 --OLD:3 amps per volt, wound around twice gives me a factor of 3/2
	PI_Controllers[0].reference = 0;

	//load it:
	cont_items[0].context = &PI_Controllers[0].ctxt;
	RegisterPIController(0, &cont_items[0]);
	return XST_SUCCESS;
}


static void HBridgeCurrent_Input(void *arg, struct controller_context *cnt)
{
	//calculate the error and store it
	PI_Controllers[0].input = PI_Controllers[0].reference - PI_Controllers[0].adc_scale*ReadADC(1);
	Leg1_error = (s32) (PI_Controllers[0].input*128);
	Leg1_ADC = (s32) (ReadADC(1)*128);
	Leg1_Current =  (s32) (PI_Controllers[0].adc_scale*ReadADC(1)*128);
}

static void HBridgeCurrent_Output(void *arg, struct controller_context *cnt)
{
	u8 duty1 = (unsigned char) 127*(1 + PI_Controllers[0].output);
	u8 duty2 = (unsigned char) 127*(1 - PI_Controllers[0].output);

	//logging
	Leg1_d1 = duty1;
	Leg1_d2 = duty2;
	Leg1_output = (s32) (PI_Controllers[0].output*128);

	//plus voltage
	WriteDutyRatio(1, duty1 );
	//minus voltage
	WriteDutyRatio(2, duty2 );
}
#endif




