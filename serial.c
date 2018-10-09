/*
 * iMX boot ROM plugin: debug serial port.
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

#include "config.h"

#define __REG(x)     (*((volatile uint32_t *)(x)))

#define UART_BAUD			115200

#if CFG_PLATFORM == PLATFORM_IMX6
#define UART_CLOCK			80000000
#define UART_PHYS			(0x02020000)
#elif CFG_PLATFORM == PLATFORM_IMX7
#define UART_CLOCK			24000000
#define UART_PHYS			(0x30860000)
#endif

#if CFG_PLATFORM == PLATFORM_IMX7
#define CCM_TARGET_ROOT_UART1	__REG(0x3038af80)
#define CCM_CCGR_UART1			__REG(0x30384940)
#define SW_MUX_CTL_PAD_UART1_RX_DATA	__REG(0x30330128)
#define SW_MUX_CTL_PAD_UART1_TX_DATA	__REG(0x3033012c)
#endif

#if CFG_PLATFORM == PLATFORM_IMX6
#define IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10	__REG(0x20E0280)
#define IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11	__REG(0x20E0284)
#define IOMUXC_UART1_IPP_UART_RXD_MUX_SELECT_INPUT	__REG(0x20E0920)
#endif

#define UART_UTXD			__REG(UART_PHYS + 0x40)
#define UART_UCR1			__REG(UART_PHYS + 0x80)
#define UART_UCR2			__REG(UART_PHYS + 0x84)
#define UART_UCR3			__REG(UART_PHYS + 0x88)
#define UART_UCR4			__REG(UART_PHYS + 0x8c)
#define UART_USR1			__REG(UART_PHYS + 0x94)
#define UART_USR2			__REG(UART_PHYS + 0x98)
#define UART_UESC			__REG(UART_PHYS + 0x9c)
#define UART_UFCR			__REG(UART_PHYS + 0x90)
#define UART_UTIM			__REG(UART_PHYS + 0xa0)
#define UART_UBIR			__REG(UART_PHYS + 0xa4)
#define UART_UBMR			__REG(UART_PHYS + 0xa8)
#define UART_UTS			__REG(UART_PHYS + 0xb4)

#define UCR1_UARTEN			(1<<0)

#define UCR2_IRTS			(1<<14)
#define UCR2_WS				(1<<5)
#define UCR2_TXEN			(1<<2)
#define UCR2_RXEN			(1<<1)
#define UCR2_SRST			(1<<0)
#define UCR3_RXDMUXSEL		(1<<2)
#define UCR3_ADNIMP			(1<<7)
#define UTS_TXEMPTY			(1<<6)
#define UFCR_RXTL_OFFS		0
#define UFCR_RFDIV_OFFS		7
#define UFCR_TXTL_OFFS		10

#define TXTL				2
#define RXTL				1
#define RFDIV				4	/* /2 */

///////////////////////////////////////////////////////////////////////////////
void dbg_init(void)
{
#if CFG_PLATFORM == PLATFORM_IMX6
	IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10 = 0x03;
	IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11 = 0x03;
	IOMUXC_UART1_IPP_UART_RXD_MUX_SELECT_INPUT = 0x01;
#endif

#if CFG_PLATFORM == PLATFORM_IMX7
	SW_MUX_CTL_PAD_UART1_RX_DATA = 0x0;
	SW_MUX_CTL_PAD_UART1_TX_DATA = 0x0;
	CCM_CCGR_UART1 = 0;
	CCM_TARGET_ROOT_UART1 = 0x10000000;
	CCM_CCGR_UART1 = 3;
#endif

	UART_UCR1 = 0x0; /* Disable */
	UART_UCR2 = 0x0; /* SWRST */
	while (!(UART_UCR2 & UCR2_SRST));

	UART_UCR3 |= UCR3_RXDMUXSEL | UCR3_ADNIMP;

	UART_UFCR = (RFDIV << UFCR_RFDIV_OFFS)
		| (TXTL << UFCR_TXTL_OFFS)
		| (RXTL << UFCR_RXTL_OFFS);
	UART_UBIR = 0xF;
	UART_UBMR = UART_CLOCK / (2 * UART_BAUD);
	UART_UCR2 = UCR2_WS | UCR2_IRTS | UCR2_RXEN | UCR2_TXEN | UCR2_SRST;
	UART_UCR1 = UCR1_UARTEN;
}

void dbg_chr(const char c)
{
	if (c == '\n')
		dbg_chr ('\r');

	UART_UTXD = c;
	while (!(UART_UTS & UTS_TXEMPTY));
}

void dbg_str(const char *str)
{
	while (*str != '\0') {
		dbg_chr(*str++);
	}
}
