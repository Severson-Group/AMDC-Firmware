/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

//#include <stdio.h>
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
#endif
#include "project_include.h"
/************************** Constant Definitions *****************************/
/*
 * The following constant maps to the name of the hardware instances that
 * were created in the EDK XPS system.
 */

//#define GPIO_EXAMPLE_DEVICE_ID          XPAR_AXI_GPIO_0_DEVICE_ID
//#define LED_CHANNEL                     1
//#define BUTTON_CHANNEL                  2
#define INTC_DEVICE_ID                  XPAR_PS7_SCUGIC_0_DEVICE_ID
//#define INTC_GPIO_INTERRUPT_ID          XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define INTC                            XScuGic
#define INTC_HANDLER                    XScuGic_InterruptHandler
//#define BUTTON_INTERRUPT                XGPIO_IR_CH2_MASK
//#define INTENSITY_STEP                  110000

//Internal functions
//int SetupGPIO_Interrupt(XScuGic *IntcInstancePtr);
int SetupInterruptSystem(XScuGic *IntcInstancePtr);
//void GpioIsr(void *InstancePtr);
void HandleTimer100usTick();
//	/void CommandParser(const char * szCmd, char *szResponse);
//void HandleCommands();
//void UpdateLEDs();

/* missing declaration in lwIP */
void lwip_init();

////GPIO
//XGpio Gpio; /* The Instance of the GPIO Driver */
//u32 count = 0;

//Interrupts
XScuGic IntcInstance; /* Interrupt Controller Instance */


//UART stuff
char szRcvCmdUart[4096];

//timer stuff:
/*static int SetupTimer(XScuGic *IntcInstancePtr, XScuTimer * TimerInstancePtr,
 u16 TimerDeviceId, u16 TimerIntrId);*/

XScuTimer TimerInstance; /* Cortex A9 Scu Private Timer Instance */

volatile u32 Timer10ms;

u32 Timing100usErrorCount = 0;

u32 bTimer = 0;

u32 ConvertCharToByte(u32 i) {
	u32 res = i;
	if (res <= 0x39)
		res -= 0x30;
	else if (res <= 'F')
		res -= 55;
	else if (res <= 'f')
		res -= 87;

	return res;
}

char TimerDisabled = TRUE;
int EnterProtection() {
	if (!TimerDisabled)
	{
		XScuGic_Disable(&IntcInstance, TIMER_IRPT_INTR);
		TimerDisabled = TRUE;
		return TRUE;
	}
	else
		return FALSE;
}

int LeaveProtection(int State) {
	if (State)
	{
		XScuGic_Enable(&IntcInstance, TIMER_IRPT_INTR);
		TimerDisabled = FALSE;
	}
	return State;
}

	extern void outbyte(char c);
	int main() {
		u32 HandleCommandState = 0;
		u32 counter;
		u32 ec_to_dev, ec_from_dev; //, failure;
		int Status;

		init_platform();

		bsp_init();

		InitPECB_IO();
		InitLog();

		/*
		 * Setup the interrupts such that interrupt processing can occur.
		 * If an error occurs then exit
		 */
//ISR
		print("Setting up the interrupts... ");
		Status = SetupInterruptSystem(&IntcInstance);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;
		print("Success!\n\r");

//TIMER
		print("Setting up the timer...");
		Status = SetupTimer(&IntcInstance, &TimerInstance, TIMER_DEVICE_ID,
				TIMER_IRPT_INTR);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;
		print("Success!\n\r");
		TimerDisabled = FALSE;

////GPIO
//		print("Setting up the GPIO...");
//		Status = XGpio_Initialize(&Gpio, GPIO_EXAMPLE_DEVICE_ID);
//		if (Status != XST_SUCCESS)
//			return XST_FAILURE;
//		print("Success!\n\r");

////GPIO ISR
//		print("Setting up the GPIO Interrupt...");
//		Status = SetupGPIO_Interrupt(&IntcInstance);
//		if (Status != XST_SUCCESS)
//			return XST_FAILURE;
//		print("Success!\n\r");

//CONTROL
		print("Setting up the controllers...");
		Status = InitControllers(&IntcInstance);
		if (Status != XST_SUCCESS)
			return XST_FAILURE;
		print("Success!\n\r");

//CONTROL app
		/*print("Initializing the control application...");
			Status = InitControlApp();
			if (Status != XST_SUCCESS)
				return XST_FAILURE;
			print("Success!\n\r");*/

//CLEAR PECB error count
		print("Clearing PECB error count...");
		PECB[CONTROL] = 1;
		for (counter = 0; counter < 0xFFFF; counter++)
			;
		PECB[CONTROL] = 0;

		ec_to_dev = PECB[ER_CNT] & 0xFFFF;
		ec_from_dev = (PECB[ER_CNT] >> 16) & 0xFFFF;
		if (ec_to_dev + ec_from_dev == 0)
			print("Success!\r\n");
		else
			printf("Uh oh... found %d errors!\r\n",
					(unsigned int) (ec_to_dev + ec_from_dev));

//PECB self-test
#if 0
		print("PECB self test...");
		for (counter = 0; counter < 12; counter++)
		PECB[WR_REG0+counter] = 0x03 + counter;

		for (counter = 0; counter < 0xFFFF; counter++)
		;

		failure = 0;

		for (counter = 0; counter < 12; counter++)
		if (PECB[RD_REG0+counter] != 0x03 + counter)
		{
			failure = 1;
			printf("\r\nPECB Self-Test Failure: Register %X reads %08X, expected %08X", (unsigned int)counter, (unsigned int)PECB[RD_REG0+counter], (unsigned int) (0x03+counter));
		}

		if (failure == 0)
		printf("Success!\r\n");
		else
		printf("\r\n");
#endif
		print("Setting up ethernet and lwip...");
		if (SetupLWIP() == XST_SUCCESS)
			printf("Success!\r\n");
		else
			printf("Failure\r\n");

		print("Setting up listening sockets...");
		if (start_tcpip() == XST_SUCCESS)
			printf("Success!\r\n");
		else
			printf("Failure\r\n");

		//so we have two options to address the IO -- either directly with its address (0x41200000)
		//or via XGpio functions.

		//Option 1:
		//print("Writing to LEDs directly: 0x24\n\r");

		//*LEDs = 0x24;

		print("Completing the initialization of the PECB interface...");
		FinishInitPECB_IO();

		print("Enter a command followed by CR\n\r");
		while (1) {
			xemacif_input(echo_netif);

			HandleCommandState = HandleCommandsUart(szRcvCmdUart,
					HandleCommandState);

			if (bTimer100usTick)
				HandleTimer100usTick();

			if (bPrimeStreamFIFO)
			{
				if (ReloadStreamFIFO(StreamFIFOComm) > 0)
					bPrimeStreamFIFO = FALSE;
			}

			if (bEth250ms)
			{
				bEth250ms = FALSE;
				lwip_timer_callback();
			}
		}

		return 0;
	}

	void HandleTimer100usTick() {
		static u32 countTo10ms;

		//Clear this immediately so we can make sure the function doesn't take more than 100us
		bTimer100usTick = 0;

		//code every 10ms
		if (++countTo10ms) {
			countTo10ms = 0;
//			UpdateLEDs();
		}

		HBA_100usTick();
		VSI_100usTick();

		if (bTimer100usTick)
			Timing100usErrorCount++;
	}

//	void UpdateLEDs()
//	{
//		u32 ec_to_dev, ec_from_dev;
//		ec_to_dev = PECB[ER_CNT] & 0xFFFF;
//		ec_from_dev = (PECB[ER_CNT] >> 16) & 0xFFFF;
//		WriteToZB_LEDs(ec_to_dev + ec_from_dev);
//	}




//	void GpioIsr(void *InstancePtr) {
//		u32 Buttons;
//
//		XGpio *GpioPtr = (XGpio *) InstancePtr;
//
//		// Disable the interrupt
//		XGpio_InterruptDisable(GpioPtr, BUTTON_INTERRUPT);
//
//		// There should not be any other interrupts occuring other than the the button changes
//		if ((XGpio_InterruptGetStatus(GpioPtr) & BUTTON_INTERRUPT)
//				!= BUTTON_INTERRUPT) {
//			return;
//		}
//
//		// Read state of push buttons and determine which ones changed
//		// states from the previous interrupt. Save a copy of the buttons
//		// for the next interrupt
//
//		Buttons = XGpio_DiscreteRead(&Gpio, BUTTON_CHANNEL);
//
//		if (Buttons == 1 && (count < 0xFF)) {
//			u32 counter;
//			count++;
//			PECB[CONTROL] = 1;
//			fprintf(stdout, "Clearing the error count\n");
//			for (counter = 0; counter < 0xFFFF; counter++)
//				;
//			PECB[CONTROL] = 0;
//			fprintf(stdout, "Error count cleared, now reads: %d\n",
//					(int) PECB[ER_CNT]);
//		}
//
//		if (Buttons == 2 && (count < 0xFF)) {
//			count++;
//		}
//
//		if (Buttons == 4 && (count >= 1)) {
//			count--;
//		}
//
//		if (Buttons == 8 && (count >= 1)) {
//			count--;
//		}
//
//		if (Buttons == 16) {
//			count = 0;
//		}
//
//		//fprintf(stdout,"Count : %X\n",(int)count);
//
//		//XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, count);
//
//		// Clear the interrupt such that it is no longer pending in the GPIO
//		(void) XGpio_InterruptClear(GpioPtr, BUTTON_INTERRUPT);
//
//		// Enable the interrupt
//		XGpio_InterruptEnable(GpioPtr, BUTTON_INTERRUPT);
//
//	}

	/*******************************************************************************/
	/*              S E T U P   I N T E R R U P T   S Y S T E M                    */
	/*******************************************************************************/
	int SetupInterruptSystem(XScuGic *IntcInstancePtr) {
		int Result;

		XScuGic_Config *IntcConfig;

		// Initialize the interrupt controller driver so that it is ready to use.

		IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);//ELS -- this is misleading! there is only one XScuGic device (0)
		if (NULL == IntcConfig) {
			return XST_FAILURE;
		}

		Result = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
				IntcConfig->CpuBaseAddress);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		// Initialize the exception table and register the interrupt
		// controller handler with the exception table

		Xil_ExceptionInit();

		Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler) INTC_HANDLER, IntcInstancePtr);

		// Enable non-critical exceptions

		Xil_ExceptionEnable();

		return XST_SUCCESS;
	}

//	int SetupGPIO_Interrupt(XScuGic *IntcInstancePtr) {
//		int Result;
//		XScuGic_SetPriorityTriggerType(IntcInstancePtr, INTC_GPIO_INTERRUPT_ID,
//				0xA0, 0x3);
//
//		// Connect the interrupt handler that will be called when an
//		// interrupt occurs for the device.
//
//		Result = XScuGic_Connect(IntcInstancePtr, INTC_GPIO_INTERRUPT_ID,
//				(Xil_ExceptionHandler) GpioIsr, &Gpio);
//		if (Result != XST_SUCCESS) {
//			return Result;
//		}
//
//		// Enable the interrupt for the GPIO device.
//
//		XScuGic_Enable(IntcInstancePtr, INTC_GPIO_INTERRUPT_ID);
//
//		// Enable the GPIO channel interrupts so that push button can be
//		// detected and enable interrupts for the GPIO device
//
//		XGpio_InterruptEnable(&Gpio, BUTTON_INTERRUPT);
//		XGpio_InterruptGlobalEnable(&Gpio);
//
//		return XST_SUCCESS;
//	}

