#include "sys/icc_tx.h"
#include "sys/icc.h"
#include "sys/scheduler.h"
#include "xil_io.h"
#include <stdint.h>

#define TASK_ICC_TX_UPDATES_PER_SEC (10000)
#define TASK_ICC_TX_INTERVAL_USEC   (USEC_IN_SEC / TASK_ICC_TX_UPDATES_PER_SEC)

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

#define BUFFER_LENGTH (10 * 1024)
static uint8_t send_buffer[BUFFER_LENGTH] = { 0 };
static uint32_t idx_writing = 0;
static uint32_t idx_reading = 0;
static uint32_t num_in_buffer = 0;

static inline char _pop(void)
{
    char ret = send_buffer[idx_reading];
    if (++idx_reading >= BUFFER_LENGTH) {
        idx_reading = 0;
    }

    num_in_buffer -= 1;

    return ret;
}

int task_icc_tx_get_buffer_space_available(void)
{
    return BUFFER_LENGTH - num_in_buffer;
}

static inline void _push(char c)
{
    send_buffer[idx_writing] = c;
    if (++idx_writing >= BUFFER_LENGTH) {
        idx_writing = 0;
    }

    num_in_buffer += 1;
}

static void task_icc_tx_callback(void *arg)
{
    // Try to give all our ringbuf TCP/IP data to CPU0
    //
    // Bounded up to MAX_NUM_BYTES_TO_TRY bytes
    //
    // If we ever find that the ICC shared FIFO gets full,
    // we'll just stop and wait until next time.

    static const int MAX_NUM_BYTES_TO_TRY = 256;

    int num_sent = 0;

    while (num_in_buffer > 0) {
        if (ICC_CPU1to0_CH0__GET_ProduceCount - ICC_CPU1to0_CH0__GET_ConsumeCount == ICC_BUFFER_SIZE) {
            // Shared buffer is full
            // Return and try again in the next time slice
            return;
        }

        // Write one byte to the sharedBuffer BEFORE incrementing produceCount
        uint8_t *sharedBuffer = ICC_CPU1to0_CH0__BufferBaseAddr;
        char c = _pop();
        sharedBuffer[ICC_CPU1to0_CH0__GET_ProduceCount % ICC_BUFFER_SIZE] = c;

        // Memory barrier required here to ensure update of the sharedBuffer is
        // visible to the other core before the update of produceCount
        //
        // Nathan thinks we don't actually have to do this since we turned off
        // caching on the OCM, so the write should flush immediately, but,
        // I might be wrong and it could be stuck in some pipeline...
        // Just to be safe, we'll insert a DMB instruction.
        dmb();

        // Increment produce count
        ICC_CPU1to0_CH0__SET_ProduceCount(ICC_CPU1to0_CH0__GET_ProduceCount + 1);

        num_sent++;
        if (num_sent > MAX_NUM_BYTES_TO_TRY) {
            // We sent enough during this time slice!
            // Stop now.
            return;
        }
    }
}

void icc_tx_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_icc_tx_callback, NULL, "icc_tx", TASK_ICC_TX_INTERVAL_USEC);

    // Register task with scheduler
    scheduler_tcb_register(&tcb);
}

void icc_tx_append_char_to_fifo(char c)
{
    _push(c);
}

void icc_tx_log_stream(int socket_id, int var_slot, uint32_t ts, uint32_t data)
{
    // Create packet of data to send to host
    //
    // Packet format: HEADER, VAR_SLOT, TS, DATA, FOOTER
    // where each entry is 32 bits
    //
    // Total packet length: 5*4 = 20 bytes
    //
    // HEADER = 0x11111111
    // FOOTER = 0x22222222

    static const int packet_len = 20;
    uint8_t bytes_to_send[20] = { 0 };

    uint32_t *ptr_header = (uint32_t *) &bytes_to_send[0];
    uint32_t *ptr_var_slot = (uint32_t *) &bytes_to_send[4];
    uint32_t *ptr_ts = (uint32_t *) &bytes_to_send[8];
    uint32_t *ptr_data = (uint32_t *) &bytes_to_send[12];
    uint32_t *ptr_footer = (uint32_t *) &bytes_to_send[16];

    *ptr_header = 0x11111111;
    *ptr_var_slot = var_slot;
    *ptr_ts = ts;
    *ptr_data = data;
    *ptr_footer = 0x22222222;

    for (int i = 0; i < packet_len; i++) {
        uint8_t d = bytes_to_send[i];

        int buffer_full = 0;
        switch (socket_id) {
        case 1:
            buffer_full = ICC_CPU1to0_CH1__IsBufferFull;
            break;
        case 2:
            buffer_full = ICC_CPU1to0_CH2__IsBufferFull;
            break;
        case 3:
            buffer_full = ICC_CPU1to0_CH3__IsBufferFull;
            break;
        case 4:
            buffer_full = ICC_CPU1to0_CH4__IsBufferFull;
            break;
        default:
            // Invalid socket
            // Return from function and fail silently
            return;
        }

        if (buffer_full) {
            // Shared buffer is full

            // This is a silent error and will cause dropped data!
            // For testing, print Q to UART terminal...
            xil_printf("Q");
            return;
        }

        // Write one byte to the sharedBuffer BEFORE incrementing produceCount
        uint8_t *sharedBuffer;
        switch (socket_id) {
        case 1:
            sharedBuffer = ICC_CPU1to0_CH1__BufferBaseAddr;
            sharedBuffer[ICC_CPU1to0_CH1__GET_ProduceCount % ICC_BUFFER_SIZE] = d;
            break;
        case 2:
            sharedBuffer = ICC_CPU1to0_CH2__BufferBaseAddr;
            sharedBuffer[ICC_CPU1to0_CH2__GET_ProduceCount % ICC_BUFFER_SIZE] = d;
            break;
        case 3:
            sharedBuffer = ICC_CPU1to0_CH3__BufferBaseAddr;
            sharedBuffer[ICC_CPU1to0_CH3__GET_ProduceCount % ICC_BUFFER_SIZE] = d;
            break;
        case 4:
            sharedBuffer = ICC_CPU1to0_CH4__BufferBaseAddr;
            sharedBuffer[ICC_CPU1to0_CH4__GET_ProduceCount % ICC_BUFFER_SIZE] = d;
            break;
        default:
            // Not a valid socket...
            // Just silently ignore for now
            break;
        }

        // Memory barrier required here to ensure update of the sharedBuffer is
        // visible to the other core before the update of produceCount
        //
        // Nathan thinks we don't actually have to do this since we turned off
        // caching on the OCM, so the write should flush immediately, but,
        // I might be wrong and it could be stuck in some pipeline...
        // Just to be safe, we'll insert a DMB instruction.
        dmb();

        // Increment produce count
        switch (socket_id) {
        case 1:
            ICC_CPU1to0_CH1__INC_ProduceCount;
            break;
        case 2:
            ICC_CPU1to0_CH2__INC_ProduceCount;
            break;
        case 3:
            ICC_CPU1to0_CH3__INC_ProduceCount;
            break;
        case 4:
            ICC_CPU1to0_CH4__INC_ProduceCount;
            break;
        default:
            // Not a valid socket...
            // Just silently ignore for now
            break;
        }
    }
}
