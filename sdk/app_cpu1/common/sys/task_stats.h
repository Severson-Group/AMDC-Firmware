#ifndef TASK_STATS_H
#define TASK_STATS_H

#include <stdbool.h>
#include <stdint.h>

#include "sys/statistics.h"

typedef struct {
    bool enabled;
    bool is_init;
    uint32_t last_fpga_ticks;

    statistics_t loop_time;
    statistics_t run_time;
} task_stats_t;

void task_stats_init(task_stats_t *stats);
void task_stats_reset(task_stats_t *stats);

void task_stats_print(task_stats_t *stats);

static inline void task_stats_enable(task_stats_t *stats)
{
    stats->enabled = true;
}

static inline void task_stats_disable(task_stats_t *stats)
{
    stats->enabled = false;
}

/**
 * Before the task you are analyzing runs, run this function. If it is enabled it will:
 *
 * 1. Measure the time at the start of the function in FPGA clock ticks
 * 2. Calculate the delta from the last time period in us.
 * 3. Calculate the statistics on the loop timing (time between callings of this function)
 * 4. Save the current time as the last fpga ticks time.
 *
 * To run the runtime (will include the time it takes to calculate statistics).
 * you must run task_stats_post_task() after the task runs.
 */
void task_stats_pre_task(task_stats_t *stats);

/**
 * Run this after the function you are analyzing. If it is enabled.
 *
 * It will calculate the time since task_stats_pre_task() was run.
 * Calculate runtime statistics on them.
 */
void task_stats_post_task(task_stats_t *stats);

#endif // TASK_STATS_H
