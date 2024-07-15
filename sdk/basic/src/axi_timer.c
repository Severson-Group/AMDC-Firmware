/*
 * control_timer.c
 *
 *  Created on: Jun 23, 2014
 *      Author: sever212
 */

#include "project_include.h"



static int AXI_TimerSetupIntrSystem(XScuGic *IntcInstancePtr,
		XTmrCtr *TimerInstancePtr, u16 TimerIntrId, u8 priority);
/*static int Init_AXI_Timer(XScuGic* IntcInstancePtr, XTmrCtr* TmrCtrInstancePtr,
		u16 DeviceId, u16 IntrId, u8 TmrCtrNumber,
		XTmrCtr_Handler InterruptHandler, u8 IntrPriority, u32 ResetValue);*/


/*****************************************************************************/
/**
 * It initializes a timer counter and then sets it up in
 * compare mode with auto reload such that a periodic interrupt is generated.
 *
 * This function uses interrupt driven mode of the timer counter.
 *
 * @param	IntcInstancePtr is a pointer to the Interrupt Controller
 *		driver Instance
 * @param	TmrCtrInstancePtr is a pointer to the XTmrCtr driver Instance
 * @param	DeviceId is the XPAR_<TmrCtr_instance>_DEVICE_ID value from
 *		xparameters.h
 * @param	IntrId is XPAR_<INTC_instance>_<TmrCtr_instance>_INTERRUPT_INTR
 *		value from xparameters.h
 * @param	TmrCtrNumber is the number of the timer to which this
 *		handler is associated with.
 *
 * @return	XST_SUCCESS if the Test is successful, otherwise XST_FAILURE
 *
 * @note		This function contains an infinite loop such that if interrupts
 *		are not working it may never return.
 *
 *****************************************************************************/
int Init_AXI_Timer(XScuGic* IntcInstancePtr, XTmrCtr* TmrCtrInstancePtr,
		u16 DeviceId, u16 IntrId, u8 TmrCtrNumber,
		XTmrCtr_Handler InterruptHandler, u8 IntrPriority, u32 ResetValue) {
	int Status;

	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	Status = XTmrCtr_Initialize(TmrCtrInstancePtr, DeviceId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built
	 * correctly, use the 1st timer in the device (0)
	 */
	Status = XTmrCtr_SelfTest(TmrCtrInstancePtr, TmrCtrNumber);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the timer counter to the interrupt subsystem such that
	 * interrupts can occur.  This function is application specific.
	 */
	Status = AXI_TimerSetupIntrSystem(IntcInstancePtr, TmrCtrInstancePtr,
			IntrId, IntrPriority);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setup the handler for the timer counter that will be called from the
	 * interrupt context when the timer expires, specify a pointer to the
	 * timer counter driver instance as the callback reference so the handler
	 * is able to access the instance data
	 */
	XTmrCtr_SetHandler(TmrCtrInstancePtr, InterruptHandler, TmrCtrInstancePtr);

	/*
	 * Enable the interrupt of the timer counter so interrupts will occur
	 * and use auto reload mode such that the timer counter will reload
	 * itself automatically and continue repeatedly, without this option
	 * it would expire once only
	 */
	XTmrCtr_SetOptions(TmrCtrInstancePtr, TmrCtrNumber,
			XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

	/*
	 * Set a reset value for the timer counter such that it will expire
	 * eariler than letting it roll over from 0, the reset value is loaded
	 * into the timer counter when it is started
	 */
	XTmrCtr_SetResetValue(TmrCtrInstancePtr, TmrCtrNumber, ResetValue);

	/*
	 * Start the timer counter such that it's incrementing by default,
	 * then wait for it to timeout a number of times
	 */
	XTmrCtr_Start(TmrCtrInstancePtr, TmrCtrNumber);

	return XST_SUCCESS;
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
 * @param 	InterruptHandler is the function to handle the interrupts
 * @param 	priority is the priority for the interrupts (0 is highest
 * 			priority, 0xF8 (248) is lowest.
 *
 * @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
 *
 * @note		None.
 *
 ******************************************************************************/
static int AXI_TimerSetupIntrSystem(XScuGic *IntcInstancePtr,
		XTmrCtr *TimerInstancePtr, u16 TimerIntrId, u8 priority) {
	int Status;

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	XScuGic_SetPriorityTriggerType(IntcInstancePtr, TimerIntrId, /*0xA8*/ priority /*0xA0*/, 0x3);

	Status = XScuGic_Connect(IntcInstancePtr, TimerIntrId,
			(XInterruptHandler) XTmrCtr_InterruptHandler,
			(void *) TimerInstancePtr);
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
	//should I do this?? XScuTimer_EnableInterrupt(TimerInstancePtr);

	return XST_SUCCESS;
}



