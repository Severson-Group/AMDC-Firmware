#include "drv/analog.h"
#include "drv/analog_defs.h"
#include "sys/defines.h"
#include <stdbool.h>

static volatile uint32_t *m_adc;

void analog_init(uint32_t base_addr)
{
    // Store base address for IP
    m_adc = (volatile uint32_t *) base_addr;

    // Set SCK to 50MHz (200MHz / 4)
    analog_set_clkdiv(ANALOG_CLKDIV4);
}

int analog_set_clkdiv(analog_clkdiv_e div)
{
    // Make sure the divisor is valid
    if (!analog_is_valid_clkdiv(div)) {
        return FAILURE;
    }

    // Read in CONTROL register
    uint32_t reg = m_adc[ANALOG_DEFS_OFFSET_CONTROL / 4];

    // Clear lower 2 bits
    reg &= ~ANALOG_DEFS_CONTROL_CLKDIV_MASK;

    // Set lower 2 bits
    reg |= (div & ANALOG_DEFS_CONTROL_CLKDIV_MASK);

    // Write out CONTROL register
    m_adc[ANALOG_DEFS_OFFSET_CONTROL / 4] = reg;

    return SUCCESS;
}

void analog_get_clkdiv(analog_clkdiv_e *out_div)
{
    // Read in CONTROL register
    uint32_t reg = m_adc[ANALOG_DEFS_OFFSET_CONTROL / 4];

    // Make sure we only look at lower 2 bits
    uint32_t value = reg & ANALOG_DEFS_CONTROL_CLKDIV_MASK;

    *out_div = value;
}

int analog_getf(analog_channel_e channel, float *out_value)
{
    // Read raw binary value from ADC
    int16_t val;
    if (analog_geti(channel, &val) != SUCCESS) {
        return FAILURE;
    }

    // Conversion from raw bits to voltage
    *out_value = (float) (val) / 400.0;

    return SUCCESS;
}

int analog_geti(analog_channel_e channel, int16_t *out_value)
{
    // Make sure channel in valid
    if (!analog_is_valid_channel(channel)) {
        return FAILURE;
    }

    // Read in ADC data register
    uint32_t reg = m_adc[channel];

    *out_value = (int16_t) reg;

    return SUCCESS;
}
