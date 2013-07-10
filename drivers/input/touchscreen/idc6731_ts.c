/* linux/drivers/input/touchscreen/IDC6731_i2c.c
 *
 * IdeaCom I2C Multi-touch screen controller
 *
 * Wayne Hsiu, Copyright (c) 2010 Ideacom Technology Inc.
 * 	http://www.ideacom.com.tw/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 * 2011-11-23  Modify wayne : Add proximity sensor
 * 
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/earlysuspend.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <asm/io.h>
#include <linux/irq.h>

#include <mach/vreg.h>


#include <linux/spinlock.h>

//#include "idc6731_touch.h"
#include <linux/input/idc6731_touch.h>

//#define CONFIG_TOUCHSCREEN_IDC6731
//#define CONFIG_IDC6731_GPLUS_35

//#define CONFIG_RESET_PIN        
//#define CONFIG_SC_ONLY      
//#define CONFIG_DELAY_INIT_PANEL
//#define CONFIG_BTN_SUP


#ifndef IDC6731_FOP_RW
#define IDC6731_FOP_RW
#endif

#ifndef CONFIG_SUPPORT_IDC_FRM_UPG
#define CONFIG_SUPPORT_IDC_FRM_UPG
#endif

#ifdef CONFIG_BTN_SUP
//#define CONFIG_BTN_SUP_M1
#define CONFIG_BTN_SUP_M2
#endif
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// inverse first
#ifdef  CONFIG_TOUCHSCREEN_IDC6731
#define IDC6731_INVX    0
#define IDC6731_INVY    0
#define IDC6731_SWAPX   0
#endif

#ifdef CONFIG_IDC6731_GPLUS_35
#define IDC6731_INVX    1
#define IDC6731_INVY    0
#define IDC6731_SWAPX   1
#endif

#ifdef CONFIG_IDC6731_SWENC_43
#define IDC6731_INVX    1
#define IDC6731_INVY    0
#define IDC6731_SWAPX   1
#endif

#ifdef CONFIG_SC_ONLY
#define IDC_INIT_DELAY  500
#else
#define IDC_INIT_DELAY  2000
#endif

//--------------------------------------

#define IDC6731_CMDLONG     0x01
#define IDC6731_PROXIMITY   0x02

#define I2C_DRIVERID_IDC6731    1100   /* ideacom IDC6731 I2C multi-touch screen contoller */

#define IDC6731_I2C_NAME    "idc6731_ts"

#define IDC6731_I2C_VERSION "1.1"
#define IDC6731_I2C_RELEASE "2012-03-13"

#define IDC6731_I2C_ADDR	0x76

#define IDC6731_POLL_TIME       4000000 // 2ms

#define IDC6731_MAX_X_RANGE     320
#define IDC6731_MAX_Y_RANGE     480

#define IDC6731_DEF_STABILITY   0

#define IDC6731_I2C_TOUCHDATA_LEN   12

#define IDC6731_INACTIVE_LEVEL      1

const static u16 IDC6731_addr[] = { (IDC6731_I2C_ADDR >> 1), I2C_CLIENT_END };
static struct i2c_driver IDC6731_i2c_driver;

#define ST_TOUCH        0x81
#define ST_UNTOUCH      0x80

//--------------------------------------------------------------------------------
#ifdef CONFIG_BTN_SUP
#define IDC6731_BTN_X_START     0
#define IDC6731_BTN_Y_START     IDC6731_MAX_Y_RANGE - 50

#define IDC6731_BTN_X_END       IDC6731_MAX_X_RANGE
#define IDC6731_BTN_Y_END       IDC6731_MAX_Y_RANGE

#define IDC6731_X_START         0
#define IDC6731_Y_START         0
#define IDC6731_X_END           IDC6731_MAX_X_RANGE
#define IDC6731_Y_END           IDC6731_BTN_Y_START

#define IDC6731_MAX_BUTTON_SUP  3

static int gIDCButtons[IDC6731_MAX_BUTTON_SUP] = {BTN_0, BTN_1, BTN_2};
static int gIDCButtonPos[IDC6731_MAX_BUTTON_SUP][4] = 
{
    {0, IDC6731_MAX_X_RANGE / 3, IDC6731_BTN_Y_START, IDC6731_MAX_Y_RANGE},
    { IDC6731_MAX_X_RANGE / 3, IDC6731_MAX_X_RANGE * 2 / 3, IDC6731_BTN_Y_START, IDC6731_MAX_Y_RANGE},
    {IDC6731_MAX_X_RANGE * 2 / 3, IDC6731_MAX_X_RANGE, IDC6731_BTN_Y_START, IDC6731_MAX_Y_RANGE},
};

#else

#define IDC6731_X_START         0
#define IDC6731_Y_START         0
#define IDC6731_X_END           IDC6731_MAX_X_RANGE
#define IDC6731_Y_END           IDC6731_MAX_Y_RANGE

#endif
//--------------------------------------------------------------------------------
#define IDC6731_MAX_I2C_LEN 32
#define MAX_BUF_SIZE        64


//--------------------------------------------------------------------------------


typedef struct _PT_DATA_ {
	int st;
	int x, y;
}PTData;

typedef struct _DrvOpBuf_{
	union {
		struct _OPS_{
			char InvX;
			char InvY;
			char SwapXY;
		}Ops;
		char  Raw[3];
	};
} IDC_DrvOp;

struct IDC6731_DATA {
	uint16_t            addr;
	struct i2c_client   *client;
	struct input_dev    *input_dev;

	int                 itimer;
	struct hrtimer      timer;
	struct work_struct  work;
    struct work_struct  cmdwork;

	PTData              dpt[2];

	int                 gpio;
	int                 eintcfg;
	int                 inputcfg;
	char                *gpioname;
	int		            irq;

	IDC_DrvOp           DrvOp;

	//=============================
    int                 iDevFlag;

	int                 iTouchLen;
	int                 iCmdLen;
	int                 iRex;
	int                 iWex;

	int                 iCalOn;
    int                 iCalLock;
	int                 cmdlen;
	int                 bNewCmd;
	char                CmdBuf[MAX_BUF_SIZE];
	struct idc6731_platform_data *pdata;
	struct early_suspend early_suspend;

    int                 bProximity;
    //int                 bcnt;
#ifdef CONFIG_BTN_SUP
    int                 bPressBtn[IDC6731_MAX_BUTTON_SUP];
#endif
};

struct IDC6731_DATA IDC6731_data;

static struct workqueue_struct *ideacom_wq;

#ifdef CONFIG_HAS_EARLYSUSPEND
static void idc6731_ts_early_suspend(struct early_suspend *h);
static void idc6731_ts_late_resume(struct early_suspend *h);
#endif

#ifdef CONFIG_SUPPORT_IDC_FRM_UPG
static unsigned char gIDC6731Frm[]=
{
  #include "AW551_IDC6731_V1.h"
};
#endif


//------------- Internal function --------------------------------
int IDC6731_TryLock_Adaptor(struct i2c_adapter *adap, int bwait);
int IDC6731_Send_Unlock(struct i2c_client *client,const char *buf ,int count);
int IDC6731_Recv_Unlock(struct i2c_client *client, char *buf ,int count);

static int IDC6731_CheckStatus(unsigned char ch);
static int IDC6731_CheckStatus1(unsigned char ch);

//static ssize_t IDC_WriteData( char *buf, size_t nlen );

int IDC6731_ChkDevice_Active(void);

int IDC6731_GetTouchData( struct i2c_client *client, int addr, unsigned char *pbuf, int len );
int IDC6731_Input_SendTouch( struct IDC6731_DATA *ts, int pt, int st, int x, int y );

int IDC6731_Input_SendButton( struct IDC6731_DATA *ts, int st, int x, int y );


#ifdef IDC6731_FOP_RW
//static ssize_t IDC6731_fop_write( struct kobject *knod, struct bin_attribute *attr, char *buf, loff_t off, size_t nlen);
//static ssize_t IDC6731_fop_read(struct kobject *knod, struct bin_attribute *attr, char *buf, loff_t off, size_t nlen);
#endif


//==========================================================
/*
 * ATTRIBUTES:
 * search from boot log
 * /sys/devices/platform/s3c2440-i2c.0/i2c-0/xxx/DevOp [rw] 
 */





static void InitialDevParam(void)
{
    IDC6731_data.iCmdLen = 4;
    IDC6731_data.iRex = 0;
    IDC6731_data.iWex = 0;
    IDC6731_data.iTouchLen = IDC6731_I2C_TOUCHDATA_LEN;
	if( IDC6731_data.iDevFlag & (IDC6731_CMDLONG | IDC6731_PROXIMITY) ) {
        IDC6731_data.iRex = 3;
		IDC6731_data.iWex = 3;
    }
	IDC6731_data.iCmdLen += IDC6731_data.iWex;
	IDC6731_data.iTouchLen += IDC6731_data.iRex;
}

//================================================================================================================
#if 0
#define IDC_I2C_Send    i2c_master_send
#define IDC_I2C_Recv    i2c_master_recv
#define IDC_I2C_Lock    1
#else
#define IDC_I2C_Send    IDC6731_Send_Unlock
#define IDC_I2C_Recv    IDC6731_Recv_Unlock
#define IDC_I2C_Lock    IDC6731_TryLock_Adaptor
#endif

int IDC6731_TryLock_Adaptor(struct i2c_adapter *adap, int iRetry)
{
    int rt = 0;
    int timeout=iRetry;

Read_Retry:

		if (in_atomic() || irqs_disabled()) 

		{
//    rt = mutex_trylock(&IDC6731_data.client->adapter->bus_lock);
	rt = rt_mutex_trylock(&IDC6731_data.client->adapter->bus_lock);
    if (!rt) 

	{
	        if(iRetry) 
				{
					/*   I2C activity is ongoing. */
			            udelay(300);
			            timeout--;
			            if( timeout) {

			                goto Read_Retry;
			            } else
			                rt =-EBUSY;
	        		}
	}
		} 

		else {
			i2c_lock_adapter(adap);
		}
	
    return rt;





//    return 1;


}

int IDC6731_Send_Unlock(struct i2c_client *client,const char *buf ,int count)
{
	unsigned long orig_jiffies;
	int ret, try;
	struct i2c_adapter *adap=client->adapter;
	struct i2c_msg msg;

	msg.addr = client->addr;
	msg.flags = client->flags & I2C_M_TEN;
	msg.len = count;
	msg.buf = (char *)buf;

    /* Retry automatically on arbitration loss */
    orig_jiffies = jiffies;
    for (ret = 0, try = 0; try <= adap->retries; try++) {
        ret = adap->algo->master_xfer(adap, &msg, 1);
        if (ret != -EAGAIN)
            break;
        if (time_after(jiffies, orig_jiffies + adap->timeout))
            break;
    }
	/* If everything went ok (i.e. 1 msg transmitted), return #bytes
	   transmitted, else error code. */
	return (ret == 1) ? count : ret;
}

int IDC6731_Recv_Unlock(struct i2c_client *client, char *buf ,int count)
{
	struct i2c_adapter *adap=client->adapter;
	struct i2c_msg msg;
	unsigned long orig_jiffies;
	int ret, try;

	msg.addr = client->addr;
	msg.flags = client->flags & I2C_M_TEN;
	msg.flags |= I2C_M_RD;
	msg.len = count;
	msg.buf = buf;

    /* Retry automatically on arbitration loss */
    orig_jiffies = jiffies;
    for (ret = 0, try = 0; try <= adap->retries; try++) {
        ret = adap->algo->master_xfer(adap, &msg, 1);
        if (ret != -EAGAIN)
            break;
        if (time_after(jiffies, orig_jiffies + adap->timeout))
            break;
    }
	/* If everything went ok (i.e. 1 msg transmitted), return #bytes
	   transmitted, else error code. */
	return (ret == 1) ? count : ret;
}

static int IDC6731_CheckStatus(unsigned char ch)
{
	int     rt = 0;
	char    Buf[MAX_BUF_SIZE];
	int     ilen = 1;
	int     rlen = 4;
	char    rBuf[10];

	memset(Buf, 0, sizeof(Buf));
	memset(rBuf, 0, sizeof(rBuf));

	Buf[ilen++] = 0xA1;
	Buf[ilen++] = 0x01;
	ilen++;
	Buf[ilen++] = ch;

	rlen = 4;
	if( IDC6731_data.iDevFlag & IDC6731_CMDLONG) {
		Buf[ilen++] = 0xA1;
		Buf[ilen++] = 0x00;
		Buf[ilen++] = ch;
		rlen = 3;
	}

	rt = i2c_master_send(IDC6731_data.client, Buf, ilen); 
	if( rt != ilen) {
		goto Exit_GetStatus;
	}

	rt = i2c_master_recv( IDC6731_data.client, rBuf, rlen); 
	if( rt != rlen) {
		goto Exit_GetStatus;
	}

	if( rBuf[0] == 0) {
		rt = 0;
	} else
		rt = 1;

	if( (rBuf[1] == 0x67 ) && (rBuf[2] == 0x31) ){
		rt |= 0x80;
	}
Exit_GetStatus:
	return rt;
}

static int IDC6731_CheckStatus1(unsigned char ch)
{
	int     rt = 0;
	char    Buf[MAX_BUF_SIZE];
	int     ilen = 1;
	int     rlen = 4;
	char    rBuf[10];

	memset(Buf, 0, sizeof(Buf));
	memset(rBuf, 0, sizeof(rBuf));

	Buf[ilen++] = 0xA1;
	Buf[ilen++] = 0x01;
	ilen++;
	Buf[ilen++] = ch;

	rlen = 4;
	if( IDC6731_data.iDevFlag & IDC6731_CMDLONG) {
		Buf[ilen++] = 0xA1;
		Buf[ilen++] = 0x00;
		Buf[ilen++] = ch;
		rlen = 3;
	}

	rt = IDC_I2C_Send(IDC6731_data.client, Buf, ilen); 
	if( rt != ilen) {
        printk("W1\n");		
		goto Exit_GetStatus;
	}

    udelay(10);

	rt = IDC_I2C_Recv( IDC6731_data.client, rBuf, rlen); 
	if( rt != rlen) {
        printk("W2\n");
		goto Exit_GetStatus;
	}

	if( rBuf[0] == 0) {
		rt = 0;
	} else
		rt = 1;

	if( (rBuf[1] == 0x67 ) && (rBuf[2] == 0x31) ){
		rt |= 0x80;
	}
Exit_GetStatus:
	return rt;
}

int IDC6731_ChkDevice_Active(void)
{
    int rt = 1;
    int i;

    for( i = 0 ; i < 5; i++) {
        rt = IDC6731_CheckStatus1(0);
        if( rt >= 0) {
            rt = 0;
            break;
        }
        udelay(100);
    }

    return rt;
}

#ifdef IDC6731_FOP_RW

static ssize_t IDC_WriteData( char *buf, size_t nlen )
{
	char    Buf[MAX_BUF_SIZE];
	int     iRet = 0;
	int     Timeout;
	int     i, nTimes, moff, outlen;
	int     wlen;
	int     iCmd;
    char    chkch;


	//if( IDC6731_data.iCalOn == 0) {
    if( IDC6731_data.iCalLock == 0) {
        if( IDC_I2C_Lock(IDC6731_data.client->adapter, 100) < 0) {
            goto Exit_fop_write_Lock;
        }
		disable_irq(IDC6731_data.irq);
	}

    moff = 0;
    iCmd = 0;
    memset(Buf, 0, sizeof(Buf));

    if( buf[1] != 0) {
		memcpy(&(Buf[1]), buf, IDC6731_data.iCmdLen);
		// command out
		// out direct
        //printk("Command Out %x\n", buf[1]);
		outlen = IDC6731_data.iCmdLen + 1;
		iRet = IDC_I2C_Send(IDC6731_data.client, Buf, outlen);
		if( iRet != outlen) {
            if( buf[1] == 0x05 ) {
                IDC6731_data.iCalOn = 0;
                //if( buf[2] == 0) {
                    IDC6731_data.iCalLock = 0;
                //}
            }
            printk("Write Error 1 %d\n", iRet);
            goto Exit_fop_write;

		}
        udelay(300);
		iCmd = 1;
	} 

	if( buf[1] == 0x04 ) {
		iRet = nlen;
		IDC6731_data.iCalOn = 1;
        if( buf[2] != 0) {
            IDC6731_data.iCalLock = 1;
            //goto Exit_fop_write_Cal;
        }
        goto Exit_fop_write;
	} else if( buf[1] == 0x05 ) {
		IDC6731_data.iCalOn = 0;
        IDC6731_data.iCalLock = 0;
		iRet = nlen;
		goto Exit_fop_write;
        //goto Exit_fop_write_Cal;
	}

	moff = IDC6731_data.iCmdLen;

	if( nlen <= moff) {
		iRet = moff;
		goto Exit_fop_write;
	}

	nlen -= moff;

	if( nlen > IDC6731_MAX_I2C_LEN ) {
		moff += IDC6731_MAX_I2C_LEN;
		nlen -= IDC6731_MAX_I2C_LEN;

		outlen = IDC6731_MAX_I2C_LEN;
		nTimes = nlen / IDC6731_MAX_I2C_LEN;
		if( nlen % IDC6731_MAX_I2C_LEN ) {
			nTimes++;
		}

	} else {
		outlen = nlen;
		nTimes = 1;
	}

	if( nlen > IDC6731_MAX_I2C_LEN) {
		if(iCmd) {
			Timeout = 2000;
			udelay(300);
            chkch = 0xA1;
			while( (IDC6731_CheckStatus1( ~chkch ) & 0x01)) {
				mdelay(1);
				Timeout--;
				if( Timeout < 0) {
					iRet = -EIO;
					printk("Write fail timeout 1\n");
					goto Exit_fop_write;
				}
			}
			if(buf[1] == 0x0d ) {
				mdelay(2000);
			} else
				mdelay(100);
		} 
	}
	//printk("Write : olen:%d, nTimes: %d, nlen:%d, off:%d, %x\n", outlen, nTimes, nlen, moff, buf[moff]);
	// out data
	Buf[0] = 0;
	for( i = 0 ; i < nTimes; i++) {
		Timeout=2000;
		//printk("Write : %d olen:%d, off:%d\n", i, outlen, moff);
		memcpy(&(Buf[1]), &(buf[moff]), IDC6731_MAX_I2C_LEN );
		wlen = 1 + outlen + IDC6731_data.iWex;

		if( i > 0 )  mdelay(1);
        chkch = Buf[1];
		while( (IDC6731_CheckStatus1( ~chkch ) & 0x01)) {
			mdelay(1);
			Timeout--;
			if( Timeout < 0) {
				iRet = -EIO;
				printk("Write fail %d, timeout\n", i);
				goto Exit_fop_write;
			}
		}
		udelay(300);
		iRet = IDC_I2C_Send(IDC6731_data.client, Buf, wlen);
		if( iRet != wlen) {
			printk("Write fail %d, Ret:%d\n", i, iRet);
			i = nTimes;
			goto Exit_fop_write;
		}

		moff += outlen;
	}

    iRet = moff;
	mdelay(50); // wait device complete, without irq

Exit_fop_write:
    //if( IDC6731_data.iCalOn == 0) {
	if( IDC6731_data.iCalLock == 0) {
       // mutex_unlock(&IDC6731_data.client->adapter->bus_lock);
    	rt_mutex_unlock(&IDC6731_data.client->adapter->bus_lock);
		enable_irq(IDC6731_data.irq);
	}
//Exit_fop_write_Cal:
Exit_fop_write_Lock:
    //printk("Write : ret %d\n", iRet);
	return iRet;
}


static ssize_t IDC6731_fop_write(struct file *file, struct kobject *knod, struct bin_attribute *attr, char *buf, loff_t off, size_t nlen)
{
	int iRet = 0;


    //printk("%s %x %x %x %x, len:%d\n", __FUNCTION__, buf[off], buf[off+1], buf[off+2], buf[off+3], nlen);
	// Queue command for ICommand
	switch( buf[0]) {
		case 0xA1:
			IDC6731_data.cmdlen = nlen;
			IDC6731_data.bNewCmd = 1;
			memcpy(&(IDC6731_data.CmdBuf[1]), buf, nlen);
			iRet = nlen;
			//return iRet;
            break;
		case 0xA2:
			IDC6731_data.cmdlen = nlen;
			IDC6731_data.bNewCmd = 1;
			memcpy(&(IDC6731_data.CmdBuf[1]), buf, nlen);
			iRet = nlen;
			break;
			//case 0xA1:
		case 0x21:

			iRet = IDC_WriteData(buf, nlen);
			break;
		case 0x22:
			memcpy(IDC6731_data.DrvOp.Raw, &(buf[1]), nlen - 1);
			iRet = nlen;
			break;
	}
    //printk("%s ret:%d\n", __FUNCTION__, iRet);
	return iRet;
}




static ssize_t IDC6731_fop_read(struct file *file, struct kobject *knod, struct bin_attribute *attr, char *buf, loff_t off, size_t nlen)
{
	int     rt = 0;
	int     i, nTimes;
	int     moff, rlen, cmdlen;
	int     Timeout;
	char    rBuf[MAX_BUF_SIZE];

	//printk(KERN_ERR "%s len:%d\n",__FUNCTION__, nlen );

    //if( IDC6731_data.iCalOn == 0) {
    if( IDC6731_data.iCalLock == 0) {
        if( IDC_I2C_Lock(IDC6731_data.client->adapter, 100) < 0) {
            goto Exit_fop_read_Lock;
        }
        disable_irq(IDC6731_data.irq);
	}

	if( IDC6731_data.CmdBuf[1] == 0xA2) {
		buf[0] = 0xA2;
		memcpy(&(buf[1]), IDC6731_data.DrvOp.Raw, nlen - 1);
		goto Exit_fop_read;
	}

	if( IDC6731_data.bNewCmd ) {
        if( IDC6731_ChkDevice_Active()) {
            printk("Awake device fail\n");
			goto Exit_fop_read;
        }

		// Issue command which queue at OCommand
		rlen = IDC6731_data.cmdlen + 1;
		rt = IDC_I2C_Send(IDC6731_data.client, IDC6731_data.CmdBuf, rlen);
		if( rt != rlen) {
			printk("Read Error 1 %d\n", rt);
			goto Exit_fop_read;
		}
        udelay(300);
	}
	IDC6731_data.bNewCmd = 0;

	//memset(buf, 0, nlen);
	moff = 0;
	nTimes = 1;
	if( nlen <= IDC6731_MAX_I2C_LEN) {
		rlen = nlen;
	} else {
		rlen = IDC6731_MAX_I2C_LEN;
		nTimes = nlen / IDC6731_MAX_I2C_LEN - 1;
		buf[0] = IDC6731_data.CmdBuf[2];
		moff = IDC6731_MAX_I2C_LEN;
	}
	cmdlen = rlen + IDC6731_data.iRex;
	//printk("Read : rlen:%d, nTimes: %d, cmdlen:%d, moff:%d\n", rlen, nTimes, cmdlen, moff);
	for( i = 0 ; i < nTimes; i++) {
		Timeout = 1000;
		// get status
		while( (IDC6731_CheckStatus1( 0 ) & 0x01) ) {
			mdelay(1);
			Timeout--;
			if( Timeout < 0) {
				rt = -EIO;
				printk("Read fail %d, timeout\n", i);
				goto Exit_fop_read;
			}
		}
		udelay(300);
		rt = IDC_I2C_Recv(IDC6731_data.client, rBuf, cmdlen );
		if( rt != cmdlen) {
			printk("Read fail %d\n", rt);
			goto Exit_fop_read;
		}
		memcpy(&(buf[moff]), rBuf, rlen );
		moff += rlen;
        udelay(300);
	}
    rt = moff;
Exit_fop_read:
	//if( IDC6731_data.iCalOn == 0) {
    if( IDC6731_data.iCalLock == 0) {
        //mutex_unlock(&IDC6731_data.client->adapter->bus_lock);
	rt_mutex_unlock(&IDC6731_data.client->adapter->bus_lock);
		enable_irq(IDC6731_data.irq);
	}
    
Exit_fop_read_Lock:
	return rt;
}

#endif


#ifdef CONFIG_SUPPORT_IDC_FRM_UPG
static int IDC6731_ReadCmd(char cmd, int off, unsigned char *rBuf, int rlen)
{
	int     rt = 0;
	char    Buf[MAX_BUF_SIZE];
	int     ilen = 0;

	memset(Buf, 0, sizeof(Buf));

	Buf[ilen++] = 0xA1;
	Buf[ilen++] = cmd;
    Buf[ilen++] = (off>>8) & 0x0fF;
    Buf[ilen++] = off & 0x0FF;
	if( IDC6731_data.iDevFlag & IDC6731_CMDLONG) {
		Buf[ilen++] = 0xA1;
		Buf[ilen++] = 0x00;
		Buf[ilen++] = 0;
	}

    IDC6731_data.cmdlen = ilen;
    IDC6731_data.bNewCmd = 1;
    memcpy(&(IDC6731_data.CmdBuf[1]), Buf, ilen);

	rt = IDC6731_fop_read( NULL, NULL, NULL, rBuf, 0, rlen);

	return (rt == rlen)? rlen : 0;
}

static int IDC6731_WriteCmd(char cmd, int off, unsigned char *wBuf, int len)
{
	int     rt = 0;
	char    Buf[MAX_BUF_SIZE];
	int     ilen = 0;

	memset(Buf, 0, sizeof(Buf));

	Buf[ilen++] = 0x21;
	Buf[ilen++] = cmd;
    Buf[ilen++] = (off>>8) & 0x0fF;
    Buf[ilen++] = off & 0x0FF;
	if( IDC6731_data.iDevFlag & IDC6731_CMDLONG) {
		Buf[ilen++] = 0xA1;
		Buf[ilen++] = 0x00;
		Buf[ilen++] = ~cmd;
	}
    memcpy(&(Buf[ilen]), wBuf, len );
    ilen += len;
	rt = IDC_WriteData( Buf, ilen);

	return (rt == ilen)? ilen : 0;
}

int IDC6731_CheckFrmUpg(void)
{
    int rt = 0;
    char Buf[MAX_BUF_SIZE];

    memset(Buf, 0, sizeof(Buf));
    rt = IDC6731_WriteCmd( 0x04, 0x100, NULL, 0);
    if( !rt)  goto Exit_CheckFrmUpg1;


    // get device info
    rt = IDC6731_ReadCmd(0x02, 0, Buf, 8);
    if( !rt )  goto Exit_CheckFrmUpg;
    if( memcmp(Buf, &(gIDC6731Frm[0x77E0]), 8) == 0 ) {
        // get status
        rt = IDC6731_ReadCmd(0x03, 0, Buf, 8);
        if( !rt )  goto Exit_CheckFrmUpg;
        if( memcmp(Buf, &(gIDC6731Frm[0x77E0 + 8]), 8) == 0 ) return 0;
    }
    // check 4k
    // full upgrade mode
    rt = IDC6731_ReadCmd(0x08, 0x21, Buf, 4);
    if( !rt )  goto Exit_CheckFrmUpg;

    Buf[0] |= 0x40;
    rt = IDC6731_WriteCmd(0x08, 0x21, Buf, 4);
Exit_CheckFrmUpg:
    IDC6731_WriteCmd( 0x05, 0, NULL, 0);

Exit_CheckFrmUpg1:    
    return rt;
}




int IDC6731_FrmUpg( void )
{
    char    TmpBuf[IDC6731_MAX_I2C_LEN+5]; 
    int     iRet = 0;
    int     i, iCmdlen;
    int     outlen;
    int     Timeout;
    int     nTimes;
    char    chkch;

    if(IDC6731_CheckFrmUpg() == 0 ) {
        return iRet;
    }

    iRet = IDC6731_WriteCmd( 0x04, 0x100, NULL, 0);
    if( !iRet)  goto Exit_FrmUpg1;
    udelay(100);

    memset(TmpBuf, 0, sizeof(TmpBuf));
    iCmdlen = 1;
    TmpBuf[iCmdlen++] = 0x21;
    TmpBuf[iCmdlen++] = 0x0D;
    TmpBuf[iCmdlen++] = 0x00;
    TmpBuf[iCmdlen++] = 0x00;
#ifdef BYTE7
    TmpBuf[iCmdlen++] = 0xA1;
    TmpBuf[iCmdlen++] = 0x00;
    TmpBuf[iCmdlen++] = 0;
#endif
    outlen = IDC6731_data.iCmdLen + 1 ;
    iRet = IDC_I2C_Send(IDC6731_data.client, TmpBuf, outlen);
    if( iRet != outlen) {
        printk("Write Command Error 1 %d\n", iRet);
        goto Exit_FrmUpg;
    }

    Timeout = 2000;
    udelay(300);
    chkch = 0xA1;
    while( (IDC6731_CheckStatus1( ~chkch ) & 0x01)) {
        mdelay(1);
        Timeout--;
        if( Timeout < 0) {
            iRet = -EIO;
            printk("wait fail timeout 1\n");
            goto Exit_FrmUpg;
        }
    }
	mdelay(200);
    nTimes = 1024 * 32 / IDC6731_MAX_I2C_LEN;

    outlen = IDC6731_MAX_I2C_LEN + 1 + IDC6731_data.iWex;
    memset(TmpBuf, 0, sizeof(TmpBuf));
    for( i = 0 ; i < nTimes; i++) {
        memcpy(&(TmpBuf[1]), &(gIDC6731Frm[i*IDC6731_MAX_I2C_LEN]), IDC6731_MAX_I2C_LEN);

        Timeout=2000;
        chkch = TmpBuf[1];
        while( (IDC6731_CheckStatus1( ~chkch ) & 0x01)) {
            mdelay(1);
            Timeout--;
            if( Timeout < 0) {
                iRet = -EIO;
                printk("wait fail %d, timeout 3\n", i);
                goto Exit_FrmUpg;
            }
        }
        udelay(100);
        iRet = IDC_I2C_Send(IDC6731_data.client, TmpBuf, outlen);
        if( iRet != outlen) {
            printk("Write fail %d, Ret:%d\n", i, iRet);
            i = nTimes;
            goto Exit_FrmUpg;
        }
        udelay(500);
    }
    printk("Frmupg finish\n");
    iRet = 1024 * 32;
	mdelay(5000); // wait device complete, without irq
Exit_FrmUpg:
    IDC6731_WriteCmd( 0x05, 0, NULL, 0);
Exit_FrmUpg1:
    return iRet;
}


#endif

static struct bin_attribute IDC6731_attr = {
	.attr = {
		.name = "DevOp",
		.mode = S_IRWXU | S_IRWXG | S_IRWXO,
        //.owner = THIS_MODULE,
	},
	.size = 2048,
#ifdef IDC6731_FOP_RW
	.read = IDC6731_fop_read,
	.write = IDC6731_fop_write,
#endif
};




int IDC6731_GetTouchData( struct i2c_client *client, int addr, unsigned char *pbuf, int len )
{
	int     rt = 0;
	unsigned char buf[10];
	int     olen;
    

	memset(buf, 0,sizeof(buf));
    memset( pbuf, 0, len);

	olen = 3;
	buf[1] = 0x55;
	buf[2] = 0xAA;

    if( IDC_I2C_Lock(IDC6731_data.client->adapter, 0) < 0) {
        printk(KERN_ERR"IDC6731: Adapter busy\n");
        goto Exit_GetTouchData_Lock;
    }

	rt = IDC_I2C_Send(IDC6731_data.client, buf, olen);
//	rt = i2c_master_send(IDC6731_data.client, buf, olen);
	if( rt != olen ) {
		printk(KERN_ERR "IDC6731: Send fail %d\n", rt);
		goto Exit_GetTouchData;
	}

	
	rt = IDC_I2C_Recv(IDC6731_data.client, pbuf, len);
//	rt = i2c_master_recv(IDC6731_data.client, pbuf, len);
	if( rt != len ) 
	{
		printk(KERN_ERR "IDC6731: Read fail %d\n", rt );
	}
Exit_GetTouchData:
//    mutex_unlock(&IDC6731_data.client->adapter->bus_lock);
    rt_mutex_unlock(&IDC6731_data.client->adapter->bus_lock);
Exit_GetTouchData_Lock:
	return rt;
}

int IDC6731_Input_SendTouch( struct IDC6731_DATA *ts, int pt, int st, int x, int y )
{
	//int rt = -EIO;
	int bpress = 0;
	int tmp;

	bpress = st & 0x0F;

/*
	if ((x < IDC6731_X_START) || (y < IDC6731_Y_START) || (x > IDC6731_X_END) || (y > IDC6731_Y_END)){
        // check button
        printk("Over range\n");
		return -EIO;
    }
*/
	if( ts->DrvOp.Ops.InvX ) {
		x = IDC6731_MAX_X_RANGE - x;
	}
	if( ts->DrvOp.Ops.InvY) {
		y = IDC6731_MAX_Y_RANGE - y;
	}

	// change scale
	if( ts->DrvOp.Ops.SwapXY) {
		tmp = x;
		x = y;
		y = tmp;
	}
    //printk("(%d, %02x, %04x, %04x)\n", pt, st, x, y);
	input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, bpress);
	input_report_abs(ts->input_dev, ABS_MT_WIDTH_MAJOR, bpress );
	input_report_abs(ts->input_dev, ABS_MT_POSITION_X, x);
	input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, y);
	input_mt_sync(ts->input_dev);
#if 0
    if( !bpress ) {
        ts->bcnt++;
        printk("break %d\n", ts->bcnt);
    }
#endif
	return bpress;
}

#ifdef CONFIG_BTN_SUP
int IDC6731_Input_SendButton( struct IDC6731_DATA *ts, int st, int x, int y )
{
    int i;
    int bpress = st & 0x0F;
    int rt = 0;
    
    for( i = 0 ; i < IDC6731_MAX_BUTTON_SUP; i++) {
        if( (x > gIDCButtonPos[i][0]) &&
            (x < gIDCButtonPos[i][1]) &&
            (y > gIDCButtonPos[i][2]) &&
            (y < gIDCButtonPos[i][3])
            ) {
            rt = 1;
            if( bpress ) {
                if( ts->bPressBtn[i] == 0 ) {
                    input_report_key(ts->input_dev, gIDCButtons[i], bpress);
                    printk("SendBtn press %d\n", gIDCButtons[i]);
                    ts->bPressBtn[i] = 1;
                }
            } else {
                if( ts->bPressBtn[i] == 1 ) {
                    input_report_key(ts->input_dev, gIDCButtons[i], bpress);
                    printk("SendBtn release %d\n", gIDCButtons[i]);
                    ts->bPressBtn[i] = 0;
                }
            }
        }
    }
    return rt;
}
#endif

#ifdef CONFIG_BTN_SUP_M2
void IDC6731_CheckAndSendBtn(struct IDC6731_DATA *ts, unsigned char buf[])
{
    int i, j;
    int bpress, x, y;
    int off;
    int nFinger = ts->iTouchLen / 6;
    int bTreat[10];

    memset(bTreat,0, sizeof(bTreat));
    for( i = 0 ; i < IDC6731_MAX_BUTTON_SUP; i++ ) {
        for( j = 0 ; j < nFinger; j++) {
            off = j * 6;
            bpress = buf[off] & 0x0F;
            y = buf[off + 1] * 256 + buf[off + 2];
            x = buf[off + 3] * 256 + buf[off + 4];

            if( (x > gIDCButtonPos[i][0]) &&
                (x < gIDCButtonPos[i][1]) &&
                (y > gIDCButtonPos[i][2]) &&
                (y < gIDCButtonPos[i][3])
                ) {
                bTreat[i] = 1;
                if( bpress ) {
                    if( ts->bPressBtn[i] == 0 ) {
                        input_report_key(ts->input_dev, gIDCButtons[i], bpress);
                        //printk("SendBtn press %d\n", gIDCButtons[i]);
                        ts->bPressBtn[i] = 1;
                    }
                } else {
                    if( ts->bPressBtn[i] == 1 ) {
                        input_report_key(ts->input_dev, gIDCButtons[i], bpress);
                        //printk("SendBtn release %d\n", gIDCButtons[i]);
                        ts->bPressBtn[i] = 0;
                    }
                }
            }
        }
        
    }
    for( i = 0 ; i < IDC6731_MAX_BUTTON_SUP; i++) {
        if( (bTreat[i] == 0 ) && (ts->bPressBtn[i] == 1)) {
            input_report_key(ts->input_dev, gIDCButtons[i], 0);
            //printk("SendBtn release %d\n", gIDCButtons[i]);
            ts->bPressBtn[i] = 0;
        }
    }
}
#endif

static void IDC6731_Report_Touch(struct IDC6731_DATA *ts)
{
	unsigned char buf[IDC6731_MAX_I2C_LEN];
	int     i = 0;
	int     rt = 0;
	int     off;
    int     nFinger;
    int     bProx;
    int     bSend;

    //printk("%s 1\n", __FUNCTION__);

	printk("%s ts->addr:%x,ts->iTouchLen:%x,ts->irq:%d\n", __FUNCTION__,ts->addr,ts->iTouchLen,ts->irq);
 
	rt = IDC6731_GetTouchData(ts->client, ts->addr, buf, ts->iTouchLen);
	printk("%s rt:%d\n", __FUNCTION__,rt);
	if( rt != ts->iTouchLen ){
		printk(KERN_ERR "Can't Read\n");
		goto Exit_Report_Touch;
	}

    
    nFinger = ts->iTouchLen / 6;
    bSend = 0;
    // check button for all buffer
    #ifdef CONFIG_BTN_SUP_M2
    IDC6731_CheckAndSendBtn(ts, buf);
    #endif
	for( i = 0 ; i < nFinger; i++ ) {
		off = i * 6;

        if( ts->dpt[i].st == ST_TOUCH) {
            if( buf[off] == 0) {
                buf[off] = ST_UNTOUCH;
            }
        }

		if( (buf[off] != ST_TOUCH ) &&
				(buf[off] != ST_UNTOUCH )){
			//printk("buf=%x to 0\n", buf[off]);
			buf[off] = 0;
			continue;
		}
		ts->dpt[i].st = buf[off];
        ts->dpt[i].y = buf[off + 1] * 256 + buf[off + 2];
        ts->dpt[i].x = buf[off + 3] * 256 + buf[off + 4];
#ifdef CONFIG_BTN_SUP
        // check button
        if( (ts->dpt[i].x > IDC6731_BTN_X_START) && 
            (ts->dpt[i].y > IDC6731_BTN_Y_START) && 
            (ts->dpt[i].x < IDC6731_BTN_X_END)    && 
            (ts->dpt[i].y < IDC6731_BTN_Y_END)
          ) 
        {
            #ifdef CONFIG_BTN_SUP_M1
            bSend |= IDC6731_Input_SendButton(ts, ts->dpt[i].st, ts->dpt[i].x, ts->dpt[i].y );
            #endif
        } else 
#endif
            bSend |= IDC6731_Input_SendTouch(ts, i, ts->dpt[i].st, ts->dpt[i].x, ts->dpt[i].y );
	}
#ifdef CONFIG_BTN_SUP_M1
    // all untouch
    if( bSend == 0) {
        for( i = 0 ; i < IDC6731_MAX_BUTTON_SUP;i++) {
            ts->bPressBtn[i] = 0;
        }
    }
#endif

    if(IDC6731_data.iDevFlag & IDC6731_PROXIMITY) {
        bProx = buf[12] & 0x01;
        if( bProx != ts->bProximity) {
            printk(KERN_ERR "Send Proximity %x\n", buf[12]);
            ts->bProximity = bProx;
            input_report_abs(ts->input_dev, ABS_DISTANCE, ts->bProximity ? 0 : 1);
        }
    }

	input_sync(ts->input_dev);
Exit_Report_Touch:
	enable_irq(ts->irq);
	return;
}

static void IDC6731_work_func(struct work_struct *work)
{
	struct IDC6731_DATA *ts = container_of(work, struct IDC6731_DATA, work);

	IDC6731_Report_Touch(ts);
}

static irqreturn_t IDC6731_irq_handler(int irq, void *dev_id)
{
	struct IDC6731_DATA *ts = (struct IDC6731_DATA*)dev_id;

	//pr_info("IDC6731_irq\n");
    if( 1/*ts->iCalOn == 0 */) {
	 printk(KERN_ERR "IDC6731_irq_handler ts->irq %d\n", ts->irq);
        disable_irq_nosync(ts->irq);
        queue_work(ideacom_wq, &ts->work);
    }

	return IRQ_HANDLED;
}

int IDC6731_DevSuspend(struct i2c_client *client)
{
    char    Buf[10];
    memset(Buf, 0, sizeof(Buf));
    Buf[0] = 0x00;
    Buf[1] = 0x21;
    Buf[2] = 0x03;
    Buf[5] = 0xa1;
    i2c_master_send(client, Buf, 8);
    return 1;
}

int IDC6731_DevReInitPanel(struct i2c_client *client)
{
    char    Buf[10];
    memset(Buf, 0, sizeof(Buf));
    Buf[0] = 0x00;
    Buf[1] = 0x21;
    Buf[2] = 0x0e;
    Buf[5] = 0xa1;
    i2c_master_send(client, Buf, 8);
    return 1;
}

extern int is_tp_driver_loaded;

static int IDC6731_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	int rt = 0;
    int i;
	struct input_dev *pidev;
	struct idc6731_platform_data * pdata = client->dev.platform_data;

	if(is_tp_driver_loaded)
	{
		printk(KERN_ERR"IDC6731_probe other driver has been loaded\n");
		return -ENODEV;
	}

	INIT_WORK(&IDC6731_data.work, IDC6731_work_func);
	IDC6731_data.client = client;
	IDC6731_data.addr = client->addr;
	IDC6731_data.itimer = IDC6731_POLL_TIME;
	IDC6731_data.pdata = pdata;

	printk(KERN_ERR "IDC6731_probe IDC6731_data.addr  :%x\n",IDC6731_data.addr );

    //IDC6731_data.bcnt = 0;
    IDC6731_data.bProximity = 0;

	i2c_set_clientdata(client, &IDC6731_data);

    rt = -ENODEV;
    for( i = 0 ; i < 3 ; i++) {
        rt = IDC6731_CheckStatus(0);
        if( rt >= 0 ) break;
        msleep(50);
    }
    if( rt < 0 ) {
        printk(KERN_ERR "IDC6731 attached: can't find IDC6731\n");
        goto err_idc6731_devscan_failed;
    }


#ifdef CONFIG_SUPPORT_IDC_FRM_UPG
	IDC6731_FrmUpg();
#endif

#ifdef CONFIG_DELAY_INIT_PANEL
    IDC6731_DevReInitPanel(client);
    msleep(IDC_INIT_DELAY);
#endif

	// create input device
	IDC6731_data.input_dev = pidev = input_allocate_device();
	if (IDC6731_data.input_dev == NULL) {
		rt = -ENOMEM;
		printk(KERN_ERR "IDC6731_attach: Failed to allocate input device\n");
		goto err_input_dev_alloc_failed;
	}

	pidev->name = client->name;
	pidev->id.bustype = BUS_I2C;
	pidev->dev.parent = &(client->dev);
	pidev->id.vendor = 0x1CB6;
	pidev->id.product = 0x6731;
	pidev->id.version = 0x0100;

	set_bit(EV_SYN, pidev->evbit);
	set_bit(EV_KEY, pidev->evbit);
	set_bit(EV_ABS, pidev->evbit);
    

	input_set_abs_params(pidev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0); // pressure, to denote multi touch pointer is touched.
	input_set_abs_params(pidev, ABS_MT_WIDTH_MAJOR, 0, 15, 0, 0); 
	input_set_abs_params(pidev, ABS_MT_POSITION_X, IDC6731_X_START, IDC6731_X_END, IDC6731_DEF_STABILITY, 0);
	input_set_abs_params(pidev, ABS_MT_POSITION_Y, IDC6731_Y_START, IDC6731_Y_END, IDC6731_DEF_STABILITY, 0);
    if( IDC6731_PROXIMITY ) {
        // proximity button
        input_set_abs_params(pidev, ABS_DISTANCE, 0, 1, 0, 0);
    }


	set_bit(ABS_X, pidev->absbit);
	set_bit(ABS_Y, pidev->absbit);
	set_bit(ABS_PRESSURE, pidev->absbit);
	set_bit(BTN_TOUCH, pidev->keybit);

	set_bit(EV_ABS, pidev->evbit);
	set_bit(EV_SYN, pidev->evbit);
	set_bit(EV_KEY, pidev->evbit);
	set_bit(BTN_MISC, pidev->keybit);
	set_bit(KEY_OK, pidev->keybit);


#ifdef CONFIG_BTN_SUP
    // set button
    for( i = 0 ; i < IDC6731_MAX_BUTTON_SUP; i++) {
        set_bit(gIDCButtons[i], pidev->keybit);
    }
#endif

	rt = sysfs_create_bin_file(&client->dev.kobj, &IDC6731_attr);
	if (rt) {
		printk(KERN_ERR "Unable to export commandpath, error: %d\n",rt);
		goto err_input_register_device_sysfs;
	}

	rt = input_register_device(pidev);
	if (rt) {
		printk(KERN_ERR "IDC6731_attach: Unable to register %s input device\n", pidev->name);
		goto err_input_register_device_failed;
	}

	// aquire gpio and irq
	IDC6731_data.irq = IDC6731_data.pdata->irq_gpio;//client->irq;
	IDC6731_data.gpioname = IDC6731_I2C_NAME;

	printk(KERN_ERR "IDC6731_probe IDC6731_data.irq :%d\n",IDC6731_data.irq);
	
	if( IDC6731_data.irq >= 0) {
		rt = request_irq( IDC6731_data.irq, IDC6731_irq_handler, 
				/*IRQF_TRIGGER_FALLING | IRQF_TIMER | IRQF_DISABLED,*/
				IRQF_TRIGGER_FALLING,
				IDC6731_data.gpioname, &IDC6731_data);
		if( rt ) {
			printk(KERN_ERR "IDC6731_attach: request_irq fail (%d)\n", rt );
		} else {
			printk(KERN_ERR "IDC6731_attach: request_irq : %d\n", IDC6731_data.irq);
			
			
		}
	} else {
		printk(KERN_ERR "IDC6731_attach: irq map fail (%d)\n", IDC6731_data.irq );
	}


	if(	0 != IDC6731_data.pdata->_pfCraeteVirKey()){
		printk(KERN_ERR"idc6731_ts_probe: CreateVirtualKey Failed");
	}
	

#ifdef CONFIG_HAS_EARLYSUSPEND
	IDC6731_data.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	IDC6731_data.early_suspend.suspend = idc6731_ts_early_suspend;
	IDC6731_data.early_suspend.resume = idc6731_ts_late_resume;
	register_early_suspend(&IDC6731_data.early_suspend);
#endif
	is_tp_driver_loaded = 1;
	printk(KERN_ERR "IDC6731 probe successfully\n");
	return rt;

err_input_register_device_sysfs:
	sysfs_remove_bin_file(&client->dev.kobj, &IDC6731_attr);
err_input_register_device_failed:
err_input_dev_alloc_failed:
err_idc6731_devscan_failed:
	return rt;
}

static int IDC6731_remove(struct i2c_client *client)
{
	struct IDC6731_DATA *pData = (struct IDC6731_DATA *)i2c_get_clientdata(client);

	printk(KERN_ERR "IDC6731_remove\n");

#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&IDC6731_data.early_suspend);
#endif

    free_irq(client->irq, pData);

	sysfs_remove_bin_file(&client->dev.kobj, &IDC6731_attr);
	input_unregister_device(pData->input_dev);

	i2c_set_clientdata(client, NULL);
	if( IDC6731_data.client )
		kfree( IDC6731_data.client );
	free_irq( pData->irq, IDC6731_data.gpioname );
	return 0;
}


static int IDC6731_suspend(struct i2c_client *client, pm_message_t mesg)
{
	
	struct IDC6731_DATA *ts = i2c_get_clientdata(client);
	int ret;

	printk(KERN_ERR "%s\n", __FUNCTION__);

	disable_irq(ts->irq);
	ret = cancel_work_sync(&ts->work);
	if (ret)
		enable_irq(ts->irq);

	switch( mesg.event) {
		case PM_EVENT_SUSPEND:
		case PM_EVENT_HIBERNATE:
			IDC6731_DevSuspend(client);
			break;
	}
    
	return 0;
}

static int IDC6731_resume(struct i2c_client *client)
{
	struct IDC6731_DATA *pData = (struct IDC6731_DATA *)i2c_get_clientdata(client);
    int     i;

	printk(KERN_ERR "%s\n", __FUNCTION__);

#ifdef CONFIG_RESET_PIN
	if (pData->pdata)
		pData->pdata->reset();
    udelay(300);
#endif 
    //pData->bcnt = 0;
    pData->bProximity = 0;
    
    for( i = 0 ; i < 3; i++) {
        IDC6731_CheckStatus(0);
    }
    enable_irq(pData->irq);

    
	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void idc6731_ts_early_suspend(struct early_suspend *h)
{
	struct IDC6731_DATA *ts;
	ts = container_of(h, struct IDC6731_DATA, early_suspend);
	IDC6731_suspend(ts->client, PMSG_SUSPEND);
}

static void idc6731_ts_late_resume(struct early_suspend *h)
{
	struct IDC6731_DATA *ts;
	ts = container_of(h, struct IDC6731_DATA, early_suspend);
	IDC6731_resume(ts->client);
}
#endif

static const struct i2c_device_id idc6731_i2c_id[] = {
	{ IDC6731_I2C_NAME, 0 },
	{ }
};

static struct i2c_driver IDC6731_i2c_driver = {
	.driver = {
		.name   = IDC6731_I2C_NAME,
		.owner  = THIS_MODULE,
	},
	.id_table = idc6731_i2c_id,
	.probe = IDC6731_probe,
	.remove = IDC6731_remove,
};

static __init int IDC6731_init(void)
{
	int rt = -ENOMEM;

	printk(KERN_ERR "IDC6731_init *******\n");
    printk(KERN_ERR "IDC6731_init version %s %s\n", IDC6731_I2C_VERSION, IDC6731_I2C_RELEASE);

	memset( &IDC6731_data, 0, sizeof( struct IDC6731_DATA ));
	IDC6731_data.DrvOp.Ops.InvX = IDC6731_INVX;
	IDC6731_data.DrvOp.Ops.InvY = IDC6731_INVY;
	IDC6731_data.DrvOp.Ops.SwapXY = IDC6731_SWAPX;
	IDC6731_data.iDevFlag = IDC6731_CMDLONG | IDC6731_PROXIMITY;
	InitialDevParam();

	ideacom_wq = create_singlethread_workqueue("ideacom_wq");

	if (ideacom_wq) {
		rt = i2c_add_driver(&IDC6731_i2c_driver);
		printk(KERN_ERR "IDC6731_init rt=%x\n", rt);
	}
	return rt;
}

static __init void IDC6731_exit(void)
{
	i2c_del_driver(&IDC6731_i2c_driver);
	if (ideacom_wq)
		destroy_workqueue(ideacom_wq);
}

	module_init(IDC6731_init)
	module_exit(IDC6731_exit)

	MODULE_AUTHOR("Wayne <wayne_hsiu@mail.ideacom.com.tw>");
	MODULE_DESCRIPTION("Ideacom IIC based multi-touch contoller driver");
	MODULE_LICENSE("GPL");
	MODULE_ALIAS("device:idc6731_ts");
    MODULE_VERSION("1.0");

