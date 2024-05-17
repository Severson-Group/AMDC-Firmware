# IP: `amdc_timing_manager_1.0`

This IP core manages the timing for acquisition/conversion in the eddy current sensors, encoder, and ADC.

## Features

- All sensors are synchronous with AMDC PWM carrier
- Configurable ratio of PWM carrier frequency to control frequency
- Acquisition time reporting for individual sensors
- Interrupt generation when all the enabled sensors are done to update timing statistics for the sensors
- Enable individual sensors (all are disabled upon system startup)

This interface generates a user-configurable trigger signal that starts an acquisition or conversion sample for every sensor that is enabled. As each sensor finishes, it records the time it took for an acquisition/conversion cycle and waits until all enabled sensors are completed.

Note that a trigger signal can only be asserted if none of the sensors are still collecting data; therefore, the ratio still counts the number of carrier occurences to assert a trigger, but a trigger may be "skipped over" if a particularly slow sensor cannot complete in one trigger cycle.

## IP Interface

This IP is accessed via the AXI4-Lite register-based interface from the DSP. This interface sends an enabling signal to the ADC, encoder, and eddy current sensors, and recieves a done signal once they have completed their respective acquisitions. This IP also interfaces with the ZYNQ7 processing system.

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | UNUSED | R/W | Unused Register |
| 0x04 | ENABLE_BITS | R/W | Configuration Register |
| 0x08 | USER_RATIO | R/W | Configuration Register |
| 0x0C | PWM_TRIGGERS | R/W | Configuration Register |
| 0x10 | UNUSED | R/W | Unused Register |
| 0x14 | EDDY_SENSOR_TIME | R | Data Register |
| 0x18 | EDDY_SENSOR_TIME | R | Data Register |
| 0x1C | SENSOR_TIME | R | Data Register |
| 0x20 | INTERRUPT_RESET | R/W | Reset Register |

### ENABLE_BITS

| Bits | Name | Description |
| -- | -- | -- |
| 7:0 | EN_BITS | Enable a specific sensor by setting one of the bits, where currently the eddy current sensors 0 - 3 are bits 0 - 3 respectively, encoder is bit 4, and ADC is bit 5 (default 0x00) |

### USER_RATIO

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | user_ratio | Ratio between the carrier frequency and control frequency (i.e. the number of the PWM event qualifer occurrences in order for a trigger to be asserted) (default 10) |

### PWM_TRIGGERS

| Bits | Name | Description |
| -- | -- | -- |
| 0 | PWM_SYNC_HIGH | If the bit is 1, sensor acquisition sequence will be triggered when the PWM carrier hits its maximum value (default 0) |
| 1 | PWM_SYNC_LOW | If the bit is 1, sensor acquisition sequence will be triggered when the PWM carrier hits its minimum value (default 1) | 

### EDDY_SENSOR_TIME

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | eddy0_time | Time (in FPGA clock cycles) that the eddy current sensor 0 took in a conversion/acquisition cycle |
| 31:16 | eddy1_time | Time (in FPGA clock cycles) that the eddy current sensor 1 took in a conversion/acquisition cycle |

### EDDY_SENSOR_TIME

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | eddy2_time | Time (in FPGA clock cycles) that the eddy current sensor 2 took in a conversion/acquisition cycle |
| 31:16 | eddy3_time | Time (in FPGA clock cycles) that the eddy current sensor 3 took in a conversion/acquisition cycle |

### SENSOR_TIME

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | encoder_time | Time (in FPGA clock cycles) that the encoder took in an acquition cycle |
| 31:16 | adc_time | Time (in FPGA clock cycles) that the ADC took in a conversion/acquisition cycle |

### INTERRUPT_RESET

| Bits | Name | Description |
| -- | -- | -- |
| 0 | reset_sched_isr | Clears the hardware interrupt once it has been recieved by the processing system |
