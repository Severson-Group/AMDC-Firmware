#include "icc.h"

///////////////////////////////////////////////////////
// THIS IS A SHARED FILE, SO IT IS ALWAYS
// IN SYNC IN BOTH CPU0 AND CPU1
//
// If you need to differentiate something between
// CPUs, use "#if XPAR_CPU_ID == ?"
///////////////////////////////////////////////////////

void icc_init()
{
#if XPAR_CPU_ID == 0
    // CPU0 HANDLES INITIALIZING THE MESSAGE BUFFERS

    // Wait for CPU1 to provide the function pointers to its callbacks ()
    while (!ICC_getFunctionPointersReady)
        ;

    // Use the getters once ready
    void (*vCPU0to1ReceiveCallback)() = ICC_getCPU0to1ReceiveCallback;
    // xil_printf("DEBUG: CPU 0 got 0to1 Receive Callback %p\r\n", (void *) vCPU0to1ReceiveCallback);
    void (*vCPU1to0SendCallback)() = ICC_getCPU1to0SendCallback;
    // xil_printf("DEBUG: CPU 0 got 1to0 Send Callback %p\r\n", (void *) vCPU1to0SendCallback);

    /* Create two message buffers for inter-core communication that use the callback
     * functions below as send and receive completed callback functions. */
    xCPU0to1MessageBufferHandle = xMessageBufferCreateStaticWithCallback(
        ICC_BUFFER_SIZE - 1,
        ICC_CPU0to1BufferSpaceAddr,
        ICC_CPU0to1BufferStructAddr,
        vCPU0to1SendCallback,     // Called by CPU0 after placing message in 0to1 buffer
        vCPU0to1ReceiveCallback); // Called by CPU1 after removing message from 0to1 buffer

    xCPU1to0MessageBufferHandle = xMessageBufferCreateStaticWithCallback(
        ICC_BUFFER_SIZE - 1,
        ICC_CPU1to0BufferSpaceAddr,
        ICC_CPU1to0BufferStructAddr,
        vCPU1to0SendCallback,     // Called by CPU1 after placing message in 1to0 buffer
        vCPU1to0ReceiveCallback); // Called by CPU0 after removing message from 1to0 buffer

    ICC_setCPU0to1Handle(xCPU0to1MessageBufferHandle);
    // xil_printf("DEBUG: CPU 0 set 0to1 Handle %p\r\n", (void *) xCPU0to1MessageBufferHandle);
    ICC_setCPU1to0Handle(xCPU1to0MessageBufferHandle);
    // xil_printf("DEBUG: CPU 0 set 1to0 Handle %p\r\n", (void *) xCPU1to0MessageBufferHandle);

    ICC_setHandleComplete;
#elif XPAR_CPU_ID == 1
    /* need to stall 10ms to "guarantee" that CPU1 does not get before CPU0 sets
     * The APU freq is 666,666,687 Hz (per ps7_init.h), so a single NOP is 1/(666,666,687) or 1.5ns
     * Therefore we need 6.66E6 NOPs to stall 10ms
     */

    // Make CPU1's callback function pointers available to CPU0
    ICC_setCPU1to0SendCallback(&vCPU1to0SendCallback);
    // xil_printf("DEBUG: CPU 1 set 1to0 Send Callback %p\r\n", &vCPU1to0SendCallback);
    ICC_setCPU0to1ReceiveCallback(&vCPU0to1ReceiveCallback);
    // xil_printf("DEBUG: CPU 1 set 0to1 Receive Callback %p\r\n", &vCPU0to1ReceiveCallback);

    ICC_setFunctionPointersReady;

    // Wait for CPU0 to finish creating buffers and providing the handles
    while (!ICC_getHandleComplete)
        ;

    xCPU0to1MessageBufferHandle = ICC_getCPU0to1Handle;
    // xil_printf("DEBUG: CPU 1 got 0to1 Handle %p\r\n", (void *) xCPU0to1MessageBufferHandle);
    xCPU1to0MessageBufferHandle = ICC_getCPU1to0Handle;
    // xil_printf("DEBUG: CPU 1 got 1to0 Handle %p\r\n", (void *) xCPU1to0MessageBufferHandle);
#endif
}

/* From FreeRTOS:
 * Insert code into callback which is invoked when a message is written to the message buffer.
 * This is useful when a message buffer is used to pass messages between
 * cores on a multi-core processor. In that scenario, this callback
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

#if XPAR_CPU_ID == 0

void vCPU0to1SendCallback(MessageBufferHandle_t xMessageBuffer,
                          BaseType_t xIsInsideISR,
                          BaseType_t *const pxHigherPriorityTaskWoken)
{
    // xil_printf("DEBUG: CPU 0 to 1 Send Callback reached (in CPU0)\r\n");
    xil_printf("K\r\n");

    // CPU 0 should send an interrupt to CPU1 to unblock its Rx task
    int status = XScuGic_SoftwareIntr(INTR_GIC_INSTANCE_ADDR, INTR_UNBLOCK_CPU1_RX_INT_ID, XSCUGIC_SPI_CPU1_MASK);
}

void vCPU1to0ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken)
{
    // xil_printf("DEBUG: CPU 1 to 0 Receive Callback reached (in CPU0)\r\n");
    xil_printf("L\r\n");

    // CPU0 should send an interrupt to CPU1 to unblock its Tx task (the buffer might have an open space now)
    int status = XScuGic_SoftwareIntr(INTR_GIC_INSTANCE_ADDR, INTR_UNBLOCK_CPU1_TX_INT_ID, XSCUGIC_SPI_CPU1_MASK);
}

#elif XPAR_CPU_ID == 1

void vCPU1to0SendCallback(MessageBufferHandle_t xMessageBuffer,
                          BaseType_t xIsInsideISR,
                          BaseType_t *const pxHigherPriorityTaskWoken)
{
    // xil_printf("DEBUG: CPU 1 to 0 Send Callback reached (in CPU1)\r\n");
    xil_printf("k\r\n");

    // CPU 1 should send an interrupt to CPU0 to unblock its Rx task
    int status = XScuGic_SoftwareIntr(INTR_GIC_INSTANCE_ADDR, INTR_UNBLOCK_CPU0_RX_INT_ID, XSCUGIC_SPI_CPU0_MASK);
}

void vCPU0to1ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken)
{
    // xil_printf("DEBUG: CPU 0 to 1 Receive Callback reached (in CPU1)\r\n");
    xil_printf("l\r\n");

    // CPU 1 should send an interrupt to CPU0 to unblock its Tx task (the buffer might have an open space)
    int status = XScuGic_SoftwareIntr(INTR_GIC_INSTANCE_ADDR, INTR_UNBLOCK_CPU0_TX_INT_ID, XSCUGIC_SPI_CPU0_MASK);
}

#endif
