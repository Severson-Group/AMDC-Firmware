/*
 * timer_stats.h
 *
 *  Created on: Jul 11, 2024
 *  Author: Ryan Srichai
 *  Notes: The timer described in
 *  https://www.freertos.org/rtos-run-time-stats.html
 */

#ifndef DRV_TIMER_STATS_H_
#define DRV_TIMER_STATS_H_

#define TIMER_STATS_USEC 10 // timer is 10x faster than FreeRTOS tick rate (which is set to 10000 ticks a second). This stats timer runs at 100KHz

void vInitialiseTimerForRunTimeStats();

#endif /* DRV_TIMER_STATS_H_ */
