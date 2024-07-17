#ifdef APP_DEMO

#include "usr/demo/app_demo.h"
#include "drv/pwm.h"
#include "usr/demo/cmd/cmd_cc.h"
#include "usr/demo/task_cc.h"

void app_demo_init(void)
{
    // Register "cc" command with system
    cmd_cc_register();

    // Set all PWM outputs to 50% duty ratio by default
    // TODO: we ought to open all switches, but FPGA does not support this yet...
    for (int i = 0; i < 24; i++) {
        pwm_set_duty(i, 0.5);
    }
}

#endif // APP_DEMO
