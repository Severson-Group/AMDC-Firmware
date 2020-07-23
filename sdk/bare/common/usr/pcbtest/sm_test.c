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
#include <stdlib.h>

static bool test_check_voltage(bool quiet_mode, analog_channel_e channel, float *voltage_out, float goal, float tol);

typedef enum sm_states_e {
    PRINT_HEADER = 0,

    // Verify +/- 10V functionality
    TEST1,
    TEST1_POS_10V_RAIL,
    TEST1_NEG_10V_RAIL,

    TEST2,
    TEST2_SET_PWM,
    TEST2_CHECK_VOUT1,
    TEST2_CHECK_VOUT2,

    END_TESTING,
    PRINT_SMART_DEBUG,
    REMOVE_TASK,
} sm_states_e;

typedef struct sm_ctx_t {
    sm_states_e state;
    task_control_block_t tcb;

    // Top level test config
    bool quiet_mode;
    int num_passes;
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
    int test2_bulk_runs;
    float curr_pwm_duty_random;
    float curr_pwm_goal_voltage1;
    float curr_pwm_goal_voltage2;
    int curr_pwm;
    int num_fails_by_vout[6];
} sm_ctx_t;

#define SM_UPDATES_PER_SEC (400)
#define SM_INTERVAL_USEC   (USEC_IN_SEC / SM_UPDATES_PER_SEC)

static void state_machine_callback(void *arg)
{
    sm_ctx_t *ctx = (sm_ctx_t *) arg;

    switch (ctx->state) {
    case PRINT_HEADER:
    {
        debug_printf("Automated PCB Test: Inverter Port %d%c\r\n", ctx->stack, ctx->tb);
        debug_printf("------------------------------------\r\n");
        debug_printf("Test started...\r\n");
        ctx->state = TEST1;
        break;
    }

    case TEST1:
    case TEST1_POS_10V_RAIL:
    {
        float voltage;
        bool pass = test_check_voltage(ctx->quiet_mode, ANALOG_IN7, &voltage, 10.0, 0.050);
        if (pass) {
            ctx->num_passed_tests++;
        }

        if (!ctx->quiet_mode) {
            debug_printf("+10V rail reads %8.4fV\r\n", voltage);
        }

        if (!pass) {
            // FAIL ... we can't read >0 voltages, so stop testing!
            debug_printf("PCB BROKEN: +10V rail read failure. Stopping test.\r\n\n");
            ctx->state = REMOVE_TASK;
        } else {
            // PASS
            ctx->state = TEST1_NEG_10V_RAIL;
        }

        ctx->num_completed_tests++;
        break;
    }

    case TEST1_NEG_10V_RAIL:
    {
        float voltage;
        bool pass = test_check_voltage(ctx->quiet_mode, ANALOG_IN8, &voltage, -10.0, 0.050);
        if (pass) {
            ctx->num_passed_tests++;
        }

        if (!ctx->quiet_mode) {
            debug_printf("-10V rail reads %8.4fV\r\n", voltage);
        }

        if (!pass) {
            // FAIL ... we can't read <0 voltages, so stop testing!
            debug_printf("PCB BROKEN: -10V rail read failure. Stopping test.\r\n\n");
            ctx->state = REMOVE_TASK;
        } else {
            // PASS
            ctx->state = TEST2;
        }

        ctx->num_completed_tests++;
        break;
    }

    case TEST2:
    {
        ctx->curr_pwm = 0;
        ctx->test2_bulk_runs = 0;

        for (int i = 0; i < 6; i++) {
            ctx->num_fails_by_vout[i] = 0;
        }

        ctx->state = TEST2_SET_PWM;
        break;
    }

    case TEST2_SET_PWM:
    {
        // Initialize PWM for TEST2
        int base = 6 * (ctx->stack - 1);
        if (ctx->tb == 'B') {
            base += 3;
        }

        // Set PWM output
        ctx->curr_pwm_duty_random = (float) rand() / (float) RAND_MAX;
        pwm_set_duty(base + ctx->curr_pwm, ctx->curr_pwm_duty_random);

        // Compute goal voltage based on VDRIVE and PWM parameters
        float pwm_dt = pwm_get_deadtime_ns();
        float pwm_fsw = pwm_get_switching_freq();
        float pwm_dt_percent = (((float) pwm_dt) / 1e9) / (1.0 / pwm_fsw);
        float pwm_dt_delta = pwm_dt_percent * ctx->vdrive;

        float goal1 = (ctx->vdrive * ctx->curr_pwm_duty_random) - pwm_dt_delta;
        float goal2 = -((ctx->vdrive * (1.0 - ctx->curr_pwm_duty_random)) - pwm_dt_delta);
        ctx->curr_pwm_goal_voltage1 = goal1;
        ctx->curr_pwm_goal_voltage2 = goal2;

        ctx->state = TEST2_CHECK_VOUT1;
        break;
    }

    case TEST2_CHECK_VOUT1:
    {
        int vout = 2 * ctx->curr_pwm;
        float voltage;
        bool pass = test_check_voltage(ctx->quiet_mode, vout, &voltage, ctx->curr_pwm_goal_voltage1, 0.100);
        if (pass) {
            ctx->num_passed_tests++;
        } else {
            ctx->num_fails_by_vout[vout]++;
        }

        if (!ctx->quiet_mode) {
            debug_printf("VOUT%d: goal = %8.4fV, meas = %8.4f\r\n", vout + 1, ctx->curr_pwm_goal_voltage1, voltage);
        }

        ctx->num_completed_tests++;
        ctx->state = TEST2_CHECK_VOUT2;
        break;
    }

    case TEST2_CHECK_VOUT2:
    {
        int vout = (2 * ctx->curr_pwm) + 1;
        float voltage;
        bool pass = test_check_voltage(ctx->quiet_mode, vout, &voltage, ctx->curr_pwm_goal_voltage2, 0.100);
        if (pass) {
            ctx->num_passed_tests++;
        } else {
            ctx->num_fails_by_vout[vout]++;
        }

        if (!ctx->quiet_mode) {
            debug_printf("VOUT%d: goal = %8.4fV, meas = %8.4f\r\n", vout + 1, ctx->curr_pwm_goal_voltage2, voltage);
        }

        ctx->num_completed_tests++;
        ctx->state = TEST2_SET_PWM;

        // Update state for next test
        ctx->curr_pwm++;
        if (ctx->curr_pwm >= 3) {
            ctx->curr_pwm = 0;
            ctx->test2_bulk_runs++;

            if (ctx->test2_bulk_runs >= ctx->num_passes) {
                ctx->state = END_TESTING;
            }
        }

        break;
    }

    case END_TESTING:
    {
        // Print status message to user
        debug_printf("%d of %d tests passed\r\n", ctx->num_passed_tests, ctx->num_completed_tests);

        if (!ctx->quiet_mode) {
            for (int i = 0; i < 6; i++) {
                debug_printf("VOUT%d fails: %d\r\n", i + 1, ctx->num_fails_by_vout[i]);
            }
        }

        debug_printf("\n");

        // Restore old PWM settings
        pwm_disable();
        pwm_set_switching_freq(ctx->old_fsw);
        pwm_set_deadtime_ns(ctx->old_dt);
        if (ctx->old_pwm_en) {
            pwm_enable();
        }

        ctx->state = PRINT_SMART_DEBUG;
        break;
    }

    case PRINT_SMART_DEBUG:
    {
        // Give the user some smart debugging info so they don't have to compute this stuff... :)
        debug_printf("Smart Debug\r\n");
        debug_printf("-----------\r\n");

        bool any_issues = false;
        bool pwm_line_is_broken[6] = { 0 };

        // Give info on analog input status
        for (int i = 0; i < 6; i++) {
            if (ctx->num_fails_by_vout[i] > ctx->num_passes / 2) {
                // VOUT is considered broken! > 50% failures
                debug_printf("ANALOG IN%d may be broken\r\n", i + 1);
                pwm_line_is_broken[i] = true;
                any_issues = true;
            }
        }

        // Give info on PWM output status

        int rawbase = 12 * (ctx->stack - 1);
        if (ctx->tb == 'B') {
            rawbase += 6;
        }

        int invnum = 2 * (ctx->stack - 1);
        if (ctx->tb == 'B') {
            invnum += 1;
        }

        for (int i = 0; i < 6; i++) {
            if (pwm_line_is_broken[i]) {
                // PWM output broken
                debug_printf("PWM OUT%d may be broken (INV%d_PWM%d)\r\n", rawbase + i + 1, invnum + 1, i + 1);
            }
        }

        if (any_issues) {
            debug_printf("\n");
            debug_printf("Good luck debugging! :(\r\n\n");
        } else {
            debug_printf("No hardware issues found! :)\r\n\n");
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

int sm_test_start_auto_test(bool quiet_mode, int num_passes, int stack, char tb, double vdrive)
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

    // NOTE: as we increase Fsw, the dead-time percentage
    // increases, so the voltage we see in the tests goes
    // further from ideal...
    //
    // At 300kHz, this is 30x the LPF bandwidth, so we should
    // see fairly DC at the ADC.
    double fsw = MIN(300e3, PWM_MAX_SWITCHING_FREQ_HZ);
    if (pwm_set_switching_freq(fsw) != SUCCESS) {
        return FAILURE;
    }

    if (pwm_set_deadtime_ns(PWM_MIN_DEADTIME_NS) != SUCCESS) {
        return FAILURE;
    }

    pwm_enable();

    // Initialize state machine variables
    ctx.state = PRINT_HEADER;
    ctx.quiet_mode = quiet_mode;
    ctx.num_passes = num_passes;
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

static bool test_check_voltage(bool quiet_mode, analog_channel_e channel, float *voltage_out, float goal, float tol)
{
    float voltage;
    analog_getf(channel, &voltage);

    *voltage_out = voltage;

    bool is_pass = false;
    if (fabsf(voltage - goal) <= tol) {
        is_pass = true;
    }

    if (!quiet_mode) {
        if (is_pass) {
            debug_printf("\tPASS: ");
        } else {
            debug_printf("X\tFAIL: ");
        }
    }

    return is_pass;
}

#endif // APP_PCBTEST
