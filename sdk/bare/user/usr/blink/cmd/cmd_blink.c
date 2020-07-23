#ifdef APP_BLINK

#include "usr/blink/cmd/cmd_blink.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "usr/blink/task_blink.h"
#include <stdlib.h>
#include <string.h>

// Stores command entry for command system module
static command_entry_t cmd_entry;

// Defines help content displayed for this command
// when user types "help" at command prompt
static command_help_t cmd_help[] = {
    { "hello <name>", "Print hello to screen" },
    { "init", "Start task" },
    { "deinit", "Stop task" },
    { "stats print", "Print stats" },
    { "stats reset", "Reset stats" },
};

void cmd_blink_register(void)
{
    // Populate the command entry block
    //
    // Here is where you define the base command string: "blink"
    // and what function is called to handle command
    commands_cmd_init(&cmd_entry, "blink", "Blink application commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_blink);

    // Register the command with the system
    commands_cmd_register(&cmd_entry);
}

int cmd_blink(int argc, char **argv)
{
    // This function is called every time the user types "blink"
    // followed by a space and any number of characters.
    //
    // Example user input: blink foo 123
    //
    // It is up to the application developer to handle this
    // incoming command in a manner that reflects this command's
    // help message, as to not confuse the user.
    //
    // The arguments passed into this function (argc & argv)
    // follow standard C convention for main() programs called
    // via command line interface (CLI).
    //
    // 'argc' is the number of CLI arguments, including the base command
    // For above example: argc = 3
    //
    // 'argv' is an array of char strings which contain the CLI arguments
    // For above example:
    // - argv[0] => "blink"
    // - argv[1] => "foo"
    // - argv[2] => "123"
    //
    // NOTE: The system constrains user CLI input to ensure responsive
    // behavior for arbitrary commands. This involves limiting individual
    // command argument length as well as the total number of arguments
    // (this is defined in sys/commands.c)

    // Handle 'hello' sub-command
    //
    // First, verify correct number of arguments (2)
    // Second, verify second argument is "hello"
    if (argc == 3 && strcmp("hello", argv[1]) == 0) {
        // Perform desired action for command...

        // Fun example: hello message based on name
        if (strcmp("nathan", argv[2]) == 0) {
            // Wow, happy to talk to Nathan!
            debug_printf("Welcome, sir Nathan!!!\r\n");
            debug_print("\r\n");

            // Indicate success, but hide SUCCESS message
            return CMD_SUCCESS_QUIET;

        } else if (strcmp("fred", argv[2]) == 0) {
            // We don't want to talk to Fred... :(
            // Treat this case as an invalid command input from user
            return CMD_INVALID_ARGUMENTS;

        } else {
            // Normal hello for anyone else
            debug_printf("Hello, %s\r\n", argv[2]);
            debug_print("\r\n");

            // Indicate success, but hide SUCCESS message
            return CMD_SUCCESS_QUIET;
        }

        // If user typed a valid command, return SUCCESS
        return CMD_SUCCESS;
    }

    if (argc == 2 && strcmp("init", argv[1]) == 0) {
        if (task_blink_init() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    if (argc == 2 && strcmp("deinit", argv[1]) == 0) {
        if (task_blink_deinit() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    if (argc >= 2 && strcmp("stats", argv[1]) == 0) {
        if (argc == 3 && strcmp("print", argv[2]) == 0) {
            task_blink_stats_print();
            return CMD_SUCCESS;
        }

        if (argc == 3 && strcmp("reset", argv[2]) == 0) {
            task_blink_stats_reset();
            return CMD_SUCCESS;
        }
    }

    // At any point, if an error is detected in given input command,
    // simply return an error code (defined in sys/defines.h)
    //
    // The return statement below is used to catch all user input which
    // didn't match the if statements above. In general, to handle commands,
    // assume they are invalid. Only after checking if each argument is
    // valid should you trust them.
    //
    // Common error return values are: FAILURE, INVALID_ARGUMENTS
    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_BLINK
