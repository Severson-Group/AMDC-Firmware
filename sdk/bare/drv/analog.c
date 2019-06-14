#include "analog.h"
#include "xil_io.h"
#include <stdio.h>

#define ANALOG_BASE_ADDR				(0x43C00000)

void analog_init(void)
{
	printf("ANLG:\tInitializing...\n");

	// Set SCK to 50MHz
	analog_set_clkdiv(ANLG_CLKDIV4);

	// Set PWM sync to both high and low
	// of triangle carrier
	analog_set_pwm_sync(1, 1);
}

void analog_set_clkdiv(analog_clkdiv_e div)
{
	printf("ANLG:\tSetting clkdiv to %d...\n", div);

	// Register 16 is read/write clkdiv value

	// Read in reg
	uint32_t reg16 = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * 16));

	// Clear lower 2 bits
	reg16 &= ~(0x00000003);

	// Set lower 2 bits
	reg16 |= (div & 0x00000003);

	// Write out reg
	Xil_Out32(ANALOG_BASE_ADDR + (16 * sizeof(uint32_t)), reg16);
}

void analog_get_clkdiv(analog_clkdiv_e *div)
{
	// Register 16 is read/write clkdiv value
	uint32_t tmp = Xil_In32(ANALOG_BASE_ADDR + (16 * sizeof(uint32_t)));

	// Make sure we only look at lower 2 bits
	uint32_t value = tmp & 0x00000003;

	*div = value;
}

void analog_getf(analog_channel_e channel, float *value)
{
	// Registers 0..15 are read-only values from ADC
	uint32_t out = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * (channel-1)));

	// Conversion from raw bits to input voltage
	float tmp = (float)((int16_t) out) / 400;

	*value = tmp;
}

void analog_geti(analog_channel_e channel, int16_t *value)
{
	// Registers 0..15 are read-only values from ADC
	uint32_t out = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * (channel-1)));

	*value = (int16_t) out;
}


void analog_set_pwm_sync(uint8_t carrier_high, uint8_t carrier_low)
{
	// Read in reg
	uint32_t reg16 = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * 16));


	// Set bit reg16[2] high for `pwm_sync_high`
	if (carrier_high) {
		reg16 |= 0x00000004;
	} else {
		reg16 &= ~(0x00000004);
	}

	// Set bit reg16[3] high for `pwm_sync_low`
	if (carrier_low) {
		reg16 |= 0x00000008;
	} else {
		reg16 &= ~(0x00000008);
	}

	// Write out reg
	Xil_Out32(ANALOG_BASE_ADDR + (16 * sizeof(uint32_t)), reg16);
}
