/*
 * Copyright (c) 2018 STMicroelectronics
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <stdio.h>
#include <sys/util.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_sensor);

#define HTS221_DEVNAME		DT_LABEL(DT_INST(0, st_hts221))

static struct sensor_value temp, hum;
const struct device *hts221 = DEVICE_DT_GET_ANY(st_hts221);

void main(void)
{

	if (hts221 == NULL) {
		LOG_ERR("Could not get HTS221 device");
		return;
	}

	while (1) {

		/* Get sensor samples */

		if (sensor_sample_fetch(hts221) < 0) {
			LOG_WRN("HTS221 Sensor sample update error");
			return;
		}

		/* Get sensor data */
		sensor_channel_get(hts221, SENSOR_CHAN_AMBIENT_TEMP, &temp);
		sensor_channel_get(hts221, SENSOR_CHAN_HUMIDITY, &hum);
		

		/* temperature */
		printk("HTS221: Temperature: %.1f C\n",
		       sensor_value_to_double(&temp));

		/* humidity */
		printk("HTS221: Relative Humidity: %.1f%%\n",
		       sensor_value_to_double(&hum));

		k_sleep(K_MSEC(2000));
	}
}
