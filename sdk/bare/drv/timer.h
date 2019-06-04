#ifndef TIMER_H
#define TIMER_H

#include "xtmrctr.h"

void timer_init(XTmrCtr_Handler timer_isr, uint32_t timer_period_usec);

#endif // TIMER_H
