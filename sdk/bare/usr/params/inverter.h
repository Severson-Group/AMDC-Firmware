#ifndef INVERTER_H
#define INVERTER_H

#include <stdint.h>

#define VOLTAGE_DC_BUS	(20.0) // V

void inverter_saturate_to_Vdc(double *voltage);
void inverter_set_voltage(uint8_t pwm_idx, double voltage, double current);

void inverter_set_dtc(double dcomp, double tau);
void inverter_set_Vdc(double Vdc);


#endif // INVERTER_H
