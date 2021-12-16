#ifndef ICC_TX_H
#define ICC_TX_H

#include <stdint.h>

void icc_tx_init(void);

void icc_tx_append_char_to_fifo(char c);
int task_icc_tx_get_buffer_space_available(void);

void icc_tx_log_stream(int socket_id, int var_slot, uint32_t ts, uint32_t data);

#endif // ICC_TX_H
