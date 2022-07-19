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


#define HTS221_DEVNAME		DT_LABEL(DT_INST(0, st_hts221))

static struct sensor_value temp, hum;
const struct device *sensor = DEVICE_DT_GET(DT_INST(0,st_hts221));

	const struct device *hts221 = device_get_binding(HTS221_DEVNAME);

void main(void)
{
	int8_t err;

	if (sensor == NULL || !device_is_ready(sensor)) {
		printf("Could not get sensor device\n");
		return;
	}

	while (1) {
		
		/* Get sensor samples */

		err = sensor_sample_fetch(sensor);
		if(err){
			printf("Sensor fetch error\n");
			return;
		}

		/* Get sensor data */
			sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, &temp);
			sensor_channel_get(sensor, SENSOR_CHAN_HUMIDITY, &hum);
		

		/* temperature */
		printf(" Sensor | Temp %.1f C | Hum %.1f%%\n",
										sensor_value_to_double(&temp),
										sensor_value_to_double(&hum));

		k_sleep(K_MSEC(2000));
	}
}

