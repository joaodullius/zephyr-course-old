/*
 * Copyright (c) 2022 Joao Dullius
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/uart.h>

#include <string.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(uart_async, 4);


#define MSG_SIZE 32

static struct msgq_item_t {
	char buffer[MSG_SIZE];
	int lenght;
};
K_MSGQ_DEFINE(uart_msgq, sizeof(struct msgq_item_t), 10, 4);

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* receive buffer used in UART ISR callback */
static char rx_buf[MSG_SIZE];

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	struct msgq_item_t rx_data;
	switch (evt->type) {
	
	case UART_TX_DONE:
		LOG_DBG("UART_TX_DONE");
		LOG_INF("Tx sent %d bytes", evt->data.tx.len);
		break;

	case UART_TX_ABORTED:
		LOG_DBG("UART_TX_ABORTED");
		LOG_ERR("Tx aborted");
		break;
		
	case UART_RX_RDY:
		LOG_DBG("UART_RX_RDY");
		LOG_INF("Received data %d bytes", evt->data.rx.len);
		strcpy(rx_data.buffer, rx_buf);
		rx_data.lenght=evt->data.rx.len;
		/* if queue is full, message is silently dropped */
		k_msgq_put(&uart_msgq, &rx_data, K_NO_WAIT);
		uart_rx_disable(uart_dev);
		break;

	case UART_RX_BUF_REQUEST:
		LOG_DBG("UART_RX_BUF_REQUEST");
		break;

	case UART_RX_BUF_RELEASED:
		LOG_DBG("UART_RX_BUF_RELEASED");
		break;
		
	case UART_RX_DISABLED:
		LOG_DBG("UART_RX_DISABLED");
		uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), 100);
		break;

	case UART_RX_STOPPED:
		LOG_DBG("UART_RX_STOPPED");
		break;
		
	default:
		break;
				}

}


void uart_send_fn(int parm1, int parm2, int parm3) {
	char tx_buf[MSG_SIZE];
	struct msgq_item_t tx_data;

	int8_t err;
	while (1) {
		k_msgq_get(&uart_msgq, &tx_data, K_FOREVER);
		strcpy(tx_buf, tx_data.buffer);
		LOG_DBG("Send Message ");
		err = uart_tx(uart_dev, tx_buf, tx_data.lenght, SYS_FOREVER_MS);
		if (err){
			LOG_ERR("Transmit Error");
			return;
		}
 	}
}
#define THREAD_STACK_SIZE 2048
K_THREAD_DEFINE(send_thread, THREAD_STACK_SIZE, uart_send_fn, NULL, NULL, NULL, 
										 K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);

void main(void)
{	
	int8_t err;
	LOG_INF("UART Assyncronous Sample");

	if (uart_dev == NULL || !device_is_ready(uart_dev)) {
		printk("UART device not found!");
		return;
	}

	err = uart_callback_set(uart_dev, uart_cb, NULL);
	__ASSERT(err==0, "Callback Set Failled");
	
	err = uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), 100);
	if (err)
	{
		printk("Enable Error!\n");
		return;
	}

}