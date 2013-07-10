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
#include "mipi_ili9486_cpt_lead.h"
#include "mipi_backlight.h"

static int current_mode = 0;
static struct dsi_buf ili9486_cpt_lead_tx_buf;
static struct dsi_buf ili9486_cpt_lead_rx_buf;

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


static char mipi_ili9486_cpt_lead_c0[]={0xC0, 0X0F, 0X03};
static char mipi_ili9486_cpt_lead_c1[]={0xC1, 0X41};
static char mipi_ili9486_cpt_lead_c5[]={0xC5, 0X00, 0X37};
static char mipi_ili9486_cpt_lead_b1[]={0xB1, 0XB0, 0X11};
static char mipi_ili9486_cpt_lead_c2[]={0xC2, 0X22};
static char mipi_ili9486_cpt_lead_b4[]={0xB4, 0X02};
static char mipi_ili9486_cpt_lead_b7[]={0xB7, 0X86};

static char mipi_ili9486_cpt_lead_b6_cmd[]={0XB6, 0x02, 0x22, 0x3B};//{0xB6, 0X22, 0x02};
static char mipi_ili9486_cpt_lead_b6_video[]={0XB6, 0x32, 0x22, 0x3B};

static char mipi_ili9486_cpt_lead_e0[]={
	0xE0, 0x0F, 0x1A, 0x17,
	0x0C, 0X10, 0X09, 0X45,
	0XA8, 0X35, 0X09, 0X13,
	0X04, 0X0A, 0X06, 0X00
};
static char mipi_ili9486_cpt_lead_e1[]={
	0xE1, 0x0F, 0x39, 0x35,
	0x0B, 0X0C, 0X06, 0X4A,
	0X75, 0X3A, 0X06, 0X0F,
	0X03, 0X28, 0X25, 0X0F
};
static char mipi_ili9486_cpt_lead_F2[]={
	0xF2, 0x18, 0xA3, 0x12,
	0x02, 0XB2, 0X12, 0XFF,
	0X10, 0X00
};
static char mipi_ili9486_cpt_lead_F8[]={
	0xF8, 0x21, 0x04
};
static char mipi_ili9486_cpt_lead_36[]={
	0x36, 0X08
};
static char mipi_ili9486_cpt_lead_3A[]={
	0x3A, 0X66
};
static char mipi_ili9486_cpt_lead_f9[]={
	0xF9, 0X00, 0X08
};

static char mipi_ili9486_cpt_lead_f1[]={
	0xF1, 0X36, 0X04, 0x00, 
	0x3c, 0x0f, 0x8f
};

static char mipi_ili9486_cpt_lead_f7[]={
	0xF7, 0XA9, 0X91, 0x2D, 
	0x8A
};
static char mipi_ili9486_cpt_lead_11[]={0x11, 0X00};
static char mipi_ili9486_cpt_lead_29[]={0x29, 0X00};


static struct dsi_cmd_desc ILI9486_cpt_lead_cmd_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_c0), mipi_ili9486_cpt_lead_c0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_c1), mipi_ili9486_cpt_lead_c1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_c5), mipi_ili9486_cpt_lead_c5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_b1), mipi_ili9486_cpt_lead_b1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_c2), mipi_ili9486_cpt_lead_c2},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_b4), mipi_ili9486_cpt_lead_b4},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_b7), mipi_ili9486_cpt_lead_b7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_b6_cmd), mipi_ili9486_cpt_lead_b6_cmd},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_e0), mipi_ili9486_cpt_lead_e0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_e1), mipi_ili9486_cpt_lead_e1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_F2), mipi_ili9486_cpt_lead_F2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_F8), mipi_ili9486_cpt_lead_F8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_36), mipi_ili9486_cpt_lead_36},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_3A), mipi_ili9486_cpt_lead_3A},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_f9), mipi_ili9486_cpt_lead_f9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_f1), mipi_ili9486_cpt_lead_f1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_f7), mipi_ili9486_cpt_lead_f7},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(mipi_ili9486_cpt_lead_11), mipi_ili9486_cpt_lead_11},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(mipi_ili9486_cpt_lead_29), mipi_ili9486_cpt_lead_29},
};

static struct dsi_cmd_desc ILI9486_cpt_lead_video_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_c0), mipi_ili9486_cpt_lead_c0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_c1), mipi_ili9486_cpt_lead_c1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_c5), mipi_ili9486_cpt_lead_c5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_b1), mipi_ili9486_cpt_lead_b1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_c2), mipi_ili9486_cpt_lead_c2},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_b4), mipi_ili9486_cpt_lead_b4},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_b7), mipi_ili9486_cpt_lead_b7},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_b6_video), mipi_ili9486_cpt_lead_b6_video},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_e0), mipi_ili9486_cpt_lead_e0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_e1), mipi_ili9486_cpt_lead_e1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_F2), mipi_ili9486_cpt_lead_F2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_F8), mipi_ili9486_cpt_lead_F8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_36), mipi_ili9486_cpt_lead_36},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_3A), mipi_ili9486_cpt_lead_3A},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_f9), mipi_ili9486_cpt_lead_f9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_f1), mipi_ili9486_cpt_lead_f1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(mipi_ili9486_cpt_lead_f7), mipi_ili9486_cpt_lead_f7},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(mipi_ili9486_cpt_lead_11), mipi_ili9486_cpt_lead_11},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(mipi_ili9486_cpt_lead_29), mipi_ili9486_cpt_lead_29},
};



static int mipi_ili9486_cpt_lead_lcd_reset(void)
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

static int mipi_ili9486_cpt_lead_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	static int first_in = 0;
	pr_debug("mipi_ili9486_cpt_lead_lcd_on Start\n");
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
	if (mipi_ili9486_cpt_lead_lcd_reset() < 0) {
        pr_debug("mipi_ili9486_cpt_lead_lcd_reset error\n");
        return -EINVAL;
    }

	if(current_mode == 0){		
		pr_info("%s, command mode", __func__);
		mipi_dsi_cmds_tx(mfd, &ili9486_cpt_lead_tx_buf, ILI9486_cpt_lead_cmd_display_on_cmds,
				ARRAY_SIZE(ILI9486_cpt_lead_cmd_display_on_cmds));
	}else{
		pr_info("%s, video mode", __func__);
		mipi_dsi_cmds_tx(mfd, &ili9486_cpt_lead_tx_buf, ILI9486_cpt_lead_video_display_on_cmds,
					ARRAY_SIZE(ILI9486_cpt_lead_video_display_on_cmds));
	}

	pr_debug("mipi_ili9486_cpt_lead_lcd_on End\n");

	return 0;
}

static int mipi_ili9486_cpt_lead_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	pr_debug("mipi_ili9486_cpt_lead_lcd_off Start\n");

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(mfd, &ili9486_cpt_lead_tx_buf, ili9486_display_off_cmds,
			ARRAY_SIZE(ili9486_display_off_cmds));
#if 0
    if (mipi_ili9486_cpt_lead_lcd_reset() < 0) {
        pr_debug("mipi_ili9486_cpt_lead_lcd_reset error\n");
        return -EINVAL;
    }
#endif
	gpio_set_value_cansleep(GPIO_AW551_LCD_RESET, 0);

	pr_debug("mipi_ili9486_cpt_lead_lcd_off End\n");

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

static int __devinit mipi_ili9486_cpt_lead_lcd_probe(struct platform_device *pdev)
{
	int ret;
	pr_debug("%s\n", __func__);
	gpio_tlmm_config(GPIO_CFG(GPIO_BACKLIGHT_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),GPIO_CFG_ENABLE);
	ret = ili9486_power_init();
	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_ili9486_cpt_lead_lcd_probe,
	.driver = {
		.name   = "mipi_ili9486_cpt_lead",
	},
};

static void mipi_ili9486_cpt_lead_set_backlight(struct msm_fb_data_type *mfd)
{
	mipi_backlight_control(mfd);	
}

static struct msm_fb_panel_data ili9486_cpt_lead_panel_data = {
	.on	= mipi_ili9486_cpt_lead_lcd_on,
	.off = mipi_ili9486_cpt_lead_lcd_off,
	.set_backlight = mipi_ili9486_cpt_lead_set_backlight,
};

static int ch_used[3];

int mipi_ili9486_cpt_lead_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	pr_debug("mipi_ili9486_cpt_lead_device_register\n");

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_ili9486_cpt_lead", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	ili9486_cpt_lead_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &ili9486_cpt_lead_panel_data,
		sizeof(ili9486_cpt_lead_panel_data));
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

void mipi_ili9486_cpt_lead_sel_mode(int mode)
{
	current_mode = mode;
	pr_info("Macle, current mode = %d", mode);
}


static int __init mipi_ili9486_cpt_lead_lcd_init(void)
{
	pr_debug("mipi_ili9486_cpt_lead_lcd_init\n");
	mipi_dsi_buf_alloc(&ili9486_cpt_lead_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&ili9486_cpt_lead_rx_buf, DSI_BUF_SIZE);
	return platform_driver_register(&this_driver);
}

module_init(mipi_ili9486_cpt_lead_lcd_init);
