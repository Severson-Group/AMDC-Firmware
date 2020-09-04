#ifdef APP_DAC

#include "usr/dac/app_dac.h"
#include "usr/dac/cmd/cmd_dac.h"
#include "usr/dac/task_dac.h"

void app_dac_init(void)
{
    // Registers the "dac" command with the system
    cmd_dac_register();

    // Initialize the dac task with the system
    task_dac_init();
}

#endif // APP_DAC
