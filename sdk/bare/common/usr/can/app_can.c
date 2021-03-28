#ifdef APP_CAN

#include "usr/can/app_can.h"
#include "usr/can/cmd/cmd_can.h"
#include "usr/can/task_can.h"

void app_can_init(void)
{
    // Registers the "can" command with the system
    cmd_can_register();

    // Initialize the can task with the system
    task_can_init();
}

#endif // APP_CAN
