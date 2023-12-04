#include "drv/timing_manager.h"
#include "sys/statistics.h"
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
volatile uint32_t *baseaddr_p = (uint32_t *) XPAR_AMDC_TIMING_MANAGER_0_S00_AXI_BASEADDR;

// Array of statistics for each sensor
statistics_t *sensor_stats[NUM_SENSORS];

/*
 * Sets up the interrupt system and enables interrupts for IRQ_F2P[1:0]
 */
uint32_t interrupt_system_init()
{
    uint32_t result;
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

    // Set priority of IRQ_F2P[0:0] to 0xA0 and a trigger for a rising edge 0x3
    XScuGic_SetPriorityTriggerType(intc_instance_ptr, INTC_INTERRUPT_ID_0, ISR0_PRIORITY, ISR_RISING_EDGE);
    
    // Connect ISR0 to the interrupt controller
    result = XScuGic_Connect(intc_instance_ptr, INTC_INTERRUPT_ID_0, (Xil_ExceptionHandler) isr0, (void *) &intc);
    if (result != XST_SUCCESS) {
        return result; // Exit setup with bad result
    }

    // Enable interrupts for IRQ_F2P[0:0]
    XScuGic_Enable(intc_instance_ptr, INTC_INTERRUPT_ID_0);

    // Set priority of IRQ_F2P[1:1] lower than [0:0] and a trigger for a rising edge 0x3
    XScuGic_SetPriorityTriggerType(intc_instance_ptr, INTC_INTERRUPT_ID_1, ISR1_PRIORITY, ISR_RISING_EDGE);

    // Connect ISR1 to the interrupt controller
    result = XScuGic_Connect(intc_instance_ptr, INTC_INTERRUPT_ID_1, (Xil_ExceptionHandler) isr1, (void *) &intc);
    if (result != XST_SUCCESS) {
        return result; // Exit setup with bad result
    }

    // Enable interrupts for IRQ_F2P[1:1]
    XScuGic_Enable(intc_instance_ptr, INTC_INTERRUPT_ID_1);

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
 * the interrupt system and sets the default PWM event qualifier
 */
void timing_manager_init()
{
    printf("TIMING MANAGER:\tInitializing...\n");
    // Initialize interrupts
    uint32_t result = 0;
    result = interrupt_system_init();
    if (result != XST_SUCCESS) {
        printf("Error initializing interrupt system.");
    }

    // Default event qualifier is PWM carrier high AND low
    timing_manager_trigger_on_pwm_both(TIMING_MANAGER_BASE_ADDR);

    // Set the user ratio for the trigger
    timing_manager_set_ratio(SYS_PWM_CARRIER_CONTROL_RATIO, TIMING_MANAGER_BASE_ADDR);

    // Enable selected sensors for timing acquisition
    timing_manager_select_sensors(DEFAULT_SENSOR_ENABLE, TIMING_MANAGER_BASE_ADDR);

    // Disable interrupt 1 - currently not needed
    XScuGic_Disable(&intc, INTC_INTERRUPT_ID_1);
}

/*
 * ISR for IRQ_F2P[0:0]. Called when sched_isr in timing
 * manager is set to 1, e.g. when all of the sensors are
 * done and the time has been collected.
 */
void isr0(void *intc_inst_ptr)
{
    // HANDLE INTERRUPT
    xil_printf("ISR0 called\n\r");
    // Push stats for each sensor
    timing_manager_sensor_stats();
}

/*
 * ISR for IRQ_F2P[1:1]. Called when interrupt_1 in timing
 * manager is set to 1.
 */
void isr1(void *intc_inst_ptr)
{
    // HANDLE INTERRUPT
    xil_printf("ISR1 called\n\r");
}

void nops(uint32_t num)
{
    for (int i = 0; i < num; i++) {
        asm("nop");
    }
}

/*
 * Set the user-defined ratio for the event qualifier
 * This ratio determines the frequency of an interrupt
 * being sent based on the specified event qualifier
 */
void timing_manager_set_ratio(uint32_t ratio, uint32_t base_addr)
{
    // Get the current address for the target config register (slv_reg2)
    uint32_t config_reg_addr = base_addr + (2 * sizeof(uint32_t));
    // Assign the ratio to the config register
    Xil_Out32(config_reg_addr, ratio);
}

/*
 * Select which sensors should be used for timing acquisition
 */
void timing_manager_select_sensors(uint8_t enable_bits, uint32_t base_addr)
{
    // Get the current address for the target config register (slv_reg2)
    uint32_t config_reg_addr = base_addr + (2 * sizeof(uint32_t));
    // Assign the enable bits to the config register
    Xil_Out32(config_reg_addr, enable_bits);
}

/*
 * Set the trigger on either PWM carrier high or low
 */
void timing_manager_trigger_on_pwm_both(uint32_t base_addr)
{
    // Get the current address of the config register
    uint32_t config_reg_addr = base_addr + (3 * sizeof(uint32_t));
    // Set both the carrier high and low trigger bits
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | 0x3));
}

/*
 * Set the trigger on PWM carrier high only
 */
void timing_manager_trigger_on_pwm_high(uint32_t base_addr)
{
    // Get the current address of the config register
    uint32_t config_reg_addr = base_addr + (3 * sizeof(uint32_t));
    // Set only the carrier high bit
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | 0x1));
}

/*
 * Set the trigger on PWM carrier low only
 */
void timing_manager_trigger_on_pwm_low(uint32_t base_addr)
{
    // Get the current address of the config register
    uint32_t config_reg_addr = base_addr + (3 * sizeof(uint32_t));
    // Set only the carrier high bit
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | 0x2));
}

/*
 * Clear the PWM event qualifier
 */
void timing_manager_trigger_on_pwm_clear(uint32_t base_addr)
{
    // Get the current address of the config register
    uint32_t config_reg_addr = base_addr + (3 * sizeof(uint32_t));
    // Clear both the carrier high and low trigger bits
    Xil_Out32(config_reg_addr, (Xil_In32(config_reg_addr) | ~0x3));
}

/*
 * Get the acquisition time for the requested sensor, in nanoseconds
 */
uint16_t timing_manager_get_time_per_sensor(sensor_t sensor, uint32_t base_addr)
{
    uint16_t clock_cycles = 0;
    uint16_t time = 0;

    if (sensor == EDDY_0) {
        // Lower 16 bits of slave reg 5
        clock_cycles = (Xil_In32(base_addr + (5 * sizeof(uint32_t)))) & LOWER_16_MASK;
    } else if (sensor == EDDY_1) {
        // Upper 16 bits of slave reg 5
        clock_cycles = (Xil_In32(base_addr + (5 * sizeof(uint32_t)))) >> UPPER_16_SHIFT;
    } else if (sensor == EDDY_2) {
        // Lower 16 bits of slave reg 6
        clock_cycles = (Xil_In32(base_addr + (6 * sizeof(uint32_t)))) & LOWER_16_MASK;
    } else if (sensor == EDDY_3) {
        // Upper 16 bits of slave reg 6
        clock_cycles = (Xil_In32(base_addr + (6 * sizeof(uint32_t)))) >> UPPER_16_SHIFT;
    } else if (sensor == ENCODER) {
        // Lower 16 bits of slave reg 7
        clock_cycles = (Xil_In32(base_addr + (7 * sizeof(uint32_t)))) & LOWER_16_MASK;
    } else if (sensor == ADC) {
        // Upper 16 bits of slave reg 7
        clock_cycles = (Xil_In32(base_addr + (7 * sizeof(uint32_t)))) >> UPPER_16_SHIFT;
    }
    // Convert clock cycles to time in ns using 200 MHz FPGA clock frequency
    time = (1/FPGA_FREQ) * clock_cycles;
    return time;
}

/*
 * Initializes and pushes statistics for each sensor, if enabled.
 */
void timing_manager_sensor_stats()
{
    // call push, make array of stats structs for each sensor, iterate through for each sensor and call psuh
    for (int i = 0; i < NUM_SENSORS; i++) {
        // Initialize the stats
        statistics_init(sensor_stats[i]);
        uint16_t val = timing_manager_get_time_per_sensor(i, TIMING_MANAGER_BASE_ADDR);
        statistics_push(sensor_stats[i], val);
    }
}

/*
 * Takes in a sensor value, and returns a pointer to the structure
 * containing the stats for that sensor
 */
statistics_t* timing_manager_get_stats_per_sensor(sensor_t sensor)
{
    // Get pointer to the stats for the specified sensor
    return sensor_stats[sensor];
}

/*
 * Function to test the interrupts, slave register values, etc.
 */
int timing_test()
{
    *(baseaddr_p + 0) = 0x00000000;
    xil_printf("Checkpoint 1\n\r");
    xil_printf("slv_reg0: 0x%08x\n\r", *(baseaddr_p + 0));
    Xil_Out32(TIMING_MANAGER_BASE_ADDR, 0x1);
    xil_printf("Checkpoint 2\n\r");
    nops(1000);
    xil_printf("slv_reg0: 0x%08x\n\r", *(baseaddr_p + 0));
    xil_printf("slv_reg0: 0x%08x\n\r", Xil_In32(TIMING_MANAGER_BASE_ADDR));
    xil_printf("slv_reg0: 0x%08x\n\r", *(baseaddr_p + 3));
    xil_printf("slv_reg3: 0x%08x\n\r", Xil_In32(TIMING_MANAGER_BASE_ADDR + (3 * sizeof(uint32_t))));
    nops(1000);

    return 0;
}
