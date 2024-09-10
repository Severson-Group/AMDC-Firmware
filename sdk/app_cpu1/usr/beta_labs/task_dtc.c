#ifdef APP_BETA_LABS

#include "usr/beta_labs/task_dtc.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/scheduler.h"
#include "usr/beta_labs/inverter.h"
#include "usr/beta_labs/machine.h"
#include <math.h>

#define L_HAT ((Ld_HAT + Lq_HAT) / 2.0)
#define R_HAT (Rs_HAT)

#define Wb (DTC_BANDWIDTH * PI2) // rad/s
#define Ts (1.0 / TASK_DTC_UPDATES_PER_SEC)
#define Kp (Wb * L_HAT)
#define Ki ((R_HAT / L_HAT) * Kp)

// Global variables for logging
double LOG_Ia = 0.0;
double LOG_Vab_star = 0.0;

// Set by command
static double I_mag_star = 0.0;
static double I_freq_star = 0.0;

// Used by controller
static double I_star = 0.0;
static double I_err_acc = 0.0;
static double theta = 0.0;

static task_control_block_t tcb;

uint8_t task_dtc_is_inited(void)
{
    return scheduler_tcb_is_registered(&tcb);
}

void task_dtc_init(void)
{
    scheduler_tcb_init(&tcb, task_dtc_callback, NULL, "dtc", TASK_DTC_INTERVAL_USEC);
    scheduler_tcb_register(&tcb);
}

void task_dtc_deinit(void)
{
    scheduler_tcb_unregister(&tcb);
}

static void _get_Iabc(double *Iabc)
{
    // Read from ADCs
    float Iabc_f[3];
    analog_getf(CC_PHASE_A_ADC, &Iabc_f[0]);
    analog_getf(CC_PHASE_B_ADC, &Iabc_f[1]);
    analog_getf(CC_PHASE_C_ADC, &Iabc_f[2]);

    // Convert ADC values to raw currents
    Iabc[0] = ((double) Iabc_f[0] * ADC_TO_AMPS_PHASE_A_GAIN) + ADC_TO_AMPS_PHASE_A_OFFSET;
    Iabc[1] = ((double) Iabc_f[1] * ADC_TO_AMPS_PHASE_B_GAIN) + ADC_TO_AMPS_PHASE_B_OFFSET;
    Iabc[2] = ((double) Iabc_f[2] * ADC_TO_AMPS_PHASE_C_GAIN) + ADC_TO_AMPS_PHASE_C_OFFSET;
}

void task_dtc_callback(void *arg)
{
    // ------------------------------------
    // Get currents
    // ------------------------------------

    double Iabc[3];
    _get_Iabc(Iabc);
    double Ia = Iabc[0];

    // ------------------------------------
    // Calculate I_star based on commanded I_mag and I_freq
    // ------------------------------------
    theta += (PI2 * I_freq_star * Ts);
    I_star = I_mag_star * cos(theta);

    // ------------------------------------
    // PI stationary frame current regulator Ia
    // ------------------------------------

    double I_err;
    double Va_star, Vb_star;
    I_err = I_star - Ia;
    I_err_acc += I_err;
    Va_star = (Kp * I_err) + (Ki * Ts * I_err_acc);
    Vb_star = -Va_star;

    // ------------------------------------
    // Write commanded voltages to inverter
    // ------------------------------------

    inverter_saturate_to_Vdc(&Va_star);
    inverter_saturate_to_Vdc(&Vb_star);

    inverter_set_voltage(CC_PHASE_A_PWM_LEG_IDX, Va_star, Iabc[0]);
    inverter_set_voltage(CC_PHASE_B_PWM_LEG_IDX, Vb_star, Iabc[1]);

    // ------------------------------------
    // Update log variables
    // ------------------------------------

    LOG_Ia = Iabc[0];
    LOG_Vab_star = Va_star - Vb_star;
}

void task_dtc_set_I_star(double A, double Hz)
{
    I_mag_star = A;
    I_freq_star = Hz;
}

void task_dtc_clear(void)
{
    task_dtc_set_I_star(0.0, 0.0);

    inverter_set_voltage(CC_PHASE_A_PWM_LEG_IDX, 0.0, 0.0);
    inverter_set_voltage(CC_PHASE_B_PWM_LEG_IDX, 0.0, 0.0);
}

#endif // APP_BETA_LABS
