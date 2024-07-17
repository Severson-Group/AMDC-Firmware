#ifndef TASK_GAME_H
#define TASK_GAME_H

#include "sys/scheduler.h"

// Frequency that this task is called (in Hz)
//
// Must be less than or equal to scheduler updates per second
// This value is defined in sys/scheduler.h and defaults to 10kHz.
// Note that it can be overridden via usr/user_config.h
#define TASK_GAME_UPDATES_PER_SEC (5)

// Microseconds interval between when task is called
//
// This is what scheduler actually uses to run task,
// but is generated via define above
#define TASK_GAME_INTERVAL_USEC (USEC_IN_SEC / TASK_GAME_UPDATES_PER_SEC)

void incrementGameScore();
void decrementGameScore();

uint8_t getGameLedOn();
void setGameLedOn(uint8_t setTo);
int randomInt(int lowerBound, int upperBound);

// Called in app init function to set up task (or via command)
int task_game_init(void);
int task_game_deinit(void);

// start the game timer
void start_game();
void reset_game();

// returns 1 if the game is already started and 0 otherwise
int game_started();

// Callback function which scheduler calls periodically
void task_game_callback(void *arg);

// Print the statistics gathered by the scheduler
void task_game_stats_print(void);

// Reset the statistics gathered by the scheduler
void task_game_stats_reset(void);

#endif // TASK_GAME_H
