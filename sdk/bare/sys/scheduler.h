#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#include "../sys/defines.h"


// SysTick
//
// The basic time quantum is defined to be SYS_TICK_FREQ (10kHz)
// At 666.6 MHz DSP clock, we have 66.6k cycles per time slice,
// meaning all tasks combined have to consume <= 66.6k cycles
//
#define SYS_TICK_FREQ	(10000) // Hz
#define SYS_TICK_USEC	(SEC_TO_USEC(1) / SYS_TICK_FREQ)

//
// Callback into application when task is run:
//
typedef void (*task_callback_t)(void *);

//
// TCB of each task
//
typedef struct task_control_block_t {
	int id;
	const char *name;
	uint8_t registered;
	task_callback_t callback;
	void *callback_arg;
	uint64_t interval_usec;
	uint64_t last_run_usec;
	struct task_control_block_t *next;
} task_control_block_t;


void scheduler_init(void);
void scheduler_run(void);

void scheduler_tcb_init(task_control_block_t *tcb, task_callback_t callback,
		void *callback_arg, const char *name, uint32_t interval_usec);
void scheduler_tcb_register(task_control_block_t *tcb);
void scheduler_tcb_unregister(task_control_block_t *tcb);
uint8_t scheduler_tcb_is_registered(task_control_block_t *tcb);

uint64_t scheduler_get_elapsed_usec(void);

#endif // SCHEDULER_H
