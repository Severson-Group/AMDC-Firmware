#ifndef INVERTER_H
#define INVERTER_H

#include <stdint.h>

void inverter_saturate_to_Vdc(int inv_idx, double *phase_voltage);
void inverter_set_voltage(int inv_idx, uint8_t pwm_idx, double phase_voltage);

void inverter_set_Vdc(int inv_idx, double Vdc);
double inverter_get_Vdc(int inv_idx);

#endif // INVERTER_H
