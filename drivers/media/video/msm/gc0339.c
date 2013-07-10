/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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
#include "gc0339.h"
/******************************************************************************
Debug Switch
******************************************************************************/
#ifdef CDBG
#undef CDBG
#endif
#ifdef CDBG_HIGH
#undef CDBG_HIGH
#endif

#define MT9P111_VERBOSE_DGB

#ifdef MT9P111_VERBOSE_DGB
#define CDBG(fmt, args...)        printk(fmt, ##args)
#define CDBG_HIGH(fmt, args...)   printk(fmt, ##args)
#else
#define CDBG(fmt, args...)        do { } while (0)
#define CDBG_HIGH(fmt, args...)   pr_debug(fmt, ##args)
#endif

/******************************************************************************
TYPE DECLARATIONS
******************************************************************************/
#define Q8    0x00000100
#define Q10    0x00000400
#define GC0339_MASTER_CLK_RATE             24000000

#define GC0339_I2C_MAX_TIMES 10
static u8 gc0339_i2c_buf[4] = {0};
static u8 gc0339_counter = 0;
static int gc0339_pwdn_gpio;
static int gc0339_reset_gpio;
static int gc0339_vcm_pwd_gpio;

struct gc0339_work_t {
    struct work_struct work;
};

static struct gc0339_work_t *gc0339_sensorw;
static struct i2c_client *gc0339_client;

struct gc0339_ctrl_t {
    const struct  msm_camera_sensor_info *sensordata;

    uint32_t sensormode;
    uint32_t fps_divider;        /* init to 1 * 0x00000400 */
    uint32_t pict_fps_divider;   /* init to 1 * 0x00000400 */
    uint16_t fps;

    uint16_t curr_lens_pos;
    uint16_t curr_step_pos;
    uint16_t my_reg_gain;
    uint32_t my_reg_line_count;
    uint16_t total_lines_per_frame;
    uint16_t config_csi;

    enum gc0339_resolution_t prev_res;
    enum gc0339_resolution_t pict_res;
    enum gc0339_resolution_t curr_res;
    enum gc0339_test_mode_t  set_test;
};

static bool GC0339_CSI_CONFIG;
static struct gc0339_ctrl_t *gc0339_ctrl;
static DECLARE_WAIT_QUEUE_HEAD(gc0339_wait_queue);
DEFINE_MUTEX(gc0339_mut);

static uint16_t prev_line_length_pck;
static uint16_t prev_frame_length_lines;
static uint16_t snap_line_length_pck;
static uint16_t snap_frame_length_lines;
extern int lcd_camera_power_onoff(int on);

/******************************************************************************

******************************************************************************/
static int gc0339_i2c_txdata(u16 saddr,u8 *txdata,int length);
static int gc0339_i2c_write_byte(unsigned int waddr,
                                  unsigned int wdata);
static int gc0339_i2c_rxdata(unsigned short saddr,
                              unsigned char *rxdata,
                              int length);
static int32_t gc0339_i2c_read_byte(unsigned int raddr,
                                     unsigned int *rdata);
static int32_t GC0339_WriteRegsTbl(PGC0339_WREG pTb,int32_t len);
static void gc0339_start_stream(void);
static void gc0339_stop_stream(void);
static void gc0339_get_pict_fps(uint16_t fps, uint16_t *pfps);
static uint16_t gc0339_get_prev_lines_pf(void);
static uint16_t gc0339_get_prev_pixels_pl(void);
static uint16_t gc0339_get_pict_lines_pf(void);
static uint16_t gc0339_get_pict_pixels_pl(void);
static uint32_t gc0339_get_pict_max_exp_lc(void);
static int32_t gc0339_set_fps(struct fps_cfg   *fps);
static int32_t gc0339_write_exp_gain(uint16_t gain, uint32_t line);
static int32_t gc0339_set_pict_exp_gain(uint16_t gain, uint32_t line);
static int32_t gc0339_test(enum gc0339_test_mode_t mo);
static int32_t gc0339_sensor_setting(int update_type, int rt);
static int32_t gc0339_video_config(int mode);
static int32_t gc0339_snapshot_config(int mode);
static int32_t gc0339_raw_snapshot_config(int mode);
static int32_t gc0339_set_sensor_mode(int mode,int res);
static int gc0339_init_client(struct i2c_client *client);
static int gc0339_i2c_probe(struct i2c_client *client,
        const struct i2c_device_id *id);
static int __devexit gc0339_remove(struct i2c_client *client);
static void gc0339_power_on(void);
static void gc0339_power_off(void);
static void gc0339_power_reset(void);
static int gc0339_read_model_id(const struct msm_camera_sensor_info *data);
static int gc0339_probe_init_gpio(const struct msm_camera_sensor_info *data);
static void gc0339_probe_free_gpio(const struct msm_camera_sensor_info *data);
int gc0339_sensor_open_init(const struct msm_camera_sensor_info *data);
static int gc0339_sensor_release(void);
static int gc0339_sensor_probe(const struct msm_camera_sensor_info *info,
                                struct msm_sensor_ctrl *s);
int gc0339_sensor_config(void __user *argp);
static int __devinit gc0339_probe(struct platform_device *pdev);
static int __init gc0339_init(void);
/******************************************************************************

******************************************************************************/
static int gc0339_i2c_txdata(u16 saddr,u8 *txdata,int length)
{
  struct i2c_msg msg[] = {
    {
      .addr  = saddr,
      .flags = 0,
      .len = 2,
      .buf = txdata,
    },
  };
  if (i2c_transfer(gc0339_client->adapter, msg, 1) < 0)
    return -EIO;
  else
    return 0;
}

/******************************************************************************

******************************************************************************/
static int gc0339_i2c_write_byte(unsigned int waddr,
                                  unsigned int wdata)
{
  int rc = -EIO;
  gc0339_counter = 0;
  gc0339_i2c_buf[0] = (waddr & 0x00FF);
  gc0339_i2c_buf[1] = (wdata & 0x00FF);
  while ((gc0339_counter < GC0339_I2C_MAX_TIMES) &&(rc != 0))
  {
    rc = gc0339_i2c_txdata(gc0339_client->addr, gc0339_i2c_buf, 2);
    if (rc < 0)
    {
      gc0339_counter++;
      CDBG("gc0339_i2c_txdata failed,Reg(0x%04x) = 0x%02x, rc=%d!\n",
            waddr,wdata,rc);
    }
  }
  return rc;
}

/******************************************************************************

******************************************************************************/
static int gc0339_i2c_rxdata(unsigned short saddr,
                              unsigned char *rxdata,
                              int length)
{
  struct i2c_msg msgs[] = {
    {
      .addr   = saddr,
      .flags = 0,
      .len   = 1,
      .buf   = rxdata,
    },
    {
      .addr   = saddr,
      .flags = I2C_M_RD,
      .len   = 1,
      .buf   = rxdata,
    },
  };
  if (i2c_transfer(gc0339_client->adapter, msgs, 2) < 0) {
    CDBG("gc0339_i2c_rxdata failed!\n");
    return -EIO;
  }
  return 0;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_i2c_read_byte(unsigned int raddr,
                                     unsigned int *rdata)
{
  int32_t rc = 0;
  unsigned char buf[4] = {0};
  if (!rdata)
    return -EIO;
  buf[0] = (raddr & 0x00FF);
  rc = gc0339_i2c_rxdata(gc0339_client->addr, buf, 1);
  if (rc < 0) {
    CDBG("gc0339_i2c_read_word failed!\n");
    return rc;
  }
  *rdata = buf[0];
  return rc;
}

/******************************************************************************

******************************************************************************/
static int32_t GC0339_WriteRegsTbl(PGC0339_WREG pTb,int32_t len)
{
  int32_t i, ret = 0;
  for (i = 0;i < len; i++)
  {
    if (0 == pTb[i].data_format){
      /*Reg Data Format:16Bit = 0*/
      CDBG("GC0339_WriteRegsTbl: wrong format!\n");
    } else {
      /*Reg Data Format:8Bit = 1*/
      if(0 == pTb[i].delay_ms){
        gc0339_i2c_write_byte(pTb[i].addr,pTb[i].data);
      }else{
        gc0339_i2c_write_byte(pTb[i].addr,pTb[i].data);
        msleep(pTb[i].delay_ms);
      }
    }
  }
  return ret;
}

/******************************************************************************

******************************************************************************/
static void gc0339_start_stream(void)
{

}

/******************************************************************************

******************************************************************************/
static void gc0339_stop_stream(void)
{

}

/******************************************************************************

******************************************************************************/
static void gc0339_get_pict_fps(uint16_t fps, uint16_t *pfps)
{
  uint32_t divider, d1, d2;
  d1 = (prev_frame_length_lines * 0x00000400) / snap_frame_length_lines;
  d2 = (prev_line_length_pck * 0x00000400) / snap_line_length_pck;
  divider = (d1 * d2) / 0x400;
  CDBG("gc0339_get_pict_fps divider = %d", divider );
  *pfps = (uint16_t) (fps * divider / 0x400);
}

/******************************************************************************

******************************************************************************/
static uint16_t gc0339_get_prev_lines_pf(void)
{
  if (gc0339_ctrl->prev_res == QTR_SIZE)
    return prev_frame_length_lines;
  else
    return snap_frame_length_lines;
}

/******************************************************************************

******************************************************************************/
static uint16_t gc0339_get_prev_pixels_pl(void)
{
  if (gc0339_ctrl->prev_res == QTR_SIZE)
    return prev_line_length_pck;
  else
    return snap_line_length_pck;
}

/******************************************************************************

******************************************************************************/
static uint16_t gc0339_get_pict_lines_pf(void)
{
  if (gc0339_ctrl->pict_res == QTR_SIZE)
    return prev_frame_length_lines;
  else
    return snap_frame_length_lines;
}

/******************************************************************************

******************************************************************************/
static uint16_t gc0339_get_pict_pixels_pl(void)
{
  if (gc0339_ctrl->pict_res == QTR_SIZE)
    return prev_line_length_pck;
  else
    return snap_line_length_pck;
}

/******************************************************************************

******************************************************************************/
static uint32_t gc0339_get_pict_max_exp_lc(void)
{
  return snap_frame_length_lines * 24;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_set_fps(struct fps_cfg   *fps)
{
  return 0;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_write_exp_gain(uint16_t gain, uint32_t line)
{
  int rc = 0;
  unsigned int  intg_time_msb, intg_time_lsb;
  if(line>2047)
    line=2047;
  intg_time_msb = (unsigned int ) ((line & 0x0F00) >> 8);
  intg_time_lsb = (unsigned int ) (line& 0x00FF);
  if(gain>0xff)
   gain=0xff;
  rc = gc0339_i2c_write_byte(0x51,(gain));//0x50//petersun
  rc = gc0339_i2c_write_byte(0x03,intg_time_msb);
  rc = gc0339_i2c_write_byte(0x04,intg_time_lsb);
  return rc;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_set_pict_exp_gain(uint16_t gain, uint32_t line)
{
  int32_t rc = 0;
 // rc = gc0339_write_exp_gain(gain,line);//petersun del
 
  return rc;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_test(enum gc0339_test_mode_t mo)
{
    int32_t rc = 0;
    if (mo != TEST_OFF)
        ;//rc = gc0339_i2c_write_word(REG_TEST_PATTERN_MODE, (uint8_t) mo);
    return rc;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_sensor_setting(int update_type, int rt)
{
  int32_t rc = 0;
  struct msm_camera_csi_params gc0339_csi_params;
  CDBG("%s: line %d\n",__func__,__LINE__);
  if (update_type == S_REG_INIT) {
    GC0339_CSI_CONFIG = 0;
    rc = 0;
  } else if (update_type == S_UPDATE_PERIODIC) {
    gc0339_stop_stream();
    msleep(20);
    if (!GC0339_CSI_CONFIG) {
      msm_camio_vfe_clk_rate_set(192000000);
      gc0339_csi_params.data_format = CSI_8BIT;
      gc0339_csi_params.lane_cnt = 1;
      gc0339_csi_params.lane_assign = 0xe4;
      gc0339_csi_params.dpcm_scheme = 0;
      gc0339_csi_params.settle_cnt = 20;
      rc = msm_camio_csi_config(&gc0339_csi_params);
      rc = GC0339_WriteSettingTbl(gc0339_init_tbl);
      if (rc < 0) {
        CDBG("sensor init setting failed\n");
        return rc;
      }
      msleep(20);
      GC0339_CSI_CONFIG = 1;
    }
    if (rt == S_RES_PREVIEW){
      rc = GC0339_WriteSettingTbl(gc0339_preview_tbl_30fps);
      if (rc < 0) {
        CDBG("sensor preview setting failed\n");
        return rc;
      }
    }
    else{
      rc = GC0339_WriteSettingTbl(gc0339_capture_tbl);
      if (rc < 0) {
        CDBG("sensor capture setting failed\n");
        return rc;
      }
    }
    msleep(20);
    gc0339_start_stream();
    msleep(30);
  }
  return rc;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_video_config(int mode)
{
  int32_t rc = 0;
  int rt;
  CDBG("%s,Line: %d \n",__func__,__LINE__);
  if (gc0339_ctrl->prev_res == S_QTR_SIZE)
    rt = S_RES_PREVIEW;
  else
    rt = S_RES_CAPTURE;
  if (gc0339_sensor_setting(S_UPDATE_PERIODIC, rt) < 0)
    return rc;
  if (gc0339_ctrl->set_test) {
    if (gc0339_test(gc0339_ctrl->set_test) < 0)
      return  rc;
  }
  gc0339_ctrl->curr_res = gc0339_ctrl->prev_res;
  gc0339_ctrl->sensormode = mode;
  return rc;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_snapshot_config(int mode)
{
  int32_t rc = 0;
  int rt;
  CDBG("%s,Line: %d \n",__func__,__LINE__);
  if (gc0339_ctrl->curr_res != gc0339_ctrl->pict_res) {
    if (gc0339_ctrl->pict_res == S_QTR_SIZE)
      rt = S_RES_PREVIEW;
    else
      rt = S_RES_CAPTURE;
    if (gc0339_sensor_setting(S_UPDATE_PERIODIC, rt) < 0)
      return rc;
  }
  gc0339_ctrl->curr_res = gc0339_ctrl->pict_res;
  gc0339_ctrl->sensormode = mode;
  return rc;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_raw_snapshot_config(int mode)
{
  int32_t rc = 0;
  int rt;
  CDBG("%s,Line: %d \n",__func__,__LINE__);
  if (gc0339_ctrl->curr_res != gc0339_ctrl->pict_res) {
    if (gc0339_ctrl->pict_res == S_QTR_SIZE)
      rt = S_RES_PREVIEW;
    else
      rt = S_RES_CAPTURE;
    if (gc0339_sensor_setting(S_UPDATE_PERIODIC, rt) < 0)
      return rc;
  }
  gc0339_ctrl->curr_res = gc0339_ctrl->pict_res;
  gc0339_ctrl->sensormode = mode;
  return rc;
}

/******************************************************************************

******************************************************************************/
static int32_t gc0339_set_sensor_mode(int mode,int res)
{
  int32_t rc = 0;
  switch (mode) {
    case SENSOR_PREVIEW_MODE:
      rc = gc0339_video_config(mode);
      break;
    case SENSOR_SNAPSHOT_MODE:
      rc = gc0339_snapshot_config(mode);
      break;
    case SENSOR_RAW_SNAPSHOT_MODE:
      rc = gc0339_raw_snapshot_config(mode);
      break;
    default:
      rc = -EINVAL;
      break;
  }
  return rc;
}

/******************************************************************************

******************************************************************************/
static int gc0339_init_client(struct i2c_client *client)
{
  init_waitqueue_head(&gc0339_wait_queue);
  return 0;
}

/******************************************************************************

******************************************************************************/
static const struct i2c_device_id gc0339_i2c_id[] = {
  {"gc0339", 0},
  { }
};

/******************************************************************************

******************************************************************************/
static struct i2c_driver gc0339_i2c_driver = {
  .id_table = gc0339_i2c_id,
  .probe  = gc0339_i2c_probe,
  .remove = __exit_p(gc0339_i2c_remove),
  .driver = {
    .name = "gc0339",
  },
};

/******************************************************************************

******************************************************************************/
static int gc0339_i2c_probe(struct i2c_client *client,
        const struct i2c_device_id *id)
{
  int rc = 0;
  CDBG("%s,Line: %d \n",__func__,__LINE__);
  if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
    CDBG("i2c_check_functionality failed\n");
    return -ENOMEM;
  }
  gc0339_sensorw = kzalloc(sizeof(struct gc0339_work_t), GFP_KERNEL);
  if (!gc0339_sensorw) {
    CDBG("kzalloc failed.\n");
    rc = -ENOMEM;
  }
  i2c_set_clientdata(client, gc0339_sensorw);
  gc0339_init_client(client);
  gc0339_client = client;
  gc0339_client->addr = client->addr;
  CDBG("gc0339_client->addr = 0x%x\n",gc0339_client->addr);
  CDBG("%s,Line: %d \n",__func__,__LINE__);
  return 0;
}

/******************************************************************************

******************************************************************************/
static int __devexit gc0339_remove(struct i2c_client *client)
{
  struct gc0339_work_t *sensorw = i2c_get_clientdata(client);
  free_irq(client->irq, sensorw);
  gc0339_client = NULL;
  kfree(sensorw);
  return 0;
}

/******************************************************************************

******************************************************************************/
static void gc0339_power_on(void)
{
  CDBG("%s\n",__func__);
//  gpio_set_value(gc0339_pwdn_gpio, 0);
  gpio_direction_output(gc0339_pwdn_gpio, 0);
 
}

/******************************************************************************

******************************************************************************/
static void gc0339_power_off(void)
{
  CDBG("%s\n",__func__);
//  gpio_set_value(gc0339_pwdn_gpio, 1);
  gpio_direction_output(gc0339_pwdn_gpio, 1);  
}

/******************************************************************************

******************************************************************************/
static void gc0339_power_reset(void)
{
  CDBG("%s\n",__func__);
//  gpio_set_value(gc0339_reset_gpio, 1);
  gpio_direction_output(gc0339_reset_gpio, 1);  
  mdelay(1);
//  gpio_set_value(gc0339_reset_gpio, 0);
  gpio_direction_output(gc0339_reset_gpio, 0);  
  mdelay(10);
//  gpio_set_value(gc0339_reset_gpio, 1);
  gpio_direction_output(gc0339_reset_gpio, 1);  
  mdelay(1);
}

/******************************************************************************

******************************************************************************/
static int gc0339_read_model_id(const struct msm_camera_sensor_info *data)
{
  int rc = 0;
  uint model_id = 0;
  CDBG("%s\n",__func__);
  rc = gc0339_i2c_write_byte(0xfc,0x10);
  rc |= gc0339_i2c_read_byte(0x00, &model_id);
  if ((rc < 0)||(model_id != 0xc8)){
    CDBG("gc0339_read_model_id model_id(0x%02x) mismatch!\n",model_id);
    return -ENODEV;
  }
  CDBG("gc0339_read_model_id model_id(0x%02x) match success!\n",model_id);
  return rc;
}

/******************************************************************************

******************************************************************************/
static int gc0339_probe_init_gpio(const struct msm_camera_sensor_info *data)
{
  int rc = 0;
  CDBG("%s\n",__func__);
  gc0339_pwdn_gpio = data->sensor_pwd;
  gc0339_reset_gpio = data->sensor_reset;
  gc0339_vcm_pwd_gpio = data->vcm_pwd ;

#if 0
  if (data->sensor_reset_enable) {
    rc = gpio_request(data->sensor_reset, "gc0339");
    if (0 == rc) {
      rc = gpio_tlmm_config(GPIO_CFG(data->sensor_reset, 0,GPIO_CFG_OUTPUT,
                            GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
      if (rc < 0) {
        gpio_free(data->sensor_reset);
        CDBG("gc0339 gpio_tlmm_config(%d) fail",data->sensor_reset);
        return rc;
      }
    }else{
      CDBG("gc0339 gpio_request(%d) fail",data->sensor_reset);
      return rc;
    }
  }

  rc = gpio_request(data->sensor_pwd, "gc0339");
  if (0 == rc) {
    rc = gpio_tlmm_config(GPIO_CFG(data->sensor_pwd, 0,GPIO_CFG_OUTPUT,
                          GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
    if (rc < 0) {
      gpio_free(data->sensor_reset);
      gpio_free(data->sensor_pwd);
      CDBG("gc0339 gpio_tlmm_config(%d) fail",data->sensor_pwd);
      return rc;
    }
  }else{
    gpio_free(data->sensor_reset);
    CDBG("gc0339 gpio_request(%d) fail",data->sensor_pwd);
    return rc;
  }
  if (data->vcm_enable)
  {
    gpio_direction_output(data->vcm_pwd, 1);
  }
#endif  
  gpio_direction_output(data->sensor_reset, 1);
  gpio_direction_output(data->sensor_pwd, 1);
  return rc;
}

/******************************************************************************

******************************************************************************/
static void gc0339_probe_free_gpio(const struct msm_camera_sensor_info *data)
{
	return;
  gpio_free(gc0339_pwdn_gpio);
  gpio_free(gc0339_reset_gpio);
  if (data->vcm_enable)
  {
    gpio_free(gc0339_vcm_pwd_gpio);
    gc0339_vcm_pwd_gpio = 0xFF ;
  }
  gc0339_pwdn_gpio = 0xFF;
  gc0339_reset_gpio = 0xFF;
}

/******************************************************************************

******************************************************************************/
int gc0339_sensor_open_init(const struct msm_camera_sensor_info *data)
{
  int32_t rc = -ENOMEM;
  CDBG("%s,Line: %d \n",__func__,__LINE__);
  gc0339_ctrl = kzalloc(sizeof(struct gc0339_ctrl_t), GFP_KERNEL);
  if (!gc0339_ctrl) {
    CDBG("gc0339_init failed!\n");
    return rc;
  }
  gc0339_ctrl->fps_divider      = 1 * 0x00000400;
  gc0339_ctrl->pict_fps_divider = 1 * 0x00000400;
  gc0339_ctrl->set_test         = S_TEST_OFF;
  gc0339_ctrl->prev_res         = S_QTR_SIZE;
  gc0339_ctrl->pict_res         = S_FULL_SIZE;
  gc0339_ctrl->config_csi       = 1;
  if (data)
    gc0339_ctrl->sensordata = data;
  lcd_camera_power_onoff(1);
  prev_frame_length_lines    = 500;
  prev_line_length_pck       = 800;
  snap_frame_length_lines    = 500;
  snap_line_length_pck       = 800;
  gc0339_power_off();
  CDBG("%s: msm_camio_clk_rate_set\n",__func__);
  msm_camio_clk_rate_set(GC0339_MASTER_CLK_RATE);
  msleep(5);
  gc0339_power_on();
  msleep(5);
  gc0339_power_reset();
  msleep(5);
  if (gc0339_ctrl->prev_res == S_QTR_SIZE)
    rc = gc0339_sensor_setting(S_REG_INIT, S_RES_PREVIEW);
  else
    rc = gc0339_sensor_setting(S_REG_INIT, S_RES_CAPTURE);
  if (rc < 0)
  {
    CDBG("%s :gc0339_sensor_setting failed. rc = %d\n",__func__,rc);
    kfree(gc0339_ctrl);
    gc0339_ctrl = NULL;
    gc0339_power_off();
    lcd_camera_power_onoff(0);
    return rc;
  }
  gc0339_ctrl->fps = 30 * Q8;
  GC0339_CSI_CONFIG = 0;
  CDBG("%s: re_init_sensor ok\n",__func__);
  return rc;
}

/******************************************************************************

******************************************************************************/
static int gc0339_sensor_release(void)
{
  CDBG("%s\n",__func__);
  mutex_lock(&gc0339_mut);

    gc0339_power_reset();

msleep(10);



  gc0339_power_off();
  lcd_camera_power_onoff(0);
  if(NULL != gc0339_ctrl)
  {
    kfree(gc0339_ctrl);
    gc0339_ctrl = NULL;
  }
  GC0339_CSI_CONFIG = 0;
  mutex_unlock(&gc0339_mut);
  return 0;
}

/******************************************************************************

******************************************************************************/
#ifdef CONFIG_LCT_AW550
#include "../../../../arch/arm/mach-msm/clock.h"
#include "../../../../arch/arm/mach-msm/clock-pcom.h"
#include "../../../../arch/arm/mach-msm/clock-voter.h"

static DEFINE_CLK_PCOM(csi0_clk,	CSI0_CLK,	CLKFLAG_SKIP_AUTO_OFF);
static DEFINE_CLK_PCOM(csi0_p_clk,	CSI0_P_CLK,	CLKFLAG_SKIP_AUTO_OFF);
static DEFINE_CLK_PCOM(csi0_vfe_clk,	CSI0_VFE_CLK,	CLKFLAG_SKIP_AUTO_OFF);

static struct clk_lookup gc0339_csi_tbl[] = {
	CLK_LOOKUP("csi_clk",	csi0_clk.c,	"msm_camera_gc0339.0"),
	CLK_LOOKUP("csi_pclk",	csi0_p_clk.c,	"msm_camera_gc0339.0"),
	CLK_LOOKUP("csi_vfe_clk",	csi0_vfe_clk.c,	"msm_camera_gc0339.0"),
};

static unsigned num_gc0339_csi_tbl = ARRAY_SIZE(gc0339_csi_tbl);
#endif


static int gc0339_sensor_probe(const struct msm_camera_sensor_info *info,
                                struct msm_sensor_ctrl *s)
{
  int rc = -ENOTSUPP;
  CDBG("%s\n",__func__);
  rc = i2c_add_driver(&gc0339_i2c_driver);
  if ((rc < 0 ) || (gc0339_client == NULL))
  {
    CDBG("i2c_add_driver failed\n");
    if(NULL != gc0339_sensorw)
    {
      kfree(gc0339_sensorw);
      gc0339_sensorw = NULL;
    }
    gc0339_client = NULL;
    return rc;
  }
  lcd_camera_power_onoff(1);
  rc = gc0339_probe_init_gpio(info);
  if (rc < 0) {
    CDBG("%s,gc0339_probe_init_gpio failed\n",__func__);
    i2c_del_driver(&gc0339_i2c_driver);
    return rc;
  }
  gc0339_power_off();
  msm_camio_clk_rate_set(GC0339_MASTER_CLK_RATE);
  msleep(5);
  gc0339_power_on();
  gc0339_power_reset();
  rc = gc0339_read_model_id(info);
  if (rc < 0)
  {
    CDBG("gc0339_read_model_id failed\n");
    CDBG("%s,unregister\n",__func__);
    i2c_del_driver(&gc0339_i2c_driver);
    gc0339_power_off();
    lcd_camera_power_onoff(0);
    gc0339_probe_free_gpio(info);
    return rc;
  }
  s->s_init = gc0339_sensor_open_init;
  s->s_release = gc0339_sensor_release;
  s->s_config  = gc0339_sensor_config;
  s->s_camera_type = FRONT_CAMERA_2D;
  s->s_mount_angle = info->sensor_platform_info->mount_angle;
#ifdef CONFIG_LCT_AW550
    info->pdata->camera_csi_config_front(gc0339_csi_tbl,num_gc0339_csi_tbl);
#endif
  
  gc0339_power_off();
  lcd_camera_power_onoff(0);
  return rc;
}

/******************************************************************************

******************************************************************************/
int gc0339_sensor_config(void __user *argp)
{
  struct sensor_cfg_data cdata;
  long   rc = 0;
  if (copy_from_user(&cdata,(void *)argp,sizeof(struct sensor_cfg_data)))
    return -EFAULT;
  mutex_lock(&gc0339_mut);
  CDBG("%s: cfgtype = %d\n",__func__,cdata.cfgtype);
  #if 1
  switch (cdata.cfgtype) {
    case CFG_GET_PICT_FPS:
      {
        CDBG("%s: cfgtype = CFG_GET_PICT_FPS = %d\n",__func__,cdata.cfgtype);
        gc0339_get_pict_fps(cdata.cfg.gfps.prevfps,&(cdata.cfg.gfps.pictfps));
        if (copy_to_user((void *)argp,&cdata,sizeof(struct sensor_cfg_data)))
          rc = -EFAULT;
      }
      break;
    case CFG_GET_PREV_L_PF:
      {
        CDBG("%s: cfgtype = CFG_GET_PREV_L_PF = %d\n",__func__,cdata.cfgtype);
        cdata.cfg.prevl_pf =gc0339_get_prev_lines_pf();
        if (copy_to_user((void *)argp,&cdata,sizeof(struct sensor_cfg_data)))
          rc = -EFAULT;
      }
      break;
    case CFG_GET_PREV_P_PL:
      {
        CDBG("%s: cfgtype = CFG_GET_PREV_P_PL = %d\n",__func__,cdata.cfgtype);
        cdata.cfg.prevp_pl = gc0339_get_prev_pixels_pl();
        if (copy_to_user((void *)argp,&cdata,sizeof(struct sensor_cfg_data)))
         rc = -EFAULT;
      }
      break;
    case CFG_GET_PICT_L_PF:
      {
        CDBG("%s: cfgtype = CFG_GET_PICT_L_PF = %d\n",__func__,cdata.cfgtype);
        cdata.cfg.pictl_pf = gc0339_get_pict_lines_pf();
        if (copy_to_user((void *)argp,&cdata,sizeof(struct sensor_cfg_data)))
          rc = -EFAULT;
      }
      break;
    case CFG_GET_PICT_P_PL:
      {
        CDBG("%s: cfgtype = CFG_GET_PICT_P_PL = %d\n",__func__,cdata.cfgtype);
        cdata.cfg.pictp_pl = gc0339_get_pict_pixels_pl();
        if (copy_to_user((void *)argp,&cdata,sizeof(struct sensor_cfg_data)))
          rc = -EFAULT;
      }
      break;
    case CFG_GET_PICT_MAX_EXP_LC:
      {
        CDBG("%s: cfgtype = CFG_GET_PICT_MAX_EXP_LC = %d\n",
          __func__,cdata.cfgtype);
        cdata.cfg.pict_max_exp_lc = gc0339_get_pict_max_exp_lc();
        if (copy_to_user((void *)argp,&cdata,sizeof(struct sensor_cfg_data)))
          rc = -EFAULT;
      }
      break;
    case CFG_SET_FPS:
    case CFG_SET_PICT_FPS:
      {
        CDBG("%s: cfgtype = CFG_SET_FPS or CFG_SET_PICT_FPS = %d\n",
          __func__,cdata.cfgtype);
        rc = gc0339_set_fps(&(cdata.cfg.fps));
      }
      break;
    case CFG_SET_EXP_GAIN:
      {
        CDBG("%s: cfgtype = CFG_SET_EXP_GAIN = %d\n",__func__,cdata.cfgtype);
        rc = gc0339_write_exp_gain(cdata.cfg.exp_gain.gain,
          cdata.cfg.exp_gain.line);
      }
      break;
    case CFG_SET_PICT_EXP_GAIN:
      {
        CDBG("%s: cfgtype = CFG_SET_PICT_EXP_GAIN = %d\n",
          __func__,cdata.cfgtype);
        rc = gc0339_set_pict_exp_gain(cdata.cfg.exp_gain.gain,
          cdata.cfg.exp_gain.line);
      }
      break;
    case CFG_SET_MODE:
      {
        CDBG("%s: cfgtype = CFG_SET_MODE = %d\n",__func__,cdata.cfgtype);
        rc = gc0339_set_sensor_mode(cdata.mode, cdata.rs);
      }
      break;
    case CFG_PWR_DOWN:
      {
        CDBG("%s: cfgtype = CFG_PWR_DOWN = %d\n",__func__,cdata.cfgtype);
        gc0339_power_off();
        rc = 0;
      }
      break;
    case CFG_MOVE_FOCUS:
      {
        CDBG("%s: cfgtype = CFG_MOVE_FOCUS = %d\n",__func__,cdata.cfgtype);
        rc = -EFAULT;
      }
      break;
    case CFG_SET_DEFAULT_FOCUS:
      {
        CDBG("%s: cfgtype = CFG_SET_DEFAULT_FOCUS = %d\n",
          __func__,cdata.cfgtype);
        rc = -EFAULT;
      }
      break;
    case CFG_GET_AF_MAX_STEPS:
      {
        CDBG("%s: cfgtype = CFG_GET_AF_MAX_STEPS = %d\n",__func__,cdata.cfgtype);
        rc = -EFAULT;
      }
      break;
	case CFG_SET_BRIGHTNESS:
	    CDBG("--CAMERA-- CFG_SET_BRIGHTNESS  !!\n");
	    break;
	case CFG_SET_CONTRAST:
	    CDBG("--CAMERA-- CFG_SET_CONTRAST  !!\n");
	    break;	  
	case CFG_SET_SATURATION:
	    CDBG("--CAMERA-- CFG_SET_SATURATION !!\n");
	    break;
	case CFG_SET_SHARPNESS:
	    CDBG("--CAMERA-- CFG_SET_SHARPNESS !!\n");
	    break;		
	case CFG_SET_ANTIBANDING:
	    CDBG("--CAMERA-- CFG_SET_ANTIBANDING antibanding = %d!!\n", cdata.cfg.antibanding);
	    break;		
	case CFG_SET_EXPOSURE_COMPENSATION:
	    CDBG("--CAMERA-- CFG_SET_EXPOSURE_COMPENSATION !\n");
	    break;	
	case CFG_SEND_WB_INFO:
		CDBG("--CAMERA-- CFG_SEND_WB_INFO (Not Support) !!\n");
		// Not Support
		break;		
    case CFG_SET_WB:
        CDBG("--CAMERA-- CFG_SET_WB!!\n");
        break;		
    case CFG_SET_EFFECT:
      {
        CDBG("%s: cfgtype = CFG_SET_EFFECT = %d\n",__func__,cdata.cfgtype);
      }
      break;
    default:
      {
        rc = -EFAULT;
      }
      break;
  }
#else
  switch (cdata.cfgtype) {
{
		case CFG_SET_MODE:
			rc = gc0339_set_sensor_mode(cdata.mode, cdata.rs);
			break;
		case CFG_SET_EFFECT:
			CDBG("--CAMERA-- CFG_SET_EFFECT mode=%d, effect = %d !!\n",cdata.mode, cdata.cfg.effect);
			//rc = gc0339_set_effect(cdata.mode, cdata.cfg.effect);
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
			CDBG("--CAMERA-- CFG_PWR_DOWN !!\n");
			gc0339_power_off();
			break;
		case CFG_WRITE_EXPOSURE_GAIN:
			CDBG("--CAMERA-- CFG_WRITE_EXPOSURE_GAIN (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SET_DEFAULT_FOCUS:
		    CDBG("--CAMERA-- CFG_SET_DEFAULT_FOCUS (Not Implement) !!\n");
		    break;
		case CFG_MOVE_FOCUS:
		    CDBG("--CAMERA-- CFG_MOVE_FOCUS (Not Implement) !!\n");
		    break;
		case CFG_REGISTER_TO_REAL_GAIN:
			CDBG("--CAMERA-- CFG_REGISTER_TO_REAL_GAIN (Not Support) !!\n");
			// Not Support
			break;
		case CFG_REAL_TO_REGISTER_GAIN:
			CDBG("--CAMERA-- CFG_REAL_TO_REGISTER_GAIN (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SET_FPS:
			CDBG("--CAMERA-- CFG_SET_FPS (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SET_PICT_FPS:
			CDBG("--CAMERA-- CFG_SET_PICT_FPS (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SET_BRIGHTNESS:
		    CDBG("--CAMERA-- CFG_SET_BRIGHTNESS  !!\n");
		    //rc = ov7692_set_brightness(cdata.cfg.brightness);
		    break;
		case CFG_SET_CONTRAST:
		    CDBG("--CAMERA-- CFG_SET_CONTRAST  !!\n");
		    break;
		case CFG_SET_ZOOM:
			CDBG("--CAMERA-- CFG_SET_ZOOM (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SET_EXPOSURE_MODE:
		    CDBG("--CAMERA-- CFG_SET_EXPOSURE_MODE !!\n");
		    break;
		case CFG_SET_WB:
			CDBG("--CAMERA-- CFG_SET_WB!!\n");
			rc = 0 ;
			break;
		case CFG_SET_ANTIBANDING:
		    CDBG("--CAMERA-- CFG_SET_ANTIBANDING antibanding = %d!!\n", cdata.cfg.antibanding);
		    break;
		case CFG_SET_EXP_GAIN:
			CDBG("--CAMERA-- CFG_SET_EXP_GAIN (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SET_PICT_EXP_GAIN:
			CDBG("--CAMERA-- CFG_SET_PICT_EXP_GAIN (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SET_LENS_SHADING:
		    CDBG("--CAMERA-- CFG_SET_LENS_SHADING !!\n");
		    break;
		case CFG_GET_PICT_FPS:
			CDBG("--CAMERA-- CFG_GET_PICT_FPS (Not Support) !!\n");
			// Not Support
			break;
		case CFG_GET_PREV_L_PF:
			CDBG("--CAMERA-- CFG_GET_PREV_L_PF (Not Support) !!\n");
			// Not Support
			break;
		case CFG_GET_PREV_P_PL:
			CDBG("--CAMERA-- CFG_GET_PREV_P_PL (Not Support) !!\n");
			// Not Support
			break;
		case CFG_GET_PICT_L_PF:
			CDBG("--CAMERA-- CFG_GET_PICT_L_PF (Not Support) !!\n");
			// Not Support
			break;
		case CFG_GET_PICT_P_PL:
			CDBG("--CAMERA-- CFG_GET_PICT_P_PL (Not Support) !!\n");
			// Not Support
			break;
		case CFG_GET_AF_MAX_STEPS:
			CDBG("--CAMERA-- CFG_GET_AF_MAX_STEPS (Not Support) !!\n");
			// Not Support
			break;
		case CFG_GET_PICT_MAX_EXP_LC:
			CDBG("--CAMERA-- CFG_GET_PICT_MAX_EXP_LC (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SEND_WB_INFO:
			CDBG("--CAMERA-- CFG_SEND_WB_INFO (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SENSOR_INIT:
			CDBG("--CAMERA-- CFG_SENSOR_INIT (Not Support) !!\n");
			// Not Support
			break;
		case CFG_SET_SATURATION:
		    CDBG("--CAMERA-- CFG_SET_SATURATION !!\n");
		    break;
		case CFG_SET_SHARPNESS:
		    CDBG("--CAMERA-- CFG_SET_SHARPNESS !!\n");
		    break;
		case CFG_SET_TOUCHAEC:
		    CDBG("--CAMERA-- CFG_SET_TOUCHAEC!!\n");
		    rc = 0 ;
		    break;
		case CFG_SET_AUTO_FOCUS:
		    CDBG("--CAMERA-- CFG_SET_AUTO_FOCUS (Not Support) !!\n");
		    // Not Support
		    break;
		case CFG_SET_AUTOFLASH:
		    CDBG("--CAMERA-- CFG_SET_AUTOFLASH (Not Support) !!\n");
		    // Not Support
		    break;
		case CFG_SET_EXPOSURE_COMPENSATION:
		    CDBG("--CAMERA-- CFG_SET_EXPOSURE_COMPENSATION !\n");
		    break;
		case CFG_SET_ISO:
			CDBG("--CAMERA-- CFG_SET_ISO !\n");
		    break;
		default:
		    CDBG("--CAMERA-- %s: Command=%d (Not Implement)!!\n", __func__, cdata.cfgtype);
		    rc = -EINVAL;
		    break;
	}
#endif  
  mutex_unlock(&gc0339_mut);
  return rc;
}

/******************************************************************************

******************************************************************************/
static int __devinit gc0339_probe(struct platform_device *pdev)
{
  return msm_camera_drv_start(pdev, gc0339_sensor_probe);
}

/******************************************************************************

******************************************************************************/
static struct platform_driver msm_camera_driver = {
  .probe = gc0339_probe,
  .driver = {
    .name = "msm_camera_gc0339",
    .owner = THIS_MODULE,
  },
};

/******************************************************************************

******************************************************************************/
static int __init gc0339_init(void)
{
  return platform_driver_register(&msm_camera_driver);
}

module_init(gc0339_init);
MODULE_DESCRIPTION("Aptina 5 MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");
