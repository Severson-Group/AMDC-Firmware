#ifndef __PECB_IO_H
#define __PECB_IO_H

//custom PL stuff
#define ER_CNT 31
#define CONTROL 31
#define WR_REG0 0
#define RD_REG0 0



//extern void WriteDutyFraction(u32 Leg, float frac);
extern void WriteConfigRegister(u8 value);
extern void WriteDutyRatio(u32 Leg, unsigned char value);
extern double ReadADC(u32 uiADCnum);
extern int InitPECB_IO();
extern int FinishInitPECB_IO();
extern void WriteToZB_LEDs(u32 LED_value);
#endif
