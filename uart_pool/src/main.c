/*
 * Copyright (c) 2022 Joao Dullius
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <drivers/uart.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(uart_app);

#define UART_NODE DT_NODELABEL(uart0)

void main(void)
{
	unsigned char recv_char;
    LOG_INF("Hello UART");

    const struct device *uart = DEVICE_DT_GET(UART_NODE);
    	if (uart == NULL || !device_is_ready(uart)) {
		    LOG_ERR("Could not open UART");
		    return;
	    }
	
	while(1) {
		while(uart_poll_in(uart, &recv_char) < 0){
			k_yield();
		}
		uart_poll_out(uart, recv_char);
		LOG_INF("Received Char = %c", recv_char);
		k_yield();
	}
    
}