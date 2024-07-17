#include "socket_manager.h"
#include "icc.h"
#include "lwip/tcp.h"
#include "ringbuf.h"
#include "xil_io.h"
#include "xil_printf.h"
#include "xstatus.h"
#include <stdint.h>

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
    void *raw_socket;
    socket_type_e type;

    // Rx stuff
    struct ringbuf_t rx_ring_buffer;
    uint8_t rx_ring_buffer_data[MAX_RX_RING_BUFFER_DATA];
} socket_t;

static socket_t socket_list[MAX_NUM_SOCKETS];

static void reset_socket(socket_t *socket_ptr)
{
    socket_ptr->raw_socket = NULL;
    socket_ptr->type = SOCKET_TYPE_UNUSED;

    ringbuf_reset(&(socket_ptr->rx_ring_buffer));
}

static void init_socket(socket_t *socket_ptr)
{
    socket_ptr->raw_socket = NULL;
    socket_ptr->type = SOCKET_TYPE_UNUSED;

    ringbuf_init(&socket_ptr->rx_ring_buffer, socket_ptr->rx_ring_buffer_data, MAX_RX_RING_BUFFER_DATA);
}

// socket_manager_init()
//
// Set up the socket data structures.
// Call on power up.
//
void socket_manager_init(void)
{
    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        init_socket(&socket_list[i]);
    }
}

// socket_manager_put()
//
// Call this upon the connection of a new socket to setup a data
// structure for it.
//
// socket: unique pointer to the socket (doesn't need to point
// 	to the socket, just needs to be unique to the socket)
//
// Returns: error code
//
int socket_manager_put(void *raw_socket)
{
    struct tcp_pcb *pcb = raw_socket;

    int err = XST_FAILURE; // default to overflow error

    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if ((socket_list[i].raw_socket == NULL) || (socket_list[i].raw_socket == raw_socket)) {
            socket_list[i].raw_socket = raw_socket;
            socket_list[i].type = SOCKET_TYPE_IDLE;

            printf("Alloc new socket: %d\n", i);

            // Send host this socket ID
            uint8_t d = (uint8_t) i;
            tcp_write(pcb, &d, 1, TCP_WRITE_FLAG_COPY);

            err = XST_SUCCESS;
            break;
        }
    }

    return err;
}

// socket_manager_remove()
//
// Call this upon the disconnection of a socket to free up
// the memory used for its data structure
//
// socket: unique pointer to the socket (doesn't need to point
// 	to the socket, just needs to be unique to the socket)
//
// Returns a status code: NOT_FOUND or SUCCESS
//
int socket_manager_remove(void *raw_socket)
{
    int err = XST_DEVICE_NOT_FOUND;

    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if (socket_list[i].raw_socket == raw_socket) {
            reset_socket(&socket_list[i]);

            printf("Remove socket: %d\n", i);

            err = XST_SUCCESS;
            break;
        }
    }

    return err;
}

void socket_manager_rx_data(void *raw_socket, uint8_t *data, uint16_t len)
{
    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if (socket_list[i].raw_socket == raw_socket) {
            // Copy the incoming data from the TCP packet into our local ring buffer
            //
            // NOTE: this copy function can overflow the ring buffer, but it just
            // overwrites old data, so it is ok... This is not really ok from our
            // application side, so let's just hope the user never sends too much data
            // in a single TCP packet before we can pull the data out of the ring buffer!
            ringbuf_memcpy_into(&socket_list[i].rx_ring_buffer, data, len);
            break;
        }
    }
}

void socket_manager_tx_data(void *raw_socket, uint8_t *data, uint16_t len)
{
    struct tcp_pcb *pcb = raw_socket;

    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if (socket_list[i].raw_socket == raw_socket) {
            if (len > 0) {
                uint16_t tcp_space_avail = tcp_sndbuf(pcb);

                uint16_t bytes_to_send = len;
                if (bytes_to_send > tcp_space_avail) {
                    // Silently truncate data that will get sent!
                    bytes_to_send = tcp_space_avail;
                }

                tcp_write(pcb, data, bytes_to_send, TCP_WRITE_FLAG_COPY);
            }

            break;
        }
    }
}

bool socket_manager_is_registered(void *raw_socket)
{
    bool is_active = false;

    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if (socket_list[i].raw_socket == raw_socket) {
            is_active = true;
            break;
        }
    }

    return is_active;
}

void socket_manager_process_rx_data(void)
{
    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        socket_t *socket = &socket_list[i];
        ringbuf_t rb = &socket->rx_ring_buffer;

        size_t data_len = ringbuf_bytes_used(rb);
        if (data_len > 0) {
            switch (socket->type) {
            case SOCKET_TYPE_IDLE:
            {
                // User is trying to assign the type of this IDLE socket.
                // The first two bytes of data determine the socket type:
                uint8_t d[2];
                ringbuf_memcpy_from(&d[0], rb, 2);

                if (d[0] == 12 && d[1] == 34) {
                    socket->type = SOCKET_TYPE_ASCII_CMD;
                } else if (d[0] == 56 && d[1] == 78) {
                    socket->type = SOCKET_TYPE_LOG_VAR;
                } else {
                    // User specified bogus type.
                    // Keep as IDLE and wait for user to try again
                }

                break;
            }

            case SOCKET_TYPE_ASCII_CMD:
            {
                // Try to give all our ringbuf TCP/IP data to CPU1
                //
                // If we ever find that the ICC shared FIFO gets full,
                // we'll just stop and wait until next time.
                for (int j = 0; j < data_len; j++) {
                    if (ICC_CPU0to1_CH0__GET_ProduceCount - ICC_CPU0to1_CH0__GET_ConsumeCount == ICC_BUFFER_SIZE) {
                        // Shared buffer is full

                        // Break out of inner for-loop which is trying to send all data
                        //
                        // This will allow us to process other sockets as well
                        break;
                    }

                    // Write one byte to the sharedBuffer BEFORE incrementing produceCount
                    uint8_t d;
                    uint8_t *sharedBuffer = ICC_CPU0to1_CH0__BufferBaseAddr;
                    ringbuf_memcpy_from(&d, rb, 1);
                    sharedBuffer[ICC_CPU0to1_CH0__GET_ProduceCount % ICC_BUFFER_SIZE] = d;

                    // Memory barrier required here to ensure update of the sharedBuffer is
                    // visible to the other core before the update of produceCount
                    //
                    // Nathan thinks we don't actually have to do this since we turned off
                    // caching on the OCM, so the write should flush immediately, but,
                    // I might be wrong and it could be stuck in some pipeline...
                    // Just to be safe, we'll insert a DMB instruction.
                    dmb();

                    // Increment produce count
                    ICC_CPU0to1_CH0__SET_ProduceCount(ICC_CPU0to1_CH0__GET_ProduceCount + 1);
                }

                break;
            }

            case SOCKET_TYPE_LOG_VAR:
            {
                // TODO: unsupported as of now, do nothing!
                break;
            }

            case SOCKET_TYPE_UNUSED:
            default:
            {
                // Do nothing;
                break;
            }
            }
        }
    }
}

void socket_manager_flush_log_socket(int socket_id)
{
    struct tcp_pcb *pcb = socket_list[socket_id].raw_socket;
    tcp_output(pcb);
}

void socket_manager_flush_ascii_cmd_sockets(void)
{
    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if (socket_list[i].type == SOCKET_TYPE_ASCII_CMD) {
            struct tcp_pcb *pcb = socket_list[i].raw_socket;
            tcp_output(pcb);
        }
    }
}

void socket_manager_log_socket_send(int socket_id, char c)
{
    struct tcp_pcb *pcb = socket_list[socket_id].raw_socket;
    uint16_t tcp_space_avail = tcp_sndbuf(pcb);

    uint16_t bytes_to_send = 1;
    if (bytes_to_send > tcp_space_avail) {
        // Silently truncate data that will get sent!
        xil_printf("L%d", socket_id);
        bytes_to_send = tcp_space_avail;
    }

    tcp_write(pcb, &c, bytes_to_send, TCP_WRITE_FLAG_COPY);
}

int socket_manager_tcp_sndbuf_space_available(int socket_id)
{
    struct tcp_pcb *pcb = socket_list[socket_id].raw_socket;
    uint16_t tcp_space_avail = tcp_sndbuf(pcb);

    return tcp_space_avail;
}

void socket_manager_broadcast_ascii_cmd_byte(char c)
{
    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if (socket_list[i].type == SOCKET_TYPE_ASCII_CMD) {
            struct tcp_pcb *pcb = socket_list[i].raw_socket;
            uint16_t tcp_space_avail = tcp_sndbuf(pcb);

            uint16_t bytes_to_send = 1;
            if (bytes_to_send > tcp_space_avail) {
                // Silently truncate data that will get sent!
                xil_printf("X");
                bytes_to_send = tcp_space_avail;
            }

            tcp_write(pcb, &c, bytes_to_send, TCP_WRITE_FLAG_COPY);
        }
    }
}
