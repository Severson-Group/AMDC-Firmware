#ifndef TIMING_MANAGER_H
#define TIMING_MANAGER_H

#include "drv/hardware_targets.h"
#include "usr/user_config.h"
#include <stdbool.h>
#include <stdint.h>

#define INTC_INTERRUPT_ID_0      61  // IRQ_F2P[0:0]
#define INTC_INTERRUPT_ID_1      62  // IRQ_F2P[1:1]
#define ISR0_PRIORITY            0xA0
#define ISR1_PRIORITY            0xA8
#define ISR_RISING_EDGE          0x3
#define DEFAULT_SENSOR_ENABLE    0x00
#define FPGA_FREQ                200
#define LOWER_16_MASK            0x00FF
#define NUM_SENSORS              6
#define TIMING_MANAGER_BASE_ADDR XPAR_AMDC_TIMING_MANAGER_0_S00_AXI_BASEADDR
#define UPPER_16_SHIFT           16

enum {
    EDDY_0 = 1,
    EDDY_1 = 2,
    EDDY_2 = 3,
    EDDY_3 = 4,
    ENCODER = 5,
    ADC = 6
};

typedef uint8_t sensor_t;

uint32_t interrupt_system_init();
void timing_manager_init();
void timing_manager_set_ratio(uint32_t ratio, uint32_t base_addr);
void nops(uint32_t num);
void timing_manager_select_sensors(uint8_t enable_bits, uint32_t base_addr);
void timing_manager_trigger_on_pwm_both(uint32_t base_addr);
void timing_manager_trigger_on_pwm_high(uint32_t base_addr);
void timing_manager_trigger_on_pwm_low(uint32_t base_addr);
void timing_manager_trigger_on_pwm_clear(uint32_t base_addr);
void timing_manager_get_time_per_sensor(uint32_t *time);
void isr0(void *intc_inst_ptr);
void isr1(void *intc_inst_ptr);
void timing_manager_get_time_per_sensor(sensor_t sensor, uint32_t base_addr);
void timing_manager_sensor_stats();
statistics_t* timing_manager_get_stats_per_sensor(sensor_t sensor);
int timing_test();

#endif // TIMING_MANAGER_H
