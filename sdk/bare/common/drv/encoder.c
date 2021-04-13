#include "drv/encoder.h"
#include "drv/hardware_targets.h"
#include "drv/io.h"
#include "sys/defines.h"
#include "sys/scheduler.h"
#include "usr/user_config.h"
#include "xil_io.h"
#include <math.h>
#include <stdio.h>

#define ENCODER_BASE_ADDR (0x43C10000)

void encoder_init(void)
{
    printf("ENC:\tInitializing...\n");
    encoder_set_pulses_per_rev_bits(ENCODER_PULSES_PER_REV_BITS);
}

void encoder_set_pulses_per_rev_bits(uint32_t bits)
{
    printf("ENC:\tSetting pulses per rev bits = %ld...\n", bits);

    Xil_Out32(ENCODER_BASE_ADDR + 2 * sizeof(uint32_t), bits);
}

void encoder_get_pulses_per_rev_bits(uint32_t *bits_out)
{
    *bits_out = Xil_In32(ENCODER_BASE_ADDR + 2 * sizeof(uint32_t));
}

void encoder_get_steps(int32_t *steps)
{
    *steps = Xil_In32(ENCODER_BASE_ADDR);
}

void encoder_get_position(uint32_t *position)
{
    *position = Xil_In32(ENCODER_BASE_ADDR + 1 * sizeof(uint32_t));
}

double encoder_calc_speed(double dt, uint32_t pos_prev)
{
	// TODO: implement this
	return 0;
}

double encoder_get_theta(void)
{
	uint32_t pos;
	encoder_get_position(&pos);

	double theta = 0;

	if (pos != -1) {
		uint32_t bits;
		encoder_get_pulses_per_rev_bits(&bits);

		uint32_t PPR = 1 << bits;

		theta = PI2 * ((double)pos / (double)PPR);
	}

	return theta;
}

double encoder_get_theta_resolution(void)
{
	// TODO: implement this
	return 0;
}




// ****************
// State Machine which finds z pulse
// ****************

typedef enum sm_states_e {
    WAIT_UNTIL_Z,
    REMOVE_TASK,
} sm_states_e;

typedef struct sm_ctx_t {
    sm_states_e state;
    double vPercent;
    double theta;
    double theta_delta;
    int counter;
    task_control_block_t tcb;
} sm_ctx_t;

#define SM_UPDATES_PER_SEC (10000)
#define SM_INTERVAL_USEC   (USEC_IN_SEC / SM_UPDATES_PER_SEC)

static void _find_z_callback(void *arg)
{
    sm_ctx_t *ctx = (sm_ctx_t *) arg;

    switch (ctx->state) {
    case WAIT_UNTIL_Z:
    {
        uint32_t pos;
        encoder_get_position(&pos);
        if (pos != -1) {
            ctx->state = REMOVE_TASK;
        }

        break;
    }

    case REMOVE_TASK:
    {
        scheduler_tcb_unregister(&ctx->tcb);

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_C
        io_led_color_t color;
        color.b = 0;
        io_led_set_c(0, 0, 1, &color);
#endif // USER_CONFIG_HARDWARE_TARGET
        break;
    }
    }
}

static sm_ctx_t ctx;

void encoder_find_z(void)
{
    // Initialize the state machine context
    ctx.state = WAIT_UNTIL_Z;

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_C
    io_led_color_t color;
    color.b = 255;
    io_led_set_c(0, 0, 1, &color);
#endif // USER_CONFIG_HARDWARE_TARGET

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx.tcb, _find_z_callback, &ctx, "find_z", SM_INTERVAL_USEC);
    scheduler_tcb_register(&ctx.tcb);
}
