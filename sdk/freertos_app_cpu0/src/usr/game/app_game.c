#ifdef APP_GAME

#include "usr/game/app_game.h"
#include "usr/game/cmd/cmd_game.h"
#include "usr/game/task_game.h"

void app_game_init(void)
{
    // Register "game" command with system
    cmd_game_register();

    // Initialize game task with system
    task_game_init();
}

#endif // APP_GAME
