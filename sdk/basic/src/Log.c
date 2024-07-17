/*
 * Log.c
 *
 *  Created on: Jun 6, 2014
 *      Author: sever212
 */


#include "xparameters.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "main.h"
#include "project_settings.h"
#include "Log.h"




//log_entry TheLog[MAX_LOG_VARS];
log_entry TheLog[MAX_LOG_VARS];


static inline int ProtectLog(u32 LogIndex)
{
	return 1; //Protection not needed since we aren't allowing overruns EnterProtection();
}

static inline int UnprotectLog(u32 LogIndex, int State)
{
	return 1; //protection not needed since we aren't allowing overruns LeaveProtection(State);
}
/*****************************
 * InitLog()
 * Call this to initialize the log.
 *
 */
int InitLog()
{
	int i;
	for (i = 0; i < MAX_LOG_VARS; i++)
	{
		int protect = ProtectLog(i);
		TheLog[i].InPos = 0;
		TheLog[i].OutPos = 0;
		TheLog[i].Settings = 0;
		TheLog[i].Flags = 0;
		TheLog[i].VarToLog = 0;
		TheLog[i].LogStatus = L_IDLE;
		UnprotectLog(i, protect);
	}

	return SUCCESS;
}

/*****************************
 * ReadFromLog()
 * Call this to read data from the log
 *
 * output: where the data is copied to
 * LogIndex: which log to read from (0 to MAX_LOG_VARS-1)
 * Index: zero-based index in the log to start reading from
 * Length: The maximum length that can be written to output (in bytes)
 * ---NOTE: since everything is 32 bit aligned, length must be
 * 	divisible by 4
 *
 * Returns number of bytes read (if positive)
 * Error code (if negative)
 */
int ReadFromLog(unsigned char *output, u32 LogIndex, u32 Length)
{
	u32 avail, in, out;
	int protect;
	if (LogIndex >= MAX_LOG_VARS)
		return INVALID_ARGUMENT;
	if ( (TheLog[LogIndex].LogStatus != L_DONE) && (TheLog[LogIndex].LogStatus != L_LOGGING) )
		return INVALID_OPERATION;
	if (Length % 4 != 0)
		return INVALID_ARGUMENT; //must by 32bit aligned

	protect = ProtectLog(LogIndex);
	//set up internal variables and adjust length
	avail = LogDataAvail(LogIndex); //in u32 (for bytes multiply by 4)
	in = TheLog[LogIndex].InPos;
	out = TheLog[LogIndex].OutPos;
	if (Length > avail*4)
		Length = avail*4;

	//Okay, now read it out
	if (out + Length/4 < LOG_LENGTH)
	{
		memcpy(output, &TheLog[LogIndex].LogEntry[out], Length);
		TheLog[LogIndex].OutPos += Length/4;
	}
	else if (out > in)
	{
		u32 lenTop = 4*(LOG_LENGTH - out); //number of bytes
		u32 lenBot = Length - lenTop; //number of bytes
		memcpy(output, &TheLog[LogIndex].LogEntry[out], lenTop);
		memcpy(&output[lenTop], TheLog[LogIndex].LogEntry, lenBot);
		TheLog[LogIndex].OutPos = lenBot/4;
	}

	UnprotectLog(LogIndex, protect);
	return Length;
}

/*****************************
 * DetermineMostFullLog()
 * Call this to determine which log has the most data and find out how many
 * bytes can be read.
 *
 * LogIndex: set to the log that has the most data (0 to MAX_LOG_VARS-1)
 *
 * Returns number of bytes available (if positive)
 * Error code (if negative)
 */
int DetermineMostFullLog(u32 *LogIndex)
{
	int i;
	u32 max_avail = 0, cur_avail = 0;
	for (i = 0; i < MAX_LOG_VARS; i++)
	{
		cur_avail = LogDataAvail(i);
		if (cur_avail > max_avail)
		{
			max_avail = cur_avail;
			*LogIndex = i;
		}
	}
	return max_avail;
}

/*****************************
 * ReadFromMostFullLog()
 * Call this to read data from the log that has the most data ready
 * to be read out.
 *
 * output: where the data is copied to
 * LogIndex: set to the log that was read from (0 to MAX_LOG_VARS-1)
 * Length: The maximum length that can be written to output (in bytes)
 * ---NOTE: since everything is 32 bit aligned, length must be
 * 	divisible by 4
 *
 * Returns number of bytes read (if positive)
 * Error code (if negative)
 */
int ReadFromMostFullLog(unsigned char *output, u32 *LogIndex, u32 Length)
{
	int i;
	u32 max_entry = 0, max_avail = 0, cur_avail = 0;
	for (i = 0; i < MAX_LOG_VARS; i++)
	{
		cur_avail = LogDataAvail(i);
		if (cur_avail > max_avail)
		{
			max_avail = cur_avail;
			max_entry = i;
			*LogIndex = i;
		}
	}
	if (max_avail > 0)
		return ReadFromLog(output, max_entry, Length);
	else
		return 0;
}

/*****************************
 * ReadLogFlags()
 * Call this to read a log's flags. Doing so
 * clears the flags
 *
 * *flags: pointer to where the flags should be written to
 * LogIndex: which log to read from (0 to MAX_LOG_VARS-1)
 *
 * Returns number of bytes read (if positive)
 * Error code (if negative)
 */
int ReadLogFlags(u32 *flags, u32 LogIndex)
{

	if (LogIndex >= MAX_LOG_VARS)
		return INVALID_ARGUMENT;
	*flags = TheLog[LogIndex].Flags;
	TheLog[LogIndex].Flags = 0;
	return SUCCESS;
}

/*****************************
 * LogDataAvail()
 * Determine how many u32 values can be read from the log
 * (the number of bytes is 4 * this)
 *
 * LogIndex: Which log the caller is inquiring about
 * Returns: number of u32's available
 */
u32 inline LogDataAvail(u32 LogIndex)
{
	u32 in, out;
	int protect;

	if (LogIndex >= MAX_LOG_VARS)
			return 0;
	protect = ProtectLog(LogIndex);
	in = TheLog[LogIndex].InPos;
	out = TheLog[LogIndex].OutPos;
	UnprotectLog(LogIndex, protect);
	if (in >= out)
		return in - out;
	else
		return LOG_LENGTH - (out - in);
}

/*****************************
 * LogInputSpaceAvail()
 * Determine how many u32 values can be stored in the log
 * (the number of bytes is 4 * this)
 *
 * LogIndex: Which log the caller is inquiring about
 * Returns: number of u32's available
 */
static inline u32 LogInputSpaceAvail(u32 LogIndex)
{
	u32 in, out;
	if (LogIndex >= MAX_LOG_VARS)
			return 0;
	in = TheLog[LogIndex].InPos;
	out = TheLog[LogIndex].OutPos;

	if (out > in)
		return (out - in) - 1;
	else
		return (LOG_LENGTH - (in - out)) - 1;
}

/*****************************
 * SetupLogVariable()
 * Call this prepare a variable for logging
 *
 * LogIndex: Which index in the log to use (0 to MAX_LOG_VARS-1)
 * Variable: Pointer to the variable to log
 * Returns: Success code
 */
int SetupLogVariable(u32 LogIndex, u32 *Variable, u32 Settings)
{
	if (LogIndex >= MAX_LOG_VARS)
		return INVALID_ARGUMENT;

	else
	{
		TheLog[LogIndex].InPos = 0;
		TheLog[LogIndex].OutPos = 0;
		TheLog[LogIndex].Settings = Settings;
		TheLog[LogIndex].Flags = 0;
		TheLog[LogIndex].VarToLog = Variable;
		TheLog[LogIndex].LogStatus = L_READY;
	}
	return SUCCESS;
}

/*****************************
 * StartLogging()
 * Call this to start the data logging
 *
 * Returns: Success code
 */
int StartLogging()
{
	int i;
	for (i = 0; i < MAX_LOG_VARS; i++)
	{
		if (TheLog[i].LogStatus == L_READY)
			TheLog[i].LogStatus = L_LOGGING;
	}

	return SUCCESS;
}

/*****************************
 * StopAllLogging()
 * Call this to stop all data logging. Anything
 * in progress will be marked as done.
 *
 * Returns: Success code
 */
int StopAllLogging()
{
	int i;
	for (i = 0; i < MAX_LOG_VARS; i++)
	{
		if (TheLog[i].LogStatus == L_LOGGING)
			TheLog[i].LogStatus = L_DONE;
	}

	return SUCCESS;
}

/*****************************
 * DoLogging()
 * Call this at fixed intervals to handle logging of variables (move to DMA??)
 *
 * Returns: Success code
 */
int DoLogging()
{
	int i;
	for (i = 0; i < MAX_LOG_VARS; i++)
	{
		if (TheLog[i].LogStatus == L_LOGGING)
		{
			//basic case: we have space:
			if ( LogInputSpaceAvail(i) )
				TheLog[i].LogEntry[TheLog[i].InPos++] = *TheLog[i].VarToLog;
			else if (TheLog[i].Settings & FIFO_RUN_ONCE) //just run once
				TheLog[i].LogStatus = L_DONE;
			else if ( !(TheLog[i].Settings & FIFO_PREVENT_OVERRUN) ) //just write over
			{
				//if we don't write over, than there is no need to protect the fifo!
				TheLog[i].Flags |= FIFO_OVERRUN_OCCURED;
				/*
				TheLog[i].OutPos++;
				TheLog[i].OutPos %= LOG_LENGTH;
				TheLog[i].LogEntry[TheLog[i].InPos++] = *TheLog[i].VarToLog;*/
			}

			/*if (TheLog[i].Flags != 0)
				TheLog[i].Flags = TheLog[i].Flags;
			if (TheLog[i].InPos >= LOG_LENGTH)
				TheLog[i].InPos = TheLog[i].InPos;*/

			//adjust indices:
			TheLog[i].InPos %= LOG_LENGTH;
		}
	}

	return SUCCESS;
}
