/* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
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

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/leds.h>
#include <mach/gpio.h>
#include <mach/pmic.h>
#include "msm_fb.h"
#include "mipi_backlight.h"

static int first_on = 1;
extern int boot_splash;


static int spi_cs;
static int spi_sclk;
static int spi_mosi;
static int gpio_backlight_en;
static int gpio_display_reset;

struct ili9340n_state_type{
	boolean disp_initialized;
	boolean display_on;
	boolean disp_powered_up;
};

static struct ili9340n_state_type ili9340n_state = { 0 };
static struct msm_panel_common_pdata *lcdc_ili9340n_pdata;


static __inline__ void ili9340n_spi_write_byte(char dc, uint8 data)
{
	uint32 bit;
	int bnum;

	gpio_set_value_cansleep(spi_sclk, 0); /* clk low */
	/* dc: 0 for command, 1 for parameter */
	gpio_set_value_cansleep(spi_mosi, dc);
	udelay(1);	/* at least 20 ns */
	gpio_set_value_cansleep(spi_sclk, 1); /* clk high */
	udelay(1);	/* at least 20 ns */
	bnum = 8;	/* 8 data bits */
	bit = 0x80;
	while (bnum) {
		gpio_set_value_cansleep(spi_sclk, 0); /* clk low */
		if (data & bit)
			gpio_set_value_cansleep(spi_mosi, 1);
		else
			gpio_set_value_cansleep(spi_mosi, 0);
		udelay(1);
		gpio_set_value_cansleep(spi_sclk, 1); /* clk high */
		udelay(1);
		bit >>= 1;
		bnum--;
	}
}

static __inline__ int ili9340n_spi_write(char cmd, char *data, int num)
{
	int i;

	gpio_set_value_cansleep(spi_cs, 0);	/* cs low */
	/* command byte first */
	ili9340n_spi_write_byte(0, cmd);
	/* followed by parameter bytes */
	for(i = 0; i < num; i++)
	{
		if(data)
			ili9340n_spi_write_byte(1, data[i]);
	}
	gpio_set_value_cansleep(spi_mosi, 1);	/* mosi high */
	gpio_set_value_cansleep(spi_cs, 1);	/* cs high */
	udelay(10);
	return 0;
}

static void spi_pin_assign(void)
{
	printk("%s E\n", __func__);

	/* Setting the Default GPIO's */
	spi_mosi	= *(lcdc_ili9340n_pdata->gpio_num);
	spi_sclk	= *(lcdc_ili9340n_pdata->gpio_num + 1);
	spi_cs		= *(lcdc_ili9340n_pdata->gpio_num + 2);
	gpio_backlight_en = *(lcdc_ili9340n_pdata->gpio_num + 3);
	gpio_display_reset = *(lcdc_ili9340n_pdata->gpio_num + 4);
	printk("spi_mosi:%d spi_sclk:%d spi_cs:%d backlight:%d reset:%d\n",
		spi_mosi, spi_sclk, spi_cs, gpio_backlight_en, gpio_display_reset);

	printk("%s X\n", __func__);
}

static void ili9340n_disp_powerup(void)
{
	printk("%s E\n", __func__);

	if (!ili9340n_state.disp_powered_up && !ili9340n_state.display_on) {
		/* Reset the hardware first */
		/* Include DAC power up implementation here */
	      ili9340n_state.disp_powered_up = TRUE;
	}

	printk("%s X\n", __func__);
}

static char init_item_cf[] = { 0x00, 0x21, 0x20, };
static char init_item_b4[] = { 0x02,};
static char init_item_c0[] = { 0x0d, 0x0f, };
static char init_item_c1[] = { 0x01, };
static char init_item_36[] = { 0x08, };
static char init_item_3a[] = { 0x66, };
static char init_item_b6[] = { 0x0A, 0x82, };
static char init_item_c5[] = { 0xe8, };
static char init_item_e8[] = { 0x84, 0x1A, 0x18, };
static char init_item_e0[] = { 
	0x00, 0x00, 0x01, 0x03,
	0x06, 0x0c, 0x36, 0x88,
	0x46, 0x08, 0x0e, 0x0b,
	0x18, 0x1a, 0x00,		
};

static char init_item_e1[] = { 
	0x0f, 0x26, 0x27, 0x03,
	0x11, 0x07, 0x39, 0x47,
	0x49, 0x03, 0x09, 0x0c,
	0x2e, 0x3f, 0x0f,		
};

static char init_item_f6[] = { 0x00, 0x00, 0x06, };
static char init_item_b7[] = { 0x06, };
static char init_item_b0[] = { 0xce, };
static char init_item_b1[] = { 0x00, 0x18, };
static char init_item_2a[] = { 0x00, 0x00, 0x00,0xef};
static char init_item_2b[] = { 0x00, 0x00, 0x01,0x3f,};


static void ili9340n_disp_reginit(void)
{
	printk("%s Edisp_powered_up:%d display_on:%d\n", __func__,ili9340n_state.disp_powered_up, ili9340n_state.display_on);
	if (ili9340n_state.disp_powered_up && !ili9340n_state.display_on) {
		gpio_set_value_cansleep(spi_cs, 1);	/* cs high */
		ili9340n_spi_write(0xCF, init_item_cf, sizeof(init_item_cf));
		ili9340n_spi_write(0xB4, init_item_b4, sizeof(init_item_b4));
		ili9340n_spi_write(0xC0, init_item_c0, sizeof(init_item_c0));
		ili9340n_spi_write(0xC1, init_item_c1, sizeof(init_item_c1));
		ili9340n_spi_write(0x36, init_item_36, sizeof(init_item_36));
		ili9340n_spi_write(0x3a, init_item_3a, sizeof(init_item_3a));
		ili9340n_spi_write(0xb6, init_item_b6, sizeof(init_item_b6));
		ili9340n_spi_write(0xC5, init_item_c5, sizeof(init_item_c5));
		ili9340n_spi_write(0xe8, init_item_e8, sizeof(init_item_e8));
		ili9340n_spi_write(0xe0, init_item_e0, sizeof(init_item_e0));
		ili9340n_spi_write(0xe1, init_item_e1, sizeof(init_item_e1));
		ili9340n_spi_write(0x2a, init_item_2a, sizeof(init_item_2a));
		ili9340n_spi_write(0x2b, init_item_2b, sizeof(init_item_2b));
		ili9340n_spi_write(0xf6, init_item_f6, sizeof(init_item_f6));
		ili9340n_spi_write(0xb7, init_item_b7, sizeof(init_item_b7));
		ili9340n_spi_write(0xb0, init_item_b0, sizeof(init_item_b0));
		ili9340n_spi_write(0xb1, init_item_b1, sizeof(init_item_b1));
		
		ili9340n_spi_write(0x11, NULL, 0);
		mdelay(120);
		ili9340n_spi_write(0x13, NULL, 0);
		ili9340n_spi_write(0x29, NULL, 0);
		ili9340n_spi_write(0x2c, NULL, 0);
		mdelay(120);
		ili9340n_state.display_on = TRUE;
	}
	printk("%s X\n", __func__);
}

extern struct list_head leds_list;

static int late_display_set_brightness(enum led_brightness brightness)
{
    struct led_classdev *led_cdev;
	int found = 0;

    //pr_debug("%s: brightness = %d\n", __func__, brightness);

    list_for_each_entry(led_cdev, &leds_list, node) {
        if (!strncmp(led_cdev->name, "lcd-backlight", 13)) {
			found = 1;
            break;
        }
    }

    if (found) {
        led_brightness_set(led_cdev, brightness);
    } else {
        pr_err("%s: set brightness failed, not found lcd-backlight\n", __func__);
        return -1;
    }

    return 0;
}

static int lcdc_ili9340n_panel_on(struct platform_device *pdev)
{
	printk("Macle %s E\n", __func__);
	/* Configure reset GPIO that drives DAC */
	if (lcdc_ili9340n_pdata->panel_config_gpio)
		lcdc_ili9340n_pdata->panel_config_gpio(1);
	gpio_set_value_cansleep(gpio_display_reset, 1);
	msleep(10);
	gpio_set_value_cansleep(gpio_display_reset, 0);
	msleep(10);
	gpio_set_value_cansleep(gpio_display_reset, 1);
	msleep(120);
	ili9340n_disp_powerup();
	ili9340n_disp_reginit();
	ili9340n_state.disp_initialized = TRUE;

	if (boot_splash && first_on) {
		late_display_set_brightness(LED_FULL);
		first_on = 0;
	}

	printk("Macle %s X\n", __func__);
	return 0;
}

static int lcdc_ili9340n_panel_off(struct platform_device *pdev)
{
	printk("%s E\n", __func__);
	if (ili9340n_state.disp_powered_up && ili9340n_state.display_on) {
		/* Main panel power off (Pull down reset) */
		gpio_set_value_cansleep(gpio_display_reset, 0);
		ili9340n_state.display_on = FALSE;
		ili9340n_state.disp_initialized = FALSE;
	}
	printk("%s X\n", __func__);
	return 0;
}

static void lcdc_ili9340n_set_backlight(struct msm_fb_data_type *mfd)
{
	mipi_backlight_control(mfd);
}

static int __devinit ili9340n_probe(struct platform_device *pdev)
{
	printk("%s E\n", __func__);

	if (pdev->id == 0) {
		lcdc_ili9340n_pdata = pdev->dev.platform_data;
		spi_pin_assign();
		return 0;
	}
	msm_fb_add_device(pdev);

	printk("%s X\n", __func__);
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = ili9340n_probe,
	.driver = {
		.name   = "lcdc_qvga_ili9340n_pt",
	},
};

static struct msm_fb_panel_data ili9340n_panel_data = {
	.on = lcdc_ili9340n_panel_on,
	.off = lcdc_ili9340n_panel_off,
	.set_backlight = lcdc_ili9340n_set_backlight,
};

static struct platform_device this_device = {
	.name   = "lcdc_qvga_ili9340n_pt",
	.id	= 1,
	.dev	= {
		.platform_data = &ili9340n_panel_data,
	}
};

int lcdc_ili9340n_panel_late_init(void)
{
	int ret;
	struct msm_panel_info *pinfo;

	printk("%s E\n", __func__);

	ret = platform_driver_register(&this_driver);
	if (ret)
		return ret;
	pinfo = &ili9340n_panel_data.panel_info;
	pinfo->xres = 240;
	pinfo->yres = 320;
	MSM_FB_SINGLE_MODE_PANEL(pinfo);
	pinfo->type = LCDC_PANEL;
	pinfo->pdest = DISPLAY_1;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 18;
	pinfo->fb_num = 2;
	/* 10Mhz mdp_lcdc_pclk and mdp_lcdc_pad_pcl */
	pinfo->clk_rate = 10240000;
	pinfo->bl_max = 16;
	pinfo->bl_min = 1;

	pinfo->lcdc.h_back_porch = 16;		/* hsw = 8 + hbp=16 */
	pinfo->lcdc.h_front_porch = 4;
	pinfo->lcdc.h_pulse_width = 8;
	pinfo->lcdc.v_back_porch = 7;		/* vsw=1 + vbp = 7 */
	pinfo->lcdc.v_front_porch = 3;
	pinfo->lcdc.v_pulse_width = 1;
	pinfo->lcdc.border_clr = 0;			/* blk */
	pinfo->lcdc.underflow_clr = 0xff;	/* blue */
	pinfo->lcdc.hsync_skew = 0;

	ret = platform_device_register(&this_device);
	if (ret) {
		printk(KERN_ERR "%s not able to register the device\n",
			 __func__);
		goto fail_driver;
	}

	printk("%s X\n", __func__);
	return ret;

fail_driver:
	platform_driver_unregister(&this_driver);
	return ret;
}

EXPORT_SYMBOL(lcdc_ili9340n_panel_late_init);

static int __init lcdc_ili9340n_panel_init(void)
{
	if (boot_splash) {
		/* boot splash, do nothing */
	} else {
		lcdc_ili9340n_panel_late_init();
	}

	return 0;
}
module_init(lcdc_ili9340n_panel_init);
