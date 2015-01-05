/*
 * Copyright (c) 2014 MediaTek Inc.
 * Author: James Liao <jamesjj.liao@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/slab.h>

#include "clk-mtk.h"
#include "clk-pll.h"
#include "clk-gate.h"
#include "clk-mt8173-pll.h"

#include <dt-bindings/clock/mt8173-clk.h>

/* ROOT */
#define clk_null		"clk_null"
#define clk26m			"clk26m"
#define clk32k			"clk32k"

#define clkph_mck_o		"clkph_mck_o"
#define dpi_ck			"dpi_ck"
#define usb_syspll_125m		"usb_syspll_125m"
#define hdmitx_dig_cts		"hdmitx_dig_cts"

/* PLL */
#define armca15pll		"armca15pll"
#define armca7pll		"armca7pll"
#define mainpll			"mainpll"
#define univpll			"univpll"
#define mmpll			"mmpll"
#define msdcpll			"msdcpll"
#define vencpll			"vencpll"
#define tvdpll			"tvdpll"
#define mpll			"mpll"
#define vcodecpll		"vcodecpll"
#define apll1			"apll1"
#define apll2			"apll2"
#define lvdspll			"lvdspll"
#define msdcpll2		"msdcpll2"

#define armca7pll_754m		"armca7pll_754m"
#define armca7pll_502m		"armca7pll_502m"
#define apll1_180p633m		apll1
#define apll2_196p608m		apll2
#define mmpll_455m		mmpll
#define msdcpll_806m		msdcpll
#define main_h546m		"main_h546m"
#define main_h364m		"main_h364m"
#define main_h218p4m		"main_h218p4m"
#define main_h156m		"main_h156m"
#define tvdpll_445p5m		"tvdpll_445p5m"
#define tvdpll_594m		"tvdpll_594m"
#define univ_624m		"univ_624m"
#define univ_416m		"univ_416m"
#define univ_249p6m		"univ_249p6m"
#define univ_178p3m		"univ_178p3m"
#define univ_48m		"univ_48m"
#define vcodecpll_370p5		"vcodecpll_370p5"
#define vcodecpll_494m		vcodecpll
#define vencpll_380m		vencpll
#define lvdspll_ck		lvdspll

/* DIV */
#define clkrtc_ext		"clkrtc_ext"
#define clkrtc_int		"clkrtc_int"
#define fpc_ck			"fpc_ck"
#define hdmitxpll_d2		"hdmitxpll_d2"
#define hdmitxpll_d3		"hdmitxpll_d3"
#define armca7pll_d2		"armca7pll_d2"
#define armca7pll_d3		"armca7pll_d3"
#define apll1_ck		"apll1_ck"
#define apll2_ck		"apll2_ck"
#define dmpll_ck		"dmpll_ck"
#define dmpll_d2		"dmpll_d2"
#define dmpll_d4		"dmpll_d4"
#define dmpll_d8		"dmpll_d8"
#define dmpll_d16		"dmpll_d16"
#define lvdspll_d2		"lvdspll_d2"
#define lvdspll_d4		"lvdspll_d4"
#define lvdspll_d8		"lvdspll_d8"
#define mmpll_ck		"mmpll_ck"
#define mmpll_d2		"mmpll_d2"
#define msdcpll_ck		"msdcpll_ck"
#define msdcpll_d2		"msdcpll_d2"
#define msdcpll_d4		"msdcpll_d4"
#define msdcpll2_ck		"msdcpll2_ck"
#define msdcpll2_d2		"msdcpll2_d2"
#define msdcpll2_d4		"msdcpll2_d4"
#define ssusb_phyd_125m_ck	usb_syspll_125m
#define syspll_d2		"syspll_d2"
#define syspll1_d2		"syspll1_d2"
#define syspll1_d4		"syspll1_d4"
#define syspll1_d8		"syspll1_d8"
#define syspll1_d16		"syspll1_d16"
#define syspll_d3		"syspll_d3"
#define syspll2_d2		"syspll2_d2"
#define syspll2_d4		"syspll2_d4"
#define syspll_d5		"syspll_d5"
#define syspll3_d2		"syspll3_d2"
#define syspll3_d4		"syspll3_d4"
#define syspll_d7		"syspll_d7"
#define syspll4_d2		"syspll4_d2"
#define syspll4_d4		"syspll4_d4"
#define tvdpll_445p5m_ck	tvdpll_445p5m
#define tvdpll_ck		"tvdpll_ck"
#define tvdpll_d2		"tvdpll_d2"
#define tvdpll_d4		"tvdpll_d4"
#define tvdpll_d8		"tvdpll_d8"
#define tvdpll_d16		"tvdpll_d16"
#define univpll_d2		"univpll_d2"
#define univpll1_d2		"univpll1_d2"
#define univpll1_d4		"univpll1_d4"
#define univpll1_d8		"univpll1_d8"
#define univpll_d3		"univpll_d3"
#define univpll2_d2		"univpll2_d2"
#define univpll2_d4		"univpll2_d4"
#define univpll2_d8		"univpll2_d8"
#define univpll_d5		"univpll_d5"
#define univpll3_d2		"univpll3_d2"
#define univpll3_d4		"univpll3_d4"
#define univpll3_d8		"univpll3_d8"
#define univpll_d7		"univpll_d7"
#define univpll_d26		"univpll_d26"
#define univpll_d52		"univpll_d52"
#define vcodecpll_ck		"vcodecpll_ck"
#define vencpll_ck		"vencpll_ck"
#define vencpll_d2		"vencpll_d2"
#define vencpll_d4		"vencpll_d4"

/* TOP */
#define axi_sel			"axi_sel"
#define mem_sel			"mem_sel"
#define ddrphycfg_sel		"ddrphycfg_sel"
#define mm_sel			"mm_sel"
#define pwm_sel			"pwm_sel"
#define vdec_sel		"vdec_sel"
#define venc_sel		"venc_sel"
#define mfg_sel			"mfg_sel"
#define camtg_sel		"camtg_sel"
#define uart_sel		"uart_sel"
#define spi_sel			"spi_sel"
#define usb20_sel		"usb20_sel"
#define usb30_sel		"usb30_sel"
#define msdc50_0_h_sel		"msdc50_0_h_sel"
#define msdc50_0_sel		"msdc50_0_sel"
#define msdc30_1_sel		"msdc30_1_sel"
#define msdc30_2_sel		"msdc30_2_sel"
#define msdc30_3_sel		"msdc30_3_sel"
#define audio_sel		"audio_sel"
#define aud_intbus_sel		"aud_intbus_sel"
#define pmicspi_sel		"pmicspi_sel"
#define scp_sel			"scp_sel"
#define atb_sel			"atb_sel"
#define venclt_sel		"venclt_sel"
#define dpi0_sel		"dpi0_sel"
#define irda_sel		"irda_sel"
#define cci400_sel		"cci400_sel"
#define aud_1_sel		"aud_1_sel"
#define aud_2_sel		"aud_2_sel"
#define mem_mfg_in_sel		"mem_mfg_in_sel"
#define axi_mfg_in_sel		"axi_mfg_in_sel"
#define scam_sel		"scam_sel"
#define spinfi_ifr_sel		"spinfi_ifr_sel"
#define hdmi_sel		"hdmi_sel"
#define dpilvds_sel		"dpilvds_sel"
#define msdc50_2_h_sel		"msdc50_2_h_sel"
#define hdcp_sel		"hdcp_sel"
#define hdcp_24m_sel		"hdcp_24m_sel"
#define rtc_sel			"rtc_sel"

#define axi_ck			axi_sel
#define mfg_ck			mfg_sel

/* INFRA */
#define infra_pmicwrap		"infra_pmicwrap"
#define infra_pmicspi		"infra_pmicspi"
#define infra_cec		"infra_cec"
#define infra_kp		"infra_kp"
#define infra_cpum		"infra_cpum"
#define infra_m4u		"infra_m4u"
#define infra_l2c_sram		"infra_l2c_sram"
#define infra_gce		"infra_gce"
#define infra_audio		"infra_audio"
#define infra_smi		"infra_smi"
#define infra_dbgclk		"infra_dbgclk"

/* PERI0 */
#define peri_nfiecc		"peri_nfiecc"
#define peri_i2c5		"peri_i2c5"
#define peri_spi0		"peri_spi0"
#define peri_auxadc		"peri_auxadc"
#define peri_i2c4		"peri_i2c4"
#define peri_i2c3		"peri_i2c3"
#define peri_i2c2		"peri_i2c2"
#define peri_i2c1		"peri_i2c1"
#define peri_i2c0		"peri_i2c0"
#define peri_uart3		"peri_uart3"
#define peri_uart2		"peri_uart2"
#define peri_uart1		"peri_uart1"
#define peri_uart0		"peri_uart0"
#define peri_irda		"peri_irda"
#define peri_nli_arb		"peri_nli_arb"
#define peri_msdc30_3		"peri_msdc30_3"
#define peri_msdc30_2		"peri_msdc30_2"
#define peri_msdc30_1		"peri_msdc30_1"
#define peri_msdc30_0		"peri_msdc30_0"
#define peri_ap_dma		"peri_ap_dma"
#define peri_usb1		"peri_usb1"
#define peri_usb0		"peri_usb0"
#define peri_pwm		"peri_pwm"
#define peri_pwm7		"peri_pwm7"
#define peri_pwm6		"peri_pwm6"
#define peri_pwm5		"peri_pwm5"
#define peri_pwm4		"peri_pwm4"
#define peri_pwm3		"peri_pwm3"
#define peri_pwm2		"peri_pwm2"
#define peri_pwm1		"peri_pwm1"
#define peri_therm		"peri_therm"
#define peri_nfi		"peri_nfi"

/* PERI1 */
#define peri_i2c6		"peri_i2c6"
#define peri_irrx		"peri_irrx"
#define peri_spi		"peri_spi"

static DEFINE_SPINLOCK(lock);

static struct mtk_fixed_factor root_clk_alias[] __initdata = {
	FACTOR(TOP_CLKPH_MCK_O, clkph_mck_o, clk_null, 1, 1),
	FACTOR(TOP_DPI_CK, dpi_ck, clk_null, 1, 1),
	FACTOR(TOP_USB_SYSPLL_125M, usb_syspll_125m, clk_null, 1, 1),
	FACTOR(TOP_HDMITX_DIG_CTS, hdmitx_dig_cts, clk_null, 1, 1),
};

static struct mtk_fixed_factor top_divs[] __initdata = {
	FACTOR(TOP_ARMCA7PLL_754M, armca7pll_754m, armca7pll, 1, 2),
	FACTOR(TOP_ARMCA7PLL_502M, armca7pll_502m, armca7pll, 1, 3),

	FACTOR(TOP_MAIN_H546M, main_h546m, mainpll, 1, 2),
	FACTOR(TOP_MAIN_H364M, main_h364m, mainpll, 1, 3),
	FACTOR(TOP_MAIN_H218P4M, main_h218p4m, mainpll, 1, 5),
	FACTOR(TOP_MAIN_H156M, main_h156m, mainpll, 1, 7),

	FACTOR(TOP_TVDPLL_445P5M, tvdpll_445p5m, tvdpll, 1, 4),
	FACTOR(TOP_TVDPLL_594M, tvdpll_594m, tvdpll, 1, 3),

	FACTOR(TOP_UNIV_624M, univ_624m, univpll, 1, 2),
	FACTOR(TOP_UNIV_416M, univ_416m, univpll, 1, 3),
	FACTOR(TOP_UNIV_249P6M, univ_249p6m, univpll, 1, 5),
	FACTOR(TOP_UNIV_178P3M, univ_178p3m, univpll, 1, 7),
	FACTOR(TOP_UNIV_48M, univ_48m, univpll, 1, 26),

	FACTOR(TOP_CLKRTC_EXT, clkrtc_ext, clk32k, 1, 1),
	FACTOR(TOP_CLKRTC_INT, clkrtc_int, clk26m, 1, 793),
	FACTOR(TOP_FPC_CK, fpc_ck, clk26m, 1, 1),

	FACTOR(TOP_HDMITXPLL_D2, hdmitxpll_d2, hdmitx_dig_cts, 1, 2),
	FACTOR(TOP_HDMITXPLL_D3, hdmitxpll_d3, hdmitx_dig_cts, 1, 3),

	FACTOR(TOP_ARMCA7PLL_D2, armca7pll_d2, armca7pll_754m, 1, 1),
	FACTOR(TOP_ARMCA7PLL_D3, armca7pll_d3, armca7pll_502m, 1, 1),

	FACTOR(TOP_APLL1_CK, apll1_ck, apll1_180p633m, 1, 1),
	FACTOR(TOP_APLL2_CK, apll2_ck, apll2_196p608m, 1, 1),

	FACTOR(TOP_DMPLL_CK, dmpll_ck, clkph_mck_o, 1, 1),
	FACTOR(TOP_DMPLL_D2, dmpll_d2, clkph_mck_o, 1, 2),
	FACTOR(TOP_DMPLL_D4, dmpll_d4, clkph_mck_o, 1, 4),
	FACTOR(TOP_DMPLL_D8, dmpll_d8, clkph_mck_o, 1, 8),
	FACTOR(TOP_DMPLL_D16, dmpll_d16, clkph_mck_o, 1, 16),

	FACTOR(TOP_LVDSPLL_D2, lvdspll_d2, lvdspll, 1, 2),
	FACTOR(TOP_LVDSPLL_D4, lvdspll_d4, lvdspll, 1, 4),
	FACTOR(TOP_LVDSPLL_D8, lvdspll_d8, lvdspll, 1, 8),

	FACTOR(TOP_MMPLL_CK, mmpll_ck, mmpll_455m, 1, 1),
	FACTOR(TOP_MMPLL_D2, mmpll_d2, mmpll_455m, 1, 2),

	FACTOR(TOP_MSDCPLL_CK, msdcpll_ck, msdcpll_806m, 1, 1),
	FACTOR(TOP_MSDCPLL_D2, msdcpll_d2, msdcpll_806m, 1, 2),
	FACTOR(TOP_MSDCPLL_D4, msdcpll_d4, msdcpll_806m, 1, 4),
	FACTOR(TOP_MSDCPLL2_CK, msdcpll2_ck, msdcpll2, 1, 1),
	FACTOR(TOP_MSDCPLL2_D2, msdcpll2_d2, msdcpll2, 1, 2),
	FACTOR(TOP_MSDCPLL2_D4, msdcpll2_d4, msdcpll2, 1, 4),

	FACTOR(TOP_SYSPLL_D2, syspll_d2, main_h546m, 1, 1),
	FACTOR(TOP_SYSPLL1_D2, syspll1_d2, main_h546m, 1, 2),
	FACTOR(TOP_SYSPLL1_D4, syspll1_d4, main_h546m, 1, 4),
	FACTOR(TOP_SYSPLL1_D8, syspll1_d8, main_h546m, 1, 8),
	FACTOR(TOP_SYSPLL1_D16, syspll1_d16, main_h546m, 1, 16),
	FACTOR(TOP_SYSPLL_D3, syspll_d3, main_h364m, 1, 1),
	FACTOR(TOP_SYSPLL2_D2, syspll2_d2, main_h364m, 1, 2),
	FACTOR(TOP_SYSPLL2_D4, syspll2_d4, main_h364m, 1, 4),
	FACTOR(TOP_SYSPLL_D5, syspll_d5, main_h218p4m, 1, 1),
	FACTOR(TOP_SYSPLL3_D2, syspll3_d2, main_h218p4m, 1, 2),
	FACTOR(TOP_SYSPLL3_D4, syspll3_d4, main_h218p4m, 1, 4),
	FACTOR(TOP_SYSPLL_D7, syspll_d7, main_h156m, 1, 1),
	FACTOR(TOP_SYSPLL4_D2, syspll4_d2, main_h156m, 1, 2),
	FACTOR(TOP_SYSPLL4_D4, syspll4_d4, main_h156m, 1, 4),

	FACTOR(TOP_TVDPLL_CK, tvdpll_ck, tvdpll_594m, 1, 1),
	FACTOR(TOP_TVDPLL_D2, tvdpll_d2, tvdpll_594m, 1, 2),
	FACTOR(TOP_TVDPLL_D4, tvdpll_d4, tvdpll_594m, 1, 4),
	FACTOR(TOP_TVDPLL_D8, tvdpll_d8, tvdpll_594m, 1, 8),
	FACTOR(TOP_TVDPLL_D16, tvdpll_d16, tvdpll_594m, 1, 16),

	FACTOR(TOP_UNIVPLL_D2, univpll_d2, univ_624m, 1, 1),
	FACTOR(TOP_UNIVPLL1_D2, univpll1_d2, univ_624m, 1, 2),
	FACTOR(TOP_UNIVPLL1_D4, univpll1_d4, univ_624m, 1, 4),
	FACTOR(TOP_UNIVPLL1_D8, univpll1_d8, univ_624m, 1, 8),
	FACTOR(TOP_UNIVPLL_D3, univpll_d3, univ_416m, 1, 1),
	FACTOR(TOP_UNIVPLL2_D2, univpll2_d2, univ_416m, 1, 2),
	FACTOR(TOP_UNIVPLL2_D4, univpll2_d4, univ_416m, 1, 4),
	FACTOR(TOP_UNIVPLL2_D8, univpll2_d8, univ_416m, 1, 8),
	FACTOR(TOP_UNIVPLL_D5, univpll_d5, univ_249p6m, 1, 1),
	FACTOR(TOP_UNIVPLL3_D2, univpll3_d2, univ_249p6m, 1, 2),
	FACTOR(TOP_UNIVPLL3_D4, univpll3_d4, univ_249p6m, 1, 4),
	FACTOR(TOP_UNIVPLL3_D8, univpll3_d8, univ_249p6m, 1, 8),
	FACTOR(TOP_UNIVPLL_D7, univpll_d7, univ_178p3m, 1, 1),
	FACTOR(TOP_UNIVPLL_D26, univpll_d26, univ_48m, 1, 1),
	FACTOR(TOP_UNIVPLL_D52, univpll_d52, univ_48m, 1, 2),

	FACTOR(TOP_VCODECPLL_CK, vcodecpll_ck, vcodecpll, 1, 3),
	FACTOR(TOP_VCODECPLL_370P5, vcodecpll_370p5, vcodecpll, 1, 4),

	FACTOR(TOP_VENCPLL_CK, vencpll_ck, vencpll_380m, 1, 1),
	FACTOR(TOP_VENCPLL_D2, vencpll_d2, vencpll_380m, 1, 2),
	FACTOR(TOP_VENCPLL_D4, vencpll_d4, vencpll_380m, 1, 4),
};

static const char *axi_parents[] __initconst = {
		clk26m,
		syspll1_d2,
		syspll_d5,
		syspll1_d4,
		univpll_d5,
		univpll2_d2,
		dmpll_d2,
		dmpll_d4};

static const char *mem_parents[] __initconst = {
		clk26m,
		dmpll_ck};

static const char *ddrphycfg_parents[] __initconst = {
		clk26m,
		syspll1_d8};

static const char *mm_parents[] __initconst = {
		clk26m,
		vencpll_d2,
		main_h364m,
		syspll1_d2,
		syspll_d5,
		syspll1_d4,
		univpll1_d2,
		univpll2_d2,
		dmpll_d2};

static const char *pwm_parents[] __initconst = {
		clk26m,
		univpll2_d4,
		univpll3_d2,
		univpll1_d4};

static const char *vdec_parents[] __initconst = {
		clk26m,
		vcodecpll_ck,
		tvdpll_445p5m_ck,
		univpll_d3,
		vencpll_d2,
		syspll_d3,
		univpll1_d2,
		mmpll_d2,
		dmpll_d2,
		dmpll_d4};

static const char *venc_parents[] __initconst = {
		clk26m,
		vcodecpll_ck,
		tvdpll_445p5m_ck,
		univpll_d3,
		vencpll_d2,
		syspll_d3,
		univpll1_d2,
		univpll2_d2,
		dmpll_d2,
		dmpll_d4};

static const char *mfg_parents[] __initconst = {
		clk26m,
		mmpll_ck,
		dmpll_ck,
		clk26m,
		clk26m,
		clk26m,
		clk26m,
		clk26m,
		clk26m,
		syspll_d3,
		syspll1_d2,
		syspll_d5,
		univpll_d3,
		univpll1_d2,
		univpll_d5,
		univpll2_d2};

static const char *camtg_parents[] __initconst = {
		clk26m,
		univpll_d26,
		univpll2_d2,
		syspll3_d2,
		syspll3_d4,
		univpll1_d4};

static const char *uart_parents[] __initconst = {
		clk26m,
		univpll2_d8};

static const char *spi_parents[] __initconst = {
		clk26m,
		syspll3_d2,
		syspll1_d4,
		syspll4_d2,
		univpll3_d2,
		univpll2_d4,
		univpll1_d8};

static const char *usb20_parents[] __initconst = {
		clk26m,
		univpll1_d8,
		univpll3_d4};

static const char *usb30_parents[] __initconst = {
		clk26m,
		univpll3_d2,
		ssusb_phyd_125m_ck,
		univpll2_d4};

static const char *msdc50_0_h_parents[] __initconst = {
		clk26m,
		syspll1_d2,
		syspll2_d2,
		syspll4_d2,
		univpll_d5,
		univpll1_d4};

static const char *msdc50_0_parents[] __initconst = {
		clk26m,
		msdcpll_ck,
		msdcpll_d2,
		univpll1_d4,
		syspll2_d2,
		syspll_d7,
		msdcpll_d4,
		vencpll_d4,
		tvdpll_ck,
		univpll_d2,
		univpll1_d2,
		mmpll_ck,
		msdcpll2_ck,
		msdcpll2_d2,
		msdcpll2_d4};

static const char *msdc30_1_parents[] __initconst = {
		clk26m,
		univpll2_d2,
		msdcpll_d4,
		univpll1_d4,
		syspll2_d2,
		syspll_d7,
		univpll_d7,
		vencpll_d4};

static const char *msdc30_2_parents[] __initconst = {
		clk26m,
		univpll2_d2,
		msdcpll_d4,
		univpll1_d4,
		syspll2_d2,
		syspll_d7,
		univpll_d7,
		vencpll_d2};

static const char *msdc30_3_parents[] __initconst = {
		clk26m,
		msdcpll2_ck,
		msdcpll2_d2,
		univpll2_d2,
		msdcpll2_d4,
		msdcpll_d4,
		univpll1_d4,
		syspll2_d2,
		syspll_d7,
		univpll_d7,
		vencpll_d4,
		msdcpll_ck,
		msdcpll_d2,
		msdcpll_d4};

static const char *audio_parents[] __initconst = {
		clk26m,
		syspll3_d4,
		syspll4_d4,
		syspll1_d16};

static const char *aud_intbus_parents[] __initconst = {
		clk26m,
		syspll1_d4,
		syspll4_d2,
		univpll3_d2,
		univpll2_d8,
		dmpll_d4,
		dmpll_d8};

static const char *pmicspi_parents[] __initconst = {
		clk26m,
		syspll1_d8,
		syspll3_d4,
		syspll1_d16,
		univpll3_d4,
		univpll_d26,
		dmpll_d8,
		dmpll_d16};

static const char *scp_parents[] __initconst = {
		clk26m,
		syspll1_d2,
		univpll_d5,
		syspll_d5,
		dmpll_d2,
		dmpll_d4};

static const char *atb_parents[] __initconst = {
		clk26m,
		syspll1_d2,
		univpll_d5,
		dmpll_d2};

static const char *venc_lt_parents[] __initconst = {
		clk26m,
		univpll_d3,
		vcodecpll_ck,
		tvdpll_445p5m_ck,
		vencpll_d2,
		syspll_d3,
		univpll1_d2,
		univpll2_d2,
		syspll1_d2,
		univpll_d5,
		vcodecpll_370p5,
		dmpll_ck};

static const char *dpi0_parents[] __initconst = {
		clk26m,
		tvdpll_d2,
		tvdpll_d4,
		clk26m,
		clk26m,
		tvdpll_d8,
		tvdpll_d16};

static const char *irda_parents[] __initconst = {
		clk26m,
		univpll2_d4,
		syspll2_d4};

static const char *cci400_parents[] __initconst = {
		clk26m,
		vencpll_ck,
		armca7pll_754m,
		armca7pll_502m,
		univpll_d2,
		syspll_d2,
		msdcpll_ck,
		dmpll_ck};

static const char *aud_1_parents[] __initconst = {
		clk26m,
		apll1_ck,
		univpll2_d4,
		univpll2_d8};

static const char *aud_2_parents[] __initconst = {
		clk26m,
		apll2_ck,
		univpll2_d4,
		univpll2_d8};

static const char *mem_mfg_in_parents[] __initconst = {
		clk26m,
		mmpll_ck,
		dmpll_ck,
		clk26m};

static const char *axi_mfg_in_parents[] __initconst = {
		clk26m,
		axi_ck,
		dmpll_d2};

static const char *scam_parents[] __initconst = {
		clk26m,
		syspll3_d2,
		univpll2_d4,
		dmpll_d4};

static const char *spinfi_ifr_parents[] __initconst = {
		clk26m,
		univpll2_d8,
		univpll3_d4,
		syspll4_d2,
		univpll2_d4,
		univpll3_d2,
		syspll1_d4,
		univpll1_d4};

static const char *hdmi_parents[] __initconst = {
		clk26m,
		hdmitx_dig_cts,
		hdmitxpll_d2,
		hdmitxpll_d3};

static const char *dpilvds_parents[] __initconst = {
		clk26m,
		lvdspll_ck,
		lvdspll_d2,
		lvdspll_d4,
		lvdspll_d8,
		fpc_ck};

static const char *msdc50_2_h_parents[] __initconst = {
		clk26m,
		syspll1_d2,
		syspll2_d2,
		syspll4_d2,
		univpll_d5,
		univpll1_d4};

static const char *hdcp_parents[] __initconst = {
		clk26m,
		syspll4_d2,
		syspll3_d4,
		univpll2_d4};

static const char *hdcp_24m_parents[] __initconst = {
		clk26m,
		univpll_d26,
		univpll_d52,
		univpll2_d8};

static const char *rtc_parents[] __initconst = {
		clkrtc_int,
		clkrtc_ext,
		clk26m,
		univpll3_d8};

static struct mtk_mux top_muxes[] __initdata = {
	/* CLK_CFG_0 */
	MUX(TOP_AXI_SEL, axi_sel, axi_parents,
		0x0040, 0, 3, INVALID_MUX_GATE_BIT /* 7 */),
	MUX(TOP_MEM_SEL, mem_sel, mem_parents,
		0x0040, 8, 1, INVALID_MUX_GATE_BIT /* 15 */),
	MUX(TOP_DDRPHYCFG_SEL, ddrphycfg_sel, ddrphycfg_parents,
		0x0040, 16, 1, 23),
	MUX(TOP_MM_SEL, mm_sel, mm_parents, 0x0040, 24, 4, 31),
	/* CLK_CFG_1 */
	MUX(TOP_PWM_SEL, pwm_sel, pwm_parents, 0x0050, 0, 2, 7),
	MUX(TOP_VDEC_SEL, vdec_sel, vdec_parents, 0x0050, 8, 4, 15),
	MUX(TOP_VENC_SEL, venc_sel, venc_parents, 0x0050, 16, 4, 23),
	MUX(TOP_MFG_SEL, mfg_sel, mfg_parents, 0x0050, 24, 4, 31),
	/* CLK_CFG_2 */
	MUX(TOP_CAMTG_SEL, camtg_sel, camtg_parents, 0x0060, 0, 3, 7),
	MUX(TOP_UART_SEL, uart_sel, uart_parents, 0x0060, 8, 1, 15),
	MUX(TOP_SPI_SEL, spi_sel, spi_parents, 0x0060, 16, 3, 23),
	MUX(TOP_USB20_SEL, usb20_sel, usb20_parents, 0x0060, 24, 2, 31),
	/* CLK_CFG_3 */
	MUX(TOP_USB30_SEL, usb30_sel, usb30_parents, 0x0070, 0, 2, 7),
	MUX(TOP_MSDC50_0_H_SEL, msdc50_0_h_sel, msdc50_0_h_parents,
		0x0070, 8, 3, 15),
	MUX(TOP_MSDC50_0_SEL, msdc50_0_sel, msdc50_0_parents,
		0x0070, 16, 4, 23),
	MUX(TOP_MSDC30_1_SEL, msdc30_1_sel, msdc30_1_parents,
		0x0070, 24, 3, 31),
	/* CLK_CFG_4 */
	MUX(TOP_MSDC30_2_SEL, msdc30_2_sel, msdc30_2_parents, 0x0080, 0, 3, 7),
	MUX(TOP_MSDC30_3_SEL, msdc30_3_sel, msdc30_3_parents, 0x0080, 8, 4, 15),
	MUX(TOP_AUDIO_SEL, audio_sel, audio_parents, 0x0080, 16, 2, 23),
	MUX(TOP_AUD_INTBUS_SEL, aud_intbus_sel, aud_intbus_parents,
		0x0080, 24, 3, 31),
	/* CLK_CFG_5 */
	MUX(TOP_PMICSPI_SEL, pmicspi_sel, pmicspi_parents,
		0x0090, 0, 3, 7 /* 7:5 */),
	MUX(TOP_SCP_SEL, scp_sel, scp_parents, 0x0090, 8, 3, 15),
	MUX(TOP_ATB_SEL, atb_sel, atb_parents, 0x0090, 16, 2, 23),
	MUX(TOP_VENC_LT_SEL, venclt_sel, venc_lt_parents, 0x0090, 24, 4, 31),
	/* CLK_CFG_6 */
	MUX(TOP_DPI0_SEL, dpi0_sel, dpi0_parents, 0x00a0, 0, 3, 7),
	MUX(TOP_IRDA_SEL, irda_sel, irda_parents, 0x00a0, 8, 2, 15),
	MUX(TOP_CCI400_SEL, cci400_sel, cci400_parents, 0x00a0, 16, 3, 23),
	MUX(TOP_AUD_1_SEL, aud_1_sel, aud_1_parents, 0x00a0, 24, 2, 31),
	/* CLK_CFG_7 */
	MUX(TOP_AUD_2_SEL, aud_2_sel, aud_2_parents, 0x00b0, 0, 2, 7),
	MUX(TOP_MEM_MFG_IN_SEL, mem_mfg_in_sel, mem_mfg_in_parents,
		0x00b0, 8, 2, 15),
	MUX(TOP_AXI_MFG_IN_SEL, axi_mfg_in_sel, axi_mfg_in_parents,
		0x00b0, 16, 2, 23),
	MUX(TOP_SCAM_SEL, scam_sel, scam_parents, 0x00b0, 24, 2, 31),
	/* CLK_CFG_12 */
	MUX(TOP_SPINFI_IFR_SEL, spinfi_ifr_sel, spinfi_ifr_parents,
		0x00c0, 0, 3, 7),
	MUX(TOP_HDMI_SEL, hdmi_sel, hdmi_parents, 0x00c0, 8, 2, 15),
	MUX(TOP_DPILVDS_SEL, dpilvds_sel, dpilvds_parents, 0x00c0, 24, 3, 31),
	/* CLK_CFG_13 */
	MUX(TOP_MSDC50_2_H_SEL, msdc50_2_h_sel, msdc50_2_h_parents,
		0x00d0, 0, 3, 7),
	MUX(TOP_HDCP_SEL, hdcp_sel, hdcp_parents, 0x00d0, 8, 2, 15),
	MUX(TOP_HDCP_24M_SEL, hdcp_24m_sel, hdcp_24m_parents,
		0x00d0, 16, 2, 23),
	MUX(TOP_RTC_SEL, rtc_sel, rtc_parents,
		0x00d0, 24, 2, INVALID_MUX_GATE_BIT /* 31 */),
};

static void __init mtk_init_clk_topckgen(void __iomem *top_base,
		struct clk_onecell_data *clk_data)
{
	int i;
	struct clk *clk;

	for (i = 0; i < ARRAY_SIZE(top_muxes); i++) {
		struct mtk_mux *mux = &top_muxes[i];

		clk = mtk_clk_register_mux(mux->name,
			mux->parent_names, mux->num_parents,
			top_base + mux->reg, mux->shift, mux->width,
			mux->gate, &lock);

		if (IS_ERR(clk)) {
			pr_err("Failed to register clk %s: %ld\n",
					mux->name, PTR_ERR(clk));
			continue;
		}

		if (clk_data)
			clk_data->clks[mux->id] = clk;
	}
}

static struct mtk_pll plls[] __initdata = {
	PLL(APMIXED_ARMCA15PLL, armca15pll, clk26m, 0x0200, 0x020c, 0x00000001,
		HAVE_PLL_HP, &mt8173_arm_pll_ops),
	PLL(APMIXED_ARMCA7PLL, armca7pll, clk26m, 0x0210, 0x021c, 0x00000001,
		HAVE_PLL_HP, &mt8173_arm_pll_ops),
	PLL(APMIXED_MAINPLL, mainpll, clk26m, 0x0220, 0x022c, 0xf0000101,
		HAVE_PLL_HP | HAVE_RST_BAR, &mt8173_sdm_pll_ops),
	PLL(APMIXED_UNIVPLL, univpll, clk26m, 0x0230, 0x023c, 0xfe000001,
		HAVE_RST_BAR | HAVE_FIX_FRQ | PLL_AO, &mt8173_univ_pll_ops),
	PLL(APMIXED_MMPLL, mmpll, clk26m, 0x0240, 0x024c, 0x00000001,
		HAVE_PLL_HP, &mt8173_mm_pll_ops),
	PLL(APMIXED_MSDCPLL, msdcpll, clk26m, 0x0250, 0x025c, 0x00000001,
		HAVE_PLL_HP, &mt8173_sdm_pll_ops),
	PLL(APMIXED_VENCPLL, vencpll, clk26m, 0x0260, 0x026c, 0x00000001,
		HAVE_PLL_HP, &mt8173_sdm_pll_ops),
	PLL(APMIXED_TVDPLL, tvdpll, clk26m, 0x0270, 0x027c, 0x00000001,
		HAVE_PLL_HP, &mt8173_sdm_pll_ops),
	PLL(APMIXED_MPLL, mpll, clk26m, 0x0280, 0x028c, 0x00000001,
		HAVE_PLL_HP, &mt8173_sdm_pll_ops),
	PLL(APMIXED_VCODECPLL, vcodecpll, clk26m, 0x0290, 0x029c, 0x00000001,
		HAVE_PLL_HP, &mt8173_sdm_pll_ops),
	PLL(APMIXED_APLL1, apll1, clk26m, 0x02a0, 0x02b0, 0x00000001,
		HAVE_PLL_HP, &mt8173_aud_pll_ops),
	PLL(APMIXED_APLL2, apll2, clk26m, 0x02b4, 0x02c4, 0x00000001,
		HAVE_PLL_HP, &mt8173_aud_pll_ops),
	PLL(APMIXED_LVDSPLL, lvdspll, clk26m, 0x02d0, 0x02dc, 0x00000001,
		HAVE_PLL_HP, &mt8173_sdm_pll_ops),
	PLL(APMIXED_MSDCPLL2, msdcpll2, clk26m, 0x02f0, 0x02fc, 0x00000001,
		HAVE_PLL_HP, &mt8173_sdm_pll_ops),
};

static void __init mtk_init_clk_apmixedsys(void __iomem *apmixed_base,
		struct clk_onecell_data *clk_data)
{
	int i;
	struct clk *clk;

	for (i = 0; i < ARRAY_SIZE(plls); i++) {
		struct mtk_pll *pll = &plls[i];

		clk = mtk_clk_register_pll(pll->name, pll->parent_name,
				apmixed_base + pll->reg,
				apmixed_base + pll->pwr_reg,
				pll->en_mask, pll->flags, pll->ops, &lock);

		if (IS_ERR(clk)) {
			pr_err("Failed to register clk %s: %ld\n",
					pll->name, PTR_ERR(clk));
			continue;
		}

		if (clk_data)
			clk_data->clks[pll->id] = clk;
	}
}

static struct mtk_gate_regs infra_cg_regs = {
	.set_ofs = 0x0040,
	.clr_ofs = 0x0044,
	.sta_ofs = 0x0048,
};

static struct mtk_gate infra_clks[] __initdata = {
	GATE(INFRA_DBGCLK, infra_dbgclk, axi_sel, infra_cg_regs,
		0, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_SMI, infra_smi, mm_sel, infra_cg_regs,
		1, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_AUDIO, infra_audio, aud_intbus_sel, infra_cg_regs,
		5, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_GCE, infra_gce, axi_sel, infra_cg_regs,
		6, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_L2C_SRAM, infra_l2c_sram, axi_sel, infra_cg_regs,
		7, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_M4U, infra_m4u, mem_sel, infra_cg_regs,
		8, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_CPUM, infra_cpum, clk_null, infra_cg_regs,
		15, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_KP, infra_kp, axi_sel, infra_cg_regs,
		16, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_CEC, infra_cec, clk26m, infra_cg_regs,
		18, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_PMICSPI, infra_pmicspi, pmicspi_sel, infra_cg_regs,
		22, &mtk_clk_gate_ops_setclr),
	GATE(INFRA_PMICWRAP, infra_pmicwrap, axi_sel, infra_cg_regs,
		23, &mtk_clk_gate_ops_setclr),
};

static struct mtk_gate_regs peri0_cg_regs = {
	.set_ofs = 0x0008,
	.clr_ofs = 0x0010,
	.sta_ofs = 0x0018,
};

static struct mtk_gate_regs peri1_cg_regs = {
	.set_ofs = 0x000c,
	.clr_ofs = 0x0014,
	.sta_ofs = 0x001c,
};

static struct mtk_gate peri_clks[] __initdata = {
	/* PERI0 */
	GATE(PERI_NFI, peri_nfi, axi_sel, peri0_cg_regs,
		0, &mtk_clk_gate_ops_setclr),
	GATE(PERI_THERM, peri_therm, axi_sel, peri0_cg_regs,
		1, &mtk_clk_gate_ops_setclr),
	GATE(PERI_PWM1, peri_pwm1, axi_sel, peri0_cg_regs,
		2, &mtk_clk_gate_ops_setclr),
	GATE(PERI_PWM2, peri_pwm2, axi_sel, peri0_cg_regs,
		3, &mtk_clk_gate_ops_setclr),
	GATE(PERI_PWM3, peri_pwm3, axi_sel, peri0_cg_regs,
		4, &mtk_clk_gate_ops_setclr),
	GATE(PERI_PWM4, peri_pwm4, axi_sel, peri0_cg_regs,
		5, &mtk_clk_gate_ops_setclr),
	GATE(PERI_PWM5, peri_pwm5, axi_sel, peri0_cg_regs,
		6, &mtk_clk_gate_ops_setclr),
	GATE(PERI_PWM6, peri_pwm6, axi_sel, peri0_cg_regs,
		7, &mtk_clk_gate_ops_setclr),
	GATE(PERI_PWM7, peri_pwm7, axi_sel, peri0_cg_regs,
		8, &mtk_clk_gate_ops_setclr),
	GATE(PERI_PWM, peri_pwm, axi_sel, peri0_cg_regs,
		9, &mtk_clk_gate_ops_setclr),
	GATE(PERI_USB0, peri_usb0, usb20_sel, peri0_cg_regs,
		10, &mtk_clk_gate_ops_setclr),
	GATE(PERI_USB1, peri_usb1, usb20_sel, peri0_cg_regs,
		11, &mtk_clk_gate_ops_setclr),
	GATE(PERI_AP_DMA, peri_ap_dma, axi_sel, peri0_cg_regs,
		12, &mtk_clk_gate_ops_setclr),
	GATE(PERI_MSDC30_0, peri_msdc30_0, msdc50_0_sel, peri0_cg_regs,
		13, &mtk_clk_gate_ops_setclr),
	GATE(PERI_MSDC30_1, peri_msdc30_1, msdc30_1_sel, peri0_cg_regs,
		14, &mtk_clk_gate_ops_setclr),
	GATE(PERI_MSDC30_2, peri_msdc30_2, msdc30_2_sel, peri0_cg_regs,
		15, &mtk_clk_gate_ops_setclr),
	GATE(PERI_MSDC30_3, peri_msdc30_3, msdc30_3_sel, peri0_cg_regs,
		16, &mtk_clk_gate_ops_setclr),
	GATE(PERI_NLI_ARB, peri_nli_arb, axi_sel, peri0_cg_regs,
		17, &mtk_clk_gate_ops_setclr),
	GATE(PERI_IRDA, peri_irda, irda_sel, peri0_cg_regs,
		18, &mtk_clk_gate_ops_setclr),
	GATE(PERI_UART0, peri_uart0, uart_sel, peri0_cg_regs,
		19, &mtk_clk_gate_ops_setclr),
	GATE(PERI_UART1, peri_uart1, uart_sel, peri0_cg_regs,
		20, &mtk_clk_gate_ops_setclr),
	GATE(PERI_UART2, peri_uart2, uart_sel, peri0_cg_regs,
		21, &mtk_clk_gate_ops_setclr),
	GATE(PERI_UART3, peri_uart3, uart_sel, peri0_cg_regs,
		22, &mtk_clk_gate_ops_setclr),
	GATE(PERI_I2C0, peri_i2c0, axi_sel, peri0_cg_regs,
		23, &mtk_clk_gate_ops_setclr),
	GATE(PERI_I2C1, peri_i2c1, axi_sel, peri0_cg_regs,
		24, &mtk_clk_gate_ops_setclr),
	GATE(PERI_I2C2, peri_i2c2, axi_sel, peri0_cg_regs,
		25, &mtk_clk_gate_ops_setclr),
	GATE(PERI_I2C3, peri_i2c3, axi_sel, peri0_cg_regs,
		26, &mtk_clk_gate_ops_setclr),
	GATE(PERI_I2C4, peri_i2c4, axi_sel, peri0_cg_regs,
		27, &mtk_clk_gate_ops_setclr),
	GATE(PERI_AUXADC, peri_auxadc, clk26m, peri0_cg_regs,
		28, &mtk_clk_gate_ops_setclr),
	GATE(PERI_SPI0, peri_spi0, spi_sel, peri0_cg_regs,
		29, &mtk_clk_gate_ops_setclr),
	GATE(PERI_I2C5, peri_i2c5, axi_sel, peri0_cg_regs,
		30, &mtk_clk_gate_ops_setclr),
	GATE(PERI_NFIECC, peri_nfiecc, axi_sel, peri0_cg_regs,
		31, &mtk_clk_gate_ops_setclr),
	/* PERI1 */
	GATE(PERI_SPI, peri_spi, spi_sel, peri1_cg_regs,
		0, &mtk_clk_gate_ops_setclr),
	GATE(PERI_IRRX, peri_irrx, spi_sel, peri1_cg_regs,
		1, &mtk_clk_gate_ops_setclr),
	GATE(PERI_I2C6, peri_i2c6, axi_sel, peri1_cg_regs,
		2, &mtk_clk_gate_ops_setclr),
};

static void __init mtk_topckgen_init(struct device_node *node)
{
	struct clk_onecell_data *clk_data;
	void __iomem *base;
	int r;

	base = of_iomap(node, 0);
	if (!base) {
		pr_err("%s(): ioremap failed\n", __func__);
		return;
	}

	clk_data = mtk_alloc_clk_data(TOP_NR_CLK);

	mtk_init_factors(root_clk_alias, ARRAY_SIZE(root_clk_alias), clk_data);
	mtk_init_factors(top_divs, ARRAY_SIZE(top_divs), clk_data);
	mtk_init_clk_topckgen(base, clk_data);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);
	if (r)
		pr_err("%s(): could not register clock provider: %d\n",
			__func__, r);
}
CLK_OF_DECLARE(mtk_topckgen, "mediatek,mt8173-topckgen", mtk_topckgen_init);

static void __init mtk_apmixedsys_init(struct device_node *node)
{
	struct clk_onecell_data *clk_data;
	void __iomem *base;
	int r;

	base = of_iomap(node, 0);
	if (!base) {
		pr_err("%s(): ioremap failed\n", __func__);
		return;
	}

	clk_data = mtk_alloc_clk_data(APMIXED_NR_CLK);

	mtk_init_clk_apmixedsys(base, clk_data);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);
	if (r)
		pr_err("%s(): could not register clock provider: %d\n",
			__func__, r);
}
CLK_OF_DECLARE(mtk_apmixedsys, "mediatek,mt8173-apmixedsys",
		mtk_apmixedsys_init);

static void __init mtk_infrasys_init(struct device_node *node)
{
	struct clk_onecell_data *clk_data;
	void __iomem *base;
	int r;

	base = of_iomap(node, 0);
	if (!base) {
		pr_err("%s(): ioremap failed\n", __func__);
		return;
	}

	clk_data = mtk_alloc_clk_data(INFRA_NR_CLK);

	mtk_init_clk_gates(base, infra_clks, ARRAY_SIZE(infra_clks),
						clk_data, &lock);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);
	if (r)
		pr_err("%s(): could not register clock provider: %d\n",
			__func__, r);
}
CLK_OF_DECLARE(mtk_infrasys, "mediatek,mt8173-infracfg", mtk_infrasys_init);

static void __init mtk_pericfg_init(struct device_node *node)
{
	struct clk_onecell_data *clk_data;
	void __iomem *base;
	int r;

	base = of_iomap(node, 0);
	if (!base) {
		pr_err("%s(): ioremap failed\n", __func__);
		return;
	}

	clk_data = mtk_alloc_clk_data(PERI_NR_CLK);

	mtk_init_clk_gates(base, peri_clks, ARRAY_SIZE(peri_clks),
						clk_data, &lock);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);
	if (r)
		pr_err("%s(): could not register clock provider: %d\n",
			__func__, r);
}
CLK_OF_DECLARE(mtk_pericfg, "mediatek,mt8173-pericfg", mtk_pericfg_init);
