#ifdef APP_PCBTEST

#include "usr/pcbtest/sm_test.h"
#include "drv/analog.h"
#include "drv/hardware_targets.h"
#include "drv/pwm.h"
#include "sys/debug.h"
#include "sys/scheduler.h"
#include "usr/user_config.h"
#include <math.h>
#include <stdint.h>

typedef enum sm_states_e {
    PRINT_HEADER = 0,

    // Verify +/- 10V functionality
    TEST1,
    TEST1_POS_10V_RAIL,
    TEST1_NEG_10V_RAIL,

    END_TESTING,
    REMOVE_TASK,
} sm_states_e;

typedef struct sm_ctx_t {
    sm_states_e state;
    task_control_block_t tcb;

    // User data
    int stack;
    char tb;

    // Old PWM info
    bool old_pwm_en;
    double old_fsw;
    uint16_t old_dt;

    // Test stats
    int num_completed_tests;
    int num_total_tests;
    int num_passed_tests;
} sm_ctx_t;

#define SM_UPDATES_PER_SEC (1000)
#define SM_INTERVAL_USEC   (USEC_IN_SEC / SM_UPDATES_PER_SEC)

static void state_machine_callback(void *arg)
{
    sm_ctx_t *ctx = (sm_ctx_t *) arg;

    switch (ctx->state) {
    case PRINT_HEADER:
    {
        debug_printf("Automated PCB Test: PS Port %d%c\r\n", ctx->stack, ctx->tb);
        ctx->state = TEST1;
        break;
    }

    case TEST1:
    case TEST1_POS_10V_RAIL:
    {
        float voltage;
        analog_getf(ANALOG_IN7, &voltage);

        float goal = 10.0;
        float tol = 0.100; // V
        if (fabsf((voltage - goal) / goal) > tol) {
            // FAIL
            debug_printf("\tFAIL: +10V rail reads %8.4fV\r\n", voltage);
            ctx->state = END_TESTING;
        } else {
            // PASS
            ctx->num_passed_tests++;
            debug_printf("\tPASS: +10V rail reads %8.4fV\r\n", voltage);
            ctx->state = TEST1_NEG_10V_RAIL;
        }

        ctx->num_completed_tests++;
        break;
    }

    case TEST1_NEG_10V_RAIL:
    {
        float voltage;
        analog_getf(ANALOG_IN8, &voltage);

        float goal = -10.0; // V
        float tol = 0.100;  // V
        if (fabsf((voltage - goal) / goal) > tol) {
            // FAIL
            debug_printf("\tFAIL: -10V rail reads %8.4fV\r\n", voltage);
            ctx->state = END_TESTING;
        } else {
            // PASS
            ctx->num_passed_tests++;
            debug_printf("\tPASS: -10V rail reads %8.4fV\r\n", voltage);
            ctx->state = END_TESTING;
        }

        ctx->num_completed_tests++;
        break;
    }

    case END_TESTING:
    {
        // Print status message to user
        debug_printf("Completed %d of %d tests, %d passed\r\n\n",
                     ctx->num_completed_tests,
                     ctx->num_total_tests,
                     ctx->num_passed_tests);

        // Restore old PWM settings
        pwm_disable();
        pwm_set_switching_freq(ctx->old_fsw);
        pwm_set_deadtime_ns(ctx->old_dt);
        if (ctx->old_pwm_en) {
            pwm_enable();
        }

        ctx->state = REMOVE_TASK;
        break;
    }

    case REMOVE_TASK:
    default:
    {
        scheduler_tcb_unregister(&ctx->tcb);
        break;
    }
    }
}

static sm_ctx_t ctx;

int sm_test_start_auto_test(int stack, char tb)
{
    // Ensure test is not already running
    if (scheduler_tcb_is_registered(&ctx.tcb)) {
        return FAILURE;
    }

    // Store old PWM settings so we can revert to them later
    ctx.old_pwm_en = pwm_is_enabled();
    ctx.old_fsw = pwm_get_switching_freq();
    ctx.old_dt = pwm_get_deadtime_ns();

    // Update PWM to as fast as possible
    pwm_disable();

    if (pwm_set_switching_freq(PWM_MAX_SWITCHING_FREQ_HZ) != SUCCESS) {
        return FAILURE;
    }

    if (pwm_set_deadtime_ns(PWM_MIN_DEADTIME_NS) != SUCCESS) {
        return FAILURE;
    }

    pwm_enable();

    // Initialize state machine variables
    ctx.state = PRINT_HEADER;
    ctx.stack = stack;
    ctx.tb = tb;
    ctx.num_total_tests = 2;
    ctx.num_completed_tests = 0;
    ctx.num_passed_tests = 0;

    // Register state machine
    scheduler_tcb_init(&ctx.tcb, state_machine_callback, &ctx, "testsm", SM_INTERVAL_USEC);
    int err = scheduler_tcb_register(&ctx.tcb);

    return err;
}

#endif // APP_PCBTEST
