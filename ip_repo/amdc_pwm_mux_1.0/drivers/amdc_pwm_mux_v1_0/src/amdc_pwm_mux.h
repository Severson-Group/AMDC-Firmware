
#ifndef AMDC_PWM_MUX_H
#define AMDC_PWM_MUX_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"

#define AMDC_PWM_MUX_S00_AXI_SLV_REG0_OFFSET 0
#define AMDC_PWM_MUX_S00_AXI_SLV_REG1_OFFSET 4
#define AMDC_PWM_MUX_S00_AXI_SLV_REG2_OFFSET 8
#define AMDC_PWM_MUX_S00_AXI_SLV_REG3_OFFSET 12
#define AMDC_PWM_MUX_S00_AXI_SLV_REG4_OFFSET 16
#define AMDC_PWM_MUX_S00_AXI_SLV_REG5_OFFSET 20
#define AMDC_PWM_MUX_S00_AXI_SLV_REG6_OFFSET 24
#define AMDC_PWM_MUX_S00_AXI_SLV_REG7_OFFSET 28
#define AMDC_PWM_MUX_S00_AXI_SLV_REG8_OFFSET 32
#define AMDC_PWM_MUX_S00_AXI_SLV_REG9_OFFSET 36
#define AMDC_PWM_MUX_S00_AXI_SLV_REG10_OFFSET 40
#define AMDC_PWM_MUX_S00_AXI_SLV_REG11_OFFSET 44
#define AMDC_PWM_MUX_S00_AXI_SLV_REG12_OFFSET 48
#define AMDC_PWM_MUX_S00_AXI_SLV_REG13_OFFSET 52
#define AMDC_PWM_MUX_S00_AXI_SLV_REG14_OFFSET 56
#define AMDC_PWM_MUX_S00_AXI_SLV_REG15_OFFSET 60
#define AMDC_PWM_MUX_S00_AXI_SLV_REG16_OFFSET 64
#define AMDC_PWM_MUX_S00_AXI_SLV_REG17_OFFSET 68
#define AMDC_PWM_MUX_S00_AXI_SLV_REG18_OFFSET 72
#define AMDC_PWM_MUX_S00_AXI_SLV_REG19_OFFSET 76
#define AMDC_PWM_MUX_S00_AXI_SLV_REG20_OFFSET 80
#define AMDC_PWM_MUX_S00_AXI_SLV_REG21_OFFSET 84
#define AMDC_PWM_MUX_S00_AXI_SLV_REG22_OFFSET 88
#define AMDC_PWM_MUX_S00_AXI_SLV_REG23_OFFSET 92
#define AMDC_PWM_MUX_S00_AXI_SLV_REG24_OFFSET 96
#define AMDC_PWM_MUX_S00_AXI_SLV_REG25_OFFSET 100
#define AMDC_PWM_MUX_S00_AXI_SLV_REG26_OFFSET 104
#define AMDC_PWM_MUX_S00_AXI_SLV_REG27_OFFSET 108
#define AMDC_PWM_MUX_S00_AXI_SLV_REG28_OFFSET 112
#define AMDC_PWM_MUX_S00_AXI_SLV_REG29_OFFSET 116
#define AMDC_PWM_MUX_S00_AXI_SLV_REG30_OFFSET 120
#define AMDC_PWM_MUX_S00_AXI_SLV_REG31_OFFSET 124
#define AMDC_PWM_MUX_S00_AXI_SLV_REG32_OFFSET 128
#define AMDC_PWM_MUX_S00_AXI_SLV_REG33_OFFSET 132
#define AMDC_PWM_MUX_S00_AXI_SLV_REG34_OFFSET 136
#define AMDC_PWM_MUX_S00_AXI_SLV_REG35_OFFSET 140
#define AMDC_PWM_MUX_S00_AXI_SLV_REG36_OFFSET 144
#define AMDC_PWM_MUX_S00_AXI_SLV_REG37_OFFSET 148
#define AMDC_PWM_MUX_S00_AXI_SLV_REG38_OFFSET 152
#define AMDC_PWM_MUX_S00_AXI_SLV_REG39_OFFSET 156
#define AMDC_PWM_MUX_S00_AXI_SLV_REG40_OFFSET 160
#define AMDC_PWM_MUX_S00_AXI_SLV_REG41_OFFSET 164
#define AMDC_PWM_MUX_S00_AXI_SLV_REG42_OFFSET 168
#define AMDC_PWM_MUX_S00_AXI_SLV_REG43_OFFSET 172
#define AMDC_PWM_MUX_S00_AXI_SLV_REG44_OFFSET 176
#define AMDC_PWM_MUX_S00_AXI_SLV_REG45_OFFSET 180
#define AMDC_PWM_MUX_S00_AXI_SLV_REG46_OFFSET 184
#define AMDC_PWM_MUX_S00_AXI_SLV_REG47_OFFSET 188
#define AMDC_PWM_MUX_S00_AXI_SLV_REG48_OFFSET 192
#define AMDC_PWM_MUX_S00_AXI_SLV_REG49_OFFSET 196


/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a AMDC_PWM_MUX register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the AMDC_PWM_MUXdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void AMDC_PWM_MUX_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define AMDC_PWM_MUX_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a AMDC_PWM_MUX register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the AMDC_PWM_MUX device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 AMDC_PWM_MUX_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define AMDC_PWM_MUX_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

/************************** Function Prototypes ****************************/
/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the AMDC_PWM_MUX instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus AMDC_PWM_MUX_Reg_SelfTest(void * baseaddr_p);

#endif // AMDC_PWM_MUX_H
