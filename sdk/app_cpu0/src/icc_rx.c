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

            // Flush TCP packets
            socket_manager_flush_ascii_cmd_sockets();
            return;
        }

        // Stop if the TCP buffers are full
        // By default, socket 0 is ascii_cmd
        int socket_id = 0;
        if (socket_manager_tcp_sndbuf_space_available(socket_id) == 0) {
            socket_manager_flush_ascii_cmd_sockets();
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

static void _log_stream_ch1(int socket_id);
static void _log_stream_ch2(int socket_id);
static void _log_stream_ch3(int socket_id);
static void _log_stream_ch4(int socket_id);

void icc_rx_process_log_stream(void)
{
    _log_stream_ch1(1);
    _log_stream_ch2(2);
    _log_stream_ch3(3);
    _log_stream_ch4(4);
}

static void _log_stream_ch1(int socket_id)
{
    static const int MAX_NUM_BYTES_TO_TRY = 1024;

    // Try to pull out the oldest MAX_NUM_BYTES_TO_TRY bytes from the shared FIFO from CPU1
    //
    // If there are less than MAX_NUM_BYTES_TO_TRY bytes in the FIFO, this code will simply
    // pull out everything and return.
    for (int i = 0; i < MAX_NUM_BYTES_TO_TRY; i++) {
        // Check if there are any bytes in the FIFO
        if (ICC_CPU1to0_CH1__IsBufferEmpty) {
            // Shared buffer is empty

            // Flush TCP packets
            socket_manager_flush_log_socket(socket_id);
            return;
        }

        // Read the oldest byte available
        uint8_t *sharedBuffer = ICC_CPU1to0_CH1__BufferBaseAddr;
        uint8_t c = sharedBuffer[ICC_CPU1to0_CH1__GET_ConsumeCount % ICC_BUFFER_SIZE];

        // Increment the consume count
        ICC_CPU1to0_CH1__INC_ConsumeCount;

        // Send the byte out to the active TCP sockets
        socket_manager_log_socket_send(socket_id, c);
    }
}

static void _log_stream_ch2(int socket_id)
{
    static const int MAX_NUM_BYTES_TO_TRY = 1024;

    // Try to pull out the oldest MAX_NUM_BYTES_TO_TRY bytes from the shared FIFO from CPU1
    //
    // If there are less than MAX_NUM_BYTES_TO_TRY bytes in the FIFO, this code will simply
    // pull out everything and return.
    for (int i = 0; i < MAX_NUM_BYTES_TO_TRY; i++) {
        // Check if there are any bytes in the FIFO
        if (ICC_CPU1to0_CH2__IsBufferEmpty) {
            // Shared buffer is empty

            // Flush TCP packets
            socket_manager_flush_log_socket(socket_id);
            return;
        }

        // Read the oldest byte available
        uint8_t *sharedBuffer = ICC_CPU1to0_CH2__BufferBaseAddr;
        uint8_t c = sharedBuffer[ICC_CPU1to0_CH2__GET_ConsumeCount % ICC_BUFFER_SIZE];

        // Increment the consume count
        ICC_CPU1to0_CH2__INC_ConsumeCount;

        // Send the byte out to the active TCP sockets
        socket_manager_log_socket_send(socket_id, c);
    }
}

static void _log_stream_ch3(int socket_id)
{
    static const int MAX_NUM_BYTES_TO_TRY = 1024;

    // Try to pull out the oldest MAX_NUM_BYTES_TO_TRY bytes from the shared FIFO from CPU1
    //
    // If there are less than MAX_NUM_BYTES_TO_TRY bytes in the FIFO, this code will simply
    // pull out everything and return.
    for (int i = 0; i < MAX_NUM_BYTES_TO_TRY; i++) {
        // Check if there are any bytes in the FIFO
        if (ICC_CPU1to0_CH3__IsBufferEmpty) {
            // Shared buffer is empty

            // Flush TCP packets
            socket_manager_flush_log_socket(socket_id);
            return;
        }

        // Read the oldest byte available
        uint8_t *sharedBuffer = ICC_CPU1to0_CH3__BufferBaseAddr;
        uint8_t c = sharedBuffer[ICC_CPU1to0_CH3__GET_ConsumeCount % ICC_BUFFER_SIZE];

        // Increment the consume count
        ICC_CPU1to0_CH3__INC_ConsumeCount;

        // Send the byte out to the active TCP sockets
        socket_manager_log_socket_send(socket_id, c);
    }
}

static void _log_stream_ch4(int socket_id)
{
    static const int MAX_NUM_BYTES_TO_TRY = 1024;

    // Try to pull out the oldest MAX_NUM_BYTES_TO_TRY bytes from the shared FIFO from CPU1
    //
    // If there are less than MAX_NUM_BYTES_TO_TRY bytes in the FIFO, this code will simply
    // pull out everything and return.
    for (int i = 0; i < MAX_NUM_BYTES_TO_TRY; i++) {
        // Check if there are any bytes in the FIFO
        if (ICC_CPU1to0_CH4__IsBufferEmpty) {
            // Shared buffer is empty

            // Flush TCP packets
            socket_manager_flush_log_socket(socket_id);
            return;
        }

        // Read the oldest byte available
        uint8_t *sharedBuffer = ICC_CPU1to0_CH4__BufferBaseAddr;
        uint8_t c = sharedBuffer[ICC_CPU1to0_CH4__GET_ConsumeCount % ICC_BUFFER_SIZE];

        // Increment the consume count
        ICC_CPU1to0_CH4__INC_ConsumeCount;

        // Send the byte out to the active TCP sockets
        socket_manager_log_socket_send(socket_id, c);
    }
}
