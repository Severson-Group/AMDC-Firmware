
# Getting Started

Following these instructions will get the AMDC firmware environment up and running on your local machine for development and testing purposes.


## Required Software

Firmware development environment needs a few things:

- Xilinx Vivado (tested on 2017.2)
- Xilinx SDK (comes with Vivado)
- `em.avnet.com:picozed_7030_fmc2:part0:1.1` board definition from online


## Cloning from GitHub

You must download the `AMDC-Firmware` git repo to your local machine. Make sure to put it in a permanent location (i.e., not `Downloads`), and ensure the path doesn't contain any spaces.

NOTE: `$REPO_DIR` represents the file system path of this repository.


## Vivado

Vivado is used to configure the Zynq-7000 SoC (clocks, pins, etc). All FPGA development happens within Vivado. All users must set up a Vivado project and build a FPGA bitstream.

If you are not doing anything specialized that would require changes to the FPGA, after following these steps, you do not need to use Vivado again. All your future development will happen within the SDK.

### Creating Vivado Project

Only source files and IP is version controlled which mean you must generate a local Vivado project. To do this:

1. Open Vivado Application
2. `Tools` > `Run Tcl Script...`
3. Select `$REPO_DIR\import.tcl` script
4. `OK`

Upon successful project generation, the block diagram will open. If the block diagram does not open, fix the errors and try reimporting project.

### Generating Bitstream

After generating the project itself, you need to generate a bitstream to load into the FPGA.

1. In Vivado...
2. `PROGRAM AND DEBUG` > `Generate Bitstream`
3. Click through the pop-ups until it starts actually working
4. If there are `Launch Run Critical Messages` about `PCW...`, ignore them and click OK

This step will take a while (~10 minutes). Upon successful generation, the bitstream is ready to load onto AMDC. This happens in the SDK section of this document.

### Export Hardware

You now need to export the hardware from Vivado to the SDK environment.

1. `File` > `Export` > `Export Hardware...`
2. Make sure to uncheck `Include bitstream`
3. Set location to export to: `$REPO_DIR\sdk`
4. `OK`

You may now close Vivado if you do not plan on changing the FPGA HDL.


## Xilinx SDK

Xilinx SDK (referred to as just SDK) is used to program the DSPs on the Zynq-7000 (i.e., C firmware). You will use the SDK to write your code and compile it. Then, you will use it to program the AMDC with your new code and debug issues. Finally, you can use the SDK to flash the AMDC after code development is complete with a permanent image (i.e., will automatically boot when powered on).

### Open SDK

1. Open Xilinx SDK 2017.2
2. Set workspace to: `$REPO_DIR\sdk`
3. Once open, close the Welcome tab

### Import Projects into SDK

The SDK workspace will initially be empty. You need to import the projects from the `sdk` directory.

1. `File` > `Open Projects from File System...`
2. `Directory...`
3. Select: `$REPO_DIR\sdk`
4. Ensure all projects are selected
5. `Finish`

### Build SDK Projects

SDK will attempt to build the projects you just imported. There will be lots of errors. Fix them as follows.

1. Right-click on the BSP project, e.g. `amdc_bsp`
2. `Re-generate BSP Sources`
3. `Yes`

SDK will now successfully compile the new BSP sources, then compile the rest of the projects successfully.

All done! Ready to program AMDC!


## Programming AMDC

Ensure the AMDC JTAG is plugged into your PC and AMDC main power is supplied.

### Setup SDK Project Run Configuration

1. Right-click on the project to are trying to run, e.g. `bare`
2. `Run As` > `Run Configurations...`
3. Ensure the `Target Setup` tab is open
4. Select `Browse...` for `Bitstream File`
5. Find the bitstream which Vivado generated (should be at `$REPO_DIR\amdc\amdc.runs\impl_1\design_1_wrapper.bit`) and click `Open`
6. Check the following boxes: `Reset entire system`, `Program FPGA`, `Run ps7_init`, `Run ps7_post_config`
7. Click `Apply`
8. Click `Close`

### Running Project on AMDC

Now, you are ready to start the code on AMDC!

1. Right-click on application, e.g. `bare`
2. `Run As` > `Launch on Hardware (System Debugger)`
3. `SDK Log` panel in the GUI will show stream of message as AMDC is programmed
    1. System reset will occur
    2. FPGA will be programmed
    3. Processor will start running your code


## Issues

Getting AMDC is start and run FPGA and C code can be hard. If it isn't working, try repeating the programming steps. Make sure to reset the board by either power cycling AMDC or pushing `RESET` button on AMDC.

Xilinx tools also have **many** quirks. Good luck getting everything working!
