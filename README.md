# AMDC-Firmware

Advanced Motor Drive Controller (AMDC), is an open-source project from the Severson Research Group at UW-Madison, affiliated with Wisconsin Electric Machines and Power Electrics Consortium (WEMPEC).

AMDC design includes both hardware and firmware. This repository contains the low-level firmware which runs AMDC, as well as example applications.

## Getting Started

Following these instructions will get the AMDC firmware environment up and running on your local machine for development and testing purposes.

NOTE: `$REPO_DIR` represents the file system path of this repository.

### Required Software

Firmware development environment needs a few things:

- Vivado (tested on 2017.2)
- `em.avnet.com:picozed_7030_fmc2:part0:1.1` board definition from online

### Creating Vivado Project

Only source files and IP is version controlled which mean you must generate a local Vivado project. To do this, follow the following steps.

1. Open Vivado Application
2. `Tools` > `Run Tcl Script...`
3. Select `$REPO_DIR\import.tcl` script
4. `OK`

Upon successful project generation, the block diagram will open.

### Generating Bitstream

After generating the project itself, you need to generate a bitstream to load into the FPGA.

1. `PROGRAM AND DEBUG` > `Generate Bitstream`
2. Click through the pop-ups until it starts actually working
3. If there are `Launch Run Critical Messages` about `PCW...`, ignore them and click OK

This step will take a while (~10 minutes). Upon successful generation, the bitstream is ready to load onto AMDC!

### Export Hardware

You now need to export the hardware from Vivado to the SDK environment.

1. `File` > `Export` > `Export Hardware...`
2. Make sure to uncheck `Include bitstream`
3. Set location to export to: `$REPO_DIR\sdk`
4. `OK`

### Open SDK

Now, you are ready to open the SDK application and initialize the projects.

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

SDK will attempt to build the projects you just imported. There will be lots of errors. Fix them.

1. Right-click on the BSP project, e.g. `helloworld_bsp`
2. `Re-generate BSP Sources`
3. `Yes`

SDK will now successfully compile the new BSP sources, then compile the rest of the projects successfully.

All done! Ready to program AMDC!


### Programming AMDC

Ensure the AMDC JTAG is plugged into your PC and AMDC main power is supplied.

AMDC programming takes two phases: loading the FPGA bitstream, and programming the processor.

#### Loading FPGA Bitstream

Back in Vivado:

1. `PROGRAM AND DEBUG` > `Open Hardware Manager`
2. `Open target`
3. `Auto Connect`
4. `Program device`
5. `Program`

If there is an error, try to program the device again. It should work the second time.

The FPGA is now running configured with the bitstream and running!

#### Starting Processor

Back in SDK:

1. Right-click on application
2. `Run As` > `Launch on Hardware (System Debugger)`
3. Open debug perspective in SDK
4. Click `Resume` (F8) to start program


#### Issues

Getting AMDC is start and run FPGA and C code can be hard. If it isn't working, try repeating the programming steps. Make sure to reset the board by either power cycling AMDC or pushing `RESET` button on AMDC.

Xilinx tools also have **many** bugs/quirks. Good luck getting everything working!


## Authors

Initial firmware design work by Nathan Petersen during 2018-2019.

## License

This project is licensed under the ????? License - see the [LICENSE.md](LICENSE.md) file for details.
