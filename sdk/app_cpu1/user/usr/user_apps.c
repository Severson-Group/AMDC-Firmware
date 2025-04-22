#include "usr/user_apps.h"

// Adding applications:
//
// To add a new application, create a block
// like the other example(s) below which
// conditionally includes the app's header file.
//
// Also, conditionally call the app's init function.

#ifdef APP_PCBTEST
#include "usr/pcbtest/app_pcbtest.h"
#endif

#ifdef APP_BLINK
#include "usr/blink/app_blink.h"
#endif

#ifdef APP_BETA_LABS
#include "usr/beta_labs/app_beta_labs.h"
#endif

#ifdef APP_DEMO
#include "usr/demo/app_demo.h"
#endif

#ifdef APP_DAC
#include "usr/dac/app_dac.h"
#endif

void user_apps_init(void)
{
#ifdef APP_PCBTEST
    app_pcbtest_init();
#endif

#ifdef APP_BLINK
    app_blink_init();
#endif

#ifdef APP_BETA_LABS
    app_beta_labs_init();
#endif

#ifdef APP_DEMO
    app_demo_init();
#endif

#ifdef APP_DAC
    app_dac_init();
#endif
}
