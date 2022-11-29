/*
 * Copyright (c) 2022 Joao Dullius
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)

const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define MY_STACK_SIZE 2048
#define MY_PRIORITY 5
K_THREAD_STACK_DEFINE(app_stack_area, MY_STACK_SIZE);
static struct k_work_q app_work_q;
static struct k_work my_work;

void work_handler(struct k_work *work)
{
   gpio_pin_toggle_dt(&led);
   printk("Piscando!\n");
}

void timer_handler(struct k_timer *timer_id)
{
    k_work_submit_to_queue(&app_work_q, &my_work);

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

    k_work_queue_start(&app_work_q, app_stack_area,
                        K_THREAD_STACK_SIZEOF(app_stack_area),
                        MY_PRIORITY, NULL);

    k_timer_start(&my_timer, K_SECONDS(1), K_SECONDS(1));

    printk("Fim do main().\n");

}