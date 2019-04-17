#ifndef DEFINES_H
#define DEFINES_H


#define UNUSED(x) (void)(x)

#define USEC_IN_SEC     (1000000)

#define SEC_TO_USEC(sec)    (sec * USEC_IN_SEC)
#define USEC_TO_SEC(usec)   (usec / USEC_IN_SEC)

#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

#define HANG printf("HANG!!!\n"); while(1)


#endif // DEFINES_H
