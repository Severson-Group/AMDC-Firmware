#ifndef INVERTER_H
#define INVERTER_H

#include <stdint.h>

#define VOLTAGE_DC_BUS	(50.0) // V

void inverter_init(void);

void inverter_saturate_to_Vdc(double *voltage);
void inverter_set_voltage(uint8_t pwm_idx, double voltage, double current);

void inverter_set_dtc(double dcomp, double current);

#endif // INVERTER_H
