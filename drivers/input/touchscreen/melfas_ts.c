//--------------------------------------------------------
//
//
//	Melfas MCS6000 Series Download base v1.0 2011.08.26
//
//
//--------------------------------------------------------

/* drivers/input/touchscreen/melfas_ts.c
 *
 * Copyright (C) 2010 Melfas, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/input/melfas_ts.h>
#include <mach/gpio.h>
#include <mach/vreg.h>

#include <linux/miscdevice.h>
#include <asm/uaccess.h>


#define MELFAS_MAX_TOUCH       2
#define FW_VERSION          	mcsdl_get_fw_version()
#define HW_VERSION			    0x01 //Check Version

#define TS_MAX_X_COORD      320 //Check resolution
#define TS_MAX_Y_COORD      480 //Check resolution
#define TS_MAX_Z_TOUCH      255
#define TS_MAX_W_TOUCH      30

#define TS_READ_EVENT_PACKET_SIZE    0x0F
#define TS_READ_START_ADDR 	         0x10
#define TS_READ_VERSION_ADDR         0xF0 //Start read H/W, S/W Version
#define TS_READ_REGS_LEN             66

#define TOUCH_TYPE_NONE       0
#define TOUCH_TYPE_SCREEN     1
#define TOUCH_TYPE_KEY        2

#define I2C_RETRY_CNT			10
// ======================
#define SET_DOWNLOAD_BY_GPIO	0 // gpg
//#define FORCE_DOWNLOAD_VERSION		0xF7
// ======================
#define ESD_DETECTED        	1

#define PRESS_KEY				1
#define RELEASE_KEY		  0
#define DEBUG_PRINT 		0 // gpg
#define CONFIG_HAS_EARLYSUSPEND	1 //gpg

#define TS_READ_ONCE          1 //gpg it must open

#define TS_SOFT_RESET			0

#define MELFAS_SUPPORT_READ_TP_VERSION	1
#define MELFAS_ASS_FUNC			1

#if SET_DOWNLOAD_BY_GPIO
#include <melfas_download.h>
#endif // SET_DOWNLOAD_BY_GPIO

enum eMELFAS_TOUCH_STATUS
{
	eMELFAS_TS_RELEASED,
	eMELFAS_TS_RELEASE,			// Release
	eMELFAS_TS_DOWN,				// Fingle down
};


struct muti_touch_info
{
    int action;
    int fingerX;
    int fingerY;
    int width;
    int strength;
	int status;
};

struct melfas_ts_data
{
    uint16_t addr;
    struct i2c_client *client;
    struct input_dev *input_dev;
    struct work_struct  work;
    uint32_t flags;
    int (*power)(int on);
    struct early_suspend early_suspend;
	uint8_t fwVer;
	uint8_t hwVer;
	uint8_t mdVer;
#if SET_DOWNLOAD_BY_GPIO
	bool bAtUpdate;
#endif
};


struct melfas_ts_data *g_MelfasTs = NULL;


#ifdef CONFIG_HAS_EARLYSUSPEND
static void melfas_ts_early_suspend(struct early_suspend *h);
static void melfas_ts_late_resume(struct early_suspend *h);
#endif

static struct muti_touch_info g_Mtouch_info[MELFAS_MAX_TOUCH];

static void melfast_reset(void)
{
	gpio_set_value(MCS6024_CE_GPIO,0);
	msleep(5);
	gpio_set_value(MCS6024_CE_GPIO,1);
	msleep(120);
}

void melfas_ts_get_info( struct melfas_ts_data *ts)
{
    uint8_t buf[10];
	
	memset(buf, 0, sizeof(buf));
	buf[0] = 0xF0;

	if( (i2c_master_send(ts->client, buf, 1) < 0) 
		|| (i2c_master_recv(ts->client, buf, 6) < 0) )
	{
		printk(KERN_ERR "%s : Error!\n", __func__);	
		ts->hwVer = 0;
	 	ts->fwVer = 0;
		ts->mdVer = 0;
		return;
	}
	
	ts->hwVer = buf[1];
 	ts->fwVer = buf[3];
	ts->mdVer = buf[5];
	printk(KERN_ERR "melfas_ts_probe: TSP: %d\n", buf[0]);
	printk(KERN_ERR "melfas_ts_probe: HW Ver: %d\n", buf[1]);
	printk(KERN_ERR "melfas_ts_probe: CP Ver: %d\n", buf[2]);
	printk(KERN_ERR "melfas_ts_probe: FW Ver: %d\n", buf[3]);
	printk(KERN_ERR "melfas_ts_probe: Chip Info: %d\n", buf[4]);
	printk(KERN_ERR "melfas_ts_probe: MF Info: %d\n",	buf[5]);
}

#if SET_DOWNLOAD_BY_GPIO
void melfas_ts_start_update(bool bForce)
{
	mcsdl_init(g_MelfasTs->client, g_MelfasTs);
	disable_irq(g_MelfasTs->client->irq);
	#ifdef FORCE_DOWNLOAD_VERSION
	if((g_MelfasTs->fwVer != FORCE_DOWNLOAD_VERSION) 
		|| (bForce) )
	#else
	if( ( /*g_MelfasTs->hwVer == HW_VERSION && */ // HW_VERSION will be 1 or 2
		g_MelfasTs->fwVer < FW_VERSION)
		|| ((g_MelfasTs->fwVer != FW_VERSION)&&(g_MelfasTs->fwVer & 0xF0)) 	// 0xFx is test Version
		|| (bForce))
	#endif
	{
		printk(KERN_ERR "Melfas start to download:CUR_HW:%d, CUR_FW:%d, NEW_FW:%d", g_MelfasTs->hwVer,g_MelfasTs->fwVer, FW_VERSION);
		g_MelfasTs->bAtUpdate = TRUE;
		//ret = mcsdl_download_binary_file();
		mcsdl_download_binary_data();
		
		g_MelfasTs->client->addr = 0x20;
		g_MelfasTs->bAtUpdate = FALSE;
		melfast_reset();
		melfas_ts_get_info(g_MelfasTs);
	}
	enable_irq(g_MelfasTs->client->irq);	

}
#endif

#if 0 //gpg
static int melfas_init_panel(struct melfas_ts_data *ts)
{
    int buf = 0x00;
	i2c_master_send(ts->client, (char *)&buf, 1);
    return true;
}
#endif //gpg

static void melfas_ts_work_func(struct work_struct *work)
{
    struct melfas_ts_data *ts = container_of(work, struct melfas_ts_data, work);
    int ret = 0, i;
    uint8_t buf[TS_READ_REGS_LEN];
    uint8_t read_num = 0;
    uint8_t touchAction = 0, touchType = 0, fingerID = 0;
	bool bRelease = 0; 

    if (ts == NULL)
    {
    	printk(KERN_ERR "melfas_ts_work_func : TS NULL\n");
		return;
	}
#if SET_DOWNLOAD_BY_GPIO
	if (ts->bAtUpdate)
	{
    	printk(KERN_ERR "melfas_ts_work_func : TP updating...\n");	
		return;
	}
#endif
	/**
    Simple send transaction:
    	S Addr Wr [A]  Data [A] Data [A] ... [A] Data [A] P
    Simple recv transaction:
    	S Addr Rd [A]  [Data] A [Data] A ... A [Data] NA P
    */
	i = 0;
	while(i < I2C_RETRY_CNT)
	{
	    buf[0] = TS_READ_EVENT_PACKET_SIZE;
    	ret = i2c_master_send(ts->client, buf, 1);
		if( ret >= 0 )
		{
			break;
		}
		msleep(10);
		i++;
		printk(KERN_ERR "melfas_ts_work_func : i2c error retry %d times.", i);
	}

	if(i >= I2C_RETRY_CNT) // i2c error
	{
		printk(KERN_ERR "melfas_ts_work_func : i2c error reset device");
		melfast_reset();

		enable_irq(ts->client->irq);
		return;	
	}
    else //(i < I2C_RETRY_CNT) // ok go next
    {
#if TS_READ_ONCE
		ret = i2c_master_recv(ts->client, buf, 13);
#else
        ret = i2c_master_recv(ts->client, buf, 1);
#endif
		if ( ret < 0 )
    	{
        	printk(KERN_ERR "melfas_ts_work_func: recv i2c failed\n");
        	enable_irq(ts->client->irq);
        	return ;
    	}
#if DEBUG_PRINT
        printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv [%d]\n", ret);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv packet_size [%d]\n", buf[0]);
		
#if TS_READ_ONCE
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv InputEvent_information [%d]\n", buf[1]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv xy_coordi [%d]\n", buf[2]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv x_coordi [%d]\n", buf[3]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv y_coordi [%d]\n", buf[4]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv width [%d]\n", buf[5]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv strength [%d]\n", buf[6]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv 2_InputEvent_information [%d]\n", buf[7]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv 2_xy_coordi [%d]\n", buf[8]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv 2_x_coordi [%d]\n", buf[9]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv 2_y_coordi [%d]\n", buf[10]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv 2_width [%d]\n", buf[11]);
		printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv 2_strength [%d]\n", buf[12]);
#endif
#endif
        if (ret >= 0)
        {
            read_num = buf[0];

#if ESD_DETECTED    
            if((read_num & 0x80))
            {
#if DEBUG_PRINT
        		printk(KERN_ERR "%s: ***** ESD Detected status ****\n", __func__);
#endif            
				//Need CE or VDD Control for TSP reset.
				melfast_reset();

				enable_irq(ts->client->irq);
				return;
            }
#endif //ESD_DETECTED           
        }
        
        if(read_num > TS_READ_REGS_LEN)
        {
          read_num = TS_READ_REGS_LEN;
          //break; // i2c success
        }
    }
  
#if !(TS_READ_ONCE)
    buf[0] = TS_READ_START_ADDR;
    ret = i2c_master_send(ts->client, buf, 1);
#if DEBUG_PRINT
    printk(KERN_ERR "melfas_ts_work_func : i2c_master_send [%d]\n", ret);
#endif
    if (ret >= 0)
    {
        ret = i2c_master_recv(ts->client, buf, read_num);
#if DEBUG_PRINT
        printk(KERN_ERR "melfas_ts_work_func : i2c_master_recv [%d]\n", ret);
#endif
        //if (ret >= 0)
        //    break; // i2c success
        if(ret < 0) 
    	{
        	printk(KERN_ERR "melfas_ts_work_func: recv i2c failed\n");
        	enable_irq(ts->client->irq);
        	return ;
    	}
    }

    if(ret < 0) 
    {
        printk(KERN_ERR "melfas_ts_work_func: i2c failed\n");
        enable_irq(ts->client->irq);
        return ;
    }
    else // MIP (Melfas Interface Protocol)
#endif
    {
#if TS_READ_ONCE
		for(i = 1; i < read_num +1; i = i + 6)
#else
        for(i = 0; i < read_num; i = i + 6)
#endif
        {
            touchAction = ((buf[i] & 0x80) == 0x80);
            //touchAction = ((buf[i] & 0x10) == 0x10);//RevB
            touchType = (buf[i] & 0x60) >> 5;
            fingerID = (buf[i] & 0x0F);

			if(fingerID < 1 || fingerID > MELFAS_MAX_TOUCH)
			{
				continue;
			}
			fingerID --;
			
#if DEBUG_PRINT
			printk(KERN_ERR "melfas_ts_work_func: touchAction : %d, touchType: %d, fingerID: %d\n", touchAction, touchType, fingerID);
#endif

            if(touchType == TOUCH_TYPE_NONE)
            {
            }
            else if(touchType == TOUCH_TYPE_SCREEN)
            {

				if(touchAction == 0)
				{
					g_Mtouch_info[fingerID].status = eMELFAS_TS_RELEASE;
					g_Mtouch_info[fingerID].strength = 0;
					g_Mtouch_info[fingerID].width = 0;
				}
				else
				{
					g_Mtouch_info[fingerID].action = touchAction;
                	g_Mtouch_info[fingerID].fingerX = (buf[i + 1] & 0x0F) << 8 | buf[i + 2];
                	g_Mtouch_info[fingerID].fingerY = (buf[i + 1] & 0xF0) << 4 | buf[i + 3];
                	g_Mtouch_info[fingerID].width = buf[i + 4];
                	g_Mtouch_info[fingerID].strength = buf[i + 5];
					g_Mtouch_info[fingerID].status = eMELFAS_TS_DOWN;
				}			
            }
			
			#if 0 // it is no use now by gpg
            else if(touchType == TOUCH_TYPE_KEY)
            {
                if (fingerID == 0x1)
                    input_report_key(ts->input_dev, KEY_MENU, touchAction ? PRESS_KEY : RELEASE_KEY);
                if (fingerID == 0x2)
                    input_report_key(ts->input_dev, KEY_HOME, touchAction ? PRESS_KEY : RELEASE_KEY);
                if (fingerID == 0x3)
                    input_report_key(ts->input_dev, KEY_BACK, touchAction ? PRESS_KEY : RELEASE_KEY);
                if (fingerID == 0x4)
                    input_report_key(ts->input_dev, KEY_SEARCH, touchAction ? PRESS_KEY : RELEASE_KEY);


                input_sync(ts->input_dev);
#if DEBUG_PRINT
                printk(KERN_ERR "melfas_ts_work_func: keyID : %d, keyState: %d\n", fingerID, touchAction);
#endif
            }
			#endif
        }

		for(fingerID = 0; fingerID < MELFAS_MAX_TOUCH; fingerID++)
		{
#if  DEBUG_PRINT
			printk(KERN_ERR "melfas_ts_work_func: Touch ID: %d, x: %d, y: %d, z: %d w: %d s: %d\n",
				fingerID, g_Mtouch_info[fingerID].fingerX, g_Mtouch_info[fingerID].fingerY, g_Mtouch_info[fingerID].strength, g_Mtouch_info[fingerID].width, g_Mtouch_info[fingerID].status);
#endif
		
			if(g_Mtouch_info[fingerID].status  > eMELFAS_TS_RELEASED)
			{
				if( g_Mtouch_info[fingerID].status == eMELFAS_TS_RELEASE )
				{
					bRelease = 1;
					g_Mtouch_info[fingerID].status--; // goto released status, released status will not report msg
				}

				input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, fingerID);
				input_report_abs(ts->input_dev, ABS_MT_POSITION_X, g_Mtouch_info[fingerID].fingerX);
				input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, g_Mtouch_info[fingerID].fingerY);
				input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR,  g_Mtouch_info[fingerID].strength);
				input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR,g_Mtouch_info[fingerID].width);
				input_mt_sync(ts->input_dev);
			}
		}
		input_sync(ts->input_dev);

		if(	bRelease )
		{
			for(fingerID = 0; fingerID < MELFAS_MAX_TOUCH; fingerID++)
			{
				if(g_Mtouch_info[fingerID].status  > eMELFAS_TS_RELEASED)
				{
					input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, fingerID);
					input_report_abs(ts->input_dev, ABS_MT_POSITION_X, g_Mtouch_info[fingerID].fingerX);
					input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, g_Mtouch_info[fingerID].fingerY);
					input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR,  g_Mtouch_info[fingerID].strength);
					input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR,g_Mtouch_info[fingerID].width);
					input_mt_sync(ts->input_dev);
				}
			}
			input_sync(ts->input_dev);
		}
    }

    enable_irq(ts->client->irq);
}

static irqreturn_t melfas_ts_irq_handler(int irq, void *handle)
{
    struct melfas_ts_data *ts = (struct melfas_ts_data *)handle;
#if DEBUG_PRINT
    printk(KERN_ERR "melfas_ts_irq_handler\n");
#endif

    disable_irq_nosync(ts->client->irq);
    schedule_work(&ts->work);

    return IRQ_HANDLED;
}

#if TS_SOFT_RESET
void melfas_soft_power(int on, struct i2c_client *client)
{
    int ret;

	if(on)
	{
		gpio_set_value(MCS6024_IRQ_GPIO,0);
		msleep(100); // enlarge delay time from 5ms to 100ms
		gpio_set_value(MCS6024_IRQ_GPIO,1);

		gpio_tlmm_config(GPIO_CFG(MCS6024_IRQ_GPIO, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_6MA),GPIO_CFG_ENABLE); 
	}
	else
	{
		ret = i2c_smbus_write_byte_data(client, 0x01, 0x00); /* deep sleep */
		if (ret < 0)
		{
			printk(KERN_ERR "melfas_ts_suspend: i2c_smbus_write_byte_data failed\n");
		}

		msleep(100); // 
		gpio_tlmm_config(GPIO_CFG(MCS6024_IRQ_GPIO, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),GPIO_CFG_ENABLE); 
		gpio_set_value(MCS6024_IRQ_GPIO,1);
	}
}
#endif

#if 0 // no use
static int melfas_ts_power(int on)
{
	
	struct vreg *vreg_vcc;
	int rc = 0;
	printk(KERN_ERR"%s: on = %d\n", __func__,on);
	if(on == 1)
	{
		/*---------------------init avdd------------------------------*/
		vreg_vcc = vreg_get(NULL, "gp2");
		if (IS_ERR(vreg_vcc)) {
			printk(KERN_ERR "%s: vreg get failed (%ld)\n",
				   __func__, PTR_ERR(vreg_vcc));
			return PTR_ERR(vreg_vcc);
		}

		/* units of mV, steps of 50 mV */
		rc = vreg_set_level(vreg_vcc, 2850);
		if (rc) {
			printk(KERN_ERR "%s: vreg set level failed (%d)\n",
				__func__, rc);
			return -EIO;
		}

		/*---------------------enable power configed------------------------------*/
		rc = vreg_enable(vreg_vcc);
		if (rc) {
			printk(KERN_ERR "%s: vreg enable failed (%d)\n",
				   __func__, rc);
			return -EIO;
		}
	#ifdef DEBUG_PRINT
		printk("--%s--\n",__func__);
	#endif
		msleep(500);
	}
	return rc;
}
#endif

static ssize_t
melfas_virtual_keys_register(struct kobject *kobj,
			     struct kobj_attribute *attr,
			     char *buf)
{
	/* center_x: center_y: width: hight*/
	return sprintf(buf,
		__stringify(EV_KEY) ":" __stringify(KEY_HOME)		":22:520:45:60"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU)	":97:520:65:60"
		":" __stringify(EV_KEY) ":" __stringify(KEY_BACK)	":195:520:80:60"
		":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH)	":285:520:70:60"
		"\n");
}


static struct kobj_attribute melfas_virtual_keys_attr = {
	.attr = {
		.name = "virtualkeys.melfas-ts",
		.mode = S_IRUGO,
	},
	.show = &melfas_virtual_keys_register,
};

static struct attribute *melfas_virtual_key_properties_attrs[] = {
        &melfas_virtual_keys_attr.attr,
        NULL
};

static struct attribute_group melfas_virtual_key_properties_attr_group = {
        .attrs = melfas_virtual_key_properties_attrs,
};

struct kobject *melfas_virtual_key_properties_kobj;

static int melfas_ts_create_virtual_key(void)     
{
	int retval = 0;
	melfas_virtual_key_properties_kobj = kobject_create_and_add( \
    "board_properties", NULL);
    if (melfas_virtual_key_properties_kobj)
            retval = sysfs_create_group(melfas_virtual_key_properties_kobj, \
            &melfas_virtual_key_properties_attr_group);
    if (!melfas_virtual_key_properties_kobj || retval)
    	pr_err("failed to create ft5206 board_properties\n");
	return retval;
}


#if MELFAS_SUPPORT_READ_TP_VERSION
static int tp_version_open(struct inode *inode, struct file *file)
{
	return 0;
}

	
static int tp_version_release(struct inode *inode, struct file *file)
{
	return 0;
}
	
static ssize_t tp_version_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
	char tp_version[30] = {0};
	if(g_MelfasTs == NULL)
	{
		return 0;
	}
	
	if(g_MelfasTs->fwVer < 0)
	{
		strcpy(tp_version,"no tp");
	}
	else
	{
	#if SET_DOWNLOAD_BY_GPIO
		snprintf(tp_version, 30,"M%xV%x  open\n", g_MelfasTs->mdVer,g_MelfasTs->fwVer);
	#else
		snprintf(tp_version, 30,"M%xV%x\n", g_MelfasTs->mdVer,g_MelfasTs->fwVer);
	#endif
	}
	if(copy_to_user(buf, tp_version, strlen(tp_version)))
		return -EFAULT;

	return strlen(tp_version);
}

static struct file_operations tp_version_fops = {
	.owner = THIS_MODULE,
	.open = tp_version_open,
	.release = tp_version_release,
	.read = tp_version_read,
};

static struct miscdevice tp_version_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "tp_version",
	.fops = &tp_version_fops,
};

#endif

#if SET_DOWNLOAD_BY_GPIO

int	bma250_pause_for_tp_update(void);
int bma250_resume_for_tp_update(void);

static int tp_update_open(struct inode *inode, struct file *file)
{
	printk(KERN_ERR "%s Enter", __func__);

	if(g_MelfasTs == NULL)
	{
		printk(KERN_ERR "%s Bad Pointer", __func__);
		return 0;
	}
	bma250_pause_for_tp_update();
	melfas_ts_start_update(1);
	bma250_resume_for_tp_update();
	return 0;
}
	
static int tp_update_release(struct inode *inode, struct file *file)
{
	return 0;
}
	
static ssize_t tp_update_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
	return 0;
}

static struct file_operations tp_update_fops = {
	.owner = THIS_MODULE,
	.open = tp_update_open,
	.release = tp_update_release,
	.read = tp_update_read,
};

static struct miscdevice tp_update_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "tp_update",
	.fops = &tp_update_fops,
};
#endif

#if MELFAS_ASS_FUNC
static int tp_reset_open(struct inode *inode, struct file *file)
{
	printk(KERN_ERR "%s Enter", __func__);

	melfast_reset();
	return 0;
}
	
static int tp_reset_release(struct inode *inode, struct file *file)
{
	return 0;
}
	
static ssize_t tp_reset_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
	return 0;
}

static struct file_operations tp_reset_fops = {
	.owner = THIS_MODULE,
	.open = tp_reset_open,
	.release = tp_reset_release,
	.read = tp_reset_read,
};

static struct miscdevice tp_reset_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "tp_reset",
	.fops = &tp_reset_fops,
};
#endif

extern int is_tp_driver_loaded;

static int melfas_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct melfas_ts_data *ts;
    int ret = 0, i;

    uint8_t buf[10];

#if DEBUG_PRINT
    printk(KERN_ERR "kim ms : melfas_ts_probe\n");
#endif

	//melfas_ts_power(1); //no use here

	if(is_tp_driver_loaded)
	{
		printk(KERN_ERR"melfas_ts_probe other driver has been loaded\n");
		return -ENODEV;
	}

	
#if TS_SOFT_RESET
	melfas_soft_power(1, client);
#endif
	melfast_reset();

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
    {
        printk(KERN_ERR "melfas_ts_probe: need I2C_FUNC_I2C\n");
        ret = -ENODEV;
        goto err_check_functionality_failed;
    }

    ts = kmalloc(sizeof(struct melfas_ts_data), GFP_KERNEL);

    if (ts == NULL)
    {
        printk(KERN_ERR "melfas_ts_probe: failed to create a state of melfas-ts\n");
        ret = -ENOMEM;
        goto err_alloc_data_failed;
    }
	g_MelfasTs = ts;
	memset(ts, 0, sizeof(struct melfas_ts_data));
	
	INIT_WORK(&ts->work, melfas_ts_work_func);
	client->irq = gpio_to_irq(MCS6024_IRQ_GPIO);
	
    ts->client = client;
    i2c_set_clientdata(client, ts);

	for(i = 0; i < I2C_RETRY_CNT; i++)
	{
    	ret = i2c_master_send(ts->client, (char *)&buf, 1);	
        if (ret >= 0)
        {
			break; // i2c success
        }
		msleep(10);
	} 
	if(i >= I2C_RETRY_CNT)
	{
		printk(KERN_ERR "melfas_ts_probe: not found melfas\n");
		goto err_detect_failed; // do not go
	}
	else
	{
		melfas_ts_get_info(ts);
	}
	
#if SET_DOWNLOAD_BY_GPIO
	if(ts->fwVer >= 0x03) // after 0x03 can update fw
	{
		melfas_ts_start_update(0);
	}
#endif // SET_DOWNLOAD_BY_GPIO

    ts->input_dev = input_allocate_device();
    if (!ts->input_dev)
    {
        printk(KERN_ERR "melfas_ts_probe: Not enough memory\n");
        ret = -ENOMEM;
        goto err_input_dev_alloc_failed;
    }

    ts->input_dev->name = "melfas-ts" ;

    ts->input_dev->evbit[0] = BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);

	set_bit(BTN_MISC, ts->input_dev->evbit);
    set_bit(KEY_OK,  ts->input_dev->evbit);	
	set_bit(EV_SYN, ts->input_dev->evbit);

    //ts->input_dev->keybit[BIT_WORD(KEY_MENU)] |= BIT_MASK(KEY_MENU);
    //ts->input_dev->keybit[BIT_WORD(KEY_HOME)] |= BIT_MASK(KEY_HOME);
    //ts->input_dev->keybit[BIT_WORD(KEY_BACK)] |= BIT_MASK(KEY_BACK);		
    //ts->input_dev->keybit[BIT_WORD(KEY_SEARCH)] |= BIT_MASK(KEY_SEARCH);			

//	__set_bit(BTN_TOUCH, ts->input_dev->keybit);
//	__set_bit(EV_ABS,  ts->input_dev->evbit);
//	ts->input_dev->evbit[0] =  BIT_MASK(EV_SYN) | BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);


    input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0, TS_MAX_X_COORD, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0, TS_MAX_Y_COORD, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0, TS_MAX_Z_TOUCH, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_TRACKING_ID, 0, MELFAS_MAX_TOUCH - 1 , 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0, TS_MAX_W_TOUCH, 0, 0);
//	__set_bit(EV_SYN, ts->input_dev->evbit);
//	__set_bit(EV_KEY, ts->input_dev->evbit);

    ret = input_register_device(ts->input_dev);
    if (ret)
    {
        printk(KERN_ERR "melfas_ts_probe: Failed to register device\n");
        ret = -ENOMEM;
        goto err_input_register_device_failed;
    }

    if (ts->client->irq)
    {
#if DEBUG_PRINT
        printk(KERN_ERR "melfas_ts_probe: trying to request irq: %s-%d\n", ts->client->name, ts->client->irq);
#endif
        ret = request_irq(client->irq, melfas_ts_irq_handler,IRQF_TRIGGER_FALLING, ts->client->name, ts);
        if (ret > 0)
        {
            printk(KERN_ERR "melfas_ts_probe: Can't allocate irq %d, ret %d\n", ts->client->irq, ret);
            ret = -EBUSY;
            goto err_request_irq;
        }
    }

    schedule_work(&ts->work);

	for (i = 0; i < MELFAS_MAX_TOUCH ; i++)  /* _SUPPORT_MULTITOUCH_ */
		g_Mtouch_info[i].strength = -1;	

#if DEBUG_PRINT
    printk(KERN_ERR "melfas_ts_probe: succeed to register input device\n");
#endif

#if CONFIG_HAS_EARLYSUSPEND
    ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
    ts->early_suspend.suspend = melfas_ts_early_suspend;
    ts->early_suspend.resume = melfas_ts_late_resume;
    register_early_suspend(&ts->early_suspend);
#endif

	melfas_ts_create_virtual_key();

#if MELFAS_SUPPORT_READ_TP_VERSION
	misc_register(&tp_version_device);
#endif

#if SET_DOWNLOAD_BY_GPIO
	misc_register(&tp_update_device);
#endif

#if MELFAS_ASS_FUNC
	misc_register(&tp_reset_device); // ts->fwVer >= 3
#endif


#if DEBUG_PRINT
    printk(KERN_INFO "melfas_ts_probe: Start touchscreen. name: %s, irq: %d\n", ts->client->name, ts->client->irq);
#endif

   is_tp_driver_loaded = 1;

    return 0;

err_request_irq:
    printk(KERN_ERR "melfas-ts: err_request_irq failed\n");
    free_irq(client->irq, ts);
err_input_register_device_failed:
    printk(KERN_ERR "melfas-ts: err_input_register_device failed\n");
    input_free_device(ts->input_dev);
err_input_dev_alloc_failed:
    printk(KERN_ERR "melfas-ts: err_input_dev_alloc failed\n");
err_detect_failed:
	printk(KERN_ERR "melfas-ts: err_detect failed\n");
err_alloc_data_failed:
    printk(KERN_ERR "melfas-ts: err_alloc_data failed_\n");
    kfree(ts);
err_check_functionality_failed:
    printk(KERN_ERR "melfas-ts: err_check_functionality failed_\n");

    return ret;
}

static int melfas_ts_remove(struct i2c_client *client)
{
    struct melfas_ts_data *ts = i2c_get_clientdata(client);

    unregister_early_suspend(&ts->early_suspend);
    free_irq(client->irq, ts);
    input_unregister_device(ts->input_dev);
    kfree(ts);
    return 0;
}

#if 1 //gpg
static int melfas_ts_suspend(struct i2c_client *client, pm_message_t mesg)
{
    int ret;
	int i;    
    struct melfas_ts_data *ts = i2c_get_clientdata(client);
	printk(KERN_ERR "%s:  Enter\n", __func__);

	disable_irq(client->irq);

	ret = cancel_work_sync(&ts->work);
	if (ret) /* if work was pending disable-count is now 2 */
	{
		printk(KERN_ERR "%s:  ts work is still going\n", __func__);
		enable_irq(client->irq);
	}
	else
	{
		//	melfas_ts_power(0); //gpg
#if TS_SOFT_RESET//gpg
		if(ts->fwVer == 0x02)	// after 0x02 support soft reset,but after 0x03 will use hard reset
		{	
			melfas_soft_power(0, client);
		}
#endif

		gpio_set_value(MCS6024_CE_GPIO,0);
		for (i = 0; i < MELFAS_MAX_TOUCH ; i++)
		{
			g_Mtouch_info[i].strength = -1;
			g_Mtouch_info[i].fingerX = 0;
			g_Mtouch_info[i].fingerY = 0;
			g_Mtouch_info[i].width = 0;
		}
	}
	
    return 0;
}

static int melfas_ts_resume(struct i2c_client *client)
{
#if TS_SOFT_RESET
    struct melfas_ts_data *ts = i2c_get_clientdata(client);
#endif
	printk(KERN_ERR "%s:	Enter\n", __func__);

//	  melfas_ts_power(1); //gpg

#if TS_SOFT_RESET
	if(ts->fwVer == 0x02)   // after 0x02 support soft reset,but after 0x03 will use hard reset
	{   
		melfas_soft_power(1, client);
	}
#endif
	gpio_set_value(MCS6024_CE_GPIO,1);

#if 0
	msleep(120);
	cancel_work_sync(&ts->work);      
	schedule_work(&ts->work);
#else	 
	enable_irq(client->irq);  //enable_irq in work function
#endif

    return 0;
}
#endif // gpg


#ifdef CONFIG_HAS_EARLYSUSPEND
static void melfas_ts_early_suspend(struct early_suspend *h)
{
    struct melfas_ts_data *ts;
    ts = container_of(h, struct melfas_ts_data, early_suspend);
#if	SET_DOWNLOAD_BY_GPIO
	if(!ts->bAtUpdate)
#endif
	{
    	melfas_ts_suspend(ts->client, PMSG_SUSPEND);
	}
}

static void melfas_ts_late_resume(struct early_suspend *h)
{
    struct melfas_ts_data *ts;
    ts = container_of(h, struct melfas_ts_data, early_suspend);
    melfas_ts_resume(ts->client);
}
#endif

static const struct i2c_device_id melfas_ts_id[] =
{
    { MELFAS_TS_NAME, 0 },
    { }
};

static struct i2c_driver melfas_ts_driver =
{
    .driver = {
    .name = MELFAS_TS_NAME,
    },
    .id_table = melfas_ts_id,
    .probe = melfas_ts_probe,
    .remove = __devexit_p(melfas_ts_remove),
#ifndef CONFIG_HAS_EARLYSUSPEND
    .suspend = melfas_ts_suspend,
    .resume = melfas_ts_resume,
#endif
};

static int __devinit melfas_ts_init(void)
{
    return i2c_add_driver(&melfas_ts_driver);
}

static void __exit melfas_ts_exit(void)
{
    i2c_del_driver(&melfas_ts_driver);
}

MODULE_DESCRIPTION("Driver for Melfas MIP Touchscreen Controller");
MODULE_AUTHOR("SuHyun, Ahn <shahn@melfas.com>");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");

module_init(melfas_ts_init);
module_exit(melfas_ts_exit);
