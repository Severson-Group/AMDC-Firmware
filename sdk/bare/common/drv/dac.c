#if 0

#include "drv/dac.h"
#include "xil_io.h"
#include <stdio.h>

#define DAC_BASE_ADDR (0x43C30000)

// NOTE:
// The DAC expansion card MUST be plugged into
// the GPIO header during start-up.
//
// The FPGA configures the DAC IC _once_ during
// start-up, so if the DAC IC misses these commands,
// it will NOT work.

void dac_init(void)
{
    printf("DAC:\tInitializing...\n");

    // Set SCLK to 10MHz
    dac_set_sclk_div(10);

    // Start with all LEDs off and 0V output
    for (int i = 0; i < 8; i++) {
        dac_set_voltage_raw(i, 0);
        dac_set_led(i, 0);
    }
}

// Set the SCLK divisor
//
// The FPGA generates SCLK based on the system clock
// which is 200MHz. SCLK frequency can be calculated:
//
// Fsclk = (200e6 / div) / 2
//
// i.e., with div = 100, Fsclk = 1MHz
//
void dac_set_sclk_div(uint32_t div)
{
    // NOTE: the FPGA enforces the divisor to be >= 10,
    // so this isn't strictly necessary
    if (div < 10) {
        div = 10;
    }

    // Write to offset 9 to set sclk_div
    Xil_Out32(DAC_BASE_ADDR + (9 * sizeof(uint32_t)), div);
}

void dac_set_output(uint8_t idx, double value, double min, double max)
{
    // Want:
    //
    // min => 0
    // ...
    // value => 0..DAC_FULL_SCALE
    // ....
    // max => DAC_FULL_SCALE

    double range = max - min;
    double v = (value - min) / range;

    uint16_t dac_out = v * DAC_FULL_SCALE;
    dac_set_voltage_raw(idx, dac_out);
}

void dac_set_voltage_raw(uint8_t idx, uint16_t value)
{
    // Make sure we are trying to write a valid value
    // 0 .. 4095
    if (value > DAC_FULL_SCALE) {
        value = DAC_FULL_SCALE;
    }

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

#endif
