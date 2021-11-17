#include "icc.h"
#include <stdint.h>

void icc_init(void)
{
	// Clear data channel from CPU0 to CPU1
	ICC_CPU0to1_CH0__SET_ProduceCount(0);
	ICC_CPU0to1_CH0__SET_ConsumeCount(0);
	uint8_t *sharedBuffer1 = ICC_CPU0to1_CH0__BufferBaseAddr;
	for (int i = 0; i < ICC_BUFFER_SIZE; i++) {
		sharedBuffer1[i] = 0;
	}

	// Clear data channel from CPU1 to CPU0
	ICC_CPU1to0_CH0__SET_ProduceCount(0);
	ICC_CPU1to0_CH0__SET_ConsumeCount(0);
	uint8_t *sharedBuffer2 = ICC_CPU1to0_CH0__BufferBaseAddr;
	for (int i = 0; i < ICC_BUFFER_SIZE; i++) {
		sharedBuffer2[i] = 0;
	}
}
