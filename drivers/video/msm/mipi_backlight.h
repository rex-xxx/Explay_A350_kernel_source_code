#ifndef MIPI_BACKLIGHT_H
#define MIPI_BACKLIGHT_H

#include "msm_fb.h"
#include "mach/gpio.h"
#include <mach/rpc_pmapp.h>
#include <mach/vreg.h>
#if defined (CONFIG_LCT_AE770)
#define GPIO_BACKLIGHT_EN 96
#elif defined (CONFIG_LCT_AW320)
#define GPIO_BACKLIGHT_EN 14
#else
#define GPIO_BACKLIGHT_EN 116
#endif
#define BACKLIGHT_CHIPLEVEL_MAX 32
extern void mipi_backlight_control(struct msm_fb_data_type *mfd);
extern void mipi_backlight_turnoff(void);
#endif
