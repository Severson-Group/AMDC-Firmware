#ifndef PWM_H
#define PWM_H

#include "usr/user_defines.h"
#include <stdbool.h>
#include <stdint.h>

#define PWM_DEFAULT_SWITCHING_FREQ_HZ (100000.0)
#define PWM_DEFAULT_DEADTIME_NS       (100.0)

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

typedef struct pwm_status_t {
    uint8_t ready;
    uint8_t fault_desat;
    uint8_t fault_temp;
} pwm_status_t;

bool pwm_is_valid_channel(pwm_channel_e channel);

void pwm_init(void);

void pwm_toggle_reset(void);

int pwm_set_switching_freq(double freq_hz);

int pwm_set_duty_raw(pwm_channel_e channel, uint16_t value);
int pwm_set_duty(pwm_channel_e channel, double duty);

int pwm_set_carrier_divisor(uint8_t divisor);
int pwm_set_carrier_max(uint16_t max);
int pwm_set_deadtime_ns(uint16_t deadtime);

#if USER_CONFIG_HARDWARE_TARGET == 3
int pwm_get_status(pwm_channel_e channel, pwm_status_t *status);
void pwm_get_all_flt_temp(uint8_t *flt_temp);
void pwm_get_all_flt_desat(uint8_t *flt_desat);
void pwm_get_all_rdy(uint8_t *rdy);
void pwm_set_all_rst(uint8_t rst);
#endif // USER_CONFIG_HARDWARE_TARGET

#endif // PWM_H
