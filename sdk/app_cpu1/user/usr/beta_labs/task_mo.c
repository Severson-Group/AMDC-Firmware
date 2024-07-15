#ifdef APP_BETA_LABS

#include "usr/beta_labs/task_mo.h"
#include "drv/dac.h"
#include "drv/encoder.h"
#include "sys/scheduler.h"
#include "usr/beta_labs/machine.h"

static task_control_block_t tcb;

static double task_mo_omega_m = 0.0;

// Forward declarations
inline static double filter(double input);

void task_mo_init(void)
{
    scheduler_tcb_init(&tcb, task_mo_callback, NULL, "mo", TASK_MO_INTERVAL_USEC);
    scheduler_tcb_register(&tcb);
}

double LOG_enc_pos = 0.0;

void task_mo_callback(void *arg)
{
    // TODO: change this to use "position"
    // and wrap things etc
    // Also be aware of negative wraps

    static int32_t last_steps = 0;
    int32_t steps;
    encoder_get_steps(&steps);
    int32_t delta = steps - last_steps;
    double rads = PI2 * ((double) delta / (double) (1 << ENCODER_PULSES_PER_REV_BITS));

    last_steps = steps;

    double omega_m = rads * (double) TASK_MO_UPDATES_PER_SEC;

    task_mo_omega_m = filter(omega_m);

    // Update log variables
    uint32_t pos;
    encoder_get_position(&pos);
    LOG_enc_pos = pos;

    double p = (double) pos / (double) ENCODER_PULSES_PER_REV;

    dac_set_output(0, p, 0.0, 1.0);
}

double task_mo_get_omega_e(void)
{
    return task_mo_omega_m * POLE_PAIRS;
}

double task_mo_get_omega_m(void)
{
    return task_mo_omega_m;
}

// Tuning for LPF based on Ts = 0.0001
#define A_1Hz   (0.9993718788200349)
#define A_5Hz   (0.9968633369849541)
#define A_10Hz  (0.9937365126247782)
#define A_50Hz  (0.9690724263048106)
#define A_100Hz (0.9391013674242926)
#define A_500Hz (0.7304026910486456)

#define A (A_100Hz)

inline static double filter(double input)
{
    static double z1 = 0.0;

    double output = (input * (1 - A)) + z1;

    z1 = output * A;

    return output;
}

#endif // APP_BETA_LABS
