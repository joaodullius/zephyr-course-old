/*
 * Copyright (c) 2022 Joao Dullius
 * Copyright (c) 2018 STMicroelectronics
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <drivers/gpio.h>
#include <stdio.h>
#include <sys/util.h>


#define HTS221_DEVNAME	DT_LABEL(DT_INST(0, st_hts221))
#define BUTTON0_NODE 	DT_NODELABEL(button0)
const struct device *hts221 = DEVICE_DT_GET_ANY(st_hts221);
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static struct gpio_callback button_cb_data;

int timestamp = 0;

K_FIFO_DEFINE(temp_fifo);

static struct data_item_t {
    void *fifo_reserved;   /* 1st word reserved for use by FIFO */
	int					timestamp;
	struct sensor_value temp;
	struct sensor_value hum;
};

void wk_print_data_h(struct k_work *work){

	struct data_item_t *rx_buf;
	rx_buf = k_fifo_get(&temp_fifo, K_FOREVER);
	printf("%4d | HTS221 | Temp  %.1f C | Rel Hum %.1f%%\n",
										rx_buf->timestamp,
										sensor_value_to_double(&rx_buf->temp),
										sensor_value_to_double(&rx_buf->hum));
}
K_WORK_DEFINE(wk_print_data, wk_print_data_h);

void wk_sample_sensor_h(struct k_work *work) {
	struct data_item_t tx_buf;
	if (sensor_sample_fetch(hts221) < 0) {
		printf("HTS221 Sensor sample update error\n");
		return;
	}
	sensor_channel_get(hts221, SENSOR_CHAN_AMBIENT_TEMP, &tx_buf.temp);
	sensor_channel_get(hts221, SENSOR_CHAN_HUMIDITY, &tx_buf.hum);
	tx_buf.timestamp=timestamp++;
	k_fifo_put(&temp_fifo, &tx_buf);
   	wk_print_data_h(NULL);
}
K_WORK_DEFINE(wk_sample_sensor, wk_sample_sensor_h);

void timer_handler(struct k_timer *timer_id){
	k_work_submit(&wk_sample_sensor);
}
K_TIMER_DEFINE(my_timer, timer_handler, NULL);

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	k_work_submit(&wk_sample_sensor);
}

void main(void){

	if (hts221 == NULL) {
		printf("Could not get HTS221 device\n");
		return;
	}

    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

	//k_timer_start(&my_timer, K_MSEC(500), K_SECONDS(2));

    printk("Fim do main().\n");

}
