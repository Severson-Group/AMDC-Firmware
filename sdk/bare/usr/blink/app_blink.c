#ifdef APP_BLINK

#include "app_blink.h"
#include "cmd/cmd_blink.h"
#include "task_blink.h"

void app_blink_init(void)
{
    // Register "blink" command with system
    cmd_blink_register();

    // Initialize blink task with system
    task_blink_init();
}

#endif // APP_BLINK
