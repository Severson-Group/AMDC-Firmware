#ifdef APP_DEMO

#include "usr/demo/app_demo.h"
#include "usr/demo/task_cc.h"
#include "usr/demo/cmd/cmd_cc.h"

void app_demo_init(void)
{
    // Register "cc" command with system
    cmd_cc_register();

    // Initialize current control task with system
    //task_cc_init();
}

#endif // APP_DEMO
