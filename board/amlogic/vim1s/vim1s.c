// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2026 Brivo Inc.
 */

#include <env.h>
#include <net-common.h>
#include <asm/arch/boot.h>
#include <asm/io.h>

#define SYSCTRL_SEC_STATUS_REG18	0xfe010348

int mmc_get_env_dev(void)
{
	switch (meson_get_boot_device()) {
	case BOOT_DEVICE_EMMC:
		return 0;
	case BOOT_DEVICE_SD:
		return 1;
	default:
		return -1;
	}
}

int misc_init_r(void)
{
	/* This procedure copied from Khadas's downstream fork */
	u32 val = readl(SYSCTRL_SEC_STATUS_REG18);
	unsigned char addr[ARP_HLEN] = {
		0x02,
		0xad,
		val >> 24,
		0x01,
		(val >> 8) & 0xff,
		val & 0xff,
	};

	return eth_env_set_enetaddr("ethaddr", addr);
}
