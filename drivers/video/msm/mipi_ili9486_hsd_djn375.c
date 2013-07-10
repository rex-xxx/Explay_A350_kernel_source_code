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
#include "mipi_ili9486_hsd_djn375.h"
#include "mipi_backlight.h"

static int current_mode = 0;
static struct dsi_buf ili9486_hsd_djn375_tx_buf;
static struct dsi_buf ili9486_hsd_djn375_rx_buf;

#define GPIO_AW551_LCD_RESET  129
#define CMD_DELAY 0
#define MIPI_SETTING_DELAY 5
#define SLEEP_OFF_DELAY 120
#define DISPLAY_ON_DELAY 5

/* common setting */
static char ili9486_display_off[2] = {0x28, 0x00};
static char ili9486_enter_sleep[2] = {0x10, 0x00};

static struct dsi_cmd_desc ili9486_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 30, sizeof(ili9486_display_off), ili9486_display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 150, sizeof(ili9486_enter_sleep), ili9486_enter_sleep}
};

static char mipi_ili9486_F7[]={0XF7, 0xA9, 0x91, 0x2D, 0x8A, 0x4F};
static char mipi_ili9486_F1[]={0XF1, 0x36, 0x04, 0x00, 0x3C, 0x0F, 0x8F};
static char mipi_ili9486_F2[]={0XF2, 0x18, 0xa3, 0x12, 0x02, 0xb2, 0x52, 0xFF, 0x10, 0x00};

static char mipi_ili9486_F8[]={0XF8, 0x21, 0x04};
static char mipi_ili9486_F9[]={0XF9, 0x00, 0x08};
static char mipi_ili9486_E0[]={0xE0, 0x00, 0x0F, 0x11, 0x06, 0x17, 0x0A, 0x3B, 0x99, 0x48, 0x09, 0x0E, 0x0B, 0x1C, 0x1F, 0x0F};
static char mipi_ili9486_E1[]={0xE1, 0x00, 0x20, 0x24, 0x04, 0x10, 0x06, 0x37, 0x66, 0x49, 0x04, 0x0D, 0x09, 0x2E, 0x30, 0x0F};
static char mipi_ili9486_B1[]={0XB1, 0xB0, 0x11};
static char mipi_ili9486_B4[]={0XB4, 0x02};
static char mipi_ili9486_B6_cmd[]={0XB6, 0x02, 0x22, 0x3B};
static char mipi_ili9486_B6_video[]={0XB6, 0x32, 0x22, 0x3B};
static char mipi_ili9486_C5[]={0XC5, 0x00, 0x17};
static char mipi_ili9486_C1[]={0XC1, 0x44};

static char mipi_ili9486_36[]={0x36, 0x08};
static char mipi_ili9486_3A[]={0x3A, 0x66};//RGB666
static char mipi_ili9486_35[]={0x35, 0x00};
static char mipi_ili9486_11[]={0x11, 0X00};
static char mipi_ili9486_29[]={0x29, 0X00};


static struct dsi_cmd_desc ili9486_hsd_djn375_cmd_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F7), mipi_ili9486_F7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F1), mipi_ili9486_F1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F2), mipi_ili9486_F2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F8), mipi_ili9486_F8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F9), mipi_ili9486_F9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_E0), mipi_ili9486_E0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_E1), mipi_ili9486_E1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_B1), mipi_ili9486_B1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_B4), mipi_ili9486_B4},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_B6_cmd), mipi_ili9486_B6_cmd},//command mode
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_C5), mipi_ili9486_C5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_C1), mipi_ili9486_C1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_36), mipi_ili9486_36},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_3A), mipi_ili9486_3A},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_35), mipi_ili9486_35},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(mipi_ili9486_11), mipi_ili9486_11},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(mipi_ili9486_29), mipi_ili9486_29},
};

static struct dsi_cmd_desc ili9486_hsd_djn375_video_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F7), mipi_ili9486_F7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F1), mipi_ili9486_F1},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F2), mipi_ili9486_F2},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F8), mipi_ili9486_F8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_F9), mipi_ili9486_F9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_E0), mipi_ili9486_E0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_E1), mipi_ili9486_E1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_B1), mipi_ili9486_B1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_B4), mipi_ili9486_B4},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_B6_video), mipi_ili9486_B6_video},//video mode
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_C5), mipi_ili9486_C5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_C1), mipi_ili9486_C1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_36), mipi_ili9486_36},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_3A), mipi_ili9486_3A},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(mipi_ili9486_11), mipi_ili9486_11},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(mipi_ili9486_29), mipi_ili9486_29},
};

static int mipi_ili9486_hsd_djn375_lcd_reset(void)
{
    int rc = 0;

    pr_debug("%s\n", __func__);

    gpio_set_value_cansleep(GPIO_AW551_LCD_RESET, 1);
    msleep(10);
    gpio_set_value_cansleep(GPIO_AW551_LCD_RESET, 0);
    msleep(10);
    gpio_set_value_cansleep(GPIO_AW551_LCD_RESET, 1);
    msleep(50);
    return rc;
}

static int mipi_ili9486_hsd_djn375_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	static int first_in = 0;
	
	pr_debug("mipi_ili9486_hsd_djn375_lcd_on Start\n");
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
	if (mipi_ili9486_hsd_djn375_lcd_reset() < 0) {
        pr_debug("mipi_ili9486_hsd_djn375_lcd_reset error\n");
        return -EINVAL;
    }

	if(current_mode == 0){		
		pr_info("%s, command mode", __func__);
		mipi_dsi_cmds_tx(mfd, &ili9486_hsd_djn375_tx_buf, ili9486_hsd_djn375_cmd_display_on_cmds,
				ARRAY_SIZE(ili9486_hsd_djn375_cmd_display_on_cmds));
	}else{
		pr_info("%s, video mode", __func__);
		mipi_dsi_cmds_tx(mfd, &ili9486_hsd_djn375_tx_buf, ili9486_hsd_djn375_video_display_on_cmds,
					ARRAY_SIZE(ili9486_hsd_djn375_video_display_on_cmds));
	}

	pr_debug("mipi_ili9486_hsd_djn375_lcd_on End\n");

	return 0;
}

static int mipi_ili9486_hsd_djn375_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	pr_debug("mipi_ili9486_hsd_djn375_lcd_off Start\n");

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(mfd, &ili9486_hsd_djn375_tx_buf, ili9486_display_off_cmds,
			ARRAY_SIZE(ili9486_display_off_cmds));
#if 0
    if (mipi_ili9486_hsd_djn375_lcd_reset() < 0) {
        pr_debug("mipi_ili9486_hsd_djn375_lcd_reset error\n");
        return -EINVAL;
    }
#endif
	gpio_set_value_cansleep(GPIO_AW551_LCD_RESET, 0);
#ifdef CONFIG_LCT_AW550
	pr_info("AW550 config LCM_ID");
	gpio_tlmm_config(GPIO_CFG(35, 0, 0, 0, 0), 0);//LCM_ID
#endif

	pr_debug("mipi_ili9486_hsd_djn375_lcd_off End\n");

	return 0;
}

static int ili9486_power_init(void){
#if 0
	struct vreg *vreg = NULL;
	int rc;
	vreg = vreg_get(NULL, "emmc");
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
	}
#endif
	return 0;
}

static int __devinit mipi_ili9486_hsd_djn375_lcd_probe(struct platform_device *pdev)
{
	int ret;
	pr_debug("%s\n", __func__);
	gpio_tlmm_config(GPIO_CFG(GPIO_BACKLIGHT_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),GPIO_CFG_ENABLE);
	ret = ili9486_power_init();
	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_ili9486_hsd_djn375_lcd_probe,
	.driver = {
		.name   = "mipi_ili9486_hsd_djn375",
	},
};

static void mipi_ili9486_hsd_djn375_set_backlight(struct msm_fb_data_type *mfd)
{
	mipi_backlight_control(mfd);	
}

static struct msm_fb_panel_data ili9486_hsd_djn375_panel_data = {
	.on	= mipi_ili9486_hsd_djn375_lcd_on,
	.off = mipi_ili9486_hsd_djn375_lcd_off,
	.set_backlight = mipi_ili9486_hsd_djn375_set_backlight,
};

static int ch_used[3];

int mipi_ili9486_hsd_djn375_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	pr_debug("mipi_ili9486_hsd_djn375_device_register\n");

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_ili9486_hsd_djn375", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	ili9486_hsd_djn375_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &ili9486_hsd_djn375_panel_data,
		sizeof(ili9486_hsd_djn375_panel_data));
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

void mipi_ili9486_hsd_djn375_sel_mode(int mode)
{
	current_mode = mode;
	pr_info("Macle, current mode = %d", mode);
}


static int __init mipi_ili9486_hsd_djn375_lcd_init(void)
{
	pr_debug("mipi_ili9486_hsd_djn375_lcd_init\n");
	mipi_dsi_buf_alloc(&ili9486_hsd_djn375_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&ili9486_hsd_djn375_rx_buf, DSI_BUF_SIZE);
	return platform_driver_register(&this_driver);
}

module_init(mipi_ili9486_hsd_djn375_lcd_init);
