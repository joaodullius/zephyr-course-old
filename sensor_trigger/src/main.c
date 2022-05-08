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

static struct sensor_value accel[3];
const struct device *sensor = DEVICE_DT_GET(DT_INST(0,st_lsm6dsl));

static int sensor_trig_cnt;

/* Trigger Handler */
static void sensor_trigger_handler(const struct device *dev,
				    const struct sensor_trigger *trig)
{
	sensor_sample_fetch_chan(dev, SENSOR_CHAN_ACCEL_XYZ);
	sensor_trig_cnt++;
}

void main(void)
{
	int8_t err;
	int cnt = 1;

	if (sensor == NULL || !device_is_ready(sensor)) {
		printf("Could not get sensor device\n");
		return;
	}


/* Set sensor accel sampling frequency to 104 Hz */
	struct sensor_value odr_attr;
	odr_attr.val1 = 208;
	odr_attr.val2 = 0;

	if (sensor_attr_set(sensor, SENSOR_CHAN_ACCEL_XYZ,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
		printk("Cannot set sampling frequency for accelerometer.\n");
		return;
	}

	/* Set Trigger */
	struct sensor_trigger trig;
	trig.type = SENSOR_TRIG_DATA_READY;
	trig.chan = SENSOR_CHAN_ACCEL_XYZ;
	sensor_trigger_set(sensor, &trig, sensor_trigger_handler);
	

	/* First fetch */
	err = sensor_sample_fetch(sensor);
		if(err){
			LOG_WRN("Sensor fetch error");
			return;
		}

	while (1) {
		/* Get sensor data */
		sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, accel);

		/* Print sensor data */
		printf("%d:: SENSOR: Accel (m.s-2): x: %.1f, y: %.1f, z: %.1f | trig %d\n",
		       cnt++,
			   sensor_value_to_double(&accel[0]),
		       sensor_value_to_double(&accel[1]),
		       sensor_value_to_double(&accel[2]),
			   sensor_trig_cnt);

		k_sleep(K_MSEC(2000));
	}
}
