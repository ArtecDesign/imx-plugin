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
#include "config.h"

#define __REG(x)     	(*((volatile uint32_t *)(x)))

struct inittable {
	uint32_t reg;
	uint32_t val;
};

static void init_from_table(const struct inittable *t) {
	while (t->reg) {
		volatile uint32_t *reg = (volatile uint32_t *)t->reg;
		*reg = t->val;
		t++;
	}
}

///////////////////////////////////////////////////////////////////////////////
/* iMX6Q Sabre board, MCIMX6QSDB, sch revC4, brd revB */
#if CFG_PLATFORM == PLATFORM_IMX6
static const struct inittable init_clocks_mx6[] = {
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

static const struct inittable init_iocon_mx6[] = {
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
static const struct inittable init_ddr_sabre6q[] = {
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

static const struct inittable init_finalize_mx6[] = {

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

///////////////////////////////////////////////////////////////////////////////
void board_early_init_hw() {
	init_from_table(init_clocks_mx6);
	init_from_table(init_iocon_mx6);
}

int board_init_hw() {
	/* Detect the board configuration here and use the correct settings. */
	init_from_table(init_ddr_sabre6q);
	init_from_table(init_finalize_mx6);
	return 0;
}

#endif /* PLATFORM_IMX6 */

///////////////////////////////////////////////////////////////////////////////
/* iMX7D Sabre board MCIMX7SABRE, sch revD1, brd revD */
#if CFG_PLATFORM == PLATFORM_IMX7

#define CCM_ANALOG_PLL_DDR		__REG(0x30360070)
#define CCM_ANALOG_PLL_DDR_CLR	__REG(0x30360078)
#define CCM_ANALOG_PLL_DDR_NUM	__REG(0x30360090)

#define CCM_TARGET_ROOT49_DRAM	__REG(0x30389880)

#define CCM_CCGR19				__REG(0x30384130)
#define IOMUXC_GPR_GPR8			__REG(0x30340020)

static void board_mx7_init_clocks() {
	/* DDR PLL, /33 (400MHz), pwrdown */
	CCM_ANALOG_PLL_DDR = 0x00703021;
	CCM_ANALOG_PLL_DDR_NUM = 0;

	/* power up */
	CCM_ANALOG_PLL_DDR_CLR = 0x00100000;

	/* wait for lock */
	while ((CCM_ANALOG_PLL_DDR_CLR & 0x80000000) == 0);

	/* DRAM_CLK_ROOT = DDR_PLL / 2 */
	CCM_TARGET_ROOT49_DRAM = 0x1;
}

static const struct inittable config_ddr_sabre7d[] = {
	{0x30340004, 0x4F400005 }, // IOMUX GPR1: enable OCRAM GP

	/* Clear then set bit30 to ensure exit from DDR retention */
	{0x30360388, 0x40000000 },
	{0x30360384, 0x40000000 },

	/* SRC: reset DDRC */
	{0x30391000, 0x00000002 },

	/* Configure DDRC */
	{0x307a0000, 0x01040001 },
	{0x307a01a0, 0x80400003 },
	{0x307a01a4, 0x00100020 },
	{0x307a01a8, 0x80100004 },
	{0x307a0064, 0x00400046 },
	{0x307a0490, 0x00000001 },
	{0x307a00d0, 0x00020083 },
	{0x307a00d4, 0x00690000 },
	{0x307a00dc, 0x09300004 },
	{0x307a00e0, 0x04080000 },
	{0x307a00e4, 0x00100004 },
	{0x307a00f4, 0x0000033f },
	{0x307a0100, 0x09081109 },
	{0x307a0104, 0x0007020d },
	{0x307a0108, 0x03040407 },
	{0x307a010c, 0x00002006 },
	{0x307a0110, 0x04020205 },
	{0x307a0114, 0x03030202 },
	{0x307a0120, 0x00000803 },
	{0x307a0180, 0x00800020 },
	{0x307a0184, 0x02000100 },
	{0x307a0190, 0x02098204 },
	{0x307a0194, 0x00030303 },
	{0x307a0200, 0x00000016 },
	{0x307a0204, 0x00080808 },
	{0x307a0210, 0x00000f0f },
	{0x307a0214, 0x07070707 },
	{0x307a0218, 0x0f070707 },
	{0x307a0240, 0x06000604 },
	{0x307a0244, 0x00000001 },

	/* SRC: clear reset DDRC */
	{0x30391000, 0x00000000 },

	/* Configure DDRP */
	{0x30790000, 0x17420f40 },
	{0x30790004, 0x10210100 },
	{0x30790010, 0x00060807 },
	{0x307900b0, 0x1010007e },
	{0x3079009c, 0x00000dee },
	{0x3079007c, 0x18181818 },
	{0x30790080, 0x18181818 },
	{0x30790084, 0x40401818 },
	{0x30790088, 0x00000040 },
	{0x3079006c, 0x40404040 },
	{0x30790020, 0x08080808 },
	{0x30790030, 0x08080808 },
	{0x30790050, 0x01000010 },
	{0x30790050, 0x00000010 },

	{ 0, 0 }
};

static void board_mx7_ddrp_zq_cal() {
	__REG(0x307900c0)= 0x0e407304;
	__REG(0x307900c0)= 0x0e447304;
	__REG(0x307900c0)= 0x0e447306;

	while((__REG(0x307900c4) & 0x01) == 0);

	__REG(0x307900c0)= 0x0e407304;
}

static void board_mx7_init_ddr() {
	init_from_table(config_ddr_sabre7d);
	board_mx7_ddrp_zq_cal();

	/* disable clock */
	CCM_CCGR19 = 0x00000000;
	/* trigger ddrc init */
	IOMUXC_GPR_GPR8 = 0x00000178;
	/* enable clock */
	CCM_CCGR19 = 0x00000002;
	/* enable PHY DQS pulldowns */
	__REG(0x30790018) = 0x0000000f;

	/* wait until in normal mode */
	while ((__REG(0x307a0004) & 0x03) == 0x01);
}

///////////////////////////////////////////////////////////////////////////////
void board_early_init_hw() {
	board_mx7_init_clocks();
}

int board_init_hw() {
	board_mx7_init_ddr();
	return 0;
}

#endif /* PLATFORM_IMX7 */
