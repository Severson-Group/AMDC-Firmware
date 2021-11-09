#include "socket_manager.h"
#include "icc.h"
#include "lwip/tcp.h"
#include "ringbuf.h"
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
    int err = XST_FAILURE; // default to overflow error

    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if ((socket_list[i].raw_socket == NULL) || (socket_list[i].raw_socket == raw_socket)) {
            socket_list[i].raw_socket = raw_socket;
            socket_list[i].type = SOCKET_TYPE_IDLE;

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
                // Try to give one data byte to CPU1
                //
                // Yes, this design implies that only ONE byte is transfered
                // between the two cores at a time. Since the other core only
                // reads the data every 100us, this means a limit of 10 kB / sec
                //    ==> SLOW!
                //
                // However, the old standard UART interface we have been using
                // runs at 115200 baud, meaning that its throughput is about
                // 10 kB / sec as well. Therefore, our single byte ICC method
                // is actually duplicating the performance of UART.
                //
                // This merits not improving it, since there are other limits
                // in the other core software arch design which we would need
                // to address first. To really improve this, we ought to redesign
                // the whole system and not support UART ascii comms...
                if (!ringbuf_is_empty(rb)) {

                    // Check if CPU1 is ready to receive a byte of data
                    if (ICC_CPU0to1__GET_CPU1_WaitingForData) {
                        // Clear CPU1's WaitingForData flag
                        ICC_CPU0to1__CLR_CPU1_WaitingForData;

                        // Read one byte from the fifo
                        uint8_t d;
                        ringbuf_memcpy_from(&d, rb, 1);

                        // Set data to shared memory space
                        ICC_CPU0to1__SET_DATA(d);

                        // Tell CPU1 that we just wrote data
                        ICC_CPU0to1__SET_CPU0_HasWrittenData;
                    }
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
