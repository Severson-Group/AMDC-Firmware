/*
 * icc.h
 *
 *  Created on: 7 nov. 2023
 *      Author: pnowa
 */

#ifndef ICC_H
#define ICC_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "message_buffer.h"

// KEEP THIS FILE IN SYNC IN BOTH CPU0 AND CPU1!!!

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

// MUST be set such that: (UINT_MAX + 1) % BUFFER_SIZE is an integer.
// On this platform, UINT_MAX = 2^32 - 1. Thus, BUFFER_SIZE needs
// to evenly go into 2^32. This is possible if we keep the buffer
// size as a power of 2: e.g. 2^10.
#define ICC_BUFFER_SIZE (4 * 1024)

StaticMessageBuffer_t CPU0_to_CPU1_MessageBuffer;
StaticMessageBuffer_t CPU1_to_CPU0_MessageBuffer;

void icc_init(void);

#endif /* ICC_H */
