
#ifdef APP_VSI

#include "usr/vsiApp/task_vsiApp.h"
#include "drv/led.h"
#include "sys/commands.h"
#include "drv/timing_manager.h"
#include "sys/scheduler.h"
#include "drv/cpu_timer.h"
#include "drv/pwm.h"
#include "drv/analog.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

// Scheduler TCB which holds task "context"
static task_control_block_t tcb = {0};


static double Ts = 1.0 / 10000.0; // [sec]
static double theta = 0.0;        // [rad]
static double omega = 10.0 * 2 * PI;       // [rad/s]
static double Do = 0.3;          // [--]
static uint8_t RMS_driven = 0;
static double RMS_target = 0.0;

// Logging variables
float LOG_current_a = 0.0;
float LOG_current_b = 0.0;
float LOG_current_c = 0.0;

/* trying this out */
#include "xil_io.h"
#define PWM_MUX_ADDR_LOGGING (0x43C40000)

float LOG_voltage_a = 0.0;
float LOG_voltage_b = 0.0;
float LOG_voltage_c = 0.0;

#define CIRCULAR_BUFFER_LENGTH 1000
float circularBuffer[CIRCULAR_BUFFER_LENGTH];
uint32_t circularBufferIndex = 0;


int task_vsiApp_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return FAILURE;
    }

    pwm_enable();

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_vsiApp_callback, NULL, "vsiApp", TASK_VSI_INTERVAL_USEC);

    task_stats_enable(&tcb.stats);

    // Register task with scheduler
    return scheduler_tcb_register(&tcb);
}

int task_vsiApp_deinit(void)
{
	pwm_disable();
	return scheduler_tcb_unregister(&tcb);
}

void task_vsiApp_callback(void *arg)
{

    // Update theta
    theta += (Ts * omega);
    theta = fmod(theta, 2.0 * M_PI); // Wrap to 2*pi

    // Calculate desired duty ratios
    double duty_a = 0.5 + Do / 2.0 * cos(theta);
    double duty_b = 0.5 + Do / 2.0 * cos(theta - 2.0 * M_PI / 3.0);
    double duty_c = 0.5 + Do / 2.0 * cos(theta - 4.0 * M_PI / 3.0);

    // Update PWM peripheral in FPGA
	pwm_set_duty(0, duty_a); // Set HB1 duty ratio (INV1, PWM1 and PWM2)
	pwm_set_duty(1, duty_b); // Set HB2 duty ratio (INV1, PWM3 and PWM4)
	pwm_set_duty(2, duty_c); // Set HB3 duty ratio (INV1, PWM5 and PWM6)

	// Update logging variables
	analog_getf(ANALOG_IN5, &LOG_current_a);
	analog_getf(ANALOG_IN6, &LOG_current_b);
	analog_getf(ANALOG_IN7, &LOG_current_c);
	LOG_current_a *= 0.5;
	LOG_current_b *= 0.5;
	LOG_current_c *= 0.5;

	LOG_voltage_a = (duty_a - 0.5) * 20;
	LOG_voltage_b = (duty_b - 0.5) * 20;
	LOG_voltage_c = (duty_c - 0.5) * 20;

	circularBuffer[circularBufferIndex] = LOG_current_b;
	circularBufferIndex++;
	if (circularBufferIndex >= CIRCULAR_BUFFER_LENGTH) {
		circularBufferIndex = 0;
	}
	pwm_get_switching_freq() / timing_manager_get_ratio(); // number of samples per second
	omega / (2.0 * M_PI); // number of periods per second
	(pwm_get_switching_freq() / timing_manager_get_ratio()) * (2.0 * M_PI) / omega; // number of samples per period

	if (RMS_driven) {
		double currentRMS = calculateRMS();
		Do += (RMS_target - currentRMS) * SQRT2 * Ts;
		if (Do > 1.0) {
			Do = 1.0;
		}
		if (Do < 0) {
			Do = 0;
		}
	}
}

double calculateRMS() {
	float average = 0.0;
	for (int i = 0; i < CIRCULAR_BUFFER_LENGTH; i++) {
		average += circularBuffer[i] * circularBuffer[i];
	}
	average /= CIRCULAR_BUFFER_LENGTH;
	return sqrt(average) * 2;
}

int task_vsiApp_amplitude(double amplitude) {
	Do = amplitude;
	return SUCCESS;
}

int task_vsiApp_frequency(double frequency) {
	omega = frequency * 2.0 * M_PI; // rad/s = frequency (Hz) * (2 * PI)
	return SUCCESS;
}

double task_vsiApp_get_amplitude() {
	cmd_resp_printf("RMS_target: %lf\n", RMS_target);
	cmd_resp_printf("RMS_driven: %d\n", RMS_driven);
	cmd_resp_printf("currentRMS: %lf\n", calculateRMS());
	return Do;
}

int task_vsiApp_RMS(double RMS) {
	RMS_target = RMS;
	cmd_resp_printf("RMS_target: %lf\n", RMS_target);
	for (int i = 0; i < CIRCULAR_BUFFER_LENGTH; i++) {
		circularBuffer[i] = RMS_target;
	}
	return SUCCESS;
}

int task_vsiApp_RMS_enable() {
	if (RMS_driven == 0) {
		for (int i = 0; i < CIRCULAR_BUFFER_LENGTH; i++) {
			circularBuffer[i] = RMS_target;
		}
		RMS_driven = 1;
		return SUCCESS;
	}
	return FAILURE;
}

int task_vsiApp_RMS_disable() {
	if (RMS_driven) {
		RMS_driven = 0;
		return SUCCESS;
	}
	return FAILURE;
}

void task_vsiApp_stats_print(void) {
    task_stats_print(&tcb.stats);
}

void task_vsiApp_stats_reset(void) {
    task_stats_reset(&tcb.stats);
}

#endif // APP_BLINK
