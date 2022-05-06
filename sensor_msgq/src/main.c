/*
 * Copyright (c) 2022 Joao Dullius
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdio.h>
#include <kernel.h>
#include <drivers/sensor.h>
#include <drivers/gpio.h>

#define MSGQ_SIZE 16
#define SAMPLE_INTERVAL 500

//Button Binding
#define BUTTON0_NODE DT_NODELABEL(button0)
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static struct gpio_callback button_cb_data;

//Sensor Binding
const struct device *hts221 = DEVICE_DT_GET_ANY(st_hts221);

static int timestamp = 0;


// Defining Message Queue Structure
static struct mqsq_item_t {
	uint32_t 	timestamp;
	struct sensor_value temp;
	struct sensor_value hum;
};

K_MSGQ_DEFINE(my_msgq, sizeof(struct mqsq_item_t), MSGQ_SIZE, 4);

// Defining Data Consumer Work Queue
void wk_consume_h(struct k_work *work){
	bool not_empty = false;
	struct mqsq_item_t rx_data;
	while(k_msgq_get(&my_msgq, &rx_data, K_NO_WAIT) == 0){
		printf("%04d | HTS221 | Temp %.1f C | Hum %.1f%%\n", rx_data.timestamp,
										sensor_value_to_double(&rx_data.temp),
										sensor_value_to_double(&rx_data.hum));
		not_empty = true;
	}
	if (not_empty == false){
		printf("Consumer | Empty Queue\n");
	}
}
static K_WORK_DEFINE(wk_consume, wk_consume_h); 

// Defining Sensor Sampler Work Queue
void wk_sample_h(struct k_work *work){
	struct sensor_value temp, hum;
	uint8_t ret = 0;
	struct mqsq_item_t tx_data;
	
	if (sensor_sample_fetch(hts221) < 0) {
			printf("HTS221 Sensor sample update error\n");
			return;
		}

	/* Get sensor data */
	sensor_channel_get(hts221, SENSOR_CHAN_AMBIENT_TEMP, &temp);
	sensor_channel_get(hts221, SENSOR_CHAN_HUMIDITY, &hum);
	tx_data.temp=temp;
	tx_data.hum=hum;
	tx_data.timestamp=timestamp++;

	ret = k_msgq_put(&my_msgq, &tx_data, K_NO_WAIT);
	if (ret != 0) {
            printf("Sampler | Lost sample - MSGQ Full\n");
        }
}
static K_WORK_DEFINE(wk_sample, wk_sample_h); 

void produce_timer_handler(struct k_timer *timer_id)
{
    k_work_submit(&wk_sample);}
K_TIMER_DEFINE(sampler_timer, produce_timer_handler, NULL);

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins){
	k_work_submit(&wk_consume);
}
void main(void){

	if (hts221 == NULL) {
		printf("Could not get HTS221 device\n");
		return;
	};
	printf("Sensor Message Queue Sample\n");
	printf("MSGQ Width:              %d\n", sizeof(struct mqsq_item_t));
	printf("Sampling Interval:       %d ms\n", SAMPLE_INTERVAL);
	printf("MSGQ Overflow Interval : %d ms\n", SAMPLE_INTERVAL*MSGQ_SIZE);

	gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

	k_timer_start(&sampler_timer, K_SECONDS(1), K_MSEC(SAMPLE_INTERVAL));

    printk("Fim do main().\n");
}
