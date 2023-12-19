/*
 * icc.h
 *
 *  Created on: 7 nov. 2023
 *      Author: pnowa
 */

#ifndef ICC_H
#define ICC_H

#include "FreeRTOS.h"
#include "message_buffer.h"
#include <stdint.h>

///////////////////////////////////////////////////////
// KEEP THIS FILE IN SYNC IN BOTH CPU0 AND CPU1
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
#define OCM_BASE_ADDR (0xFFFF0000)

/* Used to dimension the array used to hold the messages. The available
 * space will actually be one less than this, so 1023. */
#define STORAGE_SIZE_BYTES 1024

// These hold the structs for our two MessageBuffers
StaticMessageBuffer_t xCPU0toCPU1MessageBuffer;
StaticMessageBuffer_t xCPU1toCPU0MessageBuffer;

/* Defines the memory that will actually hold the messages within the message
 * buffer. Should be one more than the value passed in the xBufferSizeBytes
 * parameter. */
static uint8_t ucCPU0toCPU1MessageBufferStorage[STORAGE_SIZE_BYTES];
static uint8_t ucCPU1toCPU0MessageBufferStorage[STORAGE_SIZE_BYTES];

void icc_init(void);
void vCPU0toCPU1SendCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken);
void vCPU0toCPU1ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                                BaseType_t xIsInsideISR,
                                BaseType_t *const pxHigherPriorityTaskWoken);
void vCPU1toCPU0SendCallback(MessageBufferHandle_t xMessageBuffer,
                             BaseType_t xIsInsideISR,
                             BaseType_t *const pxHigherPriorityTaskWoken);
void vCPU1toCPU0ReceiveCallback(MessageBufferHandle_t xMessageBuffer,
                                BaseType_t xIsInsideISR,
                                BaseType_t *const pxHigherPriorityTaskWoken);

#endif /* ICC_H */
