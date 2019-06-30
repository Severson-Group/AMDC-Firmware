#ifdef APP_BETA_LABS

#include "bemfo.h"
#include "task_cc.h"
#include "machine.h"
#include <math.h>

// Tuning for Back EMF Observer with Ts = 1/20000 sec
// Bandwidth is tuned to 150 Hz
#define Ts	(5e-05)
#define Jp	(0.00636)
#define Bo	(5.340531115146804)
#define Ko	(412.6561969071234)
#define Kio	(2873.5941356473886)

typedef struct complex_t {
	double d;
	double q;
} complex_t;

// Forward declarations
static inline double sign(double x);
static inline double cross_product(complex_t *v1, complex_t *v2);

static double theta_m_error_acc;
static double Tem_hat;
static double TL_hat;
static double alpha_hat;

static double omega_m_hat_l;
static double omega_m_hat_l_next;
static double omega_m_hat_r;
static double omega_m_hat_r_last;
static double theta_m_hat_last;
static double theta_m_hat;
static double theta_e_hat_last;
static double theta_e_hat;
static double delta_theta_m_hat;

// Logging variables
double LOG_Esal_vec_d = 0.0;
double LOG_Esal_vec_q = 0.0;
double LOG_unit_vec_d = 0.0;
double LOG_unit_vec_q = 0.0;
double LOG_theta_e_err = 0.0;

complex_t unit_vec;
complex_t Esal_vec;

void bemfo_init(void)
{
	// Clear states
	theta_m_error_acc = 0.0;
	omega_m_hat_l = 0.0;
	omega_m_hat_r = 0.0;
	theta_m_hat = 0.0;
	theta_e_hat = 0.0;
}

void bemfo_start(void)
{
	// Reset the state
	theta_m_error_acc = 0.0;
	omega_m_hat_l = 0.0;
	omega_m_hat_r = 0.0;

	// Preload theta_m
	theta_m_hat = task_cc_get_theta_e_enc() / POLE_PAIRS;
}

void bemfo_update(double Esal_alpha, double Esal_beta, double Tcff_hat, double theta_e_enc)
{
	// -------------------------
	// Save 'last' values
	// -------------------------

	omega_m_hat_r_last = omega_m_hat_r;
	theta_m_hat_last = theta_m_hat;
	theta_e_hat_last = theta_e_hat;


	// -------------------------
	// Heterodyning Demodulation
	// -------------------------

	theta_e_hat = theta_m_hat_last * POLE_PAIRS;
	theta_e_hat = fmod(theta_e_hat, PI2);
	//theta_e_hat = theta_e_enc;

	// Wrap theta_e_hat
//	while (theta_e_hat > PI2) theta_e_hat -= PI2;
//	while (theta_e_hat < 0.0) theta_e_hat += PI2;

	// NOTE: If theta_e_hat comes from encoder, then the unit vector and Esal should be in-phase.
	// Because of our system setup, we must phase shift theta_e_hat by -90 degrees.
	// (This is because of the way we define our DQ transformation.)
//	unit_vec.d = sign(omega_m_hat_r_last) * cos(theta_e_hat - (PI / 2.0));
//	unit_vec.q = sign(omega_m_hat_r_last) * sin(theta_e_hat - (PI / 2.0));
//
//	// Normalize Esal vector to -1..1 in magnitude
//	double Esal_mag = sqrt((Esal_alpha*Esal_alpha) + (Esal_beta*Esal_beta));
//	Esal_vec.d = Esal_alpha / Esal_mag;
//	Esal_vec.q = Esal_beta  / Esal_mag;
//
//	LOG_Esal_vec_d = Esal_vec.d;
//	LOG_Esal_vec_q = Esal_vec.q;
//	LOG_unit_vec_d = unit_vec.d;
//	LOG_unit_vec_q = unit_vec.q;
//
//	// Generate error term (in mechanical degrees)
//	double theta_e_error = cross_product(&Esal_vec, &unit_vec);

	double theta_e_error = theta_e_enc - theta_e_hat;
	if (theta_e_error >  PI) theta_e_error -= PI2;
	if (theta_e_error < -PI) theta_e_error += PI2;

	double theta_m_error = theta_e_error / POLE_PAIRS;

	LOG_theta_e_err = theta_e_error;

	theta_m_error_acc += theta_m_error;

	// -------------------------
	// Saliency Tracking Observer
	// -------------------------


//	theta_m_error = (POLE_PAIRS * theta_e_enc) - theta_m_hat;
//	theta_m_error = fmod(theta_m_error, PI2);
//
//	if (theta_m_error >  PI) theta_m_error -= PI2;
//	if (theta_m_error < -PI) theta_m_error += PI2;
//
//	theta_m_error_acc = theta_m_error_acc + theta_m_error;

	Tem_hat = theta_m_error_acc * Kio * Ts + theta_m_error * Ko + 0;

	alpha_hat = Tem_hat * Ts / Jp; //note: alpha has Ts multiplied in

	omega_m_hat_l = omega_m_hat_l_next;
	omega_m_hat_l_next = omega_m_hat_l_next + alpha_hat;
	omega_m_hat_r = omega_m_hat_l + theta_m_error * Bo / Jp;
	delta_theta_m_hat = (omega_m_hat_r + omega_m_hat_r_last) * Ts / 2.0;
	theta_m_hat = theta_m_hat + delta_theta_m_hat;

	theta_m_hat = fmod(theta_m_hat, PI2);













	//	TL_hat = (theta_m_error_acc * Kio * Ts) + (theta_m_error * Ko);
//	Tem_hat = TL_hat + Tcff_hat;
//
//	alpha_hat = Tem_hat * Ts / Jp; // note: alpha has Ts multiplied in
//
//	omega_m_hat_l = omega_m_hat_l_next;
//	omega_m_hat_l_next += alpha_hat;
//
//	omega_m_hat_r = omega_m_hat_l + (theta_m_error * Bo / Jp);
//	delta_theta_m_hat = (omega_m_hat_r + omega_m_hat_r_last) * Ts / 2.0;
//	theta_m_hat = theta_m_hat_last + delta_theta_m_hat;



















	// Wrap theta_m
//	while (theta_m_hat > PI2) theta_m_hat -= PI2;
//	while (theta_m_hat < 0.0) theta_m_hat += PI2;

	// TODO: check if working:
	// feed encoder theta_e into unit vector generation
	// check that unit_vec.dq is in phase with Esal_vec.dq

	// TODO: give "fake" error to this module ~5 degrees
	// should see that same error on output

	// TODO: compared theta_r_hat with theta_e_enc
	// should be the "same"ish

	// TODO: close the MC loop with est, but keep ref frame trans using encoder

	// TODO: close loop with all est
}

double bemfo_get_omega_m_hat(void)
{
	return omega_m_hat_r_last;
}

double bemfo_get_theta_e_hat(void)
{
	return theta_e_hat_last;
}

static inline double sign(double x)
{
	if (x >= 0.0) return 1.0;
	return -1.0;
}

static inline double cross_product(complex_t *v1, complex_t *v2)
{
	return (v1->d * v2->q) - (v1->q * v2->d);
}

#endif // APP_BETA_LABS
