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


#define DEBUG
#define hi351_DEBUG

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
#include "hi351.h"

/* Debug switch */
#ifdef CDBG
#undef CDBG
#endif
#ifdef CDBG_HIGH
#undef CDBG_HIGH
#endif

#define hi351_VERBOSE_DGB
#define SENSOR_i2c_data_DEBUG 0
#ifdef hi351_VERBOSE_DGB
#define CDBG(fmt, args...) pr_debug(fmt, ##args)
#define CDBG_HIGH(fmt, args...) pr_debug(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#define CDBG_HIGH(fmt, args...) pr_debug(fmt, ##args)
#endif


#define FALSE 0
#define TRUE 1

static int hi351_pwdn_gpio;
static int hi351_reset_gpio;
static int hi351_driver_pwdn_gpio;


static int hi351_CSI_CONFIG = 0;

struct hi351_work {
   struct work_struct work;
};
static struct hi351_work *hi351_sensorw;
static struct i2c_client    *hi351_client;
static DECLARE_WAIT_QUEUE_HEAD(hi351_wait_queue);
DEFINE_MUTEX(hi351_mutex);
static int16_t hi351_effect = CAMERA_EFFECT_OFF;
static int16_t hi351_sharpness = CAMERA_SHARPNESS_LV2*5;
static int16_t hi351_contrast = CAMERA_CONTRAST_LV2*2;
//static int16_t hi351_iso = CAMERA_ISO_AUTO;
static int16_t hi351_saturation = CAMERA_SATURATION_LV2*2;
static int16_t hi351_brightness = CAMERA_BRIGHTNESS_LV2;
static int16_t hi351_whitebalance = CAMERA_WB_AUTO;
static int is_autoflash = 0;

struct __hi351_ctrl 
{
    const struct msm_camera_sensor_info *sensordata;
    int sensormode;
    uint fps_divider; /* init to 1 * 0x00000400 */
    uint pict_fps_divider; /* init to 1 * 0x00000400 */
    u16 curr_step_pos;
    u16 curr_lens_pos;
    u16 init_curr_lens_pos;
    u16 my_reg_gain;
    u16 my_reg_line_count;
    enum msm_s_resolution prev_res;
    enum msm_s_resolution pict_res;
    enum msm_s_resolution curr_res;
    enum msm_s_test_mode  set_test;
};
static struct __hi351_ctrl *hi351_ctrl;
static int afinit = 1;

extern struct rw_semaphore leds_list_lock;
extern struct list_head leds_list;

static int hi351_i2c_remove(struct i2c_client *client);
static int hi351_i2c_probe(struct i2c_client *client,const struct i2c_device_id *id);
#if 0
static int hi351_i2c_txdata(u16 saddr,u8 *txdata,int length)
{
    struct i2c_msg msg[] = {
        {
            .addr  = saddr,
            .flags = 0,
            .len = length,
            .buf = txdata,
        },
    };

    if (i2c_transfer(hi351_client->adapter, msg, 1) < 0)    return -EIO;
    else return 0;
}
#else
static int32_t hi351_i2c_txdata(unsigned short saddr,
	unsigned char *txdata, int length)
{
	struct i2c_msg msg[] = {
		{
			.addr = saddr,
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};

#if SENSOR_i2c_data_DEBUG
	if (length == 2)
		CDBG("msm_io_i2c_w: 0x%04x 0x%04x\n",
			*(u16 *) txdata, *(u16 *) (txdata + 2));
	else if (length == 4)
		CDBG("msm_io_i2c_w: 0x%04x\n", *(u16 *) txdata);
	else
		CDBG("msm_io_i2c_w: length = %d\n", length);
#endif
	if (i2c_transfer(hi351_client->adapter, msg, 1) < 0) {
		pr_err("hi351_i2c_txdata failed\n");
		return -EIO;
	}

	return 0;
}

#endif
static int32_t hi351_i2c_write(unsigned short saddr,
	unsigned short waddr, unsigned short wdata, enum hi351_width width)
{
	int32_t rc = -EIO;
	unsigned char buf[4];

	memset(buf, 0, sizeof(buf));
	switch (width) {
	case WORD_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = (wdata & 0xFF00)>>8;
		buf[3] = (wdata & 0x00FF);

		rc = hi351_i2c_txdata(saddr, buf, 4);
	}
		break;

	case BYTE_LEN: {
		buf[0] = (waddr & 0xFF00)>>8;
		buf[1] = (waddr & 0x00FF);
		buf[2] = wdata & 0x00FF;
		rc = hi351_i2c_txdata(saddr, buf, 3);
	}
		break;
		
	case BYTE_BYTE_LEN: { 
		buf[0] = waddr ;
		buf[1] = wdata ;
		rc = hi351_i2c_txdata(saddr, buf, 2);
	}
		break;
		
	default:
		break;
	}

	if (rc < 0)
		pr_err(
		"i2c_write failed, addr = 0x%x, val = 0x%x!\n",
		waddr, wdata);

	return rc;
}

static int32_t hi351_i2c_write_table(
	struct hi351_i2c_reg_conf const *reg_conf_tbl,
	int num_of_items_in_table)
{
	int i;
	int32_t rc = -EIO;

	for (i = 0; i < num_of_items_in_table; i++) {
		rc = hi351_i2c_write(hi351_client->addr,
			reg_conf_tbl->waddr, reg_conf_tbl->wdata,
			reg_conf_tbl->width);
#if 1 /* we don't need it return error when send between the cmd table */
		if (rc < 0)
			break;
#endif /* above is added at 2011-4-13 */
		if (reg_conf_tbl->mdelay_time != 0)
			mdelay(reg_conf_tbl->mdelay_time);
		reg_conf_tbl++;
	}

	return rc;
}

static int32_t hi351_i2c_read(unsigned short   saddr, unsigned short raddr, unsigned short *rdata, enum hi351_width width);
#if 0
static int32_t hi351_i2c_read_table(struct hi351_i2c_reg_conf const *reg_conf_tbl,int num_of_items_in_table)
{
	int i;
	int32_t rc = -EIO;
	unsigned short readValue = 0;

	for (i = 0; i < num_of_items_in_table; i++) {
	if(reg_conf_tbl->waddr == 0x098E)
	{
		rc = hi351_i2c_write(hi351_client->addr,
			reg_conf_tbl->waddr, reg_conf_tbl->wdata,
			reg_conf_tbl->width);
		if (rc < 0)
			break;
		if (reg_conf_tbl->mdelay_time != 0)
			mdelay(reg_conf_tbl->mdelay_time);
		reg_conf_tbl++;
	}else
	{
		rc = hi351_i2c_read(hi351_client->addr,
			reg_conf_tbl->waddr, &readValue,
			reg_conf_tbl->width);
		if (rc < 0)
			break;
		if (reg_conf_tbl->mdelay_time != 0)
			mdelay(reg_conf_tbl->mdelay_time);
		pr_info("reg: %x, data: %x",reg_conf_tbl->waddr,readValue);
		reg_conf_tbl++;
	}
	}

	return rc;
}
#endif

#if 0
static int hi351_i2c_rxdata(unsigned short saddr,
    unsigned char *rxdata, int length)
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
    
    if (i2c_transfer(hi351_client->adapter, msgs, 2) < 0) {
        CDBG_HIGH("--CAMERA-- hi351_i2c_rxdata failed!\n");
        return -EIO;
    }

    return 0;
}
#else
static int hi351_i2c_rxdata(unsigned short saddr,
	unsigned char *rxdata, int length)
{
	bool bLength1 = false;
	#if SENSOR_i2c_data_DEBUG
		if (length == 2)
			CDBG("msm_io_i2c_r: 0x%04x 0x%04x\n",
				*(u16 *) rxdata, *(u16 *) (rxdata + 2));
		else if (length == 4)
			CDBG("msm_io_i2c_r: 0x%04x\n", *(u16 *) rxdata);
		else
			CDBG("msm_io_i2c_r: length = %d\n", length);
	#endif
	if (length == 1)
	{ 
		bLength1 = true;
	}
	if(bLength1)
	{ 
		struct i2c_msg msgs[] = {
		{			
			.addr   = saddr,
			.flags = 0,	
			.len   = 1,
			.buf  = rxdata,
		},	
		{
			.addr   = saddr,
			.flags = I2C_M_RD,
			.len   = length,
			.buf   = rxdata,
		},
		};
	 
		if (i2c_transfer(hi351_client->adapter, msgs, 2) < 0) {
			pr_err("hi351_i2c_rxdata failed!\n");
			return -EIO;
		}
	}
	else
	{
		struct i2c_msg msgs[] = {
		{			
			.addr   = saddr,
			.flags = 0,	
			.len   = 2,
			.buf  = rxdata,
		},	
		{
			.addr   = saddr,
			.flags = I2C_M_RD,
			.len   = length,
			.buf   = rxdata,
		},
		};
	 
		if (i2c_transfer(hi351_client->adapter, msgs, 2) < 0) {
			pr_err("hi351_i2c_rxdata failed!\n");
			return -EIO;
		}
	}
	return 0;
}
#endif

#if 0
static int32_t hi351_i2c_read_byte(unsigned short   saddr,
    unsigned int raddr, unsigned int *rdata)
{
    int rc = 0;
    unsigned char buf[2];
    //CDBG("+hi351_i2c_read_byte\n");
    memset(buf, 0, sizeof(buf));

    buf[0] = (raddr & 0xFF00)>>8;
    buf[1] = (raddr & 0x00FF);

    rc = hi351_i2c_rxdata(saddr, buf, 1);
    if (rc < 0) {
        CDBG_HIGH("--CAMERA-- hi351_i2c_read_byte failed!\n");
        return rc;
    }

    *rdata = buf[0];

    //CDBG("-hi351_i2c_read_byte\n");
    return rc;
}
#endif
static int32_t hi351_i2c_read(unsigned short   saddr,
	unsigned short raddr, unsigned short *rdata, enum hi351_width width)
{
	int32_t rc = 0;
	unsigned char buf[4];

	if (!rdata)
		return -EIO;

	memset(buf, 0, sizeof(buf));

	switch (width) {
	case WORD_LEN: {
		buf[0] = (raddr & 0xFF00)>>8;
		buf[1] = (raddr & 0x00FF);

		rc = hi351_i2c_rxdata(saddr, buf, 2);
		if (rc < 0)
			return rc;

		*rdata = buf[0] << 8 | buf[1];
	}
		break;
	case BYTE_LEN: {
		buf[0] = (raddr & 0xFF00)>>8;
		buf[1] = (raddr & 0x00FF);

		rc = hi351_i2c_rxdata(saddr, buf, 2);
		if (rc < 0)
			return rc;

		*rdata = buf[0];
	}
		break;

	case BYTE_BYTE_LEN: {
		buf[0] = raddr;

		rc = hi351_i2c_rxdata(saddr, buf, 1);
		if (rc < 0)
			return rc;

		*rdata = buf[0];
	}
		break;

	default:
		break;
	}

	if (rc < 0)
		pr_err("hi351_i2c_read failed!\n");

	return rc;
}
#if 0
static int32_t hi351_i2c_read_table(
	struct hi351_i2c_reg_conf const *reg_conf_tbl,
	int num_of_items_in_table)
{
	int i;
	int32_t rc = -EIO;
	unsigned short readValue = 0;

	for (i = 0; i < num_of_items_in_table; i++) {
	if(reg_conf_tbl->waddr == 0x098e)
	{
		rc = hi351_i2c_write(hi351_client->addr,
			reg_conf_tbl->waddr, reg_conf_tbl->wdata,
			reg_conf_tbl->width);
		if (rc < 0)
			break;
		if (reg_conf_tbl->mdelay_time != 0)
			mdelay(reg_conf_tbl->mdelay_time);
		reg_conf_tbl++;
	}else
	{
		rc = hi351_i2c_read(hi351_client->addr,
			reg_conf_tbl->waddr, &readValue,
			reg_conf_tbl->width);
		if (rc < 0)
			break;
		if (reg_conf_tbl->mdelay_time != 0)
			mdelay(reg_conf_tbl->mdelay_time);
		pr_info("reg: %x, data: %x",reg_conf_tbl->waddr,readValue);
		reg_conf_tbl++;
	}
	}

	return rc;
}
#endif
#if 1
static void camera_sw_power_onoff(int v)
{
    if (v == 0) {
        CDBG("--CAMERA-- camera_sw_power_onoff: down\n");
        hi351_i2c_write(hi351_client->addr, 0x03, 0x00, BYTE_BYTE_LEN); //power down CHENJun
        hi351_i2c_write(hi351_client->addr, 0x01, 0xf1, BYTE_BYTE_LEN); //power down CHENJun
        
//   	msleep(200);
    }
    else {
        CDBG("--CAMERA-- camera_sw_power_onoff: on\n");
        hi351_i2c_write(hi351_client->addr, 0x03, 0x00, BYTE_BYTE_LEN); //power down CHENJun
        hi351_i2c_write(hi351_client->addr, 0x01, 0xf0, BYTE_BYTE_LEN); //power down CHENJun
//	msleep(200);
    }
}
#endif

static void hi351_power_off(void)
{
    int rc = 0;	
//    rc = gpio_request(hi351_pwdn_gpio,"hi351");
    if(rc < 0){
	CDBG("%s:gpio_request----pwdn failed!",__func__);    
    }
    CDBG("--CAMERA-- %s ... (Start...)\n",__func__);
//    gpio_set_value(hi351_pwdn_gpio, 0);
      gpio_direction_output(hi351_pwdn_gpio,0);	  
  CDBG("--CAMERA-- %s ... (End...)\n",__func__);
//    gpio_free(hi351_pwdn_gpio);
}

static void hi351_power_on(void)
{
    int rc = 0;
//    rc = gpio_request(hi351_pwdn_gpio,"hi351");
    if(rc < 0){
	CDBG("%s:gpio_request----pwdn failed!",__func__);    
    }
    CDBG("--CAMERA-- %s ... (Start...)\n",__func__);
//    gpio_set_value(hi351_pwdn_gpio, 1);
      gpio_direction_output(hi351_pwdn_gpio,1);	  
    CDBG("--CAMERA-- %s ... (End...)\n",__func__);
//    gpio_free(hi351_pwdn_gpio);
}

static void hi351_power_reset(void)
{
    int rc = 0;
    CDBG("--CAMERA-- %s ... (Start...)\n",__func__);
//    rc = gpio_request(hi351_reset_gpio,"hi351");
    if(rc < 0){
	CDBG("%s:gpio_request----reset failed!",__func__);    
    }
//    gpio_set_value(hi351_reset_gpio, 1);   //reset camera reset pin
    gpio_direction_output(hi351_reset_gpio, 1);   //reset camera reset pin
    mdelay(5);
//    gpio_set_value(hi351_reset_gpio, 0);
    gpio_direction_output(hi351_reset_gpio, 0);   //reset camera reset pin
    mdelay(5);
//    gpio_set_value(hi351_reset_gpio, 1);
    gpio_direction_output(hi351_reset_gpio, 1);   //reset camera reset pin
    mdelay(1);
//    gpio_free(hi351_reset_gpio);
    CDBG("--CAMERA-- %s ... (End...)\n",__func__);
}

static int hi351_probe_readID(const struct msm_camera_sensor_info *data)
{
	int rc = 0;
	unsigned short model_id;
	/* Read the Model ID of the sensor */
	rc = hi351_i2c_read(hi351_client->addr,
		REG_hi351_MODEL_ID, &model_id, BYTE_BYTE_LEN);
	if (rc < 0)
		goto read_id_fail;
       pr_info("hi351_probe_read_id model_id:0x%x\n", model_id);
	/* Check if it matches it with the value in Datasheet */
	if (model_id != hi351_MODEL_ID) {
		rc = -EINVAL;
		goto read_id_fail;
	}
	
read_id_fail:
	return rc;
}

#if 0
static void dump_af_status(void)
{
    int tmp = 0;

    hi351_i2c_read_byte(hi351_client->addr, 0x3000, &tmp);
    CDBG("    %s 0x3000 = %x\n", __func__, tmp);
    hi351_i2c_read_byte(hi351_client->addr, 0x3001, &tmp);
    CDBG("    %s 0x3001 = %x\n", __func__, tmp);
    hi351_i2c_read_byte(hi351_client->addr, 0x3004, &tmp);
    CDBG("    %s 0x3004 = %x\n", __func__, tmp);
    hi351_i2c_read_byte(hi351_client->addr, 0x3005, &tmp);
    CDBG("    %s 0x3005 = %x\n", __func__, tmp);
    hi351_i2c_read_byte(hi351_client->addr, hi351_CMD_FW_STATUS, &tmp);
    CDBG("    %s af_st = %x\n\n", __func__, tmp);
}
#endif

#if 1
static int hi351_af_setting(void)
{
    int rc = 0;
    return rc;	
#if 0	//CHENJun
    int lens = sizeof(hi351_afinit_tbl)/sizeof(hi351_afinit_tbl[0]);

    CDBG("--CAMERA-- hi351_af_setting\n");

    hi351_i2c_write(hi351_client->addr,0x3000,0x20,10);

    rc = hi351_i2c_txdata(hi351_client->addr,hi351_afinit_tbl,lens);
    //rc = hi351Core_WritePREG(hi351_afinit_tbl);

    if (rc < 0)
    {
        CDBG_HIGH("--CAMERA-- AF_init failed\n");
        return rc;
    }

    hi351_i2c_write(hi351_client->addr,hi351_CMD_MAIN,0x00,10);
    hi351_i2c_write(hi351_client->addr,hi351_CMD_ACK,0x00,10);
    hi351_i2c_write(hi351_client->addr,hi351_CMD_PARA0,0x00,10);
    hi351_i2c_write(hi351_client->addr,hi351_CMD_PARA1,0x00,10);
    hi351_i2c_write(hi351_client->addr,hi351_CMD_PARA2,0x00,10);
    hi351_i2c_write(hi351_client->addr,hi351_CMD_PARA3,0x00,10);
    hi351_i2c_write(hi351_client->addr,hi351_CMD_PARA4,0x00,10);
    hi351_i2c_write(hi351_client->addr,hi351_CMD_FW_STATUS,0x7f,10);
    hi351_i2c_write(hi351_client->addr,0x3000,0x00,10);

    return rc;
#endif	
}
#endif
#if 0
static int hi351_set_flash_light(enum led_brightness brightness)
{
    struct led_classdev *led_cdev;

    CDBG("--CAMERA-- hi351_set_flash_light brightness = %d\n", brightness);

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
        CDBG_HIGH("--CAMERA-- get flashlight device failed\n");
        return -1;
    }

    return 0;
}
#endif
#if 0
static int hi351_get_preview_exposure_gain(void)
{
    int rc = 0;
    unsigned int ret_l,ret_m,ret_h;
            
    hi351_i2c_write(hi351_client->addr,0x3503, 0x07,10);

    //get preview exp & gain
    ret_h = ret_m = ret_l = 0;
    hi351_preview_exposure = 0;
    hi351_i2c_read_byte(hi351_client->addr,0x3500, &ret_h);
    hi351_i2c_read_byte(hi351_client->addr,0x3501, &ret_m);
    hi351_i2c_read_byte(hi351_client->addr,0x3502, &ret_l);
    hi351_preview_exposure = (ret_h << 12) + (ret_m << 4) + (ret_l >> 4);
    CDBG("preview_exposure=%d\n", hi351_preview_exposure);

    ret_h = ret_m = ret_l = 0;
    hi351_preview_maxlines = 0;
    hi351_i2c_read_byte(hi351_client->addr,0x380e, &ret_h);
    hi351_i2c_read_byte(hi351_client->addr,0x380f, &ret_l);
    hi351_preview_maxlines = (ret_h << 8) + ret_l;
    CDBG("Preview_Maxlines=%d\n", hi351_preview_maxlines);

    //Read back AGC Gain for preview
    hi351_gain = 0;
    hi351_i2c_read_byte(hi351_client->addr,0x350b, &hi351_gain);
    CDBG("Gain,0x350b=0x%x\n", hi351_gain);

    return rc;
}

static int hi351_set_capture_exposure_gain(void)
{
    int rc = 0;
   //calculate capture exp & gain
   
    unsigned char ExposureLow,ExposureMid,ExposureHigh;
    unsigned int ret_l,ret_m,ret_h,Lines_10ms;
    unsigned short ulCapture_Exposure,iCapture_Gain;
    unsigned int ulCapture_Exposure_Gain,Capture_MaxLines;

    ret_h = ret_m = ret_l = 0;
    hi351_i2c_read_byte(hi351_client->addr,0x380e, &ret_h);
    hi351_i2c_read_byte(hi351_client->addr,0x380f, &ret_l);
    Capture_MaxLines = (ret_h << 8) + ret_l;
    CDBG("Capture_MaxLines=%d\n", Capture_MaxLines);

    if(m_60Hz == TRUE)
    {
        Lines_10ms = Capture_Framerate * Capture_MaxLines/12000;
    }
    else
    {
        Lines_10ms = Capture_Framerate * Capture_MaxLines/10000;
    }

    if (hi351_preview_maxlines == 0)
    {
        hi351_preview_maxlines = 1;
    }

    ulCapture_Exposure = (hi351_preview_exposure*(Capture_Framerate)*(Capture_MaxLines)) 
                            /(((hi351_preview_maxlines)*(g_Preview_FrameRate)));

    iCapture_Gain = hi351_gain;

    ulCapture_Exposure_Gain = ulCapture_Exposure * iCapture_Gain; 

    if(ulCapture_Exposure_Gain < Capture_MaxLines*16)
    {
        ulCapture_Exposure = ulCapture_Exposure_Gain/16;

        if (ulCapture_Exposure > Lines_10ms)
        {
          ulCapture_Exposure /= Lines_10ms;
          ulCapture_Exposure *= Lines_10ms;
        }
    }
    else
    {
        ulCapture_Exposure = Capture_MaxLines;
    }

    if(ulCapture_Exposure == 0)
    {
        ulCapture_Exposure = 1;
    }

    iCapture_Gain = (ulCapture_Exposure_Gain * 2 / ulCapture_Exposure + 1) / 2;

    ExposureLow = ((unsigned char)ulCapture_Exposure) << 4;

    ExposureMid = (unsigned char)(ulCapture_Exposure >> 4) & 0xff;

    ExposureHigh = (unsigned char)(ulCapture_Exposure >> 12);


    // set capture exp & gain

    hi351_i2c_write(hi351_client->addr,0x350b, iCapture_Gain, 10);
    hi351_i2c_write(hi351_client->addr,0x3502, ExposureLow, 10);
    hi351_i2c_write(hi351_client->addr,0x3501, ExposureMid, 10);
    hi351_i2c_write(hi351_client->addr,0x3500, ExposureHigh, 10);
  
    CDBG("iCapture_Gain=%d\n", iCapture_Gain);
    CDBG("ExposureLow=%d\n", ExposureLow);
    CDBG("ExposureMid=%d\n", ExposureMid);
    CDBG("ExposureHigh=%d\n", ExposureHigh);

    msleep(250);

    return rc;
}
#endif

static int hi351_sensor_setting(int update_type, int rt)
{
  int rc = 0;
 // int tmp;
  //unsigned short readValue = 0;	
  //int retry = 10;	
  struct msm_camera_csi_params hi351_csi_params;

  CDBG("--CAMERA-- update_type = %d, rt = %d\n", update_type, rt);

//    hi351_i2c_write(hi351_client->addr,0x03,0x02,BYTE_BYTE_LEN);
//    hi351_i2c_write(hi351_client->addr,0x3d,0x70,BYTE_BYTE_LEN);
  if (update_type == S_REG_INIT) {
   // rc = hi351_i2c_write(hi351_client->addr, 0x3103, 0x11, 10); //CHENJun
   // rc = hi351_i2c_write(hi351_client->addr, 0x3008, 0x82, 10); //CHENJun
    msleep(5);

//    hi351_CSI_CONFIG = 0;
    //set sensor init setting
    CDBG("--CAMERA-- set sensor init setting\n");
     rc = hi351_i2c_write_table(&hi351_regs.cambl[0],
				hi351_regs.cambl_size);
//     rc = hi351_i2c_read_table(&hi351_regs.cambl[0],
//				hi351_regs.cambl_size);
    if (rc < 0) {
      CDBG_HIGH("--CAMERA-- sensor init setting failed\n");
      return rc;
    }

#if 0
    rc =hi351_i2c_read_byte(hi351_client->addr, 0x4740, &tmp);
    CDBG("--CAMERA-- init 0x4740 value=0x%x\n", tmp);

    if (tmp != 0x21) {
      rc = hi351_i2c_write(hi351_client->addr, 0x4740, 0x21, 10);
      msleep(10);
      rc =hi351_i2c_read_byte(hi351_client->addr, 0x4740, &tmp);
      CDBG("--CAMERA-- WG 0x4740 value=0x%x\n", tmp);
    }
#endif
    CDBG("--CAMERA-- AF_init: afinit = %d\n", afinit);
    if (afinit == 1) {
      rc = hi351_af_setting();
      if (rc < 0) {
        CDBG_HIGH("hi351_af_setting failed\n");
        return rc;
      }
      afinit = 0;
    }

    /* reset fps_divider */
    hi351_ctrl->fps_divider = 1 * 0x0400;
  } else if (update_type == S_UPDATE_PERIODIC) {
    if(!hi351_CSI_CONFIG) {
      camera_sw_power_onoff(0); //standby
      msleep(20);

      hi351_csi_params.lane_cnt = 1;
      hi351_csi_params.data_format = CSI_8BIT;
      hi351_csi_params.lane_assign = 0xe4;
      hi351_csi_params.dpcm_scheme = 0;
      hi351_csi_params.settle_cnt = 0x14;

      CDBG("--CAMERA-- msm_camio_csi_config\n");
      rc = msm_camio_csi_config(&hi351_csi_params);

      msleep(20);

      camera_sw_power_onoff(1); //on
      msleep(20);

      hi351_CSI_CONFIG = 1;
    }
//     rc = hi351_i2c_write(hi351_client->addr,0x001A,0x0014,WORD_LEN);

//     rc = hi351_i2c_write_table(&hi351_regs.cambl[0],
//				hi351_regs.cambl_size);
    //turn off flash when preview
//    hi351_set_flash_light(LED_OFF);
    if (rt == QTR_SIZE) {
      /* preview setting */
    /* preview setting */
    rc = hi351_i2c_write_table(&hi351_regs.previewtbl[0],
			hi351_regs.previewtbl_size);
    } else if (rt == FULL_SIZE) {
      CDBG("--CAMERA-- snapshot in, is_autoflash - %d\n", is_autoflash);
      //hi351_get_preview_exposure_gain();
#if 0
      if (is_autoflash == 1) {
        hi351_i2c_read_byte(hi351_client->addr, 0x350b, &tmp);
        CDBG("--CAMERA-- GAIN VALUE : %x\n", tmp);
        if ((tmp & 0x80) != 0) {
          hi351_set_flash_light(LED_FULL);
        }
      }
#endif
#if 0    	
    rc = hi351_i2c_write(hi351_client->addr,0x098E,0xB008,WORD_LEN);
    rc = hi351_i2c_write(hi351_client->addr,0x0990,0x0001,WORD_LEN);
    msleep(50);
    while(retry-- > 0)
    {	 
	 pr_info("retry AF");
   	 rc = hi351_i2c_write(hi351_client->addr,0x098E,0x3000,WORD_LEN);
    	 rc = hi351_i2c_read(hi351_client->addr,0x0990, &readValue, WORD_LEN);
         if(!(readValue & 0x0002)){
		pr_info("AF OK");
		break;
	}
	msleep(50);
    }
#endif
    rc = hi351_i2c_write_table(&hi351_regs.snapshottbl[0],
					hi351_regs.snapshottbl_size); //Step2: change to full size
      //hi351_set_capture_exposure_gain();
    } else if (rt == HFR_60FPS) {
      //rc = hi351Core_WritePREG(hi351_preview_tbl_60fps);
    } else if (rt == HFR_90FPS) {
      //rc = hi351Core_WritePREG(hi351_preview_tbl_90fps);
    }
	}

  return rc;
}

static int hi351_video_config(int mode)
{
  int rc = 0;

  CDBG("--CAMERA-- hi351_video_config\n");
  /* change sensor resolution if needed */
  if ((rc = hi351_sensor_setting(S_UPDATE_PERIODIC,
    hi351_ctrl->prev_res)) < 0) {
    CDBG_HIGH("--CAMERA-- hi351_video_config failed\n");
    return rc;
  }

  hi351_ctrl->curr_res = hi351_ctrl->prev_res;
  hi351_ctrl->sensormode = mode;
  return rc;
}

static int hi351_snapshot_config(int mode)
{
  int rc = 0;

  CDBG("--CAMERA-- hi351_snapshot_config\n");
  /*change sensor resolution if needed */
  if (hi351_ctrl->curr_res != hi351_ctrl->pict_res) {
    if ((rc = hi351_sensor_setting(S_UPDATE_PERIODIC,
      hi351_ctrl->pict_res)) < 0) {
      CDBG_HIGH("--CAMERA-- hi351_snapshot_config failed\n");
      return rc;
    }
  }

  hi351_ctrl->curr_res = hi351_ctrl->pict_res;
  hi351_ctrl->sensormode = mode;
  return rc;
}

static int hi351_raw_snapshot_config(int mode)
{
  int rc = 0;

  CDBG("--CAMERA-- hi351_raw_snapshot_config\n");
  /* change sensor resolution if needed */
  if (hi351_ctrl->curr_res != hi351_ctrl->pict_res) {
    if ((rc = hi351_sensor_setting(S_UPDATE_PERIODIC,
      hi351_ctrl->pict_res)) < 0) {
      CDBG_HIGH("--CAMERA-- hi351_raw_snapshot_config failed\n");
      return rc;
    }
  }

  hi351_ctrl->curr_res = hi351_ctrl->pict_res;
  hi351_ctrl->sensormode = mode;
  return rc;
}

static int hi351_set_sensor_mode(int mode,
  int res)
{
  int rc = 0;

  switch (mode) {
  case SENSOR_PREVIEW_MODE:
  case SENSOR_HFR_60FPS_MODE:
  case SENSOR_HFR_90FPS_MODE:
    hi351_ctrl->prev_res = res;
    rc = hi351_video_config(mode);
    break;
  case SENSOR_SNAPSHOT_MODE:
    hi351_ctrl->pict_res = res;
    rc = hi351_snapshot_config(mode);
    break;
  case SENSOR_RAW_SNAPSHOT_MODE:
    hi351_ctrl->pict_res = res;
    rc = hi351_raw_snapshot_config(mode);
    break;
  default:
    rc = -EINVAL;
    break;
  }

  return rc;
}

//extern int lcd_camera_power_onoff(int on);

static int hi351_sensor_open_init(const struct msm_camera_sensor_info *data)
{
    int rc = -ENOMEM;
    CDBG("--CAMERA-- %s\n",__func__);

    data->pdata->camera_vreg_config(1,1250,1);	

    hi351_ctrl = kzalloc(sizeof(struct __hi351_ctrl), GFP_KERNEL);
    if (!hi351_ctrl)
    {
        CDBG_HIGH("--CAMERA-- kzalloc hi351_ctrl error !!\n");
        return rc;
    }
    hi351_ctrl->fps_divider = 1 * 0x00000400;
    hi351_ctrl->pict_fps_divider = 1 * 0x00000400;
    hi351_ctrl->set_test = S_TEST_OFF;
    hi351_ctrl->prev_res = QTR_SIZE;
    hi351_ctrl->pict_res = FULL_SIZE;

    if (data)
        hi351_ctrl->sensordata = data;

//    lcd_camera_power_onoff(1); //turn on LDO for PVT

    hi351_power_off();

    CDBG("--CAMERA-- %s: msm_camio_clk_rate_set\n", __func__);

    msm_camio_clk_rate_set(24000000);
    msleep(5);

    hi351_power_on();
    msleep(5);

    hi351_power_reset();
    msleep(5);

    CDBG("--CAMERA-- %s: init sequence\n", __func__);

    rc = hi351_sensor_setting(S_REG_INIT, hi351_ctrl->prev_res);
    if (rc < 0)
    {
        CDBG_HIGH("--CAMERA-- %s : hi351_setting failed. rc = %d\n",__func__,rc);
        kfree(hi351_ctrl);
        hi351_ctrl = NULL;

        hi351_power_off();

//        lcd_camera_power_onoff(0); //turn off LDO for PVT

        return rc;
    }

    hi351_CSI_CONFIG = 0;

    CDBG("--CAMERA--re_init_sensor ok!!\n");
    return rc;
}
#if 1
static void hi351_reset_parameters(void)
{
	hi351_effect = CAMERA_EFFECT_OFF;
	hi351_whitebalance = CAMERA_WB_AUTO;
	hi351_contrast = CAMERA_CONTRAST_LV2*2;
	hi351_sharpness = CAMERA_SHARPNESS_LV2*5;
	hi351_saturation = CAMERA_SATURATION_LV2*2;
	hi351_brightness = CAMERA_BRIGHTNESS_LV2;
}
#endif
	

static int hi351_sensor_release(void)
{
    CDBG("--CAMERA--hi351_sensor_release!!\n");

//    hi351_i2c_write_table(&hi351_regs.previewtbl[0],
//			hi351_regs.previewtbl_size);
    hi351_i2c_write_table(&hi351_regs.releasetbl[0],
			hi351_regs.releasetbl_size);
    msleep(250);
 
    mutex_lock(&hi351_mutex);

    hi351_i2c_write(hi351_client->addr,0x03,0x00,BYTE_BYTE_LEN);
    hi351_i2c_write(hi351_client->addr,0x01,0x03,BYTE_BYTE_LEN);
    hi351_i2c_write(hi351_client->addr,0x03,0x02,BYTE_BYTE_LEN);
    hi351_i2c_write(hi351_client->addr,0x55,0x10,BYTE_BYTE_LEN);
    msleep(5);
    hi351_i2c_write(hi351_client->addr,0x03,0x02,BYTE_BYTE_LEN);
    hi351_i2c_write(hi351_client->addr,0x3d,0x00,BYTE_BYTE_LEN);


    hi351_power_off();
    mdelay(5);
//    gpio_direction_output(hi351_reset_gpio, 0);   //reset camera reset pin
//    mdelay(5);
    hi351_reset_parameters();
//    lcd_camera_power_onoff(0);

    if(NULL != hi351_ctrl)
    {
        kfree(hi351_ctrl);
        hi351_ctrl = NULL;
    }
    hi351_CSI_CONFIG = 0;

    mutex_unlock(&hi351_mutex);
    return 0;
}

static const struct i2c_device_id hi351_i2c_id[] = {
    {"hi351", 0},{}
};

static int hi351_i2c_remove(struct i2c_client *client)
{
   if(NULL != hi351_sensorw)
   {
        kfree(hi351_sensorw);
        hi351_sensorw = NULL;

   }
    hi351_client = NULL;
   return 0;
}

static int hi351_init_client(struct i2c_client *client)
{
   /* Initialize the MSM_CAMI2C Chip */
   init_waitqueue_head(&hi351_wait_queue);
   return 0;
}
#if 1
static long hi351_set_effect(int mode, int effect)
{	
	long rc = 0;
	if (hi351_effect == effect)
		return 0;
#ifdef hi351_DEBUG
	printk("enter hi351_set_effect effect: %d,\n", effect);
#endif
	switch (effect) {
	case CAMERA_EFFECT_OFF: {
		rc = hi351_i2c_write_table(&hi351_regs.effecttbl_off[0],
					hi351_regs.effecttbl_size_off);
		if (rc < 0)
			return rc;
	}
		break;
	case CAMERA_EFFECT_MONO:{
		rc = hi351_i2c_write_table(&hi351_regs.effecttbl_mono[0],
					hi351_regs.effecttbl_size_mono);
		if (rc < 0)
			return rc;
	}
		break;
	
	case CAMERA_EFFECT_NEGATIVE: {
		rc = hi351_i2c_write_table(&hi351_regs.effecttbl_negative[0],
					hi351_regs.effecttbl_size_negative);
		if (rc < 0)
			return rc;
	}
		break;

	case CAMERA_EFFECT_SOLARIZE: {
		rc = hi351_i2c_write_table(&hi351_regs.effecttbl_solarize[0],
					hi351_regs.effecttbl_size_solarize);
		if (rc < 0)
			return rc;		
	}
		break;

	case CAMERA_EFFECT_SEPIA: {
		rc = hi351_i2c_write_table(&hi351_regs.effecttbl_sepia[0],
					hi351_regs.effecttbl_size_sepia);
		if (rc < 0)
			return rc;		
	}
		break;

	default: {
		rc = hi351_i2c_write_table(&hi351_regs.effecttbl_off[0],
					hi351_regs.effecttbl_size_off);
		if (rc < 0)
			return rc;	
	}
	}
	hi351_effect = effect;
	return rc;
}

static int hi351_set_brightness(int8_t brightness)
{
	long rc = 0;
	if (hi351_brightness == brightness)
		return 0;
#ifdef hi351_DEBUG
	printk("enter hi351_set_brightness brightness: %d, hi351_brightness:%d by ldx\n", brightness, hi351_brightness);
#endif
	switch (brightness) {
	case CAMERA_BRIGHTNESS_LV0: {		
		rc = hi351_i2c_write_table(&hi351_regs.brightnesstbl_0[0],
					hi351_regs.brightnesstbl_size_0);
		if (rc < 0)
			return rc;

	}
		break;
	case CAMERA_BRIGHTNESS_LV1: {		
		rc = hi351_i2c_write_table(&hi351_regs.brightnesstbl_1[0],
					hi351_regs.brightnesstbl_size_1);
		if (rc < 0)
			return rc;

	}
		break;
	case CAMERA_BRIGHTNESS_LV2: {		
		rc = hi351_i2c_write_table(&hi351_regs.brightnesstbl_2[0],
					hi351_regs.brightnesstbl_size_2);
		if (rc < 0)
			return rc;

	}
		break;
	case CAMERA_BRIGHTNESS_LV3: {		
		rc = hi351_i2c_write_table(&hi351_regs.brightnesstbl_3[0],
					hi351_regs.brightnesstbl_size_3);
		if (rc < 0)
			return rc;

	}
		break;
	case CAMERA_BRIGHTNESS_LV4: {		
		rc = hi351_i2c_write_table(&hi351_regs.brightnesstbl_4[0],
					hi351_regs.brightnesstbl_size_4);
		if (rc < 0)
			return rc;

	}
		break;
	case CAMERA_BRIGHTNESS_LV5:
	case CAMERA_BRIGHTNESS_LV6:
	default:
		break;
	}

	hi351_brightness= brightness;
	return rc;
}

static int hi351_set_contrast(int contrast)
{
	long rc = 0;
	if (hi351_contrast == contrast)
		return 0;
#ifdef hi351_DEBUG
	printk("enter hi351_set_contrast contrast: %d \n", contrast);
#endif
	switch (contrast/2) {
	case CAMERA_CONTRAST_LV0: {		
		rc = hi351_i2c_write_table(&hi351_regs.contrasttbl_0[0],
					hi351_regs.contrasttbl_size_0);
		if (rc < 0)
			return rc;
	}
		break;

	case CAMERA_CONTRAST_LV1: {
		rc = hi351_i2c_write_table(&hi351_regs.contrasttbl_1[0],
					hi351_regs.contrasttbl_size_1);
		if (rc < 0)
			return rc;
	}
		break;

	case CAMERA_CONTRAST_LV2: {
		rc = hi351_i2c_write_table(&hi351_regs.contrasttbl_2[0],
					hi351_regs.contrasttbl_size_2);
		if (rc < 0)
			return rc;		
	}
		break;

	case CAMERA_CONTRAST_LV3: {
		rc = hi351_i2c_write_table(&hi351_regs.contrasttbl_3[0],
					hi351_regs.contrasttbl_size_3);
		if (rc < 0)
			return rc;	
	}
		break;

	case CAMERA_CONTRAST_LV4: {
		rc = hi351_i2c_write_table(&hi351_regs.contrasttbl_4[0],
					hi351_regs.contrasttbl_size_4);
		if (rc < 0)
			return rc;		
	}
		break;
	
	default: {
		rc = hi351_i2c_write_table(&hi351_regs.contrasttbl_2[0],
					hi351_regs.contrasttbl_size_2);
		if (rc < 0)
			return rc;
	}
	}
	hi351_contrast= contrast;
	return rc;
}

static int hi351_set_sharpness(int sharpness)
{
	long rc = 0;
	if (hi351_sharpness == sharpness)
		return 0;
#ifdef hi351_DEBUG
	printk("enter hi351_set_sharpness sharpness: %d, hi351_sharpness:%d by ldx\n", sharpness, hi351_sharpness);
#endif
	switch (sharpness/6) {
	case CAMERA_SHARPNESS_LV0: {
		rc = hi351_i2c_write_table(&hi351_regs.sharpnesstbl_0[0],
					hi351_regs.sharpnesstbl_size_0);
		if (rc < 0)
			return rc;		
	}
		break;

	case CAMERA_SHARPNESS_LV1: {
		rc = hi351_i2c_write_table(&hi351_regs.sharpnesstbl_1[0],
					hi351_regs.sharpnesstbl_size_1);
		if (rc < 0)
			return rc;
	}
		break;

	case CAMERA_SHARPNESS_LV2: {
		rc = hi351_i2c_write_table(&hi351_regs.sharpnesstbl_2[0],
					hi351_regs.sharpnesstbl_size_2);
		if (rc < 0)
			return rc;
	}
		break;

	case CAMERA_SHARPNESS_LV3: {
		rc = hi351_i2c_write_table(&hi351_regs.sharpnesstbl_3[0],
					hi351_regs.sharpnesstbl_size_3);
		if (rc < 0)
			return rc;		
	}
		break;

	case CAMERA_SHARPNESS_LV4: {
		rc = hi351_i2c_write_table(&hi351_regs.sharpnesstbl_4[0],
					hi351_regs.sharpnesstbl_size_4);
		if (rc < 0)
			return rc;		
	}
		break;

	case CAMERA_SHARPNESS_LV5:
	default: {
		rc = hi351_i2c_write_table(&hi351_regs.sharpnesstbl_2[0],
					hi351_regs.sharpnesstbl_size_2);
		if (rc < 0)
			return rc;
	}
	}
	hi351_sharpness= sharpness;
	return rc;
}

static int hi351_set_saturation(int saturation)
{
	long rc = 0;
	if (hi351_saturation == saturation) 
		return 0;
#ifdef hi351_DEBUG	
	printk("enter hi351_set_saturation saturation: %d, hi351_saturation:%d by ldx\n", saturation, hi351_saturation);
#endif
	switch (saturation/2) {
	case CAMERA_SATURATION_LV0: {
		rc = hi351_i2c_write_table(&hi351_regs.saturationtbl_0[0],
					hi351_regs.saturationtbl_size_0);
		if (rc < 0)
			return rc;
	}
		break;
	case CAMERA_SATURATION_LV1: {
		rc = hi351_i2c_write_table(&hi351_regs.saturationtbl_1[0],
					hi351_regs.saturationtbl_size_1);
		if (rc < 0)
			return rc;
	}
		break;

	case CAMERA_SATURATION_LV2: {
		rc = hi351_i2c_write_table(&hi351_regs.saturationtbl_2[0],
					hi351_regs.saturationtbl_size_2);
		if (rc < 0)
			return rc;		
	}
		break;

	case CAMERA_SATURATION_LV3: {
		rc = hi351_i2c_write_table(&hi351_regs.saturationtbl_3[0],
					hi351_regs.saturationtbl_size_3);
		if (rc < 0)
			return rc;	
	}
		break;

	case CAMERA_SATURATION_LV4: {
		rc = hi351_i2c_write_table(&hi351_regs.saturationtbl_4[0],
					hi351_regs.saturationtbl_size_4);
		if (rc < 0)
			return rc;	
	}
		break;
	
	default: {
		rc = hi351_i2c_write_table(&hi351_regs.saturationtbl_2[0],
					hi351_regs.saturationtbl_size_2);
		if (rc < 0)
			return rc;
	}
	}
	hi351_saturation= saturation;
	return rc;
}

static long hi351_set_antibanding(int antibanding)
{
    long rc = 0;
    return rc;
#if 0	
    //int i = 0;
    CDBG("--CAMERA-- %s ...(Start)\n", __func__);
    CDBG("--CAMERA-- %s ...antibanding = %d\n", __func__, antibanding);

    switch (antibanding)
    {
    case CAMERA_ANTIBANDING_OFF:
        CDBG("--CAMERA--CAMERA_ANTIBANDING_OFF\n");
        break;

    case CAMERA_ANTIBANDING_60HZ:
        CDBG("--CAMERA--CAMERA_ANTIBANDING_60HZ\n");
        rc = hi351Core_WritePREG(hi351_antibanding_60z_tbl);
        break;

    case CAMERA_ANTIBANDING_50HZ:
        CDBG("--CAMERA--CAMERA_ANTIBANDING_50HZ\n");
        rc = hi351Core_WritePREG(hi351_antibanding_50z_tbl);
        break;

    case CAMERA_ANTIBANDING_AUTO:
        CDBG("--CAMERA--CAMERA_ANTIBANDING_AUTO\n");
        rc = hi351Core_WritePREG(hi351_antibanding_auto_tbl);
        break;

    default:
        CDBG_HIGH("--CAMERA--CAMERA_ANTIBANDING_ERROR COMMAND\n");
        break;
    }
    CDBG("--CAMERA-- %s ...(End)\n",__func__);
    return rc;
#endif	
}

static long hi351_set_exposure_mode(int mode)
{
    long rc = 0;
    //int i =0;
    CDBG("--CAMERA-- %s ...(Start)\n",__func__);
    CDBG("--CAMERA-- %s ...mode = %d\n",__func__ , mode);
#if 0
    switch (mode)
    {
    case CAMERA_SETAE_AVERAGE:
        CDBG("--CAMERA--CAMERA_SETAE_AVERAGE\n");
        rc = hi351Core_WritePREG(hi351_ae_average_tbl);
        break;
    case CAMERA_SETAE_CENWEIGHT:
        CDBG("--CAMERA--CAMERA_SETAE_CENWEIGHT\n");
        rc = hi351Core_WritePREG(hi351_ae_centerweight_tbl);
        break;
    default:
        CDBG("--CAMERA--ERROR COMMAND OR NOT SUPPORT\n");
        break;
    }
#endif
    CDBG("--CAMERA-- %s ...(End)\n",__func__);
    return rc;
}

static int32_t hi351_lens_shading_enable(uint8_t is_enable)
{
    int32_t rc = 0;
    return rc;
#if 0	
    CDBG("--CAMERA--%s: ...(Start). enable = %d\n", __func__, is_enable);

    if(is_enable)
    {
        CDBG("--CAMERA--%s: enable~!!\n", __func__);
        rc = hi351Core_WritePREG(hi351_lens_shading_on_tbl);
    }
    else
    {
        CDBG("--CAMERA--%s: disable~!!\n", __func__);
        rc = hi351Core_WritePREG(hi351_lens_shading_off_tbl);
    }
    CDBG("--CAMERA--%s: ...(End). rc = %d\n", __func__, rc);
    return rc;
#endif	
}

static int hi351_set_wb_oem(uint8_t param)
{
    int rc = 0;
    CDBG("--CAMERA--%s wb: %d runs\r\n",__func__,param );
    if(hi351_whitebalance==param)
	return 0;
    switch(param)
    {
    case CAMERA_WB_AUTO:

        CDBG("--CAMERA--CAMERA_WB_AUTO\n");
        rc = hi351_i2c_write_table(&hi351_regs.wbtbl_auto[0],hi351_regs.wbtbl_size_auto);
        break;

    case CAMERA_WB_FLUORESCENT:
        CDBG("--CAMERA--CAMERA_WB_FLUORESCENT\n");
        rc = hi351_i2c_write_table(&hi351_regs.wbtbl_flourescent[0],hi351_regs.wbtbl_size_flourescent);
        break;
    case CAMERA_WB_INCANDESCENT:
        CDBG("--CAMERA--CAMERA_WB_INCANDESCENT\n");
        rc = hi351_i2c_write_table(&hi351_regs.wbtbl_incandescent[0],hi351_regs.wbtbl_size_incandescent);
        break;
    case CAMERA_WB_DAYLIGHT:
        CDBG("--CAMERA--CAMERA_WB_DAYLIGHT\n");
        rc = hi351_i2c_write_table(&hi351_regs.wbtbl_daylight[0],hi351_regs.wbtbl_size_daylight);
        break;
    case CAMERA_WB_CLOUDY_DAYLIGHT:
        CDBG("--CAMERA--CAMERA_WB_CLOUDY_DAYLIGHT\n");
        rc = hi351_i2c_write_table(&hi351_regs.wbtbl_cloudy[0],hi351_regs.wbtbl_size_cloudy);
        break;
    default:
        CDBG("--CAMERA--CAMERA_WB_AUTO\n");
        rc = hi351_i2c_write_table(&hi351_regs.wbtbl_auto[0],hi351_regs.wbtbl_size_auto);
        break;
    }
    hi351_whitebalance = param;
    return rc;
}
#endif
#if 0
static int hi351_set_touchaec(uint32_t x,uint32_t y)
{
    uint8_t aec_arr[8] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11};
    int idx = 0;
    int i;
    CDBG("--CAMERA--%s x: %d ,y: %d\r\n",__func__ ,x,y);
    idx = x /2 + y *2;
    CDBG("--CAMERA--idx: %d\r\n",idx);

    if (x %2 == 0)
    {
        aec_arr[idx] = 0x10 | 0x0a;
    }
    else
    {
        aec_arr[idx] = 0x01 | 0xa0;
    }

    for (i = 0;i < 8; i++)
    {
        CDBG("--CAMERA--write : %x val : %x ", 0x5688 + i, aec_arr[i]);
        hi351_i2c_write(hi351_client->addr, 0x5688 + i, aec_arr[i], 10);
    }

    return 1;
}

//QRD
static int hi351_set_exposure_compensation(int compensation)
{
    long rc = 0;
    //int i = 0;

    CDBG("--CAMERA-- %s ...(Start)\n",__func__);

    CDBG("--CAMERA-- %s ...exposure_compensation = %d\n",__func__ , compensation);

    switch(compensation)
    {
case CAMERA_EXPOSURE_COMPENSATION_LV0:
    CDBG("--CAMERA--CAMERA_EXPOSURE_COMPENSATION_LV0\n");
    rc = hi351Core_WritePREG(hi351_exposure_compensation_lv0_tbl);
    break;
case CAMERA_EXPOSURE_COMPENSATION_LV1:
    CDBG("--CAMERA--CAMERA_EXPOSURE_COMPENSATION_LV1\n");
    rc = hi351Core_WritePREG(hi351_exposure_compensation_lv1_tbl);
    break;
case CAMERA_EXPOSURE_COMPENSATION_LV2:
    CDBG("--CAMERA--CAMERA_EXPOSURE_COMPENSATION_LV2\n");
    rc = hi351Core_WritePREG(hi351_exposure_compensation_lv2_default_tbl);
    break;
case CAMERA_EXPOSURE_COMPENSATION_LV3:
    CDBG("--CAMERA--CAMERA_EXPOSURE_COMPENSATION_LV3\n");
    rc = hi351Core_WritePREG(hi351_exposure_compensation_lv3_tbl);
    break;
case CAMERA_EXPOSURE_COMPENSATION_LV4:
    CDBG("--CAMERA--CAMERA_EXPOSURE_COMPENSATION_LV3\n");
    rc = hi351Core_WritePREG(hi351_exposure_compensation_lv4_tbl);
    break;
default:
    CDBG_HIGH("--CAMERA--ERROR CAMERA_EXPOSURE_COMPENSATION\n");
    break;
    }

    CDBG("--CAMERA-- %s ...(End)\n",__func__);

    return rc;
}
#endif
static int hi351_set_iso(int8_t iso_type)
{
    long rc = 0;
    //int i = 0;
    return rc;
#if 0	
    CDBG("--CAMERA-- %s ...(Start)\n",__func__);

    CDBG("--CAMERA-- %s ...iso_type = %d\n",__func__ , iso_type);

    switch(iso_type)
    {
case CAMERA_ISO_TYPE_AUTO:
    CDBG("--CAMERA--CAMERA_ISO_TYPE_AUTO\n");
    rc = hi351Core_WritePREG(hi351_iso_type_auto);
    break;
case CAMEAR_ISO_TYPE_HJR:
    CDBG("--CAMERA--CAMEAR_ISO_TYPE_HJR\n");
    rc = hi351Core_WritePREG(hi351_iso_type_auto);
    break;
case CAMEAR_ISO_TYPE_100:
    CDBG("--CAMERA--CAMEAR_ISO_TYPE_100\n");
    rc = hi351Core_WritePREG(hi351_iso_type_100);
    break;
case CAMERA_ISO_TYPE_200:
    CDBG("--CAMERA--CAMERA_ISO_TYPE_200\n");
    rc = hi351Core_WritePREG(hi351_iso_type_200);
    break;
case CAMERA_ISO_TYPE_400:
    CDBG("--CAMERA--CAMERA_ISO_TYPE_400\n");
    rc = hi351Core_WritePREG(hi351_iso_type_400);
    break;
case CAMEAR_ISO_TYPE_800:
    CDBG("--CAMERA--CAMEAR_ISO_TYPE_800\n");
    rc = hi351Core_WritePREG(hi351_iso_type_800);
    break;
case CAMERA_ISO_TYPE_1600:
    CDBG("--CAMERA--CAMERA_ISO_TYPE_1600\n");
    rc = hi351Core_WritePREG(hi351_iso_type_1600);
    break;

default:
    CDBG_HIGH("--CAMERA--ERROR ISO TYPE\n");
    break;
    }

    CDBG("--CAMERA-- %s ...(End)\n",__func__);

    return rc;
#endif
}



static int hi351_sensor_start_af(void)
{
    int rc = 0;
//    rc = hi351_i2c_write(hi351_client->addr,0x098E,0xB008,WORD_LEN);
//    rc = hi351_i2c_write(hi351_client->addr,0x0990,0x0001,WORD_LEN);
//    msleep(200);
    return rc;
}

static int hi351_sensor_config(void __user *argp)
{
    struct sensor_cfg_data cdata;
    long rc = 0;

    if (copy_from_user(&cdata, (void *)argp, sizeof(struct sensor_cfg_data))) 
        return -EFAULT;

    CDBG("--CAMERA-- %s %d\n",__func__,cdata.cfgtype);

    mutex_lock(&hi351_mutex);
    switch (cdata.cfgtype)
    {
        case CFG_SET_MODE:
            rc =hi351_set_sensor_mode(cdata.mode, cdata.rs);
            break;
        case CFG_SET_EFFECT:
            CDBG("--CAMERA-- CFG_SET_EFFECT mode=%d, effect = %d !!\n",cdata.mode, cdata.cfg.effect);
            rc = hi351_set_effect(cdata.mode, cdata.cfg.effect);
            break;
        case CFG_START:
            CDBG("--CAMERA-- CFG_START (Not Support) !!\n");
            // Not Support
            break;
        case CFG_PWR_UP:
            CDBG("--CAMERA-- CFG_PWR_UP (Not Support) !!\n");
            // Not Support
            break;
        case CFG_PWR_DOWN:
            CDBG("--CAMERA-- CFG_PWR_DOWN (Not Support) \n");
            hi351_power_off();
            break;
        case CFG_SET_DEFAULT_FOCUS:
            CDBG("--CAMERA-- CFG_SET_DEFAULT_FOCUS (Not Implement) !!\n");
            break;        
        case CFG_MOVE_FOCUS:
            CDBG("--CAMERA-- CFG_MOVE_FOCUS (Not Implement) !!\n");
            break;
        case CFG_SET_BRIGHTNESS:
            CDBG("--CAMERA-- CFG_SET_BRIGHTNESS  !!\n");
            rc = hi351_set_brightness(cdata.cfg.brightness);
            break;
        case CFG_SET_CONTRAST:
            CDBG("--CAMERA-- CFG_SET_CONTRAST  !!\n");
            rc = hi351_set_contrast(cdata.cfg.contrast);
            break;            
        case CFG_SET_EXPOSURE_MODE:
            CDBG("--CAMERA-- CFG_SET_EXPOSURE_MODE !!\n");
            rc = hi351_set_exposure_mode(cdata.cfg.ae_mode);
            break;
        case CFG_SET_ANTIBANDING:
            CDBG("--CAMERA-- CFG_SET_ANTIBANDING antibanding = %d!!\n", cdata.cfg.antibanding);
            rc = hi351_set_antibanding(cdata.cfg.antibanding);
            break;
        case CFG_SET_LENS_SHADING:
            CDBG("--CAMERA-- CFG_SET_LENS_SHADING !!\n");
            rc = hi351_lens_shading_enable(cdata.cfg.lens_shading);
            break;
        case CFG_SET_SATURATION:
            CDBG("--CAMERA-- CFG_SET_SATURATION !!\n");
            rc = hi351_set_saturation(cdata.cfg.saturation);
            break;
        case CFG_SET_SHARPNESS:
            CDBG("--CAMERA-- CFG_SET_SHARPNESS !!\n");
            rc = hi351_set_sharpness(cdata.cfg.sharpness);
            break;
        case CFG_SET_WB:
            CDBG("--CAMERA-- CFG_SET_WB!!\n");
            rc = hi351_set_wb_oem(cdata.cfg.wb_val);
            break;
           
        case CFG_SET_TOUCHAEC:
            CDBG("--CAMERA-- CFG_SET_TOUCHAEC!!\n");
           // hi351_set_touchaec(cdata.cfg.aec_cord.x, cdata.cfg.aec_cord.y);
            rc = 0 ;
            break;
        case CFG_SET_AUTO_FOCUS:
            CDBG("--CAMERA-- CFG_SET_AUTO_FOCUS !\n");
            rc = hi351_sensor_start_af();
            break;
        case CFG_SET_AUTOFLASH:
            CDBG("--CAMERA-- CFG_SET_AUTOFLASH !\n");
            is_autoflash = cdata.cfg.is_autoflash;
            CDBG("--CAMERA-- is autoflash %d\r\n",is_autoflash);
            rc = 0;
            break;
        case CFG_SET_EXPOSURE_COMPENSATION:
            CDBG("--CAMERA-- CFG_SET_EXPOSURE_COMPENSATION !\n");
            //rc = hi351_set_exposure_compensation(cdata.cfg.exp_compensation);
            break;

        case CFG_SET_ISO:
            CDBG("--CAMERA-- CFG_SET_ISO !\n");
            rc = hi351_set_iso(cdata.cfg.iso_type);
            break;

        default:
            CDBG_HIGH("--CAMERA-- %s: Command=%d (Not Implement)!!\n",__func__,cdata.cfgtype);
            rc = -EINVAL;
            break;
    }
    mutex_unlock(&hi351_mutex);
    return rc;    
}

static struct i2c_driver hi351_i2c_driver = {
    .id_table = hi351_i2c_id,
    .probe  = hi351_i2c_probe,
    .remove = hi351_i2c_remove,
    .driver = {
        .name = "hi351",
    },
};

static int hi351_probe_init_gpio(const struct msm_camera_sensor_info *data)
{
    int rc = 0;

    CDBG("--CAMERA-- %s\n",__func__);

    hi351_pwdn_gpio = data->sensor_pwd;
    hi351_reset_gpio = data->sensor_reset;
    hi351_driver_pwdn_gpio = data->vcm_pwd ;

//    rc = gpio_request(hi351_reset_gpio,"hi351");
    if(rc < 0){
	CDBG("%s:gpio_request----reset failed!",__func__);    
    }
	
//    rc = gpio_request(hi351_pwdn_gpio,"hi351");
    if(rc < 0){
	CDBG("%s:gpio_request----pwdn failed!",__func__);    
    }
	
  if (data->vcm_enable)
    {
//        gpio_direction_output(data->vcm_pwd, 1);
    }

//    gpio_direction_output(data->sensor_reset, 1);
//    gpio_direction_output(data->sensor_pwd, 0);

    return rc;

}
#if 0
static void hi351_probe_free_gpio(const struct msm_camera_sensor_info *data)
{
    gpio_free(hi351_pwdn_gpio);
    gpio_free(hi351_reset_gpio);

    if (data->vcm_enable)
    {
        gpio_free(hi351_driver_pwdn_gpio);
        hi351_driver_pwdn_gpio = 0xFF ;
    }

    hi351_pwdn_gpio = 0xFF;
    hi351_reset_gpio = 0xFF;
}
#endif

#if 1 // OTP PART
static unsigned short otp_id;
static const char *hi351Module = "hi351";
static const char *hi351ModuleId;

static ssize_t sensor_Module_id_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;

	if (otp_id == 0x10)
		hi351ModuleId = "Truly";
	else
		hi351ModuleId = "Kerr";
	
	sprintf(buf, "%s %s \n", hi351ModuleId, hi351Module);
	ret = strlen(buf) + 1;

	return ret;
}

static DEVICE_ATTR(sensor, 0444, sensor_Module_id_show, NULL);

static struct kobject *android_hi351;

static int hi351_sysfs_init(void)
{
	int ret ;

	android_hi351 = kobject_create_and_add("android_camera", NULL);
	if (android_hi351 == NULL) {
		pr_info("hi351_sysfs_init: subsystem_register " \
		"failed\n");
		ret = -ENOMEM;
		return ret ;
	}
	ret = sysfs_create_file(android_hi351, &dev_attr_sensor.attr);
	if (ret) {
		pr_info("hi351_sysfs_init: sysfs_create_file " \
		"failed\n");
		kobject_del(android_hi351);
	}
	return 0 ;
}

#endif
static int hi351_sensor_probe(const struct msm_camera_sensor_info *info,struct msm_sensor_ctrl *s)
{
    int rc = -ENOTSUPP;

    CDBG("--CAMERA-- %s (Start...)\n",__func__);

    info->pdata->camera_vreg_config(1,1250,1);

    rc = i2c_add_driver(&hi351_i2c_driver);
    CDBG("--CAMERA-- i2c_add_driver ret:0x%x,hi351_client=0x%x\n",
         rc, (unsigned int)hi351_client);
    if ((rc < 0 ) || (hi351_client == NULL))
    {
        CDBG_HIGH("--CAMERA-- i2c_add_driver FAILS!!\n");

        if(NULL != hi351_sensorw)
        {
            kfree(hi351_sensorw);
            hi351_sensorw = NULL;
        }

        hi351_client = NULL;
	
	info->pdata->camera_vreg_config(0,1250,1);	

        return rc;

    }

//    lcd_camera_power_onoff(1);

    rc = hi351_probe_init_gpio(info);
    if (rc < 0) {
        CDBG_HIGH("--CAMERA-- %s, init gpio failed\n",__func__);
        i2c_del_driver(&hi351_i2c_driver);

	info->pdata->camera_vreg_config(0,1250,1);

        return rc;
    }

    hi351_power_off();

    CDBG("--CAMERA-- %s: msm_camio_clk_rate_set\n", __func__);
    /* SENSOR NEED MCLK TO DO I2C COMMUNICTION, OPEN CLK FIRST*/
    msm_camio_clk_rate_set(24000000);

    msleep(5);

    hi351_power_on();
    msleep(5);
    hi351_power_reset();
    msleep(5);

    rc = hi351_probe_readID(info);
	
    if (rc < 0)
    {

        CDBG_HIGH("--CAMERA--hi351_probe_readID Fail !!~~~~!!\n");
        CDBG_HIGH("--CAMERA-- %s, unregister\n",__func__);

        i2c_del_driver(&hi351_i2c_driver);
    msleep(5);
        hi351_power_off();
    msleep(5);
//        lcd_camera_power_onoff(0);

	info->pdata->camera_vreg_config(0,1250,1);

//        hi351_probe_free_gpio(info);

        return rc;
    }


#if 1 // OTP PART
	hi351_i2c_write_table(&hi351_regs.cambl_otp[0],
	hi351_regs.cambl_otp_size);

	hi351_i2c_read(hi351_client->addr,
	0x22, &otp_id, BYTE_BYTE_LEN);

	CDBG_HIGH("--CAMERA otp id = 0x%x \n",otp_id);

	hi351_i2c_write_table(&hi351_regs.cambl_otp2[0],
	hi351_regs.cambl_otp2_size);

	hi351_sysfs_init();
#endif
	

//    rc = hi351_i2c_write_table(&hi351_regs.cambl[0],
//				hi351_regs.cambl_size);
    msleep(5);
      camera_sw_power_onoff(0); //standby
    msleep(5);
      camera_sw_power_onoff(1); //standby
    msleep(5);
//    rc = hi351_i2c_write_table(&hi351_regs.previewtbl[0],
//			hi351_regs.previewtbl_size);

    s->s_init = hi351_sensor_open_init;
    s->s_release = hi351_sensor_release;
    s->s_config  = hi351_sensor_config;
    //s->s_AF = hi351_sensor_set_af;
    //camera_init_flag = true;

    s->s_camera_type = BACK_CAMERA_2D;
    s->s_mount_angle = info->sensor_platform_info->mount_angle;

    hi351_i2c_write(hi351_client->addr,0x03,0x00,BYTE_BYTE_LEN);
    hi351_i2c_write(hi351_client->addr,0x01,0x03,BYTE_BYTE_LEN);
    hi351_i2c_write(hi351_client->addr,0x03,0x02,BYTE_BYTE_LEN);
    hi351_i2c_write(hi351_client->addr,0x55,0x10,BYTE_BYTE_LEN);
    msleep(5);
    hi351_i2c_write(hi351_client->addr,0x03,0x02,BYTE_BYTE_LEN);
    hi351_i2c_write(hi351_client->addr,0x3d,0x00,BYTE_BYTE_LEN);
    msleep(5);



	
    hi351_power_off();
    msleep(5);


    CDBG("--CAMERA-- %s (End...)\n",__func__);
    return rc;
}

static int hi351_i2c_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
    CDBG("--CAMERA-- %s ... (Start...)\n",__func__);

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
    {
      CDBG_HIGH("--CAMERA--i2c_check_functionality failed\n");
      return -ENOMEM;
    }

    hi351_sensorw = kzalloc(sizeof(struct hi351_work), GFP_KERNEL);
    if (!hi351_sensorw)
    {
      CDBG_HIGH("--CAMERA--kzalloc failed\n");
      return -ENOMEM;
    }
    i2c_set_clientdata(client, hi351_sensorw);
    hi351_init_client(client);
    hi351_client = client;

    CDBG("--CAMERA-- %s ... (End...)\n",__func__);
    return 0;
}

static int __hi351_probe(struct platform_device *pdev)
{
   return msm_camera_drv_start(pdev, hi351_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
    .probe = __hi351_probe,
    .driver = {
        .name = "msm_camera_hi351",
        .owner = THIS_MODULE,
    },
};

static int __init hi351_init(void)
{
    return platform_driver_register(&msm_camera_driver);
}


module_init(hi351_init);

MODULE_DESCRIPTION("hi351 YUV MIPI sensor driver");
MODULE_LICENSE("GPL v2");
