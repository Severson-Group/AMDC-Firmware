#include "pwm.h"
#include "xil_io.h"
#include <stdio.h>

#define PWM_BASE_ADDR		(0x43C20000)

static uint8_t carrier_divisor;
static uint16_t carrier_max;
static uint16_t deadtime;

// switching_freq = (200e6 / divisor) / (2*carrier_max)
// or
// carrier_max = ((200e6 / divisor) / (switching_freq)) / 2

void pwm_init(void)
{
	printf("PWM:\tInitializing...\n");

	pwm_set_switching_freq(100000.0);
//	pwm_set_carrier_divisor(0);
//	pwm_set_carrier_max(255);

	pwm_set_deadtime_ns(100);

	// Turn off all PWM outputs
	for (int i = 0; i < 24; i++) {
		pwm_set_duty_raw(i, 0);
	}
}

void pwm_set_switching_freq(double freq_hz)
{
	printf("PWM:\tSetting switching freq to %f...\n", freq_hz);

	// NOTE: freq_hz can be in range:
	// 1526Hz ... 100MHz

	// Always set carrier_divisor to 0... anything else reduces resolution!
	pwm_set_carrier_divisor(0);

	// Calculate what the carrier_max should be to achieve the right switching freq
	carrier_max = (uint16_t) (((200e6 / (carrier_divisor + 1)) / (freq_hz)) / 2);
	pwm_set_carrier_max(carrier_max);

	printf("PWM:\tCarrier divisor: %d, carrier_max: %d\n", carrier_divisor, carrier_max);
}

void pwm_set_duty(uint8_t idx, double duty)
{
	if (duty >= 1.0) {
		pwm_set_duty_raw(idx, carrier_max);
	} else if (duty <= 0.0) {
		pwm_set_duty_raw(idx, 0);
	} else {
		pwm_set_duty_raw(idx, duty * carrier_max);
	}

}

void pwm_set_duty_raw(uint8_t idx, uint16_t value)
{
	// Write to offset 0 to control PWM 0
	Xil_Out32(PWM_BASE_ADDR + (idx * sizeof(uint32_t)), value);
}

void pwm_set_carrier_divisor(uint8_t divisor)
{
	carrier_divisor = divisor;

	// Write to slave reg 24 to set triangle carrier clk divisor
	Xil_Out32(PWM_BASE_ADDR + (24 * sizeof(uint32_t)), divisor);
}

void pwm_set_carrier_max(uint16_t max)
{
	carrier_max = max;

	// Write to slave reg 25 to set triangle carrier max value
	Xil_Out32(PWM_BASE_ADDR + (25 * sizeof(uint32_t)), max);
}

void pwm_set_deadtime_ns(uint16_t time_ns)
{
	printf("PWM:\tSetting deadtime to %d ns...\n", time_ns);

	deadtime = time_ns / 5;

	// NOTE: FPGA enforces minimum register value of 5
	// This should prevent shoot-through events.

	// Write to slave reg 26 to set deadtime value
	Xil_Out32(PWM_BASE_ADDR + (26 * sizeof(uint32_t)), deadtime);
}

//void inverter_get_status(uint8_t idx, inverter_status_t *status)
//{
//
//}
