#include "usr/user_config.h"

#if USER_CONFIG_ENABLE_INJECTION == 1

#include "cmd_inj.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/injection.h"
#include "sys/util.h"
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "clear", "Clear all injections" },
    { "list", "List all available injection points" },
    { "const <name> <set|add|sub> <value>", "Inject a constant" },
    { "noise <name> <set|add|sub> <gain> <offset>", "Inject noise" },
    { "chirp <name> <set|add|sub> <gain> <freqMin> <freqMax> <period>", "Inject chirp" },
    { "triangle <name> <set|add|sub> <valueMin> <valueMax> <period>", "Inject triangle" },
};

void cmd_inj_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry, "inj", "Inject signals into system", cmd_help, ARRAY_SIZE(cmd_help), cmd_inj);

    // Register the command
    commands_cmd_register(&cmd_entry);
}

static int _parse_op(char *op_str, inj_op_e *inj_op)
{
    if (STREQ("set", op_str)) {
        *inj_op = SET;
        return 0;
    } else if (STREQ("add", op_str)) {
        *inj_op = ADD;
        return 0;
    } else if (STREQ("sub", op_str)) {
        *inj_op = SUB;
        return 0;
    }

    return -1;
}

int cmd_inj(int argc, char **argv)
{
    // Handle 'inj clear' command
    if (argc == 2 && STREQ("clear", argv[1])) {
        injection_clear();
        return CMD_SUCCESS;
    }

    // Handle 'inj list' command
    if (argc == 2 && STREQ("list", argv[1])) {
        injection_list();
        return CMD_SUCCESS;
    }

    // Handle 'inj const ...' command
    if (argc == 5 && STREQ("const", argv[1])) {
        // Parse out name and convert to injection context
        inj_ctx_t *ctx = injection_find_ctx_by_name(argv[2]);
        if (ctx == NULL)
            return CMD_INVALID_ARGUMENTS;

        // Parse out operation
        inj_op_e op;
        if (_parse_op(argv[3], &op) != 0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out value argument
        // and saturate to -10 .. 10
        double value = strtod(argv[5], NULL);
        if (value < -10.0 || value > 10.0)
            return CMD_INVALID_ARGUMENTS;

        injection_const(ctx, op, value);

        return CMD_SUCCESS;
    }

    // Handle 'inj noise ...' command
    if (argc == 6 && STREQ("noise", argv[1])) {
        // Parse out name and convert to injection context
        inj_ctx_t *ctx = injection_find_ctx_by_name(argv[2]);
        if (ctx == NULL)
            return CMD_INVALID_ARGUMENTS;

        // Parse out operation
        inj_op_e op;
        if (_parse_op(argv[3], &op) != 0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out gain argument
        // and saturate to 0 .. 50
        double gain = strtod(argv[4], NULL);
        if (gain < 0.0 || gain > 50.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out offset argument
        // and saturate to -100 .. 100
        double offset = strtod(argv[5], NULL);
        if (offset < -100.0 || offset > 100.0)
            return CMD_INVALID_ARGUMENTS;

        injection_noise(ctx, op, gain, offset);

        return CMD_SUCCESS;
    }

    // Handle 'inj chirp ...' command
    if (argc == 8 && STREQ("chirp", argv[1])) {
        // Parse out name and convert to injection context
        inj_ctx_t *ctx = injection_find_ctx_by_name(argv[2]);
        if (ctx == NULL)
            return CMD_INVALID_ARGUMENTS;

        // Parse out operation
        inj_op_e op;
        if (_parse_op(argv[3], &op) != 0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out gain argument
        // and saturate to 0 .. 10
        double gain = strtod(argv[4], NULL);
        if (gain < 0.0 || gain > 10.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out freqMin argument
        // and saturate to 0 .. 10000Hz
        double freqMin = strtod(argv[5], NULL);
        if (freqMin < 0.0 || freqMin > 10000.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out freqMax argument
        // and saturate to 0 .. 10000Hz
        double freqMax = strtod(argv[5], NULL);
        if (freqMax < 0.0 || freqMax > 10000.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out period argument
        // and saturate to 10ms .. 60 sec
        double period = strtod(argv[7], NULL);
        if (period < 0.010 || period > 60.0)
            return CMD_INVALID_ARGUMENTS;

        injection_chirp(ctx, op, gain, freqMin, freqMax, period);

        return CMD_SUCCESS;
    }

    // Handle 'inj triangle ...' command
    if (argc == 7 && STREQ("triangle", argv[1])) {
        // Parse out name and convert to injection context
        inj_ctx_t *ctx = injection_find_ctx_by_name(argv[2]);
        if (ctx == NULL)
            return CMD_INVALID_ARGUMENTS;

        // Parse out operation
        inj_op_e op;
        if (_parse_op(argv[3], &op) != 0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out valueMin argument
        // and saturate to -100 .. 100
        double valueMin = strtod(argv[4], NULL);
        if (valueMin < -100.0 || valueMin > 100.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out valueMax argument
        // and saturate to -100 .. 100
        double valueMax = strtod(argv[5], NULL);
        if (valueMax < -100.0 || valueMax > 100.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out period argument
        // and saturate to 10ms .. 60 sec
        double period = strtod(argv[6], NULL);
        if (period < 0.010 || period > 60.0)
            return CMD_INVALID_ARGUMENTS;

        injection_triangle(ctx, op, valueMin, valueMax, period);

        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // USER_CONFIG_ENABLE_INJECTION
