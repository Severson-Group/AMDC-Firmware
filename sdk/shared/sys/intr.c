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
    // CPU0 handles initialization of the Generic Interrrupt Controller
    xil_printf("INTR: Initializing GIC...\n");
    XScuGic_Config *gic_config_ptr = XScuGic_LookupConfig(INTR_GIC_DEVICE_ID);

    // gic_config_ptr provides the Xilinx base addresses of:
    //   - the GIC's distributor registers (the distributor is a shared resource that distributes interrupts to the
    //   CPUs)
    //   - the "CPU Interface" registers, each CPU has an interface that needs to be configured to interact with the GIC
    s32 gic_init_status = XScuGic_CfgInitialize(INTR_GIC_INSTANCE_ADDR, gic_config_ptr, gic_config_ptr->CpuBaseAddress);
    if (gic_init_status != XST_SUCCESS) {
        xil_printf("INTR: GIC Initialization Failed\n");
        return XST_FAILURE;
    }
    xil_printf("INTR: GIC Initialization Success\n");
    INTR_setGicInitReady;
#elif XPAR_CPU_ID == 1
    // Wait for CPU0 to finish GIC initialization
    while (!INTR_getGicInitReady)
        ;

    // CPU1 still needs to configure its own CPU Interface, using these lines pulled from CPUInitialize() in xscugic.c
    //   CPU0 does this by calling CPUInitialize() within XScuGic_CfgInitialize() (xscugic.c, Line 481), but because of
    //   the GIC ready check on Line 439 of xscugic.c, if CPU1 calls  XScuGic_CfgInitialize(), it will skip the call to
    //   CPUInitialize() if CPU0 has already marked the GIC as ready.
    XScuGic_CPUWriteReg(INTR_GIC_INSTANCE_ADDR, XSCUGIC_CPU_PRIOR_OFFSET, 0xF0U);
    XScuGic_CPUWriteReg(INTR_GIC_INSTANCE_ADDR, XSCUGIC_CONTROL_OFFSET, 0x07U);
#endif

    // BOTH CORES: Connect the ARM processor's InterruptHandler logic to the initialized GIC
    Xil_ExceptionRegisterHandler(
        XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, INTR_GIC_INSTANCE_ADDR);
    Xil_ExceptionEnable();

#if XPAR_CPU_ID == 0
    // Each CPU needs to connect its software interrupts to the appropriate handlers defined below, then enable
    XScuGic_Connect(INTR_GIC_INSTANCE_ADDR,
                    INTR_UNBLOCK_CPU0_RX_INT_ID,
                    (Xil_ExceptionHandler) CPU0UnblockRxHandler,
                    (void *) INTR_GIC_INSTANCE_ADDR);
    XScuGic_Enable(INTR_GIC_INSTANCE_ADDR, INTR_UNBLOCK_CPU0_RX_INT_ID);

    XScuGic_Connect(INTR_GIC_INSTANCE_ADDR,
                    INTR_UNBLOCK_CPU0_TX_INT_ID,
                    (Xil_ExceptionHandler) CPU0UnblockTxHandler,
                    (void *) INTR_GIC_INSTANCE_ADDR);
    XScuGic_Enable(INTR_GIC_INSTANCE_ADDR, INTR_UNBLOCK_CPU0_TX_INT_ID);

#elif XPAR_CPU_ID == 1
    XScuGic_Connect(INTR_GIC_INSTANCE_ADDR,
                    INTR_UNBLOCK_CPU1_RX_INT_ID,
                    (Xil_ExceptionHandler) CPU1UnblockRxHandler,
                    (void *) INTR_GIC_INSTANCE_ADDR);
    XScuGic_Enable(INTR_GIC_INSTANCE_ADDR, INTR_UNBLOCK_CPU1_RX_INT_ID);

    XScuGic_Connect(INTR_GIC_INSTANCE_ADDR,
                    INTR_UNBLOCK_CPU1_TX_INT_ID,
                    (Xil_ExceptionHandler) CPU1UnblockTxHandler,
                    (void *) INTR_GIC_INSTANCE_ADDR);
    XScuGic_Enable(INTR_GIC_INSTANCE_ADDR, INTR_UNBLOCK_CPU1_TX_INT_ID);

#endif

    return XST_SUCCESS;
}

/* We only need to define the handlers in the appropriate core
 */
#if XPAR_CPU_ID == 0
void CPU0UnblockRxHandler()
{
    xil_printf("M\r\n");

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xMessageBufferSendCompletedFromISR(xCPU1to0MessageBufferHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void CPU0UnblockTxHandler()
{
    xil_printf("N\r\n");

    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xMessageBufferReceiveCompletedFromISR(xCPU0to1MessageBufferHandle, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#elif XPAR_CPU_ID == 1
void CPU1UnblockRxHandler()
{
    xil_printf("m\r\n");

    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xMessageBufferSendCompletedFromISR(xCPU0to1MessageBufferHandle, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void CPU1UnblockTxHandler()
{
    xil_printf("n\r\n");

    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xMessageBufferReceiveCompletedFromISR(xCPU1to0MessageBufferHandle, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#endif
