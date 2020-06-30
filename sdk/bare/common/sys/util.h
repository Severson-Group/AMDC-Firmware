#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define STR_EQ(str1, str2) (strcmp(str1, str2) == 0)

uint32_t util_critical_section_enter(void);
void util_critical_section_exit(uint32_t primask);

#endif // UTIL_H
