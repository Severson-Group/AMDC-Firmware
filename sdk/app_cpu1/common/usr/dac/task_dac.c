#ifdef APP_DAC

#include "usr/dac/task_dac.h"
#include "drv/dac.h"
#include "sys/scheduler.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

// Scales the minimum phase step each callback cycle
// The minimum phase step is defined as 2 PI / callback frequency
static double FREQ = 1;

#define NUM_CHANNELS 3

int task_dac_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return FAILURE;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_dac_callback, NULL, "dac", TASK_DAC_INTERVAL_USEC);

    // Register task with scheduler
    return scheduler_tcb_register(&tcb);
}

int task_dac_deinit(void)
{
    // Unregister task with scheduler
    return scheduler_tcb_unregister(&tcb);
}

void task_dac_trigger()
{
    // Initiate a trigger
    dac_set_trigger();
}

void task_dac_frequency(double freq)
{
    // Update the frequency
    FREQ = freq;
}

void task_dac_broadcast(double voltage)
{
    // Initiate a broadcast
    dac_set_broadcast(voltage);
}

void task_dac_set_voltage(uint8_t ch, double voltage)
{
    // Set the given channels voltage to given voltage
    dac_set_voltage(ch, voltage);
}

void task_dac_set_reg(uint8_t reg, uint32_t value)
{
    // Write the raw value given in hex to the given register
    dac_set_raw(reg, value);
}

void task_dac_callback(void *arg)
{
    static int divider = 0; // Divides the frequency
    static double step = 0;

    divider++;

    // If the frequency is less than 1Hz (default) the callback will return and do
    // nothing until the appropriate amount of cycles have passed for the desired
    // frequency to be met
    if (divider >= 1 / FREQ) {

        // If the frequency is greater than 1Hz (default) the step is scaled by the
        // desired frequency
        if (FREQ > 1) {
            step += FREQ;
        } else {
            step++;
        }

        // Reset the divider for the next cycle
        divider = 0;

        // Reset the step for the next cycle
        if (step > TASK_DAC_UPDATES_PER_SEC)
            step = 0;

        // Generates a 3-phase +/- 10V output on channels 0, 2, and 4
        for (int i = 0; i < 3; i++) {
            dac_set_voltage(i * 2, 10 * sin(TASK_DAC_MIN_PHASE * step + ((double) i / 3 * 2 * PI)));
        }

        // Send all channel updates through
        dac_set_trigger();
    }
}

#endif // APP_BLINK
