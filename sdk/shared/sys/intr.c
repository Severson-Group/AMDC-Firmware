#include "intr.h"

///////////////////////////////////////////////////////
// THIS IS A SHARED FILE, SO IT IS ALWAYS
// IN SYNC IN BOTH CPU0 AND CPU1
//
// If you need to differentiate something between
// CPUs, use "#if XPAR_CPU_ID == ?"
///////////////////////////////////////////////////////

/// GENERAL INFO
/*  The functions in this file are responsible for setting up the
 *  Xilinx Generic Interrupt Controller (GIC).
 *
 *  Interrupts are needed for Inter-Core Communication, specifically
 *    the ability to trigger an interrupt in the receiving CPU after a
 *    message is placed into an empty ICC Message Buffer,
 *  OR
 *    the ability to trigger an interrupt in the sending CPU after a
 *    message is removed from a full ICC Message Buffer
 *
 *  See sys/icc.c for more info.
 * 
// RELEVENT TRM SECTIONS
 * Read Chapter 7: Interrupts in the Zynq-7000 TRM
 *   It starts on page 231 of the PDF
 * Appendix A contains all the memory-mapped register details
 *   Relevant subsection is the "Application Processing Unit (mpcore)" section
 *     that starts on page 1483.
 *   Registers starting with ICD are the ones we care about
 *     These are the "distributor" registers starting with ICDDCR at absolute address 0xF8F01000
 * 
// USEFUL XILINX-PROVIDED FILES TO READ
 * - xparameters.h   : Search for "SCUGIC" related definitions
 *   - XPAR_PS7_SCUGIC_0_DEVICE_ID     0U
 *   - XPAR_PS7_SCUGIC_0_BASEADDR      0xF8F00100U
 *   - XPAR_PS7_SCUGIC_0_HIGHADDR      0xF8F001FFU
 *   - XPAR_PS7_SCUGIC_0_DIST_BASEADDR 0xF8F01000U 
 * - xscugic_sinit.c : Contains the necessary LookupConfig() function
 * - xscugic.c       : Contains most of the useful GIC functions
 *   - XScuGic_CfgInitialize()
 *       Uses the ConfigPtr* from lookup to initialize a GIC Instance struct
 *   - XScuGic_DistWriteReg()
 *       Write a GIC register
 *   - XScuGic_DistReadReg()
 *       Read a GIC register
 *   - XScuGic_Connect()
 *   - XScuGic_Disconnect()
 *   - XScuGic_Enable()
 *   - XScuGic_Disable()
 *   - XScuGic_SoftwareIntr()
 *   - XScuGic_GetPriorityTriggerType()
 *   - XScuGic_SetPriorityTriggerType()
 *   - XScuGic_InterruptMaptoCpu()
 *   - XScuGic_InterruptUnmapFromCpu()
 *   - XScuGic_UnmapAllInterruptsFromCpu()
 *   - XScuGic_Stop()
 *   - XScuGic_SetCpuID()
 *   - XScuGic_GetCpuID()
 */

int intr_init()
{
#if XPAR_CPU_ID == 0
    // CPU0 HANDLES INITIALIZING EVERYTHING INTERRUPT-RELATED

    // Initialize the GIC Here
    xil_printf("GIC: Initializing...\n");
    XScuGic_Config * gic_config_ptr = XScuGic_LookupConfig(INTR_GIC_DEVICE_ID);
    s32 gic_init_status = XScuGic_CfgInitialize(&INTR_GIC_INSTANCE, gic_config_ptr, gic_config_ptr->CpuBaseAddress);

    if (gic_init_status != XST_SUCCESS) {
		xil_printf("GIC: Initialization Failed\n");
		while(1);
	}
	xil_printf("GIC: Initialization Success\n");




    // // Initialize the interrupt controller
    // Xil_ExceptionRegisterHandler(
    //     XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, &InterruptController);
    // Xil_ExceptionEnable();


    // Connect the given interrupt with its handler
    // XScuGic_Connect(&InterruptController, INTC_1TO0_SEND_INTERRUPT_ID, (Xil_ExceptionHandler) CPU0WakeRxHandler, NULL);
    // XScuGic_Connect(&InterruptController, INTC_0TO1_RCVE_INTERRUPT_ID, (Xil_ExceptionHandler) CPU0WakeTxHandler, NULL);

#elif XPAR_CPU_ID == 1
    // Connect the given interrupt with its handler
    // XScuGic_Connect(&InterruptController, INTC_0TO1_SEND_INTERRUPT_ID, (Xil_ExceptionHandler) CPU1WakeRxHandler, NULL);
    // XScuGic_Connect(&InterruptController, INTC_1TO0_RCVE_INTERRUPT_ID, (Xil_ExceptionHandler) CPU1WakeTxHandler, NULL);
#endif

    return XST_SUCCESS;
}

/* We only need to define the handlers in the appropriate core
 */
#if XPAR_CPU_ID == 0
void CPU0WakeTxHandler()
{
    // xil_printf("CPU 0 - WakeTxHandler reached\r\n");

    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xMessageBufferReceiveCompletedFromISR(xCPU0to1MessageBufferHandle, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void CPU0WakeRxHandler()
{
    // xil_printf("CPU 0 - WakeRxHandler reached\r\n");

    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xMessageBufferSendCompletedFromISR(xCPU1to0MessageBufferHandle, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#elif XPAR_CPU_ID == 1
void CPU1WakeTxHandler()
{
    // xil_printf("CPU 1 - WakeTxHandler reached\r\n");

    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xMessageBufferReceiveCompletedFromISR(xCPU1to0MessageBufferHandle, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void CPU1WakeRxHandler()
{
    // xil_printf("CPU 1 - WakeRxHandler reached\r\n");

    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xMessageBufferSendCompletedFromISR(xCPU0to1MessageBufferHandle, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#endif
