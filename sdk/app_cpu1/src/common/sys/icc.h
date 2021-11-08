#ifndef ICC_H
#define ICC_H

#include <stdint.h>

// Per TRM page 738, the initial mapping of OCM is split between
// low addresses and high addressses in 64 KB chunks.
//
// We will pick to use the highest 64 KB chunk as our base address:
#define OCM_BASE_ADDR (0xFFFF0000)

#define ICC_CPU0to1__SET_CPU1_WaitingForData (*((uint32_t *) (OCM_BASE_ADDR + 0)) = 1)
#define ICC_CPU0to1__CLR_CPU1_WaitingForData (*((uint32_t *) (OCM_BASE_ADDR + 0)) = 0)
#define ICC_CPU0to1__GET_CPU1_WaitingForData (*((uint32_t *) (OCM_BASE_ADDR + 0)))

#define ICC_CPU0to1__SET_CPU0_HasWrittenData (*((uint32_t *) (OCM_BASE_ADDR + 4)) = 1)
#define ICC_CPU0to1__CLR_CPU0_HasWrittenData (*((uint32_t *) (OCM_BASE_ADDR + 4)) = 0)
#define ICC_CPU0to1__GET_CPU0_HasWrittenData (*((uint32_t *) (OCM_BASE_ADDR + 4)))

#define ICC_CPU0to1__SET_DATA(x) (*((uint32_t *) (OCM_BASE_ADDR + 8)) = x)
#define ICC_CPU0to1__GET_DATA    (*((uint32_t *) (OCM_BASE_ADDR + 8)))

//#define ICC_CPU1to0_SET_DataReady (*((uint32_t *)(OCM_BASE_ADDR + 8)) = 1)
//#define ICC_CPU1to0_CLR_DataReady (*((uint32_t *)(OCM_BASE_ADDR + 8)) = 0)
//#define ICC_CPU1to0_GET_DataReady (*((uint32_t *)(OCM_BASE_ADDR + 8)))
//#define ICC_CPU1to0_SET_DATA(x)   (*((uint32_t *)(OCM_BASE_ADDR + 12)) = x)
//#define ICC_CPU1to0_GET_DATA      (*((uint32_t *)(OCM_BASE_ADDR + 12)))

#endif // ICC_H
