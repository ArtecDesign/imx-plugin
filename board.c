/*
 * iMX boot ROM plugin: board configuration.
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

#define __REG(x)     	(*((volatile uint32_t *)(x)))
#define REG_LED_GPIO	__REG(0x020b0000)
#define LED_RED			0x00000004
#define LED_GREEN		0x00000010
#define LED_BLUE		0x00000001

#define REG_HWREV_GPIO	__REG(0x0209C008)
#define HWREV_BIT0		(1<<9)
#define HWREV_BIT1		(1<<12)
#define HWREV_BIT2		(1<<13)
#define HWREV_BIT3		(1<<15)

struct inittable {
	uint32_t reg;
	uint32_t val;
};

static const struct inittable init_clocks[] = {
	{ 0x020c4068, 0xffffffff },
	{ 0x020c406c, 0xffffffff },
	{ 0x020c4070, 0xffffffff },
	{ 0x020c4074, 0xffffffff },
	{ 0x020c4078, 0xffffffff },
	{ 0x020c407c, 0xffffffff },
	{ 0x020c4080, 0xffffffff },
	{ 0x020c4084, 0xffffffff },

	{ 0, 0 }
};

static const struct inittable init_iocon[] = {
	{ 0x020e05a8, 0x00000030 },
	{ 0x020e05b0, 0x00000030 },
	{ 0x020e0524, 0x00000030 },
	{ 0x020e051c, 0x00000030 },

	{ 0x020e0518, 0x00000030 },
	{ 0x020e050c, 0x00000030 },
	{ 0x020e05b8, 0x00000030 },
	{ 0x020e05c0, 0x00000030 },

	{ 0x020e05ac, 0x00020030 },
	{ 0x020e05b4, 0x00020030 },
	{ 0x020e0528, 0x00020030 },
	{ 0x020e0520, 0x00020030 },

	{ 0x020e0514, 0x00020030 },
	{ 0x020e0510, 0x00020030 },
	{ 0x020e05bc, 0x00020030 },
	{ 0x020e05c4, 0x00020030 },

	{ 0x020e056c, 0x00020030 },
	{ 0x020e0578, 0x00020030 },
	{ 0x020e0588, 0x00020030 },
	{ 0x020e0594, 0x00020030 },

	{ 0x020e057c, 0x00020030 },
	{ 0x020e0590, 0x00003000 },
	{ 0x020e0598, 0x00003000 },
	{ 0x020e058c, 0x00000000 },

	{ 0x020e059c, 0x00003030 },
	{ 0x020e05a0, 0x00003030 },
	{ 0x020e0784, 0x00000030 },
	{ 0x020e0788, 0x00000030 },

	{ 0x020e0794, 0x00000030 },
	{ 0x020e079c, 0x00000030 },
	{ 0x020e07a0, 0x00000030 },
	{ 0x020e07a4, 0x00000030 },

	{ 0x020e07a8, 0x00000030 },
	{ 0x020e0748, 0x00000030 },
	{ 0x020e074c, 0x00000030 },
	{ 0x020e0750, 0x00020000 },

	{ 0x020e0758, 0x00000000 },
	{ 0x020e0774, 0x00020000 },
	{ 0x020e078c, 0x00000030 },
	{ 0x020e0798, 0x000C0000 },

	{ 0, 0 }
};

/* DDR init for Sabre mx6q (4x mt41j128)*/
static const struct inittable init_ddr[] = {
	{ 0x021b081c, 0x33333333 },
	{ 0x021b0820, 0x33333333 },
	{ 0x021b0824, 0x33333333 },
	{ 0x021b0828, 0x33333333 },

	{ 0x021b481c, 0x33333333 },
	{ 0x021b4820, 0x33333333 },
	{ 0x021b4824, 0x33333333 },
	{ 0x021b4828, 0x33333333 },

	{ 0x021b0018, 0x00081740 },

	{ 0x021b001c, 0x00008000 },
	{ 0x021b000c, 0x555A7974 },
	{ 0x021b0010, 0xDB538F64 },
	{ 0x021b0014, 0x01FF00DB },
	{ 0x021b002c, 0x000026D2 },

	{ 0x021b0030, 0x005A1023 },
	{ 0x021b0008, 0x09444040 },
	{ 0x021b0004, 0x00025576 },
	{ 0x021b0040, 0x00000027 },
	{ 0x021b0000, 0x831A0000 },

	{ 0x021b001c, 0x04088032 },
	{ 0x021b001c, 0x0408803A },
	{ 0x021b001c, 0x00008033 },
	{ 0x021b001c, 0x0000803B },
	{ 0x021b001c, 0x00428031 },
	{ 0x021b001c, 0x00428039 },
	{ 0x021b001c, 0x19308030 },
	{ 0x021b001c, 0x19308038 },

	{ 0x021b001c, 0x04008040 },
	{ 0x021b001c, 0x04008048 },
	{ 0x021b0800, 0xA1380003 },
	{ 0x021b4800, 0xA1380003 },
	{ 0x021b0020, 0x00005800 },
	{ 0x021b0818, 0x00022227 },
	{ 0x021b4818, 0x00022227 },

	/* read dqs gating calibration */
	{ 0x021b083c, 0x434B0350 },
	{ 0x021b0840, 0x034C0359 },
	{ 0x021b483c, 0x434B0350 },
	{ 0x021b4840, 0x03650348 },

	/* read calibration */
	{ 0x021b0848, 0x4436383B },
	{ 0x021b4848, 0x39393341 },

	/* write calibration */
	{ 0x021b0850, 0x35373933 },
	{ 0x021b4850, 0x48254A36 },

	/* write levelling calibration */
	{ 0x021b080c, 0x001F001F },
	{ 0x021b0810, 0x001F001F },
	{ 0x021b480c, 0x00440044 },
	{ 0x021b4810, 0x00440044 },

	{ 0x021b08b8, 0x00000800 },
	{ 0x021b48b8, 0x00000800 },

	{ 0x021b001c, 0x00000000 },
	{ 0x021b0404, 0x00011006 },

	{ 0, 0 }
};

static const struct inittable init_finalize[] = {

	{ 0x020c4068, 0x00C03F3F },
	{ 0x020c406c, 0x0030FC03 },
	{ 0x020c4070, 0x0FFFC000 },
	{ 0x020c4074, 0x3FF00000 },
	{ 0x020c4078, 0x00FFF300 },
	{ 0x020c407c, 0x0F0000C3 },
	{ 0x020c4080, 0x000003FF },

	/* enable AXI cache for VDOA/VPU/IPU */
	{ 0x020e0010, 0xF00000CF },
	/* set IPU AXI-id0 Qos=0xf(bypass) AXI-id1 Qos=0x7 */
	{ 0x020e0018, 0x007F007F },
	{ 0x020e001c, 0x007F007F },

	/*
	 * Setup CCM_CCOSR register as follows:
	 *
	 * cko1_en  = 1	   --> CKO1 enabled
	 * cko1_div = 111  --> divide by 8
	 * cko1_sel = 1011 --> ahb_clk_root
	 *
	 * This sets CKO1 at ahb_clk_root/8 = 132/8 = 16.5 MHz
	 */
	{ 0x020c4060, 0x000000fb },

	{ 0, 0 }
};

static void init_from_table(const struct inittable *t) {
	while (t->reg) {
		volatile uint32_t *reg = (volatile uint32_t *)t->reg;
		*reg = t->val;
		t++;
	}
}

///////////////////////////////////////////////////////////////////////////////
void board_early_init_hw() {
	init_from_table(init_clocks);
	init_from_table(init_iocon);
}

int board_init_hw() {
	/* Detect the board configuration here and use the correct settings. */
	init_from_table(init_ddr);
	init_from_table(init_finalize);
	return 0;
}
