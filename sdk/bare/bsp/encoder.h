#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

void encoder_init(void);

void encoder_reset(void);
void encoder_steps(int32_t *steps);

#endif // ENCODER_H
