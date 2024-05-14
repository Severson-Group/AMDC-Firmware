#include "sys/cmd/cmd_hw.h"
#include "drv/analog.h"
#include "drv/cpu_timer.h"
#include "drv/eddy_current_sensor.h"
#include "drv/encoder.h"
#include "drv/gp3io_mux.h"
#include "drv/gpio_direct.h"
#include "drv/gpio_mux.h"
#include "drv/ild1420.h"
#include "drv/led.h"
#include "drv/pwm.h"
#include "drv/sts_mux.h"
#include "drv/timing_manager.h"
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
    { "timer cpu now", "Read value from hardware timer" },
    { "led set <led_idx> <r> <g> <b>", "Set LED color (color is 0..255)" },
    { "mux <gpio|sts> <port> <device>", "Map the device driver in the FPGA to the hardware port" },
    { "mux <gpio|sts> list", "List the device drivers available in the FPGA to the hardware port" },
    { "gpio <read|write|toggle> <port> <pin> <HIGH|LOW>", "Read and write digital voltages directly to GPIO pins" },
    { "eddy timing <port> <sclk_freq_khz> <prop_delay_ns>",
      "The desired SCLK frequency (kHz) and one-way delay of the adapter board (ns)" },
    { "tm trigger <HIGH|LOW|BOTH>", "Trigger all sensors to sample on the PWM carrier's peak, valley, or both" },
    { "tm ratio <count>", "Set number of PWM instances that occur in order to assert trigger" },
    { "tm enable <adc|encoder|eddy> <port [if eddy]>",
      "Enable a sensor; if eddy is chosen, specify the port, otherwise, leave blank" },
    { "hw tm time <adc|encoder|eddy> <port [if eddy]>", "Read acquisition time of sensor" },
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
#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
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

#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
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

    // Handle 'eddy' sub-command
    if (argc >= 5 && STREQ("eddy", argv[1])) {

        int32_t port = atoi(argv[3]);
        uint32_t base_addr = 0;

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
        if (port < 1 || port > 2)
            return CMD_INVALID_ARGUMENTS;
        else
            base_addr = EDDY_CURRENT_SENSOR_1_BASE_ADDR;
#elif (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
        if (port == 1) {
            base_addr = EDDY_CURRENT_SENSOR_1_BASE_ADDR;
        } else if (port == 2) {
            base_addr = EDDY_CURRENT_SENSOR_2_BASE_ADDR;
        } else if (port == 3) {
            base_addr = EDDY_CURRENT_SENSOR_3_BASE_ADDR;
        } else if (port == 4) {
            base_addr = EDDY_CURRENT_SENSOR_4_BASE_ADDR;
        } else
            return CMD_INVALID_ARGUMENTS;
#endif

        // hw eddy timing <port> sclk_freq_khz prop_delay_ns
        if (argc == 6 && STREQ("timing", argv[2])) {

            uint32_t sclk_freq_khz = (uint32_t) atoi(argv[4]);
            uint32_t propogation_delay_ns = (uint32_t) atoi(argv[5]);

            eddy_current_sensor_set_timing(base_addr, sclk_freq_khz, propogation_delay_ns);

            return CMD_SUCCESS;
        }
    }

    // Handle 'tm' sub-command
    if (argc >= 2 && STREQ("tm", argv[1])) {
        // hw tm trigger_pwm <HIGH|LOW|BOTH>
        if (argc == 4 && STREQ("trigger_pwm", argv[2])) {
            if (STREQ("HIGH", argv[3])) {
                timing_manager_trigger_on_pwm_high();
            } else if (STREQ("LOW", argv[3])) {
                timing_manager_trigger_on_pwm_low();
            } else if (STREQ("BOTH", argv[3])) {
                timing_manager_trigger_on_pwm_both();
            } else {
                return CMD_INVALID_ARGUMENTS;
            }
            return CMD_SUCCESS;
        }

        // hw tm mode <AUTO|MANUAL>
        if (argc == 4 && STREQ("mode", argv[2])) {
            if (STREQ("AUTO", argv[3])) {
                timing_manager_set_mode(AUTOMATIC);
            } else if (STREQ("MANUAL", argv[3])) {
                timing_manager_set_mode(MANUAL);
            } else {
                return CMD_INVALID_ARGUMENTS;
            }
            return CMD_SUCCESS;
        }

        // hw tm send_trigger
        if (argc == 3 && STREQ("send_trigger", argv[2])) {
            // Verify that timing manager is in manual mode
            if ((Xil_In32(TIMING_MANAGER_BASE_ADDR) & 0x1) == 0) {
                timing_manager_send_manual_trigger();
            } else {
                xil_printf("Failed to send manual trigger. Is the timing manager in manual mode?");
                return CMD_FAILURE;
            }

            return CMD_SUCCESS;
        }

        // hw tm ratio <count>
        else if (argc == 4 && STREQ("ratio", argv[2])) {
            uint32_t ratio = (uint32_t)(atoi(argv[3]));
            if (ratio < 0) {
                return CMD_INVALID_ARGUMENTS;
            }
            timing_manager_set_ratio(ratio);
            return CMD_SUCCESS;
        }

        // hw tm enable <sensor> [port if eddy/amds]
        else if (argc >= 4 && STREQ("enable", argv[2])) {
            if (STREQ("encoder", argv[3])) {
                timing_manager_enable_encoder();
            } else if (STREQ("adc", argv[3])) {
                timing_manager_enable_adc();
            } else if (argc == 5 && STREQ("amds", argv[3])) {
                int32_t port = atoi(argv[4]);
                // enable AMDS based on selected port
                if (port >= 1 && port <= 4) {
                    timing_manager_enable_amds(port);
                } else {
                    return CMD_INVALID_ARGUMENTS;
                }
            } else if (argc == 5 && STREQ("eddy", argv[3])) {
                int32_t port = atoi(argv[4]);
                // enable eddy current sensor based on selected port
                if (port >= 1 && port <= 4) {
                    timing_manager_enable_eddy_current_sensor(port);
                } else {
                    return CMD_INVALID_ARGUMENTS;
                }
            }
            return CMD_SUCCESS;
        }

        // hw tm time <sensor>
        else if (argc >= 4 && STREQ("time", argv[2])) {
            statistics_t *stats;
            if (STREQ("encoder", argv[3])) {
                stats = timing_manager_get_stats_per_sensor(ENCODER);
            } else if (STREQ("amds", argv[3])) {
                int32_t port = atoi(argv[4]);
                // get time for AMDS based on selected port
                if (port == 1) {
                    stats = timing_manager_get_stats_per_sensor(AMDS_1);
                } else if (port == 2) {
                    stats = timing_manager_get_stats_per_sensor(AMDS_2);
                } else if (port == 3) {
                    stats = timing_manager_get_stats_per_sensor(AMDS_3);
                } else if (port == 4) {
                    stats = timing_manager_get_stats_per_sensor(AMDS_4);
                } else {
                    return CMD_INVALID_ARGUMENTS;
                }
            } else if (STREQ("eddy", argv[3])) {
                int32_t port = atoi(argv[4]);
                // get time for eddy current sensor based on selected port
                if (port == 1) {
                    stats = timing_manager_get_stats_per_sensor(EDDY_1);
                } else if (port == 2) {
                    stats = timing_manager_get_stats_per_sensor(EDDY_2);
                } else if (port == 3) {
                    stats = timing_manager_get_stats_per_sensor(EDDY_3);
                } else if (port == 4) {
                    stats = timing_manager_get_stats_per_sensor(EDDY_4);
                } else {
                    return CMD_INVALID_ARGUMENTS;
                }
            } else if (STREQ("adc", argv[3])) {
                stats = timing_manager_get_stats_per_sensor(ADC);
            } else {
                return CMD_INVALID_ARGUMENTS;
            }
            cmd_resp_printf("Time (us): %f\n\r", stats->value);
            return CMD_SUCCESS;
        }
    }

    // Handle 'timer' sub-command
    if (argc >= 2 && STREQ("timer", argv[1])) {
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

    // Handle 'gpio' sub-command
    // hw gpio read <port> <pin>
    // hw gpio write <port> <pin> <HIGH|LOW>
    // hw gpio toggle <port> <pin>
    if (argc >= 5 && STREQ("gpio", argv[1])) {

        // NOTE:
        // Users should enter ports and pins that are 1-indexed
        // However, the functions in gpio_direct.c require 0-indexed
        // arguments. That is why we subtract 1 from the user cmd input
        uint8_t gpio_port = atoi(argv[3]);
        uint8_t pin = atoi(argv[4]);

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
        if (gpio_port < 1 || gpio_port > 2)
            return CMD_INVALID_ARGUMENTS;

        if (pin < 1 || pin > 2)
            return CMD_INVALID_ARGUMENTS;

#elif (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
        if (gpio_port < 1 || gpio_port > 4)
            return CMD_INVALID_ARGUMENTS;

        if (pin < 1 || pin > 3)
            return CMD_INVALID_ARGUMENTS;
#endif

        if (argc == 5 && STREQ("read", argv[2])) {

            gpio_direct_level_t level = gpio_direct_read(gpio_port - 1, pin - 1);

            if (level == GPIO_DIRECT_HIGH) {
                cmd_resp_print("Read Result: HIGH\r\n");
            } else if (level == GPIO_DIRECT_LOW) {
                cmd_resp_print("Read Result: LOW\r\n");
            }

            return CMD_SUCCESS;
        } // end if "read"

        if (argc == 6 && STREQ("write", argv[2])) {

            char *level = argv[5];

            if (STREQ("HIGH", level)) {
                gpio_direct_write(gpio_port - 1, pin - 1, 1);
            } else if (STREQ("LOW", level)) {
                gpio_direct_write(gpio_port - 1, pin - 1, 0);
            } else {
                return CMD_INVALID_ARGUMENTS;
            }

            return CMD_SUCCESS;
        } // end if "write"

        if (argc == 5 && STREQ("toggle", argv[2])) {

            gpio_direct_toggle(gpio_port - 1, pin - 1);

            return CMD_SUCCESS;
        } // end if "write"

    } // end if "gpio" sub-command

    // Handle 'mux' sub-command
    // mux gpio <port#> <device#>
    if (argc >= 2 && STREQ("mux", argv[1])) {
        if (argc == 5 && STREQ("gpio", argv[2])) {
            int gpio_port = atoi(argv[3]);
            int device = atoi(argv[4]);

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
            if (device < 0 || device > GPIO_MUX_DEVICE_COUNT) {
                return CMD_INVALID_ARGUMENTS;
            }

            if (gpio_port < 1 || gpio_port > 2) {
                return CMD_INVALID_ARGUMENTS;
            }

            gpio_mux_set_device(gpio_port - 1, device);

#elif (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
            if (device < 0 || device > GP3IO_MUX_DEVICE_COUNT) {
                return CMD_INVALID_ARGUMENTS;
            }

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

#endif // USER_CONFIG_HARDWARE_TARGET

            return CMD_SUCCESS;
        } else if (argc == 4 && STREQ("gpio", argv[2]) && STREQ("list", argv[3])) {
            /* MAINTAINER NOTE:
             * These device listings come from the Vivado Block Design files,
             * amdc_revd.bd and amdc_reve.bd,
             * and should be kept in sync.
             */
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
            cmd_resp_print("AMDC REV D gpio device numbers:\r\n");
            cmd_resp_print("1. Eddy Current Sensor\r\n");
            cmd_resp_print("2. AMDS\r\n");
            cmd_resp_print("3. ILD1420 Proximity Sensor 1\r\n");
            cmd_resp_print("4. ILD1420 Proximity Sensor 2\r\n");
            cmd_resp_print("5. GPIO Direct (Port 1)\r\n");
            cmd_resp_print("6. GPIO Direct (Port 2)\r\n");
            cmd_resp_print("7. UNUSED\r\n");
            cmd_resp_print("8. UNUSED\r\n");
            return CMD_SUCCESS;

#elif (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
            cmd_resp_print("AMDC REV E & REV F gpio device numbers:\r\n");
            cmd_resp_print("1. AMDS\r\n");
            cmd_resp_print("2. Eddy Current Sensor\r\n");
            cmd_resp_print("3. ILD1420 Proximity Sensor\r\n");
            cmd_resp_print("4. GPIO Direct\r\n");
            cmd_resp_print("5. UNUSED\r\n");
            cmd_resp_print("6. UNUSED\r\n");
            cmd_resp_print("7. UNUSED\r\n");
            cmd_resp_print("8. UNUSED\r\n");
            return CMD_SUCCESS;

#endif // USER_CONFIG_HARDWARE_TARGET for hw mux gpio list
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
        } else if (argc == 4 && STREQ("sts", argv[2]) && STREQ("list", argv[3])) {
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
            cmd_resp_print("AMDC REV D sts device numbers:\r\n");
            // TODO: FILL IN BELOW AND REMOVE NEXT LINE
            cmd_resp_print("Please check the block design by opening hw/amdc_revd.bd in Vivado.\r\n");
            /*
            cmd_resp_print("1. \r\n");
            cmd_resp_print("2. \r\n");
            cmd_resp_print("3. \r\n");
            cmd_resp_print("4. \r\n");
            cmd_resp_print("5. \r\n");
            cmd_resp_print("6. \r\n");
            cmd_resp_print("7. \r\n");
            cmd_resp_print("8. \r\n");
            return CMD_SUCCESS;
            */

#elif (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
            cmd_resp_print("AMDC REV E & REV F sts device numbers:\r\n");
            // TODO: FILL IN BELOW AND REMOVE NEXT LINE
            cmd_resp_print("Please check the block design by opening hw/amdc_reve.bd in Vivado.\r\n");
            /*
            cmd_resp_print("1. \r\n");
            cmd_resp_print("2. \r\n");
            cmd_resp_print("3. \r\n");
            cmd_resp_print("4. \r\n");
            cmd_resp_print("5. \r\n");
            cmd_resp_print("6. \r\n");
            cmd_resp_print("7. \r\n");
            cmd_resp_print("8. \r\n");
            return CMD_SUCCESS;
            */

#endif // USER_CONFIG_HARDWARE_TARGET for hw mux sts list
        }
    }

    return CMD_INVALID_ARGUMENTS;
}
