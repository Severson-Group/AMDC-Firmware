#include "intr.h"

///////////////////////////////////////////////////////
// THIS IS A SHARED FILE, SO IT IS ALWAYS
// IN SYNC IN BOTH CPU0 AND CPU1
//
// If you need to differentiate something between
// CPUs, use "#if XPAR_CPU_ID == ?"
///////////////////////////////////////////////////////

/*  The functions in this file are responsible for setting up the
 *  Xilinx Generic Interrupt Controller (GIC).
 *
 *  Interrupts are needed for Inter-Core Communication, specifically
 *  the ability to trigger an interrupt in the receiving CPU after a
 *  message is placed into an empty ICC Message Buffer,
 *  OR
 *  the ability to trigger an interrupt in the sending CPU after a
 *  message is removed from a full ICC Message Buffer
 *
 *  See sys/icc.c for more info.
 */

int intr_init()
{
    int Status = XST_FAILURE;

    XScuGic_Config *IntcConfig;

    IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    XScuGic_CfgInitialize(&InterruptController, IntcConfig, IntcConfig->CpuBaseAddress);

    /*
     * Perform a self-test to ensure that the hardware was built
     * correctly
     */
    Status = XScuGic_SelfTest(&InterruptController);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // Initialize the interrupt controller
    Xil_ExceptionRegisterHandler(
        XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, &InterruptController);
    Xil_ExceptionEnable();

#if XPAR_CPU_ID == 0
    // Connect the given interrupt with its handler
    XScuGic_Connect(&InterruptController, INTC_1TO0_SEND_INTERRUPT_ID, (Xil_ExceptionHandler) CPU0WakeRxHandler, NULL);
    XScuGic_Connect(&InterruptController, INTC_0TO1_RCVE_INTERRUPT_ID, (Xil_ExceptionHandler) CPU0WakeTxHandler, NULL);

#elif XPAR_CPU_ID == 1
    // Connect the given interrupt with its handler
    XScuGic_Connect(&InterruptController, INTC_0TO1_SEND_INTERRUPT_ID, (Xil_ExceptionHandler) CPU1WakeRxHandler, NULL);
    XScuGic_Connect(&InterruptController, INTC_1TO0_RCVE_INTERRUPT_ID, (Xil_ExceptionHandler) CPU1WakeTxHandler, NULL);
#endif

    /*
    // Enable the interrupt for the second CPU core
    XScuGic_SetPriorityTriggerType(&InterruptController, INTC_INTERRUPT_ID, 0xA0, 3); // Set priority and trigger type
    XScuGic_Enable(&InterruptController, INTC_INTERRUPT_ID);

    // Enable the interrupt for CPU1
    Xil_Out32(XPAR_PS7_SCUGIC_DIST_BASEADDR + XSCUGIC_CPU_PRIOR_OFFSET + (CPU1_ID * 4), 0xFF);
    Xil_Out32(XPAR_PS7_SCUGIC_DIST_BASEADDR + XSCUGIC_CPU_TARGET_OFFSET + (CPU1_ID * 4), 0x1);

    // Enable interrupts globally
    Xil_ExceptionEnableMask(XIL_EXCEPTION_NON_CRITICAL);

    print("Interrupt system setup complete.\r\n");
    */

    return XST_SUCCESS;
}

/* We only need to define the handlers in the appropriate core
 */
#if XPAR_CPU_ID == 0
void CPU0WakeTxHandler()
{
    xil_printf("CPU 0 - WakeTxHandler reached\r\n");

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xMessageBufferReceiveCompletedFromISR(xCPU0to1MessageBufferHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void CPU0WakeRxHandler()
{
    xil_printf("CPU 0 - WakeRxHandler reached\r\n");

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xMessageBufferSendCompletedFromISR(xCPU1to0MessageBufferHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#elif XPAR_CPU_ID == 1
void CPU1WakeTxHandler()
{
    xil_printf("CPU 1 - WakeTxHandler reached\r\n");

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xMessageBufferReceiveCompletedFromISR(xCPU1to0MessageBufferHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void CPU1WakeRxHandler()
{
    xil_printf("CPU 1 - WakeRxHandler reached\r\n");

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xMessageBufferSendCompletedFromISR(xCPU0to1MessageBufferHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#endif
