/*******************************************************************************
*                                                                              *
*   File Name:    taos.c                                                      *
*   Description:   Linux device driver for Taos ambient light and         *
*   proximity sensors.                                     *
*   Author:         John Koshi                                             *
*   History:   09/16/2009 - Initial creation                          *
*           10/09/2009 - Triton version         *
*           12/21/2009 - Probe/remove mode                *
*           02/07/2010 - Add proximity          *
*                                                                              *
********************************************************************************
*    Proprietary to Taos Inc., 1001 Klein Road #300, Plano, TX 75074        *
*******************************************************************************/
// includes
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/timer.h>
#include <asm/uaccess.h>
#include <asm/errno.h>
#include <asm/delay.h>
#include <taos_common.h>
#include <linux/delay.h>
//iVIZM
#include <linux/irq.h> 
#include <linux/interrupt.h> 
#include <linux/slab.h>
#include <mach/gpio.h> 
#include <linux/poll.h> 
#include <linux/wakelock.h>
#include <linux/input.h>
#include <linux/miscdevice.h>

#define TSL27713_DEBUG 0
#define TSL27713_PS_NAME    "psdev"
#define TSL27713_LS_NAME    "lightdev"

// device name/id/address/counts
#define TAOS_DEVICE_NAME                "tsl27713"
#define TAOS_DEVICE_ID                  "tsl27713"
#define TAOS_ID_NAME_SIZE               10
#define TAOS_TRITON_CHIPIDVAL           0x00
#define TAOS_TRITON_MAXREGS             32
#define TAOS_DEVICE_ADDR1               0x29
#define TAOS_DEVICE_ADDR2               0x39
#define TAOS_DEVICE_ADDR3               0x49
#define TAOS_MAX_NUM_DEVICES            3
#define TAOS_MAX_DEVICE_REGS            32
#define I2C_MAX_ADAPTERS                8

// TRITON register offsets
#define TAOS_TRITON_CNTRL               0x00
#define TAOS_TRITON_ALS_TIME            0X01
#define TAOS_TRITON_PRX_TIME            0x02
#define TAOS_TRITON_WAIT_TIME           0x03
#define TAOS_TRITON_ALS_MINTHRESHLO     0X04
#define TAOS_TRITON_ALS_MINTHRESHHI     0X05
#define TAOS_TRITON_ALS_MAXTHRESHLO     0X06
#define TAOS_TRITON_ALS_MAXTHRESHHI     0X07
#define TAOS_TRITON_PRX_MINTHRESHLO     0X08
#define TAOS_TRITON_PRX_MINTHRESHHI     0X09
#define TAOS_TRITON_PRX_MAXTHRESHLO     0X0A
#define TAOS_TRITON_PRX_MAXTHRESHHI     0X0B
#define TAOS_TRITON_INTERRUPT           0x0C
#define TAOS_TRITON_PRX_CFG             0x0D
#define TAOS_TRITON_PRX_COUNT           0x0E
#define TAOS_TRITON_GAIN                0x0F
#define TAOS_TRITON_REVID               0x11
#define TAOS_TRITON_CHIPID              0x12
#define TAOS_TRITON_STATUS              0x13
#define TAOS_TRITON_ALS_CHAN0LO         0x14
#define TAOS_TRITON_ALS_CHAN0HI         0x15
#define TAOS_TRITON_ALS_CHAN1LO         0x16
#define TAOS_TRITON_ALS_CHAN1HI         0x17
#define TAOS_TRITON_PRX_LO              0x18
#define TAOS_TRITON_PRX_HI              0x19
#define TAOS_TRITON_TEST_STATUS         0x1F

// Triton cmd reg masks
#define TAOS_TRITON_CMD_REG             0X80
#define TAOS_TRITON_CMD_AUTO            0x20 //iVIZM  2011.07.21
#define TAOS_TRITON_CMD_BYTE_RW         0x00
#define TAOS_TRITON_CMD_WORD_BLK_RW     0x20
#define TAOS_TRITON_CMD_SPL_FN          0x60
#define TAOS_TRITON_CMD_PROX_INTCLR     0X05
#define TAOS_TRITON_CMD_ALS_INTCLR      0X06
#define TAOS_TRITON_CMD_PROXALS_INTCLR  0X07
#define TAOS_TRITON_CMD_TST_REG         0X08
#define TAOS_TRITON_CMD_USER_REG        0X09

// Triton cntrl reg masks
#define TAOS_TRITON_CNTL_PROX_INT_ENBL  0X20
#define TAOS_TRITON_CNTL_ALS_INT_ENBL   0X10
#define TAOS_TRITON_CNTL_WAIT_TMR_ENBL  0X08
#define TAOS_TRITON_CNTL_PROX_DET_ENBL  0X04
#define TAOS_TRITON_CNTL_ADC_ENBL       0x02
#define TAOS_TRITON_CNTL_PWRON          0x01

// Triton status reg masks
#define TAOS_TRITON_STATUS_ADCVALID     0x01
#define TAOS_TRITON_STATUS_PRXVALID     0x02
#define TAOS_TRITON_STATUS_ADCINTR      0x10
#define TAOS_TRITON_STATUS_PRXINTR      0x20

// lux constants
#define TAOS_MAX_LUX                    10000
#define TAOS_SCALE_MILLILUX             3
#define TAOS_FILTER_DEPTH               3
#define CHIP_ID                         0x3d

//#define TAOS_INPUT_NAME  "sensors_data2"


#define ALS_PS_GPIO   (17)

#define ALS_PS_INT   gpio_to_irq(ALS_PS_GPIO)

// forward declarations
static int taos_probe(struct i2c_client *clientp, const struct i2c_device_id *idp);
static int taos_remove(struct i2c_client *client);
#ifdef LCT_NO_USE
static int taos_open(struct inode *inode, struct file *file);
static int taos_release(struct inode *inode, struct file *file);
//static int  taos_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static long taos_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int taos_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static int taos_write(struct file *file, const char *buf, size_t count, loff_t *ppos);
static loff_t taos_llseek(struct file *file, loff_t offset, int orig);
static int taos_lux_filter(int raw_lux);
static void taos_prox_poll_timer_func(unsigned long param);
static void taos_prox_poll_timer_start(void);
#endif
static int taos_get_lux(void);
static int taos_device_name(unsigned char *bufp, char **device_name);
static int taos_prox_poll(struct taos_prox_info *prxp);
//iVIZM
static int taos_als_threshold_set(void);
static int taos_prox_threshold_set(void);
static int taos_als_get_data(void);
static int taos_interrupts_clear(void);
//static unsigned int taos_poll(struct file *filp, poll_table *wait);
static int taos_resume(struct device *dev);//(struct i2c_client *client);
static int taos_suspend(struct device *dev);//(struct i2c_client *client,pm_message_t mesg);
static int taos_sensors_als_on(void);

static DEFINE_MUTEX(tsl27713_mutex);	//zrr
static DECLARE_WAIT_QUEUE_HEAD(ps_waitqueue);
static DECLARE_WAIT_QUEUE_HEAD(ls_waitqueue);

/*
DECLARE_WAIT_QUEUE_HEAD(waitqueue_read);//iVIZM

static struct i2c_client *this_client;

#define ALS_PROX_DEBUG //iVIZM
static unsigned int ReadEnable = 0;//iVIZM
struct ReadData { //iVIZM
    unsigned int data;
    unsigned int interrupt;
};
struct ReadData readdata[2];//iVIZM
*/

// first device number
//static dev_t taos_dev_number;

// workqueue struct
//static struct workqueue_struct *taos_wq; //iVIZM

// class structure for this device
//struct class *taos_class;

// module device table
static struct i2c_device_id taos_idtable[] = {
    {TAOS_DEVICE_ID, 0},
    {}
};
MODULE_DEVICE_TABLE(i2c, taos_idtable);

// board and address info   iVIZM
struct i2c_board_info taos_board_info[] = {
    {I2C_BOARD_INFO(TAOS_DEVICE_ID, TAOS_DEVICE_ADDR2),},
};

unsigned short const taos_addr_list[2] = {TAOS_DEVICE_ADDR2, I2C_CLIENT_END};//iVIZM

// client and device
struct i2c_client *my_clientp;
//JJ 2 line
//struct i2c_client *bad_clientp[TAOS_MAX_NUM_DEVICES];
//static int num_bad = 0;
static int device_found = 0;
//iVIZM
static char pro_buf[4]; //iVIZM
static int mcount = 0; //iVIZM
static char als_buf[4]; //iVIZM
//u16 status = 0;
static int ALS_ON;

static int taos_ps_data = 0;
static int taos_ls_data = 0;
static int mode = -1;
enum
{
	TSL_SENSOR_INVALID_MODE = -1,
	TSL_SENSOR_NORMAL_MODE,
	TSL_SENSOR_FACTORY_MODE
};


static struct dev_pm_ops tsl27713_pm_ops = {
	.suspend = taos_suspend,
	.resume  = taos_resume,
};
// driver definition
static struct i2c_driver taos_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = TAOS_DEVICE_NAME,
        .pm		= &tsl27713_pm_ops,
    },
    .id_table = taos_idtable,
    .probe = taos_probe,
    //.resume = taos_resume,//iVIZM
    //.suspend = taos_suspend,//iVIZM
    .remove = __devexit_p(taos_remove),
};

// per-device data
struct taos_data {
    struct i2c_client *client;
	#if 0
    struct cdev cdev;
	#endif
    unsigned int addr;
    //struct input_dev *input_dev;//iVIZM
    struct work_struct work;//iVIZM
    struct wake_lock taos_wake_lock;//iVIZM
    char taos_id;
    char taos_name[TAOS_ID_NAME_SIZE];
    //struct semaphore update_lock;
    struct mutex update_lock;
    char valid;
    //int working;
    unsigned long last_updated;
} *taos_datap;

#ifdef LCT_NO_USE
// file operations
static struct file_operations taos_fops = {
    .owner = THIS_MODULE,
    .open = taos_open,
    .release = taos_release,
    .read = taos_read,
    .write = taos_write,
    .llseek = taos_llseek,
    //.ioctl = taos_ioctl,
    .unlocked_ioctl = taos_ioctl,
    .poll = taos_poll, //iVIZM
};
#endif

// device configuration
struct taos_cfg *taos_cfgp;
static u32 calibrate_target_param = 300000;
static u16 als_time_param = 200;
static u16 scale_factor_param = 1;
static u16 gain_trim_param = 512;
static u8 filter_history_param = 3;
static u8 filter_count_param = 1;
static u8 gain_param = 2;
static u16 prox_threshold_hi_param = 120;
static u16 prox_threshold_lo_param = 80;
static u16 als_threshold_hi_param = 3000;//iVIZM
static u16 als_threshold_lo_param = 10;//iVIZM
static u8 prox_int_time_param = 0xEE;//50ms
static u8 prox_adc_time_param = 0xFF;
static u8 prox_wait_time_param = 0xEE;
static u8 prox_intr_filter_param = 0x23;
static u8 prox_config_param = 0x00;
static u8 prox_pulse_cnt_param = 0x08;
static u8 prox_gain_param = 0x62;

// prox info
struct taos_prox_info prox_cal_info[20];
struct taos_prox_info prox_cur_info;
struct taos_prox_info *prox_cur_infop = &prox_cur_info;
#ifdef LCT_NO_USE
static u8 prox_history_hi = 0;
static u8 prox_history_lo = 0;
static int device_released = 0;
static struct timer_list prox_poll_timer;
#endif
static int iSetThreshold = 0;
static u16 sat_als = 0;
static u16 sat_prox = 0;
static int ps_on = 0;	//zrr: identify proximity sensor status
static int ps_data_changed =0;
static int ls_data_changed = 0;

static int ps_device_released = 0;
static int ls_device_released = 0;

// device reg init values
u8 taos_triton_reg_init[16] = {0x00,0xFF,0XFF,0XFF,0X00,0X00,0XFF,0XFF,0X00,0X00,0XFF,0XFF,0X00,0X00,0X00,0X00};

// lux time scale
struct time_scale_factor  {
    u16 numerator;
    u16 denominator;
    u16 saturation;
};
struct time_scale_factor TritonTime = {1, 0, 0};
struct time_scale_factor *lux_timep = &TritonTime;

// gain table
u8 taos_triton_gain_table[] = {1, 8, 16, 120};

// lux data
struct lux_data {
    u16 ratio;
    u16 clear;
    u16 ir;
};
struct lux_data TritonFN_lux_data[] = {
    { 9830,  8320,  15360 },
    { 12452, 10554, 22797 },
    { 14746, 6234,  11430 },
    { 17695, 3968,  6400  },
    { 0,     0,     0     }
};
struct lux_data *lux_tablep = TritonFN_lux_data;
static int lux_history[TAOS_FILTER_DEPTH] = {-ENODATA, -ENODATA, -ENODATA};//iVIZM


#if TSL27713_DEBUG

static ssize_t tsl27713_show_enable(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret, reg_val;

	mutex_lock(&taos_datap->update_lock);
	if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
		printk(KERN_ERR "TAOS: tsl27713_show_enable failed \n");
		return (ret);
	}
	reg_val = i2c_smbus_read_byte(taos_datap->client);
	mutex_unlock(&taos_datap->update_lock);

	return sprintf(buf, "%d\n", reg_val);
}

static ssize_t tsl27713_store_enable(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val = simple_strtoul(buf, NULL, 10);

	mutex_lock(&taos_datap->update_lock);
	if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), (unsigned int)val))) < 0) {
		printk(KERN_ERR "TAOS: tsl27713_store_enable failed\n");
		return (ret);
	}
	mutex_unlock(&taos_datap->update_lock);	

	if (ret < 0)
		return ret;

	return count;
}

static ssize_t tsl27713_show_status(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	int ret, status;

	mutex_lock(&taos_datap->update_lock);
	if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | 0x13)))) < 0) {
		printk(KERN_ERR "TAOS: tsl27713_show_status failed\n");
		return (ret);
	}
	status = i2c_smbus_read_byte(taos_datap->client);
	mutex_unlock(&taos_datap->update_lock);

	return sprintf(buf, "%d\n", status);
}


static ssize_t tsl27713_show_threshold_hi(struct device *dev,
			struct device_attribute *attr, char *buf)
{

	int prox_threshold_hi;

	mutex_lock(&taos_datap->update_lock);
	prox_threshold_hi = (unsigned int)taos_cfgp->prox_threshold_hi;
	mutex_unlock(&taos_datap->update_lock);
	
	return sprintf(buf, "%d\n", prox_threshold_hi);
}


static ssize_t tsl27713_show_threshold_lo(struct device *dev,
			struct device_attribute *attr, char *buf)
{

	int prox_threshold_lo;

	mutex_lock(&taos_datap->update_lock);
	prox_threshold_lo = (unsigned int)taos_cfgp->prox_threshold_lo;
	mutex_unlock(&taos_datap->update_lock);
	
	return sprintf(buf, "%d\n", prox_threshold_lo);
}


static DEVICE_ATTR(enable, S_IWUSR | S_IRUGO,
		   tsl27713_show_enable, tsl27713_store_enable);
static DEVICE_ATTR(status, S_IRUGO,
		   tsl27713_show_status, NULL);
static DEVICE_ATTR(threshold_hi, S_IRUGO,
		   tsl27713_show_threshold_hi, NULL);
static DEVICE_ATTR(threshold_lo, S_IRUGO,
		   tsl27713_show_threshold_lo, NULL);

static struct attribute *tsl27713_attributes[] = {
	&dev_attr_enable.attr,
	&dev_attr_status.attr,
	&dev_attr_threshold_lo.attr,
	&dev_attr_threshold_hi.attr,
	NULL
};

static const struct attribute_group tsl27713_attr_group = {
	.attrs = tsl27713_attributes
};

#endif


/*
* zrr@ 2012.03.06
* This function is only used for taos_ps_ioctl poll
*/
static int taos_prox_poll2(struct taos_prox_info *prxp) {
    //static int event = 0;
    //u16 status = 0;
    int i = 0, ret = 0; //wait_count = 0;
    u8 chdata[6];
	
/*	
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | 0x13)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte(1) failed in taos_prox_poll()\n");
        return (ret);
    }
    status = i2c_smbus_read_byte(taos_datap->client);
    while ((status & 0x30) != 0x30) { 
        status = i2c_smbus_read_byte(taos_datap->client);
        wait_count++;
        if (wait_count > 10) {
            printk(KERN_ERR "TAOS: Prox status invalid for 100 ms in taos_prox_poll()\n");
            return -ENODATA;
        }
        mdelay(10); 
    }
	
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|0x05)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte(2) failed in taos_prox_poll()\n");
        return (ret);
    }
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|0x06)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte(3) failed in taos_prox_poll()\n");
        return (ret);
    }
	*/
    for (i = 0; i < 6; i++) {
        chdata[i] = (i2c_smbus_read_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CMD_AUTO | (TAOS_TRITON_ALS_CHAN0LO + i))));
    }
    prxp->prox_clear = chdata[1];
    prxp->prox_clear <<= 8;
    prxp->prox_clear |= chdata[0];
    if ((TSL_SENSOR_FACTORY_MODE != mode) && (prxp->prox_clear > ((sat_als*80)/100)))
     {
			
	    	#if TSL27713_DEBUG
		printk(KERN_ERR "LCH2:prxp->prox_clear = %d,sat_als = %d\n",prxp->prox_clear,sat_als);
		#endif	
		
		return -ENODATA;
        
    	}
    prxp->prox_data = chdata[5];
    prxp->prox_data <<= 8;
    prxp->prox_data |= chdata[4];

    //prox_history_hi <<= 1;
    //prox_history_hi |= ((prxp->prox_data > taos_cfgp->prox_threshold_hi)? 1:0);
    //prox_history_hi &= 0x07;
    //prox_history_lo <<= 1;
    //prox_history_lo |= ((prxp->prox_data > taos_cfgp->prox_threshold_lo)? 1:0);
    //prox_history_lo &= 0x07;
    //if (prox_history_hi == 0x07)
    //    event = 1;
    //else {
    //    if (prox_history_lo == 0)
    //        event = 0;
    //}

#if TSL27713_DEBUG
	printk(KERN_ERR "LCH2 :prxp->prox_data = %d\n",prxp->prox_data);
#endif
	
    	mutex_lock(&tsl27713_mutex);
    if (prxp->prox_data < taos_cfgp->prox_threshold_lo ) 
	{
		taos_ps_data = 1;
	}
	else  if (prxp->prox_data > taos_cfgp->prox_threshold_hi )
	{

		//if (prxp->prox_clear> ((sat_als*80)/100))
		 if ((TSL_SENSOR_FACTORY_MODE != mode) && (prxp->prox_clear > ((sat_als*80)/100)))
		{		
			taos_ps_data = 1;
		}      
		else
		{
			taos_ps_data = 0;
		}		
	}
 	mutex_unlock(&tsl27713_mutex);	
	 		
    //prxp->prox_event = event;
    return (ret);
}
// release
static int taos_ps_release(struct inode *inode, struct file *file) {
    mutex_lock(&tsl27713_mutex);
     ps_device_released =1;
    mutex_unlock(&tsl27713_mutex);	 
    return 0;
}

static int taos_ps_open(struct inode *inode, struct file *file) {
    mutex_lock(&tsl27713_mutex);
    ps_device_released = 0;
    mutex_unlock(&tsl27713_mutex);	


    //enable_irq(ALS_PS_INT);//iVIZM
    return 0;
}

static long  taos_ps_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
	void __user *argp = (void __user *)arg;
	int prox_sum = 0, prox_mean = 0, prox_max = 0;
	int ret = 0, i = 0;
	u8 reg_val = 0, reg_cntrl = 0;
	int rwbuf=0;



    switch (cmd)
    {
    	case TAOS_IOCTL_SET_FACTORY_MODE:		
		  mode = TSL_SENSOR_FACTORY_MODE;		
		  break;
    	case TAOS_IOCTL_ENTER_FACTORY_MODE:			
		if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x00), 0x0f))) < 0) {
	                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl factory mode\n");
	                return (ret);
              }

		if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x01), 0xb7))) < 0) {
	                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl factory mode\n");
	                return (ret);
              }

		if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x02), 0xff))) < 0) {
	                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl factory mode\n");
	                return (ret);
              }				

		if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x03), 0xee))) < 0) {
	                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl factory mode\n");
	                return (ret);
              }

		if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0e), 0x08))) < 0) {
			printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl factory mode\n");
			return (ret);
		}

		if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0f), 0x62))) < 0) {
	                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl factory mode\n");
	                return (ret);
              }
		
		break;			
    	case TAOS_IOCTL_PROX_CALIBRATE:
            //if (prox_on) golden
            //del_timer(&prox_poll_timer);
            //else {
            //printk(KERN_ERR "TAOS: ioctl prox_calibrate was called before ioctl prox_on was called\n");
            //return -EPERM;
            //}
            	#if TSL27713_DEBUG
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            printk("^^^^^^^^^ TAOS IOCTL CALIBRATE  ^^^^^^^\n");
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		#endif
	      
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x01), 0xFF/*taos_cfgp->prox_int_time*/))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x02), taos_cfgp->prox_adc_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x03), 0xFF/*taos_cfgp->prox_wait_time*/))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }

            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0D), taos_cfgp->prox_config))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }

            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0E), taos_cfgp->prox_pulse_cnt))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0F), taos_cfgp->prox_gain))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            //if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
            //    printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl prox_on\n");
            //    return (ret);
            //}
            //reg_val = i2c_smbus_read_byte(taos_datap->client);
            reg_cntrl = reg_val | (TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_ADC_ENBL);
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }

            prox_sum = 0;
            prox_max = 0;
#if TSL27713_DEBUG	
  	printk("%s %d => %s", __FILE__, __LINE__, __func__);
#endif
            for (i = 0; i < 20; i++) {
                if ((taos_prox_poll(&prox_cal_info[i])) < 0) {
                    printk(KERN_ERR "TAOS: call to prox_poll failed in ioctl prox_calibrate\n");
                    return EINVAL;
                }
                prox_sum += prox_cal_info[i].prox_data;
                if (prox_cal_info[i].prox_data > prox_max)
                    prox_max = prox_cal_info[i].prox_data;
                mdelay(100);
            }
            prox_mean = prox_sum/20;
	     if(prox_mean <= 100)
	     {
	     		taos_cfgp->prox_threshold_hi = prox_mean* 22/10;
            		taos_cfgp->prox_threshold_lo = prox_mean* 19/10;			
	     }
	     else if((100 < prox_mean) && (prox_mean <= 300))
	     {
			taos_cfgp->prox_threshold_hi =  prox_mean* 2;
	              taos_cfgp->prox_threshold_lo = prox_mean* 17/10 ;

	     }
	     else
	     {
			taos_cfgp->prox_threshold_hi = prox_mean * 18/10;
	              taos_cfgp->prox_threshold_lo =  prox_mean* 15/10;
	     }

	    if(taos_cfgp->prox_threshold_hi > 450 || taos_cfgp->prox_threshold_lo < 120)
	    {
			taos_cfgp->prox_threshold_hi = 450;
			taos_cfgp->prox_threshold_lo = 350;					
	     }
							

	
            //taos_cfgp->prox_threshold_hi = ((((prox_max - prox_mean) * 200) + 50)/100) + prox_mean;
            //taos_cfgp->prox_threshold_lo = ((((prox_max - prox_mean) * 170) + 50)/100) + prox_mean;
            #if TSL27713_DEBUG			
             printk("TAOS:------------ taos_cfgp->prox_threshold_hi = %d\n",taos_cfgp->prox_threshold_hi );
             printk("TAOS:------------ taos_cfgp->prox_threshold_lo = %d\n",taos_cfgp->prox_threshold_lo );
		#endif
           // if (taos_cfgp->prox_threshold_lo < ((sat_prox*12)/100)) {
           //     taos_cfgp->prox_threshold_lo = ((sat_prox*12)/100);
           //     taos_cfgp->prox_threshold_hi = ((sat_prox*15)/100);
           // }
            for (i = 0; i < sizeof(taos_triton_reg_init); i++){
                if(i !=11){
                    if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|(TAOS_TRITON_CNTRL +i)), taos_triton_reg_init[i]))) < 0) {
                        printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                        return (ret);
                    }
                 }
             }

            //taos_prox_poll_timer_start();//Rambo
            break;
	 case TAOS_IOCTL_PROX_ON:
		#if TSL27713_DEBUG
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            printk("^^^^^^^^^ TAOS IOCTL PROX ON  ^^^^^^^\n");
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		#endif
            iSetThreshold = 1;
	     mode = TSL_SENSOR_NORMAL_MODE;
		 
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x01), taos_cfgp->prox_int_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x02), taos_cfgp->prox_adc_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x03), taos_cfgp->prox_wait_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }

            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0C), taos_cfgp->prox_intr_filter))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0D), taos_cfgp->prox_config))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0E), taos_cfgp->prox_pulse_cnt))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0F), taos_cfgp->prox_gain))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            //if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
            //    printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl prox_on\n");
            //    return (ret);
            //}
            //reg_val = i2c_smbus_read_byte(taos_datap->client);
            //reg_cntrl = reg_val | (TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_PROX_INT_ENBL | TAOS_TRITON_CNTL_ADC_ENBL);
            reg_cntrl = TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_PROX_INT_ENBL | 
				                                      TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_WAIT_TMR_ENBL  ;
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            taos_prox_threshold_set();//iVIZM

	     set_irq_wake(gpio_to_irq(ALS_PS_GPIO), 1);	
	     ps_on = 1;

	     if(!wake_lock_active(&taos_datap->taos_wake_lock)){
			wake_lock(&taos_datap->taos_wake_lock);
		}	

			
            break;
        case TAOS_IOCTL_PROX_OFF:
		#if TSL27713_DEBUG
		printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		printk("^^^^^^^^^ TAOS IOCTL PROX OFF  ^^^^^^^\n");
		printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		#endif

		iSetThreshold = 0;	 
		mode = TSL_SENSOR_NORMAL_MODE;	
		set_irq_wake(gpio_to_irq(ALS_PS_GPIO), 0);
		ps_on = 0;	

		if(wake_lock_active(&taos_datap->taos_wake_lock)){
			wake_unlock(&taos_datap->taos_wake_lock);
		}
		 
		if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), 0x00))) < 0) {
			printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_off\n");
			return (ret);
		}
		if (ALS_ON == 1) {
			taos_sensors_als_on();
		} else {
			cancel_work_sync(&taos_datap->work);//golden
		}

		break;

	case TAOS_IOCTL_PROX_READ_DATA:			
			if((TSL_SENSOR_FACTORY_MODE == mode) && 
				((ret = taos_prox_poll2(prox_cur_infop)) < 0))			
			{
			        printk(KERN_ERR "TAOS: call to prox_poll failed in taos_ps_ioctl()\n");
			        return ret;
			 }
			mutex_lock(&tsl27713_mutex);
			rwbuf = taos_ps_data; 
			mutex_unlock(&tsl27713_mutex);
			if(copy_to_user(argp, &rwbuf, 1))
				ret = -EFAULT;
			break;	

	default:
		break;
     }

	return ret;
}



//poll iVIZM
static unsigned int taos_ps_poll(struct file *filp, poll_table *wait)
{
	mutex_lock(&tsl27713_mutex);
	if(TSL_SENSOR_FACTORY_MODE == mode)
	{
		mutex_unlock(&tsl27713_mutex);
		return POLLIN | POLLRDNORM;
	}
		
	if(ps_data_changed) {
		ps_data_changed = 0;
		mutex_unlock(&tsl27713_mutex);
		return POLLIN | POLLRDNORM;
	} 
	poll_wait(filp, &ps_waitqueue, wait);
	mutex_unlock(&tsl27713_mutex);
	return 0;
	
}


// release
static int taos_ls_release(struct inode *inode, struct file *file) {
    mutex_lock(&tsl27713_mutex);
     ls_device_released =1;
    mutex_unlock(&tsl27713_mutex);

    return 0;
}

static int taos_ls_open(struct inode *inode, struct file *file) {
    mutex_lock(&tsl27713_mutex);
    ls_device_released = 0;
    mutex_unlock(&tsl27713_mutex);
      
    enable_irq(ALS_PS_INT);//iVIZM
    return 0;
}



static long  taos_ls_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    void __user *argp = (void __user *)arg;
     u8 reg_val = 0;	
     int  ret = 0,i=0;
     int rwbuf = 0;
	 
	switch(cmd)
	{
		 case TAOS_IOCTL_ALS_ON:
		 	#if TSL27713_DEBUG
	            printk("####################################\n");
	            printk("######## TAOS IOCTL ALS ON #########\n");
	            printk("####################################\n");
			#endif

			/*
	            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
	                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als on\n");
	                return (ret);
	            }

			reg_val = i2c_smbus_read_byte(taos_datap->client);


			
			if ((reg_val & TAOS_TRITON_CNTL_PROX_DET_ENBL) == 0x0) {
			*/
			if(0 == ps_on){
					mode = 	TSL_SENSOR_NORMAL_MODE;					
					taos_sensors_als_on();
			}	

			ALS_ON = 1;	  	
	            return (ret);
	            break;
	        case TAOS_IOCTL_ALS_OFF:
				#if TSL27713_DEBUG
	            printk("#####################################\n");
	            printk("######## TAOS IOCTL ALS OFF #########\n");
	            printk("#####################################\n");
				#endif
	            for (i = 0; i < TAOS_FILTER_DEPTH; i++)
	                lux_history[i] = -ENODATA;
	            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
	                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als off\n");
	                return (ret);
	            }
	            reg_val = i2c_smbus_read_byte(taos_datap->client);
	            if ((reg_val & TAOS_TRITON_CNTL_PROX_DET_ENBL) == 0x0) {
	               if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), 0x00))) < 0) {
	                   printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_off\n");
	                   return (ret);
				   }
				   cancel_work_sync(&taos_datap->work);//golden	   
	            }
				ALS_ON = 0;
	            return (ret);
	            break;
		case TAOS_IOCTL_ALS_READ_DATA:			
			if(TSL_SENSOR_FACTORY_MODE == mode)
			{
				rwbuf = taos_get_lux();
			}
			else
			{
				mutex_lock(&tsl27713_mutex);
				rwbuf = taos_ls_data;
				mutex_unlock(&tsl27713_mutex);
			}
			if(copy_to_user(argp, &rwbuf, 1))
				ret = -EFAULT;
			break;				
		default:
			break;
			
	}

	return ret;
}


//poll iVIZM
static unsigned int taos_ls_poll(struct file *filp, poll_table *wait)
{
	mutex_lock(&tsl27713_mutex);

	if(TSL_SENSOR_FACTORY_MODE == mode)
	{
		mutex_unlock(&tsl27713_mutex);
		return POLLIN | POLLRDNORM;
	}
	
	if(ls_data_changed) {
		ls_data_changed = 0;
		mutex_unlock(&tsl27713_mutex);
		return POLLIN | POLLRDNORM;
	} 
	poll_wait(filp, &ls_waitqueue, wait);
	mutex_unlock(&tsl27713_mutex);
	return 0;

}


// file operations
static struct file_operations taos_ps_fops = {
    .owner = THIS_MODULE,
    .open = taos_ps_open,
    .release = taos_ps_release,
    .unlocked_ioctl = taos_ps_ioctl,
    .poll = taos_ps_poll, //iVIZM
};


// file operations
static struct file_operations taos_ls_fops = {
    .owner = THIS_MODULE,
    .open = taos_ls_open,
    .release = taos_ls_release,
    .unlocked_ioctl = taos_ls_ioctl,
    .poll = taos_ls_poll, //iVIZM
};


static struct miscdevice tsl27713_ps_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = TSL27713_PS_NAME,
	.fops   = &taos_ps_fops,
};


static struct miscdevice tsl27713_ls_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = TSL27713_LS_NAME,
	.fops   = &taos_ls_fops,
};


static irqreturn_t taos_irq_handler(int irq, void *dev_id) //iVIZM
{
    //printk("TAOS:-- taos_irq_handler \n");
    schedule_work(&taos_datap->work);
    return IRQ_HANDLED;
}

static int taos_get_data(void)//iVIZM
{
    int ret = 0;
    u16 status = 0;
	
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | 0x13)))) < 0) {
        //printk(KERN_ERR "TAOS: i2c_smbus_write_byte(1) failed in taos_work_func()\n");  //zrr: always failed if system in  sleep mode
        return (ret);
    }
    status = i2c_smbus_read_byte(taos_datap->client);
  //  if (down_interruptible(&taos_datap->update_lock))
       // return -ERESTARTSYS;
       

     if(0x13 == status)
	 	return 0;
  
    if((status & 0x20) == 0x20) {		
		taos_prox_threshold_set();
		/*	
		ret = taos_prox_threshold_set();
		if(ret >= 0)
		ReadEnable = 1;
		*/
		wake_up_interruptible(&ps_waitqueue);				
    }
    else if((status & 0x10) == 0x10) {
        //ReadEnable = 1;		
        taos_als_threshold_set();
        taos_als_get_data();
	 wake_up_interruptible(&ls_waitqueue);		
    }
    //up(&taos_datap->update_lock);
    return ret;
}

static int taos_interrupts_clear(void)//iVIZM
{
    int ret = 0;
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|0x07)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte(2) failed in taos_work_func()\n");
        return (ret);
    }
    return ret;
}

static void taos_work_func(struct work_struct * work) //iVIZM
{  

	if(!ps_on && !wake_lock_active(&taos_datap->taos_wake_lock)){
		wake_lock(&taos_datap->taos_wake_lock);			 	
	}

	taos_get_data();
	taos_interrupts_clear();


	if(!ps_on && wake_lock_active(&taos_datap->taos_wake_lock)){
		wake_unlock(&taos_datap->taos_wake_lock);			 	
	}
}

static int taos_als_get_data(void)//iVIZM
{
    int ret = 0;
    u8 reg_val;
    int lux_val = 0;
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_data\n");
        return (ret);
    }
    reg_val = i2c_smbus_read_byte(taos_datap->client);
    if ((reg_val & (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON)) != (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON))
        return -ENODATA;
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_STATUS)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_data\n");
        return (ret);
    }
    reg_val = i2c_smbus_read_byte(taos_datap->client);
    if ((reg_val & TAOS_TRITON_STATUS_ADCVALID) != TAOS_TRITON_STATUS_ADCVALID)
        return -ENODATA;
    if ((lux_val = taos_get_lux()) < 0)
        printk(KERN_ERR "TAOS: call to taos_get_lux() returned error %d in ioctl als_data\n", lux_val);
    //lux_val = taos_lux_filter(lux_val);
    #if TSL27713_DEBUG
    printk("********** before taos_als_get_data ********* lux_val = %d \n",lux_val);
     #endif
	//lux_val |= (1 << 16);
	//input_event(taos_datap->input_dev,EV_MSC,MSC_RAW,lux_val);
	taos_ls_data = lux_val;
	mutex_lock(&tsl27713_mutex);
	ls_data_changed = 1;
	mutex_unlock(&tsl27713_mutex);
	#ifdef LCT_NO_USE
	input_report_abs(taos_datap->input_dev,ABS_MISC,lux_val);
	input_sync(taos_datap->input_dev); 
	#endif
	//        input_event(taos_datap->input_dev,EV_ABS,ABS_MISC,lux_val);
    //readdata[0].data = lux_val;
    //readdata[0].interrupt = HAVE_INTERRUPT;
    #if TSL27713_DEBUG
    printk("********** taos_als_get_data ********* lux_val = %d \n",lux_val);
	#endif
    return ret;
}

static int taos_als_threshold_set(void)//iVIZM
{
    int i,ret = 0;
    u8 chdata[2];
    u16 ch0;
#if TSL27713_DEBUG
  	printk("JJ taos_als_threshold_set %s %d => %s", __FILE__, __LINE__, __func__);
#endif
    for (i = 0; i < 2; i++) {
        chdata[i] = (i2c_smbus_read_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CMD_WORD_BLK_RW | (TAOS_TRITON_ALS_CHAN0LO + i))));
    }
    ch0 = chdata[0] + chdata[1]*256;
    als_threshold_hi_param = (12*ch0)/10;
	if (als_threshold_hi_param >= 65535)
        als_threshold_hi_param = 65535;	 
    als_threshold_lo_param = (8*ch0)/10;
    als_buf[0] = als_threshold_lo_param & 0x0ff;
    als_buf[1] = als_threshold_lo_param >> 8;
    als_buf[2] = als_threshold_hi_param & 0x0ff;
    als_buf[3] = als_threshold_hi_param >> 8;

    for( mcount=0; mcount<4; mcount++ ) { 
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x04) + mcount, als_buf[mcount]))) < 0) {
             printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in taos als threshold set\n");
             return (ret);
        }
		#if TSL27713_DEBUG
  	printk("JJ taos_als_threshold_set %s %d => %s", __FILE__, __LINE__, __func__);
		#endif
    }
  	//printk("JJ taos_als_threshold_set %s %d => %s", __FILE__, __LINE__, __func__);
    return ret;
}

static int taos_prox_threshold_set(void)//iVIZM
{
    int i,ret = 0;
    u8 chdata[6];
    u16 proxdata = 0;
    u16 cleardata = 0;
	//int data = 0;

    for (i = 0; i < 6; i++) {
        chdata[i] = (i2c_smbus_read_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CMD_WORD_BLK_RW| (TAOS_TRITON_ALS_CHAN0LO + i))));
    }
    cleardata = chdata[0] + chdata[1]*256;
    proxdata = chdata[4] + chdata[5]*256;
	#if TSL27713_DEBUG
    printk("================= proxdata = %d\n",proxdata);
    printk("================= taos_cfgp= %d\n",(unsigned int)taos_cfgp);
    printk("================= taos_datap= %d\n",(unsigned int)taos_datap);		
    printk("================= taos_cfgp->prox_threshold_lo= %d\n",(unsigned int)taos_cfgp->prox_threshold_lo);		
    printk("================= taos_dattaos_cfgp->prox_threshold_hi= %d\n",(unsigned int)taos_cfgp->prox_threshold_hi);			
	#endif
    //printk("================= taos_datap->input_dev= %d\n",(unsigned int)taos_datap->input_dev);
    //printk("================= taos_datap->input_dev->name= %s\n",taos_datap->input_dev->name);
    if (iSetThreshold || proxdata < taos_cfgp->prox_threshold_lo ) {
        pro_buf[0] = 0x0;
        pro_buf[1] = 0x0;
        pro_buf[2] = taos_cfgp->prox_threshold_hi & 0x0ff;
        pro_buf[3] = taos_cfgp->prox_threshold_hi >> 8;
		//pro_buf[2] = 0x2c;
        //pro_buf[3] = 0x01;
    //printk("================= proxdata 1 = %d\n",__LINE__);
        //if (prox_on == 0) {
			//data = 1;
			taos_ps_data = 1;
			//data |= (2 << 16);
	        //input_event(taos_datap->input_dev,EV_MSC,MSC_RAW,data);
	       // input_event(taos_datap->input_dev,EV_MSC,MSC_RAW,data);
	       #if 0 //zrr
	 input_report_abs(taos_datap->input_dev,ABS_DISTANCE,data);
	 input_sync(taos_datap->input_dev); 
	 #endif
            //readdata[1].data = 0;
            //readdata[1].interrupt = HAVE_INTERRUPT;
    //printk("================= proxdata 11_GREAT_4 = %d\n",__LINE__);
       // }
    } else if (proxdata > taos_cfgp->prox_threshold_hi ){
        if (cleardata > ((sat_als*80)/100))
	{
			//data=1;
			taos_ps_data = 1;
	}      
      else
	{
			pro_buf[0] = taos_cfgp->prox_threshold_lo & 0x0ff;
			pro_buf[1] = taos_cfgp->prox_threshold_lo >> 8;
			pro_buf[2] = 0xff;
			pro_buf[3] = 0xff;
			//printk("================= proxdata 2 = %d\n",__LINE__);
			
			//data = 0;
			taos_ps_data = 0;
			//data |= (2 << 16);
			//input_event(taos_datap->input_dev,EV_MSC,MSC_RAW,data);
			#if 0 //zrr
			input_report_abs(taos_datap->input_dev,ABS_DISTANCE,data);
			input_sync(taos_datap->input_dev); 
			#endif
			//readdata[1].data = 1;
			//readdata[1].interrupt = HAVE_INTERRUPT;
	}
    }

    for( mcount=0; mcount<4; mcount++ ) { 
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x08) + mcount, pro_buf[mcount]))) < 0) {
             printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in taos prox threshold set\n");
             return (ret);
        }
    }

   mutex_lock(&tsl27713_mutex);
   ps_data_changed = 1;
   mutex_unlock(&tsl27713_mutex);	
   
   iSetThreshold = 0;
    return ret;
}

// driver init
static int __init taos_init(void) {
    int ret = 0;
    //struct i2c_adapter *my_adap;   

#if 1
	taos_datap = kmalloc(sizeof(struct taos_data), GFP_KERNEL);
    if (!taos_datap) {
        printk(KERN_ERR "TAOS: kmalloc for struct taos_data failed in taos_init()\n");
        return -ENOMEM;
    }
	memset(taos_datap, 0, sizeof(struct taos_data));
#else

    if ((ret = (alloc_chrdev_region(&taos_dev_number, 0, TAOS_MAX_NUM_DEVICES, TAOS_DEVICE_NAME))) < 0) {
        printk(KERN_ERR "TAOS: alloc_chrdev_region() failed in taos_init()\n");
        return (ret);
    }
    taos_class = class_create(THIS_MODULE, TAOS_DEVICE_NAME);
    taos_datap = kmalloc(sizeof(struct taos_data), GFP_KERNEL);
    if (!taos_datap) {
        printk(KERN_ERR "TAOS: kmalloc for struct taos_data failed in taos_init()\n");
        return -ENOMEM;
    }
    memset(taos_datap, 0, sizeof(struct taos_data));
    cdev_init(&taos_datap->cdev, &taos_fops);
    taos_datap->cdev.owner = THIS_MODULE;
    if ((ret = (cdev_add(&taos_datap->cdev, taos_dev_number, 1))) < 0) {
        printk(KERN_ERR "TAOS: cdev_add() failed in taos_init()\n");
        return (ret);
    }
    //taos_wq = create_workqueue("taos_wq");//iVIZM
    //if (!taos_wq) {
    //    printk("create workqueue failed\n");
    //    return -ENOMEM;
    //}
    wake_lock_init(&taos_datap->taos_wake_lock, WAKE_LOCK_SUSPEND, "taos-wake-lock");
    device_create(taos_class, NULL, MKDEV(MAJOR(taos_dev_number), 0), &taos_driver ,"tsl27713");
   #endif
    if ((ret = (i2c_add_driver(&taos_driver))) < 0) {
        printk(KERN_ERR "TAOS: i2c_add_driver() failed in taos_init()\n");
        return (ret);
    }
/*
    for (i = 0; i < I2C_MAX_ADAPTERS; i++) {
        my_adap = i2c_get_adapter(i);
        if (my_adap == NULL) 
            break;
        my_clientp = i2c_new_probed_device(my_adap, &taos_board_info[0], taos_addr_list);
        if ((my_clientp) && (!device_found)) {
            bad_clientp[num_bad] = my_clientp;
            num_bad++;
        }
        if (device_found) 
            break;
        if (num_bad) {
            for (k = 0; k < num_bad; k++)
                i2c_unregister_device(bad_clientp[k]);
            num_bad = 0;
        }
        if (device_found) 
            break;
    }
	*/
    return (ret);
}

// driver exit
static void __exit taos_exit(void) {
    if (my_clientp)
        i2c_unregister_device(my_clientp);
    i2c_del_driver(&taos_driver);
   // unregister_chrdev_region(taos_dev_number, TAOS_MAX_NUM_DEVICES);
    //device_destroy(taos_class, MKDEV(MAJOR(taos_dev_number), 0));
    //cdev_del(&taos_datap->cdev);
    //class_destroy(taos_class);
    disable_irq(ALS_PS_INT);//iVIZM
    //if (taos_wq)
    //    destroy_workqueue(taos_wq);//iVIZM
    kfree(taos_datap);
}


// client probe
static int taos_probe(struct i2c_client *clientp, const struct i2c_device_id *idp) {
    int ret = 0;
    int i = 0;
    int chip_id;
    unsigned char buf[TAOS_MAX_DEVICE_REGS];
    char *device_name;

    //this_client = clientp;

    chip_id = i2c_smbus_read_byte_data(clientp, (TAOS_TRITON_CMD_REG | (TAOS_TRITON_CNTRL + 0x12))); //iVIZM
    if((chip_id & 0xff) != 0x09) {
        printk(" error chip_id = %d\n",chip_id);      //TSL27711=0x00 TSL27713=0x09 TMD27711=0x20 TMD27713=0x29     2011.07.21                                              
        return -ENODEV;
    }
	
    if (device_found)
        return -ENODEV;
    if (!i2c_check_functionality(clientp->adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
        printk(KERN_ERR "TAOS: taos_probe() - i2c smbus byte data functions unsupported\n");
        return -EOPNOTSUPP;
    }
    if (!i2c_check_functionality(clientp->adapter, I2C_FUNC_SMBUS_WORD_DATA)) {
        printk(KERN_ERR "TAOS: taos_probe() - i2c smbus word data functions unsupported\n");
    }
    if (!i2c_check_functionality(clientp->adapter, I2C_FUNC_SMBUS_BLOCK_DATA)) {
        printk(KERN_ERR "TAOS: taos_probe() - i2c smbus block data functions unsupported\n");
    }
    taos_datap->client = clientp;
    i2c_set_clientdata(clientp, taos_datap);	
    INIT_WORK(&(taos_datap->work),taos_work_func);
	mutex_init(&taos_datap->update_lock);
    //init_MUTEX(&taos_datap->update_lock);
    //sema_init(&taos_datap->update_lock,1);
   // ret = gpio_request(ALS_PS_GPIO,"ALS_PS_INT");//golden
   // if (ret < 0) {
   //     printk("failed to request GPIO:%d,ERRNO:%d\n",(int)ALS_PS_GPIO,ret);
   //    return(ret);
   // }
   // gpio_direction_input(ALS_PS_GPIO);//golden
   // ret = request_irq(ALS_PS_INT,taos_irq_handler, IRQ_TYPE_EDGE_FALLING, "taos_irq",taos_datap);//golden
   // if (ret != 0) {
   //     gpio_free(ALS_PS_GPIO);
   //     return(ret);
   // }
   // disable_irq(ALS_PS_INT);//golden
   #if 0
   taos_datap->input_dev = input_allocate_device();//iVIZM
   input_set_drvdata(taos_datap->input_dev, taos_datap);
   if (taos_datap->input_dev == NULL) {
       return -ENOMEM;
   }
   
   taos_datap->input_dev->name = TAOS_INPUT_NAME;
   taos_datap->input_dev->id.bustype = BUS_I2C;
   set_bit(EV_ABS,taos_datap->input_dev->evbit);
   input_set_capability(taos_datap->input_dev,EV_ABS,ABS_DISTANCE);
   input_set_abs_params(taos_datap->input_dev, ABS_DISTANCE, 0, 1, 0, 0);
   input_set_capability(taos_datap->input_dev,EV_ABS,ABS_MISC);
   ret = input_register_device(taos_datap->input_dev);
   #endif

	wake_lock_init(&taos_datap->taos_wake_lock, WAKE_LOCK_SUSPEND, "taos-wake-lock");

    for (i = 0; i < TAOS_MAX_DEVICE_REGS; i++) {
        if ((ret = (i2c_smbus_write_byte(clientp, (TAOS_TRITON_CMD_REG | (TAOS_TRITON_CNTRL + i))))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte() to control reg failed in taos_probe()\n");
            return(ret);
        }
        buf[i] = i2c_smbus_read_byte(clientp);	 
    }	 
	
    if ((ret = taos_device_name(buf, &device_name)) == 0) {
        printk(KERN_ERR "TAOS: chip id that was read found mismatched by taos_device_name(), in taos_probe()\n");
        return -ENODEV;
    }
    if (strcmp(device_name, TAOS_DEVICE_ID)) {
        printk(KERN_ERR "TAOS: chip id that was read does not match expected id in taos_probe()\n");
        return -ENODEV;
    }
    else {
        printk(KERN_ERR "TAOS: chip id of %s that was read matches expected id in taos_probe()\n", device_name);
        device_found = 1;
    }
    if ((ret = (i2c_smbus_write_byte(clientp, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte() to control reg failed in taos_probe()\n");
        return(ret);
    }
	
    strlcpy(clientp->name, TAOS_DEVICE_ID, I2C_NAME_SIZE);
    strlcpy(taos_datap->taos_name, TAOS_DEVICE_ID, TAOS_ID_NAME_SIZE);
    taos_datap->valid = 0;
    if (!(taos_cfgp = kmalloc(sizeof(struct taos_cfg), GFP_KERNEL))) {
        printk(KERN_ERR "TAOS: kmalloc for struct taos_cfg failed in taos_probe()\n");
        return -ENOMEM;
    }
	 
    taos_cfgp->calibrate_target = calibrate_target_param;
    taos_cfgp->als_time = als_time_param;
    taos_cfgp->scale_factor = scale_factor_param;
    taos_cfgp->gain_trim = gain_trim_param;
    taos_cfgp->filter_history = filter_history_param;
    taos_cfgp->filter_count = filter_count_param;
    taos_cfgp->gain = gain_param;
    taos_cfgp->als_threshold_hi = als_threshold_hi_param;//iVIZM
    taos_cfgp->als_threshold_lo = als_threshold_lo_param;//iVIZM
    taos_cfgp->prox_threshold_hi = prox_threshold_hi_param;
    taos_cfgp->prox_threshold_lo = prox_threshold_lo_param;
    taos_cfgp->prox_int_time = prox_int_time_param;
    taos_cfgp->prox_adc_time = prox_adc_time_param;
    taos_cfgp->prox_wait_time = prox_wait_time_param;
    taos_cfgp->prox_intr_filter = prox_intr_filter_param;
    taos_cfgp->prox_config = prox_config_param;
    taos_cfgp->prox_pulse_cnt = prox_pulse_cnt_param;
    taos_cfgp->prox_gain = prox_gain_param;
    sat_als = (256 - taos_cfgp->prox_int_time) << 10;
    sat_prox = (256 - taos_cfgp->prox_adc_time) << 10;

    /*dmobile ::power down for init ,Rambo liu*/
    printk("Rambo::light sensor will pwr down \n");
    if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x00), 0x00))) < 0) {
        printk(KERN_ERR "TAOS:Rambo, i2c_smbus_write_byte_data failed in power down\n");
        return (ret);
    }

  /*
   ret = gpio_request(ALS_PS_GPIO,"tsl27713_irq");//iVIZM
    if (ret < 0) {
        printk("failed to request GPIO:%d,ERRNO:%d\n",(int)ALS_PS_GPIO,ret);
       return(ret);
    }
    
 
	 printk(KERN_ERR "taos_probe() ---4\n");
    gpio_direction_input(ALS_PS_GPIO);//iVIZM
    */  

	ret = misc_register(&tsl27713_ps_dev);
	if (ret) {
		printk(KERN_ERR "LCT: taos_probe(),tsl27713  ps register misc failed\n");
		return ret;
	}

	ret = misc_register(&tsl27713_ls_dev);
	if (ret) {
		printk(KERN_ERR "LCT:taos_probe(),tsl27713 ls register misc failed\n");
		misc_deregister(&tsl27713_ps_dev);
		return ret;
	}

#if TSL27713_DEBUG
	/* Register taos sysfs hooks */
	ret = sysfs_create_group(&clientp->dev.kobj, &tsl27713_attr_group);
	if (ret)
	{
		pr_err("[%s]Sysfs create failed!\n",__func__);
		goto taos_exit;
	}
#endif
    ret = request_irq(ALS_PS_INT,taos_irq_handler, IRQ_TYPE_EDGE_FALLING, "tsl27713_irq",taos_datap);//iVIZM
    if (ret != 0) {
        gpio_free(ALS_PS_GPIO);
        return(ret);
    }
    disable_irq(ALS_PS_INT);//iVIZM	

    return 0;


#if TSL27713_DEBUG
taos_exit:
	kfree(taos_datap);
#endif

	
    return (ret);
}
#ifdef CONFIG_PM
//resume  iVIZM
//static int taos_resume(struct i2c_client *client) {
static int taos_resume(struct device *dev) {

   #if TSL27713_DEBUG
    printk(KERN_ERR "TAOS:--taos_resume\n");
   #endif
   
    if(ps_on)
    {	
    		int status =0;
		if ((i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | 0x13))) < 0) {		
		return 0;
		}
		status = i2c_smbus_read_byte(taos_datap->client);

		if(0x13  == status)
			return 0;
		
		mutex_lock(&tsl27713_mutex);
		ps_data_changed = 1;
		mutex_unlock(&tsl27713_mutex);

		taos_ps_data = 1;	

		schedule_work(&taos_datap->work);  			  		
    }
   
    return 0;
}

//suspend  iVIZM
//static int taos_suspend(struct i2c_client *client, pm_message_t mesg) {
static int taos_suspend(struct device *dev) {
    #if TSL27713_DEBUG
    printk(KERN_ERR "TAOS:--taos_suspend\n");
    #endif
	
    return 0;
}

#else

#define taos_suspend		NULL
#define taos_resume		NULL

#endif 
// client remove
static int __devexit taos_remove(struct i2c_client *client) {
	 free_irq(gpio_to_irq(ALS_PS_GPIO), client);
	
#if TSL27713_DEBUG
	sysfs_remove_group(&client->dev.kobj, &tsl27713_attr_group);
#endif

	misc_deregister(&tsl27713_ps_dev);
	misc_deregister(&tsl27713_ls_dev);



	return 0;
}

#ifdef LCT_NO_USE
// open
static int taos_open(struct inode *inode, struct file *file) {
    //struct taos_data *taos_datap;
    int ret = 0;

    device_released = 0;
    taos_datap = container_of(inode->i_cdev, struct taos_data, cdev);
	
    if (strcmp(taos_datap->taos_name, TAOS_DEVICE_ID) != 0) {
        printk(KERN_ERR "TAOS: device name incorrect during taos_open(), get %s\n", taos_datap->taos_name);
        ret = -ENODEV;
    }
    printk(KERN_ERR "taos_open()");
    memset(readdata, 0, sizeof(struct ReadData)*2);//iVIZM
    //enable_irq(ALS_PS_INT);//iVIZM
    return (ret);
}

// release
static int taos_release(struct inode *inode, struct file *file) {
   // struct taos_data *taos_datap;
    int ret = 0;

    device_released = 1;
    prox_on = 0;
    prox_history_hi = 0;
    prox_history_lo = 0;
    taos_datap = container_of(inode->i_cdev, struct taos_data, cdev);

    if (strcmp(taos_datap->taos_name, TAOS_DEVICE_ID) != 0) {
        printk(KERN_ERR "TAOS: device name incorrect during taos_release(), get %s\n", taos_datap->taos_name);
        ret = -ENODEV;
    }

    return (ret);
}

// read
static int taos_read(struct file *file, char *buf, size_t count, loff_t *ppos) {
    unsigned long flags;
    int realmax;
    int err;
    if((!ReadEnable) && (file->f_flags & O_NONBLOCK))
        return -EAGAIN;
    local_save_flags(flags);
    local_irq_disable();

    realmax = 0;
    /*JJTest if (down_interruptible(&taos_datap->update_lock))
        return -ERESTARTSYS;*/
    if (ReadEnable > 0) {
        if (sizeof(struct ReadData)*2 < count)
            realmax = sizeof(struct ReadData)*2;
        else
            realmax = count;
        err = copy_to_user(buf, readdata, realmax);
        if (err) 
            return -EAGAIN;
        ReadEnable = 0;
    }
    /*JJTest up(&taos_datap->update_lock);*/
    memset(readdata, 0, sizeof(struct ReadData)*2);
    local_irq_restore(flags);
    return realmax;
}

// write
static int taos_write(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    struct taos_data *taos_datap;
    u8 i = 0, xfrd = 0, reg = 0;
    u8 my_buf[TAOS_MAX_DEVICE_REGS];
    int ret = 0;

    //if (prox_on) golden
    //del_timer(&prox_poll_timer);
    if ((*ppos < 0) || (*ppos >= TAOS_MAX_DEVICE_REGS) || ((*ppos + count) > TAOS_MAX_DEVICE_REGS)) {
        printk(KERN_ERR "TAOS: reg limit check failed in taos_write()\n");
        return -EINVAL;
    }
    reg = (u8)*ppos;
    if ((ret =  copy_from_user(my_buf, buf, count))) {
        printk(KERN_ERR "TAOS: copy_to_user failed in taos_write()\n");
        return -ENODATA;
    }

    taos_datap = container_of(file->f_dentry->d_inode->i_cdev, struct taos_data, cdev);

    while (xfrd < count) {
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | reg), my_buf[i++]))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in taos_write()\n");
            return (ret);
        }
        reg++;
        xfrd++;
     }
     //if (prox_on) golden
     //taos_prox_poll_timer_start();
     return ((int)xfrd);
}

// llseek
static loff_t taos_llseek(struct file *file, loff_t offset, int orig) {
    int ret = 0;
    loff_t new_pos = 0;

    //if (prox_on) golden
    //del_timer(&prox_poll_timer);
    if ((offset >= TAOS_MAX_DEVICE_REGS) || (orig < 0) || (orig > 1)) {
        printk(KERN_ERR "TAOS: offset param limit or origin limit check failed in taos_llseek()\n");
        return -EINVAL;
    }
    switch (orig) {
    case 0:
        new_pos = offset;
        break;
    case 1:
        new_pos = file->f_pos + offset;
        break;
    default:
        return -EINVAL;
        break;
    }
    if ((new_pos < 0) || (new_pos >= TAOS_MAX_DEVICE_REGS) || (ret < 0)) {
        printk(KERN_ERR "TAOS: new offset limit or origin limit check failed in taos_llseek()\n");
        return -EINVAL;
    }
    file->f_pos = new_pos;
    //if (prox_on)  golden
    //taos_prox_poll_timer_start();
    return new_pos;
}

//poll iVIZM
/*
static unsigned int taos_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;
	poll_wait(filp, &waitqueue_read, wait);
	if(ReadEnable > 0)
	mask |= POLLIN | POLLRDNORM;
	return mask;
}
*/
#endif

static int taos_sensors_als_on(void) {
    int  ret = 0, i = 0;
    u8 itime = 0, reg_val = 0, reg_cntrl = 0;
    //int lux_val = 0, ret = 0, i = 0, tmp = 0;

            for (i = 0; i < TAOS_FILTER_DEPTH; i++)
                lux_history[i] = -ENODATA;
            //taos_als_threshold_set();//iVIZM
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|TAOS_TRITON_CMD_ALS_INTCLR)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_on\n");
                return (ret);
            }
            itime = (((taos_cfgp->als_time/50) * 18) - 1);
            itime = (~itime);
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_ALS_TIME), itime))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_INTERRUPT), taos_cfgp->prox_intr_filter))) < 0) {//golden
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_GAIN)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_on\n");
                return (ret);
            }
            reg_val = i2c_smbus_read_byte(taos_datap->client);
            reg_val = reg_val & 0xFC;
            reg_val = reg_val | (taos_cfgp->gain & 0x03);
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_GAIN), reg_val))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                return (ret);
            }
            //if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
            //    printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_calibrate\n");
            //    return (ret);
            //}
            //reg_val = i2c_smbus_read_byte(taos_datap->client);
            //reg_cntrl = reg_val | (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_ALS_INT_ENBL);
            reg_cntrl = (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_ALS_INT_ENBL);
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                return (ret);
            }

			i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL) );
			reg_val = i2c_smbus_read_byte(taos_datap->client);
						
      taos_als_threshold_set();//iVIZM
  
			return ret;
}	

#ifdef LCT_NO_USE
// ioctls
//atic int taos_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg) {
static long  taos_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct taos_data *taos_datap;
    int prox_sum = 0, prox_mean = 0, prox_max = 0;
    int lux_val = 0, ret = 0, i = 0, tmp = 0;
    u16 gain_trim_val = 0;
    u8 reg_val = 0, reg_cntrl = 0;
    int ret_check=0;
    int ret_m=0;
    u8 reg_val_temp=0;
    int rwbuf1,rwbuf2;
    void __user *argp = (void __user *)arg;
	
    //printk("JJ %s %d => %s", __FILE__, __LINE__, __func__);

    taos_datap = container_of(file->f_mapping->host->i_cdev, struct taos_data, cdev);
    switch (cmd) {
    case TAOS_IOCTL_SENSOR_CHECK:
        reg_val_temp=0;
        if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
            printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_CHECK failed\n");
            return (ret);
        }
        reg_val_temp = i2c_smbus_read_byte(taos_datap->client);
        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_CHECK,prox_adc_time,%d~\n",reg_val_temp);
        if ((reg_val_temp & 0xFF) == 0xF)
            return -ENODATA;

        break;

    case TAOS_IOCTL_SENSOR_CONFIG:
        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_CONFIG,test01~\n");
        ret = copy_from_user(taos_cfgp, (struct taos_cfg *)arg, sizeof(struct taos_cfg));
        if (ret) {
            printk(KERN_ERR "TAOS: copy_from_user failed in ioctl config_set\n");
            return -ENODATA;
        }

        break;

     case TAOS_IOCTL_SENSOR_ON:
        ret=0;
        reg_val=0;
        ret_m=0;

        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test01~\n");

#if 0
        /*decide the numbers of sensors*/
        ret_m=taos_sensor_mark();
        if(ret_m <0){
            printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, error~~\n");
            return -1;
        }
        if(ret_m ==0){
            printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, num >0 ~~\n");
            return 1;
        }
#endif
#if 1
        /*Register init and turn off */
        for (i = 0; i < TAOS_FILTER_DEPTH; i++){
            /*Rambo ??*/
            lux_history[i] = -ENODATA;
        }
#endif

#if 0
        for (i = 0; i < sizeof(taos_triton_reg_init); i++){
            if(i !=11){
                if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|(TAOS_TRITON_CNTRL +i)), taos_triton_reg_init[i]))) < 0) {
                    printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                    return (ret);
                }
            }
        }
#endif
        /*ALS interrupt clear*/
        if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|TAOS_TRITON_CMD_ALS_INTCLR)))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_on\n");
            return (ret);
        }

        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test02~\n");
        /*Register setting*/
#if 0
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x00), 0x00))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
            return (ret);
        }
#endif

        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test03,prox_int_time,%d~\n",taos_cfgp->prox_int_time);
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_ALS_TIME), taos_cfgp->prox_int_time))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
            return (ret);
        }

        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test04,prox_adc_time,%d~\n",taos_cfgp->prox_adc_time);
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_PRX_TIME), taos_cfgp->prox_adc_time))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
            return (ret);
        }

        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test05,prox_wait_time,%d~\n", taos_cfgp->prox_wait_time);
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_WAIT_TIME), taos_cfgp->prox_wait_time))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
            return (ret);
        }

        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test05-1,prox_intr_filter,%d~\n", taos_cfgp->prox_intr_filter);
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_INTERRUPT), taos_cfgp->prox_intr_filter))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
            return (ret);
        }

        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test06,prox_config,%d~\n",taos_cfgp->prox_config);
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_PRX_CFG), taos_cfgp->prox_config))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
            return (ret);
        }

        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test06,prox_pulse_cnt,%d~\n",taos_cfgp->prox_pulse_cnt);
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_PRX_COUNT), taos_cfgp->prox_pulse_cnt))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
            return (ret);
        }
        /*gain*/
#if 0
        reg_val_temp=0;
        if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_GAIN)))) < 0) {
            printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON failed in ioctl als_calibrate\n");
            return (ret);
        }
        reg_val_temp = i2c_smbus_read_byte(taos_datap->client);
        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test07-1,gain_init&0xC,%d~\n",reg_val_temp&0xC);
        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test07-2,prox_gain&0xFC,%d~\n",(taos_cfgp->prox_gain&0xFC));
        reg_val_temp=(taos_cfgp->prox_gain&0xFC)|(reg_val_temp&0xC);
        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test07,%d~\n",reg_val_temp);
#endif
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_GAIN), taos_cfgp->prox_gain))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
            return (ret);
        }

        /*turn on*/
#if 0
        reg_val_temp=0;
        if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
            printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON failed in ioctl als_calibrate\n");
            return (ret);
        }
        reg_val_temp = i2c_smbus_read_byte(taos_datap->client);
        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test08-1,%d~\n",reg_val_temp);
        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test08-2,%d~\n",(reg_val_temp&0x40));
        reg_val_temp=(0xF&0x3F)|(reg_val_temp&0x40);

        printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, test08,%d~\n",reg_val_temp);
#endif
        if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CNTRL), 0xF))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
            return (ret);
        }

        break;


        case TAOS_IOCTL_SENSOR_OFF:
            ret=0;
            reg_val=0;
            ret_check=0;
            ret_m=0;

#if 0
            /*chech the num of used sensor*/
            ret_check=taos_sensor_check();

            printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_OFF,test01~\n");
            if(ret_check <0){
                printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_OFF, error~~\n");
                return -1;
            }
            if(ret_check ==0){
                printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_ON, num >1 ~~\n");
                return 1;
            }
#endif
            /*turn off*/
            printk(KERN_ERR "TAOS: TAOS_IOCTL_SENSOR_OFF,test02~\n");
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x00), 0x00))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_off\n");
                return (ret);
            }

             break;
        case TAOS_IOCTL_ALS_ON:
            printk("####################################\n");
            printk("######## TAOS IOCTL ALS ON #########\n");
            printk("####################################\n");

            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_calibrate\n");
                return (ret);
            }
  	printk("JJ %s %d => %s", __FILE__, __LINE__, __func__);
            reg_val = i2c_smbus_read_byte(taos_datap->client);
			if ((reg_val & TAOS_TRITON_CNTL_PROX_DET_ENBL) == 0x0) {
  	printk("JJ %s %d => %s", __FILE__, __LINE__, __func__);
                taos_sensors_als_on();
			}	

       /*     for (i = 0; i < TAOS_FILTER_DEPTH; i++)
                lux_history[i] = -ENODATA;
            //taos_als_threshold_set();//iVIZM
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|TAOS_TRITON_CMD_ALS_INTCLR)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_on\n");
                return (ret);
            }
            itime = (((taos_cfgp->als_time/50) * 18) - 1);
            itime = (~itime);
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_ALS_TIME), itime))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_INTERRUPT), taos_cfgp->prox_intr_filter))) < 0) {//golden
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_GAIN)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_on\n");
                return (ret);
            }
            reg_val = i2c_smbus_read_byte(taos_datap->client);
            reg_val = reg_val & 0xFC;
            reg_val = reg_val | (taos_cfgp->gain & 0x03);
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_GAIN), reg_val))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_calibrate\n");
                return (ret);
            }
            reg_val = i2c_smbus_read_byte(taos_datap->client);
            reg_cntrl = reg_val | (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_ALS_INT_ENBL);
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                return (ret);
            }
            taos_als_threshold_set();//iVIZM
			*/
			ALS_ON = 1;
  	printk("JJ %s %d => %s", __FILE__, __LINE__, __func__);
            return (ret);
            break;
        case TAOS_IOCTL_ALS_OFF:
            printk("#####################################\n");
            printk("######## TAOS IOCTL ALS OFF #########\n");
            printk("#####################################\n");
            for (i = 0; i < TAOS_FILTER_DEPTH; i++)
                lux_history[i] = -ENODATA;
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_calibrate\n");
                return (ret);
            }
            reg_val = i2c_smbus_read_byte(taos_datap->client);
            if ((reg_val & TAOS_TRITON_CNTL_PROX_DET_ENBL) == 0x0) {
               if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), 0x00))) < 0) {
                   printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_off\n");
                   return (ret);
			   }
			   cancel_work_sync(&taos_datap->work);//golden
          //      reg_cntrl = reg_val & (~TAOS_TRITON_CNTL_ALS_INT_ENBL);
		//	} else {
        //        reg_cntrl = reg_val & (~(TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_ALS_INT_ENBL));
        //    printk("===========cancel_work_sync======als====\n");
		//		cancel_work_sync(&taos_datap->work);//golden
		//	}
        //    if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) {
        //        printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_off\n");
        //        return (ret);
            }
			ALS_ON = 0;
            return (ret);
            break;
        case TAOS_IOCTL_ALS_DATA:
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_data\n");
                return (ret);
            }
            reg_val = i2c_smbus_read_byte(taos_datap->client);
            if ((reg_val & (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON)) != (TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_PWRON))
                return -ENODATA;
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_STATUS)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_data\n");
                return (ret);
            }
            reg_val = i2c_smbus_read_byte(taos_datap->client);
            if ((reg_val & TAOS_TRITON_STATUS_ADCVALID) != TAOS_TRITON_STATUS_ADCVALID)
                return -ENODATA;
            if ((lux_val = taos_get_lux()) < 0)
                printk(KERN_ERR "TAOS: call to taos_get_lux() returned error %d in ioctl als_data\n", lux_val);
            lux_val = taos_lux_filter(lux_val);
            return (lux_val);
            break;
        case TAOS_IOCTL_ALS_CALIBRATE:
  printk("%s %d => %s", __FILE__, __LINE__, __func__);
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_calibrate\n");
                return (ret);
            }
            reg_val = i2c_smbus_read_byte(taos_datap->client);
            if ((reg_val & 0x07) != 0x07)
                return -ENODATA;
            if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_STATUS)))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_calibrate\n");
                return (ret);
            }
            reg_val = i2c_smbus_read_byte(taos_datap->client);
            if ((reg_val & 0x01) != 0x01)
                return -ENODATA;
            if ((lux_val = taos_get_lux()) < 0) {
                printk(KERN_ERR "TAOS: call to lux_val() returned error %d in ioctl als_data\n", lux_val);
                return (lux_val);
            }
            gain_trim_val = (u16)(((taos_cfgp->calibrate_target) * 512)/lux_val);
            taos_cfgp->gain_trim = (int)gain_trim_val;
  printk("%s %d => %s", __FILE__, __LINE__, __func__);
            return ((int)gain_trim_val);
            break;
        case TAOS_IOCTL_CONFIG_GET:
            ret = copy_to_user((struct taos_cfg *)arg, taos_cfgp, sizeof(struct taos_cfg));
            if (ret) {
                printk(KERN_ERR "TAOS: copy_to_user failed in ioctl config_get\n");
                return -ENODATA;
            }
            return (ret);
            break;
        case TAOS_IOCTL_CONFIG_SET:
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            printk("^^^^^^^^^ TAOS INCTL CONFIG SET  ^^^^^^^\n");
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            ret = copy_from_user(taos_cfgp, (struct taos_cfg *)arg, sizeof(struct taos_cfg));
            if (ret) {
                printk(KERN_ERR "TAOS: copy_from_user failed in ioctl config_set\n");
                return -ENODATA;
            }
            if(taos_cfgp->als_time < 50)
                taos_cfgp->als_time = 50;
            if(taos_cfgp->als_time > 650)
                taos_cfgp->als_time = 650;
            tmp = (taos_cfgp->als_time + 25)/50;
            taos_cfgp->als_time = tmp*50;
            sat_als = (256 - taos_cfgp->prox_int_time) << 10;
            sat_prox = (256 - taos_cfgp->prox_adc_time) << 10;
            break;
        case TAOS_IOCTL_PROX_ON:
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            printk("^^^^^^^^^ TAOS IOCTL PROX ON  ^^^^^^^\n");
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            prox_on = 1;

            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x01), taos_cfgp->prox_int_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x02), taos_cfgp->prox_adc_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x03), taos_cfgp->prox_wait_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }

            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0C), taos_cfgp->prox_intr_filter))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0D), taos_cfgp->prox_config))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0E), taos_cfgp->prox_pulse_cnt))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0F), taos_cfgp->prox_gain))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            //if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
            //    printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl prox_on\n");
            //    return (ret);
            //}
            //reg_val = i2c_smbus_read_byte(taos_datap->client);
            //reg_cntrl = reg_val | (TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_PROX_INT_ENBL | TAOS_TRITON_CNTL_ADC_ENBL);
            reg_cntrl = TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_PROX_INT_ENBL | 
				                                      TAOS_TRITON_CNTL_ADC_ENBL | TAOS_TRITON_CNTL_WAIT_TMR_ENBL  ;
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            taos_prox_threshold_set();//iVIZM
            break;
        case TAOS_IOCTL_PROX_OFF:
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            printk("^^^^^^^^^ TAOS IOCTL PROX OFF  ^^^^^^^\n");
            printk("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            //if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
            //    printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl als_calibrate\n");
            //    return (ret);
            //}
            //reg_val = i2c_smbus_read_byte(taos_datap->client);
            //if (reg_val & TAOS_TRITON_CNTL_ALS_INT_ENBL) {
            //    reg_cntrl = reg_val & (~(TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PROX_INT_ENBL));
			//} else {
            //    reg_cntrl = reg_val & (~(TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_PROX_INT_ENBL));
            //reg_cntrl = reg_val & (~(TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_PROX_INT_ENBL));
           // printk("===========cancel_work_sync=====prox=====\n");
			//	cancel_work_sync(&taos_datap->work);//golden
			//}
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), 0x00))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_off\n");
                return (ret);
            }
			if (ALS_ON == 1) {
                taos_sensors_als_on();
			} else {
				cancel_work_sync(&taos_datap->work);//golden
			}
            prox_on = 0;
            break;
        case TAOS_IOCTL_PROX_DATA:
            //Rambo
#if 1
            if ((ret = taos_prox_poll(prox_cur_infop)) < 0) {
                printk(KERN_ERR "TAOS: call to prox_poll failed in taos_prox_poll_timer_func()\n");
                return ret;
            }
#endif
            ret = copy_to_user((struct taos_prox_info *)arg, prox_cur_infop, sizeof(struct taos_prox_info));
            if (ret) {
                printk(KERN_ERR "TAOS: copy_to_user failed in ioctl prox_data\n");
                return -ENODATA;
            }
            return (ret);
            break;
        case TAOS_IOCTL_PROX_EVENT:
            if ((ret = taos_prox_poll(prox_cur_infop)) < 0) {
                printk(KERN_ERR "TAOS: call to prox_poll failed in taos_prox_poll_timer_func()\n");
                return ret;
            }

            return (prox_cur_infop->prox_event);
            break;
        case TAOS_IOCTL_PROX_CALIBRATE:
            //if (prox_on) golden
            //del_timer(&prox_poll_timer);
            //else {
            //printk(KERN_ERR "TAOS: ioctl prox_calibrate was called before ioctl prox_on was called\n");
            //return -EPERM;
            //}
  	printk("JJ %s %d => %s", __FILE__, __LINE__, __func__);
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x01), taos_cfgp->prox_int_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x02), taos_cfgp->prox_adc_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x03), taos_cfgp->prox_wait_time))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }

            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0D), taos_cfgp->prox_config))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }

            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0E), taos_cfgp->prox_pulse_cnt))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|0x0F), taos_cfgp->prox_gain))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }
            //if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL)))) < 0) {
            //    printk(KERN_ERR "TAOS: i2c_smbus_write_byte failed in ioctl prox_on\n");
            //    return (ret);
            //}
            //reg_val = i2c_smbus_read_byte(taos_datap->client);
            reg_cntrl = reg_val | (TAOS_TRITON_CNTL_PROX_DET_ENBL | TAOS_TRITON_CNTL_PWRON | TAOS_TRITON_CNTL_ADC_ENBL);
            if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CNTRL), reg_cntrl))) < 0) {
                printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl prox_on\n");
                return (ret);
            }

            prox_sum = 0;
            prox_max = 0;
  	printk("%s %d => %s", __FILE__, __LINE__, __func__);
            for (i = 0; i < 20; i++) {
                if ((ret = taos_prox_poll(&prox_cal_info[i])) < 0) {
                    printk(KERN_ERR "TAOS: call to prox_poll failed in ioctl prox_calibrate\n");
                    return (ret);
                }
                prox_sum += prox_cal_info[i].prox_data;
                if (prox_cal_info[i].prox_data > prox_max)
                    prox_max = prox_cal_info[i].prox_data;
                mdelay(100);
            }
            prox_mean = prox_sum/20;
            taos_cfgp->prox_threshold_hi = ((((prox_max - prox_mean) * 200) + 50)/100) + prox_mean;
            taos_cfgp->prox_threshold_lo = ((((prox_max - prox_mean) * 170) + 50)/100) + prox_mean;
             printk("TAOS:------------ taos_cfgp->prox_threshold_hi = %d\n",taos_cfgp->prox_threshold_hi );
             printk("TAOS:------------ taos_cfgp->prox_threshold_lo = %d\n",taos_cfgp->prox_threshold_lo );
           // if (taos_cfgp->prox_threshold_lo < ((sat_prox*12)/100)) {
           //     taos_cfgp->prox_threshold_lo = ((sat_prox*12)/100);
           //     taos_cfgp->prox_threshold_hi = ((sat_prox*15)/100);
           // }
            for (i = 0; i < sizeof(taos_triton_reg_init); i++){
                if(i !=11){
                    if ((ret = (i2c_smbus_write_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG|(TAOS_TRITON_CNTRL +i)), taos_triton_reg_init[i]))) < 0) {
                        printk(KERN_ERR "TAOS: i2c_smbus_write_byte_data failed in ioctl als_on\n");
                        return (ret);
                    }
                 }
             }

            //taos_prox_poll_timer_start();//Rambo
            break;	
        default:
            return -EINVAL;
            break;
    }
    return (ret);
}
#endif


// read/calculate lux value
static int taos_get_lux(void) {
    u16 raw_clear = 0, raw_ir = 0, raw_lux = 0;
    u32 lux = 0;
    u32 ratio = 0;
    u8 dev_gain = 0;
    u16 Tint = 0;
    struct lux_data *p;
    int ret = 0;
    u8 chdata[4];
    int tmp = 0, i = 0;

    for (i = 0; i < 4; i++) {
        if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | (TAOS_TRITON_ALS_CHAN0LO + i))))) < 0) {
            printk(KERN_ERR "TAOS: i2c_smbus_write_byte() to chan0/1/lo/hi reg failed in taos_get_lux()\n");
            return (ret);
        }
        chdata[i] = i2c_smbus_read_byte(taos_datap->client);
        #if TSL27713_DEBUG
        printk("ch(%d),data=%d\n",i,chdata[i]);
        #endif
    }
    #if TSL27713_DEBUG
    printk("ch0=%d\n",chdata[0]+chdata[1]*256);
    printk("ch1=%d\n",chdata[2]+chdata[3]*256);
    #endif

    tmp = (taos_cfgp->als_time + 25)/50;            //if atime =100  tmp = (atime+25)/50=2.5   tine = 2.7*(256-atime)=  412.5
    TritonTime.numerator = 1;
    TritonTime.denominator = tmp;

    tmp = 300 * taos_cfgp->als_time;               //tmp = 300*atime  400
    if(tmp > 65535)
        tmp = 65535;
    TritonTime.saturation = tmp;
    raw_clear = chdata[1];
    raw_clear <<= 8;
    raw_clear |= chdata[0];
    raw_ir    = chdata[3];
    raw_ir    <<= 8;
    raw_ir    |= chdata[2];

    raw_clear *= (taos_cfgp->scale_factor );
    raw_ir *= (taos_cfgp->scale_factor );

    if(raw_ir > raw_clear) {
        raw_lux = raw_ir;
        raw_ir = raw_clear;
        raw_clear = raw_lux;
    }
    dev_gain = taos_triton_gain_table[taos_cfgp->gain & 0x3];
    if(raw_clear >= lux_timep->saturation)
        return(TAOS_MAX_LUX);
    if(raw_ir >= lux_timep->saturation)
        return(TAOS_MAX_LUX);
    if(raw_clear == 0)
        return(0);
    if(dev_gain == 0 || dev_gain > 127) {
        printk(KERN_ERR "TAOS: dev_gain = 0 or > 127 in taos_get_lux()\n");
        return -1;
    }
    if(lux_timep->denominator == 0) {
        printk(KERN_ERR "TAOS: lux_timep->denominator = 0 in taos_get_lux()\n");
        return -1;
    }
    ratio = (raw_ir<<15)/raw_clear;
    for (p = lux_tablep; p->ratio && p->ratio < ratio; p++);
    if(!p->ratio) {//iVIZM
        if(lux_history[0] < 0)
            return 0;
        else
            return lux_history[0];
    }
    Tint = taos_cfgp->als_time;
    raw_clear = ((raw_clear*400 + (dev_gain>>1))/dev_gain + (Tint>>1))/Tint;
    raw_ir = ((raw_ir*400 +(dev_gain>>1))/dev_gain + (Tint>>1))/Tint;
    lux = ((raw_clear*(p->clear)) - (raw_ir*(p->ir)));
    lux = (lux + 32000)/64000;
    if(lux > TAOS_MAX_LUX) {
        lux = TAOS_MAX_LUX;
    }
    //return(lux)*taos_cfgp->filter_count;
    return(lux);
}

#ifdef LCT_NO_USE
static int taos_lux_filter(int lux)
{
    static u8 middle[] = {1,0,2,0,0,2,0,1};
    int index;

    lux_history[2] = lux_history[1];
    lux_history[1] = lux_history[0];
    lux_history[0] = lux;

    if(lux_history[2] < 0) { //iVIZM
        if(lux_history[1] > 0)
            return lux_history[1];       
        else 
            return lux_history[0];
    }
    index = 0;
    if( lux_history[0] > lux_history[1] ) 
        index += 4;
    if( lux_history[1] > lux_history[2] ) 
        index += 2;
    if( lux_history[0] > lux_history[2] )
        index++;
    return(lux_history[middle[index]]);
}
#endif

// verify device
static int taos_device_name(unsigned char *bufp, char **device_name) {
    if( (bufp[0x12]&0x00)!=0x00)        //TSL27711=0x00 TSL27713=0x09 TMD27711=0x20 TMD27713=0x29    2011.07.21
        return(0);
    if(bufp[0x10]|bufp[0x1a]|bufp[0x1b]|bufp[0x1c]|bufp[0x1d]|bufp[0x1e])
        return(0);
    if(bufp[0x13]&0x0c)
        return(0);
    *device_name=TAOS_DEVICE_ID;
    return(1);
}

// proximity poll
static int taos_prox_poll(struct taos_prox_info *prxp) {
    //static int event = 0;
    //u16 status = 0;
    int i = 0, ret = 0; //wait_count = 0;
    u8 chdata[6];
	
/*	
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG | 0x13)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte(1) failed in taos_prox_poll()\n");
        return (ret);
    }
    status = i2c_smbus_read_byte(taos_datap->client);
    while ((status & 0x30) != 0x30) { 
        status = i2c_smbus_read_byte(taos_datap->client);
        wait_count++;
        if (wait_count > 10) {
            printk(KERN_ERR "TAOS: Prox status invalid for 100 ms in taos_prox_poll()\n");
            return -ENODATA;
        }
        mdelay(10); 
    }
	
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|0x05)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte(2) failed in taos_prox_poll()\n");
        return (ret);
    }
    if ((ret = (i2c_smbus_write_byte(taos_datap->client, (TAOS_TRITON_CMD_REG|TAOS_TRITON_CMD_SPL_FN|0x06)))) < 0) {
        printk(KERN_ERR "TAOS: i2c_smbus_write_byte(3) failed in taos_prox_poll()\n");
        return (ret);
    }
	*/
    for (i = 0; i < 6; i++) {
        chdata[i] = (i2c_smbus_read_byte_data(taos_datap->client, (TAOS_TRITON_CMD_REG | TAOS_TRITON_CMD_AUTO | (TAOS_TRITON_ALS_CHAN0LO + i))));
    }
    prxp->prox_clear = chdata[1];
    prxp->prox_clear <<= 8;
    prxp->prox_clear |= chdata[0];
    //if (prxp->prox_clear > ((sat_als*80)/100))
      if ((TSL_SENSOR_FACTORY_MODE != mode) && (prxp->prox_clear > ((sat_als*80)/100)))
    	{
    	#if TSL27713_DEBUG
	printk(KERN_ERR "LCH1:prxp->prox_clear = %d,sat_als = %d\n",prxp->prox_clear,sat_als);
#endif	
        return -ENODATA;
    	}
    prxp->prox_data = chdata[5];
    prxp->prox_data <<= 8;
    prxp->prox_data |= chdata[4];

#if TSL27713_DEBUG
	printk(KERN_ERR "LCH 1:prxp->prox_data = %d\n",prxp->prox_data);
#endif	
    //prox_history_hi <<= 1;
    //prox_history_hi |= ((prxp->prox_data > taos_cfgp->prox_threshold_hi)? 1:0);
    //prox_history_hi &= 0x07;
    //prox_history_lo <<= 1;
    //prox_history_lo |= ((prxp->prox_data > taos_cfgp->prox_threshold_lo)? 1:0);
    //prox_history_lo &= 0x07;
    //if (prox_history_hi == 0x07)
    //    event = 1;
    //else {
    //    if (prox_history_lo == 0)
    //        event = 0;
    //}	 		
    //prxp->prox_event = event;
    return (ret);
}

#ifdef LCT_NO_USE
// prox poll timer function
static void taos_prox_poll_timer_func(unsigned long param) {
    int ret = 0;

     if (!device_released) {
        if ((ret = taos_prox_poll(prox_cur_infop)) < 0) {
            printk(KERN_ERR "TAOS: call to prox_poll failed in taos_prox_poll_timer_func()\n");
            return;
        }
        taos_prox_poll_timer_start();
    }
    return;
}

// start prox poll timer
static void taos_prox_poll_timer_start(void) {
    init_timer(&prox_poll_timer);
    prox_poll_timer.expires = jiffies + (HZ/10);
    prox_poll_timer.function = taos_prox_poll_timer_func;
    add_timer(&prox_poll_timer);
    return;
}
#endif


MODULE_AUTHOR("John Koshi - Surya Software");
MODULE_DESCRIPTION("TAOS ambient light and proximity sensor driver");
MODULE_LICENSE("GPL");

module_init(taos_init);
module_exit(taos_exit);

