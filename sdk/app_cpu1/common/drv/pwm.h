#ifndef PWM_H
#define PWM_H

#include "drv/hardware_targets.h"
#include "usr/user_config.h"
#include <stdbool.h>
#include <stdint.h>

#define PWM_DEFAULT_SWITCHING_FREQ_HZ (100000.0)
#define PWM_DEFAULT_DEADTIME_NS       (100.0)

#define PWM_MAX_SWITCHING_FREQ_HZ (2e6)
#define PWM_MIN_SWITCHING_FREQ_HZ (2e3)

#define PWM_MAX_DEADTIME_NS ((1 << 16) - 1)
#define PWM_MIN_DEADTIME_NS (25)

typedef enum {
    // Keep first channel index at 0!
    PWM_OUT1 = 0,
    PWM_OUT2,
    PWM_OUT3,
    PWM_OUT4,
    PWM_OUT5,
    PWM_OUT6,
    PWM_OUT7,
    PWM_OUT8,
    PWM_OUT9,
    PWM_OUT10,
    PWM_OUT11,
    PWM_OUT12,
    PWM_OUT13,
    PWM_OUT14,
    PWM_OUT15,
    PWM_OUT16,
    PWM_OUT17,
    PWM_OUT18,
    PWM_OUT19,
    PWM_OUT20,
    PWM_OUT21,
    PWM_OUT22,
    PWM_OUT23,
    PWM_OUT24,

    // Keep this as last entry!
    PWM_NUM_CHANNELS,
} pwm_channel_e;

static inline bool pwm_is_valid_channel(pwm_channel_e channel)
{
    if (channel >= PWM_OUT1 && channel < PWM_NUM_CHANNELS) {
        return true;
    }

    return false;
}

void pwm_init(void);

void pwm_toggle_reset(void);
void pwm_set_all_rst(uint8_t rst);

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
int pwm_enable_hw(bool en);
#endif

int pwm_enable(void);
int pwm_disable(void);
bool pwm_is_enabled(void);

int pwm_set_switching_freq(double freq_hz);
int pwm_set_deadtime_ns(uint16_t deadtime);

double pwm_get_switching_freq(void);
uint16_t pwm_get_deadtime_ns(void);

int pwm_set_duty(pwm_channel_e channel, double duty);

void pwm_set_all_duty_midscale(void);

// Mux for PWM output pins
int pwm_mux_set_all_pins(uint32_t *config);
int pwm_mux_set_one_pin(uint32_t pwm_pin_idx, uint32_t config);

#endif // PWM_H
