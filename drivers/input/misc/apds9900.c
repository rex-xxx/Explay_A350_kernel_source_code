/*
 *  apds9900.c - Linux kernel modules for ambient light + proximity sensor
 *
 *  Copyright (C) 2010 Lee Kai Koon <kai-koon.lee@avagotech.com>
 *  Copyright (C) 2010 Avago Technologies
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <asm/gpio.h>
#include <linux/sysctl.h>
#include <linux/poll.h>
#include <linux/jiffies.h>
#include <linux/miscdevice.h>
#include <linux/wakelock.h>
#if 1//defined(CONFIG_LCT_AE760)
#include <mach/vreg.h>
#endif

#define APDS9900_DRV_NAME	"apds9900"
#define APDS9900_PS_NAME    "psdev"  // to fit two light sensor : ltr502als and apds9900
#define APDS9900_LS_NAME    "lightdev"  // to fit two light sensor : ltr502als and apds9900
#define DRIVER_VERSION		"1.0.3"
#define GPIO_INT 17
#define APDS9900_DEBUG 1


#define APDS9900_ALS_THRESHOLD_HSYTERESIS	20	/* 20 = 20% */

/* Change History 
 *
 * 1.0.1	Functions apds9900_show_rev(), apds9900_show_id() and apds9900_show_status()
 *			have missing CMD_BYTE in the i2c_smbus_read_byte_data(). APDS-9900 needs
 *			CMD_BYTE for i2c write/read byte transaction.
 *
 *
 * 1.0.2	Include PS switching threshold level when interrupt occurred
 *
 *
 * 1.0.3	Implemented ISR and delay_work, correct PS threshold storing
 */

/*
 * Defines
 */

#define APDS9900_ENABLE_REG	0x00
#define APDS9900_ATIME_REG	0x01
#define APDS9900_PTIME_REG	0x02
#define APDS9900_WTIME_REG	0x03
#define APDS9900_AILTL_REG	0x04
#define APDS9900_AILTH_REG	0x05
#define APDS9900_AIHTL_REG	0x06
#define APDS9900_AIHTH_REG	0x07
#define APDS9900_PILTL_REG	0x08
#define APDS9900_PILTH_REG	0x09
#define APDS9900_PIHTL_REG	0x0A
#define APDS9900_PIHTH_REG	0x0B
#define APDS9900_PERS_REG	0x0C
#define APDS9900_CONFIG_REG	0x0D
#define APDS9900_PPCOUNT_REG	0x0E
#define APDS9900_CONTROL_REG	0x0F
#define APDS9900_REV_REG	0x11
#define APDS9900_ID_REG		0x12
#define APDS9900_STATUS_REG	0x13
#define APDS9900_CDATAL_REG	0x14
#define APDS9900_CDATAH_REG	0x15
#define APDS9900_IRDATAL_REG	0x16
#define APDS9900_IRDATAH_REG	0x17
#define APDS9900_PDATAL_REG	0x18
#define APDS9900_PDATAH_REG	0x19

#define CMD_BYTE	0x80
#define CMD_WORD	0xA0
#define CMD_SPECIAL	0xE0

#define CMD_CLR_PS_INT	0xE5
#define CMD_CLR_ALS_INT	0xE6
#define CMD_CLR_PS_ALS_INT	0xE7

#if defined(CONFIG_LCT_AW70)
#define ALS_GA	574
#define ALS_COE_B	221
#define ALS_COE_C	25
#define ALS_COE_D	51
#else
#define ALS_GA	48
#define ALS_COE_B	223
#define ALS_COE_C	70
#define ALS_COE_D	142
#endif

/*
 * Structs
 */

struct apds9900_data {
	struct i2c_client *client;
	struct mutex update_lock;
	struct delayed_work	dwork;

#if defined(CONFIG_LCT_AE760)
	struct vreg *vreg_vdd;
	unsigned int poweron;
#endif
	unsigned int enable;
	unsigned int atime;
	unsigned int ptime;
	unsigned int wtime;
	unsigned int ailt;
	unsigned int aiht;
	unsigned int pilt;
	unsigned int piht;
	unsigned int pers;
	unsigned int config;
	unsigned int ppcount;
	unsigned int control;

	/* PS parameters */
	//unsigned int ps_enabled;
	unsigned int ps_threshold;
	unsigned int ps_detection;		/* 0 = near-to-far; 1 = far-to-near */
	unsigned int ps_data;			/* to store PS data */

	/* ALS parameters */
	//unsigned int ls_enabled;
	unsigned int als_threshold_l;	/* low threshold */
	unsigned int als_threshold_h;	/* high threshold */
	unsigned int als_data;			/* to store ALS data */
	unsigned int als_irdata;
};

/*
 * Global data
 */
static int aTime = 0xDB; /* 100.64ms */
//static int alsGain = 1;
//static double GA=0.48;
//static double COE_B=2.23;
//static double COE_C=0.7;
//static double COE_D=1.42;
//static int DF=52;
static DEFINE_MUTEX(apds9900_mutex);
static DECLARE_WAIT_QUEUE_HEAD(ps_waitqueue);
static DECLARE_WAIT_QUEUE_HEAD(ls_waitqueue);
static struct apds9900_data *apds9900_data;
static struct i2c_client *this_client;
static struct wake_lock proximity_wake_lock;
static int psensor_opened;
static int lsensor_opened;
static char ps_data;
static char ls_data;
static int ps_data_changed;
static int ls_data_changed;
static int ps_enabled;
static int ls_enabled;
/* magic number */  
#define APDS9900_IOM      'r'
/* IOCTLs for apds9900 device */
#define APDS9900_IOC_SET_PS_ENABLE       _IOW(APDS9900_IOM, 0, char *)
#define APDS9900_IOC_SET_LS_ENABLE      _IOW(APDS9900_IOM, 1, char *)
#define APDS9900_IOC_READ_PS_DATA       _IOR(APDS9900_IOM, 2, char *)
//#define APDS9900_IOC_READ_PS_INT          _IOR(APDS9900_IOM, 3, char *)
#define APDS9900_IOC_READ_LS_DATA        _IOR(APDS9900_IOM, 4, char *)
//#define APDS9900_IOC_READ_LS_INT          _IOR(APDS9900_IOM, 5, char *)

/*
 * Management functions
 */

/* empirical value to fit two light sensor : ltr502als and apds9900 */
static int luxcalculation(int cdata, int irdata, int again, int atime)
{
	int luxValue = 0;
	
	int IAC1 = 0;
	int IAC2 = 0;
	int IAC = 0;
	int GA = ALS_GA; 
	int COE_B = ALS_COE_B; 
	int COE_C = ALS_COE_C; 
	int COE_D = ALS_COE_D;
	int DF = 52;
	
	IAC1 = (cdata -(COE_B*irdata)/100);
	IAC2 = ((COE_C*cdata)/100 - (COE_D*irdata)/100);

	if(IAC1 > IAC2)
		IAC = IAC1;
	else if (IAC1 <= IAC2)
		IAC = IAC2;
	else
		IAC = 0;

	luxValue = ((IAC*GA*DF)/100)/(((272*(256-atime))/100)*again);

	return luxValue;
}

static void apds9900_change_ps_threshold(struct i2c_client *client)
{
	struct apds9900_data *data = i2c_get_clientdata(client);

	data->ps_data =	i2c_smbus_read_word_data(client, CMD_WORD|APDS9900_PDATAL_REG);
//pr_err("rxm: %s, ps_data=%u\n", __func__, data->ps_data);
	if ( (data->ps_data > data->pilt) && (data->ps_data >= data->piht) ) {
		/* far-to-near detected */
		data->ps_detection = 1;

		i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_PILTL_REG, data->ps_threshold);
		i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_PIHTL_REG, 1023);

		data->pilt = data->ps_threshold;
		data->piht = 1023;

		//printk("far-to-near detected\n");
	}
	else if ( (data->ps_data <= data->pilt) && (data->ps_data < data->piht) ) {
		/* near-to-far detected */
		data->ps_detection = 0;

		i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_PILTL_REG, 0);
		i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_PIHTL_REG, data->ps_threshold);

		data->pilt = 0;
		data->piht = data->ps_threshold;
		
		//printk("near-to-far detected\n");
	}

	if (data->ps_detection)
		ps_data = 0;  // 0 - near 
	else
		ps_data = 1; // 1 - far
	mutex_lock(&apds9900_mutex);
	ps_data_changed = 1;
	mutex_unlock(&apds9900_mutex);
	wake_up_interruptible(&ps_waitqueue);
}

static void apds9900_change_als_threshold(struct i2c_client *client)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int again, atime, cdata, irdata;

	cdata = data->als_data = i2c_smbus_read_word_data(client, CMD_WORD|APDS9900_CDATAL_REG);
	irdata = data->als_irdata = i2c_smbus_read_word_data(client, CMD_WORD|APDS9900_IRDATAL_REG);
//pr_err("rxm: %s, ls_data=%u\n", __func__, data->als_data);
	data->als_threshold_l = (data->als_data * (100-APDS9900_ALS_THRESHOLD_HSYTERESIS) ) /100;
	data->als_threshold_h = (data->als_data * (100+APDS9900_ALS_THRESHOLD_HSYTERESIS) ) /100;

	if (data->als_threshold_h >= 65535) data->als_threshold_h = 65535;

	i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_AILTL_REG, data->als_threshold_l);

	i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_AIHTL_REG, data->als_threshold_h);

	//ls_data = data->als_data & 0xff;
	switch((data->control)&0x03)
	{
		case 0:
			again = 1;
			break;
		case 1:
			again = 8;
			break;
		case 2:
			again = 16;
			break;
		case 3:
			again = 120;
			break;
		default:
			again = 1;
		
	}
	atime = data->atime;
	ls_data = luxcalculation(cdata, irdata, again, atime);
	mutex_lock(&apds9900_mutex);
	ls_data_changed = 1;
	mutex_unlock(&apds9900_mutex);
	wake_up_interruptible(&ls_waitqueue);
	
}

static void apds9900_work_handler(struct work_struct *work)
{
	struct apds9900_data *data = container_of(work, struct apds9900_data, dwork.work);
	struct i2c_client *client=data->client;
	int	status;
	int cdata;

	status = i2c_smbus_read_byte_data(client, CMD_BYTE|APDS9900_STATUS_REG);

	i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_ENABLE_REG, 1);	/* disable 9900's ADC first */

	//printk("status = 0x%x, enable = 0x%x\n", status, data->enable);

	if ((status & data->enable & 0x30) == 0x30) {
		/* both PS and ALS are interrupted */
		apds9900_change_als_threshold(client);
		
		cdata = i2c_smbus_read_word_data(client, CMD_WORD|APDS9900_CDATAL_REG);
		if (cdata < (75*(1024*(256-data->atime)))/100)
			apds9900_change_ps_threshold(client);
		else {
			printk("Triggered by background ambient noise\n");
		}

		i2c_smbus_write_byte(client, CMD_CLR_PS_ALS_INT);
	}
	else if ((status & data->enable & 0x20) == 0x20) {
		/* only PS is interrupted */
		
		/* check if this is triggered by background ambient noise */
		cdata = i2c_smbus_read_word_data(client, CMD_WORD|APDS9900_CDATAL_REG);
		if (cdata < (75*(1024*(256-data->atime)))/100)
			apds9900_change_ps_threshold(client);
		else {
			printk("Triggered by background ambient noise\n");
		}

		i2c_smbus_write_byte(client, CMD_CLR_PS_INT);
	}
	else if ((status & data->enable & 0x10) == 0x10) {
		/* only ALS is interrupted */	
		apds9900_change_als_threshold(client);

		i2c_smbus_write_byte(client, CMD_CLR_ALS_INT);
	}

	i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_ENABLE_REG, data->enable);

	if (wake_lock_active(&proximity_wake_lock))
		wake_unlock(&proximity_wake_lock);
}

static void apds9900_reschedule_work(struct apds9900_data *data,
					  unsigned long delay)
{
	mutex_lock(&data->update_lock);

	/*
	 * If work is already scheduled then subsequent schedules will not
	 * change the scheduled time that's why we have to cancel it first.
	 */
	__cancel_delayed_work(&data->dwork);
	schedule_delayed_work(&data->dwork, delay);

	mutex_unlock(&data->update_lock);
}

/* assume this is ISR */
static irqreturn_t apds9900_interrupt(int vec, void *info)
{
	struct i2c_client *client=(struct i2c_client *)info;
	struct apds9900_data *data = i2c_get_clientdata(client);

	//printk("==> apds9900_interrupt\n");
	apds9900_reschedule_work(data, 0);	

	return IRQ_HANDLED;
}

static int apds9900_set_command(struct i2c_client *client, int command)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	int clearInt;

	if (command == 0)
		clearInt = CMD_CLR_PS_INT;
	else if (command == 1)
		clearInt = CMD_CLR_ALS_INT;
	else
		clearInt = CMD_CLR_PS_ALS_INT;
		
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_byte(client, clearInt);
	mutex_unlock(&data->update_lock);

	return ret;
}

#if 0//defined(CONFIG_LCT_AE760)
static int apds9900_power(struct i2c_client *client, int poweron)
{
	struct apds9900_data *data = i2c_get_clientdata(client);	
	int ret = -EIO;

	pr_info("rxm: %s, poweron = 0x%x\n", __func__, poweron);
	//if((NULL != data) && (data->poweron != poweron))
	{
        if(NULL == data->vreg_vdd)
        {    
            data->vreg_vdd = vreg_get(NULL, "gp2"); 
            if (IS_ERR(data->vreg_vdd)) {            
                    printk(KERN_ERR "%s: vreg get failed (%ld)\n",__func__, PTR_ERR(data->vreg_vdd));
                return PTR_ERR(data->vreg_vdd);  
            }    
            ret = vreg_set_level(data->vreg_vdd, 2850);
            if(ret)
            {    
				printk(KERN_ERR "%s: vreg set level failed (%d)\n",	__func__, ret);
				return -EIO;        
            }    
        }   
	 
		if(poweron)
		{
			ret = vreg_enable(data->vreg_vdd);
			if (ret) {
				printk(KERN_ERR "%s: vreg enable failed (%d)\n",__func__, ret);
				return -EIO;
			}
		}else
		{
			ret = vreg_disable(data->vreg_vdd);
            if (ret) {
                printk(KERN_ERR "%s: vreg enable failed (%d)\n",__func__, ret);
                return -EIO;
            }
		}
		data->poweron = poweron;
	}
	return ret;
}
#endif

static int apds9900_set_enable(struct i2c_client *client, int enable)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
pr_err("rxm: %s, enable = 0x%x\n", __func__, enable);
	ret = i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_ENABLE_REG, enable);

	data->enable = enable;

	return ret;
}

static int apds9900_set_atime(struct i2c_client *client, int atime)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_ATIME_REG, atime);
	mutex_unlock(&data->update_lock);

	data->atime = atime;

	return ret;
}

static int apds9900_set_ptime(struct i2c_client *client, int ptime)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_PTIME_REG, ptime);
	mutex_unlock(&data->update_lock);

	data->ptime = ptime;

	return ret;
}

static int apds9900_set_wtime(struct i2c_client *client, int wtime)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_WTIME_REG, wtime);
	mutex_unlock(&data->update_lock);

	data->wtime = wtime;

	return ret;
}

static int apds9900_set_ailt(struct i2c_client *client, int threshold)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_AILTL_REG, threshold);
	mutex_unlock(&data->update_lock);
	
	data->ailt = threshold;

	return ret;
}

static int apds9900_set_aiht(struct i2c_client *client, int threshold)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_AIHTL_REG, threshold);
	mutex_unlock(&data->update_lock);
	
	data->aiht = threshold;

	return ret;
}

static int apds9900_set_pilt(struct i2c_client *client, int threshold)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_PILTL_REG, threshold);
	mutex_unlock(&data->update_lock);
	
	data->pilt = threshold;

	return ret;
}

static int apds9900_set_piht(struct i2c_client *client, int threshold)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_word_data(client, CMD_WORD|APDS9900_PIHTL_REG, threshold);
	mutex_unlock(&data->update_lock);
	
	data->piht = threshold;

	data->ps_threshold = threshold;

	return ret;
}

static int apds9900_set_pers(struct i2c_client *client, int pers)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_PERS_REG, pers);
	mutex_unlock(&data->update_lock);

	data->pers = pers;

	return ret;
}

static int apds9900_set_config(struct i2c_client *client, int config)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_CONFIG_REG, config);
	mutex_unlock(&data->update_lock);

	data->config = config;

	return ret;
}

static int apds9900_set_ppcount(struct i2c_client *client, int ppcount)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_PPCOUNT_REG, ppcount);
	mutex_unlock(&data->update_lock);

	data->ppcount = ppcount;

	return ret;
}

static int apds9900_set_control(struct i2c_client *client, int control)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int ret;
	
	mutex_lock(&data->update_lock);
	ret = i2c_smbus_write_byte_data(client, CMD_BYTE|APDS9900_CONTROL_REG, control);
	mutex_unlock(&data->update_lock);

	data->control = control;

	return ret;
}

/*
 * SysFS support
 */

static ssize_t apds9900_store_command(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	if (val < 0 || val > 2)
		return -EINVAL;

	ret = apds9900_set_command(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(command, S_IWUSR,
		   NULL, apds9900_store_command);

static ssize_t apds9900_show_enable(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->enable);
}

static ssize_t apds9900_store_enable(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	//struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret=0;
	struct vreg *vreg = NULL;
  	int rc = -1;	
	vreg = vreg_get(NULL, "gp2"); //L12

	//ret = apds9900_set_enable(client, val);
	if(1 == val)
	{
		rc = vreg_enable(vreg); 
		if (rc < 0) 
		{	
			pr_err("%s: apds9900---vreg enable failed (%d)\n", __func__, rc);	
			return rc;
		}
		pr_err("%s: apds9900---vreg enable successful (%d)\n", __func__, rc);	
	}
	else if(0 == val)
	{
		rc = vreg_disable(vreg); 
		if (rc < 0) 
		{	
			pr_err("%s: apds9900---vreg disable failed (%d)\n", __func__, rc);	
			return rc;
		}
		pr_err("%s: apds9900---vreg disable successful (%d)\n", __func__, rc);	
	}
	else{
			//do nothing
	}

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(enable, S_IWUSR | S_IRUGO,
		   apds9900_show_enable, apds9900_store_enable);

static ssize_t apds9900_show_atime(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->atime);
}

static ssize_t apds9900_store_atime(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_atime(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(atime, S_IWUSR | S_IRUGO,
		   apds9900_show_atime, apds9900_store_atime);

static ssize_t apds9900_show_ptime(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->ptime);
}

static ssize_t apds9900_store_ptime(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_ptime(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(ptime, S_IWUSR | S_IRUGO,
		   apds9900_show_ptime, apds9900_store_ptime);

static ssize_t apds9900_show_wtime(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->wtime);
}

static ssize_t apds9900_store_wtime(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_wtime(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(wtime, S_IWUSR | S_IRUGO,
		   apds9900_show_wtime, apds9900_store_wtime);

static ssize_t apds9900_show_ailt(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->ailt);
}

static ssize_t apds9900_store_ailt(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_ailt(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(ailt, S_IWUSR | S_IRUGO,
		   apds9900_show_ailt, apds9900_store_ailt);

static ssize_t apds9900_show_aiht(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->aiht);
}

static ssize_t apds9900_store_aiht(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_aiht(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(aiht, S_IWUSR | S_IRUGO,
		   apds9900_show_aiht, apds9900_store_aiht);

static ssize_t apds9900_show_pilt(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->pilt);
}

static ssize_t apds9900_store_pilt(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_pilt(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(pilt, S_IWUSR | S_IRUGO,
		   apds9900_show_pilt, apds9900_store_pilt);

static ssize_t apds9900_show_piht(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->piht);
}

static ssize_t apds9900_store_piht(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_piht(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(piht, S_IWUSR | S_IRUGO,
		   apds9900_show_piht, apds9900_store_piht);

static ssize_t apds9900_show_pers(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->pers);
}

static ssize_t apds9900_store_pers(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_pers(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(pers, S_IWUSR | S_IRUGO,
		   apds9900_show_pers, apds9900_store_pers);

static ssize_t apds9900_show_config(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->config);
}

static ssize_t apds9900_store_config(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_config(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(config, S_IWUSR | S_IRUGO,
		   apds9900_show_config, apds9900_store_config);

static ssize_t apds9900_show_ppcount(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->ppcount);
}

static ssize_t apds9900_store_ppcount(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_ppcount(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(ppcount, S_IWUSR | S_IRUGO,
		   apds9900_show_ppcount, apds9900_store_ppcount);

static ssize_t apds9900_show_control(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", data->control);
}

static ssize_t apds9900_store_control(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 10);
	int ret;

	ret = apds9900_set_control(client, val);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(control, S_IWUSR | S_IRUGO,
		   apds9900_show_control, apds9900_store_control);

static ssize_t apds9900_show_rev(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);
	int rev;

	mutex_lock(&data->update_lock);
	rev = i2c_smbus_read_byte_data(client, CMD_BYTE|APDS9900_REV_REG);
	mutex_unlock(&data->update_lock);

	return sprintf(buf, "%d\n", rev);
}

static DEVICE_ATTR(rev, S_IRUGO,
		   apds9900_show_rev, NULL);

static ssize_t apds9900_show_id(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);
	int id;

	mutex_lock(&data->update_lock);
	id = i2c_smbus_read_byte_data(client, CMD_BYTE|APDS9900_ID_REG);
	mutex_unlock(&data->update_lock);

	return sprintf(buf, "%d\n", id);
}

static DEVICE_ATTR(id, S_IRUGO,
		   apds9900_show_id, NULL);

static ssize_t apds9900_show_status(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);
	int status;

	mutex_lock(&data->update_lock);
	status = i2c_smbus_read_byte_data(client, CMD_BYTE|APDS9900_STATUS_REG);
	mutex_unlock(&data->update_lock);

	return sprintf(buf, "%d\n", status);
}

static DEVICE_ATTR(status, S_IRUGO,
		   apds9900_show_status, NULL);

static ssize_t apds9900_show_cdata(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);
	int cdata;

	mutex_lock(&data->update_lock);
	cdata = i2c_smbus_read_word_data(client, CMD_WORD|APDS9900_CDATAL_REG);
	mutex_unlock(&data->update_lock);
	
	return sprintf(buf, "%d\n", cdata);
}

static DEVICE_ATTR(cdata, S_IRUGO,
		   apds9900_show_cdata, NULL);

static ssize_t apds9900_show_irdata(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);
	int irdata;

	mutex_lock(&data->update_lock);
	irdata = i2c_smbus_read_word_data(client, CMD_WORD|APDS9900_IRDATAL_REG);
	mutex_unlock(&data->update_lock);
	
	return sprintf(buf, "%d\n", irdata);
}

static DEVICE_ATTR(irdata, S_IRUGO,
		   apds9900_show_irdata, NULL);

static ssize_t apds9900_show_pdata(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct apds9900_data *data = i2c_get_clientdata(client);
	int pdata;

	mutex_lock(&data->update_lock);
	pdata = i2c_smbus_read_word_data(client, CMD_WORD|APDS9900_PDATAL_REG);
	mutex_unlock(&data->update_lock);
	
	return sprintf(buf, "%d\n", pdata);
}

static DEVICE_ATTR(pdata, S_IRUGO,
		   apds9900_show_pdata, NULL);

static struct attribute *apds9900_attributes[] = {
	&dev_attr_command.attr,
	&dev_attr_enable.attr,
	&dev_attr_atime.attr,
	&dev_attr_ptime.attr,
	&dev_attr_wtime.attr,
	&dev_attr_ailt.attr,
	&dev_attr_aiht.attr,
	&dev_attr_pilt.attr,
	&dev_attr_piht.attr,
	&dev_attr_pers.attr,
	&dev_attr_config.attr,
	&dev_attr_ppcount.attr,
	&dev_attr_control.attr,
	&dev_attr_rev.attr,
	&dev_attr_id.attr,
	&dev_attr_status.attr,
	&dev_attr_cdata.attr,
	&dev_attr_irdata.attr,
	&dev_attr_pdata.attr,
	NULL
};

static const struct attribute_group apds9900_attr_group = {
	.attrs = apds9900_attributes,
};

/*
 * Initialization /dev inode
 */
 
 static int apds9900_ls_open(struct inode *inode, struct file *file)
{
	mutex_lock(&apds9900_mutex);
	if (lsensor_opened) {
		pr_err("%s: already opened\n", __func__);
		mutex_unlock(&apds9900_mutex);
		return -EBUSY;
	}
	lsensor_opened = 1;
	mutex_unlock(&apds9900_mutex);
	return 0;
}

static int apds9900_ls_release(struct inode *inode, struct file *file)
{
	mutex_lock(&apds9900_mutex);
	lsensor_opened = 0;
	mutex_unlock(&apds9900_mutex);
	return 0;
}

static long apds9900_ls_ioctl(struct file *file,
	unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	char rwbuf;
	int ret = 0;
	int flag;

	mutex_lock(&apds9900_mutex);

	switch (cmd) {
		case APDS9900_IOC_SET_LS_ENABLE :
			if (copy_from_user(&rwbuf, argp, 1))
				{ ret = -EFAULT; break; }
			flag = rwbuf ? (apds9900_data->enable | 0x1b) : 
					(ps_enabled ? (apds9900_data->enable & ~0x12) : 0);
			
			if(apds9900_set_enable(this_client, flag)) {
				ret = -EIO;
			} else {
				pr_info("rxm: %s, set light enable, rwbuf = %d, flag = 0x%x\n", __func__, rwbuf, flag);
 				if (rwbuf) {
					ls_enabled = 1;
				} else {
					ls_enabled = 0;
					ls_data = 0;
					ls_data_changed = 0;
				}
			}
			break;

		case APDS9900_IOC_READ_LS_DATA :
			rwbuf = ls_data;
			if(copy_to_user(argp, &rwbuf, 1))
				ret = -EFAULT;
			break;

       }

	mutex_unlock(&apds9900_mutex);
	return ret;
}

static unsigned int apds9900_ls_poll(struct file *fp, poll_table * wait)
{
	mutex_lock(&apds9900_mutex);	
	if(ls_data_changed){
		//pr_info("%s, ls_data_changed 1st, ls_data = 0x%x\n", __func__, ls_data);
		ls_data_changed = 0;
		mutex_unlock(&apds9900_mutex);
		return POLLIN | POLLRDNORM;
	} 
	poll_wait(fp, &ls_waitqueue, wait);
	mutex_unlock(&apds9900_mutex);
	return 0;
}

static struct file_operations apds9900_ls_fops = {
	.owner	= THIS_MODULE,
	.open	= apds9900_ls_open,
	.release	= apds9900_ls_release,
	.unlocked_ioctl	= apds9900_ls_ioctl,
	.poll       = apds9900_ls_poll,
};

static struct miscdevice apds9900_ls_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = APDS9900_LS_NAME,
	.fops   = &apds9900_ls_fops,
};

static int apds9900_ps_open(struct inode *inode, struct file *file)
{
	mutex_lock(&apds9900_mutex);
	if (psensor_opened) {
		pr_err("%s: already opened\n", __func__);
		mutex_unlock(&apds9900_mutex);
		return -EBUSY;
	}
	psensor_opened = 1;
	mutex_unlock(&apds9900_mutex);
	return 0;
}

static int apds9900_ps_release(struct inode *inode, struct file *file)
{

	mutex_lock(&apds9900_mutex);
	psensor_opened = 0;
	mutex_unlock(&apds9900_mutex);
	return 0;
}

static long apds9900_ps_ioctl(struct file *file,
	unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	char rwbuf;
	int ret = 0;
	int flag = 0;

	mutex_lock(&apds9900_mutex);

	switch (cmd) {
		case APDS9900_IOC_SET_PS_ENABLE :
			if (copy_from_user(&rwbuf, argp, 1))
				{ ret = -EFAULT; break; }
			flag = rwbuf ? (apds9900_data->enable | 0x2d) : 
				(ls_enabled ? (apds9900_data->enable & ~0x24) : 0);
			
			if(apds9900_set_enable(this_client, flag)) {
				ret = -EIO;
			} else {
				pr_info("rxm: %s, set proximity irq wake, rwbuf = %d, flag = 0x%x\n", __func__, rwbuf, flag);
 				if (rwbuf) {
					set_irq_wake(gpio_to_irq(GPIO_INT), 1);
					ps_enabled = 1;
				} else {
					set_irq_wake(gpio_to_irq(GPIO_INT), 0);
					ps_enabled = 0;
					ps_data = 1;
					ps_data_changed = 0;
				}				
			}
			break;

		case APDS9900_IOC_READ_PS_DATA :
			rwbuf = ps_data;
			if(copy_to_user(argp, &rwbuf, 1))
				ret = -EFAULT;
			break;

       }

	mutex_unlock(&apds9900_mutex);
	return ret;
}

static unsigned int apds9900_ps_poll(struct file *fp, poll_table * wait)
{
	mutex_lock(&apds9900_mutex);	
	if(ps_data_changed) {
		ps_data_changed = 0;
		mutex_unlock(&apds9900_mutex);
		return POLLIN | POLLRDNORM;
	} 
	poll_wait(fp, &ps_waitqueue, wait);
	mutex_unlock(&apds9900_mutex);
	return 0;
}

static struct file_operations apds9900_ps_fops = {
	.owner	= THIS_MODULE,
	.open	= apds9900_ps_open,
	.release	= apds9900_ps_release,
	.unlocked_ioctl = apds9900_ps_ioctl,
	.poll		= apds9900_ps_poll,
};

static struct miscdevice apds9900_ps_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = APDS9900_PS_NAME,
	.fops   = &apds9900_ps_fops,
};

static int apds9900_init_dev(void)
{
	int err;

	err = misc_register(&apds9900_ps_dev);
	if (err) {
		pr_err("apds9900 register ps failed, err=%d\n", err);
		return err;
	}

	err = misc_register(&apds9900_ls_dev);
	if (err) {
		pr_err("apds9900 register ls failed, err=%d\n", err);
		misc_deregister(&apds9900_ps_dev);
		return err;
	}

	return 0;
}

static int apds9900_init_reg(void)
{
	int err;
	err = apds9900_set_atime(this_client, aTime); /* 100.64ms */
	if (err) {
		pr_err("rxm: %s error, err=%d\n", __func__, err);
		return err;
	}
	apds9900_set_ptime(this_client, 0xFF); /* 2.72ms */
	apds9900_set_atime(this_client, 0xDB); /*0xDB 100ms,0xF6 27.2ms */

	apds9900_set_ppcount(this_client, 4); /* 4-pulse */
	apds9900_set_control(this_client, 0x20); /* 100mA, IR-diode, 1X PGAIN, 1X AGAIN */

	apds9900_set_pilt(this_client, 0);
    apds9900_set_piht(this_client, 700);

	apds9900_set_ailt(this_client, 500);
	apds9900_set_aiht(this_client, 2000);

	apds9900_set_command(this_client, 2); /* 0=ps int, 1=als int, 2=ps+als int */

	apds9900_set_pers(this_client, 0x33); /* 3 persistence */

	//apds9900_set_enable(0x2F);

	return 0;
}

/*
 * Initialization function
 */

static int apds9900_init_client(struct i2c_client *client)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int err;

	err = apds9900_set_enable(client, 0);

	if (err < 0)
    {
		pr_err("[%s] Set enable failed! err:%d\n",__func__,err);
        return err;
    }

	mdelay(1);

	mutex_lock(&data->update_lock);
	err = i2c_smbus_read_byte_data(client, APDS9900_ENABLE_REG);
	mutex_unlock(&data->update_lock);

	if (err != 0)
		return -ENODEV;

	data->enable = 0;

	err = apds9900_init_reg();
	if (err)
		return err;

	return 0;
}

static int apds9900_read_id(struct i2c_client* client)
{
	struct apds9900_data *data = i2c_get_clientdata(client);
	int id;

	mutex_lock(&data->update_lock);
	id = i2c_smbus_read_byte_data(client, CMD_BYTE|APDS9900_ID_REG);
	mutex_unlock(&data->update_lock);

	return id;
}


/*
 * I2C init/probing/exit functions
 */

static struct i2c_driver apds9900_driver;
static int __devinit apds9900_probe(struct i2c_client *client,
				   const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct apds9900_data *data;
	int err = 0;
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE)) {
		err = -EIO;
        pr_err("[%s]check functionality failed\n",__func__);
		goto exit;
	}
    data = kzalloc(sizeof(struct apds9900_data), GFP_KERNEL);
	if (!data) {
		err = -ENOMEM;
        pr_err("[%s]No enough memory!\n",__func__);
		goto exit;
	}

	data->client = client;
	i2c_set_clientdata(client, data);

	/* Keep data ptr and client*/
	apds9900_data = data;
	this_client = client;

#if 0 //defined(CONFIG_LCT_AE760)
	data->poweron = 0;
	data->vreg_vdd = NULL;
#endif

	data->enable = 0;	/* default mode is standard */
	data->ps_threshold = 0;
	data->ps_detection = 0;	/* default to no detection */

	//dev_info(&client->dev, "rxm: enable = %s\n",
	//		data->enable ? "1" : "0");

	mutex_init(&data->update_lock);

    if(apds9900_read_id(client)<0)
    {
        err = -EIO;
        pr_err("[%s]Can't read id!\n",__func__);
        goto exit_kfree;
    }


#if 0//defined(CONFIG_LCT_AE760)
	apds9900_power(client, 1);
#endif

	/* Initialize the APDS9900 chip */
	err = apds9900_init_client(client);
	if (err)
    {
        pr_err("[%s]Init client failed\n",__func__);
		goto exit_kfree;
    }
	dev_info(&client->dev, "rxm: apds9900_init_client() \n");

#if APDS9900_DEBUG
	/* Register sysfs hooks */
	err = sysfs_create_group(&client->dev.kobj, &apds9900_attr_group);
	if (err)
    {
        pr_err("[%s]Sysfs create failed!\n",__func__);
		goto exit_kfree;
    }
	dev_info(&client->dev, "rxm: sysfs_create_group() \n");
#endif

	/* Request irq & init work & init wakelock */
	INIT_DELAYED_WORK(&data->dwork, apds9900_work_handler);
	wake_lock_init(&proximity_wake_lock, WAKE_LOCK_SUSPEND, "proximity");
	dev_info(&client->dev, "rxm: interrupt is hooked\n");

	/* Register /dev inode */
	err = apds9900_init_dev();
	if (err)
    {
        pr_err("[%s]Init dev failed\n",__func__);
		goto exit_kfree;	
    }
	dev_info(&client->dev, "rxm: apds9900_init_dev() \n");

	//dev_info(&client->dev, "rxm: support ver. %s enabled\n", DRIVER_VERSION);

	if (request_irq(gpio_to_irq(GPIO_INT), apds9900_interrupt, IRQF_DISABLED|IRQ_TYPE_EDGE_FALLING,
		APDS9900_DRV_NAME, (void *)client)) {
        
        pr_err("[%s]Request irq %d failed\n",__func__,GPIO_INT);
		goto exit_kfree;
	}
	return 0;

exit_kfree:
	kfree(data);
exit:
	return err;
}

static int __devexit apds9900_remove(struct i2c_client *client)
{
	free_irq(gpio_to_irq(GPIO_INT), client);

#if APDS9900_DEBUG
	sysfs_remove_group(&client->dev.kobj, &apds9900_attr_group);
#endif

	/* Power down the device */
	apds9900_set_enable(client, 0);

	kfree(i2c_get_clientdata(client));

	misc_deregister(&apds9900_ps_dev);
	misc_deregister(&apds9900_ls_dev);

	return 0;
}

#ifdef CONFIG_PM

static int apds9900_suspend(struct device *dev)
{
	pr_err("rxm: %s, ps_enabled = %d, ls_enabled = %d\n", __func__, ps_enabled, ls_enabled);
	if (ps_enabled) {
		//TODO
	} else {
		apds9900_set_enable(this_client, 0);
	}
	return 0;
	//return apds9900_set_enable(client, 0);
}

static int apds9900_resume(struct device *dev)
{
	pr_err("rxm: %s, ps_enabled = %d, ls_enabled = %d\n", __func__, ps_enabled, ls_enabled);
	if (ps_enabled) {
		wake_lock(&proximity_wake_lock);
		schedule_delayed_work(&apds9900_data->dwork, 0);
	} else {
		apds9900_set_enable(this_client, apds9900_data->enable);
	}
	return 0;
	//return apds9900_set_enable(client, 0);
}

#else

#define apds9900_suspend	NULL
#define apds9900_resume		NULL

#endif /* CONFIG_PM */

static const struct i2c_device_id apds9900_id[] = {
	{ "apds9900", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, apds9900_id);

static struct dev_pm_ops apds9900_pm_ops = {
	.suspend = apds9900_suspend,
	.resume  = apds9900_resume,
};

static struct i2c_driver apds9900_driver = {
	.driver = {
		.name	= APDS9900_DRV_NAME,
		.owner	= THIS_MODULE,
		.pm		= &apds9900_pm_ops,
	},
	.probe	= apds9900_probe,
	.remove	= __devexit_p(apds9900_remove),
	.id_table = apds9900_id,
};

static int __init apds9900_init(void)
{
	pr_err("rxm: %s\n", __func__);
	return i2c_add_driver(&apds9900_driver);
}

static void __exit apds9900_exit(void)
{
	pr_err("rxm: %s\n", __func__);
	i2c_del_driver(&apds9900_driver);
}

MODULE_AUTHOR("Lee Kai Koon <kai-koon.lee@avagotech.com>");
MODULE_DESCRIPTION("APDS9900 ambient light + proximity sensor driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRIVER_VERSION);

module_init(apds9900_init);
module_exit(apds9900_exit);

