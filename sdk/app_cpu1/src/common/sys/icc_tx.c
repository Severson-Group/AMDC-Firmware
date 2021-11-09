#include "sys/icc_tx.h"
#include "sys/icc.h"
#include "sys/scheduler.h"
#include <stdint.h>

#define TASK_ICC_TX_UPDATES_PER_SEC (10000)
#define TASK_ICC_TX_INTERVAL_USEC   (USEC_IN_SEC / TASK_ICC_TX_UPDATES_PER_SEC)

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

#define BUFFER_LENGTH (100 * 1024)
static uint8_t send_buffer[BUFFER_LENGTH] = { 0 };
static uint32_t idx_writing = 0;
static uint32_t idx_reading = 0;
static uint32_t num_in_buffer = 0;

static char _pop(void)
{
    char ret = send_buffer[idx_reading];
    if (++idx_reading >= BUFFER_LENGTH) {
        idx_reading = 0;
    }

    num_in_buffer -= 1;

    return ret;
}

static void _push(char c)
{
    send_buffer[idx_writing] = c;
    if (++idx_writing >= BUFFER_LENGTH) {
        idx_writing = 0;
    }

    num_in_buffer += 1;
}

static void task_icc_tx_callback(void *arg)
{
    // Make sure we have data to send
    if (num_in_buffer > 0) {

        // Check if CPU0 is ready
        if (ICC_CPU1to0__GET_CPU0_WaitingForData) {
            ICC_CPU1to0__CLR_CPU0_WaitingForData;

            // Send one byte of data over:
            char c = _pop();
            ICC_CPU1to0__SET_DATA(c);

            // Tell CPU0 that we wrote data
            ICC_CPU1to0__SET_CPU1_HasWrittenData;
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
