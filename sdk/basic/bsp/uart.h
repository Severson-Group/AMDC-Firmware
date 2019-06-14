#ifndef UART_H
#define UART_H

#include <stdint.h>

int uart_init(void);

int uart_send(char *msg, int len);

#endif // UART_H
