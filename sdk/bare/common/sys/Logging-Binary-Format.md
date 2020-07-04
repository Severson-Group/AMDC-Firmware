# Logging Binary Format

## Overview

The AMDC can dump the logged variables to the UART serial console using either text-based formatting or the binary
format. This document describes the binary formatting which appears on the UART interface.

## Dumping Variables

Each logged variable must be dumped one at a time (i.e. if the user has 10 slots, each must be dumped one by one).

To start a dump of the Nth variable slot: `log dump bin N`

## Binary Format

All data is little-endian. The following summarizes a complete data packet for one variable dump.

| Byte 0-15 | Byte 16-19 | Byte 20 - 23 | Byte 24 - 31 | ... | Byte `32 + 8*(n-1)` - `32 + 8*n-1` | Byte `32 + 8*n` -  `32 + 8*n + 15` |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| Header | Number of samples | Data Type | sample packet 0 | ... | sample packet _n_ | Footer |

### Header (16 bytes)

0x12345678 (repeated four times)

### Number of Samples in Transmission (4 bytes, unsigned integer)

Example (4660 samples): 0x00001234

### Data Type (4 bytes, integer)

- 0x00000001 = int
- 0x00000002 = float
- 0x00000003 = double

### Each Sample (8 bytes)

| Byte 0-3            | Byte 4-7 |
|---------------------|----------|
| Timestamp (usec)    | Data     |

### Footer (16 bytes)

0x11223344 (repeated four times)
