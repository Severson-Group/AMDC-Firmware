#ifndef __MAIN_H
#define __MAIN_H

extern volatile u32* PECB;
extern u32 Timing100usErrorCount;

extern int EnterProtection();
extern int LeaveProtection(int State);
#endif //__MAIN_H
