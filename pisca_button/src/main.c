/*
 * Copyright (c) 2022 Joao Dullius
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)
#define BUTTON0_NODE DT_NODELABEL(button0)

const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);
static struct gpio_callback button_cb_data;

void work_handler(struct k_work *work)
{
   gpio_pin_toggle_dt(&led);
}
K_WORK_DEFINE(my_work, work_handler);

void timer_handler(struct k_timer *timer_id)
{
    k_work_submit(&my_work);
}
K_TIMER_DEFINE(my_timer, timer_handler, NULL);

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	k_work_submit(&my_work);
}

void main(void)
{
    int ret;

    printk("Hello Pisca Button!\n");

    if (!device_is_ready(led.port)) {
        return;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

    //k_timer_start(&my_timer, K_SECONDS(1), K_SECONDS(1));

    printk("Fim do main().\n");

}