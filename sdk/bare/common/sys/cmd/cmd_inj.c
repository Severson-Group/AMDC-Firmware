#include "usr/user_defines.h"

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
    { "const <name> <set|add|sub> <mValue>", "Inject a constant" },
    { "noise <name> <set|add|sub> <mGain> <mOffset>", "Inject noise" },
    { "chirp <name> <set|add|sub> <mGain> <mFreqMin> <mFreqMax> <mPeriod>", "Inject chirp" },
    { "triangle <name> <set|add|sub> <mValueMin> <mValueMax> <mPeriod>", "Inject triangle" },
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
    if (strcmp("set", op_str) == 0) {
        *inj_op = SET;
        return 0;
    } else if (strcmp("add", op_str) == 0) {
        *inj_op = ADD;
        return 0;
    } else if (strcmp("sub", op_str) == 0) {
        *inj_op = SUB;
        return 0;
    }

    return -1;
}

int cmd_inj(int argc, char **argv)
{
    // Handle 'inj clear' command
    if (argc == 2 && strcmp("clear", argv[1]) == 0) {
        injection_clear();
        return CMD_SUCCESS;
    }

    // Handle 'inj list' command
    if (argc == 2 && strcmp("list", argv[1]) == 0) {
        injection_list();
        return CMD_SUCCESS;
    }

    // Handle 'inj const ...' command
    if (argc == 5 && strcmp("const", argv[1]) == 0) {
        // Parse out name and convert to injection context
        inj_ctx_t *ctx = injection_find_ctx_by_name(argv[2]);
        if (ctx == NULL)
            return CMD_INVALID_ARGUMENTS;

        // Parse out operation
        inj_op_e op;
        if (_parse_op(argv[3], &op) != 0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mValue argument
        // and saturate to -10 .. 10
        double mValue = (double) atoi(argv[5]);
        if (mValue < -10000.0)
            return CMD_INVALID_ARGUMENTS;
        if (mValue > 10000.0)
            return CMD_INVALID_ARGUMENTS;

        injection_const(ctx, op, mValue / 1000.0);

        return CMD_SUCCESS;
    }

    // Handle 'inj noise ...' command
    if (argc == 6 && strcmp("noise", argv[1]) == 0) {
        // Parse out name and convert to injection context
        inj_ctx_t *ctx = injection_find_ctx_by_name(argv[2]);
        if (ctx == NULL)
            return CMD_INVALID_ARGUMENTS;

        // Parse out operation
        inj_op_e op;
        if (_parse_op(argv[3], &op) != 0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mGain argument
        // and saturate to 0 .. 50
        double mGain = (double) atoi(argv[4]);
        if (mGain < 0.0)
            return CMD_INVALID_ARGUMENTS;
        if (mGain > 50000.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mOffset argument
        // and saturate to -100 .. 100
        double mOffset = (double) atoi(argv[5]);
        if (mOffset < -100000.0)
            return CMD_INVALID_ARGUMENTS;
        if (mOffset > 100000.0)
            return CMD_INVALID_ARGUMENTS;

        injection_noise(ctx, op, mGain / 1000.0, mOffset / 1000.0);

        return CMD_SUCCESS;
    }

    // Handle 'inj chirp ...' command
    if (argc == 8 && strcmp("chirp", argv[1]) == 0) {
        // Parse out name and convert to injection context
        inj_ctx_t *ctx = injection_find_ctx_by_name(argv[2]);
        if (ctx == NULL)
            return CMD_INVALID_ARGUMENTS;

        // Parse out operation
        inj_op_e op;
        if (_parse_op(argv[3], &op) != 0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mGain argument
        // and saturate to 0 .. 10
        double mGain = (double) atoi(argv[4]);
        if (mGain < 0.0)
            return CMD_INVALID_ARGUMENTS;
        if (mGain > 10000.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mFreqMin argument
        // and saturate to 0 .. 10000Hz
        double mFreqMin = (double) atoi(argv[5]);
        if (mFreqMin < 0.0)
            return CMD_INVALID_ARGUMENTS;
        if (mFreqMin > 10000000.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mFreqMax argument
        // and saturate to 0 .. 10000Hz
        double mFreqMax = (double) atoi(argv[6]);
        if (mFreqMax < 0.0)
            return CMD_INVALID_ARGUMENTS;
        if (mFreqMax > 10000000.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mPeriod argument
        // and saturate to 1 .. 60 sec
        double mPeriod = (double) atoi(argv[7]);
        if (mPeriod < 1000.0)
            return CMD_INVALID_ARGUMENTS;
        if (mPeriod > 60000.0)
            return CMD_INVALID_ARGUMENTS;

        injection_chirp(ctx, op, mGain / 1000.0, mFreqMin / 1000.0, mFreqMax / 1000.0, mPeriod / 1000.0);

        return CMD_SUCCESS;
    }

    // Handle 'inj triangle ...' command
    if (argc == 7 && strcmp("triangle", argv[1]) == 0) {
        // Parse out name and convert to injection context
        inj_ctx_t *ctx = injection_find_ctx_by_name(argv[2]);
        if (ctx == NULL)
            return CMD_INVALID_ARGUMENTS;

        // Parse out operation
        inj_op_e op;
        if (_parse_op(argv[3], &op) != 0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mValueMin argument
        // and saturate to -100 .. 100
        double mValueMin = (double) atoi(argv[4]);
        if (mValueMin < -100000.0)
            return CMD_INVALID_ARGUMENTS;
        if (mValueMin > 100000.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mValueMax argument
        // and saturate to -100 .. 100
        double mValueMax = (double) atoi(argv[5]);
        if (mValueMax < -100000.0)
            return CMD_INVALID_ARGUMENTS;
        if (mValueMax > 100000.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mPeriod argument
        // and saturate to 1 .. 60 sec
        double mPeriod = (double) atoi(argv[6]);
        if (mPeriod < 1000.0)
            return CMD_INVALID_ARGUMENTS;
        if (mPeriod > 60000.0)
            return CMD_INVALID_ARGUMENTS;

        injection_triangle(ctx, op, mValueMin / 1000.0, mValueMax / 1000.0, mPeriod / 1000.0);

        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // USER_CONFIG_ENABLE_INJECTION
