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

void icc_init(void)
{

    // MessageBufferHandle_t xMessageBufferCreateStaticWithCallback(
    //                           size_t xBufferSizeBytes,
    //                           uint8_t *pucMessageBufferStorageArea,
    //                           StaticMessageBuffer_t *pxStaticMessageBuffer,
    //                           StreamBufferCallbackFunction_t pxSendCompletedCallback,
    //                           StreamBufferCallbackFunction_t pxReceiveCompletedCallback );

    /* Create two message buffers for inter-core communication that use the callback
     * functions below as send and receive completed callback functions. */
    MessageBufferHandle_t xCPU0toCPU1MessageBufferWithCallback
        = xMessageBufferCreateStaticWithCallback(STORAGE_SIZE_BYTES - 1,
                                                 ucCPU0toCPU1MessageBufferStorage,
                                                 &xCPU0toCPU1MessageBuffer,
                                                 vCPU0toCPU1SendCallback,
                                                 vCPU0toCPU1ReceiveCallback);

    MessageBufferHandle_t xCPU1toCPU0MessageBufferWithCallback
        = xMessageBufferCreateStaticWithCallback(STORAGE_SIZE_BYTES - 1,
                                                 ucCPU1toCPU0MessageBufferStorage,
                                                 &xCPU1toCPU0MessageBuffer,
                                                 vCPU1toCPU0SendCallback,
                                                 vCPU1toCPU0ReceiveCallback);

    /* As neither the pucMessageBufferStorageArea or pxStaticMessageBuffer
     * parameters were NULL, xMessageBufferWithCallback
     * will not be NULL, and can be used to reference the created message
     * buffers in other message buffer API calls. */

    /* Other code that uses the message buffers can go here. */
}

void vCPU0toCPU1SendCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken)
{
    /* Insert code here which is invoked when a message is written to
     * the message buffer.
     * This is useful when a message buffer is used to pass messages between
     * cores on a multicore processor. In that scenario, this callback
     * can be implemented to generate an interrupt in the other CPU core,
     * and the interrupt's service routine can then use the
     * xMessageBufferSendCompletedFromISR() API function to check, and if
     * necessary unblock, a task that was waiting for message. */
}

void vCPU0toCPU1ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                                BaseType_t xIsInsideISR,
                                BaseType_t *const pxHigherPriorityTaskWoken)
{
    /* Insert code here which is invoked when a message is read from a message
     * buffer.
     * This is useful when a message buffer is used to pass messages between
     * cores on a multicore processor. In that scenario, this callback
     * can be implemented to generate an interrupt in the other CPU core,
     * and the interrupt's service routine can then use the
     * xMessageBufferReceiveCompletedFromISR() API function to check, and if
     * necessary unblock, a task that was waiting to send message. */
}

void vCPU1toCPU0SendCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken)
{
    /* Insert code here which is invoked when a message is written to
     * the message buffer.
     * This is useful when a message buffer is used to pass messages between
     * cores on a multicore processor. In that scenario, this callback
     * can be implemented to generate an interrupt in the other CPU core,
     * and the interrupt's service routine can then use the
     * xMessageBufferSendCompletedFromISR() API function to check, and if
     * necessary unblock, a task that was waiting for message. */
}

void vCPU1toCPU0ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                                BaseType_t xIsInsideISR,
                                BaseType_t *const pxHigherPriorityTaskWoken)
{
    /* Insert code here which is invoked when a message is read from a message
     * buffer.
     * This is useful when a message buffer is used to pass messages between
     * cores on a multicore processor. In that scenario, this callback
     * can be implemented to generate an interrupt in the other CPU core,
     * and the interrupt's service routine can then use the
     * xMessageBufferReceiveCompletedFromISR() API function to check, and if
     * necessary unblock, a task that was waiting to send message. */
}
