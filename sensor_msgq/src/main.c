/*
 * Copyright (c) 2022 Joao Dullius
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdio.h>
#include <kernel.h>
#include <drivers/sensor.h>
#include <drivers/gpio.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(sensor_msgq);

#define MSGQ_SIZE 16
#define SAMPLE_INTERVAL 500

//Button Binding
#define BUTTON0_NODE DT_NODELABEL(button0)
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static struct gpio_callback button_cb_data;

#if !defined(CONFIG_APP_SENSOR_SIM)
	//Sensor Binding
const struct device *sensor = DEVICE_DT_GET(DT_INST(0,st_hts221));
#endif

static int timestamp = 0;


// Defining Message Queue Structure
static struct mqsq_item_t {
	uint32_t 	timestamp;
	struct sensor_value temp;
	struct sensor_value hum;
};

K_MSGQ_DEFINE(my_msgq, sizeof(struct mqsq_item_t), MSGQ_SIZE, 4);

// Defining Data Consumer Work Queue
void wk_print_h(struct k_work *work){
	bool not_empty = false;
	struct mqsq_item_t rx_data;
	while(k_msgq_get(&my_msgq, &rx_data, K_NO_WAIT) == 0){
		printf("%04d | Sensor Temp %.1f C | Hum %.1f%%\n", rx_data.timestamp,
										sensor_value_to_double(&rx_data.temp),
										sensor_value_to_double(&rx_data.hum));
		not_empty = true;
	}
	if (not_empty == false){
		LOG_WRN("Consumer | Empty Queue");
	}
}
static K_WORK_DEFINE(wk_print, wk_print_h); 

// Defining Sensor Sampler Work Queue
void wk_sample_h(struct k_work *work){
	struct sensor_value temp, hum;
	uint8_t ret = 0;
	struct mqsq_item_t tx_data;
	
#if !defined(CONFIG_APP_SENSOR_SIM)
		uint8_t err;
		err = sensor_sample_fetch(sensor);
		if(err){
			printf("Sensor fetch error\n");
			return;
		}
	/* Get sensor data */
	sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, &temp);
	sensor_channel_get(sensor, SENSOR_CHAN_HUMIDITY, &hum);
#else
	temp.val1=20+((timestamp*11)%6);
	temp.val2=0;
	hum.val1=70+((timestamp*13)%7);
	hum.val2=0;
#endif	
	tx_data.temp=temp;
	tx_data.hum=hum;
	tx_data.timestamp=timestamp++;

	ret = k_msgq_put(&my_msgq, &tx_data, K_NO_WAIT);
	if (ret != 0) {
            LOG_WRN("Sampler | Lost sample - MSGQ Full");
        }
}
static K_WORK_DEFINE(wk_sample, wk_sample_h); 

void sample_timer_h(struct k_timer *timer_id)
{
    k_work_submit(&wk_sample);}
K_TIMER_DEFINE(sampler_timer, sample_timer_h, NULL);

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins){
	k_work_submit(&wk_print);
	}
void main(void){

#if !defined(CONFIG_APP_SENSOR_SIM)
	if (sensor == NULL || !device_is_ready(sensor)) {
		printf("Could not get sensor device\n");
		return;
	}
#else
	LOG_INF("Using simulated sensor");
#endif

	LOG_INF("Sensor Message Queue Sample");
	LOG_INF("MSGQ Size:               %d", MSGQ_SIZE);
	LOG_INF("MSGQ Width:              %d", sizeof(struct mqsq_item_t));
	LOG_INF("Sampling Interval:       %d ms", SAMPLE_INTERVAL);
	LOG_INF("MSGQ Overflow Interval : %d ms", SAMPLE_INTERVAL*MSGQ_SIZE);

	gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

	k_timer_start(&sampler_timer, K_SECONDS(1), K_MSEC(SAMPLE_INTERVAL));

    printk("Fim do main().\n");
}
