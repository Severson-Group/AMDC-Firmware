#include "user_apps.h"

// ENABLED APPLICATIONS:
//
// Uncomment the following defines to compile
// each application into the executable

#ifdef APP_PMSM_MC
#include "pmsm_mc/app_pmsm_mc.h"
#endif

#ifdef APP_DAC_TEST
#include "dac_test/app_dac_test.h"
#endif

#ifdef APP_DEADTIME_COMP
#include "deadtime_comp/app_deadtime_comp.h"
#endif

#ifdef APP_PARAMS
#include "params/app_params.h"
#endif

void user_apps_init(void)
{
#ifdef APP_PMSM_MC
	app_pmsm_mc_init();
#endif

#ifdef APP_DAC_TEST
	app_dac_test_init();
#endif

#ifdef APP_DEADTIME_COMP
	app_deadtime_comp_init();
#endif

#ifdef APP_PARAMS
	app_params_init();
#endif
}
