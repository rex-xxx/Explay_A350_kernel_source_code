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
#include "mipi_hx8357c_hsd.h"
#include "mipi_backlight.h"

static int current_mode = 0;
static struct dsi_buf hx8357c_hsd_tx_buf;
static struct dsi_buf hx8357c_hsd_rx_buf;

#define GPIO_AW551_LCD_RESET  129
#define CMD_DELAY 0
#define MIPI_SETTING_DELAY 5
#define SLEEP_OFF_DELAY 150
#define DISPLAY_ON_DELAY 5

/* common setting */
static char hx8357c_display_off[2] = {0x28, 0x00};
static char hx8357c_enter_sleep[2] = {0x10, 0x00};
static char hx8357c_sleep_out[2] = {0x11, 0x00};
static char hx8357c_display_on[2]={0x29, 0x00};

static struct dsi_cmd_desc hx8357c_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 30, sizeof(hx8357c_display_off), hx8357c_display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 150, sizeof(hx8357c_enter_sleep), hx8357c_enter_sleep}
};
static char hx8357c_extention_cmd_enable[4] = {
	0xB9, 0xFF, 0x83, 0x57
};
static char hx8357c_vcom_setting[2] = {
	0xB6, 0x24
};

#ifdef CONFIG_LCT_AW550_MCX
static char hx8357c_video_vcom_setting[2] = {
	0xB6, 0x28
};
#else
static char hx8357c_video_vcom_setting[2] = {
	0xB6, 0x2f
};
#endif

static char hx8357c_internal_osc[3] = {
	0xB0, 0x6e, 0x01//0x68->0x55
};
static char hx8357c_set_panel[2] = {
	0xCC, 0x09
};
static char hx8357c_power_ctrl[7] = {
	0xB1, 0x00, 0x14, 0x16,
	0x16, 0x83, 0x48
};

static char hx8357c_video_power_ctrl[7] = {
	0xB1, 0x00, 0x15, 0x1C,
	0x1C, 0x83, 0xaa
};


static char hx8357c_disp_cycle[8] = {
	0xB4, 0x02, 0x40, 0x00,
	0x2A, 0x2A, 0x0d, 0x4f
};


static char hx8357c_cmd_rgb_interface[5] = {
	0xB3, 0x40, 0x00, 0x06,//0x43--video mode
	0x06
};
static char hx8357c_video_rgb_interface[5] = {
	0xB3, 0x43, 0x00, 0x06,//0x43--video mode
	0x06
};

static char hx8357c_c0[7] = {
	0xC0, 0x33, 0x50, 0x01,
	0x3C, 0xC8, 0x08
};

static char hx8357c_video_c0[7] = {
	0xC0, 0x24, 0x24, 0x01,
	0x3C, 0x1E, 0x08
};

static char hx8357c_gamma_setting[] = {
	0xE0, 0x00, 0x00, 0x02,//VRP0
	0x00, 0x08, 0x00, 0x11,//VRP1 , 2
	0x00, 0x23, 0x00, 0x2c,//VRP3, 4
	0x00, 0x40, 0x00, 0x4a,//VRP5, 6 
	0x00, 0x52, 0x00, 0x48,//VRP7, 8
	0x00, 0x41, 0x00, 0x3c, //VRP9, 10
	0x00, 0x33, 0x00, 0x2e, //VRP11, 12
	0x00, 0x28, 0x00, 0x27, //VRP13, 14
	0x00, 0x1b, 0x01, 0x02, //VRP15, GMA_RELOAD, VRN0
	0x00, 0x08, 0x00, 0x11,//VRN1, 2
	0x00, 0x23, 0x00, 0x2c,//VRN3, 4
	0x00, 0x40, 0x00, 0x4a,//VRN5,6
	0x00, 0x52, 0x00, 0x48,//VRN7,8
	0x00, 0x41, 0x00, 0x3c,//VRN9,10
	0x00, 0x33, 0x00, 0x2e,//VRN11,12
	0x00, 0x28, 0x00, 0x27,//VRN13,14
	0x00, 0x1b, 0x00, 0x00//VRN15
};

static char hx8357c_cmd_pixel_format_setting[2] = {0x3A, 0x77};//RGB888; 5-bit16, 6-bit18, 7-bit24

static char hx8357c_ba[]={
	0XBA, 0x00, 0x56, 0xd4,//d6->c6, d4
	0x00, 0x0a, 0x00, 0x10,
	0x32, 0x6e, 0x04, 0x05,
	0x9a, 0x14, 0x19, 0x10,
	0x40
};

static char hx8357c_cmd_e9[2] = {0xe9, 0x30};
static char hx8357c_video_e9[2] = {0xe9, 0x20};

static struct dsi_cmd_desc hx8357c_hsd_cmd_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_extention_cmd_enable), hx8357c_extention_cmd_enable},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_vcom_setting), hx8357c_vcom_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(hx8357c_cmd_pixel_format_setting), hx8357c_cmd_pixel_format_setting},//need wait 5ms
	{DTYPE_GEN_LWRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(hx8357c_cmd_e9), hx8357c_cmd_e9},//need wait 5ms
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_set_panel), hx8357c_set_panel},	
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_cmd_rgb_interface), hx8357c_cmd_rgb_interface},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_internal_osc), hx8357c_internal_osc},
	{DTYPE_GEN_LWRITE, 1, 0, 0, MIPI_SETTING_DELAY, sizeof(hx8357c_power_ctrl), hx8357c_power_ctrl},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_disp_cycle), hx8357c_disp_cycle},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_c0), hx8357c_c0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_gamma_setting), hx8357c_gamma_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_ba), hx8357c_ba},	
	{DTYPE_DCS_WRITE, 1, 0, 0, SLEEP_OFF_DELAY, sizeof(hx8357c_sleep_out), hx8357c_sleep_out},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(hx8357c_display_on), hx8357c_display_on},
};
static struct dsi_cmd_desc hx8357c_hsd_video_display_on_cmds[] = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_extention_cmd_enable), hx8357c_extention_cmd_enable},		
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_video_vcom_setting), hx8357c_video_vcom_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_internal_osc), hx8357c_internal_osc},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_set_panel), hx8357c_set_panel},
	{DTYPE_GEN_LWRITE, 1, 0, 0, MIPI_SETTING_DELAY, sizeof(hx8357c_video_power_ctrl), hx8357c_video_power_ctrl},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_disp_cycle), hx8357c_disp_cycle},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_video_rgb_interface), hx8357c_video_rgb_interface},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_video_c0), hx8357c_video_c0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_gamma_setting), hx8357c_gamma_setting},
	{DTYPE_GEN_LWRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(hx8357c_cmd_pixel_format_setting), hx8357c_cmd_pixel_format_setting},//need wait 5ms
	{DTYPE_GEN_LWRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(hx8357c_video_e9), hx8357c_video_e9},//need wait 5ms
	{DTYPE_GEN_LWRITE, 1, 0, 0, CMD_DELAY, sizeof(hx8357c_ba), hx8357c_ba},	
	{DTYPE_DCS_WRITE, 1, 0, 0, SLEEP_OFF_DELAY, sizeof(hx8357c_sleep_out), hx8357c_sleep_out},
	{DTYPE_DCS_WRITE, 1, 0, 0, DISPLAY_ON_DELAY, sizeof(hx8357c_display_on), hx8357c_display_on},
};

static int mipi_hx8357c_hsd_lcd_reset(void)
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

static int mipi_hx8357c_hsd_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	static int first_in = 0;
	pr_debug("mipi_hx8357c_hsd_lcd_on Start\n");
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
	if (mipi_hx8357c_hsd_lcd_reset() < 0) {
        pr_debug("mipi_hx8357c_hsd_lcd_reset error\n");
        return -EINVAL;
    }

	if(current_mode == 0){		
		pr_info("%s, command mode", __func__);
		mipi_dsi_cmds_tx(mfd, &hx8357c_hsd_tx_buf, hx8357c_hsd_cmd_display_on_cmds,
				ARRAY_SIZE(hx8357c_hsd_cmd_display_on_cmds));
	}else{
		pr_info("%s, video mode", __func__);
		mipi_dsi_cmds_tx(mfd, &hx8357c_hsd_tx_buf, hx8357c_hsd_video_display_on_cmds,
					ARRAY_SIZE(hx8357c_hsd_video_display_on_cmds));
	}

	pr_debug("mipi_hx8357c_hsd_lcd_on End\n");

	return 0;
}

static int mipi_hx8357c_hsd_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	pr_debug("mipi_hx8357c_hsd_lcd_off Start\n");

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(mfd, &hx8357c_hsd_tx_buf, hx8357c_display_off_cmds,
			ARRAY_SIZE(hx8357c_display_off_cmds));
#if 0
    if (mipi_hx8357c_hsd_lcd_reset() < 0) {
        pr_debug("mipi_hx8357c_hsd_lcd_reset error\n");
        return -EINVAL;
    }
#endif
	gpio_set_value_cansleep(GPIO_AW551_LCD_RESET, 0);
#ifdef CONFIG_LCT_AW550
	pr_info("AW550 config LCM_ID");
	gpio_tlmm_config(GPIO_CFG(35, 0, 0, 0, 0), 0);//LCM_ID
#endif

	pr_debug("mipi_hx8357c_hsd_lcd_off End\n");

	return 0;
}

static int hx8357c_power_init(void){
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

static int __devinit mipi_hx8357c_hsd_lcd_probe(struct platform_device *pdev)
{
	int ret;
	pr_debug("%s\n", __func__);
	gpio_tlmm_config(GPIO_CFG(GPIO_BACKLIGHT_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),GPIO_CFG_ENABLE);
	ret = hx8357c_power_init();
	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_hx8357c_hsd_lcd_probe,
	.driver = {
		.name   = "mipi_hx8357c_hsd",
	},
};

static void mipi_hx8357c_hsd_set_backlight(struct msm_fb_data_type *mfd)
{
	mipi_backlight_control(mfd);	
}

static struct msm_fb_panel_data hx8357c_hsd_panel_data = {
	.on	= mipi_hx8357c_hsd_lcd_on,
	.off = mipi_hx8357c_hsd_lcd_off,
	.set_backlight = mipi_hx8357c_hsd_set_backlight,
};

static int ch_used[3];

int mipi_hx8357c_hsd_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	pr_debug("mipi_hx8357c_hsd_device_register\n");

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_hx8357c_hsd", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	hx8357c_hsd_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &hx8357c_hsd_panel_data,
		sizeof(hx8357c_hsd_panel_data));
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

void mipi_hx8357c_hsd_sel_mode(int mode)
{
	current_mode = mode;
	pr_info("Macle, current mode = %d", mode);
}


static int __init mipi_hx8357c_hsd_lcd_init(void)
{
	pr_debug("mipi_hx8357c_hsd_lcd_init\n");
	mipi_dsi_buf_alloc(&hx8357c_hsd_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&hx8357c_hsd_rx_buf, DSI_BUF_SIZE);
	return platform_driver_register(&this_driver);
}

module_init(mipi_hx8357c_hsd_lcd_init);
