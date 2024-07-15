#ifdef APP_PCBTEST

#include "usr/pcbtest/app_pcbtest.h"
#include "drv/pwm.h"
#include "usr/pcbtest/cmd/cmd_test.h"
#include "usr/user_config.h"

void app_pcbtest_init(void)
{
    cmd_test_register();

#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
    // Enable hardware output for PWM signals
    pwm_enable_hw(true);
#endif
}

#endif // APP_PCBTEST
