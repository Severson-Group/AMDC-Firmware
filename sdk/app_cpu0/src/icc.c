#include "icc.h"
#include <stdint.h>

void icc_init(void)
{
    uint8_t *sharedBuffer;

    // Clear data channel from CPU0 to CPU1
    ICC_CPU0to1_CH0__SET_ProduceCount(0);
    ICC_CPU0to1_CH0__SET_ConsumeCount(0);
    sharedBuffer = ICC_CPU0to1_CH0__BufferBaseAddr;
    for (int i = 0; i < ICC_BUFFER_SIZE; i++) {
        sharedBuffer[i] = 0;
    }

    // Clear data channel from CPU1 to CPU0
    ICC_CPU1to0_CH0__SET_ProduceCount(0);
    ICC_CPU1to0_CH0__SET_ConsumeCount(0);
    sharedBuffer = ICC_CPU1to0_CH0__BufferBaseAddr;
    for (int i = 0; i < ICC_BUFFER_SIZE; i++) {
        sharedBuffer[i] = 0;
    }

    // =========
    // Logging streams
    // =========

    ICC_CPU1to0_CH1__SET_ProduceCount(0);
    ICC_CPU1to0_CH1__SET_ConsumeCount(0);
    sharedBuffer = ICC_CPU1to0_CH1__BufferBaseAddr;
    for (int i = 0; i < ICC_BUFFER_SIZE; i++) {
        sharedBuffer[i] = 0;
    }

    ICC_CPU1to0_CH2__SET_ProduceCount(0);
    ICC_CPU1to0_CH2__SET_ConsumeCount(0);
    sharedBuffer = ICC_CPU1to0_CH2__BufferBaseAddr;
    for (int i = 0; i < ICC_BUFFER_SIZE; i++) {
        sharedBuffer[i] = 0;
    }

    ICC_CPU1to0_CH3__SET_ProduceCount(0);
    ICC_CPU1to0_CH3__SET_ConsumeCount(0);
    sharedBuffer = ICC_CPU1to0_CH3__BufferBaseAddr;
    for (int i = 0; i < ICC_BUFFER_SIZE; i++) {
        sharedBuffer[i] = 0;
    }

    ICC_CPU1to0_CH4__SET_ProduceCount(0);
    ICC_CPU1to0_CH4__SET_ConsumeCount(0);
    sharedBuffer = ICC_CPU1to0_CH4__BufferBaseAddr;
    for (int i = 0; i < ICC_BUFFER_SIZE; i++) {
        sharedBuffer[i] = 0;
    }
}
