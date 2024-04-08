#include "drv/timer.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xscutimer.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define TIMER_DEVICE_ID XPAR_XSCUTIMER_0_DEVICE_ID
#define INTC_DEVICE_ID  XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR XPAR_SCUTIMER_INTR

// Per ARM Cortex-A9 MPCore Technical Reference Manual, Section 4.1.1 (page 63)
//
// PERIOD = ((PRESCALER_VALUE + 1) * (LOAD_VALUE + 1)) / CLK_FREQ
//
// Rearranging leads to:
// LOAD_VALUE = ((PERIOD * CLK_FREQ) / (PRESCALER_VALUE + 1)) - 1
//
// or, in practical units:
// LOAD_VALUE = ((PERIOD_USEC * CLK_FREQ_MHZ) / (PRESCALER_VALUE + 1)) - 1
//
// In the Xilinx Zynq-7000:
//   CLK_FREQ = 666.666/2 = 333.333 MHz
//   PRESCALER_VALUE = 0 (???)
//
// Thus:
// LOAD_VALUE = (PERIOD_USEC * CLK_FREQ_MHZ) - 1
//
#define TIMER_LOAD_VALUE(period_usec) ((period_usec * 333.333) - 1)

// Cortex A9 Scu Private Timer Instance
static XScuTimer TimerInstance;

// Interrupt Controller Instance
static XScuGic IntcInstance;

void fatalError(char *str)
{
    printf("ERROR: %s\n", str);
    while (1) {
    }
}

static int
SetupIntrSystem(timer_handler_t timer_isr, XScuGic *IntcInstancePtr, XScuTimer *TimerInstancePtr, u16 TimerIntrId);

void timer_init(timer_handler_t timer_isr, uint32_t timer_period_usec)
{
    int Status;
    XScuTimer_Config *ConfigPtr;

    // Initialize the Scu Private Timer driver
    ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);

    // This is where the virtual address would be used, this example uses physical address
    Status = XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr, ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) {
        fatalError("Error 1");
        return;
    }

    // Perform a self-test to ensure that the hardware was built correctly
    Status = XScuTimer_SelfTest(&TimerInstance);
    if (Status != XST_SUCCESS) {
        fatalError("Error 2");
        return;
    }

    // Connect the device to interrupt subsystem so that interrupts can occur
    Status = SetupIntrSystem(timer_isr, &IntcInstance, &TimerInstance, TIMER_IRPT_INTR);
    if (Status != XST_SUCCESS) {
        fatalError("Error 3");
        return;
    }

    // Enable Auto reload mode
    XScuTimer_EnableAutoReload(&TimerInstance);

    // Load the timer counter register
    XScuTimer_LoadTimer(&TimerInstance, TIMER_LOAD_VALUE(timer_period_usec));

    // Start the timer counter
    XScuTimer_Start(&TimerInstance);
}

static int
SetupIntrSystem(timer_handler_t timer_isr, XScuGic *IntcInstancePtr, XScuTimer *TimerInstancePtr, u16 TimerIntrId)
{
    int Status;

    XScuGic_Config *IntcConfig;

    /*
     * Initialize the interrupt controller driver so that it is ready to
     * use.
     */
    IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (NULL == IntcConfig) {
        return XST_FAILURE;
    }

    Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    Xil_ExceptionInit();

    /*
     * Connect the interrupt controller interrupt handler to the hardware
     * interrupt handling logic in the processor.
     */
    Xil_ExceptionRegisterHandler(
        XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, IntcInstancePtr);

    /*
     * Connect the device driver handler that will be called when an
     * interrupt for the device occurs, the handler defined above performs
     * the specific interrupt processing for the device.
     */
    Status = XScuGic_Connect(IntcInstancePtr, TimerIntrId, (Xil_ExceptionHandler) timer_isr, (void *) TimerInstancePtr);
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

    /*
     * Enable interrupts in the Processor.
     */
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

void timer_clear_interrupt(void)
{
    XScuTimer_ClearInterruptStatus(&TimerInstance);
}

bool timer_is_expired(void)
{
    return XScuTimer_IsExpired(&TimerInstance);
}
