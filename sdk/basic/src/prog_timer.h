
#ifndef __PROG_TIMER_H
#define __PROG_TIMER_H
#include "xscutimer.h"
#include "xscugic.h"

#define TIMER_DEVICE_ID		XPAR_XSCUTIMER_0_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR
#define TIMER_INTERVAL		10 //value in us
#define TIMER_LOAD_VALUE	(TIMER_INTERVAL*666.66666667)/2 //0xFFFF // = timerInterval*ClockFreq/2


extern volatile u32 MainTimerTimerTick;
extern volatile char bTimer100usTick;
extern volatile  u32 Timing10usErrorCount;


extern int SetupTimer(XScuGic *IntcInstancePtr, XScuTimer * TimerInstancePtr,
		u16 TimerDeviceId, u16 TimerIntrId);
extern u32 TimeElapsed10us(u32 old);
extern void TimerDisableIntrSystem(XScuGic *IntcInstancePtr, u16 TimerIntrId);
#endif //__PROG_TIMER_H
