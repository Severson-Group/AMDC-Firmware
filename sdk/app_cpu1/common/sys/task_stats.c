#include "sys/task_stats.h"
#include "drv/fpga_timer.h"
#include "sys/commands.h"
#include "sys/scheduler.h"
#include <assert.h>
#include <stdint.h>

void tast_stats_init(task_stats_t *stats)
{
    assert(stats);

    task_stats_reset(stats);
}

void task_stats_reset(task_stats_t *stats)
{
    assert(stats);

    stats->is_init = false;

    statistics_clear(&stats->loop_time);
    statistics_clear(&stats->run_time);
}

void task_stats_pre_task(task_stats_t *stats)
{
    assert(stats);

    if (stats->enabled) {
        uint32_t task_start_ticks = fpga_timer_now();

        if (stats->is_init) {
            double loop_time_us = fpga_timer_ticks_to_usec(task_start_ticks - stats->last_fpga_ticks);
            statistics_push(&stats->loop_time, loop_time_us);
        } else {
            stats->is_init = true;
        }

        stats->last_fpga_ticks = task_start_ticks;
    }
}

void task_stats_post_task(task_stats_t *stats)
{
    assert(stats);

    if (stats->enabled && stats->is_init) {
        uint32_t task_end_ticks = fpga_timer_now();

        double run_time_us = fpga_timer_ticks_to_usec(task_end_ticks - stats->last_fpga_ticks);

        statistics_push(&stats->run_time, run_time_us);
    }
}

// ***************************
// Code for printing stats
// ***************************

typedef enum sm_states_e {
    PRINT_HEADER = 0,

    PRINT_LOOP_NUM_SAMPLES,
    PRINT_LOOP_MIN,
    PRINT_LOOP_MAX,
    PRINT_LOOP_MEAN,
    PRINT_LOOP_VARIANCE,

    PRINT_RUN_NUM_SAMPLES,
    PRINT_RUN_MIN,
    PRINT_RUN_MAX,
    PRINT_RUN_MEAN,
    PRINT_RUN_VARIANCE,

    REMOVE_TASK,
} sm_states_e;

typedef struct sm_ctx_t {
    sm_states_e state;
    task_control_block_t tcb;

    task_stats_t *stats;
} sm_ctx_t;

#define SM_UPDATES_PER_SEC (200)
#define SM_INTERVAL_USEC   (USEC_IN_SEC / SM_UPDATES_PER_SEC)

static void state_machine_callback(void *arg)
{
    sm_ctx_t *ctx = (sm_ctx_t *) arg;

    switch (ctx->state) {
    case PRINT_HEADER:
        cmd_resp_printf("Task Stats:\r\n");
        ctx->state = PRINT_LOOP_NUM_SAMPLES;
        break;

    // Loop timings
    // ...
    case PRINT_LOOP_NUM_SAMPLES:
        cmd_resp_printf("Loop Num:\t%d samples\r\n", ctx->stats->loop_time.num_samples);
        ctx->state = PRINT_LOOP_MIN;
        break;

    case PRINT_LOOP_MIN:
        cmd_resp_printf("Loop Min:\t%.2f usec\r\n", ctx->stats->loop_time.min);
        ctx->state = PRINT_LOOP_MAX;
        break;

    case PRINT_LOOP_MAX:
        cmd_resp_printf("Loop Max:\t%.2f usec\r\n", ctx->stats->loop_time.max);
        ctx->state = PRINT_LOOP_MEAN;
        break;

    case PRINT_LOOP_MEAN:
        cmd_resp_printf("Loop Mean:\t%.2f usec\r\n", ctx->stats->loop_time.mean);
        ctx->state = PRINT_LOOP_VARIANCE;
        break;

    case PRINT_LOOP_VARIANCE:
        cmd_resp_printf("Loop Var:\t%.2f usec\r\n", statistics_variance(&ctx->stats->loop_time));
        ctx->state = PRINT_RUN_NUM_SAMPLES;
        break;

    // Run timings
    // ...
    case PRINT_RUN_NUM_SAMPLES:
        cmd_resp_printf("Run Num:\t%d samples\r\n", ctx->stats->run_time.num_samples);
        ctx->state = PRINT_RUN_MIN;
        break;

    case PRINT_RUN_MIN:
        cmd_resp_printf("Run Min:\t%.2f usec\r\n", ctx->stats->run_time.min);
        ctx->state = PRINT_RUN_MAX;
        break;

    case PRINT_RUN_MAX:
        cmd_resp_printf("Run Max:\t%.2f usec\r\n", ctx->stats->run_time.max);
        ctx->state = PRINT_RUN_MEAN;
        break;

    case PRINT_RUN_MEAN:
        cmd_resp_printf("Run Mean:\t%.2f usec\r\n", ctx->stats->run_time.mean);
        ctx->state = PRINT_RUN_VARIANCE;
        break;

    case PRINT_RUN_VARIANCE:
        cmd_resp_printf("Run Var:\t%.2f usec\r\n", statistics_variance(&ctx->stats->run_time));
        ctx->state = REMOVE_TASK;
        break;

    case REMOVE_TASK:
        cmd_resp_printf("\r\n");
        cmd_resp_printf("SUCCESS\r\n\n");
        scheduler_tcb_unregister(&ctx->tcb);
        break;

    default:
        // Can't happen
        HANG;
        break;
    }
}

static sm_ctx_t ctx;

void task_stats_print(task_stats_t *stats)
{
    // Initialize the state machine context
    ctx.state = PRINT_HEADER;
    ctx.stats = stats;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx.tcb, state_machine_callback, &ctx, "printstats", SM_INTERVAL_USEC);
    scheduler_tcb_register(&ctx.tcb);
}
