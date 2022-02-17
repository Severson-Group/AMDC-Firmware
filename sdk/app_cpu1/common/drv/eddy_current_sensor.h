#ifndef eddy_current_sensor_H
#define eddy_current_sensor_H

#include <stdbool.h>
#include <stdint.h>

#include "xparameters.h"

#define EDDY_CURRENT_SENSOR_1_BASE_ADDR (XPAR_HIER_GPIO_0_AMDC_EDDY_CURRENT_SE_0_S00_AXI_BASEADDR)
#define EDDY_CURRENT_SENSOR_2_BASE_ADDR (XPAR_HIER_GPIO_1_AMDC_EDDY_CURRENT_SE_0_S00_AXI_BASEADDR)
#define EDDY_CURRENT_SENSOR_3_BASE_ADDR (XPAR_HIER_GPIO_2_AMDC_EDDY_CURRENT_SE_0_S00_AXI_BASEADDR)
#define EDDY_CURRENT_SENSOR_4_BASE_ADDR (XPAR_HIER_GPIO_3_AMDC_EDDY_CURRENT_SE_0_S00_AXI_BASEADDR)

void eddy_current_sensor_init(void);

void eddy_current_sensor_enable(uint32_t base_addr);
void eddy_current_sensor_disable(uint32_t base_addr);

void eddy_current_sensor_set_sample_rate(uint32_t base_addr, double sample_rate);
void eddy_current_sensor_set_divider(uint32_t base_addr, uint8_t divider);

double eddy_current_sensor_read_x_voltage(uint32_t base_addr);
double eddy_current_sensor_read_y_voltage(uint32_t base_addr);

#endif // EDDY_CURRENT_SENSOR_H
