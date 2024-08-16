#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "ringbuf.h"

// Communications
#define MAX_NUM_SOCKETS         (8)
#define MAX_RX_RING_BUFFER_DATA (1024 * 1024)

typedef enum {
    SOCKET_TYPE_UNUSED = 0,
    SOCKET_TYPE_IDLE,
    SOCKET_TYPE_ASCII_CMD,
    SOCKET_TYPE_LOG_VAR,
} socket_type_e;

typedef struct socket {
    Socket_t raw_socket; // this is a freertos+tcp Socket_t type
    socket_type_e type;

    // Rx stuff
    struct ringbuf_t rx_ring_buffer;
    uint8_t rx_ring_buffer_data[MAX_RX_RING_BUFFER_DATA];
} socket_t; // could change the name

void socket_manager_init(void);

bool socket_manager_is_registered(void *raw_socket);

int socket_manager_put(Socket_t raw_socket);
int socket_manager_remove(Socket_t raw_socket);

// This is called from inside the LwIP stack during packet rx. It copies the
// incoming data into a ring buffer for future use.
void socket_manager_rx_data(Socket_t raw_socket, uint8_t *data, uint16_t len);

// This is called from user code and immediately transfers the outgoing data
// into the LwIP tcp layer. The data is not actaully sent right away,
// since it is up to LwIP to decide when to actually send the data.
void socket_manager_tx_data(Socket_t raw_socket, uint8_t *data, uint16_t len);

// This is to be called in the application main loop.
int socket_recv(char *buffer, uint32_t length, Socket_t *rawSocketRet);

void socket_manager_log_socket_send(int socket_id, char c);
void socket_manager_broadcast_ascii_cmd_byte(char c);

void socket_manager_flush_log_socket(int socket_id);
void socket_manager_flush_ascii_cmd_sockets(void);

#endif // SOCKET_MANAGER_H
