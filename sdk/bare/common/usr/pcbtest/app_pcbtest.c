#ifdef APP_PCBTEST

#include "usr/pcbtest/app_pcbtest.h"
#include "drv/pwm.h"
#include "usr/pcbtest/cmd/cmd_test.h"

void app_pcbtest_init(void)
{
    cmd_test_register();

    // Set all PWM outputs to 50% duty ratio by default
    // TODO: we ought to open all switches, but FPGA does not support this yet...
    for (int i = 0; i < 24; i++) {
        pwm_set_duty(i, 0.5);
    }
}

#endif // APP_PCBTEST
