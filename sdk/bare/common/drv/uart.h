#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "sys/errors.h"

#define UART_RX_FIFO_LENGTH (64)
#define UART_TX_FIFO_LENGTH (64)

error_t uart_init(void);

int uart_send(char *msg, int len);
int uart_recv(char *msg, int len);

#endif // UART_H
