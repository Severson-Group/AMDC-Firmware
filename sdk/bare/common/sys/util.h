#ifndef UTIL_H
#define UTIL_H

#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define STR_EQ(str1, str2) (strcmp(str1, str2) == 0)

#endif // UTIL_H