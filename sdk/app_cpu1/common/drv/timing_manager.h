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

// Slave Register Offsets
#define TIMING_MANAGER_TRIG_CFG_REG_OFFSET     (0)
#define TIMING_MANAGER_ENABLE_CFG_REG_OFFSET   (1 * sizeof(uint32_t))
#define TIMING_MANAGER_RATIO_CFG_REG_OFFSET    (2 * sizeof(uint32_t))
#define TIMING_MANAGER_PWM_CFG_REG_OFFSET      (3 * sizeof(uint32_t))
#define TIMING_MANAGER_ISR_REG_OFFSET          (4 * sizeof(uint32_t))
#define TIMING_MANAGER_TRIG_TIME_REG_OFFSET    (5 * sizeof(uint32_t))
#define TIMING_MANAGER_ADC_ENC_TIME_REG_OFFSET (6 * sizeof(uint32_t))
#define TIMING_MANAGER_AMDS_01_TIME_REG_OFFSET (7 * sizeof(uint32_t))
#define TIMING_MANAGER_AMDS_23_TIME_REG_OFFSET (8 * sizeof(uint32_t))
#define TIMING_MANAGER_EDDY_01_TIME_REG_OFFSET (9 * sizeof(uint32_t))
#define TIMING_MANAGER_EDDY_23_TIME_REG_OFFSET (10 * sizeof(uint32_t))

// IMPORTANT:
// This enumeration must be kept in sync with the order of sensors in the FPGA!
// Sensors built-on to the AMDC come first, followed by sensor peripherals that
// can be attached to a GPIO port for example.
// Make sure that any updates made to this enumeration are matched to an update
// in the timing manager IP's verilog!!
typedef enum {
    ADC = 0,
    ENCODER = 1,
    AMDS_1 = 2,
    AMDS_2 = 3,
    AMDS_3 = 4,
    AMDS_4 = 5,
    EDDY_1 = 6,
    EDDY_2 = 7,
    EDDY_3 = 8,
    EDDY_4 = 9
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
