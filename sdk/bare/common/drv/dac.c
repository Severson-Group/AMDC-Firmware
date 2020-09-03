#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#include "drv/dac.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdio.h>

#define DAC_BASE_ADDR (0x43C40000)


void dac_init(void)
{
	printf("DAC:\tInitializing...\n");

	dac_set_raw(GAIN, 0x0401FF); // Default Reference divider and each channel Gain to 2/2
	dac_set_broadcast(0, 10, -10); // Default all channel values to 0 (redundant to power on reset)
}

void dac_set_voltage(uint8_t ch, double voltage, double max, double min)
{
	double range = max - min;
	uint32_t value = (voltage - min) * DAC_MAX_CODE / range;

	dac_set_raw(ch + 0x8, (ch + 0x8)<<16 | (value<<4 & 0xFFF0) );
}

void dac_set_broadcast(double voltage, double max, double min)
{
	double range = max - min;
	uint32_t value = (voltage - min) * DAC_MAX_CODE / range;

	dac_set_raw(0x6, (0x6)<<16 | (value<<4 & 0xFFF0));
}

void dac_set_trigger(void)
{
	Xil_Out32(DAC_BASE_ADDR + (TRIGGER * sizeof(uint32_t)), 0x00000010);
}

void dac_set_raw(dac_reg_t reg, uint32_t value)
{
	Xil_Out32(DAC_BASE_ADDR + (reg * sizeof(uint32_t)), value);
}

#endif // USER_CONFIG_HARDWARE_TARGET
