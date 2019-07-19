#ifndef PWM_H
#define PWM_H

#include <stdint.h>

//typedef enum {
//  INV_1 = 1,
//  INV_2,
//  INV_3,
//  INV_4,
//  INV_5,
//  INV_6,
//  INV_7,
//  INV_8,
//} inverter_e;

typedef struct pwm_status_t {
    uint8_t ready;
    uint8_t fault_desat;
    uint8_t fault_temp;
} pwm_status_t;

void pwm_init(void);

void pwm_toggle_reset(void);

void pwm_set_switching_freq(double freq_hz);

void pwm_set_duty_raw(uint8_t idx, uint16_t value);
void pwm_set_duty(uint8_t idx, double duty);

void pwm_set_carrier_divisor(uint8_t divisor);
void pwm_set_carrier_max(uint16_t max);
void pwm_set_deadtime_ns(uint16_t deadtime);

void pwm_get_status(uint8_t idx, pwm_status_t *status);

void pwm_get_all_flt_temp(uint8_t *flt_temp);
void pwm_get_all_flt_desat(uint8_t *flt_desat);
void pwm_get_all_rdy(uint8_t *rdy);
void pwm_set_all_rst(uint8_t rst);

#endif // PWM_H
