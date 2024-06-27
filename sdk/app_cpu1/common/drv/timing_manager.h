#ifndef TIMING_MANAGER_H
#define TIMING_MANAGER_H

#include "drv/hardware_targets.h"
#include "sys/statistics.h"
#include "usr/user_config.h"
#include "xparameters.h"

#include <stdbool.h>
#include <stdint.h>

// Interrupt configurations
#define TM_INTERRUPT_ID    (61) // IRQ_F2P[0:0]
#define TM_ISR_PRIORITY    (0x08)
#define TM_ISR_RISING_EDGE (3)

// Timing manager configurations
#define TM_DEFAULT_SENSOR_ENABLE (0x0000)
#define TM_DEFAULT_PWM_RATIO     (10)
#define TM_NUM_SENSORS           (10)

// Use to read separate halves of 32-bit slave registers
#define TM_LOWER_16_MASK  (0x0000FFFF)
#define TM_UPPER_16_SHIFT (16)

// Expected times (us) for all the sensors based on default values
#define TM_ADC_DEFAULT_TIME        (0.82)
#define TM_ENCODER_DEFAULT_TIME    (0.005)
#define TM_AMDS_DEFAULT_TIME       (11.0)
#define TM_EDDY_DEFAULT_TIME       (4.11)
#define TM_MAX_DEFAULT_SENSOR_TIME (TM_AMDS_DEFAULT_TIME)

// Macro functions to check which sensors are enabled
#define GET_SENSOR_ENABLE_BITS (Xil_In16(TM_BASE_ADDR + TM_SENSOR_EN_CFG_REG_OFFSET))
#define IS_ADC_ENABLED         ((bool) GET_SENSOR_ENABLE_BITS & 0x1)
#define IS_ENCODER_ENABLED     ((bool) GET_SENSOR_ENABLE_BITS & 0x2)
#define IS_ANY_AMDS_ENABLED    ((bool) GET_SENSOR_ENABLE_BITS & 0x3C)
#define IS_ANY_EDDY_ENABLED    ((bool) GET_SENSOR_ENABLE_BITS & 0x3C0)

#define TM_BASE_ADDR (XPAR_AMDC_TIMING_MANAGER_0_S00_AXI_BASEADDR)

// Slave Register Offsets
#define TM_TRIG_CFG_CNT_REG_OFFSET  (0)
#define TM_SENSOR_EN_CFG_REG_OFFSET (1 * sizeof(uint32_t))
#define TM_SENSOR_STS_REG_OFFSET    (2 * sizeof(uint32_t))
#define TM_RATIO_CFG_REG_OFFSET     (3 * sizeof(uint32_t))
#define TM_PWM_CFG_REG_OFFSET       (4 * sizeof(uint32_t))
#define TM_ISR_REG_OFFSET           (5 * sizeof(uint32_t))
#define TM_ISR_TIME_REG_OFFSET      (6 * sizeof(uint32_t))
#define TM_ADC_ENC_TIME_REG_OFFSET  (7 * sizeof(uint32_t))
#define TM_AMDS_01_TIME_REG_OFFSET  (8 * sizeof(uint32_t))
#define TM_AMDS_23_TIME_REG_OFFSET  (9 * sizeof(uint32_t))
#define TM_EDDY_01_TIME_REG_OFFSET  (10 * sizeof(uint32_t))
#define TM_EDDY_23_TIME_REG_OFFSET  (11 * sizeof(uint32_t))

// This enumeration is used by the global "pwm_update_rate" to remember if we are triggering
// on the high OR low (single update rate) or BOTH high and low (double update rate)
typedef enum { TM_PWM_SINGLE = 1, TM_PWM_DOUBLE = 2 } pwm_update_rate_e;

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
int timing_manager_interrupt_system_init(void);
void timing_manager_init(void);

// Mode: Automatic vs Manual Triggering
void timing_manager_set_mode(trigger_mode_e mode);
trigger_mode_e timing_manager_get_mode(void);
void timing_manager_send_manual_trigger(void);
uint32_t timing_manager_get_trigger_count(void);

// Set/get user ratio
void timing_manager_set_ratio(uint32_t ratio);
uint32_t timing_manager_get_ratio(void);

// Enable sensors
void timing_manager_select_sensors(uint16_t enable_bits);
void timing_manager_enable_sensor(sensor_e sensor);

// Check done status of sensors
bool timing_manager_is_sensor_done(sensor_e sensor);
bool timing_manager_are_sensors_all_done(void);

// PWM trigger
void timing_manager_trigger_on_pwm_both(void);
void timing_manager_trigger_on_pwm_high(void);
void timing_manager_trigger_on_pwm_low(void);

// Timing acquisition
void timing_manager_isr(void *intc_inst_ptr);
void timing_manager_set_scheduler_source(void);
double timing_manager_get_tick_delta(void);
double timing_manager_expected_tick_delta(void);
double timing_manager_get_time_per_sensor(sensor_e sensor);
void timing_manager_sensor_stats(void);
statistics_t *timing_manager_get_stats_per_sensor(sensor_e sensor);
void timing_manager_clear_isr(void);

#endif // TIMING_MANAGER_H
