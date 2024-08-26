#include "socket_manager.h"
#include "xil_io.h"
#include "xil_printf.h"
#include "xstatus.h"
#include <stdint.h>
/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

socket_t socket_list[MAX_NUM_SOCKETS];

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
// Returns: socket_id
//
int socket_manager_put(Socket_t raw_socket)
{
    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if ((socket_list[i].raw_socket == NULL) || (socket_list[i].raw_socket == raw_socket)) {
            socket_list[i].raw_socket = raw_socket;
            socket_list[i].time_alive = 1000;
            socket_list[i].type = SOCKET_TYPE_IDLE;

            xil_printf("Alloc new socket: %d\n", i);

            // Send host this socket ID
            uint8_t d = (uint8_t) i;
            uint32_t sent = FreeRTOS_send(raw_socket, &d, 1, 0);
            if (sent == 1) {
            	return i;
            }
            return -1;
        }
    }
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
int socket_manager_remove(Socket_t raw_socket)
{
    int err = XST_DEVICE_NOT_FOUND;

    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if (socket_list[i].raw_socket == raw_socket) {
            reset_socket(&socket_list[i]);

            xil_printf("Remove socket: %d\n", i);

            err = XST_SUCCESS;
            break;
        }
    }

    return err;
}

void socket_manager_rx_data(Socket_t raw_socket, uint8_t *data, uint16_t len)
{
	char dataChar[40];
	memcpy(dataChar, data, 20);
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

void socket_manager_tx_data(Socket_t raw_socket, uint8_t *data, uint16_t len)
{
    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if (socket_list[i].raw_socket == raw_socket) {
            if (len > 0) {
                FreeRTOS_send(raw_socket, data, len, 0);
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

int get_socket_id(Socket_t raw_socket) {
	for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
		if (socket_list[i].raw_socket == raw_socket) {
			return i;
		}
	}
	return -1;
}

int socket_recv(char *buffer, uint32_t length, Socket_t *rawSocketRet) {
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
                // Try to give all our ringbuf TCP/IP data to buffer
            	int j = 0;
                for (; j < data_len; j++) {
                    if (j >= length) {
                        // buffer is full

                        // Break out of inner for-loop which is trying to send all data
                        //
                        // This will allow us to process other sockets as well
                        break;
                    }

                    // Write one byte to the buffer
                    uint8_t d;
                    ringbuf_memcpy_from(&d, rb, 1);
                    buffer[j] = d;

                    // Memory barrier required here to ensure update of the sharedBuffer is
                    // visible to the other core before the update of produceCount
                    //
                    // Nathan thinks we don't actually have to do this since we turned off
                    // caching on the OCM, so the write should flush immediately, but,
                    // I might be wrong and it could be stuck in some pipeline...
                    // Just to be safe, we'll insert a DMB instruction.
                    dmb();
                }
                *rawSocketRet = socket->raw_socket;
                return j;
                break;
            }

            case SOCKET_TYPE_LOG_VAR:
            {
                // TODO: unsupported as of now, do nothing! No data gets received in logging mode.
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
    return 0;
}

void socket_manager_log_socket_send(int socket_id, char c)
{
    Socket_t raw = socket_list[socket_id].raw_socket;
    FreeRTOS_send(raw, &c, 1, 0);
}

//int socket_manager_tcp_sndbuf_space_available(int socket_id)
//{
//    struct tcp_pcb *pcb = socket_list[socket_id].raw_socket;
//    uint16_t tcp_space_avail = tcp_sndbuf(pcb);
//
//    return tcp_space_avail;
//}

void socket_manager_broadcast_ascii_cmd_byte(char c)
{
    for (int i = 0; i < MAX_NUM_SOCKETS; i++) {
        if (socket_list[i].type == SOCKET_TYPE_ASCII_CMD) {
        	Socket_t raw = socket_list[i].raw_socket;
        	FreeRTOS_send(raw, &c, 1, 0);
        }
    }
}

void socket_manager_set_time(int socket_id, uint32_t time) {
	socket_list[socket_id].time_alive = time;
}

int socket_manager_time_alive(int socket_id) {
	return socket_list[socket_id].time_alive;
}
