/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
#include <linux/debugfs.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/bitops.h>
#include <mach/camera.h>
#include <media/msm_camera.h>
#include "s5k5.h"

/* 16bit address - 8 bit context register structure */
#define Q8	0x00000100
#define Q10	0x00000400

/* MCLK */
#define S5K5_MASTER_CLK_RATE 24000000

/* AF Total steps parameters */
#define S5K5_TOTAL_STEPS_NEAR_TO_FAR	32

#define S5K5_REG_PREV_FRAME_LEN_1	31
#define S5K5_REG_PREV_FRAME_LEN_2	32
#define S5K5_REG_PREV_LINE_LEN_1	33
#define S5K5_REG_PREV_LINE_LEN_2	34

#define S5K5_REG_SNAP_FRAME_LEN_1	15
#define S5K5_REG_SNAP_FRAME_LEN_2	16
#define S5K5_REG_SNAP_LINE_LEN_1	17
#define S5K5_REG_SNAP_LINE_LEN_2	18
#define MSB                             1
#define LSB                             0

#ifdef CDBG
#undef CDBG
#endif
#ifdef CDBG_DW
#undef CDBG_DW
#endif

#define s5k5_VERBOSE_DGB
//#define SENSOR_i2c_data_DEBUG 0
#ifdef s5k5_VERBOSE_DGB
#define CDBG(fmt, args...) pr_debug(fmt, ##args)
#define CDBG_DW(fmt, args...) pr_debug(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#define CDBG_DW(fmt, args...) pr_debug(fmt, ##args)
#endif

static bool CSI_CONFIG;
static struct s5k5_ctrl_t *s5k5_ctrl;

static DECLARE_WAIT_QUEUE_HEAD(s5k5_wait_queue);
static DECLARE_WAIT_QUEUE_HEAD(s5k5_af_wait_queue);
DEFINE_MUTEX(s5k5_mut);



static int s5k5_pwdn_gpio;
static int s5k5_reset_gpio;

#define S5K5CAGA_WRITE_REG_EX(reg, val) do{\
    if(S5K5CAGA_write_reg_ex((unsigned short)reg, (unsigned short)val) < 0)\
    {\
        return -1;\
    }\
}while(0)

#define S5K5CAGA_READ_REG(reg, val) do{\
    if(S5K5CAGA_read_reg((unsigned int)reg, (unsigned short*)val) < 0)\
    {\
        return -1;\
    }\
}while(0)
/////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
///
/*
static int s5k5_i2c_rxdata(unsigned short saddr,
		unsigned char *rxdata, int length)
{
	struct i2c_msg msgs[] = {
		{
			.addr  = saddr,
			.flags = 0,
			.len   = 1,
			.buf   = rxdata,
		},
		{
			.addr  = saddr,
			.flags = I2C_M_RD,
			.len   = 1,
			.buf   = rxdata,
		},
	};
	if (i2c_transfer(s5k5_client->adapter, msgs, 2) < 0) {
		CDBG_DW("s5k5_i2c_rxdata faild 0x%x\n", saddr);
		return -EIO;
	}
	return 0;
}
*/
/*
static int32_t s5k5_i2c_txdata(unsigned short saddr,
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
	if (i2c_transfer(s5k5_client->adapter, msg, 1) < 0) {
		CDBG_DW("s5k5_i2c_txdata faild 0x%x\n", saddr);
		return -EIO;
	}

	return 0;
}
*/
/*
 **************************************************************************
 * FunctionName: S5K5CAGA_read_reg;
 * Description : read S5K5CAGA reg by i2c;
 * Input       : NA;
 * Output      : NA;
 * ReturnValue : NA;
 * Other       : NA;
 **************************************************************************
*/
static int S5K5CAGA_read_reg(unsigned int reg, unsigned short* val)
{
    struct i2c_msg msg[5];
    int ret;
    unsigned char  data0[4],data1[4],data2[4],data3[2];
    unsigned short valtemp;

    data0[0] = 0xfc;
    data0[1] = 0xfc;
    data0[2] = 0xd0;
    data0[3] = 0x00;
    msg[0].addr = s5k5_client->addr;
    msg[0].len = 4;
    msg[0].buf = data0;
    msg[0].flags = 0;

    data1[0] = 0x00;
    data1[1] = 0x2c;
    data1[2] = (unsigned char)((reg&0xff000000)>>24);
    data1[3] = (unsigned char)((reg&0x00ff0000)>>16);
    msg[1].addr = s5k5_client->addr;
    msg[1].len = 4;
    msg[1].buf = data1;
    msg[1].flags = 0;

    data2[0] = 0x00;
    data2[1] = 0x2e;
    data2[2] = (unsigned char)((reg&0x0000ff00)>>8);
    data2[3] = (unsigned char)((reg&0x000000ff));
    msg[2].addr = s5k5_client->addr;
    msg[2].len = 4;
    msg[2].buf = data2;
    msg[2].flags = 0;

    data3[0] = 0x0f;
    data3[1] = 0x12;
    msg[3].addr = s5k5_client->addr;
    msg[3].len = 2;
    msg[3].buf = data3;
    msg[3].flags = 0;


    msg[4].addr = s5k5_client->addr;
    msg[4].len = 2;
    msg[4].buf = (unsigned char *)&valtemp;
    msg[4].flags = I2C_M_RD;

	CDBG_DW("dw S5K5CAGA_read_reg addr:%x",s5k5_client->addr);
    if (s5k5_client->adapter == NULL) {
        CDBG_DW("%s:adapter error", __FUNCTION__);
        return -1;
    }

    ret = i2c_transfer(s5k5_client->adapter, msg, 5);
    if (ret < 0) {
        CDBG_DW("S5K5CAGA read reg error(%d) reg=%x",ret ,reg);
        return ret;
    }

    *val= (((valtemp)<<8)&0xff00)|(((valtemp)>>8)&0x00ff);

    return 0;
}
static int S5K5CAGA_write_reg_ex(unsigned short reg, unsigned short val)
{

    struct i2c_msg msg[1];
    int ret;
    unsigned char  regl, regh;
    unsigned char  data[4];

    regl = (unsigned char)(reg&0xff);
    regh = (unsigned char)((reg&0xff00)>>0x08);


    data[0] = regh;
    data[1] = regl;

    regl = (unsigned char)(val&0xff);
    regh = (unsigned char)((val&0xff00)>>0x08);

    data[2] = regh;
    data[3] = regl;

    msg[0].addr = s5k5_client->addr;
    msg[0].len = 4;
    msg[0].buf = data;
    msg[0].flags = 0;

   //print_debug("reg=0x%x value=0x%x data[2]=0x%x data[3]=0x%x!!!!!",reg,val,data[2],data[3]);


   if (s5k5_client->adapter == NULL) {
       CDBG_DW("%s:adapter error",__func__);
       return -1;
   }

   ret = i2c_transfer(s5k5_client->adapter, msg, 1);
   if(ret < 0){
       CDBG_DW("S5K5CAGA write reg error:reg=%x,val=%x", reg, val);
       return ret;
   }
   return 0;
}



/*
static int32_t s5k5_set_default_focus(uint8_t af_step)
{
	int32_t rc = 0;
	return rc;
}
*/
static void s5k5_sw_reset(void)
{
    CDBG_DW("%s\n",__func__);
   	S5K5CAGA_write_reg_ex(0xFCFC, 0xD000);
}

static int32_t s5k5_power_down(void)
{
	int rc = 0;
	CDBG_DW("%s\n",__func__);
	//gpio_set_value(s5k5_pwdn_gpio, 0);
//	rc = gpio_request(s5k5_pwdn_gpio, "s5k5");
    if (rc < 0) {
        CDBG_DW("%s: gpio_request---93 failed!", __func__);

    }
	//gpio_request(93, "s5k5");
	gpio_direction_output(s5k5_pwdn_gpio, 0);	
//	gpio_free(s5k5_pwdn_gpio);
	return rc;
}
static int32_t s5k5_power_on(void)
{
	int rc = 0;
	CDBG_DW("%s\n",__func__);
//	rc = gpio_request(s5k5_pwdn_gpio, "s5k5");
    if (rc < 0) {
        CDBG_DW("%s: gpio_request---93 failed!", __func__);

    }
	//gpio_request(93, "s5k5");
	gpio_direction_output(s5k5_pwdn_gpio, 1);
//	gpio_free(s5k5_pwdn_gpio);
	return rc;
}


static void s5k5_hw_reset(void)
{
	int rc = 0;
    CDBG_DW("--CAMERA-- %s ... (Start...)\n",__func__);
//	rc = gpio_request(s5k5_reset_gpio, "s5k5");
    if (rc < 0) {
        CDBG_DW("%s: gpio_request---reset failed!", __func__);

    }	
    gpio_direction_output(s5k5_reset_gpio, 1);   //reset camera reset pin
    mdelay(5);
    gpio_direction_output(s5k5_reset_gpio, 0);
    mdelay(5);
    gpio_direction_output(s5k5_reset_gpio, 1);
    mdelay(1);
//	gpio_free(s5k5_reset_gpio);
    CDBG_DW("--CAMERA-- %s ... (End...)\n",__func__);
}

static int32_t s5k5_sensor_setting(int update_type, int rt)
{

	int32_t rc = 0;
	int length = 0;
	int i;
//	unsigned short temp;
	struct S5K5CAGA_para *reg_settings = NULL;
	struct msm_camera_csi_params s5k5_csi_params;
	

	
	if (update_type == REG_INIT) {
		CDBG_DW("dw s5k5_sensor_setting REG_INIT");
		s5k5_sw_reset();
		CSI_CONFIG = 0;
	} 
	else if (update_type == UPDATE_PERIODIC) 
	{		
		if (rt == RES_PREVIEW)
		{	
			if(!CSI_CONFIG)
			{
			
				CDBG_DW("dw s5k5_sensor_setting CSI_CONFIG");
				msm_camio_vfe_clk_rate_set(192000000);
				s5k5_csi_params.data_format = CSI_8BIT;
				s5k5_csi_params.lane_cnt = 1;
				s5k5_csi_params.lane_assign = 0xe4;
				s5k5_csi_params.dpcm_scheme = 0;
				s5k5_csi_params.settle_cnt = 0x06;
				rc = msm_camio_csi_config(&s5k5_csi_params);
				msleep(20);
				CDBG_DW("dw s5k5_sensor_setting UPDATE_PERIODIC RES_PREVIEW INIT");
				CSI_CONFIG = 1;
				S5K5CAGA_write_reg_ex(0xFCFC, 0xD000);
				S5K5CAGA_write_reg_ex(0xFCFC, 0xD000);
				S5K5CAGA_write_reg_ex(0x0010, 0x0001);
				S5K5CAGA_write_reg_ex(0x1030, 0x0000);
				S5K5CAGA_write_reg_ex(0x0014, 0x0001);

				msleep(100);
				length = (sizeof(S5K5CAGA_init_regs) / sizeof(S5K5CAGA_init_regs[0]));	
				reg_settings = S5K5CAGA_init_regs;
				if (NULL != reg_settings)
				{
					for(i=0; i < length; i++)
					{
						 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

					}
				    rc =0;
				}
				else
				{
				    return -1;
				}
				msleep(100);
				
				length = (sizeof(S5K5CAGA_init_regs_a) / sizeof(S5K5CAGA_init_regs_a[0]));
				reg_settings = S5K5CAGA_init_regs_a;
				if (NULL != reg_settings)
				{
					for(i=0; i < length; i++)
					{
						 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

					}
				    rc =0;
				}
				else
				{
				    return -1;
				}
				msleep(100);
				length = (sizeof(S5K5CAGA_init_regs_b) / sizeof(S5K5CAGA_init_regs_b[0]));
				reg_settings = S5K5CAGA_init_regs_b;
				if (NULL != reg_settings)
				{
					for(i=0; i < length; i++)
					{
						 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

					}

				    rc =0;						
				}
				else
				{
				    return -1;
				}
			}
			else
			{
				CDBG_DW("dw s5k5_sensor_setting UPDATE_PERIODIC RES_PREVIEW PREVIEW");
				length = (sizeof(S5K5CAGA_preview_regs) / sizeof(S5K5CAGA_preview_regs[0]));
				reg_settings = S5K5CAGA_preview_regs;
				if (NULL != reg_settings)
			    {
					for(i=0; i < length; i++)
					{
						 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

					}
			        rc =0;
			    }
			    else
			    {
			        return -1;
			    }
			}			
		}
		else
		{
			CDBG_DW("dw s5k5_sensor_setting UPDATE_PERIODIC NOT_RES_PREVIEW ");
			length = (sizeof(S5K5CAGA_capture_regs) / sizeof(S5K5CAGA_capture_regs[0]));
			reg_settings = S5K5CAGA_capture_regs;
			if (NULL != reg_settings)
		    {
				for(i=0; i < length; i++)
				{
					 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

				}
		        rc =0;
		    }
		    else
		    {
		        return -1;
		    }
		}
		msleep(20);		
	}

	return rc;
}

static int32_t s5k5_video_config(int mode)
{

	int32_t rc = 0;
	//int rt;
	
	CDBG_DW("video config\n");
	s5k5_ctrl->prev_res = QTR_SIZE;
	/*
	// change sensor resolution if needed 
	if (s5k5_ctrl->prev_res == QTR_SIZE)
		rt = RES_PREVIEW;
	else
		rt = RES_CAPTURE;
	*/
	if (s5k5_sensor_setting(UPDATE_PERIODIC, RES_PREVIEW) < 0)
		return rc;
	s5k5_ctrl->curr_res = s5k5_ctrl->pict_res;
	s5k5_ctrl->sensormode = mode;
	return rc;
}

static int32_t s5k5_snapshot_config(int mode)
{
	int32_t rc = 0;
	//int rt;
	s5k5_ctrl->pict_res = FULL_SIZE;
	/*
	//change sensor resolution if needed 
	if (s5k5_ctrl->curr_res != s5k5_ctrl->pict_res) {
		if (s5k5_ctrl->pict_res == QTR_SIZE)
			rt = RES_PREVIEW;
		else
			rt = RES_CAPTURE;
		if (s5k5_sensor_setting(UPDATE_PERIODIC, rt) < 0)
			return rc;
	}
	*/
	if (s5k5_sensor_setting(UPDATE_PERIODIC, RES_CAPTURE) < 0)
		return rc;	
	s5k5_ctrl->curr_res = s5k5_ctrl->pict_res;
	s5k5_ctrl->sensormode = mode;
	return rc;
}


static int32_t s5k5_set_sensor_mode(int mode,
		int res)
{
	int32_t rc = 0;

	switch (mode) {
	case SENSOR_PREVIEW_MODE:
		CDBG_DW("dw s5k5_set_sensor_mode preview");
		rc = s5k5_video_config(mode);
		break;
	case SENSOR_SNAPSHOT_MODE:
		CDBG_DW("dw s5k5_set_sensor_mode capture");
		rc = s5k5_snapshot_config(mode);
		break;

	default:
		rc = -EINVAL;
		break;
	}
	return rc;
}



static int s5k5_probe_init_sensor(const struct msm_camera_sensor_info *data)
{
	int32_t rc = 0;
	uint16_t chipid = 0;
	CDBG_DW("%s: %d\n", __func__, __LINE__);

    s5k5_hw_reset();

	S5K5CAGA_READ_REG(0x00000040, &chipid);

	CDBG_DW("CHIP ID: %x\n", chipid);
	printk("%s ,CHIP ID: %x\n",__func__,chipid);
	
	if (chipid != 0x05ca) 
	{
		rc = -ENODEV;
		goto init_probe_fail;
	}
	return rc;

init_probe_fail:
	CDBG_DW(" s5k5_probe_init_sensor fails\n");
	return rc;
}


int s5k5_sensor_open_init(const struct msm_camera_sensor_info *data)
{
	int32_t rc = 0;

	CDBG_DW("%s: %d\n", __func__, __LINE__);

	data->pdata->camera_vreg_config(1,1800,1);
	
	s5k5_ctrl = kzalloc(sizeof(struct s5k5_ctrl_t), GFP_KERNEL);
	if (!s5k5_ctrl) {
		CDBG_DW("s5k5_init failed!\n");
		rc = -ENOMEM;
		goto init_done;
	}
	s5k5_ctrl->fps_divider = 1 * 0x00000400;
	s5k5_ctrl->pict_fps_divider = 1 * 0x00000400;
	s5k5_ctrl->set_test = TEST_OFF;
	s5k5_ctrl->prev_res = QTR_SIZE;
	s5k5_ctrl->pict_res = FULL_SIZE;
	s5k5_ctrl->fps = 30 * Q8;
	s5k5_ctrl->curr_res = INVALID_SIZE;
	if (data)
		s5k5_ctrl->sensordata = data;

	/* enable mclk first */
	msm_camio_clk_rate_set(S5K5_MASTER_CLK_RATE);
	usleep(20);
	s5k5_power_on();
	rc = s5k5_probe_init_sensor(data);
	if (rc < 0)
		goto init_fail;

	CDBG_DW("init settings\n");
	rc = s5k5_sensor_setting(REG_INIT, RES_PREVIEW);	
    if (rc < 0)
        goto init_fail;
    else
        goto init_done;

init_fail:
	CDBG_DW("init_fail\n");
	kfree(s5k5_ctrl);
init_done:
	CDBG_DW("init_done\n");
	return rc;
}

static int s5k5_init_client(struct i2c_client *client)
{
	/* Initialize the MSM_CAMI2C Chip */
	init_waitqueue_head(&s5k5_wait_queue);
	return 0;
}


static const struct i2c_device_id s5k5_i2c_id[] = {
	{"s5k5", 0},
	{ }
};

static int s5k5_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int rc = 0;
	CDBG_DW("s5k5_probe called!\n");

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		CDBG_DW("i2c_check_functionality failed\n");
		goto probe_failure;
	}

	s5k5_sensorw = kzalloc(sizeof(struct s5k5_work_t), GFP_KERNEL);
	if (!s5k5_sensorw) {
		CDBG_DW("kzalloc failed.\n");
		rc = -ENOMEM;
		goto probe_failure;
	}

	i2c_set_clientdata(client, s5k5_sensorw);
	s5k5_init_client(client);
	s5k5_client = client;

	msleep(50);

	CDBG_DW("s5k5_probe successed! rc = %d\n", rc);
	return 0;

probe_failure:
	CDBG_DW("s5k5_probe failed! rc = %d\n", rc);
	return rc;
}

static int __devexit s5k5_remove(struct i2c_client *client)
{
	struct s5k5_work_t *sensorw = i2c_get_clientdata(client);
	free_irq(client->irq, sensorw);
	s5k5_client = NULL;
	kfree(sensorw);
	return 0;
}

static struct i2c_driver s5k5_i2c_driver = {
	.id_table = s5k5_i2c_id,
	.probe  = s5k5_i2c_probe,
	.remove = __exit_p(s5k5_i2c_remove),
	.driver = {
		.name = "s5k5",
	},
};

static long s5k5_set_effect(int mode, int effect)
{	
	long rc = 0;
	int length = 0;
	int i;
	struct S5K5CAGA_para *reg_settings = NULL;
	printk("enter s5k5_set_effect effect: %d \n", effect);					
	switch (effect) {
	case CAMERA_EFFECT_OFF: {
		length = (sizeof(s5k5caga_effect_normal_tbl) / sizeof(s5k5caga_effect_normal_tbl[0]));	
		reg_settings = s5k5caga_effect_normal_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	case CAMERA_EFFECT_MONO:{
		length = (sizeof(s5k5caga_effect_mono_tbl) / sizeof(s5k5caga_effect_mono_tbl[0]));	
		reg_settings = s5k5caga_effect_mono_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	
	case CAMERA_EFFECT_NEGATIVE: {
		length = (sizeof(s5k5caga_effect_negative_tbl) / sizeof(s5k5caga_effect_negative_tbl[0]));	
		reg_settings = s5k5caga_effect_negative_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_EFFECT_SOLARIZE: {
		length = (sizeof(s5k5caga_effect_sketch_tbl) / sizeof(s5k5caga_effect_sketch_tbl[0]));	
		reg_settings = s5k5caga_effect_sketch_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_EFFECT_SEPIA: {
		length = (sizeof(s5k5caga_effect_sepia_tbl) / sizeof(s5k5caga_effect_sepia_tbl[0]));	
		reg_settings = s5k5caga_effect_sepia_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}		
		break;

	default: {
		length = (sizeof(s5k5caga_effect_normal_tbl) / sizeof(s5k5caga_effect_normal_tbl[0]));	
		reg_settings = s5k5caga_effect_normal_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}
	}
	return rc;
}

static int s5k5_set_brightness(int8_t brightness)
{
	long rc = 0;
	int length = 0;
	int i;
	struct S5K5CAGA_para *reg_settings = NULL;
	printk("enter s5k5_set_brightness brightness: %d \n", brightness);				
	switch (brightness) {
	case CAMERA_BRIGHTNESS_LV0: {
		length = (sizeof(s5k5caga_brightness_lv2_tbl) / sizeof(s5k5caga_brightness_lv2_tbl[0]));	
		reg_settings = s5k5caga_brightness_lv2_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	case CAMERA_BRIGHTNESS_LV1:{
		length = (sizeof(s5k5caga_brightness_lv3_tbl) / sizeof(s5k5caga_brightness_lv3_tbl[0]));	
		reg_settings = s5k5caga_brightness_lv3_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	
	case CAMERA_BRIGHTNESS_LV2: {
		length = (sizeof(s5k5caga_brightness_default_lv4_tbl) / sizeof(s5k5caga_brightness_default_lv4_tbl[0]));	
		reg_settings = s5k5caga_brightness_default_lv4_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_BRIGHTNESS_LV3: {
		length = (sizeof(s5k5caga_brightness_lv5_tbl) / sizeof(s5k5caga_brightness_lv5_tbl[0]));	
		reg_settings = s5k5caga_brightness_lv5_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_BRIGHTNESS_LV4: {
		length = (sizeof(s5k5caga_brightness_lv6_tbl) / sizeof(s5k5caga_brightness_lv6_tbl[0]));	
		reg_settings = s5k5caga_brightness_lv6_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}		
		break;

	default: {
		length = (sizeof(s5k5caga_brightness_default_lv4_tbl) / sizeof(s5k5caga_brightness_default_lv4_tbl[0]));	
		reg_settings = s5k5caga_brightness_default_lv4_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}
	}	
	return rc;
}

static int s5k5_set_contrast(int contrast)
{
	long rc = 0;
	int length = 0;
	int i;
	struct S5K5CAGA_para *reg_settings = NULL;
	printk("enter s5k5_set_contrast contrast: %d \n", contrast);			
	switch (contrast/2) {
	case CAMERA_CONTRAST_LV0: {
		length = (sizeof(s5k5caga_contrast_lv2_tbl) / sizeof(s5k5caga_contrast_lv2_tbl[0]));	
		reg_settings = s5k5caga_contrast_lv2_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	case CAMERA_CONTRAST_LV1:{
		length = (sizeof(s5k5caga_contrast_lv3_tbl) / sizeof(s5k5caga_contrast_lv3_tbl[0]));	
		reg_settings = s5k5caga_contrast_lv3_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	
	case CAMERA_CONTRAST_LV2: {
		length = (sizeof(s5k5caga_contrast_default_lv4_tbl) / sizeof(s5k5caga_contrast_default_lv4_tbl[0]));	
		reg_settings = s5k5caga_contrast_default_lv4_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_CONTRAST_LV3: {
		length = (sizeof(s5k5caga_contrast_lv5_tbl) / sizeof(s5k5caga_contrast_lv5_tbl[0]));	
		reg_settings = s5k5caga_contrast_lv5_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_CONTRAST_LV4: {
		length = (sizeof(s5k5caga_contrast_lv6_tbl) / sizeof(s5k5caga_contrast_lv6_tbl[0]));	
		reg_settings = s5k5caga_contrast_lv6_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}		
		break;

	default: {
		length = (sizeof(s5k5caga_contrast_default_lv4_tbl) / sizeof(s5k5caga_contrast_default_lv4_tbl[0]));	
		reg_settings = s5k5caga_contrast_default_lv4_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}
	}	
	return rc;
}

static int s5k5_set_saturation(int saturation)
{
	long rc = 0;
	int length = 0;
	int i;
	struct S5K5CAGA_para *reg_settings = NULL;	
	printk("enter s5k5_set_saturation saturation: %d \n", saturation);		
	switch (saturation/2) {
	case CAMERA_SATURATION_LV0: {
		length = (sizeof(s5k5caga_saturation_lv2_tbl) / sizeof(s5k5caga_saturation_lv2_tbl[0]));	
		reg_settings = s5k5caga_saturation_lv2_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	case CAMERA_SATURATION_LV1:{
		length = (sizeof(s5k5caga_saturation_lv3_tbl) / sizeof(s5k5caga_saturation_lv3_tbl[0]));	
		reg_settings = s5k5caga_saturation_lv3_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	
	case CAMERA_SATURATION_LV2: {
		length = (sizeof(s5k5caga_saturation_default_lv4_tbl) / sizeof(s5k5caga_saturation_default_lv4_tbl[0]));	
		reg_settings = s5k5caga_saturation_default_lv4_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_SATURATION_LV3: {
		length = (sizeof(s5k5caga_saturation_lv5_tbl) / sizeof(s5k5caga_saturation_lv5_tbl[0]));	
		reg_settings = s5k5caga_saturation_lv5_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_SATURATION_LV4: {
		length = (sizeof(s5k5caga_saturation_lv6_tbl) / sizeof(s5k5caga_saturation_lv6_tbl[0]));	
		reg_settings = s5k5caga_saturation_lv6_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}		
		break;

	default: {
		length = (sizeof(s5k5caga_saturation_default_lv4_tbl) / sizeof(s5k5caga_saturation_default_lv4_tbl[0]));	
		reg_settings = s5k5caga_saturation_default_lv4_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}
	}	
	return rc;
}


static int s5k5_set_sharpness(int sharpness)
{
	long rc = 0;
	int length = 0;
	int i;
	struct S5K5CAGA_para *reg_settings = NULL;
	printk("enter s5k5_set_sharpness sharpness: %d \n", sharpness);	
	switch (sharpness/6) {
	case CAMERA_SHARPNESS_LV0: {
		length = (sizeof(s5k5caga_sharpness_lv2_tbl) / sizeof(s5k5caga_sharpness_lv2_tbl[0]));	
		reg_settings = s5k5caga_sharpness_lv2_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	case CAMERA_SHARPNESS_LV1:{
		length = (sizeof(s5k5caga_sharpness_lv3_tbl) / sizeof(s5k5caga_sharpness_lv3_tbl[0]));	
		reg_settings = s5k5caga_sharpness_lv3_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	
	case CAMERA_SHARPNESS_LV2: {
		length = (sizeof(s5k5caga_sharpness_default_lv4_tbl) / sizeof(s5k5caga_sharpness_default_lv4_tbl[0]));	
		reg_settings = s5k5caga_sharpness_default_lv4_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_SHARPNESS_LV3: {
		length = (sizeof(s5k5caga_sharpness_lv5_tbl) / sizeof(s5k5caga_sharpness_lv5_tbl[0]));	
		reg_settings = s5k5caga_sharpness_lv5_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_SHARPNESS_LV4: {
		length = (sizeof(s5k5caga_sharpness_lv6_tbl) / sizeof(s5k5caga_sharpness_lv6_tbl[0]));	
		reg_settings = s5k5caga_sharpness_lv6_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}		
		break;

	default: {
		length = (sizeof(s5k5caga_sharpness_default_lv4_tbl) / sizeof(s5k5caga_sharpness_default_lv4_tbl[0]));	
		reg_settings = s5k5caga_sharpness_default_lv4_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}
	}	
	return rc;
}


static long s5k5_set_antibanding(int antibanding)
{
       long rc = 0;
	int length = 0;
	int i;
	struct S5K5CAGA_para *reg_settings = NULL;
	printk("enter s5k5_set_antibanding antibanding: %d \n", antibanding);
       switch (antibanding)
       {
       case CAMERA_ANTIBANDING_OFF:

           break;
   
       case CAMERA_ANTIBANDING_60HZ:{
		length = (sizeof(s5k5caga_antibanding_60z_tbl) / sizeof(s5k5caga_antibanding_60z_tbl[0]));	
		reg_settings = s5k5caga_antibanding_60z_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
           break;
   
       case CAMERA_ANTIBANDING_50HZ:{
		length = (sizeof(s5k5caga_antibanding_50z_tbl) / sizeof(s5k5caga_antibanding_50z_tbl[0]));	
		reg_settings = s5k5caga_antibanding_50z_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
           break;
   
       case CAMERA_ANTIBANDING_AUTO:{
		length = (sizeof(s5k5caga_antibanding_auto_tbl) / sizeof(s5k5caga_antibanding_auto_tbl[0]));	
		reg_settings = s5k5caga_antibanding_auto_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
           break;
   
       default:{
		length = (sizeof(s5k5caga_antibanding_auto_tbl) / sizeof(s5k5caga_antibanding_auto_tbl[0]));	
		reg_settings = s5k5caga_antibanding_auto_tbl;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
           break;
       }	
       return rc;
}

static int s5k5_set_wb_oem(uint8_t param)
{
       int rc = 0;
	int length = 0;
	int i;
	struct S5K5CAGA_para *reg_settings = NULL;
	printk("enter s5k5_set_wb_oem param: %d \n", param);
	switch (param) {
	case CAMERA_WB_AUTO: {
		length = (sizeof(s5k5caga_wb_auto) / sizeof(s5k5caga_wb_auto[0]));	
		reg_settings = s5k5caga_wb_auto;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	case CAMERA_WB_FLUORESCENT:{
		length = (sizeof(s5k5caga_wb_fluorescent) / sizeof(s5k5caga_wb_fluorescent[0]));	
		reg_settings = s5k5caga_wb_fluorescent;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;
	
	case CAMERA_WB_INCANDESCENT: {
		length = (sizeof(s5k5caga_wb_incandescent) / sizeof(s5k5caga_wb_incandescent[0]));	
		reg_settings = s5k5caga_wb_incandescent;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_WB_DAYLIGHT: {
		length = (sizeof(s5k5caga_wb_daylight) / sizeof(s5k5caga_wb_daylight[0]));	
		reg_settings = s5k5caga_wb_daylight;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}	
		break;

	case CAMERA_WB_CLOUDY_DAYLIGHT: {
		length = (sizeof(s5k5caga_wb_cloudy) / sizeof(s5k5caga_wb_cloudy[0]));	
		reg_settings = s5k5caga_wb_cloudy;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}		
		break;

	default: {
		length = (sizeof(s5k5caga_wb_auto) / sizeof(s5k5caga_wb_auto[0]));	
		reg_settings = s5k5caga_wb_auto;
		if (NULL != reg_settings)
		{
			for(i=0; i < length; i++)
			{
				 S5K5CAGA_WRITE_REG_EX(reg_settings[i].subaddr, reg_settings[i].value);

			}
		    rc =0;
		}
		else
		{
		    return -1;
		}
	}
	}	
	return rc;
}

static long s5k5_set_exposure_mode(int mode)
{
    long rc = 0;
    //int i =0;
    CDBG("--CAMERA-- %s ...(Start)\n",__func__);
    CDBG("--CAMERA-- %s ...mode = %d\n",__func__ , mode);

    return rc;
}

int s5k5_sensor_config(void __user *argp)
{
	struct sensor_cfg_data cdata;
	long   rc = 0;
	mutex_lock(&s5k5_mut);
	if (copy_from_user(&cdata,
				(void *)argp,
				sizeof(struct sensor_cfg_data)))
		return -EFAULT;
	

		CDBG_DW("s5k5_sensor_config: cfgtype = %d\n",
				cdata.cfgtype);

	switch (cdata.cfgtype) {
		case CFG_SET_MODE:
		    rc =s5k5_set_sensor_mode(cdata.mode,
		        cdata.rs);
		    break;
		case CFG_PWR_DOWN:
		    s5k5_power_down();
		    break;

	       case CFG_SET_EFFECT:
	           CDBG("--CAMERA-- CFG_SET_EFFECT mode=%d, effect = %d !!\n",cdata.mode, cdata.cfg.effect);
	           rc = s5k5_set_effect(cdata.mode, cdata.cfg.effect);
	          break;	
              case CFG_SET_BRIGHTNESS:
	           CDBG("--CAMERA-- CFG_SET_BRIGHTNESS  !!\n");
	           rc = s5k5_set_brightness(cdata.cfg.brightness);
	           break;
	       case CFG_SET_CONTRAST:
	           CDBG("--CAMERA-- CFG_SET_CONTRAST  !!\n");
	           rc = s5k5_set_contrast(cdata.cfg.contrast);
	           break;   				
             case CFG_SET_SATURATION:
                 CDBG("--CAMERA-- CFG_SET_SATURATION !!\n");
                 rc = s5k5_set_saturation(cdata.cfg.saturation);
                 break;
             case CFG_SET_SHARPNESS:
                 CDBG("--CAMERA-- CFG_SET_SHARPNESS !!\n");
                 rc = s5k5_set_sharpness(cdata.cfg.sharpness);
                 break;	
             case CFG_SET_ANTIBANDING:
                 CDBG("--CAMERA-- CFG_SET_ANTIBANDING antibanding = %d!!\n", cdata.cfg.antibanding);
                 rc = s5k5_set_antibanding(cdata.cfg.antibanding);
                 break;
             case CFG_SET_WB:
                 CDBG("--CAMERA-- CFG_SET_WB!!\n");
                 rc = s5k5_set_wb_oem(cdata.cfg.wb_val);
                 break;	
             case CFG_SET_EXPOSURE_MODE:
                 CDBG("--CAMERA-- CFG_SET_EXPOSURE_MODE !!\n");
                 rc = s5k5_set_exposure_mode(cdata.cfg.ae_mode);
                 break;				 
             case CFG_SET_EXPOSURE_COMPENSATION:
                 CDBG("--CAMERA-- CFG_SET_EXPOSURE_MODE !!\n");
                 //rc = s5k5_set_exposure_mode(cdata.cfg.exp_compensation);
                 break;				 
		default:
		    rc = -EFAULT;
		    break;
	}

	
	mutex_unlock(&s5k5_mut);

	return rc;
}

static int s5k5_sensor_release(void)
{
    int rc = -EBADF;
    mutex_lock(&s5k5_mut);
    s5k5_sw_reset();	
    s5k5_power_down();
	mdelay(250);
    kfree(s5k5_ctrl);
    s5k5_ctrl = NULL;
    CDBG_DW("s5k5_release completed\n");
    mutex_unlock(&s5k5_mut);
	
    return rc;
}

static int s5k5_probe_init_gpio(const struct msm_camera_sensor_info *data)
{
    int rc = 0;

    s5k5_pwdn_gpio = data->sensor_pwd;
    s5k5_reset_gpio = data->sensor_reset ;
    
    if(data->sensor_reset_enable)
    {
//		rc = gpio_request(s5k5_reset_gpio, "s5k5");
	    if (rc < 0) {
	        CDBG_DW("%s: gpio_request---reset failed!", __func__);

	    }
        gpio_direction_output(s5k5_reset_gpio, 1);
//		gpio_free(s5k5_reset_gpio);
    }
	
//	rc = gpio_request(s5k5_pwdn_gpio, "s5k5");
    if (rc < 0) {
        CDBG_DW("%s: gpio_request---93 failed!", __func__);

    }
	//gpio_request(93, "s5k5");
	gpio_direction_output(s5k5_pwdn_gpio, 1);
//	gpio_free(s5k5_pwdn_gpio);
    return rc;

}

#if 1 // OTP PART
static const char *s5k5Module = "05CA";
static const char *s5k5ModuleId;

static ssize_t sensor_Module_id_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;

	s5k5ModuleId = "Truly";
	
	sprintf(buf, "%s %s \n", s5k5ModuleId, s5k5Module);
	ret = strlen(buf) + 1;

	return ret;
}

static DEVICE_ATTR(sensor, 0444, sensor_Module_id_show, NULL);

static struct kobject *android_s5k5;

static int s5k5_sysfs_init(void)
{
	int ret ;

	android_s5k5 = kobject_create_and_add("android_camera", NULL);
	if (android_s5k5 == NULL) {
		pr_info("s5k5_sysfs_init: subsystem_register " \
		"failed\n");
		ret = -ENOMEM;
		return ret ;
	}
	ret = sysfs_create_file(android_s5k5, &dev_attr_sensor.attr);
	if (ret) {
		pr_info("s5k5_sysfs_init: sysfs_create_file " \
		"failed\n");
		kobject_del(android_s5k5);
	}
	return 0 ;
}

#endif


static int s5k5_sensor_probe(const struct msm_camera_sensor_info *info,
		struct msm_sensor_ctrl *s)
{
	int rc = 0;

	info->pdata->camera_vreg_config(1,1800,1);

	rc = i2c_add_driver(&s5k5_i2c_driver);
	if (rc < 0 || s5k5_client == NULL) {
		rc = -ENOTSUPP;
		CDBG_DW("I2C add driver failed 1");
		goto probe_fail;
	}
 	rc = s5k5_probe_init_gpio(info);
    if (rc < 0) {
        CDBG_DW("%s: gpio init failed\n", __func__);
        goto probe_fail;
    }

	msm_camio_clk_rate_set(S5K5_MASTER_CLK_RATE);
    s5k5_power_on();
	msleep(1);

	
	rc = s5k5_probe_init_sensor(info);
	if (rc < 0)
		goto probe_fail;

#if 1 // OTP PART
	s5k5_sysfs_init();
#endif

	s->s_init = s5k5_sensor_open_init;
	s->s_release = s5k5_sensor_release;
	s->s_config  = s5k5_sensor_config;
	s->s_mount_angle = info->sensor_platform_info->mount_angle;
	s5k5_power_down();
	return rc;



probe_fail:
	CDBG_DW("s5k5_sensor_probe: SENSOR PROBE FAILS!\n");
	i2c_del_driver(&s5k5_i2c_driver);
    	s5k5_power_down();
	info->pdata->camera_vreg_config(0,1200,1);
	return rc;
}

static int __devinit s5k5_probe(struct platform_device *pdev)
{
	return msm_camera_drv_start(pdev, s5k5_sensor_probe);
}

static struct platform_driver msm_camera_driver = {
	.probe = s5k5_probe,
	.driver = {
		.name = "msm_camera_s5k5",
		.owner = THIS_MODULE,
	},
};

static int __init s5k5_init(void)
{
	return platform_driver_register(&msm_camera_driver);
}

module_init(s5k5_init);
MODULE_DESCRIPTION("Samsung 5 MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");
