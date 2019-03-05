#include "inverter.h"
#include "xil_io.h"

#define INVERTERS_BASE_ADDR		(0x43C20000)

void inverter_init(void)
{

}

void inverter_set_duty(uint8_t idx, uint8_t value)
{
	// Write to offset 0 to control PWM 0
	Xil_Out32(INVERTERS_BASE_ADDR + (idx * sizeof(uint32_t)), value);
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
