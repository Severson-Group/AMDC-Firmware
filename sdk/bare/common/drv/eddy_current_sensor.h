#ifndef EDDY_CURRENT_SENSOR_H
#define EDDY_CURRENT_SENSOR_H

#include <stdbool.h>
#include <stdint.h>

void eddy_current_sensor_init(void);

void eddy_current_sensor_enable(void);
void eddy_current_sensor_disable(void);

void eddy_current_sensor_set_sample_rate_hz(double sample_rate_hz);

int32_t eddy_current_sensor_get_x_bits(void);
int32_t eddy_current_sensor_get_y_bits(void);

#endif // EDDY_CURRENT_SENSOR_H
