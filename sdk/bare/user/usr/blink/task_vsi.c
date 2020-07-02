#ifdef APP_BLINK

#include "usr/blink/task_vsi.h"
#include "drv/led.h"
#include "sys/scheduler.h"
#include <stdint.h>
#include <math.h>

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

// Example logging variables for testing
double LOG_vsi_a = 0;
double LOG_vsi_b = 0;
double LOG_vsi_c = 0;

static double Ts    = 1.0 / 10000.0; // [sec]
static double theta = 0.0;           // [rad]
static double omega = 10.0;        // [rad/s]
static double Do    = 0.75;          // [--]

void task_vsi_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_vsi_callback, NULL, "vsi", TASK_VSI_INTERVAL_USEC);

    // Register task with scheduler
    scheduler_tcb_register(&tcb);
}

void task_vsi_callback(void *arg)
{
    // Update theta
    theta += (Ts * omega);
    theta = fmod(theta, 2.0 * M_PI); // Wrap to 2*pi

    // Calculate desired duty ratios
    LOG_vsi_a = Do * cos(theta);
    LOG_vsi_b = Do * cos(theta + 2.0*M_PI/3.0);
    LOG_vsi_c = Do * cos(theta + 4.0*M_PI/3.0);
}

#endif // APP_BLINK
