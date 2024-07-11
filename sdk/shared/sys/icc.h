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
#define OCM_BASE_ADDR   (0xFFFF0000)
#define ICC_BUFFER_STRUCT_SIZE (sizeof(StaticMessageBuffer_t))
#define ICC_BUFFER_SIZE (4 * 1024)
#define ICC_HANDLE_SIZE (sizeof(MessageBufferHandle_t))


/* Define the pointers to the two structs (that store the metadata) and two message spaces (that hold the messages) in shared memory.
 * The ICC_BUFFER_SIZE Should be one more than the value passed in the xBufferSizeBytes parameter.
 * The two structs will be located back-to-back right at the base addr of the shared OCM, followed thereafter by the actual message buffers. */
#define ICC_CPU0to1_BufferStructAddr ((uint8_t *) (OCM_BASE_ADDR + (0 * ICC_BUFFER_STRUCT_SIZE)))
#define ICC_CPU1to0_BufferStructAddr ((uint8_t *) (OCM_BASE_ADDR + (1 * ICC_BUFFER_STRUCT_SIZE)))

#define ICC_CPU0to1_BufferSpaceAddr ((uint8_t *) (OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (0 * ICC_BUFFER_SIZE)))
#define ICC_CPU1to0_BufferSpaceAddr ((uint8_t *) (OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (1 * ICC_BUFFER_SIZE)))

// ===============================
// CH0: the ascii_cmd socket
// ===============================

#define ICC_CPU0to1_CH0__BufferBaseAddr ((uint8_t *) (OCM_BASE_ADDR + 1024 + (0 * ICC_BUFFER_SIZE)))
#define ICC_CPU1to0_CH0__BufferBaseAddr ((uint8_t *) (OCM_BASE_ADDR + 1024 + (1 * ICC_BUFFER_SIZE)))

#define ICC_CPU0to1_CH0__GET_ProduceCount    (*((uint32_t *) (OCM_BASE_ADDR + 0)))
#define ICC_CPU0to1_CH0__SET_ProduceCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 0)) = x)
#define ICC_CPU0to1_CH0__GET_ConsumeCount    (*((uint32_t *) (OCM_BASE_ADDR + 4)))
#define ICC_CPU0to1_CH0__SET_ConsumeCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 4)) = x)

#define ICC_CPU1to0_CH0__GET_ProduceCount    (*((uint32_t *) (OCM_BASE_ADDR + 8)))
#define ICC_CPU1to0_CH0__SET_ProduceCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 8)) = x)
#define ICC_CPU1to0_CH0__GET_ConsumeCount    (*((uint32_t *) (OCM_BASE_ADDR + 12)))
#define ICC_CPU1to0_CH0__SET_ConsumeCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 12)) = x)

#define ICC_CPU1to0_CH0__IsBufferFull                                                                                  \
    (ICC_CPU1to0_CH0__GET_ProduceCount - ICC_CPU1to0_CH0__GET_ConsumeCount == ICC_BUFFER_SIZE)
#define ICC_CPU1to0_CH0__IsBufferEmpty (ICC_CPU1to0_CH0__GET_ProduceCount - ICC_CPU1to0_CH0__GET_ConsumeCount == 0)

#define ICC_CPU0to1_CH0__IsBufferFull                                                                                  \
    (ICC_CPU0to1_CH0__GET_ProduceCount - ICC_CPU0to1_CH0__GET_ConsumeCount == ICC_BUFFER_SIZE)
#define ICC_CPU0to1_CH0__IsBufferEmpty (ICC_CPU0to1_CH0__GET_ProduceCount - ICC_CPU0to1_CH0__GET_ConsumeCount == 0)

// ====================
// FIFOs for log variable streaming
// Data only flows from CPU1 to CPU0 for logging
//
// CH1 to 4 are for log streams
// ====================

#define ICC_CPU1to0_CH1__BufferBaseAddr ((uint8_t *) (OCM_BASE_ADDR + 1024 + (2 * ICC_BUFFER_SIZE)))
#define ICC_CPU1to0_CH2__BufferBaseAddr ((uint8_t *) (OCM_BASE_ADDR + 1024 + (3 * ICC_BUFFER_SIZE)))
#define ICC_CPU1to0_CH3__BufferBaseAddr ((uint8_t *) (OCM_BASE_ADDR + 1024 + (4 * ICC_BUFFER_SIZE)))
#define ICC_CPU1to0_CH4__BufferBaseAddr ((uint8_t *) (OCM_BASE_ADDR + 1024 + (5 * ICC_BUFFER_SIZE)))

#define ICC_CPU1to0_CH1__GET_ProduceCount    (*((uint32_t *) (OCM_BASE_ADDR + 16)))
#define ICC_CPU1to0_CH1__SET_ProduceCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 16)) = x)
#define ICC_CPU1to0_CH1__GET_ConsumeCount    (*((uint32_t *) (OCM_BASE_ADDR + 20)))
#define ICC_CPU1to0_CH1__SET_ConsumeCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 20)) = x)

#define ICC_CPU1to0_CH2__GET_ProduceCount    (*((uint32_t *) (OCM_BASE_ADDR + 24)))
#define ICC_CPU1to0_CH2__SET_ProduceCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 24)) = x)
#define ICC_CPU1to0_CH2__GET_ConsumeCount    (*((uint32_t *) (OCM_BASE_ADDR + 28)))
#define ICC_CPU1to0_CH2__SET_ConsumeCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 28)) = x)

#define ICC_CPU1to0_CH3__GET_ProduceCount    (*((uint32_t *) (OCM_BASE_ADDR + 32)))
#define ICC_CPU1to0_CH3__SET_ProduceCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 32)) = x)
#define ICC_CPU1to0_CH3__GET_ConsumeCount    (*((uint32_t *) (OCM_BASE_ADDR + 36)))
#define ICC_CPU1to0_CH3__SET_ConsumeCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 36)) = x)

#define ICC_CPU1to0_CH4__GET_ProduceCount    (*((uint32_t *) (OCM_BASE_ADDR + 40)))
#define ICC_CPU1to0_CH4__SET_ProduceCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 40)) = x)
#define ICC_CPU1to0_CH4__GET_ConsumeCount    (*((uint32_t *) (OCM_BASE_ADDR + 44)))
#define ICC_CPU1to0_CH4__SET_ConsumeCount(x) (*((uint32_t *) (OCM_BASE_ADDR + 44)) = x)

#define ICC_CPU1to0_CH1__IsBufferFull                                                                                  \
    (ICC_CPU1to0_CH1__GET_ProduceCount - ICC_CPU1to0_CH1__GET_ConsumeCount == ICC_BUFFER_SIZE)
#define ICC_CPU1to0_CH1__IsBufferEmpty (ICC_CPU1to0_CH1__GET_ProduceCount - ICC_CPU1to0_CH1__GET_ConsumeCount == 0)
#define ICC_CPU1to0_CH2__IsBufferFull                                                                                  \
    (ICC_CPU1to0_CH2__GET_ProduceCount - ICC_CPU1to0_CH2__GET_ConsumeCount == ICC_BUFFER_SIZE)
#define ICC_CPU1to0_CH2__IsBufferEmpty (ICC_CPU1to0_CH2__GET_ProduceCount - ICC_CPU1to0_CH2__GET_ConsumeCount == 0)
#define ICC_CPU1to0_CH3__IsBufferFull                                                                                  \
    (ICC_CPU1to0_CH3__GET_ProduceCount - ICC_CPU1to0_CH3__GET_ConsumeCount == ICC_BUFFER_SIZE)
#define ICC_CPU1to0_CH3__IsBufferEmpty (ICC_CPU1to0_CH3__GET_ProduceCount - ICC_CPU1to0_CH3__GET_ConsumeCount == 0)
#define ICC_CPU1to0_CH4__IsBufferFull                                                                                  \
    (ICC_CPU1to0_CH4__GET_ProduceCount - ICC_CPU1to0_CH4__GET_ConsumeCount == ICC_BUFFER_SIZE)
#define ICC_CPU1to0_CH4__IsBufferEmpty (ICC_CPU1to0_CH4__GET_ProduceCount - ICC_CPU1to0_CH4__GET_ConsumeCount == 0)

#define ICC_CPU1to0_CH1__INC_ProduceCount (ICC_CPU1to0_CH1__SET_ProduceCount(ICC_CPU1to0_CH1__GET_ProduceCount + 1))
#define ICC_CPU1to0_CH2__INC_ProduceCount (ICC_CPU1to0_CH2__SET_ProduceCount(ICC_CPU1to0_CH2__GET_ProduceCount + 1))
#define ICC_CPU1to0_CH3__INC_ProduceCount (ICC_CPU1to0_CH3__SET_ProduceCount(ICC_CPU1to0_CH3__GET_ProduceCount + 1))
#define ICC_CPU1to0_CH4__INC_ProduceCount (ICC_CPU1to0_CH4__SET_ProduceCount(ICC_CPU1to0_CH4__GET_ProduceCount + 1))

#define ICC_CPU1to0_CH1__INC_ConsumeCount (ICC_CPU1to0_CH1__SET_ConsumeCount(ICC_CPU1to0_CH1__GET_ConsumeCount + 1))
#define ICC_CPU1to0_CH2__INC_ConsumeCount (ICC_CPU1to0_CH2__SET_ConsumeCount(ICC_CPU1to0_CH2__GET_ConsumeCount + 1))
#define ICC_CPU1to0_CH3__INC_ConsumeCount (ICC_CPU1to0_CH3__SET_ConsumeCount(ICC_CPU1to0_CH3__GET_ConsumeCount + 1))
#define ICC_CPU1to0_CH4__INC_ConsumeCount (ICC_CPU1to0_CH4__SET_ConsumeCount(ICC_CPU1to0_CH4__GET_ConsumeCount + 1))

/* These memory spaces are used to transfer the Message Buffer Handles from CPU0 (who does the initialization work, and gets the handles
 * from the xMessageBufferCreateStaticWithCallback function) to CPU1 (who doesn't initialize anything and gets the handles from CPU0, via
 * these drop-zones) */
#define ICC_CPU0to1_HandleDropzoneAddr (OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (2 * ICC_BUFFER_SIZE) + (0 * ICC_HANDLE_SIZE)))
#define ICC_CPU1to0_HandleDropzoneAddr (OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (2 * ICC_BUFFER_SIZE) + (1 * ICC_HANDLE_SIZE)))

#define ICC_getCPU0to1Handle          (*((MessageBufferHandle_t *) ICC_CPU0to1_HandleDropzoneAddr))
#define ICC_setCPU0to1Handle(handle)  (*((MessageBufferHandle_t *) ICC_CPU0to1_HandleDropzoneAddr) = handle)
#define ICC_getCPU1to0Handle          (*((MessageBufferHandle_t *) ICC_CPU1to0_HandleDropzoneAddr))
#define ICC_setCPU1to0Handle(handle)  (*((MessageBufferHandle_t *) ICC_CPU1to0_HandleDropzoneAddr) = handle)



/* These are the handles for the Message Buffers that need to be used by other tasks
 *   In reality, the handle is just the pointer to the message buffer struct (its memory address)
 *   These should end up being the addresses computed above */
MessageBufferHandle_t xCPU0to1MessageBuffer;
MessageBufferHandle_t xCPU1to0MessageBuffer;


void icc_init();
void vCPU0to1SendCallback(MessageBufferHandle_t xMessageBuffer,
                          BaseType_t xIsInsideISR,
                          BaseType_t *const pxHigherPriorityTaskWoken);
void vCPU0to1ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken);
void vCPU1to0SendCallback(MessageBufferHandle_t xMessageBuffer,
                          BaseType_t xIsInsideISR,
                          BaseType_t *const pxHigherPriorityTaskWoken);
void vCPU1to0ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken);

#endif /* ICC_H */
