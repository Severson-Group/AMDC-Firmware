# Logging Binary Format

## Overview

The AMDC can dump the logged variables to the UART serial console using either text-based formatting or the binary
format. This document describes the binary formatting which appears on the UART interface.

## Dumping Variables

Each logged variable must be dumped one at a time (i.e. if the user has 10 slots, each must be dumped one by one).

To start a dump of the Nth variable slot: `log dump bin N`

## Binary Format

All data is little-endian. The following summarizes a complete data packet for one variable dump.

| Byte 0-15 | Byte 16-19 | Byte 20-23 | Byte 24 - 27 | Byte 28 - 35 | ... | Byte `36 + 4*(n-1)` - `36 + 4*n-1` | Byte `36 + 4*n` -  `36 + 4*n + 15` | Byte `36 + 4*n + 16` -  `36 + 4*n + 19` |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| Header | Number of samples | Sample Rate | Data Type | sample packet 0 | ... | sample packet _n_ | Footer | CRC-32 |

### Header (16 bytes)

0x12345678 (repeated four times)

### Number of Samples in Transmission (4 bytes, unsigned integer)

Example (4660 samples): 0x00001234

### Sample Rate (usec) (4 bytes, unsigned integer)

Example (50000 usec): 0x0000C350

### Data Type (4 bytes, integer)

- 0x00000001 = int
- 0x00000002 = float
- 0x00000003 = double

### Each Sample (4 bytes)

- If int, bytes are in signed integer format.
- If float or double, bytes are in IEEE 754 float format (standard C `float` data type).

### Footer (16 bytes)

0x11223344 (repeated four times)

### CRC-32 (4 bytes)

A CRC-32 calculated over all data bytes (i.e. from the header through and including the footer bytes). The CRC implementation on the AMDC matches the Python libraries for CRC-32:

- `binascii.crc32()`
- `zlib.crc32()`
