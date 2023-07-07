#ifdef APP_ADDER

#include "usr/adder/app_adder.h"
#include "usr/adder/cmd/cmd_adder.h"

void app_adder_init(void) {
	cmd_adder_register();
}

#endif // APP_ADDER
