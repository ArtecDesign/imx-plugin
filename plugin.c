/*
 * iMX boot ROM plugin.
 *
 * Author: Anti Sullin <anti.sullin@artecdesign.ee>
 * Copyright (C) 2016 Artec Design LLC
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include <stdint.h>
#include <stddef.h>

#include "serial.h"
#include "board.h"
#include "config.h"

/* iMX boot ROM looks for iMX header at this offset */
#define FLASH_OFFSET				0x400

#define __REG(x)     (*((volatile uint32_t *)(x)))
#define __stringify_1(x...)				#x
#define __stringify(x...)				__stringify_1(x)

int plugin_download(void **start, uint32_t *bytes, uint32_t *ivt_offset);

///////////////////////////////////////////////////////////////////////////////
/* Plugin metadata from linker script
 * The value of a variable from linker script is the POINTER of a variable in C! */
extern uint8_t _plugin_start, _plugin_end, _plugin_size;

///////////////////////////////////////////////////////////////////////////////
struct ivt_header {
	uint8_t tag;
	uint8_t length[2];	/* big endian! */
	uint8_t version;
} __attribute__((packed));

struct boot_data {
	void* start;
	uint32_t size;
	uint32_t plugin;
} __attribute__((packed));

struct ivt {
	struct ivt_header header;
	void* entry;
	uint32_t reserved1;
	void* dcd_ptr;
	void* boot_data_ptr;
	void* self;
	uint32_t csf;
	uint32_t reserved2;
} __attribute__((packed));

struct flash_header {
	struct ivt ivt;
	struct boot_data boot;
};

/* iMX header of this plugin. The image shall be written at FLASH_OFFSET. */
__attribute__ ((used, section(".flash_header")))
		struct flash_header header = {
	.ivt = {
		.header = {
			.tag = 0xD1,
			.length = { 0x00, 0x20 },
			.version = 0x40,
		},
		.entry = plugin_download,
		.dcd_ptr = NULL,
		.boot_data_ptr = &header.boot,
		.self = &header.ivt,
	},
	.boot = {
		/* The flash read starts always at 0. Offset the dest pointer so that
		 * the data will end up at the right place. */
		.start = (&_plugin_start) - FLASH_OFFSET,
		/* Load extra 512 bytes to include the uboot flash header, which must be
		 * concatenated after this plugin image.
		 * Keep the size multiple of 512 bytes or the iMX6 SD loader will mess
		 * up the data! (iMX7 NAND loader does not have this problem) */
		.size = (uint32_t)(&_plugin_size) + FLASH_OFFSET + 512,
		.plugin = 1,
	},
};

///////////////////////////////////////////////////////////////////////////////
enum hab_status {
	HAB_STS_ANY = 0x00,
	HAB_FAILURE = 0x33,
	HAB_WARNING = 0x69,
	HAB_SUCCESS = 0xf0
};

typedef void (*pu_irom_hwcnfg_setup_t)(void **start, uint32_t *bytes, const void *boot_data);
typedef enum hab_status (*hab_rvt_entry_t)(void);
typedef void (*hab_failsafe_t)(void);

enum IMX_ROM_TYPE {
	IMX_ROM_UNIFIED,
#if CFG_PLATFORM == PLATFORM_IMX6
	IMX_ROM_LEGACY,
	IMX_ROM_DQL_NEW,	/* MX6DQ >= TO1.5; MX6DL >= TO1.2; MX6DQP */
#endif
	NUM_IMX_ROM_TYPES
};

/* iMX ROM has pointer tables, that contain pointers to some ROM functions we need.
 * Unfortunately, the ROM tables are at different locations in different SoC rev-s.
 * Information about these functions can be found in "High Assurance Boot Version 4
 * Application Programming Interface Reference Manual", which comes in the "Code
 * Signing Tool" archive. The pu_irom_hwcnfg_setup function is documented as
 * hab_loader_callback_f. */
struct imx_rom_ptrs {
	pu_irom_hwcnfg_setup_t *pu_irom_hwcnfg_setup;
	hab_rvt_entry_t	*hab_rvt_entry;
	hab_failsafe_t *hab_failsafe_t;
};

struct imx_rom_ptrs imx_rom_ptrs[NUM_IMX_ROM_TYPES] = {
	[IMX_ROM_UNIFIED] = {
		.pu_irom_hwcnfg_setup = (pu_irom_hwcnfg_setup_t*)(0x00000180 + 0x08),
		.hab_rvt_entry = (hab_rvt_entry_t*)(0x00000100 + 0x04),
		.hab_failsafe_t = (hab_failsafe_t*)(0x00000100 + 0x28),
	},
#if CFG_PLATFORM == PLATFORM_IMX6
	[IMX_ROM_LEGACY] = {
		.pu_irom_hwcnfg_setup = (pu_irom_hwcnfg_setup_t*)(0x000000C0 + 0x08),
		.hab_rvt_entry = (hab_rvt_entry_t*)(0x00000094 + 0x04),
		.hab_failsafe_t = (hab_failsafe_t*)(0x00000094 + 0x28),
	},
	[IMX_ROM_DQL_NEW] = {
		.pu_irom_hwcnfg_setup = (pu_irom_hwcnfg_setup_t*)(0x000000C4 + 0x08),
		.hab_rvt_entry = (hab_rvt_entry_t*)(0x00000098 + 0x04),
		.hab_failsafe_t = (hab_failsafe_t*)(0x00000098 + 0x28),
	},
#endif
};

///////////////////////////////////////////////////////////////////////////////
static enum IMX_ROM_TYPE get_rom_type() {
#if CFG_PLATFORM == PLATFORM_IMX7
	return IMX_ROM_UNIFIED;
#elif CFG_PLATFORM == PLATFORM_IMX6
	/* The "Chip Silicon Version" register is at USB analog PHY address space!
	 * Even this register is at different locations, we need to find it. */
	#define REG_ANATOP_DIGPROG		0x020C8260
	#define REG_ANATOP_DIGPROG_SL	0x020C8280

	uint32_t type = (__REG(REG_ANATOP_DIGPROG_SL) >> 16) & 0xFF;
	/* iMX6SL (solo light) */
	if (type == 0x60) return IMX_ROM_LEGACY;

	type = (__REG(REG_ANATOP_DIGPROG) >> 16) & 0xFF;
	/* iMX6SX? (solo x) */
	if (type == 0x62) return IMX_ROM_UNIFIED;
	/* iMX6DQ / iMXDQP (dual / quad / dual plus / quad plus) */
	if (type == 0x63) {
		uint32_t ver = __REG(REG_ANATOP_DIGPROG) & 0xFFFF;
		/* DQ >= TO1.5; DQP all */
		if (ver >= 0x05) return IMX_ROM_DQL_NEW;
		/* DQ < TO1.5 */
		return IMX_ROM_LEGACY;
	}
	/* iMX6DL (solo / dual lite) */
	if (type == 0x61) {
		uint32_t ver = __REG(REG_ANATOP_DIGPROG) & 0xFFFF;
		/* DL >= TO1.2 */
		if (ver >= 0x02) return IMX_ROM_DQL_NEW;
		/* DL < TO1.2 */
		return IMX_ROM_LEGACY;
	}
	return IMX_ROM_LEGACY;
#endif
}

///////////////////////////////////////////////////////////////////////////////
static int plugin_load_data(void **start, uint32_t *bytes, uint32_t *ivt_offset) {
	/* We assume, that a bootloader is concatenated after this plugin.
	 * Get pointer to the header of that bootloader. */
	struct flash_header *h = (struct flash_header *)&_plugin_end;

	/* Verify the header presence.
	 * Also, the boot_data should be located right after the header. */
	if (h->ivt.header.tag != 0xD1 ||
			h->ivt.boot_data_ptr - h->ivt.self != offsetof(struct flash_header, boot)) {
		return 0;
	}

	/* U-boot image does not know about the plugin, the plugin is just
	 * concatenated before the uboot image. Uboot image imx header specifies,
	 * that it is at offset 0x400 (FLASH_OFFSET), actually it is above that.
	 * Adjust the load pointers to compensate for this offset. */
	struct boot_data boot;
	boot.start = h->boot.start - (uint32_t)(&_plugin_size);
	boot.size = h->boot.size + (uint32_t)(&_plugin_size);
	boot.plugin = 0;

	/* The function pu_irom_hwcnfg_setup is supposed to "resume" loading.
	 * In: Pointer and size of "already loaded" data.
	 * Out: Pointer and size of completed data. */
	void* loaded_start = boot.start;
	uint32_t loaded_size = 0;

	/* Ask the ROM to load the bootloader to memory.
	 * ROM reads always starting from flash beginning!
	 * Actually, it copies over the data it has already loaded from
	 * SRAM buffer and then appends it as much as needed.
	 * iMX6 MMC: If the previous load was not multiple of MMC block size,
	 * the beginning of this will get corrupted! */
	struct imx_rom_ptrs *rom = &imx_rom_ptrs[get_rom_type()];
	(*rom->pu_irom_hwcnfg_setup)(&loaded_start, &loaded_size, &boot);

#if CFG_PLATFORM == PLATFORM_IMX6
	/* pu_irom_hwcnfg_setup forgets the L2 cache to enabled. Disable it. */
	__REG(0x00A02100) = 0;
#endif

	/* verify the image we got */
	struct flash_header *h2 = (struct flash_header *)(loaded_start + FLASH_OFFSET);
	if (loaded_size < boot.size || h2->ivt.header.tag != 0xD1) {
		return 0;
	}

	/* Return to ROM information with about the uboot image that is ready in SDRAM.
	 * The ROM will validate the image and run it. */
	*start = h->boot.start;
	*bytes = h->boot.size;
	*ivt_offset = FLASH_OFFSET;

	return 1;
}

static void plugin_fallthrough() {
	/* Call the failsafe handler to let the serial host upload the next part. */
	struct imx_rom_ptrs *rom = &imx_rom_ptrs[get_rom_type()];
	if ((*rom->hab_rvt_entry)() != HAB_SUCCESS) return;
	(*rom->hab_failsafe_t)();
}

/* Entry point from iMX boot ROM */
int plugin_download(void **start, uint32_t *bytes, uint32_t *ivt_offset) {
	board_early_init_hw();

	dbg_init();
	dbg_str("\n\niMX boot plugin, version " __stringify(VERSION) "\n");

	if (board_init_hw() != 0) {
		return 0;
	}

	/* If start pointer is not in SRAM, we're serial downloading. */
	if (start < (void**)0x00900000) {
		/* Go back to failsafe (serial loader) to continue loading. */
		plugin_fallthrough();
		return 0;
	}

	/* Load the next bootloader and let the ROM execute it. */
	return plugin_load_data(start, bytes, ivt_offset);
}
