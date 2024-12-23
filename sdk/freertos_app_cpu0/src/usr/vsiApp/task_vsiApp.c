
#ifdef APP_VSI

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
/* other includes */
#include "usr/vsiApp/task_vsiApp.h"
#include "drv/led.h"
#include "drv/cpu_timer.h"
#include "drv/pwm.h"
#include "drv/analog.h"
#include "drv/amds.h"
#include "drv/encoder.h"
#include "sys/defines.h"
#include "sys/log.h"
#include "sys/injection.h"
#include "sys/util.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

// Scheduler TCB which holds task "context"
static TaskHandle_t tcb;
static uint8_t taskExists = 0; // extra data to ensure tasks don't get duplicated or double free'd

static double Ts = 1.0 / 10000.0;          // [sec]
static double theta = 0.0;                 // [rad]
static double omega = 10.0 * 2 * PI;       // [rad/s]
static double Do = 0.3;                    // [--]

/* injection */
static inj_ctx_t inj_ctx_ctrl[3] = {0};

int task_vsiApp_init(void)
{
	if (taskExists) {
		return FAILURE;
	}
	/* PWM */
    if (pwm_enable_hw(true) != SUCCESS) {
    	return FAILURE;
    }
    if (pwm_enable() != SUCCESS) {
		return FAILURE;
	}

    /* Encoder bits */
    encoder_set_pulses_per_rev_bits(13);
    /* Initialize signal injection points */
	injection_ctx_init(&inj_ctx_ctrl[0], "amp*");
	injection_ctx_init(&inj_ctx_ctrl[1], "theta*");
	injection_ctx_init(&inj_ctx_ctrl[2], "omega_m*");

	/* Register all signal injection points */
	for (int i = 0; i < ARRAY_SIZE(inj_ctx_ctrl); i++) {
		injection_ctx_register(&inj_ctx_ctrl[i]);
	}

    /* create control task */
    if (create_control_task(&tcb, task_vsiApp_callback, "vsiApp", TASK_VSIAPP_UPDATES_PER_SEC) == FAILURE) {
    	return FAILURE;
    }
    taskExists = 1;
	return SUCCESS;
}

int task_vsiApp_deinit(void)
{
	if (taskExists == 0) {
		return FAILURE;
	}
	if (pwm_disable() != SUCCESS) {
		return FAILURE;
	}
	if (pwm_enable_hw(false) != SUCCESS) {
		return FAILURE;
	}
	delete_control_task(&tcb);
	taskExists = 0;
	return SUCCESS;
}

void task_vsiApp_callback(void *arg)
{
	float current_a = 0.0;
	float current_b = 0.0;
	float current_c = 0.0;
	float voltage_a = 0.0;
	float voltage_b = 0.0;
	float voltage_c = 0.0;
	float amds_current_a = 0.0;
	// Perform signal injections
	injection_inj(&Do, &inj_ctx_ctrl[0], Ts);
	injection_inj(&theta, &inj_ctx_ctrl[1], Ts);
	injection_inj(&omega, &inj_ctx_ctrl[2], Ts);

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
	analog_getf(ANALOG_IN5, &current_a);
	analog_getf(ANALOG_IN6, &current_b);
	analog_getf(ANALOG_IN7, &current_c);
	current_a *= 0.5;
	current_b *= 0.5;
	current_c *= 0.5;
//	LOG_current_a = (duty_a - 0.5) * 13.3333333;
//	LOG_current_b = (duty_b - 0.5) * 13.3333333;
//	LOG_current_c = (duty_c - 0.5) * 13.3333333;

//	Xil_Out32(PWM_MUX_ADDR_LOGGING + (0 * sizeof(uint32_t)), 1);

	/* we use a Heisenberg probabalisic model to predict the state of the PWM output at a given time by using the duty cycle and deadtime */
//	double deadtimePercentage = pwm_get_deadtime_ns() / (1000000000.0 / pwm_get_switching_freq());
//	LOG_voltage_a = ((float) rand() / RAND_MAX <= (duty_a - deadtimePercentage)) * 5.0;
//	LOG_voltage_b = ((float) rand() / RAND_MAX <= (duty_b - deadtimePercentage)) * 5.0;
//	LOG_voltage_c = ((float) rand() / RAND_MAX <= (duty_c - deadtimePercentage)) * 5.0;

	voltage_a = (duty_a - 0.5) * 20;
	voltage_b = (duty_b - 0.5) * 20;
	voltage_c = (duty_c - 0.5) * 20;

	const uint8_t amds_port = 2;
	int32_t out_ch_1;

	// Check validity of latest data for the AMDS plugged into your GPIO port
	uint8_t valid = amds_check_data_validity(amds_port);

	if (valid == 0xFF) {
		// Yay! 0xFF means the bits for all channels are valid!
		// Read in values sampled on the AMDS (plugged into your GPIO port) from all channels:
		amds_get_data(amds_port, AMDS_CH_1, &out_ch_1);
		amds_current_a = 0.00125 * out_ch_1;
		// Now, "out" variables contain the sign-extended 16-bit
		// sample value for each channel
	}

	log_callback(&current_a, LOG_FLOAT, "current_a");
	log_callback(&current_b, LOG_FLOAT, "current_b");
	log_callback(&current_c, LOG_FLOAT, "current_c");
	log_callback(&voltage_a, LOG_FLOAT, "voltage_a");
	log_callback(&voltage_b, LOG_FLOAT, "voltage_b");
	log_callback(&voltage_c, LOG_FLOAT, "voltage_c");
	log_callback(&amds_current_a, LOG_FLOAT, "amds_current_a");
	log_callback(&Do, LOG_DOUBLE, "amplitude_internal");

	uint32_t encPosition = 0;
	encoder_get_position(&encPosition);
	log_callback(&encPosition, LOG_INT, "encoder_position");
	// xil_printf("encoder value: %d\n", encPosition);

	/* delay(50us) */
//	uint32_t startDelay = cpu_timer_now();
//	while (cpu_timer_ticks_to_usec(cpu_timer_now() - startDelay) < 860) {
//		asm volatile("nop");
//	}
}

int task_vsiApp_amplitude(double amplitude) {
	Do = amplitude;
	return SUCCESS;
}

int task_vsiApp_frequency(double frequency) {
	omega = frequency * 2.0 * M_PI; // rad/s = frequency (Hz) * (2 * PI)
	return SUCCESS;
}

void task_vsiApp_stats_print(void) {
	char statsBuffer[configSTATS_BUFFER_MAX_LENGTH];
	vTaskGetRunTimeStats(statsBuffer);
	xil_printf("%s\n", statsBuffer);
}

void task_vsiApp_stats_reset(void) {
    /* does nothing */
}

int task_vsiApp_clear_inj(void) {
	// Clear all injection points
	for (int i = 0; i < ARRAY_SIZE(inj_ctx_ctrl); i++) {
		injection_ctx_clear(&inj_ctx_ctrl[i]);
	}
	return SUCCESS;
}

#endif // APP_BLINK
