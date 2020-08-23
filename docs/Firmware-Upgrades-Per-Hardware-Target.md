# Firmware Upgrades Per Hardware Target

This document should be referenced when changing hardware targets. Each revision of the AMDC hardware is slightly different, so the user code must be modified. Note that the hardware targets are almost interchangable, minus the small changes listed below.

Most changes occur automatically in the core firmware by simplying `define`ing the appropriate hardware target in the `usr/user_config.h` file.

For example, to target AMDC REV D hardware, make sure the following code exists in your `usr/user_config.h` file:

```C
// Set hardware target to AMDC REV D
#define USER_CONFIG_HARDWARE_TARGET AMDC_REV_D
```

The supported targets are: `AMDC_REV_C` and `AMDC_REV_D`

## AMDC REV C

This is the baseline supported hardware target. Note that this hardware is considered old and is minimally supported. Please upgrade to more recent hardware versions if possible.

Compile code with `AMDC_REV_C` as the hardware target.

Notable unique hardware specs include 16x analog inputs and one RGB LED output. To configure the LED, use the `drv/io.c` module.

## AMDC REV D

This is the latest hardware target and is actively supported.

Compile code with `AMDC_REV_D` as the hardware target.

### Changelog

- Only 8x analog inputs available.
> When using the `enum` values from `drv/analog.h`, the `ANALOG_IN9..16` simply won't be defined, resulting in a compiler error.

- Four RGB LEDs available.
> Control these using the `drv/led.h` module.

- PWM general status lines: A, B, C, D.
> Each power stack port has four generic status lines. The directionality is configurable by jumpers on the PCB, but is shared between ports.

- SensorCard platform motherboard support available.
> The motherboard is not supported on the REV C hardware target. It is only available on REV D hardware.
>
> It is enabled using the `usr/user_config.h` system: define `USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT` to `1` to enable. This will configure the FPGA driver automatically and add a basic command to the CLI to exercise the motherboard interface manually.
