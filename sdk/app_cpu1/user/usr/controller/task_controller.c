#ifdef APP_CONTROLLER

#include "usr/controller/task_controller.h"
#include "sys/scheduler.h"
#include "drv/pwm.h"
#include <math.h>

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

int task_controller_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return FAILURE;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_controller_callback,
                        NULL, "controller", TASK_CONTROLLER_INTERVAL_USEC);

    task_stats_enable(&tcb.stats);

    // Register task with scheduler
    return scheduler_tcb_register(&tcb);
}

int task_controller_deinit(void)
{
    return scheduler_tcb_unregister(&tcb);
}

double Ts    = 1.0 / (double) TASK_CONTROLLER_UPDATES_PER_SEC;
double theta = 0.0;    // [rad]
double omega = 377.0;  // [rad/s]
double Do    = 0.8;    // [--]

void task_controller_callback(void *arg)
{
    // Update theta
    theta += (Ts * omega);

    // Wrap to 2*pi
    theta = fmod(theta, 2.0 * M_PI);

    // Calculate desired duty ratios
    double duty_a = 0.5 + Do/2.0 * cos(theta);
    double duty_b = 0.5 + Do/2.0 * cos(theta - 2.0*M_PI/3.0);
    double duty_c = 0.5 + Do/2.0 * cos(theta - 4.0*M_PI/3.0);

    // Update PWM peripheral in FPGA
    pwm_set_duty(0, duty_a); // Set HB1 duty ratio (INV1, PWM1 and PWM2)
    pwm_set_duty(1, duty_b); // Set HB2 duty ratio (INV1, PWM3 and PWM4)
    pwm_set_duty(2, duty_c); // Set HB3 duty ratio (INV1, PWM5 and PWM6)
}

int task_controller_set_frequency(double freq)
{
    omega = freq;
    return SUCCESS;
}

int task_controller_set_amplitude(double amplitude)
{
    int err = FAILURE;

    if (amplitude >= 0.0 && amplitude <= 1.0) {
        Do = amplitude;
        err = SUCCESS;
    }

    return err;
}

void task_controller_stats_print(void)
{
    task_stats_print(&tcb.stats);
}

void task_controller_stats_reset(void)
{
    task_stats_reset(&tcb.stats);
}

#endif // APP_CONTROLLER
