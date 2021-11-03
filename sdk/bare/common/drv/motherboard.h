#ifndef MOTHERBOARD_H
#define MOTHERBOARD_H

#include <stdbool.h>
#include <stdint.h>

#define MOTHERBOARD_BASE_ADDR (0x43C90000)

typedef enum {
    // Keep first channel index at 0!
    MB_IN1 = 0,
    MB_IN2,
    MB_IN3,
    MB_IN4,
    MB_IN5,
    MB_IN6,
    MB_IN7,
    MB_IN8,

    // Keep this as last entry!
    MB_NUM_CHANNELS,
} mb_channel_e;

static inline bool motherboard_is_valid_channel(mb_channel_e channel)
{
    if (channel >= MB_IN1 && channel < MB_NUM_CHANNELS) {
        return true;
    }

    return false;
}

void motherboard_init(uint32_t base_addr);

void motherboard_set_adc_sampling(bool enable);
void motherboard_request_new_data(void);

int motherboard_get_data(mb_channel_e channel, int32_t *out);

void motherboard_print_samples(void);
void motherboard_print_counters(void);

#endif // MOTHERBOARD_H
