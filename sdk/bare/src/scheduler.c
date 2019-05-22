#include "scheduler.h"
#include "../bsp/bsp.h"
#include <stdbool.h>
#include <stdio.h>

// Used to give each task a unique ID
static int next_tcb_id = 0;

// Linked list of all registered tasks
static task_control_block_t *tasks = NULL;

// Incremented every SysTick interrupt to track time
static uint64_t elapsed_usec = 0;

static bool tasks_running = false;
static volatile bool scheduler_idle = false;

void scheduler_timer_isr(void *userParam, uint8_t TmrCtrNumber)
{
	// We should be done running tasks in a time slice before this fires,
	// so if tasks are still running, we consumed too many cycles per slice
	if (tasks_running) {
		printf("ERROR: OVERRUN SCHEDULER TIME QUANTUM!\n");
		io_led_color_t color;
		color.r = 255;
	    color.g = 0;
	    color.b = 0;
		io_led_set(&color);
		HANG;
	}

	elapsed_usec += SYS_TICK_USEC;
	scheduler_idle = false;
}

uint64_t scheduler_get_elapsed_usec(void)
{
	return elapsed_usec;
}

void scheduler_init(void)
{
	printf("SCHED:\tInitializing scheduler...\n");

	// Start system timer for periodic interrupts
	timer_init(scheduler_timer_isr, SYS_TICK_USEC);
	printf("SCHED:\tTasks per second: %d\n", SYS_TICK_FREQ);
}

void scheduler_tcb_init(task_control_block_t *tcb, task_callback_t callback, uint32_t interval_usec)
{
	tcb->id = next_tcb_id++;
	tcb->callback = callback;
	tcb->interval_usec = interval_usec;
	tcb->last_run_usec = 0;
}

void scheduler_tcb_register(task_control_block_t *tcb)
{
	// Base case: there are no tasks in linked list
	if (tasks == NULL) {
		tasks = tcb;
		tasks->next = NULL;
		return;
	}

	// Find end of list
	task_control_block_t *curr = tasks;
	while (curr->next != NULL) curr = curr->next;

	// Append new tcb to end of list
	curr->next = tcb;
	tcb->next = NULL;
}

void scheduler_run(void)
{
	printf("SCHED:\tRunning scheduler...\n");

	// This is the main event loop that runs the device
	while (1) {
		tasks_running = true;

		task_control_block_t *t = tasks;
		while (t != NULL) {
			uint64_t usec_since_last_run = elapsed_usec - t->last_run_usec;

			if (usec_since_last_run >= t->interval_usec) {
				// Time to run this task!
				t->callback();
				t->last_run_usec = elapsed_usec;
			}

			// Go to next task in linked list
			t = t->next;
		}

		tasks_running = false;

		// Wait here until unpaused (i.e. when SysTick fires)
		scheduler_idle = true;
		while (scheduler_idle);
	}
}
