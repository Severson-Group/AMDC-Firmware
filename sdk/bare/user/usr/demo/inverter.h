#ifndef INVERTER_H
#define INVERTER_H

#include <stdint.h>

void inverter_saturate_to_Vdc(double *phase_voltage);
void inverter_set_voltage(uint8_t pwm_idx, double phase_voltage);

void inverter_set_Vdc(double Vdc);
double inverter_get_Vdc(void);

#endif // INVERTER_H
