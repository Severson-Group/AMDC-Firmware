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
#define LOWER_16_MASK            0x00FF
#define NUM_SENSORS              6
#define TIMING_MANAGER_BASE_ADDR XPAR_AMDC_TIMING_MANAGER_0_S00_AXI_BASEADDR
#define UPPER_16_SHIFT           16

enum { EDDY_0 = 0, EDDY_1 = 1, EDDY_2 = 2, EDDY_3 = 3, ENCODER = 4, ADC = 5 };

typedef uint8_t sensor_t;

// Initialization
int interrupt_system_init();
void timing_manager_init();

// Set user ratio
void timing_manager_set_ratio(uint32_t ratio);

// Enable sensors
void timing_manager_select_sensors(uint8_t enable_bits);
void timing_manager_enable_eddy_1(void);
void timing_manager_enable_eddy_2(void);
void timing_manager_enable_eddy_3(void);
void timing_manager_enable_eddy_4(void);
void timing_manager_enable_encoder(void);
void timing_manager_enable_adc(void);

// PWM trigger
void timing_manager_trigger_on_pwm_both(void);
void timing_manager_trigger_on_pwm_high(void);
void timing_manager_trigger_on_pwm_low(void);
void timing_manager_trigger_on_pwm_clear(void);

// Timing acquisition
void isr_0(void *intc_inst_ptr);
double timing_manager_get_time_per_sensor(sensor_t sensor);
void timing_manager_sensor_stats(void);
statistics_t *timing_manager_get_stats_per_sensor(sensor_t sensor);
void timing_manager_clear_isr(void);

#endif // TIMING_MANAGER_H
