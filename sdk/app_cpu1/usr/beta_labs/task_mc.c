#ifdef APP_BETA_LABS

#include "usr/beta_labs/task_mc.h"
#include "drv/dac.h"
#include "drv/encoder.h"
#include "drv/io.h"
#include "machine.h"
#include "sys/injection.h"
#include "sys/scheduler.h"
#include "usr/beta_labs/bemfo.h"
#include "usr/beta_labs/mc.h"
#include "usr/beta_labs/mcff.h"
#include "usr/beta_labs/msf.h"
#include "usr/beta_labs/task_cc.h"
#include "usr/beta_labs/task_mo.h"

#define Ts (1.0 / TASK_MC_UPDATES_PER_SEC)

// Current limits which mc can command to cc
#define MIN_CC_CURRENT (-I_rated_dq) // Amps, in DQ frame
#define MAX_CC_CURRENT (+I_rated_dq) // Amps, in DQ frame

// Forward declarations
inline static int saturate(double min, double max, double *value);

// Injection contexts for motion controller
inj_ctx_t task_mc_inj_omega_m_star;
inj_ctx_t task_mc_inj_Td_star;

// Command for controller
static double omega_m_star = 0.0;
static uint8_t mcff_enabled = 0;
static uint8_t omega_m_src_use_encoder = 1;

static task_control_block_t tcb;

// Logging variables
double LOG_omega_m_star = 0.0;
double LOG_omega_m = 0.0;
double LOG_T_sfb = 0.0;
double LOG_T_cff = 0.0;
double LOG_T_d = 0.0;
double LOG_msf_w_m = 0.0;
double LOG_msf_w_m_dot = 0.0;

uint8_t task_mc_is_inited(void)
{
    return scheduler_tcb_is_registered(&tcb);
}

void task_mc_init(void)
{
    // Register scheduler task
    scheduler_tcb_init(&tcb, task_mc_callback, NULL, "mc", TASK_MC_INTERVAL_USEC);
    scheduler_tcb_register(&tcb);

    // Initialize injection contexts
    injection_ctx_init(&task_mc_inj_omega_m_star, "omega_m*");
    injection_ctx_init(&task_mc_inj_Td_star, "Td*");

    // Register injection contexts
    injection_ctx_register(&task_mc_inj_omega_m_star);
    injection_ctx_register(&task_mc_inj_Td_star);

    // Initialize motion controller
    mc_init();

    // Clear commanded speed
    omega_m_star = 0.0;

    // Initialize motion state filter
    msf_init();
}

void task_mc_deinit(void)
{
    // Unregister scheduler task
    scheduler_tcb_unregister(&tcb);

    // Clear current command
    task_cc_set_Iq_star(0.0);

    // Unregister injection context
    injection_ctx_unregister(&task_mc_inj_omega_m_star);
    injection_ctx_unregister(&task_mc_inj_Td_star);

    // Clear injection context
    injection_ctx_clear(&task_mc_inj_omega_m_star);
    injection_ctx_clear(&task_mc_inj_Td_star);

    // Initialize motion controller
    mc_init();

    // Clear commanded speed
    omega_m_star = 0.0;
}

void task_mc_callback(void *arg)
{
    // Inject signal into omega_m*
    injection_inj(&omega_m_star, &task_mc_inj_omega_m_star, Ts);

    // Get speed from encoder
    double omega_m = 0.0;
    if (omega_m_src_use_encoder) {
        omega_m = task_mo_get_omega_m();
    } else {
        omega_m = bemfo_get_omega_m_hat();
    }

    double delta_theta_m = omega_m * Ts;

    // Motion State Filter
    double delta_theta_m_star = msf_update(omega_m_star);

    // Motion Command Feed-Forward
    mcff_update(msf_get_omega_m(), msf_get_omega_m_dot());
    double Tem_cff = 0.0;
    if (mcff_enabled) {
        Tem_cff = mcff_get_Te_cff_total();
    }

    // Motion Controller
    double Tem_SFB_star = mc_update(delta_theta_m_star, delta_theta_m, Ts);

    // Torque Disturbance
    // Inject user defined signal into Td*
    double Td_star = 0.0;
    injection_inj(&Td_star, &task_mc_inj_Td_star, Ts);

    // Sum up Tem*
    double Tem_star_total = Tem_SFB_star + Tem_cff + Td_star;

    // Convert to commanded current, Iq*
    double Iq_star = Tem_star_total / Kt_HAT;

    // Saturate Iq_star to limit
    io_led_color_t color = { 0, 0, 0 };
    if (saturate(-I_rated_dq, +I_rated_dq, &Iq_star) != 0)
        color.r = 255;
    io_led_set_c(1, 0, 0, &color);

    // Set value in current controller
    task_cc_set_Iq_star(Iq_star);

    // Update logging variables
    LOG_omega_m_star = omega_m_star;
    LOG_omega_m = omega_m;
    LOG_T_sfb = Tem_SFB_star;
    LOG_T_cff = Tem_cff;
    LOG_T_d = Td_star;
    LOG_msf_w_m = msf_get_omega_m();
    LOG_msf_w_m_dot = msf_get_omega_m_dot();

    // Output to DAC
    dac_set_output(0, LOG_omega_m, -50, 50);
    dac_set_output(1, LOG_T_sfb, -1, 1);
    dac_set_output(2, LOG_T_cff, -1, 1);
}

void task_mc_set_omega_m_star(double omega_m)
{
    omega_m_star = omega_m;
}

void task_mc_set_cff_enabled(uint32_t enabled)
{
    mcff_enabled = enabled;
}

void task_mc_set_omega_m_src(uint8_t use_encoder)
{
    omega_m_src_use_encoder = use_encoder;
}

inline static int saturate(double min, double max, double *value)
{
    if (*value < min) {
        // Lower bound saturation
        *value = min;
        return -1;
    } else if (*value > max) {
        // Upper bound saturation
        *value = max;
        return 1;
    } else {
        // No saturation
        return 0;
    }
}

#endif // APP_BETA_LABS
