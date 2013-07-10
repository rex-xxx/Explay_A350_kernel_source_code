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

#include "mach/gpio.h"
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_default_panel.h"
#include <mach/rpc_pmapp.h>
#include <mach/vreg.h>
#include "mipi_backlight.h"

static void mipi_default_set_backlight(struct msm_fb_data_type *mfd)
{
	mipi_backlight_control(mfd);
	return;
}

static int mipi_default_lcd_on(struct platform_device *pdev)
{
	return 0;
}

static int mipi_default_lcd_off(struct platform_device *pdev)
{
	return 0;
}


static int __devinit mipi_default_lcd_probe(struct platform_device *pdev)
{
	pr_debug("%s\n", __func__);
	msm_fb_add_device(pdev);

	return 0;
}

static struct msm_fb_panel_data default_panel_data = {
	.on	= mipi_default_lcd_on,
	.off = mipi_default_lcd_off,
	.set_backlight = mipi_default_set_backlight,
};

static struct platform_driver this_driver = {
	.probe  = mipi_default_lcd_probe,
	.driver = {
		.name   = "mipi_default_panel",
	},
};

static int ch_used[3];

int mipi_default_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	pr_debug("mipi_default_device_register\n");

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_default_panel", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	default_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &default_panel_data,
		sizeof(default_panel_data));
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

static int __init mipi_default_lcd_init(void)
{
	pr_debug("mipi_default_lcd_init\n");
	return platform_driver_register(&this_driver);
}

module_init(mipi_default_lcd_init);
