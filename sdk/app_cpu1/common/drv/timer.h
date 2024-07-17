#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*timer_handler_t)(void *arg);

void timer_init(timer_handler_t timer_isr, uint32_t timer_period_usec);

void timer_clear_interrupt(void);
bool timer_is_expired(void);

#endif // TIMER_H
