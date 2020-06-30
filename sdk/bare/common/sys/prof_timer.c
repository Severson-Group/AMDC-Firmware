#include "sys/prof_timer.h"
#include "sys/statistics.h"
#include "drv/fpga_timer.h"
#include "sys/util.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

static prof_timer_t *m_head = NULL;

// Keep track of the number of registered items
// so that we can limit the number of loop iterations
static size_t m_num_registered = 0;

void prof_timer_register(prof_timer_t *x)
{
    assert(x);

    prof_timer_t *entry = m_head;
    size_t i = 0;

    // Find the end of the list. Since we track number of
    // registrations we can limit our loop count in case of
    // corruption in the linked list itself.
    while (entry && entry->__next && (i < m_num_registered)) {
        // If we are already registered don't double register. Just exit
        if (entry == x) {
            return;
        }

        entry = entry->__next;
        i++;
    }

    if (entry) {
        entry->__next = x;
    } else {
    	// No registered entries
    	m_head = x;
    }

    m_num_registered++;
}

void prof_timer_unregister(prof_timer_t *x)
{
    assert(x);

    if (!m_head) {
        return;
    }

    // If we are the head then just remove the reference
    // and make the next element in the list the head
    if (x == m_head) {
        m_head = m_head->__next;
        x->__next = NULL;
        m_num_registered--;
    } else {
        // Otherwise we will need to search for our reference by
        // iterating over the list to find the element of the list
        // that references us. Then we set the next reference of
        // the previous element to our next reference.
    	prof_timer_t *prev = m_head;
        size_t i = 0;
        while (prev && (prev->__next) && (i < m_num_registered)) {
            if (prev->__next == x) {
                prev->__next = x->__next;
                x->__next = NULL;
                m_num_registered--;
                break;
            }

            i++;
        }
    }
}

void __prof_timer_stop(prof_timer_t *x, uint32_t now_ticks)
{
    assert(x);

    if (x->is_enabled) {
        double dt_usec = fpga_timer_ticks_to_usec(now_ticks - x->__start_time);
        statistics_push(&(x->stats), dt_usec);
    }
}

void __prof_timer_stop_crit(prof_timer_t *x, uint32_t now_ticks)
{
    assert(x);

    __prof_timer_stop(x, now_ticks);

    if (x->is_enabled) {
        util_critical_section_exit(x->__primask);
    }
}

void prof_timer_reset(prof_timer_t *x)
{
    assert(x);

    if (x->is_enabled) {
        statistics_clear(&(x->stats));
    }
}

size_t prof_timer_num_registered(void)
{
    return m_num_registered;
}

bool prof_timer_iterate(prof_timer_t **current)
{
    assert(current);

    static size_t iteration = 0;
    if (*current == NULL) {
        *current = m_head;
        iteration = 0;
    } else {
        *current = (*current)->__next;
        iteration += 1;
    }

    return (*current != NULL) && (iteration < m_num_registered);
}

void prof_timer_reset_all(void)
{
    FOR_ALL_PROF_TIMERS(x)
    {
        prof_timer_reset(x);
    }
}
