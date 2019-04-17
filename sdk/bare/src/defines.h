#ifndef DEFINES_H
#define DEFINES_H


//#define CLOCK_FREQ (48000000)

#define UNUSED(x) (void)(x)

#define USEC_IN_SEC     (1000000)

#define SEC_TO_USEC(sec)    (sec * USEC_IN_SEC)
#define USEC_TO_SEC(usec)   (usec / USEC_IN_SEC)

#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

//#define EVEN_ODD(value,even_output,odd_output)  (((value % 2) == 0) ? even_output : odd_output)

#define HANG printf("HANG!!!\n"); while(1)


#endif // DEFINES_H
