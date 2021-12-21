#ifndef eddy_current_sensor_H
#define eddy_current_sensor_H

#include <stdbool.h>
#include <stdint.h>

void eddy_current_sensor_init(void);

void eddy_current_sensor_enable(void);

void eddy_current_sensor_disable(void);

void eddy_current_sensor_set_sample_rate(double);

void eddy_current_sensor_set_divider(uint8_t);

double eddy_current_sensor_read_x_voltage(void);

double eddy_current_sensor_read_y_voltage(void);

#endif // EDDY_CURRENT_SENSOR_H
