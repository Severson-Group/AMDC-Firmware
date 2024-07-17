#include "timer.h"

#include "xscugic.h"
#include "xtmrctr.h"
#include "xparameters.h"

#include <stdio.h>
#include "xil_printf.h"

// Parameter definitions
#define TMR_DEVICE_ID 					XPAR_CONTROL_TIMER_0_DEVICE_ID
#define INTC_TMR_INTERRUPT_ID 			XPAR_FABRIC_CONTROL_TIMER_0_INTERRUPT_INTR
#define TIMER_LOAD_VALUE(period_usec)	((0xFFFFFFFF) - (((period_usec) * 200) - 2))

// PERIOD = ((2^32-1) – (TMR_LOAD_VALUE) + 2) * 5e-9

static XScuGic intCtrl;
static XTmrCtr timer;

//// This handler is called every 20 ms
//void timerInterruptHandler(void *userParam, u8 TmrCtrNumber) {
//	print("INTR!!!\n\r");
//}

void fatalError(char *str)
{
	printf("ERROR: %s\n", str);
	while (1) {}
}

void timer_init(XTmrCtr_Handler timer_isr, uint32_t timer_period_usec)
{
	int xStatus = XTmrCtr_Initialize(&timer, TMR_DEVICE_ID);
    if (xStatus != XST_SUCCESS) {
        fatalError("Could not initialize timer");
    }
    XTmrCtr_SetHandler(&timer, timer_isr, (void*) 0x12345678);

    // Initialize the interrupt controller driver so that it is ready to use.
    XScuGic_Config *IntcConfig = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
    if (NULL == IntcConfig) {
        fatalError("XScuGic_LookupConfig() failed");
    }

    int Status = XScuGic_CfgInitialize(&intCtrl, IntcConfig,IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        fatalError("XScuGic_CfgInitialize() failed");
    }

    XScuGic_SetPriorityTriggerType(&intCtrl, INTC_TMR_INTERRUPT_ID, 0xA0, 0x3);
    Status = XScuGic_Connect(&intCtrl, INTC_TMR_INTERRUPT_ID,(Xil_InterruptHandler)XTmrCtr_InterruptHandler,&timer);
    if (Status != XST_SUCCESS) {
        fatalError("XScuGic_Connect() failed");
    }
    XScuGic_Enable(&intCtrl, INTC_TMR_INTERRUPT_ID);

    // Initialize the exception table.
    Xil_ExceptionInit();

    // Register the interrupt controller handler with the exception table.
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,(void *)&intCtrl);

    // Enable exceptions.
    Xil_ExceptionEnable();

    XTmrCtr_SetOptions(&timer, 0,   XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
    XTmrCtr_SetResetValue(&timer, 0, TIMER_LOAD_VALUE(timer_period_usec));
    XTmrCtr_Start(&timer, 0);
}
