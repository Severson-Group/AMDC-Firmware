# AMDC IP Repository

This folder houses the IP for the AMDC platform. IP, or Intellectual Property, is the Xilinx term used to describe libraries of digital circuit design used in the FPGA fabric. These libraries are generally low-level interfaces to high-performance hardware (i.e. Ethernet, DDR4 memory, PCIe, etc) which Xilinx provides as drop-in blocks for user application designs. They communicate with the main system processor (DSP) via the AXI interconnect. See [this website](https://www.xilinx.com/products/intellectual-property.html) for more information about IP.

The [AXI interconnect](https://www.xilinx.com/support/documentation/ip_documentation/ug761_axi_reference_guide.pdf) is the bus that goes between IP cores and the main processor. AXI is part of [ARM AMBA](https://en.wikipedia.org/wiki/Advanced_Microcontroller_Bus_Architecture), a family of microcontroller buses first introduced in 1996. AMBA 4.0, released in 2010, includes the second version of AXI, AXI4. Xilinx has adopted the Advanced eXtensible Interface (AXI) protocol for Intellectual Property (IP) cores.

There are three types of AXI4 interfaces:

1. AXI4 — for high-performance memory-mapped requirements
2. AXI4-Lite — for simple, low-throughput memory-mapped communication (e.g., to and from control and status registers)
3. AXI4-Stream — for high-speed streaming data.

## IP Blocks

The AMDC platform makes extensive use of the IP blocks to interface with the hardware peripherials on the circuit board. This includes PWM outputs, ADC inputs, encoders, SPIs, UARTs, and serially-addressable LEDs. These IP cores and their associated C drivers allow user applications, written in C, to easily control and interface to hardware. The AXI4-Lite interface is generally used for hardware interfacing on the AMDC. AXI4-Lite enables development of simple register-based peripherials.

### `amdc_analog`

This IP core is responsible for driving the external ADC on the AMDC hardware. The C driver let's the user configure the the sampling rate, alignment to PWM, and digital filtering. The latest digitized value for each channel is available to the C code via memory-mapped registers.

### `amdc_dac`

This IP core is responsible for driving a DAC expansion board plugged into the GPIO port on the AMDC hardware (REV C only). The core initially configures the external DAC device, then continously updates the digital values in the DAC based upon memory-mapped registers. These registers are settable via the C driver functions.

### `amdc_encoder`

This IP core interfaces to an incremental quadrature encoder (i.e. ABZ signals) to count the number of steps traveled over time. It also uses the Z index pulse to record absolute position. The position and step values are accessible to C code via memory-mapped registers.

### `amdc_inverters`

This IP core interfaces to the power stacks plugged into the AMDC hardware (up to 8x two-level three-phase inverters). Using the associated C driver, users can configure PWM switching characteristics (i.e. switching frequency and dead-time length). Duty ratios are set by users via memory-mapped registers. Four status lines per inverter are available via registers.

### `amdc_leds`

This IP core drives the serially-addressable RGB LEDs on the REV D AMDC hardware. The FPGA uses a single data line to control the color outputs of all four LEDs via a serial chain approach. Using the associated C driver, users can set color values to memory-mapped registers which automatically are flushed out to the LED hardware.
