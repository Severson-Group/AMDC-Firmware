#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#define ENCODER_PULSES_PER_REV_BITS (14)
#define ENCODER_PULSES_PER_REV      (1 << ENCODER_PULSES_PER_REV_BITS)

void encoder_init(void);

void encoder_set_pulses_per_rev_bits(uint32_t bits);

void encoder_get_steps(int32_t *steps);
void encoder_get_position(uint32_t *position);

void encoder_find_z(void);

#endif // ENCODER_H
