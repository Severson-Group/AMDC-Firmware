#include "sys/cmd/cmd_hw.h"
#include "drv/analog.h"
#include "drv/encoder.h"
#include "drv/fpga_timer.h"
#include "drv/hardware_targets.h"
#include "drv/pwm.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "usr/user_config.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
#include "drv/led.h"
#endif // USER_CONFIG_HARDWARE_TARGET

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "pwm sw <freq_switching> <deadtime_ns>", "Set the PWM switching characteristics" },
    { "pwm duty <pwm_idx> <percent>", "Set a duty ratio" },
    { "anlg read <chnl_idx>", "Read voltage on ADC channel" },
    { "enc steps", "Read encoder steps from power-up" },
    { "enc pos", "Read encoder position" },
    { "enc init", "Turn on blue LED until Z pulse found" },
    { "timer now", "Read value from FPGA timer" },

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
    { "led set <led_idx> <r> <g> <b>", "Set LED color (color is 0..255)" },
#endif // USER_CONFIG_HARDWARE_TARGET
};

void cmd_hw_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry, "hw", "Hardware related commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_hw);

    // Register the command
    commands_cmd_register(&cmd_entry);
}

// Handles the 'hw' command and all sub-commands
int cmd_hw(int argc, char **argv)
{
    // Handle 'pwm' sub-command
    if (argc >= 2 && STR_EQ("pwm", argv[1])) {
        if (argc == 5 && STR_EQ("sw", argv[2])) {
            // Parse out switching freq arg
            double fsw = strtod(argv[3], NULL);
            if (fsw > 2e6 || fsw < 2e3) {
                return CMD_INVALID_ARGUMENTS;
            }

            // Parse out dead time arg
            int dt = atoi(argv[4]);
            if (dt > 5000 || dt < 25) {
                return CMD_INVALID_ARGUMENTS;
            }

            pwm_set_deadtime_ns(dt);
            pwm_set_switching_freq(fsw);

            return CMD_SUCCESS;
        }

        if (argc == 5 && STR_EQ("duty", argv[2])) {
            // Parse out switching pwm_idx arg
            int pwm_idx = atoi(argv[3]);

            if (!pwm_is_valid_channel(pwm_idx)) {
                return CMD_INVALID_ARGUMENTS;
            }

            // Parse out percent arg
            double percent = strtod(argv[4], NULL);
            if (percent > 1.0 || percent < 0.0) {
                return CMD_INVALID_ARGUMENTS;
            }

            pwm_set_duty(pwm_idx, percent);

            return CMD_SUCCESS;
        }
    }

    // Handle 'anlg' sub-command
    if (argc >= 2 && STR_EQ("anlg", argv[1])) {
        if (argc == 4 && STR_EQ("read", argv[2])) {
            // Parse out analog channel arg
            int anlg_idx = atoi(argv[3]);

            if (!analog_is_valid_channel(anlg_idx)) {
                return CMD_INVALID_ARGUMENTS;
            }

            float out_volts;
            analog_getf(anlg_idx, &out_volts);

            debug_printf("%fV\r\n", out_volts);

            return CMD_SUCCESS;
        }
    }

    // Handle 'enc' sub-command
    if (argc >= 2 && STR_EQ("enc", argv[1])) {
        if (argc == 3 && STR_EQ("steps", argv[2])) {
            int32_t steps;
            encoder_get_steps(&steps);

            debug_printf("steps: %ld\r\n", steps);

            return CMD_SUCCESS;
        }

        if (argc == 3 && STR_EQ("pos", argv[2])) {
            uint32_t position;
            encoder_get_position(&position);

            debug_printf("pos: %ld\r\n", position);

            return CMD_SUCCESS;
        }

        if (argc == 3 && STR_EQ("init", argv[2])) {
            encoder_find_z();

            return CMD_SUCCESS;
        }
    }

    // Handle 'timer' sub-command
    if (argc >= 2 && STR_EQ("timer", argv[1])) {
        if (argc == 3 && STR_EQ("now", argv[2])) {
            uint32_t counts = fpga_timer_now();

            debug_printf("counts: %lu\r\n", counts);

            return CMD_SUCCESS;
        }
    }

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
    // Handle 'led' sub-command
    // hw led set <led_idx> <r> <g> <b>
    if (argc >= 2 && STR_EQ("led", argv[1])) {
        if (argc == 7 && STR_EQ("set", argv[2])) {
            int led_idx = atoi(argv[3]);
            if (led_idx < 0 || led_idx >= NUM_LEDS)
                return CMD_INVALID_ARGUMENTS;

            int r = atoi(argv[4]);
            int g = atoi(argv[5]);
            int b = atoi(argv[6]);

            if (r < 0 || r > 255)
                return CMD_INVALID_ARGUMENTS;
            if (g < 0 || g > 255)
                return CMD_INVALID_ARGUMENTS;
            if (b < 0 || b > 255)
                return CMD_INVALID_ARGUMENTS;

            led_set_color_bytes(led_idx, r, g, b);

            return CMD_SUCCESS;
        }
    }
#endif // USER_CONFIG_HARDWARE_TARGET

    return CMD_INVALID_ARGUMENTS;
}
