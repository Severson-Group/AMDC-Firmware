#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <stdint.h>

void socket_manager_init(void);

int socket_manager_remove(void *socket);
int socket_manager_put(void *socket);
int socket_manager_get(void *socket, char **data, uint32_t **state);

#endif // SOCKET_MANAGER_H
