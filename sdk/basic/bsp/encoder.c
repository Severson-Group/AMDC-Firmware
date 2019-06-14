#include "encoder.h"
#include <stdio.h>
#include <math.h>
#include "xil_io.h"

#define ENCODER_BASE_ADDR		(0x43C10000)

void encoder_init(void)
{
	printf("ENC:\tInitializing...\n");
	encoder_set_pulses_per_rev_bits(ENCODER_PULSES_PER_REV_BITS);
}

void encoder_set_pulses_per_rev_bits(uint32_t bits)
{
	printf("ENC:\tSetting pulses per rev bits = %ld...\n", bits);

	Xil_Out32(ENCODER_BASE_ADDR + 2*sizeof(uint32_t), bits);
}

void encoder_get_steps(int32_t *steps)
{
	*steps = Xil_In32(ENCODER_BASE_ADDR);
}

void encoder_get_position(uint32_t *position)
{
	*position = Xil_In32(ENCODER_BASE_ADDR + 1*sizeof(uint32_t));
}
