#include "dac.h"
#include "xil_io.h"
#include <stdio.h>

#define DAC_BASE_ADDR		(0x43C30000)

void dac_init(void)
{
	printf("DAC:\tInitializing...\n");

	// Start with all LEDs off and 0V output
	for (int i = 0; i < 8; i++) {
		dac_set_voltage(i, 0);
		dac_set_led(i, 0);
	}
}

void dac_set_voltage(uint8_t idx, uint16_t value)
{
	// Write to offset 0 to set DAC1
	Xil_Out32(DAC_BASE_ADDR + (idx * sizeof(uint32_t)), value);
}

void dac_set_led(uint8_t idx, uint8_t state)
{
	// Offset 8 is the LED control register

	uint32_t curr_led_state;
	curr_led_state = Xil_In32(DAC_BASE_ADDR + (8 * sizeof(uint32_t)));

	// NOTE: LEDs on DAC PCB are active low!

	if (state == 0) {
		// Trying to turn off LED, so write a 1 to its index
		curr_led_state |= (1 << idx);
	} else {
		// Trying to turn on LED, so write a 0 to its index
		curr_led_state &= ~(1 << idx);
	}

	Xil_Out32(DAC_BASE_ADDR + (8 * sizeof(uint32_t)), curr_led_state);
}
