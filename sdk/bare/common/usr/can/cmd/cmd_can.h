#ifndef CMD_CAN_H
#define CMD_CAN_H

// Called in app init function to register command with system
void cmd_can_register(void);

// Function called when user types "can" command into command prompt
int cmd_can(int argc, char **argv);

#endif // CMD_CAN_H
