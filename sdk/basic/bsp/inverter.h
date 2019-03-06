#ifndef INVERTER_H
#define INVERTER_H

#include <stdint.h>

//typedef enum {
//	INV_1 = 1,
//	INV_2,
//	INV_3,
//	INV_4,
//	INV_5,
//	INV_6,
//	INV_7,
//	INV_8,
//} inverter_e;
//
//typedef struct inverter_status_t {
//	uint8_t ready;
//	uint8_t fault_desat;
//	uint8_t fault_temp;
//} inverter_status_t;

void inverter_init(void);

void inverter_set_duty(uint8_t idx, uint8_t value);

//void inverter_set_duty_ratio(inverter_e inv, uint8_t pwm_idx, uint8_t value);
//void inverter_set_dead_time (uint8_t inv_idx, uint8_t pwm_idx, uint8_t value);

//void inverter_get_status(uint8_t idx, inverter_status_t *status);

#endif // INVERTER_H
