#ifndef ANALOG_H
#define ANALOG_H

#include "drv/hardware_targets.h"
#include "usr/user_config.h"
#include <stdbool.h>
#include <stdint.h>

// From Vivado build output
#include "xparameters.h"

#define ANALOG_BASE_ADDR (XPAR_AMDC_ADC_0_S00_AXI_BASEADDR)

typedef enum {
    // Keep first channel index at 0!
    ANALOG_IN1 = 0,
    ANALOG_IN2,
    ANALOG_IN3,
    ANALOG_IN4,
    ANALOG_IN5,
    ANALOG_IN6,
    ANALOG_IN7,
    ANALOG_IN8,

    // Keep this as last entry!
    ANALOG_NUM_CHANNELS,
} analog_channel_e;

typedef enum {
    ANALOG_CLKDIV2 = 0,
    ANALOG_CLKDIV4,
    ANALOG_CLKDIV8,
    ANALOG_CLKDIV16,
} analog_clkdiv_e;

static inline bool analog_is_valid_channel(analog_channel_e channel)
{
    if (channel >= ANALOG_IN1 && channel < ANALOG_NUM_CHANNELS) {
        return true;
    }

    return false;
}

static inline bool analog_is_valid_clkdiv(analog_clkdiv_e div)
{
    if (div == ANALOG_CLKDIV2 || div == ANALOG_CLKDIV4 || div == ANALOG_CLKDIV8 || div == ANALOG_CLKDIV16) {
        return true;
    }

    return false;
}

void analog_init(uint32_t base_addr);

int analog_set_clkdiv(analog_clkdiv_e div);
void analog_get_clkdiv(analog_clkdiv_e *out_div);

int analog_getf(analog_channel_e channel, float *out_value);
int analog_geti(analog_channel_e channel, int16_t *out_value);

void analog_set_pwm_sync(bool sync_to_carrier_high, bool sync_to_carrier_low);

// TODO(NP): Implement digital low-pass filtering in FPGA,
//           then, implement C driver which looks like this:
// void analog_set_filter(analog_channel_e channel, ...);

#endif // ANALOG_H
