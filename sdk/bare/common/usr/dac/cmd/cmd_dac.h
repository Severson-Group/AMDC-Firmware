#ifndef CMD_DAC_H
#define CMD_DAC_H

// Called in app init function to register command with system
void cmd_dac_register(void);

// Function called when user types "blink" command into command prompt
// i.e., this is the blink command handler function
int cmd_dac(int argc, char **argv);

#endif // CMD_BLINK_H
