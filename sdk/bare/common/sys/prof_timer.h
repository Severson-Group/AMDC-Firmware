/**
 * Timer for profiling specific sections of code.
 *
 * Can be used to register profiling timers for printouts later, or
 * on it's own. This implements a linked list with a static head
 * which can be iterated over to print out all statistics. This timer
 * uses the sys/statistics module and the high precision
 * drv/fpga_timer module at its core to provide high precision
 * profiling statistics for how long a piece of code takes to run.
 *
 * Usage:
 *
 * @code
 * // Some module.c
 *
 * prof_timer_t my_math_timer = {
 *   .name = "My Math",
 *   .is_enabled = true
 * };
 *
 * void module_init() {
 *   prof_timer_register(&my_math_timer);
 * }
 *
 * void module_update() {
 *   // some code
 *
 *   prof_timer_start(&my_math_timer);
 *
 *   float x = sin(random()) * sqrt(PI);
 *
 *   prof_timer_stop(&my_math_timer);
 *
 *   // Some code
 * }
 *
 * ///////////////
 *
 * // cmd_module.c
 *
 * ...
 * // Print all profiling stats means
 * FOR_ALL_PROF_TIMERS(t) {
 *   if (t->is_enabled)
 *     printf("%s %d", t->name, t->stats.mean);
 * }
 *
 * /// OR
 *
 * prof_timer_t *t = NULL;
 * while (prof_timer_iterate(&t)) {
 *   if (t->is_enabled)
 *     printf("%s %d", t->name, t->stats.mean);
 * }
 *
 * ...
 * @endcode
 */

#ifndef PROF_TIMER_H
#define PROF_TIMER_H

#include "sys/statistics.h"
#include "sys/util.h"
#include "drv/fpga_timer.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

// ---------
// Structure
// ---------

typedef struct prof_timer {
	// A name for printouts
    const char *name;

    // Will the stats actually be calculated?
    bool is_enabled;

    // Statistics for the timer.
    statistics_t stats;

    // Private struct variables
    uint32_t __start_time;
    struct prof_timer *__next;
    uint32_t __primask;
} prof_timer_t;

// ---------
// Timer API
// ---------

// Start the timer
static inline void prof_timer_start(prof_timer_t *x)
{
    x->__start_time = fpga_timer_now();
}

static inline void prof_timer_start_crit(prof_timer_t *x)
{
    if (x->is_enabled)
        x->__primask = util_critical_section_enter();
    prof_timer_start(x);
}

// Stop the timer and calculate the running statistics
void __prof_timer_stop(prof_timer_t *x, uint32_t now);
void __prof_timer_stop_crit(prof_timer_t *x, uint32_t now);

// To remove as much function call and logical overhead as possible,
// measure now before __prof_timer_stop() is called.
#define prof_timer_stop(x)      __prof_timer_stop(x, fpga_timer_now())
#define prof_timer_stop_crit(x) __prof_timer_stop_crit(x, fpga_timer_now())

/** Reset the timer statistics. */
void prof_timer_reset(prof_timer_t *x);

// ------------------------------
// Linked List / Registration API
// ------------------------------

// Register the Profiling Timer with the internal linked list
void prof_timer_register(prof_timer_t *x);

// Unregister the Profiling Timer with the internal linked list
void prof_timer_unregister(prof_timer_t *x);

// Get the number of registered profile timers.
size_t prof_timer_num_registered(void);

// -------------
// Iteration API
// -------------

/**
 * Iterate through all registered timers:
 *
 * Usage:
 *   prof_timer_t *current = NULL;
 *   while (prof_timer_iterate(&current)) {
 *     // DO stuff.
 *   }
 */
bool prof_timer_iterate(prof_timer_t **current);

/**
 * FOR_ALL_PROF_TIMERS implements a safe use case of prof_timer_iterate() which
 * scopes `var` within it. This iterates through all elements in the Profiling Timer List.
 */
#define FOR_ALL_PROF_TIMERS(var) for (prof_timer_t *var = NULL; prof_timer_iterate(&var);)

void prof_timer_reset_all(void);

#endif // PROF_TIMER_H
