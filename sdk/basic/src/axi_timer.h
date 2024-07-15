#ifndef __AXI_TIMER_H
#define __AXI_TIMER_H
extern volatile u32 ControlTimer0_TickCount;
extern volatile u32 ControlTimer1_TickCount;
#include "xparameters.h"



extern int Init_AXI_Timer(XScuGic* IntcInstancePtr, XTmrCtr* TmrCtrInstancePtr,
		u16 DeviceId, u16 IntrId, u8 TmrCtrNumber,
		XTmrCtr_Handler InterruptHandler, u8 IntrPriority, u32 ResetValue);
#endif //__AXI_TIMER_H
