/*
 * Copyright (c) 2022 Joao Dullius
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <zephyr/sys/util.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sensor_timer, 3);


const struct device *sensor = DEVICE_DT_GET(DT_INST(0, st_hts221));

#define BUTTON0_NODE DT_NODELABEL(button0)
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static struct gpio_callback button_cb_data;

#define MSGQ_SIZE 16
#define SAMPLE_INTERVAL 500

static int timestamp = 0;

static struct msgq_item_t {
	uint32_t 	timestamp;
	struct sensor_value temp;
	struct sensor_value hum;
};
K_MSGQ_DEFINE(my_msgq, sizeof(struct msgq_item_t), MSGQ_SIZE, 4);

void wk_print_h(struct k_work *work){
    bool not_empty = false;
    struct msgq_item_t rx_data;
    while(k_msgq_get(&my_msgq, &rx_data, K_NO_WAIT) == 0) {
          printf("%04d | Sensor | Temp %.1f C | Hum %.1f%%\n", 
                                    rx_data.timestamp,
                                    sensor_value_to_double(&rx_data.temp),
                                    sensor_value_to_double(&rx_data.hum));
        not_empty = true;
    }
    if (not_empty == false){
        LOG_WRN("Consumer | Empty Queue");
    }
}
static K_WORK_DEFINE(wk_print, wk_print_h); 

void wk_sample_h(struct k_work *work)
{
    static struct sensor_value temp, hum;
    struct msgq_item_t tx_data;
    uint8_t ret;

    LOG_DBG("Sensor Reading");
    sensor_sample_fetch(sensor);
    sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, &temp);
    sensor_channel_get(sensor, SENSOR_CHAN_HUMIDITY, &hum);

    tx_data.temp=temp;
    tx_data.hum=hum;
    tx_data.timestamp=timestamp++;

    ret = k_msgq_put(&my_msgq, &tx_data, K_NO_WAIT);
    if (ret != 0) {
        LOG_WRN("Sampler | Lost Sample - MSGQ Full");
    }

}
K_WORK_DEFINE(my_work, wk_sample_h);

void timer_handler(struct k_timer *timer_id)
{
    k_work_submit(&my_work);
}
K_TIMER_DEFINE(my_timer, timer_handler, NULL);

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins){
	k_work_submit(&wk_print);
	}

void main(void)
{
    uint8_t ret;
    printk("Hello Sensor MSGQ\n");

    LOG_INF("Sensor Message Queue Sample");
	LOG_INF("MSGQ Size:               %d", MSGQ_SIZE);
	LOG_INF("MSGQ Width:              %d", sizeof(struct msgq_item_t));
	LOG_INF("Sampling Interval:       %d ms", SAMPLE_INTERVAL);
	LOG_INF("MSGQ Overflow Interval : %d ms", SAMPLE_INTERVAL*MSGQ_SIZE);

    if (sensor == NULL || !device_is_ready(sensor)){
        LOG_ERR("Could not get sensor device");
        return;
    }

    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

    k_timer_start(&my_timer, K_MSEC(1000), K_MSEC(SAMPLE_INTERVAL));

    LOG_WRN("Fim do main()");
}
