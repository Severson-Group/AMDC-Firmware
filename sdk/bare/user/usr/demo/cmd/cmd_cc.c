#ifdef APP_DEMO

#include "usr/demo/cmd/cmd_cc.h"
#include "usr/demo/task_cc.h"
#include "drv/analog.h"
#include "drv/pwm.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES (6)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
	{"init", "Initialize current controller"},
	{"deinit", "Deinitialize current controller"},
	{"vdc <volts>", "Set DC link voltage"},
	{"pwm <a|b|c> <pwm_chnl>", "Configure PWM outputs per phase"},
	{"adc <a|b|c> <adc_chnl> <adc_gain> <adc_offset>", "Configure ADC input per phase"},
	{"tune <Rs> <Ld> <Lq> <bw>", "Tune with Rs [ohms], Ldq [H], and bandwidth [rad/s]"},
	{"set <Id*> <Iq*> <omega_e>", "Set operating point for Idq* [Adq] at omega_e [rad/s]"},
};

void cmd_cc_register(void)
{
    commands_cmd_init(&cmd_entry,
            "cc", "Current control (D/Q) related commands",
            cmd_help, NUM_HELP_ENTRIES,
            cmd_cc
    );

    // Register the command with the system
    commands_cmd_register(&cmd_entry);
}

static inline bool STREQ(char *in1, char *in2)
{
	return (strcmp(in1, in2) == 0) ? true : false;
}

int cmd_cc(int argc, char **argv)
{
    if (argc == 2 && STREQ("init", argv[1])) {
    	// Enable current controller
    	task_cc_init();
    	return SUCCESS;
    }

    if (argc == 2 && STREQ("deinit", argv[1])) {
    	// Disable current controller
    	task_cc_deinit();
    	return SUCCESS;
    }

    if (argc == 3 && STREQ("vdc", argv[1])) {
    	// Set DC link voltage
    	double vdc = strtod(argv[2], NULL);

    	// Sanitize inputs
    	if (vdc <= 0.0 || vdc > 1000.0) return INVALID_ARGUMENTS;

    	task_cc_vdc_set(vdc);
    	return SUCCESS;
    }

    if (argc == 4 && STREQ("pwm", argv[1])) {
    	// Extract phase user is configuring
    	uint8_t phase;
    	if (STREQ("a", argv[2])) {
    		phase = 0;
    	} else if (STREQ("b", argv[2])) {
    		phase = 1;
    	} else if (STREQ("c", argv[2])) {
    		phase = 2;
    	} else {
    		return INVALID_ARGUMENTS;
    	}

    	uint8_t pwm_chnl = atoi(argv[3]);
    	if (pwm_chnl <= 0 || pwm_chnl >= 24) {
    		return INVALID_ARGUMENTS;
    	}

    	task_cc_pwm(phase, pwm_chnl);

    	return SUCCESS;
    }

    if (argc == 6 && STREQ("adc", argv[1])) {
		// Extract phase user is configuring
		uint8_t phase;
		if (STREQ("a", argv[2])) {
			phase = 0;
		} else if (STREQ("b", argv[2])) {
			phase = 1;
		} else if (STREQ("c", argv[2])) {
			phase = 2;
		} else {
			return INVALID_ARGUMENTS;
		}

		uint8_t adc_chnl = atoi(argv[3]);
		if (adc_chnl <= 0 || adc_chnl >= ANALOG_NUM_CHANNELS) {
			return INVALID_ARGUMENTS;
		}

		double adc_gain = strtod(argv[4], NULL);
		double adc_offset = strtod(argv[4], NULL);

		if (adc_gain <= -1e6 || adc_gain >= 1e6) return INVALID_ARGUMENTS;
		if (adc_offset <= -1e6 || adc_offset >= 1e6) return INVALID_ARGUMENTS;

		task_cc_adc(phase, adc_chnl, adc_gain, adc_offset);

		return SUCCESS;
	}

    if (argc == 6 && STREQ("tune", argv[1])) {
    	// Tune current controller
    	double Rs, Ld, Lq, bw;

    	// Extract arguments
    	Rs = strtod(argv[2], NULL);
    	Ld = strtod(argv[3], NULL);
    	Lq = strtod(argv[4], NULL);
    	bw = strtod(argv[5], NULL);

    	// Sanitize inputs
    	if (Rs <= 0.0 || Rs > 100.0) return INVALID_ARGUMENTS;
    	if (Ld <= 0.0 || Ld > 100.0) return INVALID_ARGUMENTS;
    	if (Lq <= 0.0 || Lq > 100.0) return INVALID_ARGUMENTS;
    	if (bw <= 0.0 || bw > 100000.0) return INVALID_ARGUMENTS;

    	task_cc_tune(Rs, Ld, Lq, bw);
    	return SUCCESS;
    }

    if (argc == 5 && STREQ("set", argv[1])) {
    	// Tune current controller
    	double Id_star, Iq_star, omega_e;

    	// Extract arguments
    	Id_star = strtod(argv[2], NULL);
    	Iq_star = strtod(argv[3], NULL);
    	omega_e = strtod(argv[4], NULL);

    	// Sanitize inputs
    	if (Id_star <= -100.0 || Id_star >= 100.0) return INVALID_ARGUMENTS;
    	if (Iq_star <= -100.0 || Iq_star >= 100.0) return INVALID_ARGUMENTS;
    	if (omega_e <= -1e6 || omega_e >= 1e6) return INVALID_ARGUMENTS;

    	task_cc_set(Id_star, Iq_star, omega_e);
    	return SUCCESS;
    }

    return INVALID_ARGUMENTS;
}

#endif // APP_DEMO
