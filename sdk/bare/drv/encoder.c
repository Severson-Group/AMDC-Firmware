#include "encoder.h"
#include "io.h"
#include "../sys/defines.h"
#include "../sys/scheduler.h"
#include "../usr/params/inverter.h"
#include "../usr/params/machine.h"
#include <stdio.h>
#include "xil_io.h"
#include <math.h>

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






// ****************
// State Machine which finds z pulse
// ****************

typedef enum sm_states_e {
	ALIGN_DQ_AXIS,
	SPIN_UNTIL_Z,
	REMOVE_TASK
} sm_states_e;

typedef struct sm_ctx_t {
	sm_states_e state;
	double vPercent;
	double theta;
	double theta_delta;
	int counter;
	task_control_block_t tcb;
} sm_ctx_t;

#define SM_UPDATES_PER_SEC		(10000)
#define SM_INTERVAL_USEC		(USEC_IN_SEC / SM_UPDATES_PER_SEC)

static void _find_z_callback(void *arg)
{
	sm_ctx_t *ctx = (sm_ctx_t *) arg;
	double Va_star;
	double Vb_star;
	double Vc_star;

	switch (ctx->state) {
	case ALIGN_DQ_AXIS:
		Va_star = 0.1 * inverter_get_Vdc() * cos(-PI23);
		Vb_star = 0.1 * inverter_get_Vdc() * cos(0.0);
		Vc_star = 0.1 * inverter_get_Vdc() * cos(PI23);

		inverter_set_voltage(0, Va_star, 0.0);
		inverter_set_voltage(1, Vb_star, 0.0);
		inverter_set_voltage(2, Vc_star, 0.0);

		ctx->counter++;
		if (ctx->counter > SM_UPDATES_PER_SEC) {
			ctx->state = SPIN_UNTIL_Z;
		}

		break;

	case SPIN_UNTIL_Z:
		ctx->theta += ctx->theta_delta;
		while (ctx->theta > PI2) {
			ctx->theta -= PI2;
		}

		Va_star = ctx->vPercent * inverter_get_Vdc() * cos(ctx->theta - PI23);
		Vb_star = ctx->vPercent * inverter_get_Vdc() * cos(ctx->theta);
		Vc_star = ctx->vPercent * inverter_get_Vdc() * cos(ctx->theta + PI23);

		inverter_set_voltage(0, Va_star, 0.0);
		inverter_set_voltage(1, Vb_star, 0.0);
		inverter_set_voltage(2, Vc_star, 0.0);

		uint32_t pos;
		encoder_get_position(&pos);
		if (pos != -1) ctx->state = REMOVE_TASK;
		break;

	case REMOVE_TASK:
		scheduler_tcb_unregister(&ctx->tcb);

		inverter_set_voltage(0, 0.0, 0.0);
		inverter_set_voltage(1, 0.0, 0.0);
		inverter_set_voltage(2, 0.0, 0.0);

		io_led_color_t color;
		color.b = 0;
		io_led_set_c(0, 0, 1, &color);

		break;
	}
}

static sm_ctx_t ctx;

void encoder_find_z(double rpm, double vPercent)
{
	// Initialize the state machine context
	ctx.state = ALIGN_DQ_AXIS;
	ctx.vPercent = vPercent;
	ctx.theta = 0.0;
	ctx.counter = 0;
	ctx.theta_delta = POLE_PAIRS * (rpm / 60.0) / (double) SM_UPDATES_PER_SEC;

	io_led_color_t color;
	color.b = 255;
	io_led_set_c(0, 0, 1, &color);

	// Initialize the state machine callback tcb
	scheduler_tcb_init(&ctx.tcb, _find_z_callback, &ctx, "find_z", SM_INTERVAL_USEC);
	scheduler_tcb_register(&ctx.tcb);
}
