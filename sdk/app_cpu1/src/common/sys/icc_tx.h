#ifndef ICC_TX_H
#define ICC_TX_H

#include <stdint.h>

void icc_tx_init(void);

void icc_tx_append_char_to_fifo(char c);
void icc_tx_log_stream(int socket_id, uint32_t ts, uint32_t data);

#endif // ICC_TX_H
