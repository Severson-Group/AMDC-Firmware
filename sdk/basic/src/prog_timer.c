/*
 * timer.c
 *
 *  Created on: Jun 13, 2014
 *      Author: sever212
 */
#if 0
#include "platform.h"
#include "xparameters.h"
#include "netif/xadapter.h"
#include "xgpio.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xuartps_hw.h"
#include <stdio.h>
#include "commands.h"
#include "Log.h"
#include "prog_timer.h"
#include "lwip_glue.h"
#endif
#include "project_include.h"

static void TimerIntrHandler(void *CallBackRef);
static int TimerSetupIntrSystem(XScuGic *IntcInstancePtr,
								XScuTimer *TimerInstancePtr, u16 TimerIntrId);
//static void TimerDisableIntrSystem(XScuGic *IntcInstancePtr, u16 TimerIntrId);

volatile u32 MainTimerTimerTick;
volatile char bTimer100usTick = 0;
volatile u32 Timing10usErrorCount = 0;

int SetupTimer(XScuGic *IntcInstancePtr, XScuTimer * TimerInstancePtr,
		u16 TimerDeviceId, u16 TimerIntrId)
{
	int Status;
		//int Timer10ms = 0;
		XScuTimer_Config *ConfigPtr;

		/*
		 * Initialize the Scu Private Timer driver.
		 */
		ConfigPtr = XScuTimer_LookupConfig(TimerDeviceId);

		/*
		 * This is where the virtual address would be used, this example
		 * uses physical address.
		 */
		Status = XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr,
						ConfigPtr->BaseAddr);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Perform a self-test to ensure that the hardware was built correctly.
		 */
		Status = XScuTimer_SelfTest(TimerInstancePtr);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Connect the device to interrupt subsystem so that interrupts
		 * can occur.
		 */
		Status = TimerSetupIntrSystem(IntcInstancePtr,
						TimerInstancePtr, TimerIntrId);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Enable Auto reload mode.
		 */
		XScuTimer_EnableAutoReload(TimerInstancePtr);

		/*
		 * Load the timer counter register.
		 */
		XScuTimer_LoadTimer(TimerInstancePtr, TIMER_LOAD_VALUE);

		/*
		 * Start the timer counter and then wait for it
		 * to timeout a number of times.
		 */
		XScuTimer_Start(TimerInstancePtr);

		return XST_SUCCESS;
}



/*****************************
 * TimeElapsed10us()
 * Return the time elapsed since old in units of 10us
 *
 *
 * old: an old timer tick value
 */
u32 TimeElapsed10us(u32 old)
{
	if (MainTimerTimerTick >= old)
		return (MainTimerTimerTick - old);
	else
		return (0xFFFFFFFF - (old - MainTimerTimerTick)) + 1;
}

/*****************************************************************************/
/**
*
* This function sets up the interrupt system such that interrupts can occur
* for the device.
*
* @param	IntcInstancePtr is a pointer to the instance of XScuGic driver.
* @param	TimerInstancePtr is a pointer to the instance of XScuTimer
*		driver.
* @param	TimerIntrId is the Interrupt Id of the XScuTimer device.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
static int TimerSetupIntrSystem(XScuGic *IntcInstancePtr,
			      XScuTimer *TimerInstancePtr, u16 TimerIntrId)
{
	int Status;

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, TimerIntrId, /*0xA8*/ 0 /*0xA0*/, 0x3);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, TimerIntrId,
				(Xil_ExceptionHandler)TimerIntrHandler,
				(void *)TimerInstancePtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the interrupt for the device.
	 */
	XScuGic_Enable(IntcInstancePtr, TimerIntrId);

	/*
	 * Enable the timer interrupts for timer mode.
	 */
	XScuTimer_EnableInterrupt(TimerInstancePtr);


	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function is the Interrupt handler for the Timer interrupt of the
* Timer device. It is called on the expiration of the timer counter in
* interrupt context.
*
* @param	CallBackRef is a pointer to the callback function.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void TimerIntrHandler(void *CallBackRef)
{

	XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;
	//static u32 local1us = 0;
	static u32 tickTo10Counter = 0, tickTo1msCounter = 0;
	static u32 Eth_1ms = 0;
	/*
	 * Check if the timer counter has expired, checking is not necessary
	 * since that's the reason this function is executed, this just shows
	 * how the callback reference can be used as a pointer to the instance
	 * of the timer counter that expired, increment a shared variable so
	 * the main thread of execution can see the timer expired.
	 */
	if (XScuTimer_IsExpired(TimerInstancePtr)) {
		XScuTimer_ClearInterruptStatus(TimerInstancePtr);

		MainTimerTimerTick++;
		//local1us += 10;

		if (++tickTo10Counter > 9)
		{
			//every 100 us code

			bTimer100usTick = 1;
			tickTo10Counter = 0;

			if (++tickTo1msCounter > 9)
			{
				//every 1ms code

				if ( (++Eth_1ms > 249) && (bEnableEthTimer == TRUE) )
				{
					Eth_1ms = 0;

					//Ethernet stuff needs to be dealt with every 250ms
					bEth250ms = TRUE;
				}
			}

		}

		DoLogging();

		/*if (Timer10ms == 500)
		{
			Timer10ms = 0;
			if (bTimer)
				print("5 seconds elapsed\r\n");
		}
		if (local1us >= 1000)
		{
			local1us = 0;
		}*/

		/*TimerExpired++;
		if (TimerExpired == 3) {
			XScuTimer_DisableAutoReload(TimerInstancePtr);
		}*/

		if (XScuTimer_IsExpired(TimerInstancePtr))
			Timing10usErrorCount++;
	}

}

/*****************************************************************************/
/**
*
* This function disables the interrupts that occur for the device.
*
* @param	IntcInstancePtr is the pointer to the instance of XScuGic
*		driver.
* @param	TimerIntrId is the Interrupt Id for the device.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
//#if 0
void TimerDisableIntrSystem(XScuGic *IntcInstancePtr, u16 TimerIntrId)
{
	/*
	 * Disconnect and disable the interrupt for the Timer.
	 */
	XScuGic_Disconnect(IntcInstancePtr, TimerIntrId);
}
//#endif

