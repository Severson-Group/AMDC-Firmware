# AMDC IP: `amdc_leds`

This IP core drives the serially-addressable RGB LEDs on the AMDC hardware. Serially-addressable LEDs are "smarter" than typical discrete LEDs -- they have tiny processors in them to parse a serial bit stream which encodes the desired LED brightness levels for the red, green, and blue components. Once a single LED has received its desired output setting, it acts as a pass-through device and forwards all future incoming bit streams onto the next LED in the chain. This allows a single data line to control arbitrarily many RGB LEDs.

Diagram of serially connected LEDs:
<img src="" width=500 />

## Theory of Operation

This IP core adheres to the AXI4-Lite interface, meaning it uses memory-mapped registers for data transfer between the processor and the IP core. The data which is transferred is the color values for each of the four LEDs on the AMDC hardware. Each register corresponds to one RGB LED in hardware. The 32-bit register is used to store the current color setting for the RGB LED in the following format: the lower 24 bits encode the red, green, and blue components (8-bit green, 8-bit red, 8-bit blue).

Diagram of single register contents:
<img src="" width=500 />

## IP Registers

This IP core has four 32-bit memory-mapped registers.

#### Register 0: RGB LED 0
#### Register 1: RGB LED 1
#### Register 2: RGB LED 2
#### Register 3: RGB LED 3
