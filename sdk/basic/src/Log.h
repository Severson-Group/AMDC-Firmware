
#ifndef __LOG_H
#define __LOG_H

#define MAX_LOG_VARS 14
#define LOG_LENGTH (100000 + 1) //+1 is for empty position of circular buffer

typedef enum {L_IDLE, L_READY, L_LOGGING, L_DONE} LogStatus_t;

//FIFO settings:
#define FIFO_PREVENT_OVERRUN 1 //set this to prevent the log from overwriting old data (error will be flagged)
#define FIFO_RUN_ONCE 2		//set this to let the log fill up once and then stop

// FIFO flags:
#define FIFO_OVERRUN_OCCURED 1

typedef struct log_entry {
	u32 LogEntry[LOG_LENGTH];
	u32 InPos;
	u32 OutPos;
	u32 Settings;
	u32 Flags;
	u32 *VarToLog;
	LogStatus_t LogStatus;
} log_entry;


extern int InitLog();
extern int ReadFromLog(unsigned char *output, u32 LogIndex, u32 Length);
extern int DetermineMostFullLog(u32 *LogIndex);
extern int ReadFromMostFullLog(unsigned char *output, u32 *LogIndex, u32 Length);
extern int ReadLogFlags(u32 *flags, u32 LogIndex);
extern u32 LogDataAvail(u32 LogIndex);
extern int SetupLogVariable(u32 LogIndex, u32 *Variable, u32 Settings);
extern int StartLogging();
extern int StopAllLogging();
extern int DoLogging();

#endif //__LOG_H

