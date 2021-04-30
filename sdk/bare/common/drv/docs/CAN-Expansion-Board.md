# CAN Expansion Board Drivers

## Table of Contents
- [Purpose of Document](#purpose-of-document)
- [Using the CAN Board](#using-the-can-board)
- [What is Provided](#what-is-provided)
- [Hardware Discussion](#hardware-discussion)
- [Files](#files)
- [API Interface](#api-interface)
## Purpose of Document
This document highlights the architecture of the CAN firmware targetting the AMDC hardware board REV D such that it can interface with devices on a CAN bus using the CAN expansion board. Specifically, the document describes the drivers thar are used to control and configure the CAN peripherals baked into the Zync-7000 SoC.

## Using the CAN Board
In order to use the CAN board, you will need an `AMDC` board (targetted to `REVD`), a `CAN` board, and one GPIO Expansion cable (high density DB15 cable). Plug in the `CAN` board to the `AMDC` board via the high density cable.

## What is Provided
The following are provided for the development of the CAN firmware.
1. Two CAN peripherals baked into the silicon of the Zync-7000 SoC
2. CAN IP Block for Xilinx FPGA (although a license is required, more on this [below](#hardware-discussion))
3. Drivers to use these CAN peripherals

Given that the drivers are already provided, the API interface is simply a wrapper built on top of the given drivers to abstract the low-level register details from users of the CAN board.

## Hardware Discussion
Before discussing the actual firmware, let's take a closer look at the hardware provided. As mentioned above, there are two options: 
1. the CAN peripherals baked into the silicon of the Zync-7000
2. the CAN IP block for the FPGA provided by Xilinx. 

The latter option was the preferred option such that pins on the processor don't have to be used up by the CAN peripheral, however, after an initial attempt to generate a bitstream using the CAN IP blocks, a failure was reported because the Severson Group doesn't have a license in order to use the IP block. Thus, the first option was used instead.

One problem to get around is that the `GPIO` Expansion signals are electrically connected to the FPGA. Thus, we had to somehow use the CAN peripheral on the DSP, but send the signal through the FPGA to the correct pins. Luckily, there is a way to do this exaxt operation by specifying in Vivado that the pins of the CAN peripheral are EMIO pins such that the signal is sent through the AXI interface to the FPGA. By doing so, we can route `CAN_TX` and `CAN_RX` through the FPGA to the `GPIO` Expansion ports on the `AMDC`.

## Files
All files for the CAN drivers are in the driver directory ([`common/drv/`](github.com/Severson-Group/AMDC-Firmware/tree/develop/sdk/bare/common/drv)).
```
drv/
|-- can.c
|-- can.h
```

## API Interface
As mentioned, the firmware written is built on top of the provided CAN drivers with the goal of abstracting low-level details and creating an easy-to-use API interface. Note that we repeatedly state the phrase "the current CAN peripheral in use" below. As mentioned above, there are two CAN periphals baked into the silicon of the DSP: `CAN0` and `CAN1`. Users of the API can use either peripheral and set the current CAN peripheral in use via an API call. 

Below is a description of each API function...

### Configuring the CAN Peripherals
```
int can_setmode(can_mode_t mode);
```
Sets the mode of the current CAN peripheral in use, options include config, normal, sleep, loopback, and snoop. The parameter `mode` is of type `uint32_t` and can be any of the following found in the enum in `can.h`: 
- `CAN_CONFIG`
- `CAN_LOOPBACK`
- `CAN_NORMAL`
- `CAN_SLEEP`
- `CAN_SNOOP`

Note that if the current mode of the CAN peripheral is `CAN_LOOPBACK` and the user attempts to go to any other mode other than `CAN_CONFIG`, a `FAILURE` is returned. Similarly, if the CAN peripheral is in `CAN_NORMAL` and the user attempts to go to either `CAN_LOOPBACK` or `CAN_SNOOP`, a `FAILURE` is returned. These restrictions are specified in the Zync-7000 Reference Manual, Chapter 18.
```
int can_setbaud(int rate);
```
Sets the baud rate register of the current CAN peripheral in use to the user specified `rate`. To set the baud rate to the default values, simply use the macros defined in `can.h`. Note that the CAN periphal must be in `CAN_CONFIG` before attempting to change the baud rate register.

```
int can_set_btr(uint8_t sjw, uint8_t ts2, uint8_t ts1);
```
Sets the bit timing register bits of the current CAN peripheral in use to the user specified inputs. To set these bits to defaults, simply use the macros defined in `can.h`. Note that the CAN periphal must be in `CAN_CONFIG` before attempting to change the bit timing register.
```
int can_set_peripheral(int device_id);
```
Sets the current CAN peripheral in use, either `CAN0` or `CAN1`. The parameter `device_id` can be either `0` or `1`. 

Note that this API function is only necessary to use if the user initialized both CAN peripherals via the `can_init()` API function. If only one CAN peripheral was initialized, the one that was initialized is automatically the current one in use. 
```
int can_init(int device_id);
```
Initializes the specified CAN peripheral. This includes activating the CAN peripheral, setting the baud rate an bit timing register to default options, and running a self-test on the peripheral to verify its alive and working. The parameter `device_id` is the CAN peripheral to initialize and can either be 0 or 1. 

As mentioned in the `can_set_peripheral()` description, the peripheral that is initialized is automatically set to the current CAN peripheral in use. 

### Using the CAN Peripherals
```
int can_send(can_packet_t *packet);
```
Sends 8 bit packets of data on the CAN bus. The parameter `*packet` is a pointer to a user defined `can_packet_t` struct (definition of this struct can be found in `can.h`). The struct should be populated with `message_id`, `num_bytes` and `buffer`. The number of elements in the buffer must match `num_bytes`. 

Note that because the `AMDC` system firmware utilizes a cooperative scheduler, this driver simply checks whether the `TxFIFO` buffer is full upon the time when the user is attempting to send packets of data on the `CAN` bus. If the `FIFO` is full, then a `FAILURE` is returned with an optional print statement communicating to the user the scenario that occured and possible suggestions. 

```
int can_rcv(can_packet_t *packet);
```
Populates the user initialized `can_packet_t` struct with the latest CAN packet found in the `RxFIFO`. Specifically, the `message_id`, `num_bytes`, and `buffer` are populated via this API call.

Note that because the `AMDC` system firmware utilizes a cooperative scheduler, this driver simply checks whether the `RxFIFO` is empty upon the time when the user is attempting to view the most recent sent data on the `CAN` bus. If the `FIFO` is empty, then a `FAILURE` is returned with an optional printe statement communicating to the user the scneario that occured and possible suggestions.

### Debugging CAN Peripherals
These drivers have been written for debugging purposes during development, but can be useful for users during utilization of the API to debug the state of the `CAN` peripherals. 
```
void can_print_mode();
```
Prints the mode of the current CAN peripheral in use. One of 5 modes will be printed:
- `XCANPS_MODE_CONFIG`
- `XCANPS_MODE_NORMAL`
- `XCANPS_MODE_SLEEP`
- `XCANPS_MODE_LOOPBACK`
- `XCANPS_MODE_SNOOP`
```
void can_print_peripheral();
```
Prints which CAN peripheral is in use. Either `CAN0` or `CAN1` is printed.
```
int can_loopback_test();
```
Runs a lopoback test within the PicoZed SoM such that data is sent via the `TxFIFO` buffer from the Zync-7000 to the Xilinx FPGA and received back in the `RxFIFO` buffer. This data is validated that it was indeed the data that was originally sent. 

## Final Word

Final word, there is a user app that is implemented to illustrate how to use the CAN API via the CLI. While this user app isn't actually something that is necessarily helpful for any researchers, that is you wouldn't use the CAN peripheral via the command line, it is meant to provide guidance on how to use the interface. This user app can be found in `common/usr/can`.