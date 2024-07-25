#include "drv/cpu_timer.h"
#include "sys/defines.h"
//#include "xparameters.h"
//#include "xtmrctr.h"

// Code from StackOverflow post:
// https://stackoverflow.com/questions/3247373/3250835#3250835

static inline void init_perfcounters(int32_t do_reset, int32_t enable_divider);

static const double CPU_CLOCK_FREQ_MHZ = 666.666;
static const double CPU_CLOCK_FREQ_HZ = CPU_CLOCK_FREQ_MHZ * 1e6;

void cpu_timer_init(void)
{
    /* enable user-mode access to the performance counter*/
    asm("MCR p15, 0, %0, C9, C14, 0\n\t" ::"r"(1));

    /* disable counter overflow interrupts (just in case)*/
    asm("MCR p15, 0, %0, C9, C14, 2\n\t" ::"r"(0x8000000f));

    init_perfcounters(1, 0);
}

uint32_t cpu_timer_now(void)
{
    uint32_t value;
    // Read CCNT Register
    asm volatile("MRC p15, 0, %0, c9, c13, 0\t\n" : "=r"(value));
    return value;
}

double cpu_timer_ticks_to_usec(uint32_t ticks)
{
    double usec = (double) ticks / CPU_CLOCK_FREQ_MHZ;
    return usec;
}

double cpu_timer_ticks_to_sec(uint32_t ticks)
{
    double sec = (double) ticks / CPU_CLOCK_FREQ_HZ;
    return sec;
}

static inline void init_perfcounters(int32_t do_reset, int32_t enable_divider)
{
    // in general enable all counters (including cycle counter)
    int32_t value = 1;

    // peform reset:
    if (do_reset) {
        value |= 2; // reset all counters to zero.
        value |= 4; // reset cycle counter to zero.
    }

    if (enable_divider)
        value |= 8; // enable "by 64" divider for CCNT.

    value |= 16;

    // program the performance-counter control-register:
    asm volatile("MCR p15, 0, %0, c9, c12, 0\t\n" ::"r"(value));

    // enable all counters:
    asm volatile("MCR p15, 0, %0, c9, c12, 1\t\n" ::"r"(0x8000000f));

    // clear overflows:
    asm volatile("MCR p15, 0, %0, c9, c12, 3\t\n" ::"r"(0x8000000f));
}
