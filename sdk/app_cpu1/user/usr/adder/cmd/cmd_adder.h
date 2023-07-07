#ifndef CMD_ADDER_H
#define CMD_ADDER_H

// Called in app init function to register command with system
void cmd_adder_register(void);

// Function called when user types "adder" command into command prompt
// i.e., this is the blink command handler function
int cmd_adder(int argc, char **argv);

#endif // CMD_ADDER_H
