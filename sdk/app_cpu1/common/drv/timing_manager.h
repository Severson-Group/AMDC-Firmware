#ifndef TIMING_MANAGER_H
#define TIMING_MANAGER_H

#include "drv/hardware_targets.h"
#include "sys/statistics.h"
#include "usr/user_config.h"
#include "xparameters.h"

#include <stdbool.h>
#include <stdint.h>

#define INTC_INTERRUPT_ID_0      (61) // IRQ_F2P[0:0]
#define ISR0_PRIORITY            (0x08)
#define ISR_RISING_EDGE          (0x3)
#define DEFAULT_SENSOR_ENABLE    (0x0000)
#define DEFAULT_PWM_RATIO        (0x1)
#define LOWER_16_MASK            (0x0000FFFF)
#define NUM_SENSORS              (10)
#define TIMING_MANAGER_BASE_ADDR (XPAR_AMDC_TIMING_MANAGER_0_S00_AXI_BASEADDR)
#define UPPER_16_SHIFT           (16)

typedef enum {
    AMDS_1 = 0,
    AMDS_2 = 1,
    AMDS_3 = 2,
    AMDS_4 = 3,
    EDDY_1 = 4,
    EDDY_2 = 5,
    EDDY_3 = 6,
    EDDY_4 = 7,
    ENCODER = 8,
    ADC = 9
} sensor_e;

typedef enum { TM_MANUAL = 0, TM_AUTOMATIC = 1 } trigger_mode_e;

// Initialization
int interrupt_system_init(void);
void timing_manager_init(void);

// Mode: Automatic vs Manual Triggering
void timing_manager_set_mode(trigger_mode_e mode);
trigger_mode_e timing_manager_get_mode(void);
void timing_manager_send_manual_trigger(void);

// Set user ratio
void timing_manager_set_ratio(uint32_t ratio);

// Enable sensors
void timing_manager_select_sensors(uint16_t enable_bits);
void timing_manager_enable_amds(uint8_t amds_port_number);
void timing_manager_enable_eddy_current_sensor(uint8_t eddy_current_sensor_port_number);
void timing_manager_enable_encoder(void);
void timing_manager_enable_adc(void);

// PWM trigger
void timing_manager_trigger_on_pwm_both(void);
void timing_manager_trigger_on_pwm_high(void);
void timing_manager_trigger_on_pwm_low(void);
void timing_manager_trigger_on_pwm_clear(void);

// Timing acquisition
void isr_0(void *intc_inst_ptr);
double timing_manager_get_time_per_sensor(sensor_e sensor);
void timing_manager_sensor_stats(void);
statistics_t *timing_manager_get_stats_per_sensor(sensor_e sensor);
void timing_manager_clear_isr(void);

#endif // TIMING_MANAGER_H
