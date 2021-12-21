// Code generated by universal_crc by Danjel McGougan
// Website: http://www.mcgougan.se/universal_crc/
//
// Run the C script using the following arguments:
// ./universal_crc -b 32 -p 0x04c11db7 -i 0xffffffff -x 0xffffffff -r -a tab
//
// CRC parameters used:
//   bits:       32
//   poly:       0x04c11db7
//   init:       0xffffffff
//   xor:        0xffffffff
//   reverse:    true
//   non-direct: false
//
// CRC of the string "123456789" is 0xcbf43926

#ifndef CRC32_H
#define CRC32_H

#include <stddef.h>
#include <stdint.h>

#define CRC32_DEFAULT_INIT (0xffffffff)

// Perform full CRC-32 calculation (matches Python zlib.crc32 or binascii.crc32)
uint32_t crc32_calc_full(const uint8_t *data, size_t len);

// Let user pass in initial value for CRC (perform CRC in chunks)
// NOTE: User must invert final output!
uint32_t crc32_calc_part(const uint8_t *data, size_t len, uint32_t init);

#endif // CRC32_H