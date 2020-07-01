#include "drv/analog.h"
#include "sys/defines.h"
#include "xil_io.h"
#include <stdbool.h>
#include <stdio.h>

#define ANALOG_BASE_ADDR (0x43C00000)

void analog_init(void)
{
    printf("ANLG:\tInitializing...\n");

    // Set SCK to 50MHz (200MHz / 4)
    analog_set_clkdiv(ANALOG_CLKDIV4);

    // Set PWM sync to both high and low of triangle carrier
    analog_set_pwm_sync(1, 1);
}

int analog_set_clkdiv(analog_clkdiv_e div)
{
    printf("ANLG:\tSetting clkdiv to %d...\n", div);

    // Make sure the divisor is valid
    if (!analog_is_valid_clkdiv(div)) {
        return FAILURE;
    }

    // Register 16 is read/write clkdiv value

    // NOTE: this code updates the register value using read / modify / write semantics.
    // This allows the other bits of the register to be used for other things (which they are!)

    // Read in reg
    uint32_t reg16 = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * 16));

    // Clear lower 2 bits
    reg16 &= ~(0x00000003);

    // Set lower 2 bits
    reg16 |= (div & 0x00000003);

    // Write out reg
    Xil_Out32(ANALOG_BASE_ADDR + (16 * sizeof(uint32_t)), reg16);

    return SUCCESS;
}

void analog_get_clkdiv(analog_clkdiv_e *out_div)
{
    // Register 16 is read/write clkdiv value
    uint32_t tmp = Xil_In32(ANALOG_BASE_ADDR + (16 * sizeof(uint32_t)));

    // Make sure we only look at lower 2 bits
    uint32_t value = tmp & 0x00000003;

    *out_div = value;
}

int analog_getf(analog_channel_e channel, float *out_value)
{
    // Make sure channel in valid
    if (!analog_is_valid_channel(channel)) {
        return FAILURE;
    }

    // Registers 0..15 are read-only values from ADC
    uint32_t out = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * channel));

    // Conversion from raw bits to input voltage
    float tmp = (float) ((int16_t) out) / 400.0;

    *out_value = tmp;

    return SUCCESS;
}

int analog_geti(analog_channel_e channel, int16_t *out_value)
{
    // Make sure channel in valid
    if (!analog_is_valid_channel(channel)) {
        return FAILURE;
    }

    // Registers 0..15 are read-only values from ADC
    uint32_t out = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * channel));

    *out_value = (int16_t) out;

    return SUCCESS;
}

void analog_set_pwm_sync(bool sync_to_carrier_high, bool sync_to_carrier_low)
{
    // Read in reg
    uint32_t reg16 = Xil_In32(ANALOG_BASE_ADDR + (sizeof(uint32_t) * 16));

    // Set bit reg16[2] high for `pwm_sync_high`
    if (sync_to_carrier_high) {
        reg16 |= 0x00000004;
    } else {
        reg16 &= ~(0x00000004);
    }

    // Set bit reg16[3] high for `pwm_sync_low`
    if (sync_to_carrier_low) {
        reg16 |= 0x00000008;
    } else {
        reg16 &= ~(0x00000008);
    }

    // Write out reg
    Xil_Out32(ANALOG_BASE_ADDR + (16 * sizeof(uint32_t)), reg16);
}
