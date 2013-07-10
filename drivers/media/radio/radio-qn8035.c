/*
 * driver/media/radio/radio-QN8035.c
 *
 * Driver for QN8035 radio chip for linux 2.6.
 * This driver is for QN8035 chip from NXP, used in EZX phones from Motorola.
 * The I2C protocol is used for communicate with chip.
 *
 * Based in radio-tea5761.c Copyright (C) 2005 Nokia Corporation
 *
 *  Copyright (c) 2008 Fabio Belavenuto <belavenuto@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * History:
 * 2008-12-06   Fabio Belavenuto <belavenuto@gmail.com>
 *              initial code
 *
 * TODO:
 *  add platform_data support for IRQs platform dependencies
 *  add RDS support
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <mach/gpio.h>
#include <linux/init.h>			/* Initdata			*/
#include <linux/delay.h>
#include <linux/videodev2.h>		/* kernel radio structs		*/
#include <linux/i2c.h>			/* I2C				*/
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <linux/version.h>      	/* for KERNEL_VERSION MACRO     */
#include <mach/vreg.h>
#include <linux/qn8035.h>
#include <linux/slab.h>


#define DRIVER_VERSION	"v0.01"
#define RADIO_VERSION	KERNEL_VERSION(0, 0, 1)

#define DRIVER_AUTHOR	"Teng Jingdong <stony.tjd@gmail.com>"
#define DRIVER_DESC	"A driver for the QN8035 radio chip for longcheer Phones."

#define PINFO(format, ...)\
	printk(KERN_INFO KBUILD_MODNAME ": "\
		DRIVER_VERSION ": " format "\n", ## __VA_ARGS__)
#define PWARN(format, ...)\
	printk(KERN_WARNING KBUILD_MODNAME ": "\
		DRIVER_VERSION ": " format "\n", ## __VA_ARGS__)
# define PDEBUG(format, ...)\
	printk(KERN_DEBUG KBUILD_MODNAME ": "\
		DRIVER_VERSION ": " format "\n", ## __VA_ARGS__)

#define QNM_SetRxThreshold(radio,db)                  QND_WriteReg(CCA, db)

/* Frequency limits in MHz -- these are European values.  For Japanese
devices, that would be 76000 and 91000.  */
#define FREQ_MIN  7600
#define FREQ_MAX 10800
#define FREQ_MUL 16
#define  FM_OUT_HEADSET 255
#define  FM_OUT_SPEAKER 254

//#define FM_CEN                  90
#define FM_INT                  17
#define QN8035_RETRY_COUNT 5

static int radio_nr = -1;
//static uint8_t seeked_val = 1;
//static int seek = 0;
static int rssiTH_delta = 0;
static int snrTH_delta = 0;
static int cancel_search = 0;
struct i2c_client *this_client;
//#if defined (CONFIG_LCT_AE760_LX)
#if defined(CONFIG_LCT_AE760_LX) || defined(CONFIG_LCT_AE500_LX)
static int num = 0;
#endif

extern int get_hw_version(char *val);

struct QN8035_device {
	struct i2c_client		*i2c_client;
	struct video_device		*videodev;

	uint8_t	qnd_CurFreq;
	uint32_t	rangelow;
	uint32_t	rangehigh;
	int	step;
	int	th;

	uint8_t 	   enable_status;
	uint8_t 	   mute_status;
	uint8_t       powermode;
	struct mutex			mutex;
	int				users;
};

void qn8035_power_up(struct QN8035_device *radio);
void qn8035_power_down(struct QN8035_device *radio);

static int QN8035_i2c_read(uint8_t address)
{
	int retry;
	int val;

	/*struct i2c_msg msg[] = {
		{
			.addr = this_client->addr,
			.flags = 0,
			.len = 1,
			.buf = &address,
		},
		{
			.addr = this_client->addr,
			.flags = I2C_M_RD,
			.len = 1,
			.buf = &val,
		}
	};
	*/
	for (retry = 0; retry < QN8035_RETRY_COUNT; retry++) {
		///if (i2c_transfer(this_client->adapter, msg, 2) == 2)
		val = i2c_smbus_read_byte_data(this_client, address);
		if(val >= 0)
			break;
		msleep(10);
	}
	if (retry == QN8035_RETRY_COUNT) {
		printk("QN8035_i2c_read retry over %d\n",
			QN8035_RETRY_COUNT);
		return -EIO;
	}
	return val;

}

static int QN8035_i2c_write(uint8_t address, uint8_t data)
{
	int retry;
	/*uint8_t buf[2];

	struct i2c_msg msg[] = {
		{
			.addr = this_client->addr,
			.flags = 0,
			.len =  2,
			.buf = buf,
		}
	};

	buf[0] = address;
	buf[1] = data;
*/
      int val;
	for (retry = 0; retry < QN8035_RETRY_COUNT; retry++) {
		//if (i2c_transfer(this_client->adapter, msg, 1) == 1)
		val = i2c_smbus_write_byte_data(this_client, address, data);
		if(val == 0)
			break;
		msleep(10);
	}

	if (retry == QN8035_RETRY_COUNT) {
		printk("QN8035_i2c_write retry over %d\n",
			QN8035_RETRY_COUNT);
		return -EIO;
	}
	return 0;

}


int QND_ReadReg(uint8_t adr)
{
    int reg_val;

/*
	radio->reg_addr = ((unsigned short)adr)&0xff;
	if(QN8035_i2c_read(radio)>0) reg_val = (uint8_t)QN8035_i2c_read(radio);
*/
	reg_val = QN8035_i2c_read(adr);
	if (reg_val < 0)
		printk("QND_ReadReg error\n");

	return reg_val;
}



uint8_t QND_WriteReg(uint8_t adr, uint8_t value)
{
	/*unsigned short *reg_data = &(radio->reg_value);
	unsigned short *reg_addr = &(radio->i2c_client->addr);

	*reg_data = ((unsigned short)value)0xff;
	*reg_addr = ((unsigned short)adr)0xff;
	
	*/
	return QN8035_i2c_write( adr, value);
}

/* V4L2 code related */
static struct v4l2_queryctrl radio_qctrl[] = {
	{
		.id            = V4L2_CID_AUDIO_MUTE,
		.name          = "Mute",
		.minimum       = 0,
		.maximum       = 1,
		.default_value = 1,
		.type          = V4L2_CTRL_TYPE_BOOLEAN,
	},
	{
		.id            = V4L2_CID_AUDIO_VOLUME,
		.name          = "Volume",
		.minimum       = 0,
		.maximum       = 47,
		.default_value = 1,
		.type          = V4L2_CTRL_TYPE_INTEGER,
	},
	{
		.id            = V4L2_CID_PRIVATE_TAVARUA_STATE,
		.name          = "fmOnOff",
		.minimum       = 0,
		.maximum       = 2,
		.default_value = 1,
		.type          = V4L2_CTRL_TYPE_INTEGER,
	},
	{
		.id            = V4L2_CID_PRIVATE_TAVARUA_LP_MODE,
		.name          = "LowPwrMode",
		.minimum       = 0,
		.maximum       = 1,
		.default_value = 0,
		.type          = V4L2_CTRL_TYPE_BOOLEAN,
	},	
	{
		.id 		   = V4L2_CID_PRIVATE_TAVARUA_SPACING,
		.name		   = "ch_step",
		.minimum	   = 0,
		.maximum	   = 2,
		.default_value = 0,
		.type		   = V4L2_CTRL_TYPE_INTEGER,
	},

};


void QND_Delay(uint16_t ms)
{
	msleep(ms);
}

int QNF_SetRegBit(uint8_t reg, uint8_t bitMask, uint8_t data_val)
{
	uint8_t  reg_val;
	reg_val = QND_ReadReg(reg);
	reg_val &= (uint8_t)(~bitMask);
	reg_val |= data_val & bitMask;

	return QND_WriteReg(reg, reg_val);
}

/**********************************************************************
void QN_ChipInitialization()
**********************************************************************
Description: chip first step initialization, called only by QND_Init()

Parameters:
None
Return Value:
None
**********************************************************************/
void QN_ChipInitialization(void)
{
    QND_WriteReg(0x00, 0x81);
    QND_Delay(10);
    qnd_ChipID = QND_ReadReg(CID1) & 0x03;
    // change crystal frequency setting here
    //setting input clock source type:sine-wave clock or digital clock
    QNF_SetRegBit(0x01,0x80,QND_DIGITAL_CLOCK);
    QND_WriteReg(XTAL_DIV0, QND_XTAL_DIV0);
    QND_WriteReg(XTAL_DIV1, QND_XTAL_DIV1);
    QND_WriteReg(XTAL_DIV2, QND_XTAL_DIV2);
    QND_Delay(10);
    QND_WriteReg(0x54, 0x47);//mod PLL setting
    QND_WriteReg(0x19, 0x40);//AGC setting
    QND_WriteReg(0x2d, 0xD6);//notch filter threshold adjusting
    QND_WriteReg(0x43, 0x10);//notch filter threshold enable
    if(qnd_ChipID == CHIPSUBID_QN8035A1)
    {
		 QNF_SetRegBit(0x47,0x0C,0x08);
    }
    //these variables are used in QNF_SetCh() function.
    qnd_R16 = QND_ReadReg(0x16);
    qnd_R17 = QND_ReadReg(0x17);
    qnd_R46 = QND_ReadReg(0x46);
}

/**********************************************************************
void QNF_RXInit()
**********************************************************************
Description: set to SNR based MPX control. Call this function before
             tune to one specific channel

Parameters:
None
Return Value:
None
**********************************************************************/
void QNF_RXInit(void)
{
    QNF_SetRegBit(0x1B,0x08,0x00);  //Let NFILT adjust freely
    QNF_SetRegBit(0x2C,0x3F,0x12);
    QNF_SetRegBit(0x1D,0x40,0x00);
    QNF_SetRegBit(0x41,0x0F,0x0A);
    QND_WriteReg(0x45,0x50);
    QNF_SetRegBit(0x40,0x70,0x70);
    QNF_SetRegBit(0x19,0x80,0x80);
    QNF_SetRegBit(0x3E,0x80,0x80);
    QNF_SetRegBit(0x41,0xE0,0xC0);
    if(qnd_ChipID == CHIPSUBID_QN8035A0)
    {
        QNF_SetRegBit(0x42,0x10,0x10);
    }
    QNF_SetRegBit(0x34, 0x7F,SMSTART_VAL);
    QNF_SetRegBit(0x35,0x7F,SNCSTART_VAL);
    QNF_SetRegBit(0x36,0x7F,HCCSTART_VAL);
}

/**********************************************************************
void QNF_SetMute(uint8_t On)
**********************************************************************
Description: set register specified bit

Parameters:
On:        1: mute, 0: unmute
Return Value:
None
**********************************************************************/
void QNF_SetMute(uint8_t On)
{
	if(On)
	{
        if(qnd_ChipID == CHIPSUBID_QN8035A0)
        {
            QNF_SetRegBit(REG_DAC, 0x0B, 0x0B);
        }
        else
        {
            QNF_SetRegBit(0x4A, 0x20, 0x20);
        }
	}
	else
	{
        if(qnd_ChipID == CHIPSUBID_QN8035A0)
        {
            QNF_SetRegBit(REG_DAC, 0x0B, 0x00);
        }
        else
        {
            QNF_SetRegBit(0x4A, 0x20, 0x00);
        }
	}
}


/**********************************************************************
void QND_SetSysMode(struct QN8035_device *radio,uint16_t mode)
***********************************************************************
Description: Set device system mode(like: sleep ,wakeup etc)
Parameters:
mode:  set the system mode , it will be set by  some macro define usually:

SLEEP : set chip to sleep mode
WAKEUP: wake up chip
RX:     set chip work on RX mode
Return Value:
None
**********************************************************************/
void QND_SetSysMode(uint16_t mode)
{
    uint8_t val;
    switch(mode)
    {
        case QND_MODE_SLEEP:                       //set sleep mode
            QNF_SetRegBit(REG_DAC, 0x08, 0x00);    //make sure Power down control by FSM control
            QNF_SetRegBit(SYSTEM1, STNBY_RX_MASK, STNBY_MODE);
            break;
        case QND_MODE_WAKEUP:                      //set wakeup mode
            QNF_SetRegBit(REG_DAC, 0x08, 0x00);    //make sure Power down control by FSM control
            QNF_SetRegBit(SYSTEM1, STNBY_RX_MASK, RX_MODE);
            QNF_SetMute(1);
            QND_Delay(SLEEP_TO_WAKEUP_DELAY_TIME); //avoid noise from sleep to wakeup mode during.
            QNF_SetMute(0);
            break;
        default:
            val = (uint8_t)(mode >> 8);
            if (val)
            {
                val = val >> 3;
                if(val&0x10)
                    // set to new mode if it's not same as old
                    if((QND_ReadReg(SYSTEM1) & STNBY_RX_MASK) != val)
                    {
                        QNF_SetMute(1);
                        QNF_SetRegBit(SYSTEM1, STNBY_RX_MASK, val);
                    }
            }
            break;
    }
}

/***********************************************************************
void QND_RXSetTH(uint8_t th)
***********************************************************************
Description: Setting the threshold value of automatic scan channel
th:
  Setting threshold for quality of channel to be searched,
  the range of th value:CCA_SENSITIVITY_LEVEL_0 ~ CCA_SENSITIVITY_LEVEL_9
Return Value:
  None
***********************************************************************/
void QND_RXSetTH(uint8_t th)
{
    uint8_t rssiTH;
    uint8_t snrTH;
    uint16_t rssi_snr_TH;
    uint16_t rssi_snr_TH_tbl [10] = { CCA_SENSITIVITY_LEVEL_0,CCA_SENSITIVITY_LEVEL_1,
                                    CCA_SENSITIVITY_LEVEL_2,CCA_SENSITIVITY_LEVEL_3,
                                    CCA_SENSITIVITY_LEVEL_4,CCA_SENSITIVITY_LEVEL_5,
                                    CCA_SENSITIVITY_LEVEL_6,CCA_SENSITIVITY_LEVEL_7,
                                    CCA_SENSITIVITY_LEVEL_8,CCA_SENSITIVITY_LEVEL_9
                                  };

    rssi_snr_TH = rssi_snr_TH_tbl[th];
    rssiTH = (uint8_t) (rssi_snr_TH >> 8);
    snrTH = (uint8_t) (rssi_snr_TH & 0xff);
    QNF_SetRegBit(REG_REF,ICPREF,0x0A);
    QNF_SetRegBit(GAIN_SEL,0x08,0x08);//NFILT program is enabled
	//selection filter:filter3
    QNF_SetRegBit(CCA1,0x30,0x30);
    //Enable the channel condition filter3 adaptation,Let ccfilter3 adjust freely
    QNF_SetRegBit(SYSTEM_CTL2,0x40,0x00);
    QND_WRITE(CCA_CNT1,0x00);
    QNF_SetRegBit(CCA_CNT2,0x3F,0x02);
	//selection the time of CCA FSM wait SNR calculator to settle:20ms
	//0x00:	    20ms(default)
	//0x40:	    40ms
	//0x80:	    60ms
	//0xC0:	    100m
    QNF_SetRegBit(CCA_SNR_TH_1 , 0xC0, 0x40);
    //selection the time of CCA FSM wait RF front end and AGC to settle:20ms
    //0x00:     10ms
	//0x40:     20ms(default)
    //0x80:     40ms
	//0xC0:     60ms
    QNF_SetRegBit(CCA_SNR_TH_2, 0xC0, 0x80);
    QNF_SetRegBit(CCA, 0x3F, rssiTH-rssiTH_delta);  //setting RSSI threshold for CCA
    QNF_SetRegBit(CCA_SNR_TH_1 , 0x3F, snrTH-snrTH_delta); //setting SNR threshold for CCA
}

/************************************************************************
void QND_RXConfigAudio(uint8_t optiontype, uint8_t option )
*************************************************************************
Description: config audio
Parameters:
  optiontype: option
    QND_CONFIG_MONO; ¡®option¡¯control mono, 0: stereo receive mode ,1: mono receiver mode
    QND_CONFIG_MUTE; ¡®option¡¯control mute, 0:mute disable,1:mute enable
    QND_CONFIG_VOLUME: 'option' control the volume gain,range : 0~47(-47db~0db)

Return Value:
    none
**********************************************************************/
void QND_RXConfigAudio(uint8_t optiontype, uint8_t option )
{
    uint8_t regVal;
    switch(optiontype)
    {
        case QND_CONFIG_MONO:
            if (option)
                QNF_SetRegBit(SYSTEM1,RX_MONO,RX_MONO);
            else
                QNF_SetRegBit(SYSTEM1,RX_MONO,0x00);
            break;
        case QND_CONFIG_MUTE:
            if (option)
                QNF_SetMute(1); //mute audio
            else
                QNF_SetMute(0); // disable mute audio
            break;
        case QND_CONFIG_VOLUME:
            if (option > 47)
            {
                option = 47;
            }
            if (option == 0)    //audio is muted when the volume is adjusted to minimum
            {
                QNF_SetRegBit(VOL_CTL, 0x80, 0x80); //mute audio
            }
            else
            {
                QNF_SetRegBit(VOL_CTL, 0x80, 0x00); // disable mute audio
            }
            regVal = (uint8_t)(option/6);
            QNF_SetRegBit(VOL_CTL, 0x07, regVal);   //set analog gain
            regVal = (uint8_t)(option%6);
            QNF_SetRegBit(VOL_CTL, 0x38, (uint8_t)((5-regVal)<<3));   //set digital gain
            break;
        default:
            break;
    }
}

/**********************************************************************
void QNF_SetCh(uint16_t freq)
**********************************************************************
Description: set channel frequency

Parameters:
    freq:  channel frequency to be set
Return Value:
    None
**********************************************************************/
void QNF_SetCh(uint16_t freq)
{
    // calculate ch parameter used for register setting
    uint8_t tStep;
    uint8_t tCh;
    uint16_t f;
    uint16_t pll_dlt;

    if(freq == 8540 || freq == 8550 || freq == 8560)
    {
        pll_dlt = (uint16_t)qnd_R16 >> 3;
        pll_dlt |=(uint16_t)qnd_R17 << 5;
        pll_dlt -= 1039;
        QND_WriteReg(0x16, (uint8_t)pll_dlt << 3);
        QND_WriteReg(0x17, (uint8_t)(pll_dlt >> 5));
        if(freq == 8540) QND_WriteReg(0x46, 0x9D);
        else if(freq == 8550) QND_WriteReg(0x46, 0x69);
        else QND_WriteReg(0x46, 0x34);
        freq = 8570;
    }
    else
    {
        QND_WriteReg(0x16, qnd_R16);
        QND_WriteReg(0x17, qnd_R17);
        QND_WriteReg(0x46, qnd_R46);
    }
	//Manually set RX Channel index
    QNF_SetRegBit(SYSTEM1, CCA_CH_DIS, CCA_CH_DIS);
    f = FREQ2CHREG(freq);
    // set to reg: CH
    tCh = (uint8_t) f;
    QND_WriteReg(CH, tCh);
    // set to reg: CH_STEP
    tStep = QND_ReadReg(CH_STEP);
    tStep &= ~CH_CH;
    tStep |= ((uint8_t) (f >> 8) & CH_CH);
    QND_WriteReg(CH_STEP, tStep);
}

/**********************************************************************
uint16_t QNF_GetCh()
**********************************************************************
Description: get current channel frequency

Parameters:
	None
Return Value:
	channel frequency
**********************************************************************/
uint16_t QNF_GetCh(void)
{
	uint8_t tCh;
	uint8_t  tStep;
	uint16_t ch = 0;
	// set to reg: CH_STEP
	tStep = QND_ReadReg(CH_STEP);
	tStep &= CH_CH;
	ch	=  tStep ;
	tCh= QND_ReadReg(CH);
	ch = (ch<<8)+tCh;
	return CHREG2FREQ(ch);
}

/**********************************************************************
void QNF_ConfigScan(uint16_t start,uint16_t stop, uint8_t step)
**********************************************************************
Description: config start, stop, step register for FM/AM CCA or CCS

Parameters:
    start
        Set the frequency (10kHz) where scan to be started,
        e.g. 7600 for 76.00MHz.
    stop
        Set the frequency (10kHz) where scan to be stopped,
        e.g. 10800 for 108.00MHz
    step
        1: set leap step to (FM)100kHz / 10kHz(AM)
        2: set leap step to (FM)200kHz / 1kHz(AM)
        0:  set leap step to (FM)50kHz / 9kHz(AM)
Return Value:
         None
**********************************************************************/
void QNF_ConfigScan(uint16_t start,uint16_t stop, uint8_t step)
{
    // calculate ch para
    uint8_t tStep = 0;
    uint8_t tS;
    uint16_t fStart;
    uint16_t fStop;

    fStart = FREQ2CHREG(start);
    fStop = FREQ2CHREG(stop);
    // set to reg: CH_START
    tS = (uint8_t) fStart;
    QND_WriteReg(CH_START, tS);
    tStep |= ((uint8_t) (fStart >> 6) & CH_CH_START);
    // set to reg: CH_STOP
    tS = (uint8_t) fStop;
    QND_WriteReg(CH_STOP, tS);
    tStep |= ((uint8_t) (fStop >> 4) & CH_CH_STOP);
    // set to reg: CH_STEP
    tStep |= step << 6;
    QND_WriteReg(CH_STEP, tStep);
}

/**********************************************************************
void QND_TuneToCH(uint16_t ch)
**********************************************************************
Description: Tune to the specific channel. call QND_SetSysMode() before
call this function
Parameters:
ch
Set the frequency (10kHz) to be tuned,
e.g. 101.30MHz will be set to 10130.
Return Value:
None
**********************************************************************/
void QND_TuneToCH(uint16_t ch)
{
    uint8_t reg;
    uint8_t imrFlag = 0;

    QNF_SetRegBit(REG_REF,ICPREF,0x0A);
    QNF_RXInit();
    QNF_SetMute(1);
    if((ch==8430)||(ch==7290)||(ch==6910))
    {
    	imrFlag = 1;
    }
    else if(qnd_ChipID == CHIPSUBID_QN8035A1)
    {
        if((ch==10710)||(ch==7860))
        {
            imrFlag = 1;
        }
    }
    if(imrFlag)
    {
        QNF_SetRegBit(CCA, IMR, IMR);
    }
    else
    {
        QNF_SetRegBit(CCA, IMR, 0x00);
    }
    QNF_ConfigScan(ch, ch, qnd_CH_STEP);
    QNF_SetCh(ch);
    QNF_SetRegBit(SYSTEM1, CHSC, CHSC);
    //Auto tuning
    QND_WriteReg(0x4F, 0x80);
    reg = QND_ReadReg(0x4F);
    reg >>= 1;
    QND_WriteReg(0x4F, reg);
    if(qnd_ChipID == CHIPSUBID_QN8035A0)
    {
        QND_Delay(CH_SETUP_DELAY_TIME+300);
    }
    else
    {
        QND_Delay(CH_SETUP_DELAY_TIME);
    }
    QNF_SetRegBit(REG_REF,ICPREF,0x00);
    QNF_SetMute(0);
}


/**********************************************************************
uint8_t QND_GetRSSI(uint16_t ch)
**********************************************************************
Description:    Get the RSSI value
Parameters:
Return Value:
RSSI value  of the channel settled
**********************************************************************/
uint8_t QND_GetRSSI(struct QN8035_device *radio, uint16_t ch)
{
    QNF_SetRegBit(REG_REF,ICPREF,0x0A);
    QNF_ConfigScan(ch, ch, qnd_CH_STEP);
    QNF_SetCh(ch);
    if(qnd_ChipID == CHIPSUBID_QN8035A0)
    {
        //Enter CCA mode, speed up the PLL locking.
	 QNF_SetRegBit(SYSTEM1, CHSC, CHSC);
        QND_Delay(QND_READ_RSSI_DELAY+90);
    }
    else
    {
        //if this delay time effects the CCA time,it may be shorted to 20ms
        QND_Delay(50);
    }
    QNF_SetRegBit(REG_REF,ICPREF,0x00);
    if(QND_ReadReg(RDSCOSTAS) & 0x80)
    {
        return (QND_ReadReg(RSSISIG)+9);
    }
    else
    {
        return QND_ReadReg(RSSISIG);
    }
}

/***********************************************************************
uint8_t QND_RXValidCH(uint16_t freq, uint8_t step);
***********************************************************************
Description: to validate a ch (frequency)(if it's a valid channel)
Freq: specific channel frequency, unit: 10Khz
  e.g. 108.00MHz will be set to 10800.
Step:
  FM:
  QND_FMSTEP_100KHZ: set leap step to 100kHz
  QND_FMSTEP_200KHZ: set leap step to 200kHz
  QND_FMSTEP_50KHZ:  set leap step to 50kHz
Return Value:
  0: not a valid channel
  1: a valid channel at this frequency
***********************************************************************/
uint8_t QND_RXValidCH(uint16_t freq, uint8_t step)
{
    uint8_t regValue;
    uint8_t timeOut = CCA_TIME_OUT;
    uint8_t isValidChannelFlag = 0;

    QNF_ConfigScan(freq, freq, step);
	//enter CCA mode,channel index is decided by internal CCA
    QNF_SetRegBit(SYSTEM1,RXCCA_MASK,RX_CCA);
    do
    {
        regValue = QND_ReadReg(SYSTEM1);
        QND_Delay(5);   //delay 5ms
        timeOut--;
        printk("QND_RXValidCH--CHSC:0x%02x timeOut:%d \n",regValue&CHSC,timeOut);
	if(cancel_search)
		return 0;
    } while((regValue & CHSC) && timeOut);//when seeking a channel or time out,be quited the loop
    //read out the rxcca_fail flag of RXCCA status
    isValidChannelFlag = QND_ReadReg(STATUS1) & RXCCA_FAIL ? 0:1;
    if(isValidChannelFlag)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/***********************************************************************
uint16_t QND_RXSeekCH(uint16_t start, uint16_t stop, uint16_t step, uint8_t db, uint8_t up);
***********************************************************************
Description: Automatically scans the frequency range, and detects the
first channel(FM, it will be determine by the system mode which set
by QND_SetSysMode).
A threshold value needs to be passed in for channel detection.
Parameters:
start
Set the frequency (10kHz) where scan will be started,
e.g. 76.00MHz will be set to 7600.
stop
Set the frequency (10kHz) where scan will be stopped,
e.g. 108.00MHz will be set to 10800.
step
FM:
QND_FMSTEP_100KHZ: set leap step to 100kHz
QND_FMSTEP_200KHZ: set leap step to 200kHz
QND_FMSTEP_50KHZ:  set leap step to 50kHz
db:
Set threshold for quality of channel to be searched,
the range of db value:CCA_SENSITIVITY_LEVEL_0 ~ CCA_SENSITIVITY_LEVEL_19
up:
Set the seach direction :
Up;0,seach from stop to start
Up:1 seach from start to stop
Return Value:
The channel frequency (unit: 10kHz)
***********************************************************************/
uint16_t QND_RXSeekCH(uint16_t start, uint16_t stop, uint8_t step, uint8_t db, uint8_t up)
{
    uint16_t freq = start;
    uint16_t validCH;
    uint8_t stepValue;
    uint16_t pStart = start;
    uint16_t pStop = stop;
    uint8_t regValue;
    uint8_t timeOut;
    uint8_t isValidChannelFlag = 0;

    up=(start <= stop) ? 1 : 0;
    stepValue = qnd_StepTbl[step];
    if(qnd_ChipID == CHIPSUBID_QN8035A0)
    {
        QNF_SetMute(1);
        QND_RXSetTH(db);
        do
        {
            validCH = QND_RXValidCH(freq, step);
		if(cancel_search){
			cancel_search = 0;
			return 0;
		}
            if (validCH == 0)
            {
                 if ((!up && (freq <= stop)) || (up && (freq >= stop)))
                 {
                   break;
                 }
                 else
                 {
                    freq = freq + (up ? stepValue : -stepValue);
                 }
            }
        } while (validCH == 0);
        QND_TuneToCH(freq);
    }
    else
    {
        if(qnd_AutoScanAll == 0)
        {
            QNF_SetMute(1);
            QND_RXSetTH(db);
        }
        do
        {
            QNF_ConfigScan(pStart, pStop, step);
        	//enter CCA mode,channel index is decided by internal CCA
            QNF_SetRegBit(SYSTEM1,RXCCA_MASK,RX_CCA);
            timeOut = CCA_TIME_OUT;
            do
            {
                regValue = QND_ReadReg(SYSTEM1);
                QND_Delay(5);   //delay 5ms
                timeOut--;
		if(cancel_search){
			cancel_search = 0;
			return 0;
		}
                //printk("QND_RXSeekCH---CHSC:0x%02x timeOut:%d \n",regValue&CHSC,timeOut);
            } while((regValue & CHSC) && timeOut);//when it seeks a channel or the time is overtime,the loop is quited
            isValidChannelFlag = QND_ReadReg(STATUS1) & RXCCA_FAIL ? 0:1;//reading the rxcca_fail flag of RXCCA status
            freq = QNF_GetCh();
	     if(isValidChannelFlag == 0) {
                pStart = freq + (up ? stepValue : -stepValue);
            }
	     printk("isValidChannelFlag=%d, feq=%d,up=%d, pstart=%d, pstop=%d, stepValue=%d, reg03=%d\n", isValidChannelFlag, freq, up, pStart, pStop,stepValue,QND_ReadReg(0x03));
	     if(up ? (pStart >= pStop):(pStart <= pStop)){
		 	printk("break; up=%d\n", up);
		 	break;
	     	}
        }while (isValidChannelFlag == 0);

        if(!isValidChannelFlag)
        {
            freq = 0;
        }
        if (qnd_AutoScanAll == 0)
        {
            QND_TuneToCH(freq ? freq: stop);
        }
    }
    return freq;
}


/**********************************************************************
int QND_Init()
**********************************************************************
Description: Initialize device to make it ready to have all functionality ready for use.

Parameters:
    None
Return Value:
    1: Device is ready to use.
    0: Device is not ready to serve function.
**********************************************************************/
uint8_t QND_Init(void)
{
    QN_ChipInitialization();
    QND_RXConfigAudio(QND_CONFIG_VOLUME, 50);
    return 1;
}


static int QN8035_enable(struct QN8035_device *radio)
{
   if (radio->enable_status == 0) {
	    QND_SetSysMode(QND_MODE_RX);
   }
    radio->enable_status = 1;
    return 0;
}


static void QN8035_disable(struct QN8035_device *radio)
{
    if (radio->enable_status) {
	   //gpio_set_value(FM_CEN, 0);
	  QND_SetSysMode(QND_MODE_SLEEP);
    }
   radio->enable_status = 0;
}

/* get current powermode of the FM core. 1 for LPM and 0 Normal mode */
static void QN8035_power_mode(struct QN8035_device *radio, int mode)
{
	// FIXME
	if(mode) {//low power
	 	QND_SetSysMode(QND_MODE_SLEEP);
		radio->powermode = 1;
	}else {
		QND_SetSysMode(QND_MODE_WAKEUP);
		radio->powermode = 0;
	}
}

static int QN8035_get_powermode(struct QN8035_device *radio)
{
	//FIXME
	return radio->powermode;
}

/* tune an frequency, freq is defined by v4l's TUNER_LOW, i.e. 1/16th kHz */
static void QN8035_tune(struct QN8035_device *radio, int freq)
{
	QND_TuneToCH(freq);
	//	PWARN("Could not set frequency!");
}

static void QN8035_set_audout_mode(int audmode)
{

	if (audmode == V4L2_TUNER_MODE_MONO)
		QND_RXConfigAudio(QND_CONFIG_MONO, 1);
	else
		QND_RXConfigAudio(QND_CONFIG_MONO, 0);
}

static int QN8035_get_audout_mode(void)
{
	if (QNM_GetMonoMode())
		return V4L2_TUNER_MODE_MONO;
	else
		return V4L2_TUNER_MODE_STEREO;
}

static void QN8035_mute(struct QN8035_device *radio, int on)
{
	QND_RXConfigAudio(QND_CONFIG_MUTE, on);
	radio->mute_status = on;
}

/* V4L2 vidioc */
static int qn8035_vidioc_querycap(struct file *file, void  *priv,
					struct v4l2_capability *v)
{
	struct QN8035_device *radio = video_drvdata(file);
	struct video_device *dev = radio->videodev;

	strlcpy(v->driver, dev->dev.driver->name, sizeof(v->driver));
	strlcpy(v->card, dev->name, sizeof(v->card));
	sprintf(v->bus_info, "I2C");
	v->version = RADIO_VERSION;
	v->capabilities = V4L2_CAP_TUNER | V4L2_CAP_RADIO;
	return 0;
}

static int qn8035_vidioc_g_tuner(struct file *file, void *priv,
				struct v4l2_tuner *v)
{
	struct QN8035_device *radio = video_drvdata(file);

	if (v->index > 0)
		return -EINVAL;

	memset(v, 0, sizeof(v));
	strcpy(v->name, "FM");
	v->type = V4L2_TUNER_RADIO;

	v->rangelow   = radio->rangelow * 10 * FREQ_MUL;
	v->rangehigh  = radio->rangehigh * 10 * FREQ_MUL;
	v->capability = V4L2_TUNER_CAP_LOW | V4L2_TUNER_CAP_STEREO;
	v->signal = QNM_GetRssi();
	v->audmode = QN8035_get_audout_mode();

	return 0;
}

static int qn8035_vidioc_s_tuner(struct file *file, void *priv,
				struct v4l2_tuner *v)
{
	struct QN8035_device *radio = video_drvdata(file);;

	if (v->index > 0)
		return -EINVAL;

	if (v->index == 1) {
		QN8035_set_audout_mode(v->audmode);
	}
	if(v->index == 0) {
		radio->rangelow = (v->rangelow / FREQ_MUL) / 10;
		radio->rangehigh= (v->rangehigh / FREQ_MUL) / 10;
	}
	return 0;
}

static int qn8035_vidioc_s_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct QN8035_device *radio = video_drvdata(file);

	if (f->type != V4L2_TUNER_RADIO)
		return -EINVAL;
	
	QN8035_tune(radio, (f->frequency / FREQ_MUL) / 10);
	msleep(10);
	printk(KERN_ERR"=====s_freq =%d=======\n", (f->frequency / FREQ_MUL) / 10);
	return 0;
}

static int qn8035_vidioc_g_frequency(struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	//struct QN8035_device *radio = video_drvdata(file);

	memset(f, 0, sizeof(f));
	f->type = V4L2_TUNER_RADIO;
	/*
	if(seeked_val != 0) {
		f->frequency = QNF_GetCh() * FREQ_MUL/100;
	} else {
		f->frequency = 0;
		seeked_val = 1;
	}
	*/
	f->frequency = QNF_GetCh() * 10 * FREQ_MUL;
	return 0;
}

static int qn8035_vidioc_queryctrl(struct file *file, void *priv,
			    struct v4l2_queryctrl *qc)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(radio_qctrl); i++) {
		if (qc->id && qc->id == radio_qctrl[i].id) {
			memcpy(qc, &(radio_qctrl[i]), sizeof(*qc));
			return 0;
		}
	}
	return -EINVAL;
}

static int qn8035_vidioc_g_ctrl(struct file *file, void *priv,
			    struct v4l2_control *ctrl)
{
	struct QN8035_device *radio = video_drvdata(file);

	switch (ctrl->id) {
	case V4L2_CID_AUDIO_MUTE:
		ctrl->value = radio->mute_status;
		return 0;
	case V4L2_CID_PRIVATE_TAVARUA_LP_MODE:
		ctrl->value = QN8035_get_powermode(radio);
		return 0;
	default:
		return 0;
	}
	return -EINVAL;
}

static int qn8035_vidioc_s_ctrl(struct file *file, void *priv,
			    struct v4l2_control *ctrl)
{
	struct QN8035_device *radio = video_drvdata(file);
	if(radio == NULL){
		printk(KERN_ERR "radio NULL\n");
	}
	switch (ctrl->id) {
	case V4L2_CID_AUDIO_MUTE:
		QN8035_mute(radio, ctrl->value);
		return 0;
	case V4L2_CID_AUDIO_VOLUME:
		QND_RXConfigAudio(QND_CONFIG_VOLUME, ctrl->value);
		return 0;
	case V4L2_CID_PRIVATE_TAVARUA_STATE:
		if(ctrl->value == 0) {	
			//#if defined (CONFIG_LCT_AE760_LX)
			#if defined(CONFIG_LCT_AE760_LX) || defined(CONFIG_LCT_AE500_LX)
			if(num==1)
				{
			             qn8035_power_down(radio);
				      num = 0;
				}			
			#else
			QN8035_disable(radio);
			#endif						
		}else {
			//#if defined (CONFIG_LCT_AE760_LX)
			#if defined(CONFIG_LCT_AE760_LX) || defined(CONFIG_LCT_AE500_LX)
			       if (num==0)
				{
			              qn8035_power_up(radio);
				      num = 1;
				}
			#else
			QN8035_enable(radio);
			#endif
		}
		return 0;
       case V4L2_CID_PRIVATE_TAVARUA_SRCHON:
		cancel_search = 1;
		return 0;
	case V4L2_CID_PRIVATE_TAVARUA_LP_MODE:
		QN8035_power_mode(radio, ctrl->value);
		return 0;
	case V4L2_CID_PRIVATE_TAVARUA_SPACING:
		radio->step = ctrl->value;
		return 0;
	case FM_OUT_HEADSET:
		QND_RXConfigAudio(QND_CONFIG_VOLUME, ctrl->value);
		return 0;
	case FM_OUT_SPEAKER:
		QND_RXConfigAudio(QND_CONFIG_VOLUME, ctrl->value);
		return 0;
	default:
		return 0;
	}

	return -EINVAL;
}

/*
 * qn8035_vidioc_s_hw_freq_seek - set hardware frequency seek
 */
static int qn8035_vidioc_s_hw_freq_seek(struct file *file, void *priv,
                struct v4l2_hw_freq_seek *seek)
{
        struct QN8035_device *radio =  video_get_drvdata(video_devdata(file));
        int retval = 0;
	//uint16_t start = (seek->reserved[0] / FREQ_MUL) / 10;
	uint16_t stop = seek->seek_upward ? radio->rangehigh: radio->rangelow;
	retval = QND_RXSeekCH(QNF_GetCh(), stop, radio->step, radio->th, seek->seek_upward);
	return retval*10;//10k->k
}

void init_device_members(struct QN8035_device *radio)
{
	radio->th = 3;
	rssiTH_delta = 19;	
	radio->enable_status = 0;
	radio->powermode = 0;
	radio->mute_status= 0;
	radio->step = 1;
	cancel_search = 0;
	radio->users = 0;
}

static int QN8035_open(struct file *file)
{
	struct QN8035_device *radio = video_get_drvdata(video_devdata(file));
	if (!radio)
		return -ENODEV;

	return 0;
}

static int QN8035_close(struct file *file)
{
	struct QN8035_device *radio = video_get_drvdata(video_devdata(file));

	if (!radio)
		return -ENODEV;

	return 0;
}

static ssize_t fm_seek_show(struct device *dev,
                                         struct device_attribute *attr, char *buf)
{
	//struct QN8035_device *radio = dev_get_drvdata(dev);
	size_t count = 0;

	count += sprintf(buf, "local freq %u\n", QNF_GetCh());
	return count;
}

static ssize_t fm_seek_store(struct device *dev,
                                          struct device_attribute *attr,
                                          const char *buf, size_t count)
{
	struct QN8035_device *radio = dev_get_drvdata(dev);
	unsigned long val;
	int retval;

	retval = strict_strtoul(buf, 10, &val);
	if (retval)
		return retval;

	if(val)
		retval = QND_RXSeekCH(QNF_GetCh()+10, qnd_CH_STOP, radio->step, radio->th, 1);
	else
		retval = QND_RXSeekCH(QNF_GetCh()-10, qnd_CH_START, radio->step, radio->th, 0);

	printk("get freq : %d\n", retval);
	return count;
}

static DEVICE_ATTR(fm_seek, 0644, fm_seek_show, fm_seek_store);

static ssize_t fm_tune_show(struct device *dev,
                                         struct device_attribute *attr, char *buf)
{
	//struct QN8035_device *radio = dev_get_drvdata(dev);
	size_t count = 0;
	count += sprintf(buf, "%d\n", QNF_GetCh());
	return count;
}

static ssize_t fm_tune_store(struct device *dev,
                                          struct device_attribute *attr,
                                          const char *buf, size_t count)
{
	struct QN8035_device *radio = dev_get_drvdata(dev);
	unsigned long val;
	int error;

	error = strict_strtoul(buf, 10, &val);
	if (error)
		return error;

	QN8035_tune(radio, val);
	printk("reg03=%0x\n", QND_ReadReg(0x03));
	
	return count;
}

static DEVICE_ATTR(fm_tune, 0644, fm_tune_show, fm_tune_store);


static ssize_t fm_mute_show(struct device *dev,
                                         struct device_attribute *attr, char *buf)
{
	struct QN8035_device *radio = dev_get_drvdata(dev);
	size_t count = 0;
	
	count += sprintf(buf, "%d\n", radio->mute_status);
	return count;
}

static ssize_t fm_mute_store(struct device *dev,
                                          struct device_attribute *attr,
                                          const char *buf, size_t count)
{
	struct QN8035_device *radio = dev_get_drvdata(dev);
	unsigned long val;
	int error;

	error = strict_strtoul(buf, 10, &val);
	if (error)
		return error;

	QN8035_mute(radio, val);
	
	return count;
}

static DEVICE_ATTR(fm_mute, 0644, fm_mute_show, fm_mute_store);

static ssize_t fm_volume_show(struct device *dev,
                                         struct device_attribute *attr, char *buf)
{
	//struct QN8035_device *radio = dev_get_drvdata(dev);
	size_t count = 0;
	
	count += sprintf(buf, "%d\n", 1);
	return count;
}

static ssize_t fm_volume_store(struct device *dev,
                                          struct device_attribute *attr,
                                          const char *buf, size_t count)
{
	//struct QN8035_device *radio = dev_get_drvdata(dev);
	unsigned long val;
	int error;

	error = strict_strtoul(buf, 10, &val);
	if (error)
		return error;

	QND_RXConfigAudio(QND_CONFIG_VOLUME, val);
	
	return count;
}

static DEVICE_ATTR(fm_volume, 0644, fm_volume_show, fm_volume_store);

static ssize_t fm_enable_show(struct device *dev,
                                         struct device_attribute *attr, char *buf)
{
	struct QN8035_device *radio = dev_get_drvdata(dev);
	size_t count = 0;

	count += sprintf(buf, "%d\n", radio->enable_status);
	return count;
}

static ssize_t fm_enable_store(struct device *dev,
                                          struct device_attribute *attr,
                                          const char *buf, size_t count)
{
	struct QN8035_device *radio = dev_get_drvdata(dev);
	unsigned long val;
	int error;

	error = strict_strtoul(buf, 10, &val);
	if (error)
		return error;

	if (val) {
		QN8035_enable(radio);
	} else {
		QN8035_disable(radio);
	}
	
	return count;
}

static DEVICE_ATTR(fm_enable, 0644, fm_enable_show, fm_enable_store);


static ssize_t fm_th_show(struct device *dev,
                                         struct device_attribute *attr, char *buf)
{
	struct QN8035_device *radio = dev_get_drvdata(dev);
	size_t count = 0;

	count += sprintf(buf, "%d\n", radio->th);
	return count;
}

static ssize_t fm_th_store(struct device *dev,
                                          struct device_attribute *attr,
                                          const char *buf, size_t count)
{
	struct QN8035_device *radio = dev_get_drvdata(dev);
	unsigned long val;
	int error;

	error = strict_strtoul(buf, 10, &val);
	if (error)
		return error;

	radio->th = val;

	return count;
}

static DEVICE_ATTR(fm_th, 0644, fm_th_show, fm_th_store);

static ssize_t snrTH_delta_show(struct device *dev,
                                         struct device_attribute *attr, char *buf)
{
//	struct QN8035_device *radio = dev_get_drvdata(dev);
	size_t count = 0;

	count += sprintf(buf, "%d\n", snrTH_delta);
	return count;
}

static ssize_t snrTH_delta_store(struct device *dev,
                                          struct device_attribute *attr,
                                          const char *buf, size_t count)
{
	//struct QN8035_device *radio = dev_get_drvdata(dev);
	unsigned long val;
	int error;

	error = strict_strtoul(buf, 10, &val);
	if (error)
		return error;

	snrTH_delta = val;

	return count;
}

static DEVICE_ATTR(snrTH_delta, 0644, snrTH_delta_show, snrTH_delta_store);

static ssize_t rssiTH_delta_show(struct device *dev,
                                         struct device_attribute *attr, char *buf)
{
	//struct QN8035_device *radio = dev_get_drvdata(dev);
	size_t count = 0;

	count += sprintf(buf, "%d\n", rssiTH_delta);
	return count;
}

static ssize_t rssiTH_delta_store(struct device *dev,
                                          struct device_attribute *attr,
                                          const char *buf, size_t count)
{
	//struct QN8035_device *radio = dev_get_drvdata(dev);
	unsigned long val;
	int error;

	error = strict_strtoul(buf, 10, &val);
	if (error)
		return error;

	rssiTH_delta = val;

	return count;
}

static DEVICE_ATTR(rssiTH_delta, 0644, rssiTH_delta_show, rssiTH_delta_store);

static struct attribute *qn8035_attributes[] = {
	&dev_attr_fm_th.attr,
	&dev_attr_rssiTH_delta.attr,
	&dev_attr_snrTH_delta.attr,
	&dev_attr_fm_enable.attr,
	&dev_attr_fm_mute.attr,
	&dev_attr_fm_tune.attr,
	&dev_attr_fm_seek.attr,
	&dev_attr_fm_volume.attr,
	NULL
};

static const struct attribute_group qn8035_attr_group = {
	.attrs = qn8035_attributes,
};


/* File system interface */
static const struct v4l2_file_operations QN8035_fops = {
	.owner		= THIS_MODULE,
	.open           = QN8035_open,
	.release        = QN8035_close,
	.ioctl		= video_ioctl2,
};

static const struct v4l2_ioctl_ops QN8035_ioctl_ops = {
	.vidioc_querycap    = qn8035_vidioc_querycap,
	.vidioc_queryctrl   = qn8035_vidioc_queryctrl,
	.vidioc_g_tuner     = qn8035_vidioc_g_tuner,
	.vidioc_s_tuner     = qn8035_vidioc_s_tuner,
	.vidioc_g_frequency = qn8035_vidioc_g_frequency,
	.vidioc_s_frequency = qn8035_vidioc_s_frequency,
	.vidioc_g_ctrl      = qn8035_vidioc_g_ctrl,
	.vidioc_s_ctrl      = qn8035_vidioc_s_ctrl,
	.vidioc_s_hw_freq_seek = qn8035_vidioc_s_hw_freq_seek,
};

/* V4L2 interface */
static struct video_device QN8035_radio_template = {
	.name		= "QN8035 FM-Radio",
	.fops           = &QN8035_fops,
	.ioctl_ops 	= &QN8035_ioctl_ops,
	.release	= video_device_release,
};

#if 0
void  QN8035_enable_pins(void)
{
	struct msm_gpio qn8035_gpio_table[] = {
		{ GPIO_CFG(FM_CEN,  0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "fm_cen" },
	};

	if (msm_gpios_request_enable(qn8035_gpio_table, ARRAY_SIZE(qn8035_gpio_table)) < 0)
		pr_err("failed to request gpios for qn8035\n");
	
	gpio_set_value(FM_CEN, 1);
}



void  QN8035_disable_pins(void)
{
	struct msm_gpio qn8035_gpio_table[] = {
		{ GPIO_CFG(FM_CEN,  0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), "fm_cen" },
	};

	msm_gpios_disable_free(qn8035_gpio_table, ARRAY_SIZE(qn8035_gpio_table));
	gpio_set_value(FM_CEN, 0);
}
#endif

int qn8035_power_disable(void)
{

	int rc;	
	struct vreg *vreg_vcc=NULL;
	#if defined (CONFIG_LCT_AE550) || defined (CONFIG_LCT_AE770)
	vreg_vcc = vreg_get(NULL, "gp2");
	#else
	vreg_vcc = vreg_get(NULL, "usim2");
	#endif	
	if (IS_ERR(vreg_vcc)) {
		printk(KERN_ERR "%s: vreg get failed (%ld)\n",
		__func__, PTR_ERR(vreg_vcc));
		return PTR_ERR(vreg_vcc);
	rc = vreg_disable(vreg_vcc);
	if (rc) {
		printk(KERN_ERR "%s: vreg set level failed (%d)\n",
			__func__, rc);
		return -EIO;
	}
		msleep(20);
		return rc;
	}
	return 1;	
	
}

int qn8035_power_init(void)
{
	struct vreg *vreg_vcc=NULL;
	int rc;
	#if defined (CONFIG_LCT_AE550) || defined (CONFIG_LCT_AE770)
	vreg_vcc = vreg_get(NULL, "gp2");
	#else
	vreg_vcc = vreg_get(NULL, "usim2");
	#endif
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

	/*-------enable power configed----*/
	rc = vreg_enable(vreg_vcc);
	if (rc) {
		printk(KERN_ERR "%s: vreg enable failed (%d)\n",
		       __func__, rc);
		return -EIO;
	}
	printk("--%s--\n",__func__);
	msleep(20);
	return rc;
}
void qn8035_power_up(struct QN8035_device *radio)
{

	printk(KERN_ERR"xuchangqing qn8035 power up\n");
	qn8035_power_init();
	QND_Init();
	QND_SetSysMode(QND_MODE_RX|QND_MODE_FM);
	QN8035_mute(radio, 1);
	QN8035_enable(radio);
	
}

void qn8035_power_down(struct QN8035_device *radio)
{
	printk(KERN_ERR"=== qn8035 power down===\n");
	QNF_SetMute( 0);
	QN8035_disable(radio);
	qn8035_power_disable();
}

/* I2C probe: check if the device exists and register with v4l if it is */
static int __devinit QN8035_i2c_probe(struct i2c_client *client,
					const struct i2c_device_id *id)
{
	struct QN8035_device *radio;
	int ret;
	ret = qn8035_power_init();
	if(ret < 0) {
		return ret;
	}
	printk(KERN_ERR"QN8035_i2c_probe, addr=%x\n", client->addr);
	radio = kmalloc(sizeof(struct QN8035_device), GFP_KERNEL);
	if (!radio)
	return -ENOMEM;
	mutex_init(&radio->mutex);
	radio->i2c_client = client;
	this_client = client;
	init_device_members(radio);
	ret = QND_ReadReg(CID1);
	if (ret < 0) {
		ret = -EIO;
		goto errfr;
	}
	printk(KERN_ERR"CHIP_ID=0x%x\n", ret & 0x03);	
	QND_Init();
	radio->videodev = video_device_alloc();
	if (!(radio->videodev)) {
		ret = -ENOMEM;
		goto errfr;
	}
	memcpy(radio->videodev, &QN8035_radio_template,sizeof(QN8035_radio_template));
	i2c_set_clientdata(client, radio);
	video_set_drvdata(radio->videodev, radio);
	ret = video_register_device(radio->videodev, VFL_TYPE_RADIO, radio_nr);
	if (ret < 0) {
		PWARN("Could not register video device!");
		goto errrel;
	}
	QN8035_enable(radio);
	QN8035_mute(radio, 1);
	QN8035_disable(radio);	
	ret = sysfs_create_group(&client->dev.kobj, &qn8035_attr_group);
	if (ret) {
		printk(KERN_ERR"failed to create sysfs file for input device\n");
		goto errrel;
	}
	PINFO("registered.");	
	qn8035_power_down(radio);	
	return 0;
errrel:
	video_device_release(radio->videodev);
errfr:
	kfree(radio);
	return ret;
}

static int __devexit QN8035_i2c_remove(struct i2c_client *client)
{
	struct QN8035_device *radio = i2c_get_clientdata(client);

	PDEBUG("remove");
	if (radio) {
		QN8035_disable(radio);
		sysfs_remove_group(&client->dev.kobj, &qn8035_attr_group);
		video_unregister_device(radio->videodev);
		kfree(radio);
	}
	return 0;
}

/* I2C subsystem interface */
static const struct i2c_device_id QN8035_id[] = {
	{ "radio-QN8035", 0 },
	{ }					/* Terminating entry */
};
MODULE_DEVICE_TABLE(i2c, QN8035_id);

static struct i2c_driver QN8035_i2c_driver = {
	.driver = {
		.name = "radio-QN8035",
		.owner = THIS_MODULE,
	},
	.probe = QN8035_i2c_probe,
	.remove = __devexit_p(QN8035_i2c_remove),
	.id_table = QN8035_id,
};

/* init the driver */
static int __init QN8035_init(void)
{
	int ret = i2c_add_driver(&QN8035_i2c_driver);

	printk(KERN_INFO KBUILD_MODNAME ": " DRIVER_VERSION ": "
		DRIVER_DESC "\n");
	return ret;
}

/* cleanup the driver */
static void __exit QN8035_exit(void)
{
	i2c_del_driver(&QN8035_i2c_driver);
}

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

module_param(radio_nr, int, 0);
MODULE_PARM_DESC(radio_nr, "video4linux device number to use");

module_init(QN8035_init);
module_exit(QN8035_exit);

