#include "sys/scheduler.h"
#include "drv/hardware_targets.h"
#include "drv/led.h"
#include "drv/timing_manager.h"
#include "drv/watchdog.h"
#include "xil_printf.h"
#include <stdbool.h>
#include <stdio.h>

// Used to give each task a unique ID
static int next_tcb_id = 0;

// Linked list of all registered tasks
static task_control_block_t *tasks = NULL;

// For debugging, this variable is set to point
// at the currently running task
static task_control_block_t *running_task = NULL;

// Incremented every SysTick interrupt to track time
static volatile double elapsed_usec = 0;

static bool tasks_running = false;
static volatile bool scheduler_idle = false;

/*
 * This function is called only from the timing manager ISR, and thus only
 * runs in the context of the ISR.
 *
 * The elapsed time since the last ISR call is recorded and the state of the
 * the scheduler is moved out of idle to run the tasks.
 */
void scheduler_tick(void)
{
#if USER_CONFIG_ENABLE_TIME_QUANTUM_CHECKING == 1
    // We should be done running tasks in a time slice before this fires,
    // so if tasks are still running, we consumed too many cycles per slice
    if (tasks_running) {
        // For the first callback after enabling the sensors in the timing manager,
        // the overrun check will occur due to the fast speed of the sensor quickly
        // calling the ISR before it aligns with the normal trigger rate
        //
        // Thus, we will make sure to only check this when the elapsed time
        // since the last call is larger than twice the maximum sensor time
        if (timing_manager_get_tick_delta() >= (2 * TM_MAX_DEFAULT_SENSOR_TIME)) {
            // Use raw printf so this goes directly to the UART device
            xil_printf("ERROR: OVERRUN SCHEDULER TIME QUANTUM!\r\n");
            xil_printf("ERROR: CURRENT TASK IS %s\n", running_task->name);

            led_set_color(0, LED_COLOR_RED);
            led_set_color(1, LED_COLOR_RED);
            led_set_color(2, LED_COLOR_RED);
            led_set_color(3, LED_COLOR_RED);

            // Hang here so the user can debug why the code took so long
            // and overran the time slice! See the `running_task` variable.
            while (1) {
            }
        }
    }
#endif // USER_CONFIG_ENABLE_TIME_QUANTUM_CHECKING
    elapsed_usec += timing_manager_get_tick_delta();
    scheduler_idle = false; // run task
}

double scheduler_get_elapsed_usec(void)
{
    return elapsed_usec;
}

void scheduler_tcb_init(
    task_control_block_t *tcb, task_callback_t callback, void *callback_arg, const char *name, uint32_t interval_usec)
{
    tcb->id = next_tcb_id++;
    tcb->name = name;
    tcb->callback = callback;
    tcb->callback_arg = callback_arg;
    tcb->interval_usec = interval_usec;
    tcb->last_run_usec = 0;

#if USER_CONFIG_ENABLE_TASK_STATISTICS_BY_DEFAULT == 1
    tcb->stats.enabled = true;
#else
    tcb->stats.enabled = false;
#endif // USER_CONFIG_ENABLE_TASK_STATISTICS_BY_DEFAULT
}

int scheduler_tcb_register(task_control_block_t *tcb)
{
    // Don't let clients re-register their tcb
    if (tcb->is_registered) {
        return FAILURE;
    }

    // Mark as registered
    tcb->is_registered = true;

    if (tasks == NULL) {
        // There are no tasks in linked list
        tasks = tcb;
        tasks->next = NULL;
    } else {
        // Find end of list
        task_control_block_t *curr = tasks;
        while (curr->next != NULL) {
            curr = curr->next;
        }

        // Append new tcb to end of list
        curr->next = tcb;
        tcb->next = NULL;
    }

    return SUCCESS;
}

int scheduler_tcb_register_high_priority(task_control_block_t *tcb)
{
    // Don't let clients re-register their tcb
    if (tcb->is_registered) {
        return FAILURE;
    }

    // Mark as registered
    tcb->is_registered = true;

    if (tasks == NULL) {
        // There are no tasks in linked list
        tasks = tcb;
        tasks->next = NULL;
    } else {
        // Put new tcb at front of linked list
        tcb->next = tasks;
        tasks = tcb;
    }

    return SUCCESS;
}

int scheduler_tcb_unregister(task_control_block_t *tcb)
{
    // Don't let clients unregister their already unregistered tcb
    if (!tcb->is_registered) {
        return FAILURE;
    }

    // Make sure list isn't empty
    if (tasks == NULL) {
        return FAILURE;
    }

    // Mark as unregistered
    tcb->is_registered = false;

    // Special case: trying to remove the head of the list
    if (tasks->id == tcb->id) {
        tasks = tasks->next;
        return SUCCESS;
    }

    // Now we know that 'tcb' to remove is NOT first node

    task_control_block_t *prev = NULL;
    task_control_block_t *curr = tasks;

    // Find spot in linked list to remove tcb
    while (curr->id != tcb->id) {
        prev = curr;
        curr = curr->next;
    }

    // 'curr' is now the one we want to remove!

    prev->next = curr->next;

    return SUCCESS;
}

bool scheduler_tcb_is_registered(task_control_block_t *tcb)
{
    return tcb->is_registered;
}

void scheduler_run(void)
{
    printf("SCHED:\tRunning scheduler...\n");

    // This is the main event loop that runs the device
    while (1) {
        double my_elapsed_usec = elapsed_usec;
        tasks_running = true;

        task_control_block_t *t = tasks;
        while (t != NULL) {
            double usec_since_last_run = my_elapsed_usec - t->last_run_usec;

            if (usec_since_last_run >= t->interval_usec) {
                // Time to run this task!
                task_stats_pre_task(&t->stats);
                running_task = t;
                t->callback(t->callback_arg);
                running_task = NULL;
                task_stats_post_task(&t->stats);

                t->last_run_usec = my_elapsed_usec;
            }

            // Go to next task in linked list
            t = t->next;
        }

        tasks_running = false;

        // Wait here until unpaused
        scheduler_idle = true;
        while (scheduler_idle) {
        }

#if USER_CONFIG_ENABLE_WATCHDOG == 1
        // Reset the watchdog timer after timer fires
        watchdog_reset();
#endif
    }
}
