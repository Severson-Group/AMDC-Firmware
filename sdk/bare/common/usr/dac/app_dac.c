#ifdef APP_DAC

#include "usr/dac/app_dac.h"
#include "usr/dac/cmd/cmd_dac.h"
#include "usr/dac/task_dac.h"

void app_dac_init(void)
{
    // Register "blink" command with system
    cmd_dac_register();

    // Initialize blink task with system
    task_dac_init();
}

#endif // APP_DAC

