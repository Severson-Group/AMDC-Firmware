# IP: `amdc_timing_manager_1.0`

This IP core manages the timing for acquisition/conversion in the eddy current sensors, encoder, and ADC.

## Features

- All sensors are synchronous with AMDC PWM carrier
- Configurable ratio of PWM carrier frequency to control frequency
- Acquisition time reporting for individual sensors
- Two modes of interrupt to update timing statistics for the sensors and drive the scheduler
- Records time between ISR calls to use for the scheduler
- Enable/disable individual sensors

This interface generates a user-configurable trigger signal that starts an acquisition or conversion sample for every sensor that is enabled. As each sensor finishes, it records the time it took for an acquisition/conversion cycle and waits until all enabled sensors are completed.

Note that a trigger signal can only be asserted if none of the sensors are still collecting data; therefore, the ratio still counts the number of carrier occurences to assert a trigger, but a trigger may be "skipped over" if a particularly slow sensor cannot complete in one trigger cycle.

## IP Interface

This IP is accessed via the AXI4-Lite register-based interface from the DSP. This interface sends an enable signal to the ADC, encoder, AMDS, and eddy current sensors, and recieves a done signal once they have completed their respective acquisitions. This IP also interfaces with the ZYNQ7 processing system.

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | TRIG_CFG| R/W | Configuration Register |
| 0x04 | ENABLE_CFG | R/W | Configuration Register |
| 0x08 | RATIO_CFG | R/W | Configuration Register |
| 0x0C | PWM_CFG | R/W | Configuration Register |
| 0x10 | ISR_REG | R/W | Configuration Register |
| 0x14 | ISR_TIME | R | Data Register |
| 0x18 | ADC_ENC_TIME | R | Data Register |
| 0x1C | AMDS_01_TIME | R | Data Register |
| 0x20 | AMDS_23_TIME | R | Data Register |
| 0x24 | EDDY_01_TIME | R | Data Register |
| 0x28 | EDDY_23_TIME | R | Data Register |

### TRIG_CFG

| Bits | Name | Description |
| -- | -- | -- |
| 0 | DO_AUTO_TRIGGERING | Trigger is sent whenever <user_ratio> PWM events have occurred |
| 1 | SEND_MANUAL_TRIGGER | Send a single trigger event aligned to the next PWM carrier peak or valley |

### ENABLE_CFG

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | EN_BITS | Enable a specific sensor by setting its assigned bit (default 0x00) |

### RATIO_CFG

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | USER_RATIO | Ratio between the carrier frequency and control frequency (i.e. the number of the PWM event qualifer occurrences in order for a trigger to be asserted) (default 10) |

### PWM_CFG

| Bits | Name | Description |
| -- | -- | -- |
| 0 | PWM_SYNC_HIGH | If the bit is 1, sensor acquisition sequence will be triggered when the PWM carrier hits its maximum value (default 0) |
| 1 | PWM_SYNC_LOW | If the bit is 1, sensor acquisition sequence will be triggered when the PWM carrier hits its minimum value (default 1) | 

### ISR_REG

| Bits | Name | Description |
| -- | -- | -- |
| 0 | RESET_SCHED_ISR | Clears the hardware interrupt once it has been recieved by the processing system |
| 1 | SCHED_SOURCE_MODE | Determines the source of the interrupt for the scheduler: legacy mode or timing manager-synchronized |

### ISR_TIME

| Bits | Name | Description |
| -- | -- | -- |
| 31:0 | SCHED_TICK_TIME | Number of clock cycles between each generated interrupt |

### ADC_ENC_TIME

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | ADC_TIME | Time (in FPGA clock cycles) that the ADC took in a conversion/acquisition cycle |
| 31:16 | ENCODER_TIME | Time (in FPGA clock cycles) that the encoder took in an acquisition cycle |

### AMDS_01_TIME

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | AMDS_0_TIME | Time (in FPGA clock cycles) that the AMDS in GPIO 0 took in a conversion/acquisition cycle |
| 31:16 | AMDS_1_TIME | Time (in FPGA clock cycles) that the AMDS in GPIO 1 took in a conversion/acquisition cycle |

### AMDS_23_TIME

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | AMDS_2_TIME | Time (in FPGA clock cycles) that the AMDS in GPIO 2 took in a conversion/acquisition cycle |
| 31:16 | AMDS_3_TIME | Time (in FPGA clock cycles) that the AMDS in GPIO 3 took in a conversion/acquisition cycle |

### EDDY_01_TIME

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | EDDY_0_TIME | Time (in FPGA clock cycles) that the eddy current sensor in GPIO 0 took in a conversion/acquisition cycle |
| 31:16 | EDDY_1_TIME | Time (in FPGA clock cycles) that the eddy current sensor in GPIO 1 took in a conversion/acquisition cycle |

### EDDY_23_TIME

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | EDDY_2_TIME | Time (in FPGA clock cycles) that the eddy current sensor in GPIO 2 took in a conversion/acquisition cycle |
| 31:16 | EDDY_3_TIME | Time (in FPGA clock cycles) that the eddy current sensor in GPIO 3 took in a conversion/acquisition cycle |
