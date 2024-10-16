#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#include "xparameters.h"

#define ENCODER_BASE_ADDR (XPAR_AMDC_ENCODER_0_S00_AXI_BASEADDR)

#define ENCODER_PULSES_PER_REV_BITS (14)
#define ENCODER_PULSES_PER_REV      (1 << ENCODER_PULSES_PER_REV_BITS)

void encoder_init(void);

void encoder_set_pulses_per_rev_bits(uint32_t bits);
void encoder_set_pulses_per_rev(uint32_t pulses);
void encoder_get_steps(int32_t *steps);
void encoder_get_steps_instantaneous(int32_t *steps);
void encoder_get_position(uint32_t *position);
void encoder_get_position_instantaneous(uint32_t *position);

void encoder_find_z(void);

#endif // ENCODER_H
