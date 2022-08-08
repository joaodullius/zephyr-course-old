/*
 * Copyright (c) 2022 João Dullius
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <inttypes.h>
#include <errno.h>
#include <zephyr.h>
#include <sys/printk.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/gatt_dm.h>
#include <bluetooth/scan.h>
#include <settings/settings.h>

#define MANUFACTOR_LEN	4

static bool data_cb(struct bt_data *data, void *user_data)
{
	uint8_t *manufact_data = user_data;
	uint8_t len;

	switch (data->type) {
	case BT_DATA_MANUFACTURER_DATA:
		len = MIN(data->data_len, MANUFACTOR_LEN);
		memcpy(manufact_data, data->data, len);
		return false;
	default:
		return true;
	}
}

static void scan_filter_match(struct bt_scan_device_info *device_info,
			      struct bt_scan_filter_match *filter_match,
			      bool connectable)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(device_info->recv_info->addr, addr, sizeof(addr));
	// Parse advertising packet searching for advertising name.
	char manufact[MANUFACTOR_LEN];
	bt_data_parse(device_info->adv_data, data_cb, manufact);
		printk("Filters matched. Address: %s RSSI %d connectable: %s ",
											addr, device_info->recv_info->rssi,
											 connectable ? "yes" : "no");
		printk("manuf_data: %x|%x|%x|%x\n",
							manufact[0], manufact[1], manufact[2], manufact[3]);
		
}

BT_SCAN_CB_INIT(scan_cb, scan_filter_match, NULL,
		NULL, NULL);

static void scan_init(void)
{
	int err;

	struct bt_scan_init_param scan_init = {
		.connect_if_match = 0,
		.scan_param = NULL,
	};

	bt_scan_init(&scan_init);
	bt_scan_cb_register(&scan_cb);

	err = bt_scan_filter_add(BT_SCAN_FILTER_TYPE_UUID, BT_UUID_DECLARE_16(0XFEAA));
	if (err) {
		printk("Scanning filters cannot be set (err %d)\n", err);
		return;
	}

	err = bt_scan_filter_enable(BT_SCAN_UUID_FILTER, false);
	if (err) {
		printk("Filters cannot be turned on (err %d)\n", err);
	}
}


void main(void)
{
	int err;

	printk("Starting Bluetooth Scanner sample");

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	scan_init();

	err = bt_scan_start(BT_SCAN_TYPE_SCAN_ACTIVE);
	if (err) {
		printk("Scanning failed to start (err %d)\n", err);
		return;
	}

	printk("Scanning successfully started\n");
}
