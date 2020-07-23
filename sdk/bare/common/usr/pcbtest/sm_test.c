#ifdef APP_PCBTEST

#include "usr/pcbtest/sm_test.h"
#include "drv/analog.h"
#include "drv/hardware_targets.h"
#include "drv/pwm.h"
#include "sys/debug.h"
#include "sys/scheduler.h"
#include "sys/util.h"
#include "usr/user_config.h"
#include <math.h>
#include <stdint.h>

static bool test_check_voltage(analog_channel_e channel, float *voltage_out, float goal, float tol);

typedef enum sm_states_e {
    PRINT_HEADER = 0,

    // Verify +/- 10V functionality
    TEST1,
    TEST1_POS_10V_RAIL,
    TEST1_NEG_10V_RAIL,

    TEST2,
    TEST2_VOUT,

    END_TESTING,
    REMOVE_TASK,
} sm_states_e;

typedef struct sm_ctx_t {
    sm_states_e state;
    task_control_block_t tcb;

    // Top level test config
    int stack;
    char tb;
    float vdrive;

    // Old PWM info
    bool old_pwm_en;
    double old_fsw;
    uint16_t old_dt;

    // Test stats
    int num_completed_tests;
    int num_passed_tests;

    // TEST2 data
    int curr_vout;
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
        bool pass = test_check_voltage(ANALOG_IN7, &voltage, 10.0, 0.050);
        if (pass) {
            ctx->num_passed_tests++;
        }

        debug_printf("+10V rail reads %8.4fV\r\n", voltage);

        ctx->state = TEST1_NEG_10V_RAIL;
        ctx->num_completed_tests++;
        break;
    }

    case TEST1_NEG_10V_RAIL:
    {
        float voltage;
        bool pass = test_check_voltage(ANALOG_IN8, &voltage, -10.0, 0.050);
        if (pass) {
            ctx->num_passed_tests++;
        }

        debug_printf("-10V rail reads %8.4fV\r\n", voltage);

        ctx->state = TEST2;
        ctx->num_completed_tests++;
        break;
    }

    case TEST2:
    {
        // Initialize PWM for TEST2
        int base = 6 * ctx->stack;
        if (ctx->tb == 'B') {
            base += 3;
        }

        pwm_set_duty(base + PWM_OUT1, 0.5);
        pwm_set_duty(base + PWM_OUT2, 0.5);
        pwm_set_duty(base + PWM_OUT3, 0.5);

        ctx->curr_vout = 0;
        ctx->state = TEST2_VOUT;
        break;
    }

    case TEST2_VOUT:
    {
        float goal = 0.5 * ctx->vdrive;
        if (ctx->curr_vout % 2 == 1) {
            goal *= -1;
        }

        float voltage;
        bool pass = test_check_voltage(ctx->curr_vout, &voltage, goal, 0.100);
        if (pass) {
            ctx->num_passed_tests++;
        }

        debug_printf("VOUT%d: goal = %8.4fV, meas = %8.4f\r\n", ctx->curr_vout, goal, voltage);

        ctx->num_completed_tests++;

        ctx->curr_vout++;
        if (ctx->curr_vout >= 6) {
            ctx->state = END_TESTING;
        }

        break;
    }

    case END_TESTING:
    {
        // Print status message to user
        debug_printf("Done testing: %d of %d tests passed\r\n\n", ctx->num_passed_tests, ctx->num_completed_tests);

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

int sm_test_start_auto_test(int stack, char tb, double vdrive)
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

    double fsw = MIN(400e3, PWM_MAX_SWITCHING_FREQ_HZ);
    if (pwm_set_switching_freq(fsw) != SUCCESS) {
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
    ctx.vdrive = vdrive;
    ctx.num_completed_tests = 0;
    ctx.num_passed_tests = 0;

    // Register state machine
    scheduler_tcb_init(&ctx.tcb, state_machine_callback, &ctx, "testsm", SM_INTERVAL_USEC);
    int err = scheduler_tcb_register(&ctx.tcb);

    return err;
}

// Helper function

static bool test_check_voltage(analog_channel_e channel, float *voltage_out, float goal, float tol)
{
    float voltage;
    analog_getf(channel, &voltage);

    *voltage_out = voltage;

    bool is_pass = false;
    if (fabsf(voltage - goal) <= tol) {
        is_pass = true;
    }

    if (is_pass) {
        debug_printf("\tPASS: ");
    } else {
        debug_printf("\tFAIL: ");
    }

    return is_pass;
}

#endif // APP_PCBTEST
