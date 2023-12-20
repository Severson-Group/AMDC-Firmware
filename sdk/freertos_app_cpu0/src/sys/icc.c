/*
 * icc.c
 *
 *  Created on: 7 nov. 2023
 *      Author: pnowa
 */

#include "icc.h"

///////////////////////////////////////////////////////
// KEEP THIS FILE IN SYNC IN BOTH CPU0 AND CPU1
///////////////////////////////////////////////////////

void icc_init(uint32_t cpu_num)
{
	if (cpu_num == 0){
		/* CPU 0 is responsible for:
		 * 1. Creating its own send and receive tasks which interact with the message buffers */
		xil_printf("ICC INIT CPU0\r\n");
	}
	else if (cpu_num == 1){
		/* CPU 1 is responsible for:
		 * 1. Creating its own send and receive tasks which interact with the message buffers */
		xil_printf("ICC INIT CPU1\r\n");
	}
	else{
		// TODO: This should probably throw a proper exception
		xil_printf("Invalid CPU number passed to ICC initialization.\r\n");
		while(1){}
	}

    // MessageBufferHandle_t xMessageBufferCreateStaticWithCallback(
    //                           size_t xBufferSizeBytes,
    //                           uint8_t *pucMessageBufferStorageArea,
    //                           StaticMessageBuffer_t *pxStaticMessageBuffer,
    //                           StreamBufferCallbackFunction_t pxSendCompletedCallback,
    //                           StreamBufferCallbackFunction_t pxReceiveCompletedCallback );

    /* Create two message buffers for inter-core communication that use the callback
     * functions below as send and receive completed callback functions. */
    xCPU0to1MessageBuffer = xMessageBufferCreateStaticWithCallback(ICC_BUFFER_SIZE - 1,
                                                 ICC_CPU0to1_BufferBaseAddr,
                                                 &xCPU0to1MessageBufferStruct,
                                                 vCPU0to1SendCallback,
                                                 vCPU0to1ReceiveCallback);

    xCPU1to0MessageBuffer = xMessageBufferCreateStaticWithCallback(ICC_BUFFER_SIZE - 1,
                                                 ICC_CPU1to0_BufferBaseAddr,
                                                 &xCPU1to0MessageBufferStruct,
                                                 vCPU1to0SendCallback,
                                                 vCPU1to0ReceiveCallback);
}

void vCPU0to1SendCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken)
{
    /* Insert code here which is invoked when a message is written to the message buffer.
     * This is useful when a message buffer is used to pass messages between
     * cores on a multicore processor. In that scenario, this callback
     * can be implemented to generate an interrupt in the other CPU core,
     * and the interrupt's service routine can then use the
     * xMessageBufferSendCompletedFromISR() API function to check, and if
     * necessary unblock, a task that was waiting for message. */
}

void vCPU0to1ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                                BaseType_t xIsInsideISR,
                                BaseType_t *const pxHigherPriorityTaskWoken)
{
    /* Insert code here which is invoked when a message is read from a message buffer.
     * This is useful when a message buffer is used to pass messages between
     * cores on a multicore processor. In that scenario, this callback
     * can be implemented to generate an interrupt in the other CPU core,
     * and the interrupt's service routine can then use the
     * xMessageBufferReceiveCompletedFromISR() API function to check, and if
     * necessary unblock, a task that was waiting to send message. */
}

void vCPU1to0SendCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken)
{
    /* Insert code here which is invoked when a message is written to the message buffer.
     * This is useful when a message buffer is used to pass messages between
     * cores on a multicore processor. In that scenario, this callback
     * can be implemented to generate an interrupt in the other CPU core,
     * and the interrupt's service routine can then use the
     * xMessageBufferSendCompletedFromISR() API function to check, and if
     * necessary unblock, a task that was waiting for message. */
}

void vCPU1to0ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                                BaseType_t xIsInsideISR,
                                BaseType_t *const pxHigherPriorityTaskWoken)
{
    /* Insert code here which is invoked when a message is read from a message buffer.
     * This is useful when a message buffer is used to pass messages between
     * cores on a multicore processor. In that scenario, this callback
     * can be implemented to generate an interrupt in the other CPU core,
     * and the interrupt's service routine can then use the
     * xMessageBufferReceiveCompletedFromISR() API function to check, and if
     * necessary unblock, a task that was waiting to send message. */
}

/* Wrapper functions for ease-of-use
 *   Only available in the correct CPU
 *   CPU_NUM is defined in each core's FreeRTOSConfig.h */



/*
#if CPU_NUM == 0
void icc_send_cpu0_to_cpu1 (void * data){
	size_t size_of_thing = xMessageBufferSend();
}
#endif

#if CPU_NUM == 1
void icc_send_cpu1_to_cpu0 (void * data){
	size_t size_of_thing = xMessageBufferSend();
}
#endif
*/
