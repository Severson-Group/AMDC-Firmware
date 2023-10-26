# How to use FreeRTOS apps on AMDC

Follow the existing AMDC Tutorial on Building and Running Firmware located [here](https://docs.amdc.dev/firmware/xilinx-tools/building-and-running-firmware.html).

The following changes are required from the tutorial:

### FreeRTOS BSPs

After getting the hw\_platform project into the SDK, you will need to create a Board Support Package (BSP) for each of the Zynq-7000 cores.

1. Go to File  > New > Board Support Package
2. In the pop-up window, name the BSP project "amdc\_freertos\_bsp\_cpu0"
3. At the bottom of the pop-up window, select "freertos10\_xilinx" for the Board Support Package OS
4. Click Finish
5. Just as with the standard firmware process, check both "lwip\*\*\*" and "xilffs"

For the cpu1 BSP, change the name and add `-DUSE\_AMP=1`, just as described in the standard firmware process
 
