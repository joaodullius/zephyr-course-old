/*
 * Copyright (c) 2022 Joao Dullius
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)

const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void main(void)
{
    uint8_t ret;
    printk("Hello Led");

    if (!device_is_ready(led.port)) {
        return;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret <0) {
        return;
    }

    while(1) {
        ret = gpio_pin_toggle_dt(&led);
        if (ret <0) {
            return;
        }
        k_msleep(CONFIG_LED_BLINK_PERIOD);
    }
}