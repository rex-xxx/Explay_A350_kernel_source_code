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
#include "mipi_hx8369a_tcl.h"
#include "mipi_backlight.h"

//static struct msm_panel_common_pdata *mipi_hx8369a_tcl_pdata;
static struct dsi_buf hx8369a_tcl_tx_buf;
static struct dsi_buf hx8369a_tcl_rx_buf;

#define HX8369A_CMD_DELAY 0
#define MIPI_SETTING_DELAY 5
#define HX8369A_SLEEP_OFF_DELAY 120
#define HX8369A_DISPLAY_ON_DELAY 5

/* common setting */
static char extend_cmd_enable[4] = {
0xB9, 0xFF, 0x83, 0x69
};
static char exit_sleep[2] = {0x11, 0x00};
static char display_on[2] = {0x29, 0x00};
static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};

static char mipi_setting[14] = {
	0xBA, 0x00, 0xA0, 0xC6,
	0x00, 0x0A, 0x00, 0x10,
	0x30, 0x6C, 0x02, 0x11,//0X6F->0x6C
	0x18, 0x40, 
};

static struct dsi_cmd_desc truly_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 150, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 150, sizeof(enter_sleep), enter_sleep}
};

/* command mode setting */
static char cmd_oscillator_setting[3] = {
	0xB0, 0x01, 0x09,//must be less or equal 0x09
};

static char cmd_power_setting[20] = {
	0xB1, 0x01, 0x00, 0x34, 
	0x07, 0x00, 0x0F, 0x0F, 
	0x21, 0x28, 0x3F, 0x3F, 
	0x07, 0x23, 0x01, 0xE6, 
	0xE6, 0xE6, 0xE6, 0xE6
};


static char cmd_display_setting[16] = {
	0xB2, 0x00, 0x20, 0x0A,
	0x0A, 0x70, 0x00, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 
	0x03, 0x03, 0x00, 0x01
};

static char cmd_wave_cycle_setting[6] = {0xB4, 
	0x00, 0x0C, 0x84, 0x0C, 0x01
};
static char cmd_vcom_setting[3] = {0xB6,
	0x2C, 0x29
};
static char cmd_gip_setting[27] = {
	0xD5, 0x00, 0x05, 0x03, 
	0x00, 0x01, 0x09, 0x10, 
	0x80, 0x37, 0x37, 0x20, 
	0x31, 0x46, 0x8A, 0x57, 
	0x9B, 0x20, 0x31, 0x46,
	0x8A, 0x57, 0x9B, 0x07, 
	0x0F, 0x07, 0x00
};
static char cmd_gamma_setting[35] = {0xE0,
	0x00, 0x06, 0x06, 0x29, 0x2D, 
	0x3F, 0x13, 0x32, 0x08, 0x0c,
	0x0d, 0x11, 0x14, 0x11, 0x14,
	0x0e, 0x15, 0x00, 0x06, 0x06,
	0x29, 0x2D, 0x3F, 0x13, 0x32,
	0x08, 0x0c, 0x0d, 0x11, 0x14,
	0x11, 0x14, 0x0e, 0x15
};

static char cmd_pixel_format_setting[2] = {0x3A, 0x77};

static struct dsi_cmd_desc hx8369a_tcl_cmd_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369A_CMD_DELAY, sizeof(extend_cmd_enable), extend_cmd_enable},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369A_CMD_DELAY, sizeof(cmd_oscillator_setting), cmd_oscillator_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369A_CMD_DELAY, sizeof(cmd_power_setting), cmd_power_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369A_CMD_DELAY, sizeof(cmd_display_setting), cmd_display_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369A_CMD_DELAY, sizeof(cmd_wave_cycle_setting), cmd_wave_cycle_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369A_CMD_DELAY, sizeof(cmd_vcom_setting), cmd_vcom_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369A_CMD_DELAY, sizeof(cmd_gip_setting), cmd_gip_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, MIPI_SETTING_DELAY, sizeof(cmd_gamma_setting), cmd_gamma_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, MIPI_SETTING_DELAY, sizeof(mipi_setting), mipi_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, HX8369A_CMD_DELAY, sizeof(cmd_pixel_format_setting), cmd_pixel_format_setting},
	{DTYPE_DCS_WRITE, 1, 0, 0, HX8369A_SLEEP_OFF_DELAY, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, HX8369A_CMD_DELAY, sizeof(display_on), display_on},
};

#define GPIO_HX8369A_LCD_RESET  129
static int mipi_hx8369a_tcl_lcd_reset(void)
{
    int rc = 0;

    pr_debug("%s\n", __func__);

    gpio_set_value_cansleep(GPIO_HX8369A_LCD_RESET, 1);
    msleep(1);
    gpio_set_value_cansleep(GPIO_HX8369A_LCD_RESET, 0);
    msleep(1);
    gpio_set_value_cansleep(GPIO_HX8369A_LCD_RESET, 1);

    return rc;
}

static int mipi_hx8369a_tcl_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;

	static int first_in = 0;
	pr_debug("mipi_hx8369a_tcl_lcd_on Start\n");
	if(first_in == 0){//booted then doesnot reset
		first_in = 1;
		return 0;
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
	if (mipi_hx8369a_tcl_lcd_reset() < 0) {
        pr_debug("mipi_hx8369a_tcl_lcd_reset error\n");
        return -EINVAL;
    }
	msleep(120);
	mipi_dsi_cmds_tx(mfd, &hx8369a_tcl_tx_buf, hx8369a_tcl_cmd_display_on_cmds,
			ARRAY_SIZE(hx8369a_tcl_cmd_display_on_cmds));

	pr_debug("mipi_hx8369a_tcl_lcd_on End\n");

	return 0;
}

static int mipi_hx8369a_tcl_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	pr_debug("mipi_hx8369a_tcl_lcd_off Start\n");

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(mfd, &hx8369a_tcl_tx_buf, truly_display_off_cmds,
			ARRAY_SIZE(truly_display_off_cmds));

    if (mipi_hx8369a_tcl_lcd_reset() < 0) {
        pr_debug("mipi_hx8369a_tcl_lcd_reset error\n");
        return -EINVAL;
    }

	pr_debug("mipi_hx8369a_tcl_lcd_off End\n");

	return 0;
}
static int hx8369a_power_init(void){
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

static int __devinit mipi_hx8369a_tcl_lcd_probe(struct platform_device *pdev)
{
	int ret;
	pr_debug("%s\n", __func__);
	gpio_tlmm_config(GPIO_CFG(GPIO_BACKLIGHT_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),GPIO_CFG_ENABLE);
	ret = hx8369a_power_init();

	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_hx8369a_tcl_lcd_probe,
	.driver = {
		.name   = "mipi_hx8369a_tcl",
	},
};

static void mipi_hx8369a_tcl_set_backlight(struct msm_fb_data_type *mfd)
{
	mipi_backlight_control(mfd);	
}


static struct msm_fb_panel_data hx8369a_tcl_panel_data = {
	.on	= mipi_hx8369a_tcl_lcd_on,
	.off = mipi_hx8369a_tcl_lcd_off,
	.set_backlight = mipi_hx8369a_tcl_set_backlight,
};

static int ch_used[3];

int mipi_hx8369a_tcl_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	pr_debug("mipi_hx8369a_tcl_device_register\n");

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_hx8369a_tcl", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	hx8369a_tcl_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &hx8369a_tcl_panel_data,
		sizeof(hx8369a_tcl_panel_data));
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

static int __init mipi_hx8369a_tcl_lcd_init(void)
{
	pr_debug("mipi_hx8369a_tcl_lcd_init\n");
	
	mipi_dsi_buf_alloc(&hx8369a_tcl_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&hx8369a_tcl_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}

module_init(mipi_hx8369a_tcl_lcd_init);
