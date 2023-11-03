#ifndef TIMING_MANAGER_H
#define TIMING_MANAGER_H

#include "drv/hardware_targets.h"
#include "usr/user_config.h"
#include <stdbool.h>
#include <stdint.h>

#define INTC_INTERRUPT_ID_0 		61  // IRQ_F2P[0:0]
#define INTC_INTERRUPT_ID_1 		62  // IRQ_F2P[1:1]
#define ISR0_PRIORITY       		0xA0
#define ISR1_PRIORITY       		0xA8
#define ISR_RISING_EDGE     		0x3
#define TIMING_MANAGER_BASE_ADDR	XPAR_AMDC_TIMING_MANAGER_0_S00_AXI_BASEADDR

uint32_t interrupt_system_init();

void timing_manager_init();
void timing_manager_set_ratio(uint32_t ratio, uint32_t base_addr);
void timing_manager_trigger_on_pwm_both(uint32_t base_addr);
void timing_manager_trigger_on_pwm_high(uint32_t base_addr);
void timing_manager_trigger_on_pwm_low(uint32_t base_addr);
void timing_manager_trigger_on_pwm_clear(uint32_t base_addr);
void isr0 (void *intc_inst_ptr);
void isr1 (void *intc_inst_ptr);
void nops (uint32_t num);
int timing_test();

#endif // TIMING_MANAGER_H
