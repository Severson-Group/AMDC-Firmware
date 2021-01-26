#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <string.h>

#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static inline bool STREQ(char *in1, char *in2)
{
    return (strcmp(in1, in2) == 0) ? true : false;
}

#endif // UTIL_H
