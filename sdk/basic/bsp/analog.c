#include "../bsp/analog.h"
#include "xil_io.h"
#include <stdio.h>


#define ANALOG_BASE_ADDR				(0x43C00000)

float analog_offsetf[ANALOG_NUM_CHANNELS] = {0};

void analog_init(void)
{
	printf("ANLG:\tInitializing...\n");

	// Set SCK to 50MHz
	analog_set_clkdiv(ANLG_CLKDIV4);
}

void analog_calibrate(uint32_t num)
{
    for (int i = 0; i < ANALOG_NUM_CHANNELS; i++) {
    	float value = 0;

    	// Average ANALOG_CALIBRATE_NUM_SAMPLES reads
    	for (int j = 0; j < ANALOG_CALIBRATE_NUM_SAMPLES; j++) {
    		float tmp;
            analog_getf(i+1, &tmp);
            value += tmp;
    	}

    	analog_offsetf[i] = value / ANALOG_CALIBRATE_NUM_SAMPLES;
    }

    printf("Analog Calibration Done:\n");
    for (int i = 0; i < ANALOG_NUM_CHANNELS; i++) {
    	printf("Channel #%d offset: %f volts\n", i+1, analog_offsetf[i]);
    }
}

void analog_set_clkdiv(analog_clkdiv_e div)
{
	printf("ANLG:\tSetting clkdiv to %d...\n", div);

	// Make sure we are only writing lower 2 bits
	uint32_t value = div & 0x00000003;

	// Register 16 is read/write clkdiv value
	Xil_Out32(ANALOG_BASE_ADDR + (16 * sizeof(uint32_t)), value);
}

void analog_get_clkdiv(analog_clkdiv_e *div)
{
	// Register 16 is read/write clkdiv value
	uint32_t tmp = Xil_In32(ANALOG_BASE_ADDR + (16 * sizeof(uint32_t)));

	*div = tmp;
}

void analog_getf(analog_channel_e channel, float *value)
{
	// Registers 0..15 are read-only values from ADC
	uint32_t out = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * (channel-1)));

	// Conversion from raw bits to input voltage
	float tmp = (float)((int16_t) out) / 400;

	// Subtract off the measured offset bias
	*value = tmp - analog_offsetf[channel-1];
}

void analog_geti(analog_channel_e channel, int16_t *value)
{
	// Registers 0..15 are read-only values from ADC
	uint32_t out = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * (channel-1)));

	*value = (int16_t) out;
}
