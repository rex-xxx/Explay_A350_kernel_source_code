/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include <media/msm_camera.h>
#include <mach/gpio.h>
#include <mach/camera.h>
#include "ov2655.h"
/******************************************************************************
Debug Switch
******************************************************************************/
#ifdef CDBG
#undef CDBG
#endif
#ifdef CDBG_HIGH
#undef CDBG_HIGH
#endif

#define OV2655_VERBOSE_DGB

#ifdef OV2655_VERBOSE_DGB
#define CDBG(fmt, args...)        printk(fmt, ##args)
#define CDBG_HIGH(fmt, args...)   printk(fmt, ##args)
#else
#define CDBG(fmt, args...)        do { } while (0)
#define CDBG_HIGH(fmt, args...)   pr_debug(fmt, ##args)
#endif

/******************************************************************************
Local Definition
******************************************************************************/
#define OV2655_I2C_MAX_TIMES 10
#define OV2655_MASTER_CLK_RATE             24000000

static int ov2655_pwdn_gpio;
static int ov2655_reset_gpio;
static int ov2655_vcm_pwd_gpio;

static unsigned int ov2655_preview_shutter;
static unsigned int ov2655_preview_gain16;
static unsigned short ov2655_preview_binning;
static unsigned int ov2655_preview_sysclk;
static unsigned int ov2655_preview_HTS;

static unsigned int OV2655_CAMERA_WB_AUTO = 0;
static unsigned int OV2655_preview_R_gain;
static unsigned int OV2655_preview_G_gain;
static unsigned int OV2655_preview_B_gain;

static int OV2655_CSI_CONFIG = 0;

struct ov2655_work {
  struct work_struct work;
};
static struct ov2655_work *ov2655_sensorw;
static struct i2c_client    *ov2655_client;
static DECLARE_WAIT_QUEUE_HEAD(ov2655_wait_queue);
DEFINE_MUTEX(ov2655_mutex);
static u8 ov2655_i2c_buf[4] = {0};
static u8 ov2655_counter = 0;
static int16_t ov2655_effect = CAMERA_EFFECT_OFF;
static int is_autoflash = 0;
static int ov2655_effect_value = 0;

struct __ov2655_ctrl
{
  const struct msm_camera_sensor_info *sensordata;
  int sensormode;
  uint fps_divider;
  uint pict_fps_divider;
  u16 curr_step_pos;
  u16 curr_lens_pos;
  u16 init_curr_lens_pos;
  u16 my_reg_gain;
  u16 my_reg_line_count;
  enum ov2655_resolution_t prev_res;
  enum ov2655_resolution_t pict_res;
  enum ov2655_resolution_t curr_res;
  enum ov2655_test_mode_t set_test;
};
static struct __ov2655_ctrl *ov2655_ctrl;
static int afinit = 1;

extern struct rw_semaphore leds_list_lock;
extern struct list_head leds_list;
extern int lcd_camera_power_onoff(int on);

/******************************************************************************
Local Function Declare
******************************************************************************/
static int ov2655_i2c_txdata(u16 saddr,u8 *txdata,int length);
static int ov2655_i2c_write_byte(unsigned short saddr,
                                  unsigned int waddr,
                                  unsigned int wdata);
static int ov2655_i2c_rxdata(unsigned short saddr,
                              unsigned char *rxdata,
                              int length);
static int32_t ov2655_i2c_read_byte(unsigned short saddr,
                                     unsigned int raddr,
                                     unsigned int *rdata);
static int32_t OV2655_WriteRegsTbl(POV2655_WREG pTb,int32_t len);
static void ov2655_stream_switch(int bOn);
static int ov2655_set_flash_light(enum led_brightness brightness);
static int ov2655_sensor_setting(int update_type, int rt);
static int ov2655_video_config(int mode);
static int ov2655_snapshot_config(int mode);
static int ov2655_raw_snapshot_config(int mode);
static int ov2655_sensor_open_init(const struct msm_camera_sensor_info *data);
static int ov2655_sensor_release(void);
static int ov2655_i2c_remove(struct i2c_client *client);
static int ov2655_init_client(struct i2c_client *client);
static int ov2655_set_sensor_mode(int mode,int res);
static long ov2655_set_effect(int mode, int effect);
static void ov2655_power_off(void);
static int ov2655_set_brightness(int8_t brightness);
static int ov2655_set_contrast(int contrast);
static long ov2655_set_exposure_mode(int mode);
static int ov2655_set_wb_oem(uint8_t param);
static long ov2655_set_antibanding(int antibanding);
static int32_t ov2655_lens_shading_enable(uint8_t is_enable);
static int ov2655_set_saturation(int saturation);
static int ov2655_set_sharpness(int sharpness);
static int ov2655_set_touchaec(uint32_t x,uint32_t y);
static int ov2655_sensor_start_af(void);
static int ov2655_set_exposure_compensation(int compensation);
static int ov2655_set_iso(int8_t iso_type);
static int ov2655_sensor_config(void __user *argp);
static void ov2655_power_on(void);
static void ov2655_power_reset(void);
static int ov2655_read_model_id(const struct msm_camera_sensor_info *data);
static int ov2655_probe_init_gpio(const struct msm_camera_sensor_info *data);
static void ov2655_probe_free_gpio(const struct msm_camera_sensor_info *data);
static int ov2655_sensor_probe(const struct msm_camera_sensor_info *info,
                                struct msm_sensor_ctrl *s);
static int ov2655_i2c_probe(struct i2c_client *client,
                             const struct i2c_device_id *id);
static int __ov2655_probe(struct platform_device *pdev);
static int __init ov2655_init(void);
/******************************************************************************

******************************************************************************/
static int ov2655_i2c_txdata(u16 saddr,u8 *txdata,int length)
{
  struct i2c_msg msg[] = {
    {
      .addr  = saddr,
      .flags = 0,
      .len = length,
      .buf = txdata,
    },
  };
  if (i2c_transfer(ov2655_client->adapter, msg, 1) < 0)
    return -EIO;
  else
    return 0;
}

/******************************************************************************

******************************************************************************/
static int ov2655_i2c_write_byte(unsigned short saddr,
                                  unsigned int waddr,
                                  unsigned int wdata)
{
  int rc = -EIO;
  ov2655_counter = 0;
  ov2655_i2c_buf[0] = (waddr & 0xFF00)>>8;
  ov2655_i2c_buf[1] = (waddr & 0x00FF);
  ov2655_i2c_buf[2] = (wdata & 0x00FF);
  while ((ov2655_counter < OV2655_I2C_MAX_TIMES) &&(rc != 0))
  {
    rc = ov2655_i2c_txdata(saddr, ov2655_i2c_buf, 3);
    if (rc < 0)
    {
      ov2655_counter++;
      CDBG("ov2655_i2c_txdata failed,Reg(0x%04x) = 0x%02x, rc=%d!\n",
            waddr,wdata,rc);
    }
  }
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_i2c_rxdata(unsigned short saddr,
                              unsigned char *rxdata,
                              int length)
{
    struct i2c_msg msgs[] = {
        {
            .addr   = saddr,
            .flags = 0,
            .len   = 2,
            .buf   = rxdata,
        },
        {
            .addr   = saddr,
            .flags = I2C_M_RD,
            .len   = length,
            .buf   = rxdata,
        },
    };

    if (i2c_transfer(ov2655_client->adapter, msgs, 2) < 0) {
        CDBG("ov2655_i2c_rxdata failed!\n");
        return -EIO;
    }

    return 0;
}

/******************************************************************************


******************************************************************************/
static int32_t ov2655_i2c_read_byte(unsigned short saddr,
                                     unsigned int raddr,
                                     unsigned int *rdata)
{
    int32_t rc = 0;
    unsigned char buf[4] = {0};

    if (!rdata)
        return -EIO;

    buf[0] = (raddr & 0xFF00)>>8;
    buf[1] = (raddr & 0x00FF);

    rc = ov2655_i2c_rxdata(saddr, buf, 1);
    if (rc < 0) {
        CDBG("ov2655_i2c_read_byte failed!\n");
        return rc;
    }

    *rdata = buf[0];
    return rc;
}

/******************************************************************************

******************************************************************************/
static int32_t OV2655_WriteRegsTbl(POV2655_WREG pTb,int32_t len)
{
  int32_t i, ret = 0;
  unsigned int regVal = 0;
  for (i = 0;i < len; i++)
  {
    if (0 == pTb[i].data_format){
      /*Reg Data Format:16Bit = 0*/
      CDBG("OV2655_WriteRegsTbl: wrong data_format\n");
    } else {
      /*Reg Data Format:8Bit = 1*/
      if(0 == pTb[i].delay_mask){
        ov2655_i2c_write_byte(ov2655_client->addr,pTb[i].addr,pTb[i].data);
      }else{
        ov2655_i2c_read_byte(ov2655_client->addr,pTb[i].addr,&regVal);
        regVal &= (~pTb[i].delay_mask);
        regVal |= (pTb[i].data & pTb[i].delay_mask);
        ov2655_i2c_write_byte(ov2655_client->addr,pTb[i].addr,regVal);
      }
    }
  }
  return ret;
}

/******************************************************************************

******************************************************************************/
static void ov2655_stream_switch(int bOn)
{
  if (bOn == 0) {
    CDBG("ov2655_stream_switch: off\n");
    ov2655_i2c_write_byte(ov2655_client->addr,0x3104, 0x20);
  }
  else {
    CDBG("ov2655_stream_switch: on\n");
    ov2655_i2c_write_byte(ov2655_client->addr,0x3104, 0x00);
  }
}

/******************************************************************************

******************************************************************************/

static int ov2655_set_flash_light(enum led_brightness brightness)
{
  CDBG("%s brightness = %d\n",__func__ , brightness);
  #if 0
  down_read(&leds_list_lock);
  list_for_each_entry(led_cdev, &leds_list, node) {
    if (!strncmp(led_cdev->name, "flashlight", 10)) {
      break;
    }
  }
  up_read(&leds_list_lock);
  if (led_cdev) {
    led_brightness_set(led_cdev, brightness);
  } else {
    CDBG("get flashlight device failed\n");
    return -1;
  }
  #endif
  return 0;
}

/******************************************************************************

******************************************************************************/
static unsigned int OV2655_get_shutter(void)
{
  // read shutter, in number of line period
  unsigned int shutter = 0, extra_line = 0;
  unsigned int ret_l,ret_h;
  ret_l = ret_h = 0;
  ov2655_i2c_read_byte(ov2655_client->addr,0x3002, &ret_h);
  ov2655_i2c_read_byte(ov2655_client->addr,0x3003, &ret_l);
  shutter = (ret_h << 8) | (ret_l & 0xff) ;
  ret_l = ret_h = 0;
  ov2655_i2c_read_byte(ov2655_client->addr,0x302d, &ret_h);
  ov2655_i2c_read_byte(ov2655_client->addr,0x302e, &ret_l);
  extra_line = (ret_h << 8) | (ret_l & 0xff) ;
  return shutter + extra_line;
}

/******************************************************************************

******************************************************************************/
static int OV2655_set_shutter(unsigned int shutter)
{
  // write shutter, in number of line period
  int rc = 0;
  unsigned int temp;
  shutter = shutter & 0xffff;
  temp = shutter & 0xff;
  ov2655_i2c_write_byte(ov2655_client->addr, 0x3003, temp);
  temp = shutter >> 8;
  ov2655_i2c_write_byte(ov2655_client->addr, 0x3002, temp);
  return rc;
}

/******************************************************************************

******************************************************************************/
static unsigned int OV2655_get_gain16(void)
{
  unsigned int gain16, temp;
  temp = 0;
  ov2655_i2c_read_byte(ov2655_client->addr,0x3000, &temp);
  CDBG("%s:Reg(0x3000) = 0x%x\n",__func__,temp);
  gain16 = ((temp>>4) + 1) * (16 + (temp & 0x0f));
  return gain16;
}

/******************************************************************************

******************************************************************************/
static int OV2655_set_gain16(unsigned int gain16)
{
  int rc = 0;
  unsigned int reg;
  gain16 = gain16 & 0x1ff;	// max gain is 32x
  reg = 0;
  if (gain16 > 32){
    gain16 = gain16 /2;
    reg = 0x10;
  }
  if (gain16 > 32){
    gain16 = gain16 /2;
    reg = reg | 0x20;
  }
  if (gain16 > 32){
    gain16 = gain16 /2;
    reg = reg | 0x40;
  }
  if (gain16 > 32){
    gain16 = gain16 /2;
    reg = reg | 0x80;
  }
  reg = reg | (gain16 -16);
  rc = ov2655_i2c_write_byte(ov2655_client->addr,0x3000,reg + 1);
  msleep(100);
  rc |= ov2655_i2c_write_byte(ov2655_client->addr,0x3000,reg);
  return rc;
}

/******************************************************************************

******************************************************************************/
static unsigned int OV2655_get_sysclk(void)
{
  // calculate sysclk
  unsigned int temp1, temp2, XVCLK;
  unsigned int Indiv2x, Bit8Div, FreqDiv2x, PllDiv, SensorDiv, ScaleDiv, DvpDiv, ClkDiv, VCO, sysclk;
  unsigned int Indiv2x_map[] = { 2, 3, 4, 6, 4, 6, 8, 12};
  unsigned int Bit8Div_map[] = { 1, 1, 4, 5};
  unsigned int FreqDiv2x_map[] = { 2, 3, 4, 6};
  unsigned int DvpDiv_map[] = { 1, 2, 8, 16};
  ov2655_i2c_read_byte(ov2655_client->addr,0x300e, &temp1);
  // bit[5:0] PllDiv
  PllDiv = 64 - (temp1 & 0x3f);
  ov2655_i2c_read_byte(ov2655_client->addr,0x300f, &temp1);
  // bit[2:0] Indiv
  temp2 = temp1 & 0x07;
  Indiv2x = Indiv2x_map[temp2];
  // bit[5:4] Bit8Div
  temp2 = (temp1 >> 4) & 0x03;
  Bit8Div = Bit8Div_map[temp2];
  // bit[7:6] FreqDiv
  temp2 = temp1 >> 6;
  FreqDiv2x = FreqDiv2x_map[temp2];
  ov2655_i2c_read_byte(ov2655_client->addr,0x3010, &temp1);
  //bit[3:0] ScaleDiv
  temp2 = temp1 & 0x0f;
  if(temp2==0) {
    ScaleDiv = 1;
  } else {
    ScaleDiv = temp2 * 2;
  }
  // bit[4] SensorDiv
  if(temp1 & 0x10) {
    SensorDiv = 2;
  } else {
    SensorDiv = 1;
  }
  // bit[5] LaneDiv
  // bit[7:6] DvpDiv
  temp2 = temp1 >> 6;
  DvpDiv = DvpDiv_map[temp2];
  ov2655_i2c_read_byte(ov2655_client->addr,0x3011, &temp1);
  // bit[5:0] ClkDiv
  temp2 = temp1 & 0x3f;
  ClkDiv = temp2 + 1;
  XVCLK = OV2655_MASTER_CLK_RATE/10000;
  CDBG("%s:XVCLK = 0x%x\n",__func__,XVCLK);
  CDBG("%s:Bit8Div = 0x%x\n",__func__,Bit8Div);
  CDBG("%s:FreqDiv2x = 0x%x\n",__func__,FreqDiv2x);
  CDBG("%s:PllDiv = 0x%x\n",__func__,PllDiv);
  CDBG("%s:Indiv2x = 0x%x\n",__func__,Indiv2x);
  VCO = XVCLK * Bit8Div * FreqDiv2x * PllDiv / Indiv2x ;
  sysclk = VCO / Bit8Div / SensorDiv / ClkDiv / 4;
  CDBG("%s:ClkDiv = 0x%x\n",__func__,ClkDiv);
  CDBG("%s:SensorDiv = 0x%x\n",__func__,SensorDiv);
  CDBG("%s:sysclk = 0x%x\n",__func__,sysclk);
  return sysclk;
}

/******************************************************************************

******************************************************************************/
static unsigned int OV2655_get_HTS(void)
{
  // read HTS from register settings
  unsigned int HTS, extra_HTS;
  unsigned int ret_l,ret_h;
  ret_l = ret_h = 0;
  ov2655_i2c_read_byte(ov2655_client->addr,0x3028, &ret_h);
  ov2655_i2c_read_byte(ov2655_client->addr,0x3029, &ret_l);
  HTS = (ret_h << 8) | (ret_l & 0xff) ;
  ov2655_i2c_read_byte(ov2655_client->addr,0x302c, &ret_l);
  extra_HTS = ret_l;
  return HTS + extra_HTS;
}

/******************************************************************************

******************************************************************************/
static unsigned int OV2655_get_VTS(void)
{
  // read VTS from register settings
  unsigned int VTS, extra_VTS;
  unsigned int ret_l,ret_h;
  ret_l = ret_h = 0;
  ov2655_i2c_read_byte(ov2655_client->addr,0x302a, &ret_h);
  ov2655_i2c_read_byte(ov2655_client->addr,0x302b, &ret_l);
  VTS = (ret_h << 8) | (ret_l & 0xff) ;
  ov2655_i2c_read_byte(ov2655_client->addr,0x302d, &ret_h);
  ov2655_i2c_read_byte(ov2655_client->addr,0x302e, &ret_l);
  extra_VTS = (ret_h << 8) | (ret_l & 0xff) ;
  return VTS + extra_VTS;
}

/******************************************************************************

******************************************************************************/
static int OV2655_set_VTS(unsigned int VTS)
{
  // write VTS to registers
  int rc = 0;
  unsigned int temp;
  temp = VTS & 0xff;
  rc = ov2655_i2c_write_byte(ov2655_client->addr, 0x302b, temp);
  temp = VTS>>8;
  rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x302a, temp);
  return rc;
}

/******************************************************************************

******************************************************************************/
static unsigned int OV2655_get_binning(void)
{
  // write VTS to registers
  unsigned int temp, binning;
  ov2655_i2c_read_byte(ov2655_client->addr,0x300b, &temp);
  if(temp==0x52){
    // OV2650
    binning = 2;
  } else {
    // OV2655
    binning = 1;
  }
  return binning;
}

/******************************************************************************

******************************************************************************/
static unsigned int OV2655_get_light_frequency(void)
{
  // get banding filter value
  unsigned int temp, light_frequency;
  ov2655_i2c_read_byte(ov2655_client->addr,0x3014, &temp);
  if (temp & 0x40) {
    // auto
    ov2655_i2c_read_byte(ov2655_client->addr,0x508e, &temp);
    if (temp & 0x01){
      light_frequency = 50;
    } else {
      light_frequency = 60;
    }
  } else {
    // manual
    if (temp & 0x80){
      // 50Hz
      light_frequency = 50;
    } else {
      // 60Hz
      light_frequency = 60;
    }
  }
  return light_frequency;
}

/******************************************************************************

******************************************************************************/
static int OV2655_set_bandingfilter(void)
{
  int rc = 0;
  unsigned int preview_VTS;
  unsigned int band_step60, max_band60, band_step50, max_band50;
  // read preview PCLK
  ov2655_preview_sysclk = OV2655_get_sysclk();
  // read preview HTS
  ov2655_preview_HTS = OV2655_get_HTS();
  // read preview VTS
  preview_VTS = OV2655_get_VTS();
  // calculate banding filter
  CDBG("%s:ov2655_preview_sysclk = 0x%x\n",__func__,ov2655_preview_sysclk);
  CDBG("%s:ov2655_preview_HTS = 0x%x\n",__func__,ov2655_preview_HTS);
  CDBG("%s:preview_VTS = 0x%x\n",__func__,preview_VTS);
  // 60Hz
  band_step60 = ov2655_preview_sysclk * 100/ov2655_preview_HTS * 100/120;
  rc = ov2655_i2c_write_byte(ov2655_client->addr, 0x3073, (band_step60 >> 8));
  rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x3072, (band_step60 & 0xff));
  max_band60 = ((preview_VTS-4)/band_step60);
  rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x301d, max_band60-1);
  // 50Hz
  CDBG("%s:band_step60 = 0x%x\n",__func__,band_step60);
  CDBG("%s:max_band60 = 0x%x\n",__func__,max_band60);
  band_step50 = ov2655_preview_sysclk * 100/ov2655_preview_HTS;
  rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x3071, (band_step50 >> 8));
  rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x3070, (band_step50 & 0xff));
  max_band50 = ((preview_VTS-4)/band_step50);
  rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x301c, max_band50-1);
  CDBG("%s:band_step50 = 0x%x\n",__func__,band_step50 );
  CDBG("%s:max_band50 = 0x%x\n",__func__,max_band50);
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_set_nightmode(int NightMode)

{
  int rc = 0;
  unsigned int temp;
  switch (NightMode) {
    case 0:{//Off
        ov2655_i2c_read_byte(ov2655_client->addr,0x3014, &temp);
  CDBG("%s: case 0\n",__func__ );
       // temp = temp & 0xf7;			// night mode off, bit[3] = 0
        temp = temp | 0x08;
        ov2655_i2c_write_byte(ov2655_client->addr, 0x3014, temp);
        // clear dummy lines
        ov2655_i2c_write_byte(ov2655_client->addr, 0x302d, 0);
        ov2655_i2c_write_byte(ov2655_client->addr, 0x302e, 0);
      }
      break;
    case 1: {// On
  CDBG("%s: case 1\n",__func__ );

        ov2655_i2c_read_byte(ov2655_client->addr,0x3014, &temp);
        temp = temp | 0x08;			// night mode on, bit[3] = 1
        ov2655_i2c_write_byte(ov2655_client->addr, 0x3014, temp);
      }
      break;
    default:
      break;
  }
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_get_preview_exposure_gain(void)
{
  int rc = 0;
  ov2655_preview_shutter = OV2655_get_shutter();
  // read preview gain
  ov2655_preview_gain16 = OV2655_get_gain16();
  ov2655_preview_binning = OV2655_get_binning();
  // turn off night mode for capture
  rc = ov2655_set_nightmode(0);
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_set_preview_exposure_gain(void)
{
  int rc = 0;
  rc = OV2655_set_shutter(ov2655_preview_shutter);
  rc = OV2655_set_gain16(ov2655_preview_gain16);
  if(OV2655_CAMERA_WB_AUTO)
  {
//	rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x3306, 0x00); //set to WB_AUTO
  }
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_set_capture_exposure_gain(void)
{
  int rc = 0;
  unsigned int capture_shutter, capture_gain16, capture_sysclk, capture_HTS, capture_VTS;
  unsigned int light_frequency, capture_bandingfilter, capture_max_band;
  unsigned long capture_gain16_shutter;
  unsigned int temp;

  //Step3: calculate and set capture exposure and gain
  // turn off AEC, AGC
  ov2655_i2c_read_byte(ov2655_client->addr,0x3013, &temp);
  temp = temp & 0xfa;
  ov2655_i2c_write_byte(ov2655_client->addr, 0x3013, temp);
  // read capture sysclk
  capture_sysclk = OV2655_get_sysclk();
  // read capture HTS
  capture_HTS = OV2655_get_HTS();
  // read capture VTS
  capture_VTS = OV2655_get_VTS();
  // calculate capture banding filter
  light_frequency = OV2655_get_light_frequency();
  if (light_frequency == 60) {
    // 60Hz
    capture_bandingfilter = capture_sysclk * 100 / capture_HTS * 100 / 120;
  } else {
    // 50Hz
    capture_bandingfilter = capture_sysclk * 100 / capture_HTS;
  }
  capture_max_band = ((capture_VTS-4)/capture_bandingfilter);
  // calculate capture shutter
  capture_shutter = ov2655_preview_shutter;
  // gain to shutter
  capture_gain16 = ov2655_preview_gain16 * capture_sysclk/ov2655_preview_sysclk
  * ov2655_preview_HTS/capture_HTS * ov2655_preview_binning;
  if (capture_gain16 < 16) {
    capture_gain16 = 16;
  }
  capture_gain16_shutter = capture_gain16 * capture_shutter;
  if(capture_gain16_shutter < (capture_bandingfilter * 16)) {
    // shutter < 1/100
    capture_shutter = capture_gain16_shutter/16;
    capture_gain16 = capture_gain16_shutter/capture_shutter;
  } else {
    if(capture_gain16_shutter > (capture_bandingfilter*capture_max_band*16)) {
      // exposure reach max
      capture_shutter = capture_bandingfilter*capture_max_band;
      capture_gain16 = capture_gain16_shutter / capture_shutter;
    } else {
      // 1/100 < capture_shutter < max, capture_shutter = n/100
      capture_shutter = (capture_gain16_shutter/16/capture_bandingfilter)
        * capture_bandingfilter;
      capture_gain16 = capture_gain16_shutter / capture_shutter;
    }
  }
  // write capture gain
  rc |= OV2655_set_gain16(capture_gain16);
  // write capture shutter
  if (capture_shutter > (capture_VTS - 4)) {
    capture_VTS = capture_shutter + 4;
    rc |= OV2655_set_VTS(capture_VTS);
  }
  rc |= OV2655_set_shutter(capture_shutter);
  if(OV2655_CAMERA_WB_AUTO)
  {
    rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x3306, 0x02);
    rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x3337, OV2655_preview_R_gain);
    rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x3338, OV2655_preview_G_gain);
    rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x3339, OV2655_preview_B_gain);
  }
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_sensor_setting(int update_type, int rt)
{
  int rc = -EINVAL;
  unsigned int temp = 0;
  struct msm_camera_csi_params ov2655_csi_params;
  CDBG("update_type = %d, rt = %d\n", update_type, rt);
  if (update_type == S_REG_INIT) {
    rc = ov2655_i2c_write_byte(ov2655_client->addr, 0x3012, 0x80);
    msleep(5);
    rc = OV2655_WriteSettingTbl(ov2655_init_tbl);
    if (rc < 0) {
      CDBG("sensor init setting failed\n");
      return rc;
    }
    CDBG("AF_init: afinit = %d\n", afinit);
    if (afinit == 1) {
      rc = OV2655_WriteSettingTbl(ov2655_afinit_tbl);
      if (rc < 0) {
        CDBG_HIGH("ov2655 af setting failed\n");
        return rc;
      }
      afinit = 0;
    }
    /* reset fps_divider */
    ov2655_ctrl->fps_divider = 1 * 0x0400;
	ov2655_preview_shutter = OV2655_get_shutter();
	ov2655_preview_gain16 = OV2655_get_gain16();
    rc = 0;
  } else if (update_type == S_UPDATE_PERIODIC) {
    if(OV2655_CAMERA_WB_AUTO)
    {
      rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x330c, 0x02);
      ov2655_i2c_read_byte(ov2655_client->addr,0x330f, &OV2655_preview_R_gain);
      rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x330c, 0x03);
      ov2655_i2c_read_byte(ov2655_client->addr,0x330f, &OV2655_preview_G_gain);
      rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x330c, 0x04);
      ov2655_i2c_read_byte(ov2655_client->addr,0x330f, &OV2655_preview_B_gain);
    }
    ov2655_stream_switch(0); //stream off
    msleep(20);
    if(!OV2655_CSI_CONFIG) {
      ov2655_csi_params.lane_cnt = 1;
      ov2655_csi_params.data_format = CSI_8BIT;
      ov2655_csi_params.lane_assign = 0xe4;
      ov2655_csi_params.dpcm_scheme = 0;
      ov2655_csi_params.settle_cnt = 0x14;
      rc = msm_camio_csi_config(&ov2655_csi_params);
      msleep(20);
      OV2655_CSI_CONFIG = 1;
    }
    //turn off flash when preview
    ov2655_set_flash_light(LED_OFF);
    if (rt == S_QTR_SIZE) {
      /* preview setting */
      rc = OV2655_WriteSettingTbl(ov2655_preview_tbl_30fps);
      if(rc < 0){
	CDBG("%s, ov2655_preview_tbl_30fps set failed\n",__func__);
      }	      
	rc |= OV2655_set_bandingfilter();
      // turn on AEC, AGC
      rc |= ov2655_i2c_read_byte(ov2655_client->addr,0x3013, &temp);
      temp = temp | 0x05;
      rc |= ov2655_i2c_write_byte(ov2655_client->addr, 0x3013, temp);
      ov2655_set_preview_exposure_gain();
	msleep(250);
      CDBG("%s, ov2655_preview_tbl_30fps is set\n",__func__);
    } else if (rt == S_FULL_SIZE) {
      CDBG("snapshot in, is_autoflash - %d\n", is_autoflash);
      ov2655_get_preview_exposure_gain();
      if (is_autoflash == 1) {
        ov2655_set_flash_light(LED_FULL);
      }
	msleep(150);
      rc = OV2655_WriteSettingTbl(ov2655_capture_tbl);
      CDBG("%s, ov2655_capture_tbl is set\n",__func__);
      ov2655_set_capture_exposure_gain();
    }
    msleep(100);
      if(rc < 0){
	CDBG("%s, ov2655_reg set failed\n",__func__);
      }	      
    ov2655_stream_switch(1); //stream on
  }
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_video_config(int mode)
{
  int rc = 0;
  CDBG("%s\n",__func__);
  if ((rc = ov2655_sensor_setting(S_UPDATE_PERIODIC,
    ov2655_ctrl->prev_res)) < 0) {
    CDBG("ov2655_video_config failed\n");
    return rc;
  }
  ov2655_ctrl->curr_res = ov2655_ctrl->prev_res;
  ov2655_ctrl->sensormode = mode;
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_snapshot_config(int mode)
{
  int rc = 0;
  CDBG("%s\n",__func__);
  if (ov2655_ctrl->curr_res != ov2655_ctrl->pict_res) {
    if ((rc = ov2655_sensor_setting(S_UPDATE_PERIODIC,
      ov2655_ctrl->pict_res)) < 0) {
      CDBG("ov2655_snapshot_config failed\n");
      return rc;
    }
  }
  ov2655_ctrl->curr_res = ov2655_ctrl->pict_res;
  ov2655_ctrl->sensormode = mode;
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_raw_snapshot_config(int mode)
{
  int rc = 0;
  CDBG("%s\n",__func__);
  if (ov2655_ctrl->curr_res != ov2655_ctrl->pict_res) {
    if ((rc = ov2655_sensor_setting(S_UPDATE_PERIODIC,
        ov2655_ctrl->pict_res)) < 0) {
      CDBG("ov2655_raw_snapshot_config failed\n");
      return rc;
    }
  }
  ov2655_ctrl->curr_res = ov2655_ctrl->pict_res;
  ov2655_ctrl->sensormode = mode;
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_sensor_open_init(const struct msm_camera_sensor_info *data)
{
  int rc = -ENOMEM;
  CDBG("%s\n",__func__);

  	data->pdata->camera_vreg_config(1,1500,1);

  ov2655_ctrl = kzalloc(sizeof(struct __ov2655_ctrl), GFP_KERNEL);
  if (!ov2655_ctrl)
  {
    CDBG("kzalloc ov2655_ctrl error\n");
    return rc;
  }
  ov2655_ctrl->fps_divider = 1 * 0x00000400;
  ov2655_ctrl->pict_fps_divider = 1 * 0x00000400;
  ov2655_ctrl->set_test = S_TEST_OFF;
  ov2655_ctrl->prev_res = S_QTR_SIZE;
  ov2655_ctrl->pict_res = S_FULL_SIZE;
  if (data)
    ov2655_ctrl->sensordata = data;
  lcd_camera_power_onoff(1);
  ov2655_power_off();
  CDBG("%s: msm_camio_clk_rate_set\n",__func__);
  msm_camio_clk_rate_set(OV2655_MASTER_CLK_RATE);
  msleep(5);
  ov2655_power_on();
  msleep(5);
  ov2655_power_reset();
  msleep(5);
  rc = ov2655_sensor_setting(S_REG_INIT, ov2655_ctrl->prev_res);
  if (rc < 0)
  {
    CDBG("%s :ov2655_sensor_setting failed. rc = %d\n",__func__,rc);
    kfree(ov2655_ctrl);
    ov2655_ctrl = NULL;
    ov2655_power_off();
    lcd_camera_power_onoff(0);
    return rc;
  }
  OV2655_CSI_CONFIG = 0;
  CDBG("%s: re_init_sensor ok\n",__func__);
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_sensor_release(void)
{
  CDBG("%s\n",__func__);
  mutex_lock(&ov2655_mutex);
  ov2655_power_off();
  lcd_camera_power_onoff(0);
  if(NULL != ov2655_ctrl)
  {
    kfree(ov2655_ctrl);
    ov2655_ctrl = NULL;
  }
  OV2655_CSI_CONFIG = 0;
  mutex_unlock(&ov2655_mutex);
  return 0;
}

/******************************************************************************

******************************************************************************/

static const struct i2c_device_id ov2655_i2c_id[] = {
  {"ov2655", 0},{}
};

/******************************************************************************

******************************************************************************/
static int ov2655_i2c_remove(struct i2c_client *client)
{
  if(NULL != ov2655_sensorw)
  {
    kfree(ov2655_sensorw);
    ov2655_sensorw = NULL;
  }
  ov2655_client = NULL;
  return 0;
}

/******************************************************************************

******************************************************************************/
static int ov2655_init_client(struct i2c_client *client)
{
  init_waitqueue_head(&ov2655_wait_queue);
  return 0;
}

/******************************************************************************
case CFG_SET_MODE:                  // 0
******************************************************************************/
static int ov2655_set_sensor_mode(int mode,int res)
{
  int rc = 0;
  CDBG("%s mode = %d\n",__func__ , mode);
  switch (mode) {
    case SENSOR_PREVIEW_MODE:
    case SENSOR_HFR_60FPS_MODE:
    case SENSOR_HFR_90FPS_MODE:
      {
        ov2655_ctrl->prev_res = res;
        rc = ov2655_video_config(mode);
      }
      break;
    case SENSOR_SNAPSHOT_MODE:
      {
        ov2655_ctrl->pict_res = res;
        rc = ov2655_snapshot_config(mode);
      }
      break;
    case SENSOR_RAW_SNAPSHOT_MODE:
      {
        ov2655_ctrl->pict_res = res;
        rc = ov2655_raw_snapshot_config(mode);
      }
      break;
    default:
      {
        rc = -EINVAL;
      }
      break;
  }
  return rc;
}

/******************************************************************************
case CFG_SET_EFFECT:                // 1
******************************************************************************/
static long ov2655_set_effect(int mode, int effect)
{
  int rc = 0;
  CDBG("%s mode = %d,effect = %d\n",__func__ ,mode,effect);
  switch (mode)
  {
    case SENSOR_PREVIEW_MODE:
    case SENSOR_HFR_60FPS_MODE:
    case SENSOR_HFR_90FPS_MODE:
      {
        ;
      }
      break;
    case SENSOR_SNAPSHOT_MODE:
      {
        ;
      }
      break;
    default:
      {
        CDBG("%s Default(Not Support)\n",__func__);
      }
      break;
  }
  ov2655_effect_value = effect;
  switch (effect)
  {
    case CAMERA_EFFECT_OFF:        //0
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_normal_tbl);
      }
      break;
    case CAMERA_EFFECT_MONO:       //1
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_mono_tbl);
      }
      break;
    case CAMERA_EFFECT_NEGATIVE:   //2
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_negative_tbl);
      }
      break;
    case CAMERA_EFFECT_SOLARIZE:   //3
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_solarize_tbl);
      }
      break;
    case CAMERA_EFFECT_SEPIA:       //4
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_sepia_tbl);
      }
      break;
    case CAMERA_EFFECT_POSTERIZE:   //5
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_posterize_tbl);
      }
      break;
    case CAMERA_EFFECT_WHITEBOARD:  //6
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_whiteboard_tbl);
      }
      break;
    case CAMERA_EFFECT_BLACKBOARD:  //7
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_blackboard_tbl);
      }
      break;
    case CAMERA_EFFECT_AQUA:        //8
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_aqua_tbl);
      }
      break;
    case CAMERA_EFFECT_BW:         //10
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_bw_tbl);
      }
      break;
    case CAMERA_EFFECT_BLUISH:     //11
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_bluish_tbl);
      }
      break;
    case CAMERA_EFFECT_REDDISH:    //13
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_reddish_tbl);
      }
      break;
    case CAMERA_EFFECT_GREENISH:   //14
      {
        rc = OV2655_WriteSettingTbl(ov2655_effect_greenish_tbl);
      }
      break;
    default:
      {
        CDBG("%s ...Default(Not Support)\n",__func__);
      }
      break;
  }
  ov2655_effect = effect;
  return rc;
}

/******************************************************************************
case CFG_PWR_DOWN:                  // 4
******************************************************************************/
static void ov2655_power_off(void)
{
  CDBG("%s\n",__func__);
  gpio_direction_output(ov2655_pwdn_gpio, 1);
  gpio_direction_output(ov2655_reset_gpio, 0);
}

/******************************************************************************
case CFG_SET_BRIGHTNESS:            //12
******************************************************************************/
static int ov2655_set_brightness(int8_t brightness)
{
  int rc = 0;
  CDBG("%s brightness = %d\n",__func__ , brightness);
#if 0  
  switch (brightness){
    case CAMERA_BRIGHTNESS_LV0:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv0_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV1:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv1_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV2:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv2_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV3:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv3_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV4:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_default_lv4_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV5:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv5_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV6:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv6_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV7:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv7_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV8:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv8_tbl);
      }
      break;
    default:
      {
        CDBG("CAMERA_BRIGHTNESS_ERROR COMMAND\n");
      }
      break;
  }
#endif  
  switch (brightness){
    case CAMERA_BRIGHTNESS_LV0:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv0_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV1:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv2_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV2:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv3_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV3:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv6_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV4:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv8_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV5:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv5_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV6:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv6_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV7:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv7_tbl);
      }
      break;
    case CAMERA_BRIGHTNESS_LV8:
      {
        rc = OV2655_WriteSettingTbl(ov2655_brightness_lv8_tbl);
      }
      break;
    default:
      {
        CDBG("CAMERA_BRIGHTNESS_ERROR COMMAND\n");
      }
      break;
  }
  return rc;
}

/******************************************************************************
case CFG_SET_CONTRAST:              //13
******************************************************************************/
static int ov2655_set_contrast(int contrast)
{
  int rc = 0;
  CDBG("%s contrast = %d\n",__func__ , contrast);
  if (ov2655_effect_value == CAMERA_EFFECT_OFF)
  {
    switch (contrast)
    {
      case CAMERA_CONTRAST_LV0:
        {
          rc = OV2655_WriteSettingTbl(ov2655_contrast_lv0_tbl);
        }
        break;
      case CAMERA_CONTRAST_LV1:
        {
          rc = OV2655_WriteSettingTbl(ov2655_contrast_lv1_tbl);
        }
        break;
      case CAMERA_CONTRAST_LV2:
        {
          rc = OV2655_WriteSettingTbl(ov2655_contrast_lv2_tbl);
        }
        break;
      case CAMERA_CONTRAST_LV3:
        {
          rc = OV2655_WriteSettingTbl(ov2655_contrast_lv3_tbl);
        }
        break;
      case CAMERA_CONTRAST_LV4:
        {
          rc = OV2655_WriteSettingTbl(ov2655_contrast_default_lv4_tbl);
        }
        break;
      case CAMERA_CONTRAST_LV5:
        {
          rc = OV2655_WriteSettingTbl(ov2655_contrast_lv5_tbl);
        }
        break;
      case CAMERA_CONTRAST_LV6:
        {
          rc = OV2655_WriteSettingTbl(ov2655_contrast_lv6_tbl);
        }
        break;
      case CAMERA_CONTRAST_LV7:
        {
          rc = OV2655_WriteSettingTbl(ov2655_contrast_lv7_tbl);
        }
        break;
      case CAMERA_CONTRAST_LV8:
        {
          rc = OV2655_WriteSettingTbl(ov2655_contrast_lv8_tbl);
        }
        break;
      default:
        {
          CDBG("CAMERA_CONTRAST_ERROR COMMAND\n");
        }
        break;
    }
  }
  return rc;
}

/******************************************************************************
case CFG_SET_EXPOSURE_MODE:         //15
******************************************************************************/
static long ov2655_set_exposure_mode(int mode)
{
  long rc = 0;
  CDBG("%s ...mode = %d\n",__func__ , mode);
  switch (mode)
  {
    case CAMERA_SETAE_AVERAGE:
      {
        rc = OV2655_WriteSettingTbl(ov2655_ae_average_tbl);
      }
      break;
    case CAMERA_SETAE_CENWEIGHT:
      {
        rc = OV2655_WriteSettingTbl(ov2655_ae_centerweight_tbl);
      }
      break;
    default:
      {
        CDBG("ERROR COMMAND OR NOT SUPPORT\n");
      }
      break;
  }
  return rc;
}

/******************************************************************************
case CFG_SET_WB:                    //16
******************************************************************************/
static int ov2655_set_wb_oem(uint8_t param)
{
  int rc = 0;
  CDBG("%s param = %d\n", __func__, param);
  OV2655_CAMERA_WB_AUTO = 0;
  switch(param){
    case CAMERA_WB_AUTO:              //1
      {
		OV2655_CAMERA_WB_AUTO = 1;
        rc = OV2655_WriteSettingTbl(ov2655_wb_Auto);
      }
      break;
    case CAMERA_WB_CUSTOM:            //2
      {
        rc = OV2655_WriteSettingTbl(ov2655_wb_custom);
      }
      break;
    case CAMERA_WB_INCANDESCENT:      //3
      {
        rc = OV2655_WriteSettingTbl(ov2655_wb_inc);
      }
      break;
    case CAMERA_WB_FLUORESCENT:       //4
      {
        rc = OV2655_WriteSettingTbl(ov2655_wb_fluorescent);
      }
      break;
    case CAMERA_WB_DAYLIGHT:          //5
      {
        rc = OV2655_WriteSettingTbl(ov2655_wb_daylight);
      }
      break;
    case CAMERA_WB_CLOUDY_DAYLIGHT:   //6
      {
        rc = OV2655_WriteSettingTbl(ov2655_wb_cloudy);
      }
      break;
    case CAMERA_WB_TWILIGHT:          //7
      {
        rc = OV2655_WriteSettingTbl(ov2655_wb_twilight);
      }
      break;
    case CAMERA_WB_SHADE:            //8
      {
        rc = OV2655_WriteSettingTbl(ov2655_wb_shade);
      }
      break;
    default:
      break;
  }
  return rc;
}

/******************************************************************************
case CFG_SET_ANTIBANDING:           //17
******************************************************************************/
static long ov2655_set_antibanding(int antibanding)
{
  long rc = 0;
  CDBG("%s antibanding = %d\n", __func__, antibanding);
  switch (antibanding)
  {
    case CAMERA_ANTIBANDING_OFF:
      {
        ;
      }
      break;
    case CAMERA_ANTIBANDING_60HZ:
      {
        rc = OV2655_WriteSettingTbl(ov2655_antibanding_60z_tbl);
      }
      break;
    case CAMERA_ANTIBANDING_50HZ:
      {
        rc = OV2655_WriteSettingTbl(ov2655_antibanding_50z_tbl);
      }
      break;
    case CAMERA_ANTIBANDING_AUTO:
      {
        rc = OV2655_WriteSettingTbl(ov2655_antibanding_auto_tbl);
      }
      break;
    default:
      {
        CDBG("CAMERA_ANTIBANDING_ERROR COMMAND\n");
      }
      break;
  }
  return rc;
}

/******************************************************************************
case CFG_SET_LENS_SHADING:          //20
******************************************************************************/
static int32_t ov2655_lens_shading_enable(uint8_t is_enable)
{
  int32_t rc = 0;
  if(is_enable)
  {
    CDBG("%s: enable\n",__func__);
    rc = OV2655_WriteSettingTbl(ov2655_lens_shading_on_tbl);
  }
  else
  {
    CDBG("%s: disable\n",__func__);
    rc = OV2655_WriteSettingTbl(ov2655_lens_shading_off_tbl);
  }
  return rc;
}

/******************************************************************************
case CFG_SET_SATURATION:            //32
******************************************************************************/
static int ov2655_set_saturation(int saturation)
{
  long rc = 0;
  CDBG("%s saturation = %d\n",__func__ , saturation);
  if (ov2655_effect_value == CAMERA_EFFECT_OFF)
  {
    switch (saturation){
      case CAMERA_SATURATION_LV0:    //0
        {
          rc = OV2655_WriteSettingTbl(ov2655_saturation_lv0_tbl);
        }
        break;
      case CAMERA_SATURATION_LV1:    //1
        {
          rc = OV2655_WriteSettingTbl(ov2655_saturation_lv1_tbl);
        }
        break;
      case CAMERA_SATURATION_LV2:    //2
        {
          rc = OV2655_WriteSettingTbl(ov2655_saturation_lv2_tbl);
        }
        break;
      case CAMERA_SATURATION_LV3:    //3
        {
          rc = OV2655_WriteSettingTbl(ov2655_saturation_lv3_tbl);
        }
        break;
      case CAMERA_SATURATION_LV4:    //4
        {
          rc = OV2655_WriteSettingTbl(ov2655_saturation_default_lv4_tbl);
        }
        break;
      case CAMERA_SATURATION_LV5:    //5
        {
          rc = OV2655_WriteSettingTbl(ov2655_saturation_lv5_tbl);
        }
        break;
      case CAMERA_SATURATION_LV6:    //6
        {
          rc = OV2655_WriteSettingTbl(ov2655_saturation_lv6_tbl);
        }
        break;
      case CAMERA_SATURATION_LV7:    //7
        {
          rc = OV2655_WriteSettingTbl(ov2655_saturation_lv7_tbl);
        }
        break;
      case CAMERA_SATURATION_LV8:    //8
        {
          rc = OV2655_WriteSettingTbl(ov2655_saturation_lv8_tbl);
        }
        break;
      default:
        {
          CDBG("CAMERA_SATURATION_ERROR COMMAND\n");
        }
        break;
    }
  }
  return rc;
}

/******************************************************************************
case CFG_SET_SHARPNESS:             //33
******************************************************************************/
static int ov2655_set_sharpness(int sharpness)
{
  int rc = 0;
  CDBG("%s sharpness = %d\n",__func__ , sharpness);
  if (ov2655_effect_value == CAMERA_EFFECT_OFF)
  {
    switch(sharpness*5/6){
      case CAMERA_SHARPNESS_LV0:    //0
        {
          rc = OV2655_WriteSettingTbl(ov2655_sharpness_lv0_tbl);
        }
        break;
      case CAMERA_SHARPNESS_LV1:    //5
        {
          rc = OV2655_WriteSettingTbl(ov2655_sharpness_lv1_tbl);
        }
        break;
      case CAMERA_SHARPNESS_LV2:    //10
        {
          rc = OV2655_WriteSettingTbl(ov2655_sharpness_default_lv2_tbl);
        }
        break;
      case CAMERA_SHARPNESS_LV3:    //15
        {
          rc = OV2655_WriteSettingTbl(ov2655_sharpness_lv3_tbl);
        }
        break;
      case CAMERA_SHARPNESS_LV4:    //20
        {
          rc = OV2655_WriteSettingTbl(ov2655_sharpness_lv4_tbl);
        }
        break;
      case CAMERA_SHARPNESS_LV5:    //25
        {
          rc = OV2655_WriteSettingTbl(ov2655_sharpness_lv5_tbl);
        }
        break;
      case CAMERA_SHARPNESS_LV6:    //30
        {
          rc = OV2655_WriteSettingTbl(ov2655_sharpness_lv6_tbl);
        }
        break;
      case CAMERA_SHARPNESS_LV7:    //35
        {
          rc = OV2655_WriteSettingTbl(ov2655_sharpness_lv7_tbl);
        }
        break;
      case CAMERA_SHARPNESS_LV8:    //40
        {
          rc = OV2655_WriteSettingTbl(ov2655_sharpness_lv8_tbl);
        }
        break;
      default:
        {
          CDBG("CAMERA_SHARPNESS_ERROR COMMAND\n");
        }
        break;
    }
  }
  return rc;
}

/******************************************************************************
case CFG_SET_TOUCHAEC:              //34
******************************************************************************/
static int ov2655_set_touchaec(uint32_t x,uint32_t y)
{
  int rc = 0;
  return rc;
}

/******************************************************************************
case CFG_SET_AUTO_FOCUS:            //35
******************************************************************************/
static int ov2655_sensor_start_af(void)
{
  int rc = 0;
  return rc;
}

/******************************************************************************
case CFG_SET_EXPOSURE_COMPENSATION: //37
******************************************************************************/
static int ov2655_set_exposure_compensation(int compensation)
{
  long rc = 0;
  CDBG("%s compensation = %d\n",__func__ , compensation);
  switch(compensation)
  {
    case CAMERA_EXPOSURE_COMPENSATION_LV0:  // 12
      {
        rc = OV2655_WriteSettingTbl(ov2655_exposure_compensation_lv0_tbl);
      }
      break;
    case CAMERA_EXPOSURE_COMPENSATION_LV1:  //  6
      {
        rc = OV2655_WriteSettingTbl(ov2655_exposure_compensation_lv1_tbl);
      }
      break;
    case CAMERA_EXPOSURE_COMPENSATION_LV2:  //  0
      {
        rc = OV2655_WriteSettingTbl(ov2655_exposure_compensation_lv2_default_tbl);
      }
      break;
    case CAMERA_EXPOSURE_COMPENSATION_LV3:  // -6
      {
        rc = OV2655_WriteSettingTbl(ov2655_exposure_compensation_lv3_tbl);
      }
      break;
    case CAMERA_EXPOSURE_COMPENSATION_LV4:  //-12
      {
        rc = OV2655_WriteSettingTbl(ov2655_exposure_compensation_lv4_tbl);
      }
      break;
    default:
      {
        CDBG("ERROR CAMERA_EXPOSURE_COMPENSATION\n");
      }
      break;
  }
  return rc;
}

/******************************************************************************
case CFG_SET_ISO:                   //38
******************************************************************************/
static int ov2655_set_iso(int8_t iso_type)
{
  long rc = 0;
  CDBG("%s iso_type = %d\n",__func__ , iso_type);
  switch(iso_type){
  case CAMERA_ISO_TYPE_AUTO:        // 0
    {
      rc = OV2655_WriteSettingTbl(ov2655_iso_type_auto);
    }
    break;
  case CAMEAR_ISO_TYPE_HJR:         // 1
    {
      rc = OV2655_WriteSettingTbl(ov2655_iso_type_auto);
    }
    break;
  case CAMEAR_ISO_TYPE_100:         // 2
    {
      rc = OV2655_WriteSettingTbl(ov2655_iso_type_100);
    }
    break;
  case CAMERA_ISO_TYPE_200:         // 3
    {
      rc = OV2655_WriteSettingTbl(ov2655_iso_type_200);
    }
    break;
  case CAMERA_ISO_TYPE_400:         // 4
    {
      rc = OV2655_WriteSettingTbl(ov2655_iso_type_400);
    }
    break;
  case CAMEAR_ISO_TYPE_800:         // 5
    {
      rc = OV2655_WriteSettingTbl(ov2655_iso_type_800);
    }
    break;
  case CAMERA_ISO_TYPE_1600:        // 6
    {
      rc = OV2655_WriteSettingTbl(ov2655_iso_type_1600);
    }
    break;
  default:
    {
      CDBG("ERROR ISO TYPE\n");
    }
    break;
  }
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_sensor_config(void __user *argp)
{
  struct sensor_cfg_data cdata;
  long rc = 0;
  if (copy_from_user(&cdata, (void *)argp, sizeof(struct sensor_cfg_data)))
    return -EFAULT;
  CDBG("%s %d\n",__func__,cdata.cfgtype);
  mutex_lock(&ov2655_mutex);
  switch (cdata.cfgtype) {
    case CFG_SET_MODE:                  // 0
      {
        rc =ov2655_set_sensor_mode(cdata.mode, cdata.rs);
      }
      break;
    case CFG_SET_EFFECT:                // 1
      {
        rc = ov2655_set_effect(cdata.mode, cdata.cfg.effect);
      }
      break;
    case CFG_START:                     // 2
      {
        rc = -EINVAL;
      }
      break;
    case CFG_PWR_UP:                    // 3
      {
        rc = -EINVAL;
      }
      break;
    case CFG_PWR_DOWN:                  // 4
      {
        ov2655_power_off();
      }
      break;
    case CFG_WRITE_EXPOSURE_GAIN:       // 5
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SET_DEFAULT_FOCUS:         // 6
      {
        rc = -EINVAL;
      }
      break;
    case CFG_MOVE_FOCUS:                // 7
      {
        rc = -EINVAL;
      }
      break;
    case CFG_REGISTER_TO_REAL_GAIN:     // 8
      {
        rc = -EINVAL;
      }
      break;
    case CFG_REAL_TO_REGISTER_GAIN:     // 9
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SET_FPS:                   //10
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SET_PICT_FPS:              //11
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SET_BRIGHTNESS:            //12
      {
        rc = ov2655_set_brightness(cdata.cfg.brightness);
      }
      break;
    case CFG_SET_CONTRAST:              //13
      {
        rc = ov2655_set_contrast(cdata.cfg.contrast);
      }
      break;
    case CFG_SET_ZOOM:                  //14
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SET_EXPOSURE_MODE:         //15
      {
        rc = ov2655_set_exposure_mode(cdata.cfg.ae_mode);
      }
      break;
    case CFG_SET_WB:                    //16
      {
        ov2655_set_wb_oem(cdata.cfg.wb_val);
      }
      break;
    case CFG_SET_ANTIBANDING:           //17
      {
        rc = ov2655_set_antibanding(cdata.cfg.antibanding);
      }
      break;
    case CFG_SET_EXP_GAIN:              //18
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SET_PICT_EXP_GAIN:         //19
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SET_LENS_SHADING:          //20
      {
        rc = ov2655_lens_shading_enable(cdata.cfg.lens_shading);
      }
      break;
    case CFG_GET_PICT_FPS:              //21
      {
        rc = -EINVAL;
      }
      break;
    case CFG_GET_PREV_L_PF:             //22
      {
        rc = -EINVAL;
      }
      break;
    case CFG_GET_PREV_P_PL:             //23
      {
        rc = -EINVAL;
      }
      break;
    case CFG_GET_PICT_L_PF:             //24
      {
        rc = -EINVAL;
      }
      break;
    case CFG_GET_PICT_P_PL:             //25
      {
        rc = -EINVAL;
      }
      break;
    case CFG_GET_AF_MAX_STEPS:          //26
      {
        rc = -EINVAL;
      }
      break;
    case CFG_GET_PICT_MAX_EXP_LC:       //27
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SEND_WB_INFO:              //28
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SENSOR_INIT:               //29
      {
        rc = -EINVAL;
      }
      break;
    case CFG_GET_3D_CALI_DATA:          //30
      {
        rc = -EINVAL;
      }
      break;
    case CFG_GET_CALIB_DATA:            //31
      {
        rc = -EINVAL;
      }
      break;
    case CFG_SET_SATURATION:            //32
      {
        rc = ov2655_set_saturation(cdata.cfg.saturation);
      }
      break;
    case CFG_SET_SHARPNESS:             //33
      {
        rc = ov2655_set_sharpness(cdata.cfg.sharpness);
      }
      break;
    case CFG_SET_TOUCHAEC:              //34
      {
        rc = ov2655_set_touchaec(cdata.cfg.aec_cord.x, cdata.cfg.aec_cord.y);
      }
      break;
    case CFG_SET_AUTO_FOCUS:            //35
      {
        rc = ov2655_sensor_start_af();
      }
      break;
    case CFG_SET_AUTOFLASH:             //36
      {
        is_autoflash = cdata.cfg.is_autoflash;
      }
      break;
    case CFG_SET_EXPOSURE_COMPENSATION: //37
      {
        rc = ov2655_set_exposure_compensation(cdata.cfg.exp_compensation);
      }
      break;
    case CFG_SET_ISO:                   //38
      {
        rc = ov2655_set_iso(cdata.cfg.iso_type);
      }
      break;
    case CFG_SET_AUTO_LED_START:        //39
      {
        rc = -EINVAL;
      }
      break;
    default:
      CDBG("%s: Command=%d (Not Implement)!!\n",__func__,cdata.cfgtype);
      break;
  }
  mutex_unlock(&ov2655_mutex);
  return rc;
}

/******************************************************************************

******************************************************************************/
static struct i2c_driver ov2655_i2c_driver = {
    .id_table = ov2655_i2c_id,
    .probe  = ov2655_i2c_probe,
    .remove = ov2655_i2c_remove,
    .driver = {
        .name = "ov2655",
    },
};

/******************************************************************************

******************************************************************************/
static void ov2655_power_on(void)
{
  CDBG("%s\n",__func__);
  gpio_direction_output(ov2655_pwdn_gpio, 0);
}

/******************************************************************************

******************************************************************************/
static void ov2655_power_reset(void)
{
  CDBG("%s\n",__func__);
  gpio_direction_output(ov2655_reset_gpio, 1);
  mdelay(1);
  gpio_direction_output(ov2655_reset_gpio, 0);
  mdelay(1);
  gpio_direction_output(ov2655_reset_gpio, 1);
  mdelay(1);
}

/******************************************************************************

******************************************************************************/
static int ov2655_read_model_id(const struct msm_camera_sensor_info *data)
{
  int rc = 0;
  uint model_id_high_byte = 0;
  uint model_id_low_byte = 0;
  uint model_id = 0;
  CDBG("%s\n",__func__);
  rc = ov2655_i2c_read_byte(ov2655_client->addr, 0x300A, &model_id_high_byte);
  if (rc < 0){
    CDBG("ov2655_read_model_id ov2655_i2c_read_byte failed!\n");
    return rc;
  }
  rc = ov2655_i2c_read_byte(ov2655_client->addr, 0x300B, &model_id_low_byte);
  if (rc < 0){
    CDBG("ov2655_read_model_id ov2655_i2c_read_byte failed!\n");
    return rc;
  }
  model_id = (model_id_high_byte<<8)+model_id_low_byte;
  if (model_id != 0x2656){
    CDBG("ov2655_read_model_id model_id(0x%04x) mismatch!\n",model_id);
    return -ENODEV;
  }
  CDBG("ov2655_read_model_id model_id(0x%04x) match success!\n",model_id);
  return rc;
}

/******************************************************************************

******************************************************************************/
static int ov2655_probe_init_gpio(const struct msm_camera_sensor_info *data)
{
  int rc = 0;
  CDBG("%s\n",__func__);
  ov2655_pwdn_gpio = data->sensor_pwd;
  ov2655_reset_gpio = data->sensor_reset;
#if 0  
  ov2655_vcm_pwd_gpio = data->vcm_pwd ;
  if (data->sensor_reset_enable) {
    rc = gpio_request(data->sensor_reset, "ov2655");
    if (0 == rc) {
      rc = gpio_tlmm_config(GPIO_CFG(data->sensor_reset, 0,GPIO_CFG_OUTPUT,
                            GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
      if (rc < 0) {
        gpio_free(data->sensor_reset);
        CDBG("ov2655 gpio_tlmm_config(%d) fail",data->sensor_reset);
        return rc;
      }
    }else{
      CDBG("ov2655 gpio_request(%d) fail",data->sensor_reset);
      return rc;
    }
  }

  rc = gpio_request(data->sensor_pwd, "ov2655");
  if (0 == rc) {
    rc = gpio_tlmm_config(GPIO_CFG(data->sensor_pwd, 0,GPIO_CFG_OUTPUT,
                          GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
    if (rc < 0) {
      gpio_free(data->sensor_reset);
      gpio_free(data->sensor_pwd);
      CDBG("ov2655 gpio_tlmm_config(%d) fail",data->sensor_pwd);
      return rc;
    }
  }else{
    gpio_free(data->sensor_reset);
    CDBG("ov2655 gpio_request(%d) fail",data->sensor_pwd);
    return rc;
  }
  if (data->vcm_enable)
  {
    gpio_direction_output(data->vcm_pwd, 1);
  }
  gpio_direction_output(data->sensor_reset, 1);
  gpio_direction_output(data->sensor_pwd, 1);
#endif
  return rc;
}

/******************************************************************************

******************************************************************************/
static void ov2655_probe_free_gpio(const struct msm_camera_sensor_info *data)
{
  gpio_free(ov2655_pwdn_gpio);
  gpio_free(ov2655_reset_gpio);
  if (data->vcm_enable)
  {
    gpio_free(ov2655_vcm_pwd_gpio);
    ov2655_vcm_pwd_gpio = 0xFF ;
  }
  ov2655_pwdn_gpio = 0xFF;
  ov2655_reset_gpio = 0xFF;
}
/******************************************************************************

******************************************************************************/
static int ov2655_sensor_probe(const struct msm_camera_sensor_info *info,
                                struct msm_sensor_ctrl *s)
{
  int rc = -ENOTSUPP;
  CDBG("%s\n",__func__);

	info->pdata->camera_vreg_config(1,1500,1);
  
  rc = i2c_add_driver(&ov2655_i2c_driver);
  if ((rc < 0 ) || (ov2655_client == NULL))
  {
    CDBG("i2c_add_driver failed\n");
    if(NULL != ov2655_sensorw)
    {
      kfree(ov2655_sensorw);
      ov2655_sensorw = NULL;
    }
    ov2655_client = NULL;
    return rc;
  }
  lcd_camera_power_onoff(1);
  rc = ov2655_probe_init_gpio(info);
  if (rc < 0) {
    CDBG("%s,ov2655_probe_init_gpio failed\n",__func__);
    i2c_del_driver(&ov2655_i2c_driver);
    return rc;
  }
  ov2655_power_off();
  msm_camio_clk_rate_set(OV2655_MASTER_CLK_RATE);
  msleep(5);
  ov2655_power_on();
  ov2655_power_reset();
  rc = ov2655_read_model_id(info);
  if (rc < 0)
  {
    CDBG("ov2655_read_model_id failed\n");
    CDBG("%s,unregister\n",__func__);
    i2c_del_driver(&ov2655_i2c_driver);
    ov2655_power_off();
    lcd_camera_power_onoff(0);
    ov2655_probe_free_gpio(info);
	info->pdata->camera_vreg_config(0,1500,0);	
    return rc;
  }
  s->s_init = ov2655_sensor_open_init;
  s->s_release = ov2655_sensor_release;
  s->s_config  = ov2655_sensor_config;
  s->s_camera_type = BACK_CAMERA_2D;
  s->s_mount_angle = info->sensor_platform_info->mount_angle;
  ov2655_power_off();
  lcd_camera_power_onoff(0);
  return rc;
}
/******************************************************************************

******************************************************************************/
static int ov2655_i2c_probe(struct i2c_client *client,
                             const struct i2c_device_id *id)
{
  CDBG("%s\n",__func__);
  if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
  {
    CDBG("i2c_check_functionality failed\n");
    return -ENOMEM;
  }

  ov2655_sensorw = kzalloc(sizeof(struct ov2655_work), GFP_KERNEL);
  if (!ov2655_sensorw)
  {
    CDBG("kzalloc failed\n");
    return -ENOMEM;
  }
  i2c_set_clientdata(client, ov2655_sensorw);
  ov2655_init_client(client);
  ov2655_client = client;
  CDBG("%s:ov2655_client->addr = 0x%02x\n",__func__,ov2655_client->addr);
  return 0;
}
/******************************************************************************

******************************************************************************/
static int __ov2655_probe(struct platform_device *pdev)
{
  return msm_camera_drv_start(pdev, ov2655_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
  .probe = __ov2655_probe,
  .driver = {
    .name = "msm_camera_ov2655",
    .owner = THIS_MODULE,
  },
};

static int __init ov2655_init(void)
{
  ov2655_i2c_buf[0]=0x5A;
  return platform_driver_register(&msm_camera_driver);
}

module_init(ov2655_init);

MODULE_DESCRIPTION("OV2655 YUV MIPI sensor driver");
MODULE_LICENSE("GPL v2");
