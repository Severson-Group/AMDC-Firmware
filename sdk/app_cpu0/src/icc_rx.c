#include "icc_rx.h"
#include "icc.h"
#include "socket_manager.h"

void icc_rx_process(void)
{
	static const int MAX_NUM_BYTES_TO_TRY = 1024;

	// Try to pull out the oldest MAX_NUM_BYTES_TO_TRY bytes from the shared FIFO from CPU1
	//
	// If there are less than MAX_NUM_BYTES_TO_TRY bytes in the FIFO, this code will simply
	// pull out everything and return.
	for (int i = 0; i < MAX_NUM_BYTES_TO_TRY; i++) {
		// Check if there are any bytes in the FIFO
		if (ICC_CPU1to0_CH0__GET_ProduceCount - ICC_CPU1to0_CH0__GET_ConsumeCount == 0) {
			// Shared buffer is empty
			return;
		}

		// Read the oldest byte available
		uint8_t *sharedBuffer = ICC_CPU1to0_CH0__BufferBaseAddr;
		uint8_t c = sharedBuffer[ICC_CPU1to0_CH0__GET_ConsumeCount % ICC_BUFFER_SIZE];

		// Increment the consume count
		ICC_CPU1to0_CH0__SET_ConsumeCount(ICC_CPU1to0_CH0__GET_ConsumeCount + 1);

		// Send the byte out to the active TCP sockets
		socket_manager_broadcast_ascii_cmd_byte(c);
	}
}
