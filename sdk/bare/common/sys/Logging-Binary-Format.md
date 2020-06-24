# Logging Binary Format

## Overview

The AMDC can dump the logged variables to the UART serial console using either text-based formatting or the binary
format. This document describes the binary formatting which appears on the UART interface.

## Dumping Variables

Each logged variable must be dumped one at a time (i.e. if the user has 10 slots, each must be dumped one by one).

To start a dump of the Nth variable slot: `log dump bin N`

## Binary Format

All data is little-endian.

### Magic Header (16 bytes)

0x12345678 (repeated four times)

### Number of Samples (4 bytes, integer)

0x00000010

### Data Type (4 bytes, integer)

- 0x00000001 = int
- 0x00000002 = float
- 0x00000003 = double

### Each Sample (8 bytes)

| Byte 0-3            | Byte 4-7 |
|---------------------|----------|
| Timestamp (usec)    | Data     |

### Magic Footer (16 bytes)

0x11223344 (repeated four times)
