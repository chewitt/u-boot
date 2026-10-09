// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2023 SberDevices, Inc.
 */

#include <asm/arch/boot.h>
#include <asm/arch/mem.h>
#include <asm/arch/s4.h>
#include <asm/armv8/mmu.h>
#include <asm/io.h>
#include <linux/sizes.h>

int meson_get_boot_device(void)
{
	return (readl(S4_SEC_STATUS_REG2) & S4_BOOT_DEVICE_MASK)
		>> S4_BOOT_DEVICE_SHIFT;
}

/*
 * Reserve the BL31 and BL32 memory zones that the secure firmware reports in
 * SEC_STATUS_REG15 (sizes), REG16 (BL32 start) and REG17 (BL31 start). The
 * sizes are in KiB, or in 64KiB units when the top nibble of the BL31 size
 * field is set.
 */
void meson_init_reserved_memory(void *fdt)
{
	u64 bl31_size, bl31_start;
	u64 bl32_size, bl32_start;
	u64 unit = SZ_1K;
	u32 reg;

	reg = readl(S4_SEC_STATUS_REG15);
	if (reg & S4_RSVMEM_SIZE_64K_UNITS)
		unit = SZ_64K;

	bl31_size = ((reg & S4_BL31_RSVMEM_SIZE_MASK)
			>> S4_BL31_RSVMEM_SIZE_SHIFT) * unit;
	bl32_size = (reg & S4_BL32_RSVMEM_SIZE_MASK) * unit;

	bl31_start = readl(S4_SEC_STATUS_REG17);
	bl32_start = readl(S4_SEC_STATUS_REG16);

	if (bl31_start && bl31_size)
		meson_board_add_reserved_memory(fdt, bl31_start, bl31_size);

	if (bl32_start && bl32_size)
		meson_board_add_reserved_memory(fdt, bl32_start, bl32_size);
}

static struct mm_region s4_mem_map[] = {
	{
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0x80000000UL,
		.phys = 0x80000000UL,
		.size = 0x7FE00000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			PTE_BLOCK_NON_SHARE |
			PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		/*
		 * This mem region contains in/out shared memory with bl31,
		 * hence it's marked as NORMAL memory type
		 */
		.virt = 0xFFE00000UL,
		.phys = 0xFFE00000UL,
		.size = 0x00200000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			PTE_BLOCK_INNER_SHARE
	}, {
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = s4_mem_map;
