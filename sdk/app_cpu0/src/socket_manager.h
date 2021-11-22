#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

void socket_manager_init(void);

bool socket_manager_is_registered(void *raw_socket);

int socket_manager_put(void *raw_socket);
int socket_manager_remove(void *raw_socket);

// This is called from inside the LwIP stack during packet rx. It copies the
// incoming data into a ring buffer for future use.
void socket_manager_rx_data(void *raw_socket, uint8_t *data, uint16_t len);

// This is called from user code and immediately transfers the outgoing data
// into the LwIP tcp layer. The data is not actaully sent right away,
// since it is up to LwIP to decide when to actually send the data.
void socket_manager_tx_data(void *raw_socket, uint8_t *data, uint16_t len);

// This is to be called in the application main loop.
//
// It takes the received data from the TCP sockets and processes it.
void socket_manager_process_rx_data(void);

int socket_manager_tcp_sndbuf_space_available(int socket_id);

void socket_manager_log_socket_send(int socket_id, char c);
void socket_manager_broadcast_ascii_cmd_byte(char c);

void socket_manager_flush_log_socket(int socket_id);
void socket_manager_flush_ascii_cmd_sockets(void);

#endif // SOCKET_MANAGER_H
