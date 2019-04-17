#include "scheduler.h"
#include "../bsp/bsp.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct task_t {
	task_callback_t callback;
	uint64_t interval_usec;
	uint64_t last_run_usec;
} task_t;

// Incremented every SysTick interrupt to track time
static uint64_t elapsed_usec = 0;

// Holds all our registered tasks running on the device
static task_t tasks[MAX_NUM_TASKS];
static uint8_t num_tasks = 0;

static bool task_running = false;
static volatile bool scheduler_paused = false;

void scheduler_timer_isr(void *userParam, uint8_t TmrCtrNumber)
{
	// We should be done running tasks in a time slice before this fires,
	// so if a task is still running, we consumed too many cycles per slice
	if (task_running) {
		printf("ERROR: OVERRUN SCHEDULER TIME QUANTUM!\n");
	}

	elapsed_usec += SYS_TICK_USEC;
	scheduler_paused = false;

}

void scheduler_init(void)
{
	// Start system timer for periodic interrupts
	timer_init(scheduler_timer_isr, SYS_TICK_USEC);
}

void scheduler_register_task(task_callback_t callback, uint32_t interval_usec)
{
	// If we are full of tasks, die here
	if (num_tasks >= MAX_NUM_TASKS) { HANG; }

	// Append task to task list
	tasks[num_tasks].callback = callback;
	tasks[num_tasks].interval_usec = interval_usec;
	tasks[num_tasks].last_run_usec = 0;

	num_tasks++;
}

void scheduler_run(void)
{
	// This is the main event loop that runs the device
	while (1) {
		for (uint8_t i = 0; i < num_tasks; i++) {
			task_t *t = &tasks[i];

			uint64_t usec_since_last_run = elapsed_usec - t->last_run_usec;

			if (usec_since_last_run >= t->interval_usec) {
				// Time to run this task!
				task_running = true;
				t->callback();
				task_running = false;

				t->last_run_usec = elapsed_usec;
			}
		}

		// Wait here until unpaused (i.e. when SysTick fires)
		scheduler_paused = true;
		while (scheduler_paused);
	}
}