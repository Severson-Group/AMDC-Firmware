#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

#define ENCODER_PULSES_PER_REV_BITS (14)
#define ENCODER_PULSES_PER_REV      (1 << ENCODER_PULSES_PER_REV_BITS)

void encoder_init(void);

void encoder_set_pulses_per_rev_bits(uint32_t bits);
void encoder_get_pulses_per_rev_bits(uint32_t *bits_out);

void encoder_get_steps(int32_t *steps);
void encoder_get_position(uint32_t *position);

// Compute the current speed in mechanical rad/s
//
// This function will subtract the previous position from the current position and divide by delta time.
// Finally, it will use the steps per rev to get the answer in a standardized mechanical rad/s
double encoder_calc_speed(double dt, uint32_t pos_prev);

// Compute the current position in mechanical rad/s
//
// This function simply uses the steps per rev to get the answer in a standardized mechanical radians
double encoder_get_theta(void);

// Compute resolution in mechanical rads
//
// This function returns the resolution of the encoder output in mechanical radians
double encoder_get_theta_resolution(void);

void encoder_find_z(void);

#endif // ENCODER_H
