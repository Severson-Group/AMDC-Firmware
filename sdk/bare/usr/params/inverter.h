#ifndef INVERTER_H
#define INVERTER_H

#include <stdint.h>

void inverter_saturate_to_Vdc(double *voltage);
void inverter_set_voltage(uint8_t pwm_idx, double voltage, double current);

void inverter_set_dtc(double dcomp, double currLimit);
void inverter_set_Vdc(double Vdc);
double inverter_get_Vdc(void);

#endif // INVERTER_H
