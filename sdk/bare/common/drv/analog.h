#ifndef ANALOG_H
#define ANALOG_H

#include "usr/user_defines.h"
#include <stdbool.h>
#include <stdint.h>

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

// REV C hardware is the only hardware
// which supports more than 8 analog inputs.
#if USER_CONFIG_HARDWARE_TARGET == 3
    ANALOG_IN9,
    ANALOG_IN10,
    ANALOG_IN11,
    ANALOG_IN12,
    ANALOG_IN13,
    ANALOG_IN14,
    ANALOG_IN15,
    ANALOG_IN16,
#endif // USER_CONFIG_HARDWARE_TARGET

    // Keep this as last entry!
    ANALOG_NUM_CHANNELS,
} analog_channel_e;

typedef enum {
    ANALOG_CLKDIV2 = 0,
    ANALOG_CLKDIV4,
    ANALOG_CLKDIV8,
    ANALOG_CLKDIV16,
} analog_clkdiv_e;

bool analog_is_valid_channel(analog_channel_e channel);
bool analog_is_valid_clkdiv(analog_clkdiv_e div);

void analog_init(void);

int analog_set_clkdiv(analog_clkdiv_e div);
void analog_get_clkdiv(analog_clkdiv_e *out_div);

int analog_getf(analog_channel_e channel, float *out_value);
int analog_geti(analog_channel_e channel, int16_t *out_value);

void analog_set_pwm_sync(bool sync_to_carrier_high, bool sync_to_carrier_low);

// TODO(NP): Implement digital low-pass filtering in FPGA,
//           then, implement C driver which looks like this:
// void analog_set_filter(analog_channel_e channel, ...);

#endif // ANALOG_H
