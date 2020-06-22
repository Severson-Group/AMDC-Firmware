#include "cmd_hw.h"
#include "drv/analog.h"
#include "drv/encoder.h"
#include "drv/led.h"
#include "drv/pwm.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES (7)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
    {"pwm sw <freq_switching> <deadtime_ns>", "Set the PWM switching characteristics"},
    {"pwm duty <pwm_idx> <percent>", "Set a duty ratio"},
    {"anlg read <chnl_idx>", "Read voltage on ADC channel"},
    {"led set <led_idx> <r> <g> <b>", "Set LED color (color is 0..255)"},
    {"enc steps", "Read encoder steps from power-up"},
    {"enc pos", "Read encoder position"},
    {"enc init", "Turn on blue LED until Z pulse found"},
};

void cmd_hw_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry, "hw", "Hardware related commands", cmd_help, NUM_HELP_ENTRIES, cmd_hw);

    // Register the command
    commands_cmd_register(&cmd_entry);
}

//
// Handles the 'hw' command
// and all sub-commands
//
int cmd_hw(int argc, char **argv)
{
    // Handle 'pwm' sub-command
    if (strcmp("pwm", argv[1]) == 0) {
        if (strcmp("sw", argv[2]) == 0) {
            // Check correct number of arguments
            if (argc != 5)
                return INVALID_ARGUMENTS;

            // Parse out switching freq arg
            double fsw = (double) atoi(argv[3]);
            if (fsw > 1000000.0)
                return INVALID_ARGUMENTS;
            if (fsw < 2000.0)
                return INVALID_ARGUMENTS;

            // Parse out dead time arg
            int dt = atoi(argv[4]);
            if (dt > 5000)
                return INVALID_ARGUMENTS;
            if (dt < 25)
                return INVALID_ARGUMENTS;

            pwm_set_deadtime_ns(dt);
            pwm_set_switching_freq(fsw);

            return SUCCESS;
        }

        if (strcmp("duty", argv[2]) == 0) {
            // Check correct number of arguments
            if (argc != 5)
                return INVALID_ARGUMENTS;

            // Parse out switching pwm_idx arg
            int pwm_idx = atoi(argv[3]);
            if (pwm_idx > 23)
                return INVALID_ARGUMENTS;
            if (pwm_idx < 0)
                return INVALID_ARGUMENTS;

            // Parse out percent arg
            int percent = atoi(argv[4]);
            if (percent > 100)
                return INVALID_ARGUMENTS;
            if (percent < 0)
                return INVALID_ARGUMENTS;

            pwm_set_duty(pwm_idx, (double) percent / 100.0);

            return SUCCESS;
        }
    }

    // Handle 'anlg' sub-command
    if (strcmp("anlg", argv[1]) == 0) {
        if (strcmp("read", argv[2]) == 0) {
            // Check correct number of arguments
            if (argc != 4)
                return INVALID_ARGUMENTS;

            // Parse out switching pwm_idx arg
            int anlg_idx = atoi(argv[3]);
            if (anlg_idx > 15)
                return INVALID_ARGUMENTS;
            if (anlg_idx < 0)
                return INVALID_ARGUMENTS;

            float value;
            analog_getf(anlg_idx + 1, &value);

            debug_printf("%fV\r\n", value);

            return SUCCESS;
        }
    }

    // Handle 'led' sub-command
    // hw led set <led_idx> <r> <g> <b>
    if (argc == 7 && strcmp("led", argv[1]) == 0) {
    	if (strcmp("set", argv[2]) == 0) {
            int led_idx = atoi(argv[3]);
            if (led_idx < 0 || led_idx >= NUM_LEDS) return INVALID_ARGUMENTS;

            int r = atoi(argv[4]);
            int g = atoi(argv[5]);
            int b = atoi(argv[6]);

            if (r < 0 || r > 255) return INVALID_ARGUMENTS;
            if (g < 0 || g > 255) return INVALID_ARGUMENTS;
            if (b < 0 || b > 255) return INVALID_ARGUMENTS;

            led_set_color_bytes(led_idx, r, g, b);

            return SUCCESS;
    	}
    }


    // Handle 'enc' sub-command
    if (strcmp("enc", argv[1]) == 0) {
        if (strcmp("steps", argv[2]) == 0) {
            // Check correct number of arguments
            if (argc != 3)
                return INVALID_ARGUMENTS;

            int32_t steps;
            encoder_get_steps(&steps);

            debug_printf("steps: %ld\r\n", steps);

            return SUCCESS;
        }

        if (strcmp("pos", argv[2]) == 0) {
            // Check correct number of arguments
            if (argc != 3)
                return INVALID_ARGUMENTS;

            uint32_t position;
            encoder_get_position(&position);

            debug_printf("pos: %ld\r\n", position);

            return SUCCESS;
        }

        if (strcmp("init", argv[2]) == 0) {
            // Check correct number of arguments
            if (argc != 3)
                return INVALID_ARGUMENTS;

            encoder_find_z();

            return SUCCESS;
        }
    }

    return INVALID_ARGUMENTS;
}
