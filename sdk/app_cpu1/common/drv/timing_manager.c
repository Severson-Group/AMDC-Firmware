#include "drv/timing_manager.h"
#include "drv/clock.h"
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
 * Sets up the interrupt system and enables interrupts for IRQ_F2P[1:0]
 */
int interrupt_system_init(void)
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

    // Initialize the exception table and register the interrupt controller handler with the exception table
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(
        XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, intc_instance_ptr);

    // Set priority of IRQ_F2P[0:0] to 0xA0 and a trigger for a rising edge 0x3
    XScuGic_SetPriorityTriggerType(intc_instance_ptr, INTC_INTERRUPT_ID_0, ISR0_PRIORITY, ISR_RISING_EDGE);

    XScuGic_InterruptMaptoCpu(intc_instance_ptr, 1, INTC_INTERRUPT_ID_0);

    // Connect ISR0 to the interrupt controller
    result = XScuGic_Connect(
        intc_instance_ptr, INTC_INTERRUPT_ID_0, (Xil_ExceptionHandler) isr_0, (void *) intc_instance_ptr);
    if (result != XST_SUCCESS) {
        return result; // Exit setup with bad result
    }

    // Enable interrupts for IRQ_F2P[0:0]
    XScuGic_Enable(intc_instance_ptr, INTC_INTERRUPT_ID_0);

    // Enable non-critical exceptions
    Xil_ExceptionEnable();
    // Successfully initialized ISR
    return XST_SUCCESS;
}

/*
 * Initialize the timing manager driver. This initializes
 * the interrupt system and sets the default PWM event qualifier
 */
void timing_manager_init(void)
{
    printf("TIMING MANAGER:\tInitializing...\n");
    // Initialize interrupts
    int result = 0;
    result = interrupt_system_init();

    if (result != XST_SUCCESS) {
        printf("Error initializing interrupt system.");
    }

    // Set the timing manager to automatic triggering
    // (this call is redundant, as the FPGA should reset slv_reg0 to 0x0000_0001)
    timing_manager_set_mode(TM_AUTOMATIC);

    // Default event qualifier is PWM carrier high AND low
    timing_manager_trigger_on_pwm_both();

    // Set the user ratio for the trigger
    timing_manager_set_ratio(DEFAULT_PWM_RATIO);

    // Enable selected sensors for timing acquisition
    timing_manager_select_sensors(DEFAULT_SENSOR_ENABLE);

    // Initialize the stats
    for (int i = 0; i < NUM_SENSORS; i++) {
        // ensure each sensor has their own statistics
        statistics_init(&sensor_stats[i]);
    }
}

/* Sets the timing manager's trigger mode by writing to the mode bit in the configuration
 * register (slv_reg0). Accepted arguments for mode are AUTOMATIC (default) and MANUAL
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
 * ISR for IRQ_F2P[0:0]. Called when sched_isr in timing
 * manager is set to 1, e.g. when all of the sensors are
 * done and the time has been collected.
 */
void isr_0(void *intc_inst_ptr)
{
    // Push stats for each sensor
    timing_manager_sensor_stats();
    timing_manager_clear_isr();
}

/*
 * Clear the interrupt once the ISR
 * has been called
 */
void timing_manager_clear_isr(void)
{
    Xil_Out32(TIMING_MANAGER_BASE_ADDR + (8 * sizeof(uint32_t)), 1);
    Xil_Out32(TIMING_MANAGER_BASE_ADDR + (8 * sizeof(uint32_t)), 0);
}

/*
 * Set the user-defined ratio for the event qualifier
 * This ratio determines the frequency of an interrupt
 * being sent based on the specified event qualifier
 */
void timing_manager_set_ratio(uint32_t ratio)
{
    // Get the current address for the target config register (slv_reg2)
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (2 * sizeof(uint32_t));
    // Assign the ratio to the config register
    Xil_Out32(config_reg_addr, ratio);
}

/*
 * Enables/disables all user-requested sensors for timing acquisition, according to provided enable bits.
 * Can be used to disable all sensors if enable_bits argument is 0x0000.
 */
void timing_manager_select_sensors(uint16_t enable_bits)
{
    // There are only 10 sensors supported in the timing manager, so the first 6 enable bits should always be cleared
    if (enable_bits & 0xFC00) {
        // If here, there was a non-zero bit in the most significant six bits
        xil_printf("Timing Manager: Invalid enable_bits argument passed to timing_manager_select_sensors()");
    } else {
        // Get the current address for the target config register (slv_reg1)
        uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (1 * sizeof(uint32_t));
        // Assign the enable bits to the config register
        Xil_Out32(config_reg_addr, enable_bits);
    }
}

/*
 * Enables AMDS in a single GPIO port
 */
void timing_manager_enable_amds(uint8_t amds_port_number)
{
    // Get the address for the target config register (slv_reg1)
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (1 * sizeof(uint32_t));

    if (amds_port_number >= 1 && amds_port_number <= 4) {
        // AMDS enables are bits 0-3 in slv_reg1,
        // therefore enabling AMDS in GPIO Port 1 (1-indexed) is bit 0
        uint32_t enable_bits = 0x1 << (amds_port_number - 1);
        Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | enable_bits));
    } else {
        xil_printf("Timing Manager: AMDS port number should be in the range 1 - 4");
    }
}

/*
 * Enables user-requested eddy current sensor in a single GPIO port
 */
void timing_manager_enable_eddy_current_sensor(uint8_t eddy_sensor_port_number)
{
    // Get the address for the target config register (slv_reg1)
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (1 * sizeof(uint32_t));

    if (eddy_sensor_port_number >= 1 && eddy_sensor_port_number <= 4) {
        // Eddy Current Sensors enables are bits 4-7 in slv_reg1,
        // therefore enabling eddy current sensor 1 (the first GPIO port) is bit 4
        uint32_t enable_bits = 0x1 << (eddy_sensor_port_number + 3);
        Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | enable_bits));
    } else {
        xil_printf("Timing Manager: Eddy Current Sensor port number should be in the range 1 - 4");
    }
}

/*
 * Enable encoder
 */
void timing_manager_enable_encoder(void)
{
    // Get the address for the target config register (slv_reg1)
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (1 * sizeof(uint32_t));
    // Write to the config register
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | 0x100));
}

/*
 * Enable ADC
 */
void timing_manager_enable_adc(void)
{
    // Get the address for the target config register (slv_reg1)
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (1 * sizeof(uint32_t));
    // Write to the config register
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | 0x200));
}

/*
 * Set the trigger on either PWM carrier high or low
 */
void timing_manager_trigger_on_pwm_both(void)
{
    // Get the current address of the config register
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (3 * sizeof(uint32_t));
    // Set both the carrier high and low trigger bits
    Xil_Out32(config_reg_addr, 0x0003);
}

/*
 * Set the trigger on PWM carrier high only
 */
void timing_manager_trigger_on_pwm_high(void)
{
    // Get the current address of the config register
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (3 * sizeof(uint32_t));
    // Set only the carrier high bit
    Xil_Out32(config_reg_addr, 0x0001);
}

/*
 * Set the trigger on PWM carrier low only
 */
void timing_manager_trigger_on_pwm_low(void)
{
    // Get the current address of the config register
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (3 * sizeof(uint32_t));
    // Set only the carrier low bit
    Xil_Out32(config_reg_addr, 0x0002);
}

/*
 * Clear the PWM event qualifier
 */
void timing_manager_trigger_on_pwm_clear(void)
{
    // Get the current address of the config register
    uint32_t config_reg_addr = TIMING_MANAGER_BASE_ADDR + (3 * sizeof(uint32_t));
    // Clear both the carrier high and low trigger bits
    Xil_Out32(config_reg_addr, 0x0000);
}

/*
 * Get the acquisition time for the requested sensor, in nanoseconds
 */
double timing_manager_get_time_per_sensor(sensor_e sensor)
{
    uint32_t clock_cycles = 0;
    double time = 0;

    if (sensor == AMDS_1) {
        // Lower 16 bits of slave reg 11
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (11 * sizeof(uint32_t)))) & LOWER_16_MASK;
    } else if (sensor == AMDS_2) {
        // Upper 16 bits of slave reg 11
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (11 * sizeof(uint32_t)))) >> UPPER_16_SHIFT;
    } else if (sensor == AMDS_3) {
        // Lower 16 bits of slave reg 12
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (12 * sizeof(uint32_t)))) & LOWER_16_MASK;
    } else if (sensor == AMDS_4) {
        // Upper 16 bits of slave reg 12
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (12 * sizeof(uint32_t)))) >> UPPER_16_SHIFT;
    } else if (sensor == EDDY_1) {
        // Lower 16 bits of slave reg 5
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (5 * sizeof(uint32_t)))) & LOWER_16_MASK;
    } else if (sensor == EDDY_2) {
        // Upper 16 bits of slave reg 5
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (5 * sizeof(uint32_t)))) >> UPPER_16_SHIFT;
    } else if (sensor == EDDY_3) {
        // Lower 16 bits of slave reg 6
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (6 * sizeof(uint32_t)))) & LOWER_16_MASK;
    } else if (sensor == EDDY_4) {
        // Upper 16 bits of slave reg 6
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (6 * sizeof(uint32_t)))) >> UPPER_16_SHIFT;
    } else if (sensor == ENCODER) {
        // Lower 16 bits of slave reg 7
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (7 * sizeof(uint32_t)))) & LOWER_16_MASK;
    } else if (sensor == ADC) {
        // Upper 16 bits of slave reg 7
        clock_cycles = (Xil_In32(TIMING_MANAGER_BASE_ADDR + (7 * sizeof(uint32_t)))) >> UPPER_16_SHIFT;
    }
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
