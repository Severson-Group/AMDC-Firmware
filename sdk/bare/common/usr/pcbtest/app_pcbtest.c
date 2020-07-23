#ifdef APP_PCBTEST

#include "usr/pcbtest/app_pcbtest.h"
#include "drv/pwm.h"
#include "usr/pcbtest/cmd/cmd_test.h"

void app_pcbtest_init(void)
{
    cmd_test_register();
}

#endif // APP_PCBTEST
