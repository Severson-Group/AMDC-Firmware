#include "user_apps.h"

// Adding applications:
//
// To add a new application, create a block
// like the other example(s) below which
// conditionally includes the app's header file.
//
// Also, conditionally call the app's init function.

#ifdef APP_BETA_LABS
#include "beta_labs/app_beta_labs.h"
#endif

void user_apps_init(void)
{
#ifdef APP_BETA_LABS
	app_beta_labs_init();
#endif
}
