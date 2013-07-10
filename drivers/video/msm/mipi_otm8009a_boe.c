/* Copyright (c) 2008-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define DEBUG

#include "mach/gpio.h"

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_otm8009a_boe.h"
#include "mipi_backlight.h"

static int current_mode = 0;
#define HX8369A
static struct dsi_buf otm8009a_boe_tx_buf;
static struct dsi_buf otm8009a_boe_rx_buf;

#define CMD_DELAY 0
#define MIPI_SETTING_DELAY 10
#define SLEEP_OFF_DELAY 120
#define DISPLAY_ON_DELAY 30

static char otm8009a_enableEXTC[]={0xff, 0x80, 0x09, 0x01};
static char otm8009a_enableOriseMode_shiftaddr[]={0x00, 0x80};
static char otm8009a_enableOriseMode[]={0xff,0x80,0x09};

static char otm8009a_columnInversion_shiftaddr[]={0x00, 0xb4};
static char otm8009a_columnInversion[]={0xc0, 0x50};

static char otm8009a_pump45_shiftaddr[]={0x00, 0x92};
static char otm8009a_pump45[]={0xc5, 0x01};

static char otm8009a_pump1_shiftaddr[]={0x00, 0x90};
static char otm8009a_pump1[]={0xc5, 0x96};

static char otm8009a_pump34_shiftaddr[]={0x00, 0x95};
static char otm8009a_pump34[]={0xc5, 0x34};

static char otm8009a_pump12_shiftaddr[]={0x00, 0x94};
static char otm8009a_pump12[]={0xc5, 0x33};

static char otm8009a_sourcepch_shiftaddr[]={0x00, 0xa3};
static char otm8009a_sourcepch[]={0xc0, 0x1b};

static char otm8009a_pump23_shiftaddr[]={0x00, 0x82};
static char otm8009a_pump23[]={0xc5, 0x83};

static char otm8009a_sourcebias_shiftaddr[]={0x00, 0x81};
static char otm8009a_sourcebias[]={0xc4, 0x83};

static char otm8009a_vdd_shiftaddr[]={0x00, 0xb1};
static char otm8009a_vdd[]={0xc5, 0xa9};

static char otm8009a_vgh_shiftaddr[]={0x00, 0x91};
static char otm8009a_vgh[]={0xc5, 0x2c};

static char otm8009a_gvdd_shiftaddr[]={0x00, 0x00};
static char otm8009a_gvdd[]={0xd8, 0x79};

static char otm8009a_ngvdd_shiftaddr[]={0x00, 0x01};
static char otm8009a_ngvdd[]={0xd8, 0x79};

static char otm8009a_framerate_shiftaddr[]={0x00, 0x81};
static char otm8009a_framerate[]={0xc1, 0x66};

static char otm8009a_vs_shiftaddr[]={0x00, 0xa1};
static char otm8009a_vs[]={0xc1, 0x0e};

static char otm8009a_vcom_shiftaddr[]={0x00, 0x00};
static char otm8009a_vcom[]={0xd9, 0x3e};

static char otm8009a_gamma_EC[]={0xEC, 0x40, 0x44, 0x44, 0x44, 
								0x44,0x44,0x44,0x44,
								0x44,0x44,0x44,0x44,
								0x44,0x44,0x44,0x44,
								0x44,0x44,0x44,0x44,
								0x44,0x44,0x44,0x44,
								0x44,0x44,0x44,0x44,
								0x44,0x44,0x44,0x42,0x06
};
static char otm8009a_gamma_ED[]={0xED,0x40,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
			  					  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,0x03
};
static char otm8009a_gamma_EE[]={0xEE,0x40,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
			  					  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x44,
								  0x44,0x44,0x44,0x22,0x08
};
 //--------------------------------------------------------------------------------
//		initial setting 2 < tcon_goa_wave >
//--------------------------------------------------------------------------------
static char otm8009a_CE8x_shiftaddr[]={0x00,0x80};
static char otm8009a_CE8x[]={0xce,0x86,0x01,0x00,0x85,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char otm8009a_CEAx_shiftaddr[]={0x00,0xa0};
static char otm8009a_CEAx[]={0xCE,0x18,0x04,0x03,0x21,0x00,0x00,0x00,0x18,0x03,0x03,0x22,0x00,0x00,0x00};

static char otm8009a_CEBx_shiftaddr[]={0x00,0xb0};
static char otm8009a_CEBx[]={0xCE,0x18,0x02,0x03,0x23,0x00,0x00,0x00,0x18,0x01,0x03,0x24,0x00,0x00,0x00};

static char otm8009a_CFCx_shiftaddr[]={0x00,0xc0};
static char otm8009a_CFCx[]={0xCF,0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x00,0x00,0x00};

static char otm8009a_CFDx_shiftaddr[]={0x00,0xd0};
static char otm8009a_CFDx[]={0xCF,0x00};

static char otm8009a_CBCx_shiftaddr[]={0x00,0xc0};
static char otm8009a_CBCx[]={0xCB,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char otm8009a_CBDx_shiftaddr[]={0x00,0xD0};
static char otm8009a_CBDx[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00};

static char otm8009a_CBEx_shiftaddr[]={0x00,0xE0};
static char otm8009a_CBEx[]={0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char otm8009a_CBFx_shiftaddr[]={0x00,0xf0};
static char otm8009a_CBFx[]={0xCB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

static char otm8009a_CC8x_shiftaddr[]={0x00,0x80};
static char otm8009a_CC8x[]={0xCC,0x00,0x26,0x09,0x0b,0x01,0x25,0x00,0x00,0x00,0x00};

static char otm8009a_CC9x_shiftaddr[]={0x00,0x90};
static char otm8009a_CC9x[]={0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0a,0x0c,0x02};

static char otm8009a_CCAx_shiftaddr[]={0x00,0xa0};
static char otm8009a_CCAx[]={0xCC,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char otm8009a_CCBx_shiftaddr[]={0x00,0xb0};
static char otm8009a_CCBx[]={0xCC,0x00,0x25,0x0c,0x0a,0x02,0x26,0x00,0x00,0x00,0x00};


static char otm8009a_CCCx_shiftaddr[]={0x00,0xc0};
static char otm8009a_CCCx[]={0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x0b,0x09,0x01};

static char otm8009a_CCDx_shiftaddr[]={0x00,0xd0};
static char otm8009a_CCDx[]={0xCC,0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char otm8009a_quitsleep[]={0x11};
static char otm8009a_displayon[]={0x29};

static struct dsi_cmd_desc otm8009a_boe_cmd_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_enableEXTC), otm8009a_enableEXTC},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_enableOriseMode_shiftaddr), otm8009a_enableOriseMode_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_enableOriseMode), otm8009a_enableOriseMode},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_columnInversion_shiftaddr), otm8009a_columnInversion_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_columnInversion), otm8009a_columnInversion},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump45_shiftaddr), otm8009a_pump45_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump45), otm8009a_pump45},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump1_shiftaddr), otm8009a_pump1_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump1), otm8009a_pump1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump34_shiftaddr), otm8009a_pump34_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump34), otm8009a_pump34},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump12_shiftaddr), otm8009a_pump12_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump12), otm8009a_pump12},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_sourcepch_shiftaddr), otm8009a_sourcepch_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_sourcepch), otm8009a_sourcepch},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump23_shiftaddr), otm8009a_pump23_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_pump23), otm8009a_pump23},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_sourcebias_shiftaddr), otm8009a_sourcebias_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_sourcebias), otm8009a_sourcebias},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_vdd_shiftaddr), otm8009a_vdd_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_vdd), otm8009a_vdd},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_vgh_shiftaddr), otm8009a_vgh_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_vgh), otm8009a_vgh},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_gvdd_shiftaddr), otm8009a_gvdd_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_gvdd), otm8009a_gvdd},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_ngvdd_shiftaddr), otm8009a_ngvdd_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_ngvdd), otm8009a_ngvdd},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_framerate_shiftaddr), otm8009a_framerate_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_framerate), otm8009a_framerate},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_vs_shiftaddr), otm8009a_vs_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_vs), otm8009a_vs},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_vcom_shiftaddr), otm8009a_vcom_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_vcom), otm8009a_vcom},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_gamma_EC), otm8009a_gamma_EC},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_gamma_ED), otm8009a_gamma_ED},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_gamma_EE), otm8009a_gamma_EE},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CE8x_shiftaddr), otm8009a_CE8x_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CE8x), otm8009a_CE8x},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CEAx_shiftaddr), otm8009a_CEAx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CEAx), otm8009a_CEAx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CEBx_shiftaddr), otm8009a_CEBx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CEBx), otm8009a_CEBx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CFCx_shiftaddr), otm8009a_CFCx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CFCx), otm8009a_CFCx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CFDx_shiftaddr), otm8009a_CFDx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CFDx), otm8009a_CFDx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CBCx_shiftaddr), otm8009a_CBCx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CBCx), otm8009a_CBCx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CBDx_shiftaddr), otm8009a_CBDx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CBDx), otm8009a_CBDx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CBEx_shiftaddr), otm8009a_CBEx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CBEx), otm8009a_CBEx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CBFx_shiftaddr), otm8009a_CBFx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CBFx), otm8009a_CBFx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CC8x_shiftaddr), otm8009a_CC8x_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CC8x), otm8009a_CC8x},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CC9x_shiftaddr), otm8009a_CC9x_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CC9x), otm8009a_CC9x},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CCAx_shiftaddr), otm8009a_CCAx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CCAx), otm8009a_CCAx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CCBx_shiftaddr), otm8009a_CCBx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CCBx), otm8009a_CCBx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CCCx_shiftaddr), otm8009a_CCCx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CCCx), otm8009a_CCCx},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CCDx_shiftaddr), otm8009a_CCDx_shiftaddr},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(otm8009a_CCDx), otm8009a_CCDx},
	{DTYPE_DCS_WRITE, 1, 0, 0, SLEEP_OFF_DELAY, sizeof(otm8009a_quitsleep), otm8009a_quitsleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(otm8009a_displayon), otm8009a_displayon},
};

static char display_off[]={0x28, 0x00};
static char enter_sleep[]={0x10, 0x00};
static struct dsi_cmd_desc otm8009a_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 150, sizeof(enter_sleep), enter_sleep}
};

#define GPIO_HX8363B_LCD_RESET  129
static int mipi_otm8009a_boe_lcd_reset(void)
{
    int rc = 0;

    pr_debug("%s\n", __func__);

    gpio_set_value_cansleep(GPIO_HX8363B_LCD_RESET, 1);
    msleep(10);
    gpio_set_value_cansleep(GPIO_HX8363B_LCD_RESET, 0);
    msleep(10);
    gpio_set_value_cansleep(GPIO_HX8363B_LCD_RESET, 1);
    msleep(50);
    return rc;
}

static int mipi_otm8009a_boe_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	static int first_in = 0;
	pr_debug("mipi_otm8009a_boe_lcd_on Start\n");
	if(first_in == 0){//booted then doesnot reset
		first_in = 1;			
		if(current_mode == 0){
			return 0;
		}else{
			mipi_backlight_turnoff();
		}
	}
	mfd = platform_get_drvdata(pdev);

	if (!mfd) {
		return -ENODEV;
	}
	
	if (mfd->key != MFD_KEY) {
		return -EINVAL;
	}
	
	mipi  = &mfd->panel_info.mipi;
	pr_debug("%s: mode = %d\n", __func__, mipi->mode);
	if (mipi_otm8009a_boe_lcd_reset() < 0) {
        pr_debug("mipi_otm8009a_boe_lcd_reset error\n");
        return -EINVAL;
    }
	//msleep(120);
	if(current_mode == 0){		
		pr_info("%s, command mode", __func__);
		mipi_dsi_cmds_tx(mfd, &otm8009a_boe_tx_buf, otm8009a_boe_cmd_display_on_cmds,
				ARRAY_SIZE(otm8009a_boe_cmd_display_on_cmds));
	}else{
		pr_info("%s, video mode", __func__);
		mipi_dsi_cmds_tx(mfd, &otm8009a_boe_tx_buf, otm8009a_boe_cmd_display_on_cmds,
					ARRAY_SIZE(otm8009a_boe_cmd_display_on_cmds));
	}

	pr_debug("mipi_otm8009a_boe_lcd_on End\n");

	return 0;
}

static int mipi_otm8009a_boe_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	pr_debug("mipi_otm8009a_boe_lcd_off Start\n");

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(mfd, &otm8009a_boe_tx_buf, otm8009a_display_off_cmds,
			ARRAY_SIZE(otm8009a_display_off_cmds));

    if (mipi_otm8009a_boe_lcd_reset() < 0) {
        pr_debug("mipi_otm8009a_boe_lcd_reset error\n");
        return -EINVAL;
    }

	pr_debug("mipi_otm8009a_boe_lcd_off End\n");

	return 0;
}

static int otm8009a_power_init(void){
	/*struct vreg *vreg = NULL;
	int rc;
	vreg = vreg_get(NULL, "bt");
	if (IS_ERR(vreg)) {
		pr_err("%s: vreg get failed (%ld)\n",
			__FUNCTION__, PTR_ERR(vreg));
		return PTR_ERR(vreg);
	}
	rc = vreg_set_level(vreg, 2850);
	if(rc){
		printk(KERN_ERR "%s: vreg set level failed (%d)\n", __FUNCTION__, rc);
		return -1;
	}
	rc = vreg_enable(vreg);
	if(rc){
		printk(KERN_ERR "%s: vreg enable failed (%d)\n", __FUNCTION__, rc);
		return -2;
	}

	mdelay(200);
	vreg = vreg_get(NULL, "msme1");
	if (IS_ERR(vreg)) {
		pr_err("%s: vreg get failed (%ld)\n",
			__FUNCTION__, PTR_ERR(vreg));
		return PTR_ERR(vreg);
	}
	rc = vreg_set_level(vreg, 1800);
	if(rc){
		printk(KERN_ERR "%s: vreg set level failed (%d)\n", __FUNCTION__, rc);
		return -1;
	}
	rc = vreg_enable(vreg);
	if(rc){
		printk(KERN_ERR "%s: vreg enable failed (%d)\n", __FUNCTION__, rc);
		return -2;
	}*/
	return 0;
}

static int __devinit mipi_otm8009a_boe_lcd_probe(struct platform_device *pdev)
{
	int ret;
	pr_debug("%s\n", __func__);
	gpio_tlmm_config(GPIO_CFG(GPIO_BACKLIGHT_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),GPIO_CFG_ENABLE);
	ret = otm8009a_power_init();
	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_otm8009a_boe_lcd_probe,
	.driver = {
		.name   = "mipi_otm8009a_boe",
	},
};

static void mipi_otm8009a_boe_set_backlight(struct msm_fb_data_type *mfd)
{
	mipi_backlight_control(mfd);	
}

static struct msm_fb_panel_data otm8009a_boe_panel_data = {
	.on	= mipi_otm8009a_boe_lcd_on,
	.off = mipi_otm8009a_boe_lcd_off,
	.set_backlight = mipi_otm8009a_boe_set_backlight,
};

static int ch_used[3];

int mipi_otm8009a_boe_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	pr_debug("mipi_otm8009a_boe_device_register\n");

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_otm8009a_boe", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	otm8009a_boe_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &otm8009a_boe_panel_data,
		sizeof(otm8009a_boe_panel_data));
	if (ret) {
		pr_debug("%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		pr_debug("%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

void mipi_otm8009a_boe_sel_mode(int mode)
{
	current_mode = mode;
	pr_info("Macle, current mode = %d", mode);
}


static int __init mipi_otm8009a_boe_lcd_init(void)
{
	pr_debug("mipi_otm8009a_boe_lcd_init\n");
	mipi_dsi_buf_alloc(&otm8009a_boe_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&otm8009a_boe_rx_buf, DSI_BUF_SIZE);
	return platform_driver_register(&this_driver);
}

module_init(mipi_otm8009a_boe_lcd_init);
