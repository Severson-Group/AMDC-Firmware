#include "../bsp/encoder.h"

#include "xil_io.h"

#define ENCODER_BASE_ADDR		(0x43C10000)

void encoder_init(void)
{

}

void encoder_reset(void)
{

}

void encoder_steps(int32_t *steps)
{
	*steps = Xil_In32(ENCODER_BASE_ADDR);
}
