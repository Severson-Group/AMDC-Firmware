#ifndef CMD_ENCODER_H
#define CMD_ENCODER_H

// Called in app init function to register command with system
void cmd_encoder_register(void);

// Function called when user types "blink" command into command prompt
// i.e., this is the blink command handler function
int cmd_encoder(int argc, char **argv);

#endif // CMD_ENCODER_H
