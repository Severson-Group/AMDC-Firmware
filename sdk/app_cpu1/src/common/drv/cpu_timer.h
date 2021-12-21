#ifndef CPU_TIMER_H
#define CPU_TIMER_H

#include <stdint.h>

void cpu_timer_init(void);

// Return the current time in CPU clock cycles (666.666 MHZ clock)
uint32_t cpu_timer_now(void);

// Convert CPU clock ticks to usec
double cpu_timer_ticks_to_usec(uint32_t ticks);

// Convert CPU clock ticks to sec
double cpu_timer_ticks_to_sec(uint32_t ticks);

#endif // CPU_TIMER_H
