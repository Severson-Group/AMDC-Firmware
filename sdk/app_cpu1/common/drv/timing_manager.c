#include "drv/timing_manager.h"
#include "drv/clock.h"
#include "sys/scheduler.h"
#include "usr/user_config.h"
#include "xil_assert.h"
#include "xil_exception.h"
#include "xil_io.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xscugic.h"
#include <stdint.h>
#include <stdio.h>

// Instance of the interrupt controller
static XScuGic intc;

// Address of AXI PL interrupt generator (timing manger IP)
volatile uint32_t *baseaddr_p = (uint32_t *) TIMING_MANAGER_BASE_ADDR;

// Array of statistics for each sensor
statistics_t sensor_stats[NUM_SENSORS];

/*
 * Sets up the interrupt system and enables interrupts for IRQ_F2P[0]
 */
int timing_manager_interrupt_system_init(void)
{
    int result;
    XScuGic *intc_instance_ptr = &intc;
    XScuGic_Config *intc_config;
    // Get config for interrupt controller
    intc_config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
    if (intc_config == NULL) {
        return XST_FAILURE;
    }

    // Initialize interrupt controller driver
    result = XScuGic_CfgInitialize(intc_instance_ptr, intc_config, intc_config->CpuBaseAddress);
    if (result != XST_SUCCESS) {
        return result; // Exit setup with bad result
    }

    // Set priority of IRQ_F2P[0:0] and a trigger for a rising edge 0x3
    XScuGic_SetPriorityTriggerType(intc_instance_ptr, TM_INTERRUPT_ID, TM_ISR_PRIORITY, ISR_RISING_EDGE);

    // Send interrupt to CPU 1
    XScuGic_InterruptMaptoCpu(intc_instance_ptr, 1, TM_INTERRUPT_ID);

    // Connect ISR0 to the interrupt controller
    result = XScuGic_Connect(
        intc_instance_ptr, TM_INTERRUPT_ID, (Xil_ExceptionHandler) timing_manager_isr, (void *) intc_instance_ptr);
    if (result != XST_SUCCESS) {
        return result; // Exit setup with bad result
    }

    // Enable interrupts for IRQ_F2P[0:0]
    XScuGic_Enable(intc_instance_ptr, TM_INTERRUPT_ID);

    // Initialize the exception table and register the interrupt controller handler with the exception table
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(
        XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, intc_instance_ptr);

    // Enable non-critical exceptions
    Xil_ExceptionEnable();
    // Successfully initialized ISR
    return XST_SUCCESS;
}

/*
 * Initialize the timing manager driver. This initializes
 * the interrupt system and sets the default configurations
 */
void timing_manager_init(void)
{
    printf("TIMING MANAGER:\tInitializing...\n");
    // Initializes the interrupt used to update the sensor data and run scheduler tasks
    int result = 0;
    result = timing_manager_interrupt_system_init();
    if (result != XST_SUCCESS) {
        printf("Error initializing interrupt system.");
    }

    // Set the interrupt source for the scheduler based on user configuration settings
    timing_manager_set_scheduler_source();

    // Set the timing manager to automatic triggering
    // (this call is redundant, as the FPGA should reset slv_reg0 to 0x0000_0001)
    timing_manager_set_mode(TM_AUTOMATIC);

    // Default event qualifier is PWM carrier low
    timing_manager_trigger_on_pwm_low();

    // Set the user ratio for the trigger
    timing_manager_set_ratio(DEFAULT_PWM_RATIO);

    // Enable selected sensors for timing acquisition
    timing_manager_select_sensors(DEFAULT_SENSOR_ENABLE);

    // Initialize the stats
    for (int i = 0; i < NUM_SENSORS; i++) {
        // ensure each sensor has their own statistics
        statistics_init(&sensor_stats[i]);
    }

    // Clear interrupt to ensure that it does not get stuck at 1
    timing_manager_clear_isr();
}

/* Sets the timing manager's trigger mode by writing to the mode bit in the trigger
 * configuration register. Accepted arguments for mode are AUTOMATIC (default) and MANUAL
 *
 * AUTOMATIC mode will trigger all enabled sensors according to the settings set by the
 * timing_manager_set_ratio() when all enabled sensors are done sampling
 *
 * MANUAL mode will only trigger sensors when the user makes a call to the
 * timing_manager_send_manual_trigger() function, although manual triggers will still be aligned
 * to the peaks and/or valleys of the PWM carrier
 */
void timing_manager_set_mode(trigger_mode_e mode)
{
    if (mode == TM_AUTOMATIC) {
        // AUTOMATIC: Set slv_reg0[0]
        Xil_Out32(TIMING_MANAGER_BASE_ADDR, (Xil_In32(TIMING_MANAGER_BASE_ADDR) | 0x00000001));
    } else if (mode == TM_MANUAL) {
        // MANUAL: Clear slv_reg0[0]
        Xil_Out32(TIMING_MANAGER_BASE_ADDR, (Xil_In32(TIMING_MANAGER_BASE_ADDR) & 0xFFFFFFFE));
    } else {
        // Do nothing
    }
}

trigger_mode_e timing_manager_get_mode(void)
{
    return (trigger_mode_e)(Xil_In32(TIMING_MANAGER_BASE_ADDR) & 0x1);
}

/* timing_manager_send_manual_trigger() can be called to trigger all enabled sensors once,
 * on the next qualifying PWM peak/valley. Calling this function is only effective if
 * the user has set the timing manager to MANUAL mode by calling timing_manager_set_mode(MANUAL)
 */
void timing_manager_send_manual_trigger(void)
{
    // A manual trigger is initiated in the FPGA by flipping slv_reg0[1]
    Xil_Out32(TIMING_MANAGER_BASE_ADDR, Xil_In32(TIMING_MANAGER_BASE_ADDR) ^ 0x00000002);
}

/*
 * Sepcify the interrupt source of the scheduler ISR:
 *
 * Mode 0 uses the timing manager's 'trigger' signal, i.e. the control
 * frequency based on the PWM carrier frequency and the specified user ratio.
 *
 * Mode 1 uses the timing manager's 'all_done' signal, calling the scheduler
 * when all the sensors are done with acquisition. When no sensors are enabled,
 * the scheduler is called as in mode 0 (based on the trigger). This mode
 * supports reporting of the timing for each sensor
 */
void timing_manager_set_scheduler_source(void)
{
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_ISR_REG_OFFSET;
#if USER_CONFIG_ISR_SOURCE == 0
    // Clear slv_reg4[1]
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) & 0xFFFFFFFD));
#elif USER_CONFIG_ISR_SOURCE == 1
    // Set slv_reg4[1]
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | 0x00000002));
#endif
}

/*
 * ISR for IRQ_F2P[0:0]; updates the timing statistics for each
 * sensor and updates the state of the scheduler
 */
void timing_manager_isr(void *intc_inst_ptr)
{
    // Push stats for each sensor
    timing_manager_sensor_stats();
    // Increment elapsed time and run tasks
    scheduler_tick();
    // Clear the interrupt once done handling ISR
    timing_manager_clear_isr();
}

/*
 * Clear the interrupt once the ISR
 * has been called
 */
void timing_manager_clear_isr(void)
{
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_ISR_REG_OFFSET;
    // Set slv_reg4[0]
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | 0x00000001));
    // Clear slv_reg4[0]
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) & 0xFFFFFFFE));
}

/*
 * Gets the time (in us) between ISR calls
 */
double timing_manager_get_tick_delta(void)
{
    double time = 0.0;
    uint32_t clock_cycles;
    // Get the number of clock cycles since the last ISR call to convert to us
    clock_cycles = Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_ISR_TIME_REG_OFFSET);
    time = (double) clock_cycles / CLOCK_FPGA_CLK_FREQ_MHZ;
    return time;
}

/*
 * Set the user-defined ratio for the event qualifier
 * This ratio determines the frequency of an interrupt
 * being sent based on the specified event qualifier
 */
void timing_manager_set_ratio(uint32_t ratio)
{
    // Get the current address for the target config register
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_RATIO_CFG_REG_OFFSET;
    // Assign the ratio to the config register
    Xil_Out32(config_reg_addr, ratio);
}

/*
 * Enables/disables all user-requested sensors for timing acquisition, according to provided enable bits.
 * Can be used to disable all sensors if enable_bits argument is 0x0000.
 */
void timing_manager_select_sensors(uint16_t enable_bits)
{
    // Get the address for the enable bit config register
    uint32_t enable_reg_addr = TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_ENABLE_CFG_REG_OFFSET;
    // Assign the enable bits to the config register
    Xil_Out32(enable_reg_addr, enable_bits);
}

/*
 * Enables a single user-requested sensor (by configuring the appropriate bit in the timing manager)
 */
void timing_manager_enable_sensor(sensor_e sensor)
{
    // Get the address for the enable configuration register
    uint32_t enable_reg_addr = TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_ENABLE_CFG_REG_OFFSET;

    // IMPORTANT:
    //   sensor_e enumeration is a critical enumeration where the enumeration order matters!
    //   Since the ADC is "sensor 0", its enable bit in the timing manager's enable
    //   configuration register is slv_reg1[0]
    //   Since the ENCODER is "sensor 1", its enable bit is slv_reg1[1]
    //   ... and so on for all the other sensors defined in the enumeration ...
    //   The enumeration MUST be kept in agreement with the sensor order in the FPGA, so
    //   that we can use this simple bit-shift to set the correct bit in the FPGA
    uint32_t enable_bit = 0x1 << sensor;
    Xil_Out32(enable_reg_addr, (Xil_In32(enable_reg_addr) | enable_bit));
}

/*
 * Set the trigger on either PWM carrier high or low
 */
void timing_manager_trigger_on_pwm_both(void)
{
    // Get the current address of the config register
    uint32_t pwm_config_reg_addr = TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_PWM_CFG_REG_OFFSET;
    // Set both the carrier high and low trigger bits
    Xil_Out32(pwm_config_reg_addr, 0x0003);
}

/*
 * Set the trigger on PWM carrier high only
 */
void timing_manager_trigger_on_pwm_high(void)
{
    // Get the current address of the config register
    uint32_t pwm_config_reg_addr = TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_PWM_CFG_REG_OFFSET;
    // Set only the carrier high bit
    Xil_Out32(pwm_config_reg_addr, 0x0001);
}

/*
 * Set the trigger on PWM carrier low only
 */
void timing_manager_trigger_on_pwm_low(void)
{
    // Get the current address of the config register
    uint32_t pwm_config_reg_addr = TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_PWM_CFG_REG_OFFSET;
    // Set only the carrier low bit
    Xil_Out32(pwm_config_reg_addr, 0x0002);
}

/*
 * Clear the PWM event qualifier
 */
void timing_manager_trigger_on_pwm_clear(void)
{
    // Get the current address of the config register
    uint32_t pwm_config_reg_addr = TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_PWM_CFG_REG_OFFSET;
    // Clear both the carrier high and low trigger bits
    Xil_Out32(pwm_config_reg_addr, 0x0000);
}

/*
 * Get the acquisition time for the requested sensor, in nanoseconds
 */
double timing_manager_get_time_per_sensor(sensor_e sensor)
{
    uint32_t clock_cycles = 0;
    double time = 0;

    if (sensor == ADC)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_ADC_ENC_TIME_REG_OFFSET)) & LOWER_16_MASK;
    else if (sensor == ENCODER)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_ADC_ENC_TIME_REG_OFFSET)) >> UPPER_16_SHIFT;
    else if (sensor == AMDS_1)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_AMDS_01_TIME_REG_OFFSET)) & LOWER_16_MASK;
    else if (sensor == AMDS_2)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_AMDS_01_TIME_REG_OFFSET)) >> UPPER_16_SHIFT;
    else if (sensor == AMDS_3)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_AMDS_23_TIME_REG_OFFSET)) & LOWER_16_MASK;
    else if (sensor == AMDS_4)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_AMDS_23_TIME_REG_OFFSET)) >> UPPER_16_SHIFT;
    else if (sensor == EDDY_1)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_EDDY_01_TIME_REG_OFFSET)) & LOWER_16_MASK;
    else if (sensor == EDDY_2)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_EDDY_01_TIME_REG_OFFSET)) >> UPPER_16_SHIFT;
    else if (sensor == EDDY_3)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_EDDY_23_TIME_REG_OFFSET)) & LOWER_16_MASK;
    else if (sensor == EDDY_4)
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + TIMING_MANAGER_EDDY_23_TIME_REG_OFFSET)) >> UPPER_16_SHIFT;

    // Convert clock cycles to time in us using 200 MHz FPGA clock frequency
    time = (double) clock_cycles / CLOCK_FPGA_CLK_FREQ_MHZ;
    return time;
}

/*
 * Initializes and pushes statistics for each sensor, if enabled.
 */
void timing_manager_sensor_stats(void)
{
    static int times_called;
    times_called++;
    // Iterate through for each sensor push the status
    for (int i = 0; i < NUM_SENSORS; i++) {
        double val = timing_manager_get_time_per_sensor(i);
        statistics_push(&sensor_stats[i], val);
    }
}

/*
 * Takes in a sensor value, and returns a reference to the struct
 * containing the stats for that sensor
 */
statistics_t *timing_manager_get_stats_per_sensor(sensor_e sensor)
{
    // Get pointer to the stats for the specified sensor
    return &sensor_stats[sensor];
}
