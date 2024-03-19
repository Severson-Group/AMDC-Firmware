#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "message_buffer.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"

///////////////////////////////////////////////////////
// THIS IS A SHARED FILE, SO IT IS ALWAYS
// IN SYNC IN BOTH CPU0 AND CPU1
//
// If you need to differentiate something between
// CPUs, use "#if XPAR_CPU_ID == ?"
///////////////////////////////////////////////////////

/* This file contains all the macro definitions that need to be shared between CPU0 and CPU1
 * This includes definitions for Inter-Core Communication, Inter-Core Interrupts,
 *   the Generic Interrupt Controller (GIC) definitions and metadata, etc
 *
 * Per Zynq-7000 TRM Ch. 4: System Addresses (page 106), the initial mapping
 *   of OCM is split between low addresses and high addresses in 64 KB chunks.
 * We will pick to use the highest 64 KB chunk as our base address: */
#define SHARED_OCM_BASE_ADDR (0xFFFF0000)

///////////////////////////////////
//  INTER-CORE COMMUNICATION
/////////////////////////////////
#define ICC_BUFFER_STRUCT_SIZE (sizeof(StaticMessageBuffer_t))
#define ICC_BUFFER_SIZE        (4 * 1024)
#define ICC_HANDLE_SIZE        (sizeof(MessageBufferHandle_t))
#define ICC_LOCK_SIZE          (sizeof(uint8_t))
#define ICC_FUNC_PTR_SIZE      (sizeof(void *))

/* These are the handles for the Message Buffers that need to be used by other tasks
 *   In reality, the handle is just the pointer to the message buffer struct (its memory address)
 *   These should end up being the addresses computed above */
MessageBufferHandle_t xCPU0to1MessageBufferHandle;
MessageBufferHandle_t xCPU1to0MessageBufferHandle;

/* Define the pointers to the two structs (that store the metadata) and two message spaces (that hold the messages) in
 * shared memory. The ICC_BUFFER_SIZE Should be one more than the value passed in the xBufferSizeBytes parameter. The
 * two structs will be located back-to-back right at the base addr of the shared OCM, followed thereafter by the actual
 * message buffers. */
#define ICC_CPU0to1BufferStructAddr ((uint8_t *) (SHARED_OCM_BASE_ADDR + (0 * ICC_BUFFER_STRUCT_SIZE)))
#define ICC_CPU1to0BufferStructAddr ((uint8_t *) (SHARED_OCM_BASE_ADDR + (1 * ICC_BUFFER_STRUCT_SIZE)))

#define ICC_CPU0to1BufferSpaceAddr                                                                                     \
    ((uint8_t *) (SHARED_OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (0 * ICC_BUFFER_SIZE)))
#define ICC_CPU1to0BufferSpaceAddr                                                                                     \
    ((uint8_t *) (SHARED_OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (1 * ICC_BUFFER_SIZE)))

/* These memory spaces are used to transfer the Message Buffer Handles from CPU0 (who does the initialization work, and
 * gets the handles from the xMessageBufferCreateStaticWithCallback function) to CPU1 (who doesn't initialize anything
 * and gets the handles from CPU0, via these drop-zones) */
#define ICC_CPU0to1HandleDropzoneAddr (SHARED_OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (2 * ICC_BUFFER_SIZE) + (0 * ICC_HANDLE_SIZE)))
#define ICC_CPU1to0HandleDropzoneAddr (SHARED_OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (2 * ICC_BUFFER_SIZE) + (1 * ICC_HANDLE_SIZE)))

#define ICC_getCPU0to1Handle          ((*((MessageBufferHandle_t *) ICC_CPU0to1HandleDropzoneAddr))
#define ICC_setCPU0to1Handle(handle)  ((*((MessageBufferHandle_t *) ICC_CPU0to1HandleDropzoneAddr) = handle)
#define ICC_getCPU1to0Handle          ((*((MessageBufferHandle_t *) ICC_CPU1to0HandleDropzoneAddr))
#define ICC_setCPU1to0Handle(handle)  ((*((MessageBufferHandle_t *) ICC_CPU1to0HandleDropzoneAddr) = handle)

/* We need a concurrency lock in the shared memory to make sure that CPU0 always sets the handles before
 * CPU1 attempts to get them. This is initialized to 0, and CPU0 sets it to 1. CPU1 will wait till it sees
 * this change before attempting to get the handles. */
#define ICC_initLockAddr (SHARED_OCM_BASE_ADDR + (2 * ICC_BUFFER_STRUCT_SIZE) + (2 * ICC_BUFFER_SIZE) + (2 * ICC_HANDLE_SIZE)))

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

///////////////////////////////////
//  INTERRUPTS / GIC
/////////////////////////////////
#define INTR_UNBLOCK_CPU0_RX_INT_ID 0
#define INTR_UNBLOCK_CPU0_TX_INT_ID 1
#define INTR_UNBLOCK_CPU1_RX_INT_ID 2
#define INTR_UNBLOCK_CPU1_TX_INT_ID 3

#define INTR_SHARED_MEMORY_BASE_ADDR (0xFFFFF000) //(ICC_functionPointersLockAddr + sizeof(uint8_t))
#define INTR_GIC_INSTANCE_SIZE       (sizeof(XScuGic))

// Interrupt Controller Instance
//   Defined here to be accessible in both sys/icc.c and sys/intr.h
#define INTR_GIC_INSTANCE_ADDR ((XScuGic *) INTR_SHARED_MEMORY_BASE_ADDR)
#define INTR_gicInstance       (*((XScuGic *) INTR_GIC_INSTANCE_ADDR))

// Interrupt Controller Initializaton Lock w/ getter & setter
#define INTR_gicInitLockAddr (INTR_GIC_INSTANCE_ADDR + INTR_GIC_INSTANCE_SIZE)

#define INTR_getGicInitReady (*((uint8_t *) INTR_gicInitLockAddr))
#define INTR_setGicInitReady (*((uint8_t *) INTR_gicInitLockAddr) = 1)

#endif /* SHARED_MEMORY_H */
