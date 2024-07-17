/*
 * control.c
 *
 *  Created on: Jun 24, 2014
 *      Author: sever212
 */

#include "project_include.h"

/*4294947296 for 10kHz*/
/*4294957296 for 20kHz*/
#define TIMER0_TICK_FREQ 20000
#define TIMER0_RESET_VAL ((0xFFFFFFFF-XPAR_CONTROL_TIMER_0_CLOCK_FREQ_HZ/TIMER0_TICK_FREQ) + 1) //4294957296 //32bit counter, 100MHz clock, time = (0x1_0000_0000-RESET_VALUE)/200e6 --> RESET_VALUE = 4294967296 - time*200e6
									//I don't know why the clock is 200MHz, maybe it is clocked from the AXI clock?

#define TIMER1_TICK_FREQ 1000
#define TIMER1_RESET_VAL ((0xFFFFFFFF-XPAR_CONTROL_TIMER_1_CLOCK_FREQ_HZ/TIMER1_TICK_FREQ) + 1) //4294957296 //32bit counter, 100MHz clock, time = (0x1_0000_0000-RESET_VALUE)/200e6 --> RESET_VALUE = 4294967296 - time*200e6
									//I don't know why the clock is 200MHz, maybe it is clocked from the AXI clock?


#define AXI_TIMER0_PRIORITY 0xA8		//0xF8 is lowest priority, 0 is highest: READ THIS: actually if the priority's upper nibble is F, it doesn't work at all!
#define AXI_TIMER1_PRIORITY 0xA9		//0xF8 is lowest priority, 0 is highest: READ THIS: actually if the priority's upper nibble is F, it doesn't work at all!


#define TMRCTR0_INTERRUPT_ID		61 //I don't know why, but it won't recognize these XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR
#define TMRCTR1_INTERRUPT_ID		62 //XPAR_FABRIC_AXI_TIMER_1_INTERRUPT_INTR

#define TMRCTR0_DEVICE_ID		XPAR_TMRCTR_0_DEVICE_ID
#define TMRCTR1_DEVICE_ID		XPAR_TMRCTR_1_DEVICE_ID

#define TIMER_CNTR_0	 0
#define TIMER_CNTR_1	 0

XTmrCtr TmrCtrInstance0;
XTmrCtr TmrCtrInstance1;
static void ControlTimer0_ISR(void *CallBackRef, u8 TmrCtrNumber);
static void ControlTimer1_ISR(void *CallBackRef, u8 TmrCtrNumber);
volatile u32 ControlTimer0_TickCount = 0;
volatile u32 ControlTimer1_TickCount = 0;

volatile u32 ControlTimer0ErrorCount = 0;
volatile u32 ControlTimer1ErrorCount = 0;

struct controller_linked_list_item *C0s = 0;		//list of the PI controllers for controller 0
//struct controller_linked_list_item *C0_PIDs = 0;		//list of the PI controllers for controller 0
struct controller_linked_list_item *C1s = 0;		//list of the PI controllers for controller 0
//struct controller_linked_list_item *C1_PIDs = 0;		//list of the PI controllers for controller 0

/************************TIMERS************************/
static int InitControlTimers(XScuGic* IntcInstancePtr)
{
	XTmrCtr *TmrCtrInstance0Ptr =  &TmrCtrInstance0;
	XTmrCtr *TmrCtrInstance1Ptr =  &TmrCtrInstance1;
	int Status;

	Status = Init_AXI_Timer(IntcInstancePtr,
			TmrCtrInstance0Ptr, TMRCTR0_DEVICE_ID, TMRCTR0_INTERRUPT_ID,
			TIMER_CNTR_0, ControlTimer0_ISR, AXI_TIMER0_PRIORITY, TIMER0_RESET_VAL);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*return Init_AXI_Timer(IntcInstancePtr,
			TmrCtrInstance1Ptr, TMRCTR1_DEVICE_ID, TMRCTR1_INTERRUPT_ID,
			TIMER_CNTR_1, ControlTimer1_ISR, AXI_TIMER1_PRIORITY, TIMER1_RESET_VAL);*/
	return XST_SUCCESS;

}

static void ServiceControllers(struct controller_linked_list_item *cnts)
{
	while(cnts)
	{
		struct controller_context *basic_cnt = cnts->context->control_io;
		if (++(basic_cnt->clockCount) >= basic_cnt->NumClocks)
		{
			basic_cnt->clockCount = 0;

			//input callback
			cnts->context->input_callback(cnts->context->arg_in, basic_cnt);

			//run the controller
			switch(basic_cnt->Type)
			{
				case C_PI:
					RunPI_Controller(cnts->context);
					break;
				case C_PID:
					RunPID_Controller (cnts->context);
				default:
					//UNSUPPORTED!
					break;
			}

			//output callback
			cnts->context->output_callback(cnts->context->arg_out, basic_cnt);
		}
		cnts = cnts->next;
		//basic_cnt = ((struct pi_controller *)cnts->controller)->controller;
	}
}

u32 starve = 0;
volatile unsigned char currentlyInISR = 0;
u32 LastMainTickCount = 0;
static void ControlTimer0_ISR(void *CallBackRef, u8 TmrCtrNumber) {
	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;
	u32 ControlStatusReg = XTmrCtr_ReadReg(InstancePtr->BaseAddress,
						TmrCtrNumber, XTC_TCSR_OFFSET);
	//clear the interrupt
	XTmrCtr_WriteReg(InstancePtr->BaseAddress,
							 TmrCtrNumber,
							 XTC_TCSR_OFFSET,
							 ControlStatusReg |
							 XTC_CSR_INT_OCCURED_MASK);
	if (currentlyInISR)
		ControlTimer0ErrorCount++;
	else
	{

		u32 startMainTicks = MainTimerTimerTick;
		if (TimeElapsed10us(LastMainTickCount) > 6)
			ControlTimer0ErrorCount++;

		LastMainTickCount = startMainTicks;
		//u32 i;
		ControlTimer0_TickCount++;

		currentlyInISR = 1;
		Xil_EnableNestedInterrupts();

		//do PIs:
		ServiceControllers(C0s);

		/*
		for (i=0; i<starve; i++)
			ServiceControllers(C0_PIs);
		starve++;
		//if (ControlTimer0_Count % 100000 == 0)*/
		/*if (ControlTimer0_TickCount == 200000 )
		{
			ControlTimer0_TickCount = 0;
			print("10 second tick - CISR0\r\n");
		}*/

		//if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber))
		if (TimeElapsed10us(startMainTicks) > 5)
		{
			ControlTimer0ErrorCount++;
			starve = 0;
		}
		currentlyInISR = 0;
		Xil_DisableNestedInterrupts();
	}
}

static void ControlTimer1_ISR(void *CallBackRef, u8 TmrCtrNumber) {

	u32 startMainTicks = MainTimerTimerTick;
	ControlTimer1_TickCount++;
	//if (ControlTimer0_Count % 100000 == 0)
	/*if (ControlTimer1_TickCount == 10000 )
	{
		ControlTimer1_TickCount = 0;
		print("10 second tick - CISR1\r\n");
	}*/
	if (TimeElapsed10us(startMainTicks) >= 10)
			ControlTimer1ErrorCount++;
}
/*****************************************************/

static double SaturationBlock(double in, double sat_up, double sat_lower)
{
	if (in > sat_up)
		in = sat_up;
	else if (in < sat_lower)
		in = sat_lower;
	return in;
}

static double BasicIntegrator(double in, double last, double sat_up, double sat_lower)
{
	return SaturationBlock(in + last, sat_up, sat_lower);
}

int InitControllers(XScuGic* IntcInstancePtr)
{
	int Status = InitControlTimers(IntcInstancePtr);

	if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	return XST_SUCCESS;
}

void RemoveLinkedListItem(struct controller_linked_list_item **list, struct controller_linked_list_item *item)
{
	if (*list)
	{
		struct controller_linked_list_item *head = *list;
		while ( (head != item) && (head->next) )
			head = head->next;

		//okay, we either found our item or it isnt here:
		if (head == item)
		{
			if (head->prev)
				head->prev->next = item->next;
			if (head->next)
				head->next->prev = item->prev;
			//update where the list points to
			if (head == *list)
				*list = head->next;
		}
		item->prev = 0;
		item->next = 0;
		item->bRegistered = FALSE;
	}
}

void AddLinkedListItem(struct controller_linked_list_item **list, struct controller_linked_list_item *item)
{
	if (*list)
	{
		struct controller_linked_list_item *head = *list;
		while (head->next)
			head = head->next;

		//okay, at the end
		head->next = item;
		item->prev = head;
		item->next = 0;
	}
	else
	{
		*list = item;
		(*list)->next = 0;
		(*list)->prev = 0;
	}
	item->bRegistered = TRUE;
}

int UnregisterController(int contNum, struct controller_linked_list_item *cnt)
{
	if (contNum == 0)
		RemoveLinkedListItem(&C0s, cnt);
	else if (contNum == 1)
		RemoveLinkedListItem(&C1s, cnt);
	else
		return XST_FAILURE;

	return XST_SUCCESS;
}

int RegisterController(int contNum, struct controller_linked_list_item *cnt)
{
	if (contNum == 0)
		AddLinkedListItem(&C0s, cnt);
	else if (contNum == 1)
		AddLinkedListItem(&C1s, cnt);
	else
		return XST_FAILURE;

	return XST_SUCCESS;
}

/*****************************
 * RunPI_Controller()
 * Do the computation for a pi controller
 *
 *
 *
 *pi: the pi controller context
 */
void RunPI_Controller (controller_holder* ctxt)
{
	struct pi_controller *pi = ctxt->controller_info;
	struct controller_context *cont_io = ctxt->control_io;
	double error = *cont_io->inputs;
	double p = pi->kp*error;
	//double kiTs = pi->ki* ((double)(cont_io->NumClocks*cont_io->ClockTime))*0.000001;
	double i_in = pi->kits*error;
	pi->lastInt = BasicIntegrator(i_in, pi->lastInt, pi->plusSat, pi->negSat);
	*(cont_io->outputs) = pi->lastInt + p; //don't saturate output, just prevent windup //SaturationBlock(pi->lastInt + p, pi->plusSat, pi->negSat);

	//check for NaN (conditions with NaN are always false)
	if (pi->lastInt != pi->lastInt)
		pi->lastInt = 0;
}

s32 PID_D, PID_I, PID_P, PID_LastD, PID_Err, PID_out;
/*****************************
 * RunPID_Controller()
 * Do the computation for our custom pid controller
 *
 *
 *
 *ctxt: the controller context
 */
void RunPID_Controller (controller_holder* ctxt)
{
	static int i = 0;
	struct pid_controller *pid = ctxt->controller_info;
	struct controller_context *cont_io = ctxt->control_io;
	double error = *cont_io->inputs;
	double p = pid->kp*error;
	double i_in = pid->kits*error;
	double d = pid->kdn*error + pid->kdn1*pid->lastErr + pid->kdm1*pid->lastD;
	pid->lastInt = BasicIntegrator(i_in, pid->lastInt, pid->plusSat, pid->negSat);
	pid->lastD = SaturationBlock(d, pid->plusSat, pid->negSat); //saturate to prevent windup
	pid->lastErr = error;

	*(cont_io->outputs) = pid->lastInt + p + d; //don't saturate output, just prevent windup //SaturationBlock(pid->lastInt + p + d, pid->plusSat, pid->negSat);
	//*(cont_io->outputs) = SaturationBlock(pid->lastInt + p + d, 10.0*pid->plusSat, 10.0*pid->negSat);

	//check for NaN (conditions with NaN are always false)
	if (pid->lastInt != pid->lastInt)
		pid->lastInt = 0;
	if (pid->lastD != pid->lastD)
		pid->lastD = 0;
	if (pid->lastErr != pid->lastErr)
		pid->lastErr = 0;

	//logging
	if (i == 0)
	{
		PID_D = (s32) (128*d);
		PID_I = (s32) (128*(pid->lastInt));
		PID_P = (s32) (128*p);
		PID_LastD = (s32) (128*(pid->lastD));
		PID_Err = (s32) (512*(error));
		PID_out = (s32) (128*(*(cont_io->outputs)));
	}
	i++;
	if (i == 4)
		i = 0;
}


