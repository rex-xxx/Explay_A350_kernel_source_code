/*
 * Copyright (C) 2011 MCUBE, Inc.
 *
 * Initial Code:
 *	Tan Liang
 */


/*! \file mc32x0_driver.c
    \brief This file contains all function implementations for the mc32x0 in linux
    
    Details.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <asm/uaccess.h>
#include <linux/unistd.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>

#include <mc32x0_driver.h>
#include <mach/socinfo.h>


#define MC32X0_IOC_MAGIC 'B' //#define MC32X0_IOC_MAGIC 'M'

#ifdef CONFIG_LCT_AW550
#define MC32X0_READ_HW_VER_CHIP_ID		_IOWR(MC32X0_IOC_MAGIC,3,signed char)
#endif
#define MC32X0_SET_RANGE				_IOWR(MC32X0_IOC_MAGIC,4, unsigned char)
#define MC32X0_GET_RANGE				_IOWR(MC32X0_IOC_MAGIC,5, unsigned char)
#define MC32X0_SET_MODE					_IOWR(MC32X0_IOC_MAGIC,6, unsigned char)
#define MC32X0_GET_MODE					_IOWR(MC32X0_IOC_MAGIC,7, unsigned char)
#define MC32X0_SET_BANDWIDTH			_IOWR(MC32X0_IOC_MAGIC,8, unsigned char)
#define MC32X0_GET_BANDWIDTH			_IOWR(MC32X0_IOC_MAGIC,9, unsigned char)
#define MC32X0_READ_ACCEL_X				_IOWR(MC32X0_IOC_MAGIC,10,short)
#define MC32X0_READ_ACCEL_Y				_IOWR(MC32X0_IOC_MAGIC,11,short)
#define MC32X0_READ_ACCEL_Z				_IOWR(MC32X0_IOC_MAGIC,12,short)
#define MC32X0_GET_INTERRUPT_STATUS		_IOWR(MC32X0_IOC_MAGIC,13,unsigned char)
#define MC32X0_READ_ACCEL_XYZ			_IOWR(MC32X0_IOC_MAGIC,1,signed char) //_IOWR(MC32X0_IOC_MAGIC,14,short)


#define MC32X0_IOC_MAXNR				50

//#define MC32X0_DEBUG

static struct i2c_client *mc32x0_client = NULL;  

struct mc32x0_data{
	mc32x0_t mc32x0;
};

static char mc32x0_i2c_write(unsigned char reg_addr, unsigned char *data, unsigned char len);
static char mc32x0_i2c_read(unsigned char reg_addr, unsigned char *data, unsigned char len);
static void mc32x0_i2c_delay(unsigned int msec);


/*	i2c delay routine for eeprom	*/
static inline void mc32x0_i2c_delay(unsigned int msec)
{
	mdelay(msec);
}

/*	i2c write routine for mc32x0	*/
static inline char mc32x0_i2c_write(unsigned char reg_addr, unsigned char *data, unsigned char len)
{
	s32 dummy;
	unsigned char buffer[2];
	if( mc32x0_client == NULL )	/*	No global client pointer?	*/
		return -1;

	while(len--)
	{
		buffer[0] = reg_addr;
		buffer[1] = *data;
		dummy = i2c_master_send(mc32x0_client, (char*)buffer, 2);

		reg_addr++;
		data++;
		if(dummy < 0)
			return -1;
	}
	return 0;
}

/*	i2c read routine for mc32x0	*/
static inline char mc32x0_i2c_read(unsigned char reg_addr, unsigned char *data, unsigned char len) 
{
	s32 dummy;
	if( mc32x0_client == NULL )	/*	No global client pointer?	*/
		return -1;

       
		dummy = i2c_master_send(mc32x0_client, (char*)&reg_addr, 1);
		if(dummy < 0)
			return -1;
		dummy = i2c_master_recv(mc32x0_client, (char*)data, len);
		if(dummy < 0)
			return -1;
		
	
	return 0;
}


/*	read command for MC32X0 device file	*/
static ssize_t mc32x0_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{	
	mc32x0acc_t acc;	
	int ret;
	if( mc32x0_client == NULL )
	{
#ifdef MC32X0_DEBUG
		printk(KERN_INFO "I2C driver not install\n");
#endif
		return -1;
	}

	mc32x0_read_accel_xyz(&acc);
#ifdef MC32X0_DEBUG
	printk(KERN_INFO "#####################################################MC32X0: X/Y/Z axis: %-8d %-8d %-8d\n" ,
		(int)acc.x, (int)acc.y, (int)acc.z);  
#endif

/*
ret = acc.y;
acc.y = (-1)*acc.x;
acc.x = ret;
*/

	if( count != sizeof(acc) )
	{
		return -1;
	}
	ret = copy_to_user(buf,&acc, sizeof(acc));
	if( ret != 0 )
	{
#ifdef MC32X0_DEBUG
	printk(KERN_INFO "MC32X0: copy_to_user result: %d\n", ret);
#endif
	}
	return sizeof(acc);
}

/*	write command for MC32X0 device file	*/
static ssize_t mc32x0_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	if( mc32x0_client == NULL )
		return -1;
#ifdef MC32X0_DEBUG
	printk(KERN_INFO "MC32X0 should be accessed with ioctl command\n");
#endif
	return 0;
}

/*	open command for MC32X0 device file	*/
static int mc32x0_open(struct inode *inode, struct file *file)
{
#ifdef MC32X0_DEBUG
		printk(KERN_WARNING "**********************************************************%s\n",__FUNCTION__); 
#endif

	if( mc32x0_client == NULL)
	{
#ifdef MC32X0_DEBUG
		printk(KERN_INFO "I2C driver not install\n"); 
#endif
		return -1;
	}

	mc32x0_set_image();
	mc32x0_set_mode(MC32X0_WAKE);

#ifdef MC32X0_DEBUG
	printk(KERN_INFO "MC32X0 has been opened\n");
#endif
	return 0;
}

/*	release command for MC32X0 device file	*/
static int mc32x0_close(struct inode *inode, struct file *file)
{
#ifdef MC32X0_DEBUG
	printk(KERN_INFO "%s\n",__FUNCTION__);	
#endif
	//mc32x0_set_mode(MC32X0_STANDBY);		//ruirong.zhang removed for calibration apk call  mc32x0_close to make g-sensor power down
	return 0;
}


static unsigned int mc32x0_poll(struct file *fp, poll_table * wait)
{
	return POLLIN | POLLRDNORM;
}


/*	ioctl command for MC32X0 device file	*/
//static int mc32x0_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
static long mc32x0_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	#ifdef CONFIG_LCT_AW550	
	signed char rwbuf[3] ={0};
	#endif

	int err = 0;
	unsigned char data[6];

#ifdef MC32X0_DEBUG
	//printk(KERN_INFO "ioctl ioctl ioctl ioctl ioctl ioctl%s, cmd = %d\n",__FUNCTION__, cmd);	
#endif

	/* check cmd */
	if(_IOC_TYPE(cmd) != MC32X0_IOC_MAGIC)	
	{
#ifdef MC32X0_DEBUG		
		printk(KERN_INFO "cmd magic type error\n");
#endif
		return -ENOTTY;
	}
	if(_IOC_NR(cmd) > MC32X0_IOC_MAXNR)
	{
#ifdef MC32X0_DEBUG
		printk(KERN_INFO "cmd number error\n");
#endif
		return -ENOTTY;
	}

	if(_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE,(void __user*)arg, _IOC_SIZE(cmd));
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user*)arg, _IOC_SIZE(cmd));
	if(err)
	{
#ifdef MC32X0_DEBUG
		printk(KERN_INFO "cmd access_ok error\n");
#endif
		return -EFAULT;
	}
	/* check mc32x0_client */
	if( mc32x0_client == NULL)
	{
#ifdef MC32X0_DEBUG
		printk(KERN_INFO "I2C driver not install\n"); 
#endif
		return -EFAULT;
	}
	
	/* cmd mapping */

	switch(cmd)
	{
			#ifdef CONFIG_LCT_AW550
		case MC32X0_READ_HW_VER_CHIP_ID:
			   if (SOCINFO_VERSION_MINOR(socinfo_get_platform_version()) == 2) 
			   {
			   	rwbuf[0] = 'm';
			   	rwbuf[1] = 6;
				if(copy_to_user((char*)arg, rwbuf, 3)){					
					return-EFAULT;
				}
			   }
			   else
			   {			    
				rwbuf[0] = 'm';	   
				rwbuf[1] = 4;			
				if(copy_to_user((char*)arg, rwbuf, 3)){					
					return-EFAULT;
				}

			   }
			   
			   //printk(KERN_INFO "--mc32x0 ioctl  chip id = %c, dir = %d\n",rwbuf[0],rwbuf[1]);
			return 0;
		#endif	

	case MC32X0_SET_RANGE:
		if(copy_from_user(data,(unsigned char*)arg,1)!=0)
		{
#ifdef MC32X0_DEBUG			
			printk(KERN_INFO "copy_from_user error\n");
#endif
			return -EFAULT;
		}
		err = mc32x0_set_range(*data);
		return err;

	case MC32X0_GET_RANGE:
		err = mc32x0_get_range(data);
		if(copy_to_user((unsigned char*)arg,data,1)!=0)
		{
#ifdef MC32X0_DEBUG			
			printk(KERN_INFO "copy_to_user error\n");
#endif
			return -EFAULT;
		}
		return err;

	case MC32X0_SET_MODE:
		if(copy_from_user(data,(unsigned char*)arg,1)!=0)
		{
#ifdef MC32X0_DEBUG			
			printk(KERN_INFO "copy_from_user error\n");
#endif
			return -EFAULT;
		}
		err = mc32x0_set_mode(*data);
		return err;

	case MC32X0_GET_MODE:
		err = mc32x0_get_mode(data);
		if(copy_to_user((unsigned char*)arg,data,1)!=0)
		{
#ifdef MC32X0_DEBUG			
			printk(KERN_INFO "copy_to_user error\n");
#endif
			return -EFAULT;
		}
		return err;

	case MC32X0_SET_BANDWIDTH:
		if(copy_from_user(data,(unsigned char*)arg,1)!=0)
		{
#ifdef MC32X0_DEBUG
			printk(KERN_INFO "copy_from_user error\n");
#endif
			return -EFAULT;
		}
		err = mc32x0_set_bandwidth(*data);
		return err;

	case MC32X0_GET_BANDWIDTH:
		err = mc32x0_get_bandwidth(data);
		if(copy_to_user((unsigned char*)arg,data,1)!=0)
		{
#ifdef MC32X0_DEBUG
			printk(KERN_INFO "copy_to_user error\n");
#endif
			return -EFAULT;
		}
		return err;

	case MC32X0_READ_ACCEL_X:
		err = mc32x0_read_accel_x((short*)data);
		if(copy_to_user((short*)arg,(short*)data,2)!=0)
		{
#ifdef MC32X0_DEBUG
			printk(KERN_INFO "copy_to_user error\n");
#endif
			return -EFAULT;
		}
		return err;

	case MC32X0_READ_ACCEL_Y:
		err = mc32x0_read_accel_y((short*)data);
		if(copy_to_user((short*)arg,(short*)data,2)!=0)
		{
#ifdef MC32X0_DEBUG
			printk(KERN_INFO "copy_to_user error\n");
#endif
			return -EFAULT;
		}
		return err;

	case MC32X0_READ_ACCEL_Z:
		err = mc32x0_read_accel_z((short*)data);
		if(copy_to_user((short*)arg,(short*)data,2)!=0)
		{
#ifdef MC32X0_DEBUG
			printk(KERN_INFO "copy_to_user error\n");
#endif
			return -EFAULT;
		}
		return err;

	case MC32X0_READ_ACCEL_XYZ:
		err = mc32x0_read_accel_xyz((mc32x0acc_t*)data);

		
		if(copy_to_user((mc32x0acc_t*)arg,(mc32x0acc_t*)data,6)!=0)
		{
#ifdef MC32X0_DEBUG
			printk(KERN_INFO "copy_to error\n");
#endif
			return -EFAULT;
		}
		
		return err;	

	case MC32X0_GET_INTERRUPT_STATUS:
		err = mc32x0_get_interrupt_status(data);
		if(copy_to_user((unsigned char*)arg,data,1)!=0)
		{
#ifdef MC32X0_DEBUG
			printk(KERN_INFO "copy_to_user error\n");
#endif
			return -EFAULT;
		}
		return err;


	default:
		return 0;
	}
}


static const struct file_operations mc32x0_fops = {
	.owner = THIS_MODULE,
	.read = mc32x0_read,
	.write = mc32x0_write,
	.open = mc32x0_open,
	.release = mc32x0_close,
	 //.ioctl = mc32x0_ioctl,
	.unlocked_ioctl = mc32x0_ioctl,
	.poll		= mc32x0_poll,
};


static struct miscdevice mcube_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "accdev",
	.fops = &mc32x0_fops,
};

/*
static int mc32x0_detect(struct i2c_client *client, int kind,
			  struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = client->adapter;
#ifdef MC32X0_DEBUG
	printk(KERN_INFO "%s\n", __FUNCTION__);
#endif
	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C))
		return -ENODEV;

	strlcpy(info->type, "mc32x0", I2C_NAME_SIZE);

	return 0;
}
*/

static int mc32x0_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
		struct mc32x0_data *data;
		int err = 0;
		int tempvalue;
	
#ifdef MC32X0_DEBUG
		printk(KERN_INFO "%s\n",__FUNCTION__);
#endif
	
		if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		{
			printk(KERN_INFO "i2c_check_functionality error\n");
			goto exit;
		}
	
		/* OK. For now, we presume we have a valid client. We now create the
		   client structure, even though we cannot fill it completely yet. */
		if (!(data = kmalloc(sizeof(struct mc32x0_data), GFP_KERNEL)))
		{
			err = -ENOMEM;
			printk(KERN_INFO "kmalloc error\n");
			goto exit;
		}
		memset(data, 0, sizeof(struct mc32x0_data));
	
		i2c_set_clientdata(client, data);
	
			
		/* read chip id */
		//tempvalue = MC32X0_CHIP_ID;

		/*
		i2c_master_send(client, (char*)&tempvalue, 1);
		i2c_master_recv(client, (char*)&tempvalue, 1);
		*/
		tempvalue = i2c_smbus_read_word_data(client, MC32X0_CHIP_ID);
		
		if((tempvalue&0x00FF) == 0x0002)
		{
			printk(KERN_INFO "mCube Device detected!\nMC32X0 registered I2C driver!\n");
			mc32x0_client = client;
		}
		else
		{
			printk(KERN_INFO "mCube Device not found, i2c error %d \n", tempvalue);
			
		data = i2c_get_clientdata(client);
	
			mc32x0_client = NULL;
			err = -1;
			goto exit_kfree;
		}
		
		err = misc_register(&mcube_device);
		if (err) {
			printk(KERN_ERR "mc32x0 device register failed\n");
			goto exit_kfree;
		}
		printk(KERN_INFO "mc32x0 device create ok\n");
	
		data->mc32x0.bus_write = mc32x0_i2c_write;
		data->mc32x0.bus_read = mc32x0_i2c_read;
		mcube_mc32x0_init(&(data->mc32x0));
		return 0;
	exit_kfree:
		kfree(data);
	exit:
		return err;
}



static int mc32x0_suspend(struct device *dev)
{	
	mc32x0_set_mode(MC32X0_STANDBY);	
	return 0;
}


static int mc32x0_resume(struct device *dev)
{
	mc32x0_set_mode(MC32X0_WAKE);	
	return 0;
}



static int mc32x0_remove(struct i2c_client *client)
{
	struct mc32x0_data *data;
#ifdef MC32X0_DEBUG
	printk(KERN_INFO "%s\n",__FUNCTION__);
#endif	
	mc32x0_set_mode(MC32X0_STANDBY);	//ruirong.zhang add on@21120427
	data = i2c_get_clientdata(client);
	mc32x0_client = NULL;
	misc_deregister(&mcube_device);
	kfree(data);
	return 0;
}


//static unsigned short normal_i2c[] = { I2C_CLIENT_END};
//I2C_CLIENT_INSMOD_1(mc32x0);

static const struct i2c_device_id mc32x0_id[] = {
	{ "mc32x0", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, mc32x0_id);

static struct dev_pm_ops mc32x0_pmops = {
		.resume 		= mc32x0_resume,
		.suspend		= mc32x0_suspend,
};
#define MC32X0_PMOPS &mc32x0_pmops

static struct i2c_driver mc32x0_driver = {	
	.driver = {
		.owner	= THIS_MODULE,	
		.name	= "mc32x0",
		.pm = MC32X0_PMOPS,
	},
	.class		= I2C_CLASS_HWMON,
	.id_table	= mc32x0_id,
	//.address_data	= &addr_data,
	.probe		= mc32x0_probe,
	.remove		= mc32x0_remove,
	//.detect		= mc32x0_detect,
};

static int __init MC32X0_init(void)
{
#ifdef MC32X0_DEBUG
	printk(KERN_INFO "=================================================%s\n",__FUNCTION__);
#endif
	return i2c_add_driver(&mc32x0_driver);
}

static void __exit MC32X0_exit(void)
{
	i2c_del_driver(&mc32x0_driver);
	printk(KERN_ERR "MC32X0 exit\n");
}


MODULE_DESCRIPTION("MC32X0 driver");

module_init(MC32X0_init);
module_exit(MC32X0_exit);
