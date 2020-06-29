#ifndef FPGA_TIMER_H
#define FPGA_TIMER_H

#include <stdint.h>

void fpga_timer_init(void);

// Return the current time in FPGA clock cycles (200MHz clock)
uint32_t fpga_timer_now(void);

// Convert FPGA clock ticks to usec
double fpga_timer_ticks_to_usec(uint32_t ticks);

// Convert FPGA clock ticks to sec
double fpga_timer_ticks_to_sec(uint32_t ticks);

#endif // FPGA_TIMER_H
