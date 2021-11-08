#include "sys/scheduler.h"
#include "sys/command_icc.h"
#include "sys/icc.h"
#include "sys/debug.h"
#include <stdint.h>

#define TASK_COMMAND_ICC_UPDATES_PER_SEC (10000)
#define TASK_COMMAND_ICC_INTERVAL_USEC   (USEC_IN_SEC / TASK_COMMAND_ICC_UPDATES_PER_SEC)

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

static void task_command_icc_callback(void *arg)
{
	// Check if CPU0 wrote data
	if (ICC_CPU0to1__GET_CPU0_HasWrittenData) {
		ICC_CPU0to1__CLR_CPU0_HasWrittenData;

		// Read the byte from shared memory
		uint8_t c = (uint8_t) ICC_CPU0to1__GET_DATA;

		// TODO: for testing, echo the data
		debug_printf("%c", c);

		// Tell CPU0 that we are now waiting for more data
		ICC_CPU0to1__SET_CPU1_WaitingForData;
	}
}

void command_icc_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_command_icc_callback, NULL, "cmd_icc", TASK_COMMAND_ICC_INTERVAL_USEC);

    // Register task with scheduler
    scheduler_tcb_register(&tcb);

    // Tell CPU0 we are ready for them to start sending data to us
    ICC_CPU0to1__SET_CPU1_WaitingForData;
}
