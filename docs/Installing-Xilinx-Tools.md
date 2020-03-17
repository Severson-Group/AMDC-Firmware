# Installing Xilinx Tools

This document describes how to install the required Xilinx tools for building, compiling, debugging, and flashing the AMDC firmware. In order to use the AMDC, you will need to install the following programs:
1. Xilinx Vivado
2. Xilinx SDK

Xilinx regularly updates the Vivado suite of tools. They release main updates in the format of year and version numbering YYYY.V (i.e. 2017.2). These upgrades are sometimes backwards compatible, sometimes not. Vivado 2019.1 is the last version of the Xilinx tools which uses the SDK environment -- 2019.2 and onward use the new Xilinx Vitis environment. We will be using the SDK, so we will download and install Vivado 2019.1.

**Warning:** The Vivado suite of tools takes a lot of space on your PC! Please ensure you have ~35GB of local free space before proceeding! You will also need a fast internet connection to download the required files!

## Installing Vivado 2019.1

*This tutorial will assume you are installing Vivado locally on your personal **Windows** PC.*

Ensure you have no Xilinx tools previously installed.

### Downloading installation executable

1. Go to the [Xilinx Downloads page](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2019-1.html) for 2019.1.

2. Scroll down to the "Full Product Installation" section for 2019.1

3. It is strongly recommended to use the web installer (you will need to download ~8GB of data). Click on the link for: "Vivado HLx 2019.1: WebPACK and Editions - Windows Self Extracting Web Installer"

4. This will make you create a Xilinx account. Do this and continue.

5. Fill out the required "Name and Address Verification" page and click Download.

### Installing

6. Run the installer. Wait for it to extract itself... Once it runs, allow it to change your hard drive.

7. When the installer opens, it will prompt you to get the latest software version. Decline by clicking `Continue`.

8. Click `Next >`

9. Enter your Xilinx account login info you created previously and click `Next >`

10. Check all boxes to agree to terms and click `Next >`

11. Select "Vivado HL System Edition" and click `Next >`

12. *Recommended but optional to reduce disk space:* Under `Devices`, only select `SoCs` > `Zynq-7000`

13. Ensure Vivado and SDK are both selected (they are by default) and click `Next >`
