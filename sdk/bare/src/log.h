#ifndef LOG_H
#define LOG_H

#include <stdint.h>

typedef enum log_var_type_e {
	S_INTEGER = 1,
	U_INTEGER,
	FLOAT,
	DOUBLE
} log_var_type_e;

void log_init(void);
void log_register_var(char* name, void *addr, log_var_type_e type, uint32_t interval_usec);
void log_callback(void);
void log_dump_callback(void);
void log_dump_uart(void);

#endif // LOG_H
