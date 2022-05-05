/*
 * Copyright (c) 2021 Joao Dullius
 * Copyright (c) 2020 Sigurd Nevstad
 * Based on:
 * https://github.com/sigurdnev/ncs-playground/tree/master/samples/i2c_scanner
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/i2c.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <drivers/gpio.h>

#define I2C_NODE DT_NODELABEL(arduino_i2c)

void main(void)
{

    printk("Starting i2c scanner at %s\n", DT_LABEL(I2C_NODE));

	const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);
    	if (i2c_dev == NULL || !device_is_ready(i2c_dev)) {
		printk("I2C: Device driver not found.\n");
		return;
	}
	
	i2c_dev = device_get_binding(DT_LABEL(I2C_NODE));

	uint8_t ret = 0;
	
	i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_STANDARD));
	
	for (uint8_t i = 0; i <= 0x7F; i++) {	

        ret = i2c_reg_write_byte(i2c_dev,i, 0, 1);

        if (ret == 0) {
			printk("0x%2x FOUND\n", i);
		}
		else {
			//printk("error %d \n", error);
		}
		
	}
	printk("Scanning done\n");
	
}