/*
 * HBridge_CC_8Leg.c
 *
 *  Created on: Jun 27, 2014
 *      Author: sever212
 */
#include "../src/project_include.h"

#define KP_GAIN 0.452
#define KiTs_GAIN 0.005
#define ADC_SCALE 1 		//(important to make sure this is recognized as a double and not an int) new: 3 amps per volt, wound around three times gives me a factor of 1 --OLD:3 amps per volt, wound around twice gives me a factor of 3/2



HB_context HB_Controllers[NUM_HBRIDGE];
controller_linked_list_item cont_items [NUM_HBRIDGE];


//Logging
u32 HB_H1_d1, HB_H1_d2, HB_H1_error, HB_H1_output, HB_H1_reference, HB_H1_Current, dummy;
u32 HB_H2_d1, HB_H2_d2, HB_H2_error, HB_H2_output, HB_H2_reference, HB_H2_Current;
u32 HB_H3_d1, HB_H3_d2, HB_H3_error, HB_H3_output, HB_H3_reference, HB_H3_Current;
u32 HB_H4_d1, HB_H4_d2, HB_H4_error, HB_H4_output, HB_H4_reference, HB_H4_Current;

//callback declarations:
static void HB_Input(void *arg, struct controller_context *cnt);
static void HB_Output(void *arg, struct controller_context *cnt);

static u8 ExampleHB_NeedsInput(void *arg, void *HBContext);
static void ExampleHB_WriteOutput(void *arg, void *HBContext);


/*****************************
 * Init_VSI_DQ_VSPWM()
 * Sets up the controllers memory to default values
 * and optionally starts the controllers running.
 * The default settings can be overridden by manually
 * editing the HB_Controllers structure.
 *
 *StartAllControllers: if this is set to TRUE, all the
 * controllers are registered in control.c and started.
 *returns a success code
 */
int Init_HBridge_CC(u8 StartAllControllers)
{
	//pi_house *pi_ptr = HB_Controllers;
	int i;
	for (i = 0; i<NUM_HBRIDGE; i++)
	{
		//PI controller
		HB_Controllers[i].c.pi.kits = KiTs_GAIN;
		HB_Controllers[i].c.pi.kp = KP_GAIN;
		HB_Controllers[i].c.pi.lastInt = 0;
		HB_Controllers[i].c.pi.plusSat = 0.9;
		HB_Controllers[i].c.pi.negSat = -0.9;

		//basic
		HB_Controllers[i].c.io.inputs = &HB_Controllers[i].c.input;
		HB_Controllers[i].c.io.outputs = &HB_Controllers[i].c.output;
		HB_Controllers[i].c.io.NumInputs = 1;
		HB_Controllers[i].c.io.NumOutputs = 1;
		HB_Controllers[i].c.io.NumClocks = 1;
		HB_Controllers[i].c.io.ClockTime = 5; //5 us
		HB_Controllers[i].c.io.clockCount = 0;
		HB_Controllers[i].c.io.Type = C_PI;
		HB_Controllers[i].c.io.Status = C_RUNNING;

		//context
		HB_Controllers[i].c.ctxt.arg_in = 0;
		HB_Controllers[i].c.ctxt.arg_out = 0;
		HB_Controllers[i].c.ctxt.control_io = &HB_Controllers[i].c.io;
		HB_Controllers[i].c.ctxt.controller_info = &HB_Controllers[i].c.pi;

		HB_Controllers[i].c.ctxt.input_callback = HB_Input;
		HB_Controllers[i].c.ctxt.output_callback = HB_Output;

		//HB_Controllers[i].c.adc_scale = ADC_SCALE;
		HB_Controllers[i].c.reference = 0;

		//callbacks
		HB_Controllers[i].get_input = ExampleHB_NeedsInput;
		HB_Controllers[i].write_output = ExampleHB_WriteOutput;

		//load it:
		cont_items[i].context = &HB_Controllers[i].c.ctxt;
		if (StartAllControllers)
			Start_HBridge_CC_Controller(i+1);

	}
	return XST_SUCCESS;
}


/*****************************
 * Start_HBridge_CC_Controller()
 * Registers the HBridge current controller with control.c.
 *
 *HB: the HB number to register (1 - NUM_HBRIDGE)
 *returns a success code
 */
int Start_HBridge_CC_Controller(u32 HB)
{
	u32 i = HB-1;
	if (i >= NUM_HBRIDGE)
		return INVALID_ARGUMENT;
	if (cont_items[i].bRegistered == TRUE)
		return INVALID_OPERATION;

	RegisterController(0, &cont_items[i]);

	return SUCCESS;
}


/*****************************
 * Stop_HBridge_CC_Controller()
 * Unregisters the HBridge current controller from control.c.
 *
 *HB: the HB number to unregister (1 - NUM_HBRIDGE)
 *returns a success code
 */
int Stop_HBridge_CC_Controller(u32 HB)
{
	u32 i = HB-1;
	if (i >= NUM_HBRIDGE)
		return INVALID_ARGUMENT;
	if (cont_items[i].bRegistered == TRUE)
		UnregisterController(0, &cont_items[i]);

	return SUCCESS;
}

/*****************************
 * HBridge_CC_StopCommand()
 * Process an ascii command for stopping the HB controllers
 * Stop_HBridge_CC_Controller is called
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int HBridge_CC_StopCommand(const char *szCmd, char *szResponse, void *CommDevice)
{
	int i;
	for (i=0; i< NUM_HBRIDGE; i++)
		Stop_HBridge_CC_Controller(i+1);

	strcpy(szResponse, "OK");
	return strlen(szResponse);
}


/*****************************
 * HBridge_CC_StartCommand()
 * Process an ascii command for starting HB current controllers
 * Init_HBridge_CC_8Leg is called
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int HBridge_CC_StartCommand(const char *szCmd, char *szResponse, void *CommDevice)
{
	Init_HBridge_CC(TRUE);
	strcpy(szResponse, "OK");
	return strlen(szResponse);
}

/*****************************
 * HBridge_CC_Command()
 * Process an ascii command for setting a reference current
 * Syntax: n,val
 *  n is the Hbridge number (1 - NUM_HBRIDGE),
 *	val is the current reference value * 128
 *
 *szCmd: incoming command
 *szResponse: the response to the command
 *CommDevice: pointer to information about the communication device
 * this was received on
 */
int HBridge_CC_Command(const char * szCmd, char *szResponse, void *CommDevice)
{
	const char *ptr;
	u32 hbridge = atoi(szCmd);
	u8 bError = FALSE;
	if (hbridge < 1 || hbridge > NUM_HBRIDGE)
		bError = TRUE;
	else
	{
		for (ptr = szCmd; ptr != &szCmd[strlen(szCmd)-1]; ptr++)
			if (*ptr == ',')
				break;
		ptr++;	//advance past the comma
		if (ptr >= &szCmd[strlen(szCmd)])
			bError = TRUE;
		else
		{
			double current_ref = ((double) atoi(ptr))/128;
			if (HBridge_CC_SetCurrentRef(hbridge, current_ref) == SUCCESS)
				strcpy(szResponse, "OK");
			else
				bError = TRUE;
		}

	}
	if (bError)
		strcpy(szResponse, "ERROR");

	return strlen(szResponse);
}


/*****************************
 * HBridge_CC_SetCurrentRef()
 * Set a reference current for an H-Bridge
 *
 *hbridge: index of the HBridge to set the reference current of
 *			Should be 1 - NUM_HBRIDGE
 *reference: the value of the new reference current
 */
int HBridge_CC_SetCurrentRef(u32 hbridge, double reference)
{
	if (hbridge < 1 || hbridge > NUM_HBRIDGE)
		return INVALID_ARGUMENT;
	HB_Controllers[hbridge-1].c.reference = reference;

	//logging:
	switch(hbridge)
	{
	case 1:
		HB_H1_reference = (s32)(128 * reference);
		break;
	case 2:
		HB_H2_reference = (s32)(128 * reference);
		break;
	case 3:
		HB_H3_reference = (s32)(128 * reference);
		break;
	case 4:
		HB_H4_reference = (s32)(128 * reference);
		break;
	}
	return SUCCESS;
}


//Internal function called by control.c before executing a controller
static void HB_Input(void *arg, struct controller_context *cnt)
{
	u32 i=0;
	u32 *Herror, *Hcurrent;
	Herror = &dummy;
	Hcurrent = &dummy;

	for (i= 0; i< NUM_HBRIDGE; i++)
		if (&HB_Controllers[i].c.io == cnt)
		{
			switch(i)
			{
			case 0:
				Herror = &HB_H1_error;
				Hcurrent = &HB_H1_Current;
				break;

			case 1:
				Herror = &HB_H2_error;
				Hcurrent = &HB_H2_Current;
				break;

			case 2:
				Herror = &HB_H3_error;
				Hcurrent = &HB_H3_Current;
				break;

			case 3:
				Herror = &HB_H4_error;
				Hcurrent = &HB_H4_Current;
				break;

			}
			break;	//we have our match
		}

	if (i < NUM_HBRIDGE)
	{
		HB_Controllers[i].InputAtTickCount = MainTimerTimerTick;

		//get the input
		HB_Controllers[i].get_input(HB_Controllers[i].input_arg, &HB_Controllers[i]);

		//calculate the error and store it
		HB_Controllers[i].c.input = HB_Controllers[i].c.reference - HB_Controllers[i].I;
		*Herror = (s32) (HB_Controllers[i].c.input*128);
		*Hcurrent =  (s32) (HB_Controllers[i].I*128);
	}
	else
		; //should not happen!!
}

//Internal function called by control.c when an output is ready
static void HB_Output(void *arg, struct controller_context *cnt)
{
	u32 i = 0;

	//logging
	u32 *Hd1, *Hd2, *Houtput;
	Hd1 = &dummy;
	Hd2 = &dummy;
	Houtput = &dummy;

	for (i= 0; i< NUM_HBRIDGE; i++)
		if (&HB_Controllers[i].c.io == cnt)
		{
			//logging
			switch(i)
			{
			case 0:
				Hd1 = &HB_H1_d1;
				Hd2 = &HB_H1_d2;
				Houtput = &HB_H1_output;
				break;

			case 1:
				Hd1 = &HB_H2_d1;
				Hd2 = &HB_H2_d2;
				Houtput = &HB_H2_output;
				break;

			case 2:
				Hd1 = &HB_H3_d1;
				Hd2 = &HB_H3_d2;
				Houtput = &HB_H3_output;
				break;

			case 3:
				Hd1 = &HB_H4_d1;
				Hd2 = &HB_H4_d2;
				Houtput = &HB_H4_output;
				break;
			}
			break;
		}

	if (i < NUM_HBRIDGE)
	{
		//safety to avoid roll-over:
		if (HB_Controllers[i].c.output > 1.0)
			HB_Controllers[i].c.output = 1.0;
		if (HB_Controllers[i].c.output < -1.0)
			HB_Controllers[i].c.output = -1.0;

		//calculate duty ratios
		HB_Controllers[i].d[0] = (unsigned char) 127*(1 + HB_Controllers[i].c.output);
		HB_Controllers[i].d[1] = (unsigned char) 127*(1 - HB_Controllers[i].c.output);

		//write them out:
		HB_Controllers[i].write_output(HB_Controllers[i].output_arg, &HB_Controllers[i]);

		//logging
		*Hd1 = HB_Controllers[i].d[0];
		*Hd2 = HB_Controllers[i].d[1];
		*Houtput = (s32) (HB_Controllers[i].c.output*128);
	}
	else
		; //should not happen!!
}

//HB_Controllers[i].get_input = ExampleHB_NeedsInput;
//HB_Controllers[i].write_output = ExampleHB_WriteOutput;

//return false
static u8 ExampleHB_NeedsInput(void *arg, void *HBContext)
{
	u32 i=0;
	u32 adc_num = 0;
	struct HB_context *cnt = HBContext;

	for (i= 0; i< NUM_HBRIDGE; i++)
		if (&HB_Controllers[i] == cnt)
		{
			if (i == 0)
				adc_num = 4;
			else if (i == 1)
				adc_num = 5;
			else if (i == 2)
				adc_num = 6;
			else if (i == 3)
				adc_num = 1;
			break;	//we have our match
		}

	//measure the current and store it
	if (i < NUM_HBRIDGE)
		HB_Controllers[i].I = 0.6*ReadADC(adc_num);
	else
		; //uh oh! should not get here!

	return FALSE;
}


//return false if ABC values set, true if DQ values set
static void ExampleHB_WriteOutput(void *arg, void *HBContext)
{
	u32 legs[2];
	struct HB_context *cnt = HBContext;
	int i;

	legs[0] = 0; legs[1] = 0; legs[2] = 0;
	for (i= 0; i< NUM_HBRIDGE; i++)
	{
		if (&HB_Controllers[i] == cnt)
		{
			if (i == 0)
				{ legs[0] = 5; legs[1] = 6; }
			else if (i == 1)
				{ legs[0] = 13; legs[1] = 14; }
			else if (i == 2)
				{ legs[0] = 17; legs[1] = 18; }
			else if (i == 3)
				{ legs[0] = 11; legs[1] = 12; }
			else
				i = NUM_HBRIDGE;	//safety, don't write out to other legs.
			break;
		}
	}

	if (i < NUM_HBRIDGE)
	{
		//write them out:
		WriteDutyRatio(legs[0], HB_Controllers[i].d[0]);
		WriteDutyRatio(legs[1], HB_Controllers[i].d[1]);
	}
}

