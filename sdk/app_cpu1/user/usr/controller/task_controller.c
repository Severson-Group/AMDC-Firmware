#ifdef APP_CONTROLLER

#include "usr/controller/task_controller.h"
#include "sys/scheduler.h"
#include "drv/pwm.h"
#include "drv/amds.h"
#include <math.h>

#define FBC_CURRENT_DEFAULT_GAIN        312
#define FBC_CURRENT_DEFAULT_OFFSET      2.500351

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

int task_controller_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return FAILURE;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_controller_callback,
                        NULL, "ctrl", TASK_CONTROLLER_INTERVAL_USEC);

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

const uint8_t amds_port = 1;
int32_t out_ch_1, out_ch_2, out_ch_3, out_ch_4;
int32_t out_ch_5, out_ch_6, out_ch_7, out_ch_8;

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

	// Check validity of latest data for the AMDS plugged into your GPIO port
	uint8_t valid = amds_check_data_validity(amds_port);

	if (valid == 0xFF) {
		// 0xFF means the bits for all channels are valid!
		// Read in values sampled on the AMDS (plugged into your GPIO port) from all channels:

		amds_get_data(amds_port, AMDS_CH_1, &out_ch_1);
		amds_get_data(amds_port, AMDS_CH_2, &out_ch_2);
		amds_get_data(amds_port, AMDS_CH_3, &out_ch_3);
		amds_get_data(amds_port, AMDS_CH_4, &out_ch_4);
		amds_get_data(amds_port, AMDS_CH_5, &out_ch_5);
		amds_get_data(amds_port, AMDS_CH_6, &out_ch_6);
		amds_get_data(amds_port, AMDS_CH_7, &out_ch_7);
		amds_get_data(amds_port, AMDS_CH_8, &out_ch_8);

		// Now, "out" variables contain the sign-extended 16-bit
		// sample value for each channel
	}
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

void task_controller_get_raw(void) {
	printf("\nRaw AMDS Channel 1: %ld\r\n", out_ch_1);
	printf("Raw AMDS Channel 2: %ld\r\n", out_ch_2);
	printf("Raw AMDS Channel 3: %ld\r\n", out_ch_3);
	printf("Raw AMDS Channel 4: %ld\r\n", out_ch_4);
	printf("Raw AMDS Channel 5: %ld\r\n", out_ch_5);
	printf("Raw AMDS Channel 6: %ld\r\n", out_ch_6);
	printf("Raw AMDS Channel 7: %ld\r\n", out_ch_7);
	printf("Raw AMDS Channel 8: %ld\r\n", out_ch_8);
}


int task_controller_get_data(int ch) {

	double raw_voltage;
	double voltage;
	int channel;

	switch (ch) {
	case 1:
		channel = AMDS_CH_1;
		break;
	case 2:
		channel = AMDS_CH_2;
		break;
	case 3:
		channel = AMDS_CH_3;
		break;
	case 4:
		channel = AMDS_CH_4;
		break;
	case 5:
		channel = AMDS_CH_5;
		break;
	case 6:
		channel = AMDS_CH_6;
		break;
	case 7:
		channel = AMDS_CH_7;
		break;
	case 8:
		channel = AMDS_CH_8;
		break;
	default:
		return FAILURE;
	}

	amds_get_voltage(1, channel, AMDS_CURRENT_CARD, &raw_voltage);
	amds_convert_voltage(raw_voltage, FBC_CURRENT_DEFAULT_OFFSET, FBC_CURRENT_DEFAULT_GAIN, &voltage);
	printf("\nCurrent Sensor Reading: %f\r\n", voltage);

	return SUCCESS;
}

#endif // APP_CONTROLLER
