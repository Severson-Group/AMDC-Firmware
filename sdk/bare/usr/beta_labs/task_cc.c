#ifdef APP_BETA_LABS

#include "task_cc.h"
#include "inverter.h"
#include "task_mo.h"
#include "co_sync.h"
#include "co_stat.h"
#include "bemfo.h"
#include "machine.h"
#include "cmd/cmd_cc.h"
#include "../../sys/debug.h"
#include "../../sys/defines.h"
#include "../../sys/scheduler.h"
#include "../../sys/injection.h"
#include "../../sys/transform.h"
#include "../../drv/analog.h"
#include "../../drv/encoder.h"
#include "../../drv/io.h"
#include "../../drv/dac.h"
#include "../../drv/pwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Controller tuning
#define Wb      (controller_bw * PI2) // rad/s
#define Ts      (1.0 / TASK_CC_UPDATES_PER_SEC)
#define Kp_d    (Wb * Ld_HAT)
#define Kp_q    (Wb * Lq_HAT)
#define Ki_d    ((Rs_HAT / Ld_HAT) * Kp_d)
#define Ki_q    ((Rs_HAT / Lq_HAT) * Kp_q)

// Variables for logging
double LOG_Id = 0.0;
double LOG_Iq = 0.0;
double LOG_Id_star = 0.0;
double LOG_Iq_star = 0.0;
double LOG_Vd_star = 0.0;
double LOG_Vq_star = 0.0;
double LOG_theta_e_enc = 0.0;
double LOG_theta_e_hat = 0.0;
double LOG_omega_m_hat = 0.0;
double LOG_omega_m_enc = 0.0;
double LOG_omega_e_avg = 0.0;

// Commands for Id and Iq -- Idq*
static double Id_star = 0.0;
static double Iq_star = 0.0;

static int32_t dq_offset = 410;

// Note: user should override this initial value
static double controller_bw = 1.0;

// Used to switch controller to use estimated position / speed
static uint8_t theta_e_src_use_encoder = 1;
static uint8_t omega_e_src_use_encoder = 1;

// Static variables for controller
static double Id_err_acc;
static double Iq_err_acc;

// Injection contexts for current controller
inj_ctx_t cc_inj_ctx_Id_star;
inj_ctx_t cc_inj_ctx_Iq_star;
inj_ctx_t cc_inj_ctx_Vd_star;
inj_ctx_t cc_inj_ctx_Vq_star;
inj_ctx_t cc_inj_ctx_Ialpha_star;
inj_ctx_t cc_inj_ctx_Ibeta_star;

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;


uint8_t task_cc_is_inited(void)
{
    return scheduler_tcb_is_registered(&tcb);
}

static void _clear_state(void)
{
    // Clear controller static variables
    Id_err_acc = 0.0;
    Iq_err_acc = 0.0;

    bemfo_init();
}

void task_cc_init(void)
{
    // Register task with scheduler
    scheduler_tcb_init(&tcb, task_cc_callback, NULL, "cc", TASK_CC_INTERVAL_USEC);
    scheduler_tcb_register(&tcb);

    // Initialize all cc signal injection points
    injection_ctx_init(&cc_inj_ctx_Id_star, "Id*");
    injection_ctx_init(&cc_inj_ctx_Iq_star, "Iq*");
    injection_ctx_init(&cc_inj_ctx_Vd_star, "Vd*");
    injection_ctx_init(&cc_inj_ctx_Vq_star, "Vq*");
    injection_ctx_init(&cc_inj_ctx_Ialpha_star, "Ialpha*");
    injection_ctx_init(&cc_inj_ctx_Ibeta_star,  "Ibeta*");

    // Register all cc signal injection points
    injection_ctx_register(&cc_inj_ctx_Id_star);
    injection_ctx_register(&cc_inj_ctx_Iq_star);
    injection_ctx_register(&cc_inj_ctx_Vd_star);
    injection_ctx_register(&cc_inj_ctx_Vq_star);
    injection_ctx_register(&cc_inj_ctx_Ialpha_star);
    injection_ctx_register(&cc_inj_ctx_Ibeta_star);

    // Clear controller state
    _clear_state();
}

void task_cc_deinit(void)
{
    // Unregister task with scheduler
    scheduler_tcb_unregister(&tcb);

    // Unregister all cc signal injection points
    injection_ctx_unregister(&cc_inj_ctx_Id_star);
    injection_ctx_unregister(&cc_inj_ctx_Iq_star);
    injection_ctx_unregister(&cc_inj_ctx_Vd_star);
    injection_ctx_unregister(&cc_inj_ctx_Vq_star);
    injection_ctx_unregister(&cc_inj_ctx_Ialpha_star);
    injection_ctx_unregister(&cc_inj_ctx_Ibeta_star);


    // Clear all injection points
    injection_ctx_clear(&cc_inj_ctx_Id_star);
    injection_ctx_clear(&cc_inj_ctx_Iq_star);
    injection_ctx_clear(&cc_inj_ctx_Vd_star);
    injection_ctx_clear(&cc_inj_ctx_Vq_star);
    injection_ctx_clear(&cc_inj_ctx_Ialpha_star);
    injection_ctx_clear(&cc_inj_ctx_Ibeta_star);

    // Clear controller state
    _clear_state();
}

void task_cc_reset(void)
{
    _clear_state();
}

double task_cc_get_theta_e_enc(void)
{
    // Get raw encoder position
    uint32_t position;
    encoder_get_position(&position);

    // Add offset (align to DQ frame)
    position += dq_offset;

    while (position >= ENCODER_PULSES_PER_REV) {
        position -= ENCODER_PULSES_PER_REV;
    }

    double theta_e_enc = 0.0;

    // Convert to radians
    theta_e_enc = (double)PI2 * ((double)position / (double)ENCODER_PULSES_PER_REV);

    // Multiple by pole pairs to convert mechanical to electrical degrees
    theta_e_enc *= POLE_PAIRS;

    // Mod by 2 pi
    while (theta_e_enc > PI2) {
        theta_e_enc -= PI2;
    }

    return theta_e_enc;
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

void task_cc_callback(void *arg)
{
    // -------------------
    // Inject signals into Idq*
    // (constants, chirps, noise, etc)
    // -------------------
    injection_inj(&Id_star, &cc_inj_ctx_Id_star, Ts);
    injection_inj(&Iq_star, &cc_inj_ctx_Iq_star, Ts);


    // -------------------
    // Update theta_e using either encoder or estimation
    // -------------------
    double theta_e_enc = task_cc_get_theta_e_enc();
    double theta_e_hat = bemfo_get_theta_e_hat();

    double theta_e = 0.0;
    if (theta_e_src_use_encoder) {
        theta_e = theta_e_enc;
    } else {
        theta_e = theta_e_hat;
    }

    LOG_theta_e_enc = theta_e_enc;
    LOG_theta_e_hat = theta_e_hat;

    LOG_omega_m_enc = task_mo_get_omega_m();
    LOG_omega_m_hat = bemfo_get_omega_m_hat();


    // ------------------------------
    // Update omega_e_avg in rads/sec
    // ------------------------------
    double omega_e_enc = task_mo_get_omega_e();
    double omega_e_hat = bemfo_get_omega_e_hat();

    LOG_omega_e_avg = omega_e_enc;

    double omega_e_avg = 0.0;
    if (omega_e_src_use_encoder) {
        omega_e_avg = omega_e_enc;
    } else {
        omega_e_avg = omega_e_hat;
    }


    // ------------------------------
    // LED status to indicate if sensorless is acquired
    // ------------------------------
    io_led_color_t rgb_led = {0, 0, 0};
    if (abs(omega_e_enc - omega_e_hat) <= RPM_TO_RAD_PER_SEC(10)) {
        // Acquired!
        rgb_led.g = 255;
    } else {
        // Not acquired... :(
        rgb_led.r = 255;
    }
    io_led_set(&rgb_led);




    // ----------------------
    // Get current values
    // ----------------------
    double Iabc[3];
    _get_Iabc(Iabc);


    // ---------------------
    // Convert ABC to DQ
    // ---------------------
    double Ixyz[3];  // alpha beta gamma currents
    double Idq0[3]; // d q 0 currents
    transform_clarke(TRANS_DQZ_C_INVARIANT_POWER, Iabc, Ixyz);

    // -------------------
    // Inject signals into I_alpha,beta*
    // (constants, chirps, noise, etc)
    // -------------------
    injection_inj(&Ixyz[0], &cc_inj_ctx_Ialpha_star, Ts);
    injection_inj(&Ixyz[1], &cc_inj_ctx_Ibeta_star,  Ts);

    transform_park(theta_e, Ixyz, Idq0);


    // -----------------------------
    // Run through block diagram of CVCR to get Vdq_star
    // -----------------------------
    double Id = Idq0[0];
    double Iq = Idq0[1];

    // d-axis
    double Id_err;
    double Vd_star;
    Id_err = Id_star - Id;
    Id_err_acc += Id_err;
    Vd_star = (Kp_d * Id_err) + (Ki_d * Ts * Id_err_acc) - (omega_e_avg * Kp_q * Ts * Iq_err_acc);

    // q-axis
    double Iq_err;
    double Vq_star;
    Iq_err = Iq_star - Iq;
    Iq_err_acc += Iq_err;
    Vq_star = (Kp_q * Iq_err) + (Ki_q * Ts * Iq_err_acc) + (omega_e_avg * Kp_d * Ts * Id_err_acc) + (omega_e_avg * Lambda_pm_HAT);


//    static int counter = 0;
//    if (++counter >= 1000) {
//        counter = 0;
//        debug_printf("Vd*=%f, Vq*=%f\r\n", Vd_star, Vq_star);
//    }



    // -------------------
    // Inject signals into Vdq*
    // (constants, chirps, noise, etc)
    // -------------------
    injection_inj(&Vd_star, &cc_inj_ctx_Vd_star, Ts);
    injection_inj(&Vq_star, &cc_inj_ctx_Vq_star, Ts);


    // --------------------------------
    // Perform inverse DQ transform of Vdq_star
    // --------------------------------

    double Vabc_star[3];
    double Vxyz_star[3];
    double Vdq0_star[3];
    Vdq0_star[0] = Vd_star;
    Vdq0_star[1] = Vq_star;
    Vdq0_star[2] = 0.0;

    // DQZ to alpha beta gamma:
    transform_park_inverse(theta_e, Vdq0_star, Vxyz_star);

    // Full DQZ to ABC:
    transform_dqz_inverse(TRANS_DQZ_C_INVARIANT_POWER,
                theta_e, Vdq0_star, Vabc_star);

    // ------------------------------------
    // Saturate Vabc_star to inverter bus voltage
    // ------------------------------------
    inverter_saturate_to_Vdc(&Vabc_star[0]);
    inverter_saturate_to_Vdc(&Vabc_star[1]);
    inverter_saturate_to_Vdc(&Vabc_star[2]);


    // --------------------------------------
    // Write voltages out to PWM hardware
    // --------------------------------------
    inverter_set_voltage(CC_PHASE_A_PWM_LEG_IDX, Vabc_star[0], Iabc[0]);
    inverter_set_voltage(CC_PHASE_B_PWM_LEG_IDX, Vabc_star[1], Iabc[1]);
    inverter_set_voltage(CC_PHASE_C_PWM_LEG_IDX, Vabc_star[2], Iabc[2]);


    // -------------------
    // Store LOG variables
    // -------------------
    LOG_Vd_star = Vd_star;
    LOG_Vq_star = Vq_star;
    LOG_Id_star = Id_star;
    LOG_Iq_star = Iq_star;
    LOG_Id = Id;
    LOG_Iq = Iq;


    // -------------------
    // Self-sensing:
    // -------------------

    // Back-EMF State Filter (stationary ref frame)
    co_stat_update(
            Ixyz[0], Ixyz[1],
            Vxyz_star[0], Vxyz_star[1],
            omega_e_avg);

    // Pull out Esal
    double Esal_alpha, Esal_beta;
    co_stat_get_Esal_hat(&Esal_alpha, &Esal_beta);

    // Back-EMF Tracking Position State Filter
    bemfo_update(Esal_alpha, Esal_beta, 0.0);
}

void task_cc_clear(void)
{
    Id_star = 0.0;
    Iq_star = 0.0;

    inverter_set_voltage(CC_PHASE_A_PWM_LEG_IDX, 0.0, 0.0);
    inverter_set_voltage(CC_PHASE_B_PWM_LEG_IDX, 0.0, 0.0);
    inverter_set_voltage(CC_PHASE_C_PWM_LEG_IDX, 0.0, 0.0);
}

void task_cc_set_dq_offset(int32_t offset) {
    dq_offset = offset;
}

void task_cc_set_bw(double bw)
{
    controller_bw = bw;
}

void task_cc_set_Iq_star(double value) {
    Iq_star = value;
}

void task_cc_set_Id_star(double value) {
    Id_star = value;
}

void task_cc_set_theta_e_src(uint8_t use_encoder)
{
    theta_e_src_use_encoder = use_encoder;
}

void task_cc_set_omega_e_src(uint8_t use_encoder)
{
    omega_e_src_use_encoder = use_encoder;
}

#endif // APP_BETA_LABS
