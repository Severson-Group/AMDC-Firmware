#include "icc.h"

///////////////////////////////////////////////////////
// THIS IS A SHARED FILE, SO IT IS ALWAYS
// IN SYNC IN BOTH CPU0 AND CPU1
//
// If you need to differentiate something between
// CPUs, use "#if XPAR_CPU_ID == ?"
///////////////////////////////////////////////////////

void icc_init(uint32_t cpu_num)
{
#if XPAR_CPU_ID == 0
	// ONLY CPU 0 INITIALIZES THE MESSAGE BUFFERS

    /* Create two message buffers for inter-core communication that use the callback
     * functions below as send and receive completed callback functions. */
    xCPU0to1MessageBuffer = xMessageBufferCreateStaticWithCallback(ICC_BUFFER_SIZE - 1,
                                                                   ICC_CPU0to1_BufferSpaceAddr,
																   ICC_CPU0to1_BufferStructAddr,
                                                                   vCPU0to1SendCallback,
                                                                   vCPU0to1ReceiveCallback);

    xCPU1to0MessageBuffer = xMessageBufferCreateStaticWithCallback(ICC_BUFFER_SIZE - 1,
                                                                   ICC_CPU1to0_BufferSpaceAddr,
																   ICC_CPU1to0_BufferStructAddr,
                                                                   vCPU1to0SendCallback,
                                                                   vCPU1to0ReceiveCallback);
#endif
}

/* From FreeRTOS:
 * Insert code into callback which is invoked when a message is written to the message buffer.
 * This is useful when a message buffer is used to pass messages between
 * cores on a multicore processor. In that scenario, this callback
 * can be implemented to generate an interrupt in the other CPU core,
 * and the interrupt's service routine can then use the
 * xMessageBufferSendCompletedFromISR() API function to check, and if
 * necessary unblock, a task that was waiting for message. */

/* !! IMPORTANT !!
 * These callback functions must ALL exist in BOTH CPUs for the above Message Buffer creations
 * to work. HOWEVER, the callbacks only have to DO SOMETHING in the relevant CPU
 * For example, the behavior of the 0 to 1 Send Callback must be implemented in CPU 0, since
 * CPU 0 needs to send an interrupt to CPU 1 when it sends to the buffer. But CPU 1 will never
 * send to the 0 to 1 buffer, so in CPU 1 this callback doesn't need to DO ANYTHING except exist.
 * - Patrick */

void vCPU0to1SendCallback(MessageBufferHandle_t xMessageBuffer,
                          BaseType_t xIsInsideISR,
                          BaseType_t *const pxHigherPriorityTaskWoken)
{
#if XPAR_CPU_ID == 0
	xil_printf("DEBUG: CPU 0 to 1 Send Callback reached\r\n");
    // In CPU 0, this callback should send an interrupt to CPU 1's Rx task
    XScuGic_SoftwareIntr(&InterruptController, INTC_0TO1_SEND_INTERRUPT_ID, CPU1_ID);
#endif
}

void vCPU1to0ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken)
{
#if XPAR_CPU_ID == 0
	xil_printf("DEBUG: CPU 1 to 0 Receive Callback reached\r\n");
    // In CPU 0, this callback should send an interrupt to CPU 1's Tx task
    XScuGic_SoftwareIntr(&InterruptController, INTC_1TO0_RCVE_INTERRUPT_ID, CPU1_ID);
#endif
}

void vCPU1to0SendCallback(MessageBufferHandle_t xMessageBuffer,
                          BaseType_t xIsInsideISR,
                          BaseType_t *const pxHigherPriorityTaskWoken)
{
#if XPAR_CPU_ID == 1
	xil_printf("DEBUG: CPU 1 to 0 Send Callback reached\r\n");
    // In CPU 1, this callback should send an interrupt to CPU 0's Rx task
    XScuGic_SoftwareIntr(&InterruptController, INTC_1TO0_SEND_INTERRUPT_ID, CPU0_ID);
#endif
}

void vCPU0to1ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken)
{
#if XPAR_CPU_ID == 1
	xil_printf("DEBUG: CPU 0 to 1 Receive Callback reached\r\n");
    // In CPU 1, this callback should send an interrupt to CPU 0's Tx task
    XScuGic_SoftwareIntr(&InterruptController, INTC_0TO1_RCVE_INTERRUPT_ID, CPU0_ID);
#endif
}
