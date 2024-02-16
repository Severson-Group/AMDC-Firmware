#ifndef TIMING_MANAGER_H
#define TIMING_MANAGER_H

#include "drv/hardware_targets.h"
#include "sys/statistics.h"
#include "usr/user_config.h"
#include "xparameters.h"

#include <stdbool.h>
#include <stdint.h>

#define INTC_INTERRUPT_ID_0      61 // IRQ_F2P[0:0]
#define INTC_INTERRUPT_ID_1      62 // IRQ_F2P[1:1]
#define ISR0_PRIORITY            0x08
#define ISR1_PRIORITY            0x00
#define ISR_RISING_EDGE          0x3
#define DEFAULT_SENSOR_ENABLE    0x00
#define FPGA_FREQ                200
#define LOWER_16_MASK            0x00FF
#define NUM_SENSORS              6
#define TIMING_MANAGER_BASE_ADDR XPAR_AMDC_TIMING_MANAGER_0_S00_AXI_BASEADDR
#define UPPER_16_SHIFT           16

enum { EDDY_0 = 1, EDDY_1 = 2, EDDY_2 = 3, EDDY_3 = 4, ENCODER = 5, ADC = 6 };

typedef uint8_t sensor_t;

// Initialization
int interrupt_system_init();
void timing_manager_init();
void nops(uint32_t num);

// Set user ratio
void timing_manager_set_ratio(uint32_t ratio);

// Enable sensors
void timing_manager_select_sensors(uint8_t enable_bits);
void timing_manager_enable_eddy_1();
void timing_manager_enable_eddy_2();
void timing_manager_enable_eddy_3();
void timing_manager_enable_eddy_4();
void timing_manager_enable_encoder();
void timing_manager_enable_adc();

// PWM trigger
void timing_manager_trigger_on_pwm_both();
void timing_manager_trigger_on_pwm_high();
void timing_manager_trigger_on_pwm_low();
void timing_manager_trigger_on_pwm_clear();

// Timing acquisition
void isr_0(void *intc_inst_ptr);
void isr_1(void *intc_inst_ptr);
uint16_t timing_manager_get_time_per_sensor(sensor_t sensor);
void timing_manager_sensor_stats();
statistics_t *timing_manager_get_stats_per_sensor(sensor_t sensor);

void test_int();

#endif // TIMING_MANAGER_H
