#ifndef ICC_H
#define ICC_H

#include "FreeRTOS.h"
#include "intr.h"
#include "message_buffer.h"
#include "xil_printf.h"
#include <stdint.h>

///////////////////////////////////////////////////////
// THIS IS A SHARED FILE, SO IT IS ALWAYS
// IN SYNC IN BOTH CPU0 AND CPU1
//
// If you need to differentiate something between
// CPUs, use "#if XPAR_CPU_ID == ?"
///////////////////////////////////////////////////////

// ========================
// Inter-Core Communication
// ========================
//
// This module implements communication between the two cores, CPU0 and CPU1.
// CPU0 acts as the master and initializes all shared memory spaces.
//
// In the OCM, we implement two FreeRTOS Message Buffers: one from CPU0 to CPU1,
// and one from CPU1 to CPU0. Each FIFO and associated data are used to build a
// simple producer-consumer system.
//
// This implementation is known to be thread (dual-core) safe.
//
// For more details and pseudo code, see these resources:
// https://www.freertos.org/RTOS-message-buffer-API.html
// https://www.freertos.org/2020/02/simple-multicore-core-to-core-communication-using-freertos-message-buffers.html

// Per Zynq-7000 TRM Ch. 4: System Addresses (page 106), the initial mapping
// of OCM is split between low addresses and high addresses in 64 KB chunks.
//
// We will pick to use the highest 64 KB chunk as our base address:
#define OCM_BASE_ADDR          (0xFFFF0000)
#define ICC_BUFFER_STRUCT_SIZE (sizeof(StaticMessageBuffer_t))
#define ICC_BUFFER_SIZE        (4 * 1024)
#define ICC_HANDLE_SIZE        (sizeof(MessageBufferHandle_t))
#define ICC_LOCK_SIZE          (sizeof(uint8_t))
#define ICC_FUNC_PTR_SIZE      (sizeof(void *))

/* Define the pointers to the two structs (that store the metadata) and two message spaces (that hold the messages) in
 * shared memory. The ICC_BUFFER_SIZE Should be one more than the value passed in the xBufferSizeBytes parameter. The
 * two structs will be located back-to-back right at the base addr of the shared OCM, followed thereafter by the actual
 * message buffers. */
#define ICC_CPU0to1BufferStructAddr ((uint8_t *) (OCM_BASE_ADDR + (0 * ICC_BUFFER_STRUCT_SIZE)))
#define ICC_CPU1to0BufferStructAddr ((uint8_t *) (OCM_BASE_ADDR + (1 * ICC_BUFFER_STRUCT_SIZE)))

#define ICC_CPU0to1BufferSpaceAddr ((uint8_t *) (OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (0 * ICC_BUFFER_SIZE)))
#define ICC_CPU1to0BufferSpaceAddr ((uint8_t *) (OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (1 * ICC_BUFFER_SIZE)))

/* These memory spaces are used to transfer the Message Buffer Handles from CPU0 (who does the initialization work, and
 * gets the handles from the xMessageBufferCreateStaticWithCallback function) to CPU1 (who doesn't initialize anything
 * and gets the handles from CPU0, via these drop-zones) */
#define ICC_CPU0to1HandleDropzoneAddr (OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (2 * ICC_BUFFER_SIZE) + (0 * ICC_HANDLE_SIZE)))
#define ICC_CPU1to0HandleDropzoneAddr (OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (2 * ICC_BUFFER_SIZE) + (1 * ICC_HANDLE_SIZE)))

#define ICC_getCPU0to1Handle          ((*((MessageBufferHandle_t *) ICC_CPU0to1HandleDropzoneAddr))
#define ICC_setCPU0to1Handle(handle)  ((*((MessageBufferHandle_t *) ICC_CPU0to1HandleDropzoneAddr) = handle)
#define ICC_getCPU1to0Handle          ((*((MessageBufferHandle_t *) ICC_CPU1to0HandleDropzoneAddr))
#define ICC_setCPU1to0Handle(handle)  ((*((MessageBufferHandle_t *) ICC_CPU1to0HandleDropzoneAddr) = handle)

/* We need a concurrency lock in the shared memory to make sure that CPU0 always sets the handles before
 * CPU1 attempts to get them. This is initialized to 0, and CPU0 sets it to 1. CPU1 will wait till it sees
 * this change before attempting to get the handles. */
#define ICC_initLockAddr (OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (2 * ICC_BUFFER_SIZE) + (2 * ICC_HANDLE_SIZE)))

#define ICC_getHandleComplete  ((*((uint8_t *) ICC_initLockAddr))
#define ICC_setHandleComplete  ((*((uint8_t *) ICC_initLockAddr) = 1)

/* These memory spaces are used to transfer callback function pointers from CPU1 to CPU0 for initialization */
#define ICC_CPU1to0SendCallbackAddr    (ICC_initLockAddr + ICC_LOCK_SIZE + (0 * ICC_FUNC_PTR_SIZE))
#define ICC_CPU0to1ReceiveCallbackAddr (ICC_initLockAddr + ICC_LOCK_SIZE + (1 * ICC_FUNC_PTR_SIZE))

#define ICC_getCPU1to0SendCallback                ((*((void **) ICC_CPU1to0SendCallbackAddr))
#define ICC_setCPU1to0SendCallback(func_ptr)      ((*((void **) ICC_CPU1to0SendCallbackAddr) = func_ptr)
#define ICC_getCPU0to1ReceiveCallback             ((*((void **) ICC_CPU0to1ReceiveCallbackAddr))
#define ICC_setCPU0to1ReceiveCallback(func_ptr)   ((*((void **) ICC_CPU0to1ReceiveCallbackAddr) = func_ptr)

/* We need a concurrency lock in the shared memory to make sure that CPU1 always provides the function pointers before
 * CPU0 attempts to get them. This is initialized to 0, and CPU1 sets it to 1. CPU0 will wait till it sees
 * this change before attempting to get the function pointers. */
#define ICC_functionPointersLockAddr (ICC_initLockAddr + ICC_LOCK_SIZE + (2 * ICC_FUNC_PTR_SIZE))

#define ICC_getFunctionPointersReady  ((*((uint8_t *) ICC_functionPointersLockAddr))
#define ICC_setFunctionPointersReady  ((*((uint8_t *) ICC_functionPointersLockAddr) = 1)

/* These are the handles for the Message Buffers that need to be used by other tasks
 *   In reality, the handle is just the pointer to the message buffer struct (its memory address)
 *   These should end up being the addresses computed above */
MessageBufferHandle_t xCPU0to1MessageBufferHandle;
MessageBufferHandle_t xCPU1to0MessageBufferHandle;

void icc_init();
#if XPAR_CPU_ID == 0
void vCPU0to1SendCallback(MessageBufferHandle_t xMessageBuffer,
                          BaseType_t xIsInsideISR,
                          BaseType_t *const pxHigherPriorityTaskWoken);
void vCPU1to0ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken);
#elif XPAR_CPU_ID == 1
void vCPU1to0SendCallback(MessageBufferHandle_t xMessageBuffer,
                          BaseType_t xIsInsideISR,
                          BaseType_t *const pxHigherPriorityTaskWoken);
void vCPU0to1ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken);
#endif /* Callback declarations */

#endif /* ICC_H */
