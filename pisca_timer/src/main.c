/*
 * Copyright (c) 2022 Joao Dullius
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)

const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void timer_handler(struct k_timer *timer_id)
{
    gpio_pin_toggle_dt(&led);
}
K_TIMER_DEFINE(my_timer, timer_handler, NULL);


void main(void)
{
    int ret;

    printk("Hello Led!\n");

    if (!device_is_ready(led.port)) {
        return;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

    k_timer_start(&my_timer, K_SECONDS(1), K_SECONDS(1));

    printk("Fim do main().\n");

}