#include "drv/encoder.h"
#include "sys/defines.h"
#include "sys/scheduler.h"
#include "xil_io.h"
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

void encoder_get_steps(int32_t *steps)
{
    *steps = Xil_In32(ENCODER_BASE_ADDR);
}

void encoder_get_position(uint32_t *position)
{
    *position = Xil_In32(ENCODER_BASE_ADDR + 1 * sizeof(uint32_t));
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
        break;
    }
    }
}

static sm_ctx_t ctx;

void encoder_find_z(void)
{
    // Initialize the state machine context
    ctx.state = WAIT_UNTIL_Z;

    // Initialize the state machine callback tcb
    scheduler_tcb_init(&ctx.tcb, _find_z_callback, &ctx, "find_z", SM_INTERVAL_USEC);
    scheduler_tcb_register(&ctx.tcb);
}
