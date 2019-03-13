#include "pwm.h"

#include "xil_io.h"

#define PWM_BASE_ADDR		(0x43C20000)

void pwm_init(void)
{
	pwm_set_switching_freq(3);
}

void pwm_set_duty(uint8_t idx, uint8_t value)
{
	// Write to offset 0 to control PWM 0
	Xil_Out32(PWM_BASE_ADDR + (idx * sizeof(uint32_t)), value);
}


void pwm_set_switching_freq(uint8_t divisor)
{
	// Write to slave reg 24 to set triangle carrier clk divisor
	Xil_Out32(PWM_BASE_ADDR + (24 * sizeof(uint32_t)), divisor);
}

//void inverter_set_duty_ratio(uint8_t inv_idx, uint8_t pwm_idx, uint8_t value)
//{
//
//}
//
//void inverter_set_dead_time(uint8_t inv_idx, uint8_t pwm_idx, uint8_t value)
//{
//
//}
//
//void inverter_get_status(uint8_t idx, inverter_status_t *status)
//{
//
//}
