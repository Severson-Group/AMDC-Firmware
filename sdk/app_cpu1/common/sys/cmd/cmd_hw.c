#include "sys/cmd/cmd_hw.h"
#include "drv/analog.h"
#include "drv/cpu_timer.h"
#include "drv/encoder.h"
#include "drv/fpga_timer.h"
#include "drv/gp3io_mux.h"
#include "drv/gpio_mux.h"
#include "drv/ild1420.h"
#include "drv/led.h"
#include "drv/pwm.h"
#include "drv/sts_mux.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "usr/user_config.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "pwm <on|off>", "Turn on/off PWM switching" },
    { "pwm sw <freq_switching> <deadtime_ns>", "Set the PWM switching characteristics" },
    { "pwm duty <pwm_idx> <percent>", "Set a duty ratio" },
    { "anlg read <chnl_idx>", "Read voltage on ADC channel" },
    { "ild read", "Read the latest packet from ILD1420 sensor" },
    { "enc steps", "Read encoder steps from power-up" },
    { "enc pos", "Read encoder position" },
    { "enc init", "Turn on blue LED until Z pulse found" },
    { "timer <fpga|cpu> now", "Read value from hardware timer" },
    { "led set <led_idx> <r> <g> <b>", "Set LED color (color is 0..255)" },
    { "mux <gpio|sts> <port> <device>", "Map the device driver in the FPGA to the hardware port" },
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
    if (argc >= 2 && STREQ("pwm", argv[1])) {
        if (argc == 3 && STREQ("on", argv[2])) {
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
            // Turn on PWM hardware before enabling
            // so that the first cycle has correct duty
            pwm_enable_hw(true);
#endif

            if (pwm_enable() != SUCCESS) {
                return CMD_FAILURE;
            }

            return CMD_SUCCESS;
        }

        if (argc == 3 && STREQ("off", argv[2])) {
            if (pwm_disable() != SUCCESS) {
                return CMD_FAILURE;
            }

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
            // Turn off PWM hardware after disabling
            // so that the last cycle has correct duty
            pwm_enable_hw(false);
#endif

            return CMD_SUCCESS;
        }

        if (argc == 5 && STREQ("sw", argv[2])) {
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

            if (pwm_set_deadtime_ns(dt) != SUCCESS) {
                return CMD_FAILURE;
            }

            if (pwm_set_switching_freq(fsw) != SUCCESS) {
                return CMD_FAILURE;
            }

            return CMD_SUCCESS;
        }

        if (argc == 5 && STREQ("duty", argv[2])) {
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
    if (argc >= 2 && STREQ("anlg", argv[1])) {
        if (argc == 4 && STREQ("read", argv[2])) {
            // Parse out analog channel arg
            int anlg_idx = atoi(argv[3]);

            if (!analog_is_valid_channel(anlg_idx)) {
                return CMD_INVALID_ARGUMENTS;
            }

            float out_volts;
            analog_getf(anlg_idx, &out_volts);

            cmd_resp_printf("%fV\r\n", out_volts);

            return SUCCESS;
        }
    }

    // Handle 'ild' sub-command
    // hw ild read
    if (argc == 4 && strcmp("ild", argv[1]) == 0) {
        if (strcmp("read", argv[2]) == 0) {
            int sensor = atoi(argv[3]);

            if (sensor < 0 || sensor >= ILD1420_NUM_SENSORS)
                return CMD_INVALID_ARGUMENTS;

            ild1420_packet_t packet = ild1420_get_latest_packet(sensor);
            cmd_resp_printf("dist:  %x\r\n", packet.distance);
            cmd_resp_printf("err:   %X\r\n", packet.error);
            cmd_resp_printf("fresh: %X\r\n", packet.fresh);

            return CMD_SUCCESS;
        }
    }

    // Handle 'enc' sub-command
    if (argc >= 2 && STREQ("enc", argv[1])) {
        if (argc == 3 && STREQ("steps", argv[2])) {
            int32_t steps;
            encoder_get_steps(&steps);

            cmd_resp_printf("steps: %ld\r\n", steps);

            return CMD_SUCCESS;
        }

        if (argc == 3 && STREQ("pos", argv[2])) {
            uint32_t position;
            encoder_get_position(&position);

            cmd_resp_printf("pos: %ld\r\n", position);

            return CMD_SUCCESS;
        }

        if (argc == 3 && STREQ("init", argv[2])) {
            encoder_find_z();

            return CMD_SUCCESS;
        }
    }

    // Handle 'timer' sub-command
    if (argc >= 2 && STREQ("timer", argv[1])) {
        if (argc == 4 && STREQ("fpga", argv[2]) && STREQ("now", argv[3])) {
            uint32_t counts1 = fpga_timer_now();
            uint32_t counts2 = fpga_timer_now();

            cmd_resp_printf("counts1: %lu\r\n", counts1);
            cmd_resp_printf("counts2: %lu\r\n", counts2);
            cmd_resp_printf("time delta = %8.3f ns\r\n", 1e3 * fpga_timer_ticks_to_usec(counts2 - counts1));

            return CMD_SUCCESS;
        }

        if (argc == 4 && STREQ("cpu", argv[2]) && STREQ("now", argv[3])) {
            uint32_t counts1 = cpu_timer_now();
            uint32_t counts2 = cpu_timer_now();

            cmd_resp_printf("counts1: %lu\r\n", counts1);
            cmd_resp_printf("counts2: %lu\r\n", counts2);
            cmd_resp_printf("time delta = %8.3f ns\r\n", 1e3 * cpu_timer_ticks_to_usec(counts2 - counts1));

            return CMD_SUCCESS;
        }
    }

    // Handle 'led' sub-command
    // hw led set <led_idx> <r> <g> <b>
    if (argc >= 2 && STREQ("led", argv[1])) {
        if (argc == 7 && STREQ("set", argv[2])) {
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

    // Handle 'mux' sub-command
    // mux gpio <port#> <device#>
    if (argc >= 2 && STREQ("mux", argv[1])) {
        if (argc == 5 && STREQ("gpio", argv[2])) {
            int gpio_port = atoi(argv[3]);
            int device = atoi(argv[4]);

            if (device < 0 || device > 4) {
                return CMD_INVALID_ARGUMENTS;
            }

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
            if (gpio_port < 1 || gpio_port > 2) {
                return CMD_INVALID_ARGUMENTS;
            }

            gpio_mux_set_device(gpio_port - 1, device);
#elif USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
            if (gpio_port < 1 || gpio_port > 4) {
                return CMD_INVALID_ARGUMENTS;
            }

            switch (gpio_port) {
            case 1:
                gp3io_mux_set_device(GP3IO_MUX_1_BASE_ADDR, device);
                break;
            case 2:
                gp3io_mux_set_device(GP3IO_MUX_2_BASE_ADDR, device);
                break;
            case 3:
                gp3io_mux_set_device(GP3IO_MUX_3_BASE_ADDR, device);
                break;
            case 4:
                gp3io_mux_set_device(GP3IO_MUX_4_BASE_ADDR, device);
                break;
            default:
                return CMD_INVALID_ARGUMENTS;
                break;
            }
#endif

            return CMD_SUCCESS;
        }

        if (argc == 5 && STREQ("sts", argv[2])) {
            int sts_port = atoi(argv[3]);
            int device = atoi(argv[4]);

            if (sts_port < 1 || sts_port > 8)
                return CMD_INVALID_ARGUMENTS;
            if (device < 0 || device > 8)
                return CMD_INVALID_ARGUMENTS;

            sts_mux_set_device(sts_port - 1, device);

            return CMD_SUCCESS;
        }
    }

    return CMD_INVALID_ARGUMENTS;
}
