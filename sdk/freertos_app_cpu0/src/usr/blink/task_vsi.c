#ifdef APP_BLINK
/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
/* other includes */
#include "usr/blink/task_vsi.h"
#include "drv/led.h"
#include "sys/defines.h"
#include <math.h>
#include <stdint.h>

// Scheduler TCB which holds task "context"
static TaskHandle_t tcb;

// Example logging variables for testing
int LOG_task_vsi_runs = 0;
double LOG_vsi_a = 0;
float LOG_vsi_b = 0;
int LOG_vsi_c = 0;

static double Ts = 1.0 / 10000.0; // [sec]
static double theta = 0.0;        // [rad]
static double omega = 10.0;       // [rad/s]
static double Do = 0.75;          // [--]

void task_vsi_init(void)
{
    // Fill TCB with parameters
    xTaskCreate(task_vsi, (const char *) "vsi", configMINIMAL_STACK_SIZE,
				NULL, tskIDLE_PRIORITY, &tcb);
}

void task_vsi(void *arg)
{
	for (;;) {
		vTaskDelay(TASK_VSI_INTERVAL_TICKS);
		LOG_task_vsi_runs += 1;

		// Update theta
		theta += (Ts * omega);
		theta = fmod(theta, 2.0 * M_PI); // Wrap to 2*pi

		// Calculate desired duty ratios
		double vsi_a = Do * cos(theta);
		double vsi_b = Do * cos(theta + 2.0 * M_PI / 3.0);
		double vsi_c = Do * cos(theta + 4.0 * M_PI / 3.0);

		// Update logging variables
		LOG_vsi_a = (double) (10e3 * vsi_a);
		LOG_vsi_b = (float) (10e3 * vsi_b);
		LOG_vsi_c = (int) (10e3 * vsi_c);
	}
}

#endif // APP_BLINK
