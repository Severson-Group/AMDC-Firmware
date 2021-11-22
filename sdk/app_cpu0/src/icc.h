#ifndef ICC_H
#define ICC_H

#include <stdint.h>

// KEEP THIS FILE IN SYNC BETWEEN THE TWO CORES!!

// ========================
// Inter-Core Communication
// ========================
//
// This module implements communication betwen the two cores, CPU0 and CPU1.
// CPU0 acts as the master and initializes all shared memory spaces.
//
// In the OCM, we implement two FIFOs: one from CPU0 to 1,
// and one from CPU1 to 0. Each FIFO and assoicated data are
// used to build a simple producer-consumer system.
//
// This implementation is known to be thread (dual-core) safe.
//
// For more details and pseudo code of the what is implemented here:
// https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem

// Per TRM page 738, the initial mapping of OCM is split between
// low addresses and high addresses in 64 KB chunks.
//
// We will pick to use the highest 64 KB chunk as our base address:
#define OCM_BASE_ADDR (0xFFFF0000)

// MUST be set such that: (UINT_MAX + 1) % BUFFER_SIZE is an integer.
// On this platform, UINT_MAX = 2^32 - 1. Thus, BUFFER_SIZE needs
// to evenly go into 2^32. This is possible if we keep the buffer
// size as a power of 2: e.g. 2^10.
#define ICC_BUFFER_SIZE (4 * 1024)

// Keep 1024 bytes free at the beginning of the OCM so we can have shared memory spaces for registers, counters, etc

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

void icc_init(void);

#endif // ICC_H
