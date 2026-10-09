// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2026 Brivo Inc.
 */

#include <env.h>
#include <net-common.h>
#include <asm/arch/boot.h>
#include <asm/arch/s4.h>
#include <asm/io.h>

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
	u8 mac_addr[ARP_HLEN];
	u32 val;

	if (!eth_env_get_enetaddr("ethaddr", mac_addr)) {
		val = readl(S4_SEC_STATUS_REG18);
		mac_addr[0] = 0x02;
		mac_addr[1] = 0xad;
		mac_addr[2] = val >> 24;
		mac_addr[3] = 0x01;
		mac_addr[4] = (val >> 8) & 0xff;
		mac_addr[5] = val & 0xff;
		eth_env_set_enetaddr("ethaddr", mac_addr);
	}

	return 0;
}
