/*
 * Copyright (c) 2022 Joao Dullius
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <zephyr/drivers/gpio.h>

#include <os_mgmt/os_mgmt.h>
#include <img_mgmt/img_mgmt.h>

#define LED0_NODE DT_ALIAS(led0)

const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void main(void)
{
    uint8_t ret;
    printk("Hello Led");

    os_mgmt_register_group();
    img_mgmt_register_group();

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