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
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <asm/mach-types.h>

extern int boot_splash;

/* MDP controller */
#define MDP_BASE 0xAA200000
#define MDP_SIZE 0x000F1008
#define MDP_DSI_VIDEO_EN 0xF0000
#define MDP_INTR_ENABLE 0x020
#define MDP_INTR_CLEAR 0x028
#define LCDC_BASE 0xE0000

/* DSI controller */
#define MIPI_DSI_HW_BASE 0xA1100000
#define MIPI_DSI_HW_SIZE 0x000F0000
#define DSI_INT_CTRL 0x10C
#define DSI_ERR_INT_MASK0 0x108
#define DSIPHY_PLL_CTRL0 0x200
#define DSI_CLK_CTRL 0x118
#define DSI_CTRL 0x000

/* backlight control */
#define SKU1_GPIO_BACKLIGHT_EN 116
#define SKU3_GPIO_BACKLIGHT_EN 89
#define MAX_BACKLIGHT_BRIGHTNESS 255

static int prev_bl = 17;
static int gpio_backlight_en;

#define LATE_DISPLAY_IOC_MAGIC  'Q'
#define QRD_IOCTL_LATE_DISLAY _IO(LATE_DISPLAY_IOC_MAGIC, 1)

static int display_inited = 0;

int late_display_major = 0;

struct late_display_dev {
	struct cdev dev;
};

struct late_display_dev *late_display_devices;
static struct class *late_display_class;

static void late_display_set_backlight(int brightness)
{
	int step = 0, i = 0;
	unsigned long flags;
	int bl_level = brightness;

	/* real backlight level, 1 - max, 16 - min, 17 - off */
	bl_level = 17 - bl_level;

	if (bl_level > prev_bl) {
		step = bl_level - prev_bl;
		if (bl_level == 17) {
			step--;
		}
	} else if (bl_level < prev_bl) {
		step = bl_level + 16 - prev_bl;
	} else {
		//pr_debug("%s: no change\n", __func__);
		return;
	}

	if (bl_level == 17) {
		/* turn off backlight */
		pr_debug("%s: turn off backlight\n", __func__);
		gpio_set_value(gpio_backlight_en, 0);
	} else {
		local_irq_save(flags);

		if (prev_bl == 17) {
			/* turn on backlight */
			pr_debug("%s: turn on backlight\n", __func__);
			gpio_set_value(gpio_backlight_en, 1);
			udelay(30);
		}

		/* adjust backlight level */
		for (i = 0; i < step; i++) {
			gpio_set_value(gpio_backlight_en, 0);
			udelay(1);
			gpio_set_value(gpio_backlight_en, 1);
			udelay(1);
		}

		local_irq_restore(flags);
	}
	msleep(1);

	prev_bl = bl_level;

	return;
}

static void late_display_set_bl_brightness(struct led_classdev *led_cdev,
					enum led_brightness value)
{
	int bl_lvl;

	//pr_debug("%s: value = %d\n", __func__, value);

	if (value > MAX_BACKLIGHT_BRIGHTNESS)
		value = MAX_BACKLIGHT_BRIGHTNESS;

	/* This maps android backlight level 0 to 255 into
	   driver backlight level 0 to bl_max with rounding */
	bl_lvl = (2 * value * 16 + MAX_BACKLIGHT_BRIGHTNESS)
		/(2 * MAX_BACKLIGHT_BRIGHTNESS);

	if (!bl_lvl && value)
		bl_lvl = 1;

	late_display_set_backlight(bl_lvl);
}

static struct led_classdev backlight_led = {
	.name		= "lcd-backlight",
	.brightness	= MAX_BACKLIGHT_BRIGHTNESS,
	.brightness_set	= late_display_set_bl_brightness,
};

int late_display_open(struct inode *inode, struct file *filp)
{
	struct late_display_dev *dev;

	pr_debug("%s\n", __func__);

	dev = container_of(inode->i_cdev, struct late_display_dev, dev);
	filp->private_data = dev;

	return 0;          /* success */
}

int late_display_release(struct inode *inode, struct file *filp)
{
	pr_debug("%s\n", __func__);
	return 0;
}

extern int lcd_camera_power_onoff(int on);
extern int msm_fb_late_init(void);
extern int mdp_driver_late_init(void);
extern int mipi_dsi_driver_late_init(void);
extern int mipi_truly_lcd_late_init(void);
extern int mipi_video_truly_wvga_pt_late_init(void);
extern int mipi_cmd_truly_wvga_pt_late_init(void);
extern int lcdc_driver_late_init(void);
extern int lcdc_truly_panel_late_init(void);

static int mdp_shutdown(void)
{
	unsigned char *mdp_base = ioremap(MDP_BASE, MDP_SIZE);
	if (!mdp_base) {
		printk(KERN_ERR "MDP IOMAP failed\n");
		return -EIO;
	}

	if (machine_is_msm7x27a_sku1()) {
		/* disable LCD controller */
		writel(0x00000000, mdp_base + MDP_DSI_VIDEO_EN);
		msleep(60);
		/* disable MDP interrupts */
		writel(0x00000000, mdp_base + MDP_INTR_ENABLE);
		/* clear MDP interrupts */
		writel(0x01ffffff, mdp_base + MDP_INTR_CLEAR);
	} else if (machine_is_msm7x27a_sku3()) {
		/* disable LCDC controller */
		writel(0, mdp_base + LCDC_BASE + 0x0);
	}

	iounmap(mdp_base);

	return 0;
}

static int mipi_dsi_shutdown(void)
{
	unsigned char *mipi_dsi_base = ioremap(MIPI_DSI_HW_BASE, MIPI_DSI_HW_SIZE);
	if (!mipi_dsi_base) {
		printk(KERN_ERR "DSI IOMAP failed\n");
		return -EIO;
	}

	/* clear DSI interrupts */
	writel(0x01010101, mipi_dsi_base + DSI_INT_CTRL);
	writel(0x00000000, mipi_dsi_base + DSI_INT_CTRL);
	/* clear DSI Err interrupts */
	writel(0x13FF3BFF, mipi_dsi_base + DSI_ERR_INT_MASK0);
	writel(0x00000000, mipi_dsi_base + DSI_ERR_INT_MASK0);
	/* disable DSI PLL */
	writel(0, mipi_dsi_base + DSIPHY_PLL_CTRL0);
	/* disable DSI CLK */
	writel(0, mipi_dsi_base + DSI_CLK_CTRL);
	/* disable DSI controller */
	writel(0, mipi_dsi_base + DSI_CTRL);

	iounmap(mipi_dsi_base);

	return 0;
}

static int display_shutdown(void)
{
	late_display_set_backlight(0);

	if (mdp_shutdown()) {
		return -EIO;
	}

	if (machine_is_msm7x27a_sku1()) {
		if (mipi_dsi_shutdown()) {
			return -EIO;
		}
	}

	/* shutdown display power */
	lcd_camera_power_onoff(0);

	return 0;
}

long late_display_ioctl(struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
	long ret = 0;

	pr_debug("%s\n", __func__);

	switch(cmd) {
	case QRD_IOCTL_LATE_DISLAY:
		pr_debug("%s: cmd = %x\n", __func__, cmd);

		if (boot_splash && !display_inited) {
			/* shutdown display of LK */
			if (display_shutdown()) {
				return -EIO;
			}

			pr_debug("%s: shutdown LK display\n", __func__);

			/* init display for kernel */
			msm_fb_late_init();
			mdp_driver_late_init();

#ifdef CONFIG_FB_MSM_MIPI_DSI
			mipi_dsi_driver_late_init();
#endif
#ifdef CONFIG_FB_MSM_MIPI_DSI_TRULY
			mipi_truly_lcd_late_init();
#endif
#ifdef CONFIG_FB_MSM_MIPI_TRULY_VIDEO_WVGA_PT
			mipi_video_truly_wvga_pt_late_init();
#endif
#ifdef CONFIG_FB_MSM_MIPI_TRULY_CMD_WVGA_PT
			mipi_cmd_truly_wvga_pt_late_init();
#endif
#ifdef CONFIG_FB_MSM_LCDC
			lcdc_driver_late_init();
#endif
#ifdef CONFIG_FB_MSM_LCDC_TRULY_HVGA_IPS3P2335
			lcdc_truly_panel_late_init();
#endif
			display_inited = 1;
		}
		break;

	default:
		pr_debug("%s: unsupported cmd\n", __func__);
		return -EINVAL;
	}

	return ret;
}

struct file_operations late_display_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = late_display_ioctl,
	.open =	late_display_open,
	.release = late_display_release,
};

static int late_display_setup_cdev(struct late_display_dev *dev)
{
	int err = 0;
	int devno = MKDEV(late_display_major, 0);

	pr_debug("%s\n", __func__);

	cdev_init(&dev->dev, &late_display_fops);
	dev->dev.owner = THIS_MODULE;
	dev->dev.ops = &late_display_fops;

	err = cdev_add(&dev->dev, devno, 1);
	if (err) {
		pr_err("Error %d adding late_display", err);
	}

	return err;
}

static int __init late_display_init(void)
{
	int result = 0;
	struct device *device = NULL;
	dev_t dev = MKDEV(late_display_major, 0);

	pr_debug("%s: boot_splash = %d\n", __func__, boot_splash);

	if (boot_splash) {
		result = alloc_chrdev_region(&dev, 0, 1, "late_display");
		late_display_major = MAJOR(dev);
		if (result < 0)
			return result;

		pr_debug("%s: major = %d, minor = %d\n", __func__, MAJOR(dev), MINOR(dev));

		late_display_devices = kmalloc(sizeof(struct late_display_dev), GFP_KERNEL);
		if (!late_display_devices) {
			result = -EIO;
			goto fail_malloc;
		}
		memset(late_display_devices, 0, sizeof(struct late_display_dev));

		late_display_class = class_create(THIS_MODULE, "late_display");
		if (IS_ERR(late_display_class)) {
			result = PTR_ERR(late_display_class);
			pr_err("%s: fail to create late_display_class: %d\n", __func__, result);
			goto fail_class;
		}

		device = device_create(late_display_class, NULL, dev, NULL, "control");
		if (IS_ERR(device)) {
			result = PTR_ERR(device);
			pr_err("%s: error creating late_display device: %d\n", __func__, result);
			goto fail_dev;
		}

		/* late_display control */
		result = late_display_setup_cdev(late_display_devices);
		if (result) {
			goto fail_cdev;
		}

		/* lcd-backlight */
		if (machine_is_msm7x27a_sku1())
			gpio_backlight_en = SKU1_GPIO_BACKLIGHT_EN;
		else if (machine_is_msm7x27a_sku3())
			gpio_backlight_en = SKU3_GPIO_BACKLIGHT_EN;
		else
			gpio_backlight_en = 0xFF;

		pr_debug("%s: register lcd-backlight\n", __func__);
		if ((result = led_classdev_register(NULL, &backlight_led))) {
			pr_err("%s: led_classdev_register failed\n", __func__);
			goto fail_gpio;
		}
	} else {
		/* normal display without splash */
		pr_debug("%s: normal display without splash\n", __func__);

		if (!display_inited) {
			if (mdp_shutdown()) {
				return -EIO;
			}

			if (machine_is_msm7x27a_sku1()) {
				if (mipi_dsi_shutdown()) {
					return -EIO;
				}
			}
			display_inited = 1;
		}
	}

	return result;

fail_gpio:
	gpio_free(gpio_backlight_en);
fail_cdev:
	device_destroy(late_display_class, dev);
fail_dev:
	class_destroy(late_display_class);
fail_class:
	kfree(late_display_devices);
	late_display_devices = NULL;
fail_malloc:
	unregister_chrdev_region(dev, 1);
	return result;
}

static void late_display_cleanup(void)
{
	pr_debug("%s\n", __func__);

	if (boot_splash) {
		/* late display */
		cdev_del(&late_display_devices->dev);
		if (late_display_devices) {
			kfree(late_display_devices);
			late_display_devices = NULL;
		}

		device_destroy(late_display_class, MKDEV(late_display_major, 0));
		class_destroy(late_display_class);
		unregister_chrdev_region(MKDEV(late_display_major, 0), 1);

		/* lcd-backlight */
		gpio_free(gpio_backlight_en);

		led_classdev_unregister(&backlight_led);
	}
}

module_init(late_display_init);
module_exit(late_display_cleanup);

MODULE_LICENSE("GPL v2");
MODULE_VERSION("0.1");
MODULE_AUTHOR("QRD China");
MODULE_DESCRIPTION("LK & Kernel splash screen driver");
