#ifdef APP_BETA_LABS

#include "usr/beta_labs/mcff.h"
#include "usr/beta_labs/machine.h"

// Output States
static double Te_cff_jp = 0.0;
static double Te_cff_bp = 0.0;
static double Te_cff_total = 0.0;

void mcff_update(double omega_m_star, double omega_dot_m_star)
{
    Te_cff_jp = Jp_HAT * omega_dot_m_star;
    Te_cff_bp = Bp_HAT * omega_m_star;
    Te_cff_total = Te_cff_jp + Te_cff_bp;
}

double mcff_get_Te_cff_total(void)
{
    return Te_cff_total;
}

double mcff_get_Te_cff_bp(void)
{
    return Te_cff_bp;
}

double mcff_get_Te_cff_jp(void)
{
    return Te_cff_jp;
}

#endif // APP_BETA_LABS
