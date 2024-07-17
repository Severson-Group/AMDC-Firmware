#ifdef APP_GAME
#include "cmd_game.h"
#include "usr/game/task_game.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry1;
static command_entry_t cmd_entry2;
static command_entry_t cmd_entry3;
static command_entry_t cmd_entry4;
static command_entry_t cmd_entryRestart;

static command_help_t cmd_help1[] = {
    { "1", "Send when LED 1 is on" }
};

static command_help_t cmd_help2[] = {
    { "2", "Send when LED 2 is on" }
};

static command_help_t cmd_help3[] = {
    { "3", "Send when LED 3 is on" }
};

static command_help_t cmd_help4[] = {
    { "4", "Send when LED 4 is on" }
};

static command_help_t cmd_helpRestart[] = {
    { "r", "Send to restart the game" }
};

void cmd_game_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry1, "1", "Game 1 command", cmd_help1, ARRAY_SIZE(cmd_help1), cmd_game1);
    commands_cmd_init(&cmd_entry2, "2", "Game 2 command", cmd_help2, ARRAY_SIZE(cmd_help2), cmd_game2);
    commands_cmd_init(&cmd_entry3, "3", "Game 3 command", cmd_help3, ARRAY_SIZE(cmd_help3), cmd_game3);
    commands_cmd_init(&cmd_entry4, "4", "Game 4 command", cmd_help4, ARRAY_SIZE(cmd_help4), cmd_game4);
    commands_cmd_init(&cmd_entryRestart, "r", "Restart command", cmd_helpRestart, ARRAY_SIZE(cmd_helpRestart), cmd_gameRestart);

    // Register the command
    commands_cmd_register(&cmd_entry1);
    commands_cmd_register(&cmd_entry2);
    commands_cmd_register(&cmd_entry3);
    commands_cmd_register(&cmd_entry4);
    commands_cmd_register(&cmd_entryRestart);
}

int cmd_game1(int argc, char **argv) {
    if (getGameLedOn() == 1) {
        while (getGameLedOn() == 1) {
            setGameLedOn(randomInt(1, 4));
        }
        incrementGameScore();
        return CMD_SUCCESS;
    }
    decrementGameScore();
    return CMD_FAILURE;
}

int cmd_game2(int argc, char **argv) {
    if (getGameLedOn() == 2) {
        while (getGameLedOn() == 2) {
            setGameLedOn(randomInt(1, 4));
        }
        incrementGameScore();
        return CMD_SUCCESS;
    }
    decrementGameScore();
    return CMD_FAILURE;
}

int cmd_game3(int argc, char **argv) {
    if (getGameLedOn() == 3) {
        while (getGameLedOn() == 3) {
            setGameLedOn(randomInt(1, 4));
        }
        incrementGameScore();
        return CMD_SUCCESS;
    }
    decrementGameScore();
    return CMD_FAILURE;
}

int cmd_game4(int argc, char **argv) {
    if (getGameLedOn() == 4) {
        while (getGameLedOn() == 4) {
            setGameLedOn(randomInt(1, 4));
        }
        incrementGameScore();
        return CMD_SUCCESS;
    }
    decrementGameScore();
    return CMD_FAILURE;
}

int cmd_gameRestart(int argc, char **argv) {
	reset_game();
	return CMD_SUCCESS;
}
#endif // APP_GAME
