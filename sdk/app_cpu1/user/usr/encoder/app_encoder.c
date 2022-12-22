#ifdef APP_ENCODER

#include "usr/encoder/app_encoder.h"
#include "usr/encoder/cmd/cmd_encoder.h"
#include "usr/encoder/task_encoder.h"
#include "usr/encoder/task_vsi.h"

void app_encoder_init(void)
{
    // Register "blink" command with system
    cmd_encoder_register();

    // Initialize blink task with system
    task_encoder_init();

    task_vsi_init();
}

#endif // APP_ENCODER
