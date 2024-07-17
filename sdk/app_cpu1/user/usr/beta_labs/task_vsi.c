#ifdef APP_BETA_LABS

#include "usr/beta_labs/task_vsi.h"
#include "drv/pwm.h"
#include "sys/scheduler.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

// Forward declarations
static double omega_ramp_fcn(double *current, double *setpoint, double ramprate);

static uint8_t VSI_leg1;
static uint8_t VSI_leg2;
static uint8_t VSI_leg3;
static double VSI_Vpercent = 0;
static double VSI_omega = 0;
static double theta = 0;

static double VSI_omega_ramp = 0;
static double VSI_old_Vpercent = 0;
static double VSI_old_omega = 0;

static double VSI_R;
static double VSI_V0;

static task_control_block_t tcb;

static void _clear_state(void)
{
    task_vsi_set(0, 0, 0);
    pwm_set_duty(VSI_leg1, 0.0);
    pwm_set_duty(VSI_leg2, 0.0);
    pwm_set_duty(VSI_leg3, 0.0);
}

uint8_t task_vsi_is_inited(void)
{
    return scheduler_tcb_is_registered(&tcb);
}

void task_vsi_init(void)
{
    // Register scheduler task
    scheduler_tcb_init(&tcb, task_vsi_callback, NULL, "vsi", TASK_VSI_INTERVAL_USEC);
    scheduler_tcb_register(&tcb);

    // Clear output
    _clear_state();
}

void task_vsi_deinit(void)
{
    // Unregister scheduler task
    scheduler_tcb_unregister(&tcb);

    // Clear output
    _clear_state();
}

void task_vsi_callback(void *arg)
{
    double update_da = omega_ramp_fcn(&VSI_old_omega, &VSI_omega, VSI_omega_ramp);

    theta += update_da;
    if (theta > 6.283185307179586)
        theta -= 6.283185307179586;

    double v = (VSI_R * VSI_old_omega) + VSI_V0;

    double percent1 = v * cos(theta);
    double percent2 = v * cos(theta - PI23);
    double percent3 = v * cos(theta + PI23);

    pwm_set_duty(VSI_leg1, (1 + percent1) / 2.0);
    pwm_set_duty(VSI_leg2, (1 + percent2) / 2.0);
    pwm_set_duty(VSI_leg3, (1 + percent3) / 2.0);
}

void task_vsi_set_legs(int phAIdx, int phBIdx, int phCIdx)
{
    VSI_leg1 = phAIdx;
    VSI_leg2 = phBIdx;
    VSI_leg3 = phCIdx;
}

void task_vsi_set(int vPercent, int freq, double ramptime)
{
    // Convert voltage percent input to double percentage
    VSI_old_Vpercent = VSI_Vpercent;
    VSI_Vpercent = vPercent / 100.0;

    // Convert freq input to rad/sec
    VSI_old_omega = VSI_omega;
    VSI_omega = 2 * PI * freq;

    if (ramptime > 0.0) {
        VSI_omega_ramp = abs(VSI_old_omega - VSI_omega) / ramptime; // (rad/sec) / sec
    } else {
        VSI_omega_ramp = 0;
    }

    if (VSI_omega != VSI_old_omega) {
        // There is delta freq requested
        VSI_R = (VSI_Vpercent - VSI_old_Vpercent) / (VSI_omega - VSI_old_omega);
        VSI_V0 = VSI_Vpercent - (VSI_R * VSI_omega);
    } else {
        // Instaneous voltage change
        VSI_R = 0;
        VSI_V0 = VSI_Vpercent;
    }
}

// Ramps between `current` and `setpoint` values assuming Fs = 10kHz
// Updates `current` as it ramps.
//
// `ramprate` is in (units of `current`) / sec
//     0 implies no ramp
//
//
// Returns delta in Ts for theta update
//
static double omega_ramp_fcn(double *current, double *setpoint, double ramprate)
{
    double ret;

    if (ramprate != 0 && *current != *setpoint) {
        // Ramping
        double dir = (*setpoint > *current) ? 1 : -1;
        double del = ramprate / TASK_VSI_UPDATES_PER_SEC;

        *current += (dir * del);
        ret = *current / TASK_VSI_UPDATES_PER_SEC;

        // Check if done ramping
        if ((dir == 1) && (*current > *setpoint))
            *current = *setpoint;
        if ((dir == -1) && (*current < *setpoint))
            *current = *setpoint;
    } else {
        // Not ramping
        ret = *setpoint / TASK_VSI_UPDATES_PER_SEC;
    }

    return ret;
}

#endif // APP_BETA_LABS
