/*
 * Copyright (c) 2022 Joao Dullius
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>
#include <zephyr/sys/util.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sensor_msgq, 4);

#define MSGQ_SIZE 16
#define SAMPLE_INTERVAL 500

const struct device *sensor = DEVICE_DT_GET(DT_INST(0, st_hts221));

static int timestamp = 0;

static struct msgq_item_t {
    uint32_t    timestamp;
    struct sensor_value temp;
    struct sensor_value hum;
};
K_MSGQ_DEFINE(my_msgq, sizeof(struct msgq_item_t), MSGQ_SIZE, 4);

void wk_sample_h(struct k_work *work)
{
    struct sensor_value temp, hum;
    struct msgq_item_t tx_data;
    uint8_t ret;

    LOG_DBG("Sensor Sampled");
    sensor_sample_fetch(sensor);
    sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, &temp);
    sensor_channel_get(sensor, SENSOR_CHAN_HUMIDITY, &hum);

    tx_data.temp=temp;
    tx_data.hum=hum;
    tx_data.timestamp=timestamp++;

    ret = k_msgq_put(&my_msgq, &tx_data, K_NO_WAIT);
    if (ret != 0) {
        LOG_WRN("Sampler WQ | Lost Sample - MSGQ Full");
    }

    /*
    printf(" Sensor | Temp %.1f C | Hum %.1f%%\n", 
                                    sensor_value_to_double(&temp),
                                    sensor_value_to_double(&hum));
    */
}
K_WORK_DEFINE(my_work, wk_sample_h);

void timer_handler(struct k_timer *timer_id)
{
    k_work_submit(&my_work);
}
K_TIMER_DEFINE(my_timer, timer_handler, NULL);

void main(void)
{
    uint8_t ret;
    printk("Hello Sensor MSGQ\n");

    if (sensor == NULL || !device_is_ready(sensor)){
        LOG_ERR("Could not get sensor device");
        return;
    }
    
    LOG_INF("Sensor Message Queue Sample");
	LOG_INF("MSGQ Size:               %d", MSGQ_SIZE);
	LOG_INF("MSGQ Width:              %d", sizeof(struct mqsq_item_t));
	LOG_INF("Sampling Interval:       %d ms", SAMPLE_INTERVAL);
	LOG_INF("MSGQ Overflow Interval : %d ms", SAMPLE_INTERVAL*MSGQ_SIZE);

    k_timer_start(&my_timer, K_MSEC(1000), K_MSEC(SAMPLE_INTERVAL));

    LOG_WRN("Fim do main()");
}