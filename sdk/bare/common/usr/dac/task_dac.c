#ifdef APP_DAC

#include "usr/dac/task_dac.h"
#include "drv/hardware_targets.h"
#include "sys/scheduler.h"
#include "usr/user_config.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "drv/dac.h"
#include "drv/led.h"

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;
double FREQ = 1;
#define NUM_CHANNELS 3
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

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

void task_dac_trigger () {
	dac_set_trigger();
}
void task_dac_frequency(double freq) {
	FREQ = freq;
}

void task_dac_broadcast(double voltage) {
	dac_set_broadcast(voltage, 10, -10);
}

void task_dac_set_voltage(uint8_t ch, double voltage) {
	dac_set_voltage(ch, voltage, 10, -10);
}

void task_dac_set_reg(uint8_t reg, uint32_t value) {
	dac_set_raw(reg, value);
}

void task_dac_callback(void *arg)
{
	static int divider = 0;
	static double step = 0;
	divider++;
	if(divider >= 1/FREQ) {
			step++;
			divider = 0;


		if (step > 8192) step = 0;

		for (int i = 0; i < 3; i++) {
			dac_set_voltage(i,10 * sin(TASK_DAC_MIN_PHASE*step + ((double)i / 3 * 2 * PI)), 10, -10);
		}
	}
}

#endif // APP_BLINK
