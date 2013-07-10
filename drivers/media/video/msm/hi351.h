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


/*
[SENSOR]
Sensor Model:   hi351
Camera Module:
Lens Model:
Driver IC:
PV Size         = 640 x 480
Cap Size        = 2592 x 1944
Output Format   = YUYV
MCLK Speed      = 24M
PV DVP_PCLK     = 28M
Cap DVP_PCLK    = 56M
PV Frame Rate   = 30fps
Cap Frame Rate  = 7.5fps
I2C Slave ID    = 0x78
I2C Mode        = 16Addr, 8Data
*/

#ifndef CAMSENSOR_hi351
#define CAMSENSOR_hi351

#define INVMASK(v)  (0xff-v)

#if 0
#define hi351Core_WritePREG(pTbl)     hi351_WritePRegs(pTbl,sizeof(pTbl)/sizeof(pTbl[0]))

/*OV SENSOR SCCB*/
typedef struct {
	uint16_t addr;
	uint8_t data;
	uint8_t mask;
} hi351_WREG, *Phi351_WREG;

//Auto Focus Command
#define hi351_CMD_MAIN 0x3022
#define hi351_CMD_ACK 0x3023
#define hi351_CMD_PARA0 0x3024
#define hi351_CMD_PARA1 0x3025
#define hi351_CMD_PARA2 0x3026
#define hi351_CMD_PARA3 0x3027
#define hi351_CMD_PARA4 0x3028
#define hi351_CMD_FW_STATUS 0x3029
#define Capture_Framerate 750     //7.5fps capture frame rate
#define g_Preview_FrameRate 3000  //30fps preview frame rate
#endif


enum hi351_resolution_t {
  QTR_SIZE,
  FULL_SIZE,
  HFR_60FPS,
  HFR_90FPS,
  HFR_120FPS,
  INVALID_SIZE
};

//Sensor ID
#define  REG_hi351_MODEL_ID     0x04
#define hi351_MODEL_ID 0xa4

enum hi351_width{
	WORD_LEN,
	BYTE_LEN,
	BYTE_BYTE_LEN
};

struct hi351_i2c_reg_conf{
	unsigned short waddr;
	unsigned short wdata;
	enum hi351_width width;
	unsigned short mdelay_time;
};

struct hi351_reg {
/* -------------- sensor init -----------------*/
	const struct hi351_i2c_reg_conf *cambl;
	uint16_t cambl_size;

	const struct hi351_i2c_reg_conf *cambl_otp;
	uint16_t cambl_otp_size;	

	const struct hi351_i2c_reg_conf *cambl_otp2;
	uint16_t cambl_otp2_size;
	
/* ------------ white balance --------------*/
	const struct hi351_i2c_reg_conf *wbtbl_auto;
	uint16_t wbtbl_size_auto;
	const struct hi351_i2c_reg_conf *wbtbl_daylight;
	uint16_t wbtbl_size_daylight;
	const struct hi351_i2c_reg_conf *wbtbl_cloudy;
	uint16_t wbtbl_size_cloudy;
	const struct hi351_i2c_reg_conf *wbtbl_flourescent;
	uint16_t wbtbl_size_flourescent;
	const struct hi351_i2c_reg_conf *wbtbl_incandescent;
	uint16_t wbtbl_size_incandescent;
/* ------------- iso --------------  */
	const struct hi351_i2c_reg_conf *isotbl_auto;
	uint16_t isotbl_size_auto;
	const struct hi351_i2c_reg_conf *isotbl_100;
	uint16_t isotbl_size_100;
	const struct hi351_i2c_reg_conf *isotbl_200;
	uint16_t isotbl_size_200;
	const struct hi351_i2c_reg_conf *isotbl_400;
	uint16_t isotbl_size_400;
	const struct hi351_i2c_reg_conf *isotbl_800;
	uint16_t isotbl_size_800;

/* ---------------- exposure -----------------*/
	const struct hi351_i2c_reg_conf *exposuretbl_averg;
	uint16_t exposuretbl_size_averg;
	const struct hi351_i2c_reg_conf *exposuretbl_center;
	uint16_t exposuretbl_size_center;
	const struct hi351_i2c_reg_conf *exposuretbl_spot;
	uint16_t exposuretbl_size_spot;

/* -------------------- brightness --------------*/
	const struct hi351_i2c_reg_conf *brightnesstbl_0;
	uint16_t brightnesstbl_size_0;
	const struct hi351_i2c_reg_conf *brightnesstbl_1;
	uint16_t brightnesstbl_size_1;
	const struct hi351_i2c_reg_conf *brightnesstbl_2;
	uint16_t brightnesstbl_size_2;
	const struct hi351_i2c_reg_conf *brightnesstbl_3;
	uint16_t brightnesstbl_size_3;
	const struct hi351_i2c_reg_conf *brightnesstbl_4;
	uint16_t brightnesstbl_size_4;

/* -------------------- sharpness ----------------- */
	const struct hi351_i2c_reg_conf *sharpnesstbl_0;
	uint16_t sharpnesstbl_size_0;
	const struct hi351_i2c_reg_conf *sharpnesstbl_1;
	uint16_t sharpnesstbl_size_1;
	const struct hi351_i2c_reg_conf *sharpnesstbl_2;
	uint16_t sharpnesstbl_size_2;
	const struct hi351_i2c_reg_conf *sharpnesstbl_3;
	uint16_t sharpnesstbl_size_3;
	const struct hi351_i2c_reg_conf *sharpnesstbl_4;
	uint16_t sharpnesstbl_size_4;
/* --------------------- saturation ------------------*/
	const struct hi351_i2c_reg_conf *saturationtbl_0;
	uint16_t saturationtbl_size_0;
	const struct hi351_i2c_reg_conf *saturationtbl_1;
	uint16_t saturationtbl_size_1;
	const struct hi351_i2c_reg_conf *saturationtbl_2;
	uint16_t saturationtbl_size_2;
	const struct hi351_i2c_reg_conf *saturationtbl_3;
	uint16_t saturationtbl_size_3;
	const struct hi351_i2c_reg_conf *saturationtbl_4;
	uint16_t saturationtbl_size_4;
/* ------------- contrast  ------------ */
	const struct hi351_i2c_reg_conf *contrasttbl_0;
	uint16_t contrasttbl_size_0;
	const struct hi351_i2c_reg_conf *contrasttbl_1;
	uint16_t contrasttbl_size_1;
	const struct hi351_i2c_reg_conf *contrasttbl_2;
	uint16_t contrasttbl_size_2;
	const struct hi351_i2c_reg_conf *contrasttbl_3;
	uint16_t contrasttbl_size_3;
	const struct hi351_i2c_reg_conf *contrasttbl_4;
	uint16_t contrasttbl_size_4;
/* ------------- effect -----------------*/
	const struct hi351_i2c_reg_conf *effecttbl_off;
	uint16_t effecttbl_size_off;
	const struct hi351_i2c_reg_conf *effecttbl_mono;
	uint16_t effecttbl_size_mono;
	const struct hi351_i2c_reg_conf *effecttbl_negative;
	uint16_t effecttbl_size_negative;
	const struct hi351_i2c_reg_conf *effecttbl_solarize;
	uint16_t effecttbl_size_solarize;
	const struct hi351_i2c_reg_conf *effecttbl_sepia;
	uint16_t effecttbl_size_sepia;
/* -------------- set mode --------------*/
	const struct hi351_i2c_reg_conf *previewtbl;
	uint16_t previewtbl_size;
	const struct hi351_i2c_reg_conf *snapshottbl;
	uint16_t snapshottbl_size;
	const struct hi351_i2c_reg_conf *rawsnapshottbl;
	uint16_t rawsnapshottbl_size;
	const struct hi351_i2c_reg_conf *releasetbl;
	uint16_t releasetbl_size;

};

static const struct hi351_i2c_reg_conf const cam_init_tbl_otp[] =  {
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x01, 0xf1, BYTE_BYTE_LEN, 0}, //Initial_111221_AWB(EV)_target_ColorRatio_lsc75p_AGC_D0_50_deSat_Ysat
{0x01, 0xf3, BYTE_BYTE_LEN, 0},
{0x01, 0xf1, BYTE_BYTE_LEN, 0},
///////////////////////////////////////////
// OTP read
///////////////////////////////////////////
{0x03, 0x07, BYTE_BYTE_LEN, 0},  //Page setting
{0x40, 0x0c, BYTE_BYTE_LEN, 0},  //Timing setting
{0x47, 0x02, BYTE_BYTE_LEN, 0},

{0x11, 0x00, BYTE_BYTE_LEN, 0},  //Control register setting
{0x13, 0x10, BYTE_BYTE_LEN, 0},	             

{0x20, 0x00, BYTE_BYTE_LEN, 0},  //OTP_addr_H 
{0x21, 0x06, BYTE_BYTE_LEN, 0},  //OTP_addr_L	
//Data read             
//I2C Read (0x22) 
//I2C Read (0x22) 
};

static const struct hi351_i2c_reg_conf const cam_init_tbl_otp2[] =  {
{0x11, 0x00, BYTE_BYTE_LEN, 0},  //clear control register  
{0x13, 0x00, BYTE_BYTE_LEN, 0},	
///////////////////////////////////////////
// 48 Page MIPI setting
///////////////////////////////////////////
{0x03, 0x48, BYTE_BYTE_LEN, 0},
{0x09, 0xa2, BYTE_BYTE_LEN, 0}, //MIPI CLK
{0x10, 0x1C, BYTE_BYTE_LEN, 0}, //MIPI ON
{0x11, 0x00, BYTE_BYTE_LEN, 0}, //Normal Mode //[4] '1' contenous, '0'non-contenous
{0x14, 0x50, BYTE_BYTE_LEN, 0}, //Skew
{0x16, 0x04, BYTE_BYTE_LEN, 0}, 

{0x1a, 0x11, BYTE_BYTE_LEN, 0},
{0x1b, 0x0d, BYTE_BYTE_LEN, 0}, //Short Packet
{0x1c, 0x0a, BYTE_BYTE_LEN, 0}, //Control DP
{0x1d, 0x0f, BYTE_BYTE_LEN, 0}, //Control DN
{0x1e, 0x09, BYTE_BYTE_LEN, 0},
{0x1f, 0x04, BYTE_BYTE_LEN, 0}, // 20120305 LSW 0x07, BYTE_BYTE_LEN, 0},
{0x20, 0x00, BYTE_BYTE_LEN, 0},
{0x24, 0x1e, BYTE_BYTE_LEN, 0}, //Bayer8 : 2a, Bayer10 : 2b, YUV : 1e

{0x30, 0x00, BYTE_BYTE_LEN, 0}, //2048*2
{0x31, 0x05, BYTE_BYTE_LEN, 0},
{0x32, 0x0f, BYTE_BYTE_LEN, 0}, // Tclk zero
{0x34, 0x03, BYTE_BYTE_LEN, 0}, //120302 jk//0x06, BYTE_BYTE_LEN, 0}, // Tclk prepare
{0x35, 0x04, BYTE_BYTE_LEN, 0}, //20120305 LSW 0x06(default)

{0x39, 0x03, BYTE_BYTE_LEN, 0}, //Drivability 00

{0x03, 0x00, BYTE_BYTE_LEN, 0},
//{0x0c, 0xf0, BYTE_BYTE_LEN, 0}, //Parallel Line Off

{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x01, 0xf0, BYTE_BYTE_LEN, 0}, //sleep off

{0x03, 0xC0, BYTE_BYTE_LEN, 0},
{0x33, 0x00, BYTE_BYTE_LEN, 0},
{0x32, 0x01, BYTE_BYTE_LEN, 0}, //DMA on

//////////////////////////////////////////////
// Delay
//////////////////////////////////////////////
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0},
};	

static const struct hi351_i2c_reg_conf const cam_init_tbl[] =  {
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x01, 0xf1, BYTE_BYTE_LEN, 0}, //Initial_111221_AWB(EV)_target_ColorRatio_lsc75p_AGC_D0_50_deSat_Ysat
	{0x01, 0xf3, BYTE_BYTE_LEN, 0},
	{0x01, 0xf1, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// 0 Page PLL setting
	///////////////////////////////////////////
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x07, 0x25, BYTE_BYTE_LEN, 0}, //24/(5+1) = 4Mhz
	{0x08, 0x30, BYTE_BYTE_LEN, 0}, // 48Mhz
	{0x09, 0x82, BYTE_BYTE_LEN, 0},
	{0x07, 0xa5, BYTE_BYTE_LEN, 0},
	{0x07, 0xa5, BYTE_BYTE_LEN, 0},
	{0x09, 0xa2, BYTE_BYTE_LEN, 0},
	
	{0x0A, 0x01, BYTE_BYTE_LEN, 0}, // MCU hardware reset
	{0x0A, 0x00, BYTE_BYTE_LEN, 0},
	{0x0A, 0x01, BYTE_BYTE_LEN, 0},
	{0x0A, 0x00, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// 20 Page OTP/ROM LSC download select setting
	///////////////////////////////////////////
	{0x03, 0x20, BYTE_BYTE_LEN, 0},  
	{0x3a, 0x00, BYTE_BYTE_LEN, 0}, 
	{0x3b, 0x00, BYTE_BYTE_LEN, 0}, 
	{0x3c, 0x00, BYTE_BYTE_LEN, 0}, 
		
	///////////////////////////////////////////
	// 30 Page MCU reset, enable setting
	///////////////////////////////////////////
	{0x03, 0x30, BYTE_BYTE_LEN, 0},
	{0x30, 0x86, BYTE_BYTE_LEN, 0},
	{0x31, 0x00, BYTE_BYTE_LEN, 0},
	{0x32, 0x0c, BYTE_BYTE_LEN, 0},
	{0xe0, 0x02, BYTE_BYTE_LEN, 0},// CLK INVERSION
	{0x24, 0x02, BYTE_BYTE_LEN, 0},// PCON WRITE SET
	{0x25, 0x1e, BYTE_BYTE_LEN, 0},// PCON WAKE NORMAL
	{0x10, 0x80, BYTE_BYTE_LEN, 0},//jktest 0210 400khz//0x81, BYTE_BYTE_LEN, 0}, // mcu reset high
	{0x10, 0x88, BYTE_BYTE_LEN, 0},//jktest 0210 400khz//0x89, BYTE_BYTE_LEN, 0}, // mcu enable high
	{0x11, 0x08, BYTE_BYTE_LEN, 0}, // xdata memory reset high
	{0x11, 0x00, BYTE_BYTE_LEN, 0}, // xdata memory reset low
	
	///////////////////////////////////////////
	// 7 Page OTP/ROM color ratio download select setting
	/////////////////////////////////////////// 
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	{0x12, 0x07, BYTE_BYTE_LEN, 0}, 
	{0x40, 0x0E, BYTE_BYTE_LEN, 0}, 
	{0x47, 0x03, BYTE_BYTE_LEN, 0}, 
	{0x2e, 0x00, BYTE_BYTE_LEN, 0}, 				
	{0x2f, 0x20, BYTE_BYTE_LEN, 0}, 
	{0x30, 0x00, BYTE_BYTE_LEN, 0}, 	
	{0x31, 0xD6, BYTE_BYTE_LEN, 0}, 
	{0x32, 0x00, BYTE_BYTE_LEN, 0}, 
	{0x33, 0xFF, BYTE_BYTE_LEN, 0}, 
	{0x10, 0x02, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x07, BYTE_BYTE_LEN, 0}, //delay_setting
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	
	{0x2e, 0x03, BYTE_BYTE_LEN, 0}, // color ratio reg down
	{0x2f, 0x20, BYTE_BYTE_LEN, 0},
	{0x30, 0x20, BYTE_BYTE_LEN, 0},
	{0x31, 0xa6, BYTE_BYTE_LEN, 0},
	{0x32, 0x01, BYTE_BYTE_LEN, 0},
	{0x33, 0x00, BYTE_BYTE_LEN, 0},
	{0x10, 0x02, BYTE_BYTE_LEN, 0},
	
	
	{0x03, 0x07, BYTE_BYTE_LEN, 0}, //delay_setting
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	
	{0x12, 0x00, BYTE_BYTE_LEN, 0}, 
	{0x98, 0x00, BYTE_BYTE_LEN, 0}, 
	{0x97, 0x01, BYTE_BYTE_LEN, 0}, 
	 
	{0x8C, 0x08, BYTE_BYTE_LEN, 0},
	{0x8F, 0x20, BYTE_BYTE_LEN, 0},
	{0x92, 0x4C, BYTE_BYTE_LEN, 0},
	{0x93, 0x00, BYTE_BYTE_LEN, 0}, // Set OTP Offset
	{0x94, 0xF0, BYTE_BYTE_LEN, 0},
	{0x95, 0x00, BYTE_BYTE_LEN, 0}, // Full Size normal XY-flip 

	///////////////////////////////////////////
	// 30 Page MCU reset, enable setting
	///////////////////////////////////////////
	{0x03, 0x30, BYTE_BYTE_LEN, 0},
	{0x10, 0x08, BYTE_BYTE_LEN, 0},//jktest 0210 400khz//0x09, BYTE_BYTE_LEN, 0}, //0x08, BYTE_BYTE_LEN, 0},//i2c 400k speed up jktest 0209//0x09, BYTE_BYTE_LEN, 0}, // mcu reset low  = mcu start!!
	
	///////////////////////////////////////////
	// 0 Page 
	///////////////////////////////////////////
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x0B, 0x02, BYTE_BYTE_LEN, 0}, //PLL lock time
	{0x10, 0x00, BYTE_BYTE_LEN, 0},
	
	{0x13, 0x80, BYTE_BYTE_LEN, 0},
	{0x14, 0x70, BYTE_BYTE_LEN, 0},
	{0x15, 0x03, BYTE_BYTE_LEN, 0},
	{0x17, 0x04, BYTE_BYTE_LEN, 0}, //Parallel, MIPI : 04, JPEG : 0c
	
	{0x20,0x00, BYTE_BYTE_LEN, 0},
	{0x21,0x00, BYTE_BYTE_LEN, 0},
	{0x22,0x00, BYTE_BYTE_LEN, 0},
	{0x23,0x12, BYTE_BYTE_LEN, 0},
	{0x24,0x06, BYTE_BYTE_LEN, 0},
	{0x25,0x00, BYTE_BYTE_LEN, 0},
	{0x26,0x08, BYTE_BYTE_LEN, 0},
	{0x27,0x00, BYTE_BYTE_LEN, 0},
	
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //Page 0
{0x50, 0x02, BYTE_BYTE_LEN, 0}, //Hblank 678
{0x51, 0xa6, BYTE_BYTE_LEN, 0},
{0x52, 0x00, BYTE_BYTE_LEN, 0}, //Vblank 80
{0x53, 0x50, BYTE_BYTE_LEN, 0},

	//BLC             
	{0x80,0x02, BYTE_BYTE_LEN, 0},
	{0x81,0x87, BYTE_BYTE_LEN, 0},
	{0x82,0x28, BYTE_BYTE_LEN, 0},
	{0x83,0x08, BYTE_BYTE_LEN, 0},
	{0x84,0x8c, BYTE_BYTE_LEN, 0},
	{0x85,0x0c, BYTE_BYTE_LEN, 0}, //blc on
	{0x86,0x00, BYTE_BYTE_LEN, 0},
	{0x87,0x00, BYTE_BYTE_LEN, 0}, //dont touch by KHS//0xf0, BYTE_BYTE_LEN, 0},
	
	{0x88,0x98, BYTE_BYTE_LEN, 0},
	{0x89,0x10, BYTE_BYTE_LEN, 0},
	{0x8a,0x80, BYTE_BYTE_LEN, 0},
	{0x8b,0x00, BYTE_BYTE_LEN, 0},
	{0x8c,0x80, BYTE_BYTE_LEN, 0},
	{0x8d,0x00, BYTE_BYTE_LEN, 0},
	{0x8e,0x80, BYTE_BYTE_LEN, 0},
	{0x8f,0x0f, BYTE_BYTE_LEN, 0},     

	{0x90, 0x0c, BYTE_BYTE_LEN, 0}, //BLC_TIME_TH_ON
	{0x91, 0x0c, BYTE_BYTE_LEN, 0}, //BLC_TIME_TH_OFF 
{0x92, 0xa0, BYTE_BYTE_LEN, 0}, //BLC_AG_TH_ON
{0x93, 0x90, BYTE_BYTE_LEN, 0}, //BLC_AG_TH_OFF
	{0x94,0x80, BYTE_BYTE_LEN, 0},   
	{0x95,0x00, BYTE_BYTE_LEN, 0},   
	{0x96,0xfe, BYTE_BYTE_LEN, 0}, //BLC_OUT_TH   
	{0x97,0xfd, BYTE_BYTE_LEN, 0}, //BLC_OUT_TH   
	{0x98,0x20, BYTE_BYTE_LEN, 0}, 

	{0xa0,0x85, BYTE_BYTE_LEN, 0}, //odd_adj_normal            
	{0xa1,0x83, BYTE_BYTE_LEN, 0}, //out r
	{0xa2,0x88, BYTE_BYTE_LEN, 0}, //in
	{0xa3,0x8b, BYTE_BYTE_LEN, 0}, //dark
	{0xa4,0x85, BYTE_BYTE_LEN, 0}, //even_adj_normal             
	{0xa5,0x85, BYTE_BYTE_LEN, 0}, //out b 
	{0xa6,0x87, BYTE_BYTE_LEN, 0}, //in
	{0xa7,0x89, BYTE_BYTE_LEN, 0}, //dark   

	{0xbb,0x20, BYTE_BYTE_LEN, 0},
	///////////////////////////////////////////
	// 2 Page								   
	///////////////////////////////////////////
	
	{0x03, 0x02, BYTE_BYTE_LEN, 0},
	{0x10, 0x00, BYTE_BYTE_LEN, 0},
	{0x13, 0x00, BYTE_BYTE_LEN, 0},
	{0x14, 0x00, BYTE_BYTE_LEN, 0},
	{0x15, 0x08, BYTE_BYTE_LEN, 0},
	{0x1a, 0x00, BYTE_BYTE_LEN, 0},//ncp adaptive off
	{0x1b, 0x00, BYTE_BYTE_LEN, 0},
	{0x1c, 0xc0, BYTE_BYTE_LEN, 0},
	{0x1d, 0x00, BYTE_BYTE_LEN, 0},//MCU update bit[4]
	{0x20, 0x44, BYTE_BYTE_LEN, 0},
	{0x21, 0x02, BYTE_BYTE_LEN, 0},
	{0x22, 0x22, BYTE_BYTE_LEN, 0},
	{0x23, 0x30, BYTE_BYTE_LEN, 0},//clamp on 10 -30
	{0x24, 0x77, BYTE_BYTE_LEN, 0},
	{0x2b, 0x00, BYTE_BYTE_LEN, 0},
	{0x2c, 0x0C, BYTE_BYTE_LEN, 0},
	{0x2d, 0x80, BYTE_BYTE_LEN, 0},
	{0x2e, 0x00, BYTE_BYTE_LEN, 0},
	{0x2f, 0x00, BYTE_BYTE_LEN, 0},
	{0x30, 0x00, BYTE_BYTE_LEN, 0},
	{0x31, 0xf0, BYTE_BYTE_LEN, 0},
	{0x32, 0x22, BYTE_BYTE_LEN, 0},
	{0x33, 0x42, BYTE_BYTE_LEN, 0}, //auto flicker
	{0x34, 0x30, BYTE_BYTE_LEN, 0},
	{0x35, 0x00, BYTE_BYTE_LEN, 0},
	{0x36, 0x08, BYTE_BYTE_LEN, 0},
	{0x37, 0x40, BYTE_BYTE_LEN, 0}, //auto flicker
	{0x38, 0x14, BYTE_BYTE_LEN, 0},
	{0x39, 0x02, BYTE_BYTE_LEN, 0}, 
	{0x3a, 0x00, BYTE_BYTE_LEN, 0},
	
	{0x3d, 0x70, BYTE_BYTE_LEN, 0},
	{0x3e, 0x04, BYTE_BYTE_LEN, 0},
	{0x3f, 0x00, BYTE_BYTE_LEN, 0},
	{0x40, 0x01, BYTE_BYTE_LEN, 0},
	{0x41, 0x8a, BYTE_BYTE_LEN, 0},
	{0x42, 0x00, BYTE_BYTE_LEN, 0},
	{0x43, 0x25, BYTE_BYTE_LEN, 0},
	{0x44, 0x00, BYTE_BYTE_LEN, 0},
	{0x46, 0x00, BYTE_BYTE_LEN, 0},
	{0x47, 0x00, BYTE_BYTE_LEN, 0},
	{0x48, 0x3C, BYTE_BYTE_LEN, 0},
	{0x49, 0x10, BYTE_BYTE_LEN, 0},
	{0x4a, 0x00, BYTE_BYTE_LEN, 0},
	{0x4b, 0x10, BYTE_BYTE_LEN, 0},
	{0x4c, 0x08, BYTE_BYTE_LEN, 0},
	{0x4d, 0x70, BYTE_BYTE_LEN, 0},
	{0x4e, 0x04, BYTE_BYTE_LEN, 0},
	{0x4f, 0x38, BYTE_BYTE_LEN, 0},
	{0x50, 0xa0, BYTE_BYTE_LEN, 0},
	{0x51, 0x00, BYTE_BYTE_LEN, 0},
	{0x52, 0x70, BYTE_BYTE_LEN, 0},  
	{0x53, 0x00, BYTE_BYTE_LEN, 0},
	{0x54, 0xc0, BYTE_BYTE_LEN, 0},
	{0x55, 0x40, BYTE_BYTE_LEN, 0},
	{0x56, 0x11, BYTE_BYTE_LEN, 0},
	{0x57, 0x00, BYTE_BYTE_LEN, 0},
	{0x58, 0x10, BYTE_BYTE_LEN, 0},
	{0x59, 0x0E, BYTE_BYTE_LEN, 0},
	{0x5a, 0x00, BYTE_BYTE_LEN, 0},
	{0x5b, 0x00, BYTE_BYTE_LEN, 0},
	{0x5c, 0x00, BYTE_BYTE_LEN, 0},
	{0x5d, 0x00, BYTE_BYTE_LEN, 0},
	{0x60, 0x04, BYTE_BYTE_LEN, 0},
	{0x61, 0xe2, BYTE_BYTE_LEN, 0},
	{0x62, 0x00, BYTE_BYTE_LEN, 0},
	{0x63, 0xc8, BYTE_BYTE_LEN, 0},
	{0x64, 0x00, BYTE_BYTE_LEN, 0},
	{0x65, 0x00, BYTE_BYTE_LEN, 0},
	{0x66, 0x00, BYTE_BYTE_LEN, 0},
	{0x67, 0x3f, BYTE_BYTE_LEN, 0},
	{0x68, 0x3f, BYTE_BYTE_LEN, 0},
	{0x69, 0x3f, BYTE_BYTE_LEN, 0},
	{0x6a, 0x04, BYTE_BYTE_LEN, 0},
	{0x6b, 0x38, BYTE_BYTE_LEN, 0},
	{0x6c, 0x00, BYTE_BYTE_LEN, 0},
	{0x6d, 0x00, BYTE_BYTE_LEN, 0},
	{0x6e, 0x00, BYTE_BYTE_LEN, 0},
	{0x6f, 0x00, BYTE_BYTE_LEN, 0},
	{0x70, 0x00, BYTE_BYTE_LEN, 0},
	{0x71, 0x50, BYTE_BYTE_LEN, 0},
	{0x72, 0x05, BYTE_BYTE_LEN, 0},
	{0x73, 0xa5, BYTE_BYTE_LEN, 0},
	{0x74, 0x00, BYTE_BYTE_LEN, 0},
	{0x75, 0x50, BYTE_BYTE_LEN, 0},
	{0x76, 0x02, BYTE_BYTE_LEN, 0},
	{0x77, 0xfa, BYTE_BYTE_LEN, 0},
	{0x78, 0x01, BYTE_BYTE_LEN, 0},
	{0x79, 0xb4, BYTE_BYTE_LEN, 0},
	{0x7a, 0x01, BYTE_BYTE_LEN, 0},
	{0x7b, 0xb8, BYTE_BYTE_LEN, 0},
	{0x7c, 0x00, BYTE_BYTE_LEN, 0},
	{0x7d, 0x00, BYTE_BYTE_LEN, 0},
	{0x7e, 0x00, BYTE_BYTE_LEN, 0},
	{0x7f, 0x00, BYTE_BYTE_LEN, 0},
	{0xa0, 0x00, BYTE_BYTE_LEN, 0},
	{0xa1, 0xEB, BYTE_BYTE_LEN, 0},
	{0xa2, 0x02, BYTE_BYTE_LEN, 0},
	{0xa3, 0x2D, BYTE_BYTE_LEN, 0},
	{0xa4, 0x02, BYTE_BYTE_LEN, 0},
	{0xa5, 0xB9, BYTE_BYTE_LEN, 0},
	{0xa6, 0x05, BYTE_BYTE_LEN, 0},
	{0xa7, 0xED, BYTE_BYTE_LEN, 0},
	{0xa8, 0x00, BYTE_BYTE_LEN, 0},
	{0xa9, 0xEB, BYTE_BYTE_LEN, 0},
	{0xaa, 0x01, BYTE_BYTE_LEN, 0},
	{0xab, 0xED, BYTE_BYTE_LEN, 0},
	{0xac, 0x02, BYTE_BYTE_LEN, 0},
	{0xad, 0x79, BYTE_BYTE_LEN, 0},
	{0xae, 0x04, BYTE_BYTE_LEN, 0},
	{0xaf, 0x2D, BYTE_BYTE_LEN, 0},
	{0xb0, 0x00, BYTE_BYTE_LEN, 0},
	{0xb1, 0x56, BYTE_BYTE_LEN, 0},
	{0xb2, 0x01, BYTE_BYTE_LEN, 0},
	{0xb3, 0x08, BYTE_BYTE_LEN, 0},
	{0xb4, 0x00, BYTE_BYTE_LEN, 0},
	{0xb5, 0x2B, BYTE_BYTE_LEN, 0},
	{0xb6, 0x03, BYTE_BYTE_LEN, 0},
	{0xb7, 0x2B, BYTE_BYTE_LEN, 0},
	{0xb8, 0x00, BYTE_BYTE_LEN, 0},
	{0xb9, 0x56, BYTE_BYTE_LEN, 0},
	{0xba, 0x00, BYTE_BYTE_LEN, 0},
	{0xbb, 0xC8, BYTE_BYTE_LEN, 0},
	{0xbc, 0x00, BYTE_BYTE_LEN, 0},
	{0xbd, 0x2B, BYTE_BYTE_LEN, 0},
	{0xbe, 0x01, BYTE_BYTE_LEN, 0},
	{0xbf, 0xAB, BYTE_BYTE_LEN, 0},
	{0xc0, 0x00, BYTE_BYTE_LEN, 0},
	{0xc1, 0x54, BYTE_BYTE_LEN, 0},
	{0xc2, 0x01, BYTE_BYTE_LEN, 0},
	{0xc3, 0x0A, BYTE_BYTE_LEN, 0},
	{0xc4, 0x00, BYTE_BYTE_LEN, 0},
	{0xc5, 0x29, BYTE_BYTE_LEN, 0},
	{0xc6, 0x03, BYTE_BYTE_LEN, 0},
	{0xc7, 0x2D, BYTE_BYTE_LEN, 0},
	{0xc8, 0x00, BYTE_BYTE_LEN, 0},
	{0xc9, 0x54, BYTE_BYTE_LEN, 0},
	{0xca, 0x00, BYTE_BYTE_LEN, 0},
	{0xcb, 0xCA, BYTE_BYTE_LEN, 0},
	{0xcc, 0x00, BYTE_BYTE_LEN, 0},
	{0xcd, 0x29, BYTE_BYTE_LEN, 0},
	{0xce, 0x01, BYTE_BYTE_LEN, 0},
	{0xcf, 0xAD, BYTE_BYTE_LEN, 0},
	{0xd0, 0x10, BYTE_BYTE_LEN, 0},
	{0xd1, 0x14, BYTE_BYTE_LEN, 0},
	{0xd2, 0x20, BYTE_BYTE_LEN, 0},
	{0xd3, 0x00, BYTE_BYTE_LEN, 0},

	{0xd4, 0x0c, BYTE_BYTE_LEN, 0}, //DCDC_TIME_TH_ON
	{0xd5, 0x0c, BYTE_BYTE_LEN, 0}, //DCDC_TIME_TH_OFF 
{0xd6, 0xa0, BYTE_BYTE_LEN, 0}, //DCDC_AG_TH_ON
{0xd7, 0x90, BYTE_BYTE_LEN, 0}, //DCDC_AG_TH_OFF
	{0xE0, 0xf0, BYTE_BYTE_LEN, 0},//ncp adaptive
	{0xE1, 0xf0, BYTE_BYTE_LEN, 0},//ncp adaptive
	{0xE2, 0xf0, BYTE_BYTE_LEN, 0},//ncp adaptive
	{0xE3, 0xf0, BYTE_BYTE_LEN, 0},//ncp adaptive
	{0xE4, 0xd0, BYTE_BYTE_LEN, 0},//ncp adaptive e1 -) d0
	{0xE5, 0x00, BYTE_BYTE_LEN, 0},//ncp adaptive
	{0xE6, 0x00, BYTE_BYTE_LEN, 0},
	{0xE7, 0x00, BYTE_BYTE_LEN, 0},
	{0xE8, 0x00, BYTE_BYTE_LEN, 0},
	{0xE9, 0x00, BYTE_BYTE_LEN, 0},
	{0xEA, 0x15, BYTE_BYTE_LEN, 0},
	{0xEB, 0x15, BYTE_BYTE_LEN, 0},
	{0xEC, 0x15, BYTE_BYTE_LEN, 0},
	{0xED, 0x05, BYTE_BYTE_LEN, 0},
	{0xEE, 0x05, BYTE_BYTE_LEN, 0},
	{0xEF, 0x65, BYTE_BYTE_LEN, 0},
	{0xF0, 0x0c, BYTE_BYTE_LEN, 0},
	{0xF3, 0x05, BYTE_BYTE_LEN, 0},
	{0xF4, 0x0a, BYTE_BYTE_LEN, 0},
	{0xF5, 0x05, BYTE_BYTE_LEN, 0},
	{0xF6, 0x05, BYTE_BYTE_LEN, 0},
	{0xF7, 0x15, BYTE_BYTE_LEN, 0},
	{0xF8, 0x15, BYTE_BYTE_LEN, 0},
	{0xF9, 0x15, BYTE_BYTE_LEN, 0},
	{0xFA, 0x15, BYTE_BYTE_LEN, 0},
	{0xFB, 0x15, BYTE_BYTE_LEN, 0},
	{0xFC, 0x55, BYTE_BYTE_LEN, 0},
	{0xFD, 0x55, BYTE_BYTE_LEN, 0},
	{0xFE, 0x05, BYTE_BYTE_LEN, 0}, 
	{0xFF, 0x00, BYTE_BYTE_LEN, 0},
	///////////////////////////////////////////
	//3Page
	///////////////////////////////////////////
	{0x03, 0x03, BYTE_BYTE_LEN, 0},
	{0x10, 0x00, BYTE_BYTE_LEN, 0},
	{0x11, 0x64, BYTE_BYTE_LEN, 0},
	{0x12, 0x00, BYTE_BYTE_LEN, 0},
	{0x13, 0x32, BYTE_BYTE_LEN, 0},
	{0x14, 0x02, BYTE_BYTE_LEN, 0},
	{0x15, 0x51, BYTE_BYTE_LEN, 0},
	{0x16, 0x02, BYTE_BYTE_LEN, 0},
	{0x17, 0x59, BYTE_BYTE_LEN, 0},
	{0x18, 0x00, BYTE_BYTE_LEN, 0},
	{0x19, 0x97, BYTE_BYTE_LEN, 0},
	{0x1a, 0x01, BYTE_BYTE_LEN, 0},
	{0x1b, 0x7C, BYTE_BYTE_LEN, 0},
	{0x1c, 0x00, BYTE_BYTE_LEN, 0},
	{0x1d, 0x97, BYTE_BYTE_LEN, 0},
	{0x1e, 0x01, BYTE_BYTE_LEN, 0},
	{0x1f, 0x7C, BYTE_BYTE_LEN, 0},
	{0x20, 0x00, BYTE_BYTE_LEN, 0},
	{0x21, 0x97, BYTE_BYTE_LEN, 0},
	{0x22, 0x00, BYTE_BYTE_LEN, 0},
	{0x23, 0xe3, BYTE_BYTE_LEN, 0}, //cds 2 off time sunspot
	{0x24, 0x00, BYTE_BYTE_LEN, 0},
	{0x25, 0x97, BYTE_BYTE_LEN, 0},
	{0x26, 0x00, BYTE_BYTE_LEN, 0},
	{0x27, 0xe3, BYTE_BYTE_LEN, 0}, //cds 2 off time  sunspot
	           
	{0x28, 0x00, BYTE_BYTE_LEN, 0},
	{0x29, 0x97, BYTE_BYTE_LEN, 0},
	{0x2a, 0x00, BYTE_BYTE_LEN, 0},
	{0x2b, 0xE6, BYTE_BYTE_LEN, 0},
	{0x2c, 0x00, BYTE_BYTE_LEN, 0},
	{0x2d, 0x97, BYTE_BYTE_LEN, 0},
	{0x2e, 0x00, BYTE_BYTE_LEN, 0},
	{0x2f, 0xE6, BYTE_BYTE_LEN, 0},
	{0x30, 0x00, BYTE_BYTE_LEN, 0},
	{0x31, 0x0a, BYTE_BYTE_LEN, 0},
	{0x32, 0x03, BYTE_BYTE_LEN, 0},
	{0x33, 0x31, BYTE_BYTE_LEN, 0},
	{0x34, 0x00, BYTE_BYTE_LEN, 0},
	{0x35, 0x0a, BYTE_BYTE_LEN, 0},
	{0x36, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x38, 0x00, BYTE_BYTE_LEN, 0},
	{0x39, 0x0A, BYTE_BYTE_LEN, 0},
	{0x3a, 0x01, BYTE_BYTE_LEN, 0},
	{0x3b, 0xB0, BYTE_BYTE_LEN, 0},
	{0x3c, 0x00, BYTE_BYTE_LEN, 0},
	{0x3d, 0x0A, BYTE_BYTE_LEN, 0},
	{0x3e, 0x01, BYTE_BYTE_LEN, 0},
	{0x3f, 0xB0, BYTE_BYTE_LEN, 0},
	{0x40, 0x00, BYTE_BYTE_LEN, 0},
	{0x41, 0x04, BYTE_BYTE_LEN, 0},
	{0x42, 0x00, BYTE_BYTE_LEN, 0},
	{0x43, 0x1c, BYTE_BYTE_LEN, 0},
	{0x44, 0x00, BYTE_BYTE_LEN, 0},
	{0x45, 0x02, BYTE_BYTE_LEN, 0},
	{0x46, 0x00, BYTE_BYTE_LEN, 0},
	{0x47, 0x34, BYTE_BYTE_LEN, 0},
	{0x48, 0x00, BYTE_BYTE_LEN, 0},
	{0x49, 0x06, BYTE_BYTE_LEN, 0},
	{0x4a, 0x00, BYTE_BYTE_LEN, 0},
	{0x4b, 0x1a, BYTE_BYTE_LEN, 0},
	{0x4c, 0x00, BYTE_BYTE_LEN, 0},
	{0x4d, 0x06, BYTE_BYTE_LEN, 0},
	{0x4e, 0x00, BYTE_BYTE_LEN, 0},
	{0x4f, 0x1a, BYTE_BYTE_LEN, 0},
	{0x50, 0x00, BYTE_BYTE_LEN, 0},
	{0x51, 0x08, BYTE_BYTE_LEN, 0},
	{0x52, 0x00, BYTE_BYTE_LEN, 0},
	{0x53, 0x18, BYTE_BYTE_LEN, 0},
	{0x54, 0x00, BYTE_BYTE_LEN, 0},
	{0x55, 0x08, BYTE_BYTE_LEN, 0},
	{0x56, 0x00, BYTE_BYTE_LEN, 0},
	{0x57, 0x18, BYTE_BYTE_LEN, 0},
	{0x58, 0x00, BYTE_BYTE_LEN, 0},
	{0x59, 0x08, BYTE_BYTE_LEN, 0},
	{0x5A, 0x00, BYTE_BYTE_LEN, 0},
	{0x5b, 0x18, BYTE_BYTE_LEN, 0},
	{0x5c, 0x00, BYTE_BYTE_LEN, 0},
	{0x5d, 0x06, BYTE_BYTE_LEN, 0},
	{0x5e, 0x00, BYTE_BYTE_LEN, 0},
	{0x5f, 0x1c, BYTE_BYTE_LEN, 0},
	{0x60, 0x00, BYTE_BYTE_LEN, 0},
	{0x61, 0x00, BYTE_BYTE_LEN, 0},
	{0x62, 0x00, BYTE_BYTE_LEN, 0},
	{0x63, 0x00, BYTE_BYTE_LEN, 0},
	{0x64, 0x00, BYTE_BYTE_LEN, 0},
	{0x65, 0x00, BYTE_BYTE_LEN, 0},
	{0x66, 0x00, BYTE_BYTE_LEN, 0},
	{0x67, 0x00, BYTE_BYTE_LEN, 0},
	{0x68, 0x00, BYTE_BYTE_LEN, 0},
	{0x69, 0x02, BYTE_BYTE_LEN, 0},
	{0x6A, 0x00, BYTE_BYTE_LEN, 0},
	{0x6B, 0x1e, BYTE_BYTE_LEN, 0},
	{0x6C, 0x00, BYTE_BYTE_LEN, 0},
	{0x6D, 0x00, BYTE_BYTE_LEN, 0},
	{0x6E, 0x00, BYTE_BYTE_LEN, 0},
	{0x6F, 0x00, BYTE_BYTE_LEN, 0},
	{0x70, 0x00, BYTE_BYTE_LEN, 0},
	{0x71, 0x66, BYTE_BYTE_LEN, 0},
	{0x72, 0x01, BYTE_BYTE_LEN, 0},
	{0x73, 0x86, BYTE_BYTE_LEN, 0},
	{0x74, 0x00, BYTE_BYTE_LEN, 0},
	{0x75, 0x6B, BYTE_BYTE_LEN, 0},
	{0x76, 0x00, BYTE_BYTE_LEN, 0},
	{0x77, 0x93, BYTE_BYTE_LEN, 0},
	{0x78, 0x01, BYTE_BYTE_LEN, 0},
	{0x79, 0x84, BYTE_BYTE_LEN, 0},
	{0x7a, 0x01, BYTE_BYTE_LEN, 0},
	{0x7b, 0x88, BYTE_BYTE_LEN, 0},
	{0x7c, 0x01, BYTE_BYTE_LEN, 0},
	{0x7d, 0x84, BYTE_BYTE_LEN, 0},
	{0x7e, 0x01, BYTE_BYTE_LEN, 0},
	{0x7f, 0x88, BYTE_BYTE_LEN, 0},
	{0x80, 0x01, BYTE_BYTE_LEN, 0},
	{0x81, 0x13, BYTE_BYTE_LEN, 0},
	{0x82, 0x01, BYTE_BYTE_LEN, 0},
	{0x83, 0x3B, BYTE_BYTE_LEN, 0},
	{0x84, 0x01, BYTE_BYTE_LEN, 0},
	{0x85, 0x84, BYTE_BYTE_LEN, 0},
	{0x86, 0x01, BYTE_BYTE_LEN, 0},
	{0x87, 0x88, BYTE_BYTE_LEN, 0},
	{0x88, 0x01, BYTE_BYTE_LEN, 0},
	{0x89, 0x84, BYTE_BYTE_LEN, 0},
	{0x8a, 0x01, BYTE_BYTE_LEN, 0},
	{0x8b, 0x88, BYTE_BYTE_LEN, 0},
	{0x8c, 0x01, BYTE_BYTE_LEN, 0},
	{0x8d, 0x16, BYTE_BYTE_LEN, 0},
	{0x8e, 0x01, BYTE_BYTE_LEN, 0},
	{0x8f, 0x42, BYTE_BYTE_LEN, 0},
	{0x90, 0x00, BYTE_BYTE_LEN, 0},
	{0x91, 0x68, BYTE_BYTE_LEN, 0},
	{0x92, 0x01, BYTE_BYTE_LEN, 0},
	{0x93, 0x80, BYTE_BYTE_LEN, 0},
	{0x94, 0x00, BYTE_BYTE_LEN, 0},
	{0x95, 0x68, BYTE_BYTE_LEN, 0},
	{0x96, 0x01, BYTE_BYTE_LEN, 0},
	{0x97, 0x80, BYTE_BYTE_LEN, 0},
	{0x98, 0x01, BYTE_BYTE_LEN, 0},
	{0x99, 0x80, BYTE_BYTE_LEN, 0},
	{0x9a, 0x00, BYTE_BYTE_LEN, 0},
	{0x9b, 0x68, BYTE_BYTE_LEN, 0},
	{0x9c, 0x01, BYTE_BYTE_LEN, 0},
	{0x9d, 0x80, BYTE_BYTE_LEN, 0},
	{0x9e, 0x00, BYTE_BYTE_LEN, 0},
	{0x9f, 0x68, BYTE_BYTE_LEN, 0},
	{0xa0, 0x00, BYTE_BYTE_LEN, 0},
	{0xa1, 0x08, BYTE_BYTE_LEN, 0},
	{0xa2, 0x00, BYTE_BYTE_LEN, 0},
	{0xa3, 0x04, BYTE_BYTE_LEN, 0},
	{0xa4, 0x00, BYTE_BYTE_LEN, 0},
	{0xa5, 0x08, BYTE_BYTE_LEN, 0},
	{0xa6, 0x00, BYTE_BYTE_LEN, 0},
	{0xa7, 0x04, BYTE_BYTE_LEN, 0},
	{0xa8, 0x00, BYTE_BYTE_LEN, 0},
	{0xa9, 0x73, BYTE_BYTE_LEN, 0},
	{0xaa, 0x00, BYTE_BYTE_LEN, 0},
	{0xab, 0x64, BYTE_BYTE_LEN, 0},
	{0xac, 0x00, BYTE_BYTE_LEN, 0},
	{0xad, 0x73, BYTE_BYTE_LEN, 0},
	{0xae, 0x00, BYTE_BYTE_LEN, 0},
	{0xaf, 0x64, BYTE_BYTE_LEN, 0},
	{0xc0, 0x00, BYTE_BYTE_LEN, 0},
	{0xc1, 0x1d, BYTE_BYTE_LEN, 0},
	{0xc2, 0x00, BYTE_BYTE_LEN, 0},
	{0xc3, 0x2f, BYTE_BYTE_LEN, 0},
	{0xc4, 0x00, BYTE_BYTE_LEN, 0},
	{0xc5, 0x1d, BYTE_BYTE_LEN, 0},
	{0xc6, 0x00, BYTE_BYTE_LEN, 0},
	{0xc7, 0x2f, BYTE_BYTE_LEN, 0},
	{0xc8, 0x00, BYTE_BYTE_LEN, 0},
	{0xc9, 0x1f, BYTE_BYTE_LEN, 0},
	{0xca, 0x00, BYTE_BYTE_LEN, 0},
	{0xcb, 0x2d, BYTE_BYTE_LEN, 0},
	{0xcc, 0x00, BYTE_BYTE_LEN, 0},
	{0xcd, 0x1f, BYTE_BYTE_LEN, 0},
	{0xce, 0x00, BYTE_BYTE_LEN, 0},
	{0xcf, 0x2d, BYTE_BYTE_LEN, 0},
	{0xd0, 0x00, BYTE_BYTE_LEN, 0},
	{0xd1, 0x21, BYTE_BYTE_LEN, 0},
	{0xd2, 0x00, BYTE_BYTE_LEN, 0},
	{0xd3, 0x2b, BYTE_BYTE_LEN, 0},
	{0xd4, 0x00, BYTE_BYTE_LEN, 0},
	{0xd5, 0x21, BYTE_BYTE_LEN, 0},
	{0xd6, 0x00, BYTE_BYTE_LEN, 0},
	{0xd7, 0x2b, BYTE_BYTE_LEN, 0},
	{0xd8, 0x00, BYTE_BYTE_LEN, 0},
	{0xd9, 0x23, BYTE_BYTE_LEN, 0},
	{0xdA, 0x00, BYTE_BYTE_LEN, 0},
	{0xdB, 0x29, BYTE_BYTE_LEN, 0},
	{0xdC, 0x00, BYTE_BYTE_LEN, 0},
	{0xdD, 0x23, BYTE_BYTE_LEN, 0},
	{0xdE, 0x00, BYTE_BYTE_LEN, 0},
	{0xdF, 0x29, BYTE_BYTE_LEN, 0},
	{0xe0, 0x00, BYTE_BYTE_LEN, 0},
	{0xe1, 0x6B, BYTE_BYTE_LEN, 0},
	{0xe2, 0x00, BYTE_BYTE_LEN, 0},
	{0xe3, 0xE8, BYTE_BYTE_LEN, 0},
	{0xe4, 0x00, BYTE_BYTE_LEN, 0},
	{0xe5, 0xEB, BYTE_BYTE_LEN, 0},
	{0xe6, 0x01, BYTE_BYTE_LEN, 0},
	{0xe7, 0x7E, BYTE_BYTE_LEN, 0},
	{0xe8, 0x00, BYTE_BYTE_LEN, 0},
	{0xe9, 0x95, BYTE_BYTE_LEN, 0},
	{0xea, 0x00, BYTE_BYTE_LEN, 0},
	{0xeb, 0xF1, BYTE_BYTE_LEN, 0},
	{0xec, 0x00, BYTE_BYTE_LEN, 0},
	{0xed, 0xdd, BYTE_BYTE_LEN, 0},
	{0xee, 0x00, BYTE_BYTE_LEN, 0},
	{0xef, 0x00, BYTE_BYTE_LEN, 0},
	           
	{0xf0, 0x00, BYTE_BYTE_LEN, 0},
	{0xf1, 0x34, BYTE_BYTE_LEN, 0},
	{0xf2, 0x00, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// 10 Page
	///////////////////////////////////////////
	{0x03, 0x10, BYTE_BYTE_LEN, 0},
	{0xe0, 0xff, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xe1, 0x3f, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xe2, 0xff, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xe3, 0xff, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xe4, 0xf7, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xe5, 0x79, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xe6, 0xce, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xe7, 0x1f, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xe8, 0x5f, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xe9, 0x00, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xea, 0x00, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xeb, 0x00, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xec, 0x00, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xed, 0x00, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xf0, 0x3f, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xf1, 0x00, BYTE_BYTE_LEN, 0}, // don't touch update
	{0xf2, 0x40, BYTE_BYTE_LEN, 0}, // don't touch update
	
	{0x10, 0x03, BYTE_BYTE_LEN, 0}, //YUV422-YUYV
	{0x12, 0x00, BYTE_BYTE_LEN, 0}, //0x30, BYTE_BYTE_LEN, 0}, //0220 test dyoffset on//0x10, BYTE_BYTE_LEN, 0}, //Y,DY offset Enb
	{0x13, 0x02, BYTE_BYTE_LEN, 0}, //Bright2, Contrast Enb
	{0x20, 0x80, BYTE_BYTE_LEN, 0},
	{0x48, 0x90, BYTE_BYTE_LEN, 0}, //jktest 0208//0x80, BYTE_BYTE_LEN, 0},// contrast
	{0x50, 0x90, BYTE_BYTE_LEN, 0}, //0220 add, AGbgt//
	{0x60, 0x03, BYTE_BYTE_LEN, 0}, //Sat, Trans Enb
	{0x61, 0x80, BYTE_BYTE_LEN, 0},
	{0x62, 0x80, BYTE_BYTE_LEN, 0},
	//Desat - Chroma              
	{0x70,0x0c, BYTE_BYTE_LEN, 0},            
	{0x71,0x00, BYTE_BYTE_LEN, 0}, //jktest 0216 for saturation//0x01, BYTE_BYTE_LEN, 0},            
	{0x72,0xaf, BYTE_BYTE_LEN, 0}, //jktest 0216 for saturation0x66, BYTE_BYTE_LEN, 0},            
	{0x73,0xcc, BYTE_BYTE_LEN, 0},            
	{0x74,0x88, BYTE_BYTE_LEN, 0},            
	{0x75,0x05, BYTE_BYTE_LEN, 0},            
	{0x76,0x40, BYTE_BYTE_LEN, 0}, //jktest 0216 for saturation//0x33, BYTE_BYTE_LEN, 0},            
	{0x77,0x49, BYTE_BYTE_LEN, 0},            
	
	{0x78,0x99, BYTE_BYTE_LEN, 0},            
	{0x79,0x56, BYTE_BYTE_LEN, 0},            
	{0x7a,0x66, BYTE_BYTE_LEN, 0},            
	{0x7b,0x46, BYTE_BYTE_LEN, 0},            
	{0x7c,0x66, BYTE_BYTE_LEN, 0},            
	{0x7d,0x0e, BYTE_BYTE_LEN, 0},            
	{0x7e,0x1e, BYTE_BYTE_LEN, 0},            
	{0x7f,0x3c, BYTE_BYTE_LEN, 0},  
	
	///////////////////////////////////////////
	// 11 page D-LPF
	///////////////////////////////////////////  
	//DLPF
	{0x03, 0x11, BYTE_BYTE_LEN, 0},
	{0xf0, 0x00, BYTE_BYTE_LEN, 0},
	{0xf1, 0x00, BYTE_BYTE_LEN, 0},
{0xf2, 0x6e, BYTE_BYTE_LEN, 0}, //0x88, BYTE_BYTE_LEN, 0}, // ag에 따라 설정.
{0xf3, 0x64, BYTE_BYTE_LEN, 0}, //0x80, BYTE_BYTE_LEN, 0},
	{0xf4, 0xfe, BYTE_BYTE_LEN, 0},
	{0xf5, 0xfd, BYTE_BYTE_LEN, 0},
	{0xf6, 0x00, BYTE_BYTE_LEN, 0},
	{0xf7, 0x00, BYTE_BYTE_LEN, 0},
	
	// STEVE Luminanace level setting (DMA에 추가해야함)
	{0x32, 0x8b, BYTE_BYTE_LEN, 0},
	{0x33, 0x54, BYTE_BYTE_LEN, 0},
	{0x34, 0x2c, BYTE_BYTE_LEN, 0},
	{0x35, 0x29, BYTE_BYTE_LEN, 0},
	{0x36, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x38, 0x17, BYTE_BYTE_LEN, 0},

	///////////////////////////////////////////
	// 12 page DPC / GBGR /LensDebulr
	/////////////////////////////////////////// 			
	{0x03, 0x12, BYTE_BYTE_LEN, 0}, 					 
	{0x10, 0x57, BYTE_BYTE_LEN, 0},
	{0x11, 0x29, BYTE_BYTE_LEN, 0},
	{0x12, 0x08, BYTE_BYTE_LEN, 0},
	{0x13, 0x00, BYTE_BYTE_LEN, 0},
	{0x14, 0x00, BYTE_BYTE_LEN, 0},
	{0x15, 0x00, BYTE_BYTE_LEN, 0},
	{0x16, 0x00, BYTE_BYTE_LEN, 0},
	{0x17, 0x00, BYTE_BYTE_LEN, 0},
					
	{0x18, 0xc8, BYTE_BYTE_LEN, 0},
	{0x19, 0x7d, BYTE_BYTE_LEN, 0},
	{0x1a, 0x32, BYTE_BYTE_LEN, 0},
	{0x1b, 0x02, BYTE_BYTE_LEN, 0},
	{0x1c, 0x77, BYTE_BYTE_LEN, 0},
	{0x1d, 0x1e, BYTE_BYTE_LEN, 0},
	{0x1e, 0x28, BYTE_BYTE_LEN, 0},
	{0x1f, 0x28, BYTE_BYTE_LEN, 0},
	           
	{0x20, 0x14, BYTE_BYTE_LEN, 0},
	{0x21, 0x11, BYTE_BYTE_LEN, 0},
	{0x22, 0x0f, BYTE_BYTE_LEN, 0},
	{0x23, 0x16, BYTE_BYTE_LEN, 0},
	{0x24, 0x15, BYTE_BYTE_LEN, 0},
	{0x25, 0x14, BYTE_BYTE_LEN, 0},
	{0x26, 0x28, BYTE_BYTE_LEN, 0},
	{0x27, 0x3c, BYTE_BYTE_LEN, 0},
	           
	{0x28, 0x78, BYTE_BYTE_LEN, 0},
	{0x29, 0xa0, BYTE_BYTE_LEN, 0},
	{0x2a, 0xb4, BYTE_BYTE_LEN, 0},
	{0x2b, 0x08, BYTE_BYTE_LEN, 0},//DPC threshold
	{0x2c, 0x08, BYTE_BYTE_LEN, 0},//DPC threshold
	{0x2d, 0x08, BYTE_BYTE_LEN, 0},//DPC threshold
	{0x2e, 0x06, BYTE_BYTE_LEN, 0},//DPC threshold
	{0x2f, 0x64, BYTE_BYTE_LEN, 0},
	           
	{0x30, 0x64, BYTE_BYTE_LEN, 0},
	{0x31, 0x64, BYTE_BYTE_LEN, 0},
	{0x32, 0x64, BYTE_BYTE_LEN, 0},  
	//GBGR			
	{0x33, 0xaa, BYTE_BYTE_LEN, 0},
	{0x34, 0x96, BYTE_BYTE_LEN, 0},
	{0x35, 0x04, BYTE_BYTE_LEN, 0},
	{0x36, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	           
	{0x38, 0x04, BYTE_BYTE_LEN, 0},
	{0x39, 0x04, BYTE_BYTE_LEN, 0},
	{0x3a, 0x03, BYTE_BYTE_LEN, 0},
	{0x3b, 0x0c, BYTE_BYTE_LEN, 0},
	{0x3C, 0x00, BYTE_BYTE_LEN, 0},
	{0x3D, 0x00, BYTE_BYTE_LEN, 0},
	{0x3E, 0x00, BYTE_BYTE_LEN, 0},
	{0x3F, 0x00, BYTE_BYTE_LEN, 0},
	
	{0x40, 0x33, BYTE_BYTE_LEN, 0},
	{0xE0, 0x08, BYTE_BYTE_LEN, 0}, //0216 by KJH//0x0c, BYTE_BYTE_LEN, 0},
	{0xE1, 0x58, BYTE_BYTE_LEN, 0},
	{0xE3, 0x00, BYTE_BYTE_LEN, 0}, //0216 by KJH//0x0c, BYTE_BYTE_LEN, 0},
	{0xEC, 0x10, BYTE_BYTE_LEN, 0},
	{0xEE, 0x03, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// 13 page YC2D LPF
	///////////////////////////////////////////
	{0x03, 0x13, BYTE_BYTE_LEN, 0},
	
	{0x10, 0x33, BYTE_BYTE_LEN, 0}, //Don't touch
	{0xa0, 0x0f, BYTE_BYTE_LEN, 0}, //Don't touch
	
	{0xe1, 0x07, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// 14 page Sharpness
	///////////////////////////////////////////
	{0x03, 0x14, BYTE_BYTE_LEN, 0},
	
	{0x10, 0x27, BYTE_BYTE_LEN, 0}, //Don't touch
	{0x11, 0x02, BYTE_BYTE_LEN, 0}, //Don't touch
	{0x12, 0x40, BYTE_BYTE_LEN, 0}, //Don't touch
	{0x20, 0x82, BYTE_BYTE_LEN, 0}, //Don't touch
	{0x30, 0x82, BYTE_BYTE_LEN, 0}, //Don't touch
	{0x40, 0x84, BYTE_BYTE_LEN, 0}, //Don't touch
	{0x50, 0x84, BYTE_BYTE_LEN, 0}, //Don't touch
	
	///////////////////////////////////////////
	// 15 Page LSC off
	///////////////////////////////////////////
	{0x03, 0x15, BYTE_BYTE_LEN, 0},
	{0x10, 0x82, BYTE_BYTE_LEN, 0}, //lsc off

	{0x03, 0xFE, BYTE_BYTE_LEN, 0}, //need to merge for solving preview rainbow problem
	{0xFE, 0x32, BYTE_BYTE_LEN, 10},//jktest 0210 400khz 50ms//0x0a, BYTE_BYTE_LEN, 0}, //delay 10ms//0x32, BYTE_BYTE_LEN, 0}, //delay 50ms
	
	///////////////////////////////////////////
	// 7 Page LSC data (STEVE 75p)
	///////////////////////////////////////////
	{0x03, 0x07, BYTE_BYTE_LEN, 0},
	{0x12, 0x04, BYTE_BYTE_LEN, 0},//07
	{0x34, 0x00, BYTE_BYTE_LEN, 0},
	{0x35, 0x00, BYTE_BYTE_LEN, 0},
	{0x13, 0x85, BYTE_BYTE_LEN, 0},
	{0x13, 0x05, BYTE_BYTE_LEN, 0},
	
	//================ LSC set start
	//start
	{0x37, 0x4f, BYTE_BYTE_LEN, 0},
	{0x37, 0x47, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x3c, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x3f, BYTE_BYTE_LEN, 0},
	{0x37, 0x44, BYTE_BYTE_LEN, 0},
	{0x37, 0x4b, BYTE_BYTE_LEN, 0},
	{0x37, 0x51, BYTE_BYTE_LEN, 0},
	{0x37, 0x5b, BYTE_BYTE_LEN, 0},
	{0x37, 0x43, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x28, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x48, BYTE_BYTE_LEN, 0},
	{0x37, 0x4f, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x30, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x47, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x28, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x3a, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x3c, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x0f, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x08, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x01, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x01, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x1c, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x28, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x45, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x44, BYTE_BYTE_LEN, 0},
	{0x37, 0x4a, BYTE_BYTE_LEN, 0},
	{0x37, 0x43, BYTE_BYTE_LEN, 0},
	{0x37, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x3c, BYTE_BYTE_LEN, 0},
	{0x37, 0x42, BYTE_BYTE_LEN, 0},
	{0x37, 0x49, BYTE_BYTE_LEN, 0},
	{0x37, 0x51, BYTE_BYTE_LEN, 0},
	{0x37, 0x56, BYTE_BYTE_LEN, 0},
	{0x37, 0x4e, BYTE_BYTE_LEN, 0},
	{0x37, 0x49, BYTE_BYTE_LEN, 0},
	{0x37, 0x42, BYTE_BYTE_LEN, 0},
	{0x37, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x3a, BYTE_BYTE_LEN, 0},
	{0x37, 0x3f, BYTE_BYTE_LEN, 0},
	{0x37, 0x45, BYTE_BYTE_LEN, 0},
	{0x37, 0x4c, BYTE_BYTE_LEN, 0},
	{0x37, 0x53, BYTE_BYTE_LEN, 0},
	{0x37, 0x59, BYTE_BYTE_LEN, 0},
	{0x37, 0x63, BYTE_BYTE_LEN, 0},
	{0x37, 0x4c, BYTE_BYTE_LEN, 0},
	{0x37, 0x46, BYTE_BYTE_LEN, 0},
	{0x37, 0x40, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x28, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x3a, BYTE_BYTE_LEN, 0},
	{0x37, 0x42, BYTE_BYTE_LEN, 0},
	{0x37, 0x4a, BYTE_BYTE_LEN, 0},
	{0x37, 0x50, BYTE_BYTE_LEN, 0},
	{0x37, 0x57, BYTE_BYTE_LEN, 0},
	{0x37, 0x43, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x1c, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x49, BYTE_BYTE_LEN, 0},
	{0x37, 0x50, BYTE_BYTE_LEN, 0},
	{0x37, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x10, BYTE_BYTE_LEN, 0},
	{0x37, 0x0f, BYTE_BYTE_LEN, 0},
	{0x37, 0x10, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x43, BYTE_BYTE_LEN, 0},
	{0x37, 0x4a, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x0f, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x08, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x08, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x1c, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x3c, BYTE_BYTE_LEN, 0},
	{0x37, 0x44, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x0f, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x40, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x28, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x28, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x0f, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x01, BYTE_BYTE_LEN, 0},
	{0x37, 0x01, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x1c, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x08, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x42, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x0f, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x3f, BYTE_BYTE_LEN, 0},
	{0x37, 0x47, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x1c, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x46, BYTE_BYTE_LEN, 0},
	{0x37, 0x4d, BYTE_BYTE_LEN, 0},
	{0x37, 0x47, BYTE_BYTE_LEN, 0},
	{0x37, 0x42, BYTE_BYTE_LEN, 0},
	{0x37, 0x3c, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x46, BYTE_BYTE_LEN, 0},
	{0x37, 0x4e, BYTE_BYTE_LEN, 0},
	{0x37, 0x54, BYTE_BYTE_LEN, 0},
	{0x37, 0x4f, BYTE_BYTE_LEN, 0},
	{0x37, 0x4a, BYTE_BYTE_LEN, 0},
	{0x37, 0x45, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x3f, BYTE_BYTE_LEN, 0},
	{0x37, 0x47, BYTE_BYTE_LEN, 0},
	{0x37, 0x4e, BYTE_BYTE_LEN, 0},
	{0x37, 0x55, BYTE_BYTE_LEN, 0},
	{0x37, 0x5d, BYTE_BYTE_LEN, 0},
	{0x37, 0x4a, BYTE_BYTE_LEN, 0},
	{0x37, 0x43, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x30, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x42, BYTE_BYTE_LEN, 0},
	{0x37, 0x48, BYTE_BYTE_LEN, 0},
	{0x37, 0x52, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x3a, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x1f, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x3a, BYTE_BYTE_LEN, 0},
	{0x37, 0x3f, BYTE_BYTE_LEN, 0},
	{0x37, 0x45, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x1c, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x1f, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x10, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x10, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x1c, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x10, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x08, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x08, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x1f, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x10, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x30, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x10, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x30, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x01, BYTE_BYTE_LEN, 0},
	{0x37, 0x01, BYTE_BYTE_LEN, 0},
	{0x37, 0x01, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x0f, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x08, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x0f, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x3a, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x3a, BYTE_BYTE_LEN, 0},
	{0x37, 0x3f, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x1c, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x3f, BYTE_BYTE_LEN, 0},
	{0x37, 0x43, BYTE_BYTE_LEN, 0},
	{0x37, 0x3a, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x1f, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x30, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x3c, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x48, BYTE_BYTE_LEN, 0},
	{0x37, 0x4f, BYTE_BYTE_LEN, 0},
	{0x37, 0x47, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x3c, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x3f, BYTE_BYTE_LEN, 0},
	{0x37, 0x44, BYTE_BYTE_LEN, 0},
	{0x37, 0x4b, BYTE_BYTE_LEN, 0},
	{0x37, 0x51, BYTE_BYTE_LEN, 0},
	{0x37, 0x5b, BYTE_BYTE_LEN, 0},
	{0x37, 0x43, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x28, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x48, BYTE_BYTE_LEN, 0},
	{0x37, 0x4f, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x30, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x47, BYTE_BYTE_LEN, 0},
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x28, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x3a, BYTE_BYTE_LEN, 0},
	{0x37, 0x41, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x3c, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x0f, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x08, BYTE_BYTE_LEN, 0},
	{0x37, 0x0e, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x39, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x37, 0x06, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x2b, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x01, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x01, BYTE_BYTE_LEN, 0},
	{0x37, 0x03, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x0c, BYTE_BYTE_LEN, 0},
	{0x37, 0x13, BYTE_BYTE_LEN, 0},
	{0x37, 0x1c, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x28, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x05, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x04, BYTE_BYTE_LEN, 0},
	{0x37, 0x07, BYTE_BYTE_LEN, 0},
	{0x37, 0x0b, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x20, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x31, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x16, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x09, BYTE_BYTE_LEN, 0},
	{0x37, 0x0a, BYTE_BYTE_LEN, 0},
	{0x37, 0x0d, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x17, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x37, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x18, BYTE_BYTE_LEN, 0},
	{0x37, 0x14, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x11, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0},
	{0x37, 0x15, BYTE_BYTE_LEN, 0},
	{0x37, 0x19, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x36, BYTE_BYTE_LEN, 0},
	{0x37, 0x3e, BYTE_BYTE_LEN, 0},
	{0x37, 0x45, BYTE_BYTE_LEN, 0},
	{0x37, 0x3b, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x32, BYTE_BYTE_LEN, 0},
	{0x37, 0x2c, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x21, BYTE_BYTE_LEN, 0},
	{0x37, 0x1d, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x1a, BYTE_BYTE_LEN, 0},
	{0x37, 0x1b, BYTE_BYTE_LEN, 0},
	{0x37, 0x1e, BYTE_BYTE_LEN, 0},
	{0x37, 0x22, BYTE_BYTE_LEN, 0},
	{0x37, 0x27, BYTE_BYTE_LEN, 0},
	{0x37, 0x2e, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x44, BYTE_BYTE_LEN, 0},
	{0x37, 0x4a, BYTE_BYTE_LEN, 0},
	{0x37, 0x43, BYTE_BYTE_LEN, 0},
	{0x37, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0},
	{0x37, 0x33, BYTE_BYTE_LEN, 0},
	{0x37, 0x2d, BYTE_BYTE_LEN, 0},
	{0x37, 0x29, BYTE_BYTE_LEN, 0},
	{0x37, 0x25, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x23, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0},
	{0x37, 0x26, BYTE_BYTE_LEN, 0},
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},
	{0x37, 0x2f, BYTE_BYTE_LEN, 0},
	{0x37, 0x35, BYTE_BYTE_LEN, 0},
	{0x37, 0x3c, BYTE_BYTE_LEN, 0},
	{0x37, 0x42, BYTE_BYTE_LEN, 0},
	{0x37, 0x49, BYTE_BYTE_LEN, 0},
	{0x37, 0x51, BYTE_BYTE_LEN, 0},


	//0224


	//END
	
	//================ LSC set end
	
	{0x12, 0x00, BYTE_BYTE_LEN, 0},
	{0x13, 0x00, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x15, BYTE_BYTE_LEN, 0},
	{0x10, 0x83, BYTE_BYTE_LEN, 0}, // LSC ON
	
	///////////////////////////////////////////
	// 16 Page CMC
	///////////////////////////////////////////
	{0x03, 0x16, BYTE_BYTE_LEN, 0},
	
	{0x10, 0x0f, BYTE_BYTE_LEN, 0}, //cmc
	{0x17, 0x2f, BYTE_BYTE_LEN, 0}, //CMC SIGN
	{0x60, 0x7f, BYTE_BYTE_LEN, 0}, //jktest 0202//r5 on//0x3f, BYTE_BYTE_LEN, 0}, //mcmc steve MCMC ON 20111221 
	
//automatic saturation 
	{0x8a,0x7f, BYTE_BYTE_LEN, 0}, //jktest 0216 for Dsat//0x6f, BYTE_BYTE_LEN, 0},
	{0x8b,0x7f, BYTE_BYTE_LEN, 0}, //jktest 0216 for Dsat//0x75, BYTE_BYTE_LEN, 0},
	{0x8c,0x7f, BYTE_BYTE_LEN, 0}, //jktest 0216 for Dsat//0x7b, BYTE_BYTE_LEN, 0},
	{0x8d,0x7f, BYTE_BYTE_LEN, 0},
	{0x8e,0x7f, BYTE_BYTE_LEN, 0},
	{0x8f,0x7f, BYTE_BYTE_LEN, 0},
	{0x90,0x7f, BYTE_BYTE_LEN, 0},
	{0x91,0x7f, BYTE_BYTE_LEN, 0},

	{0x92,0x7f, BYTE_BYTE_LEN, 0},
	{0x93,0x7f, BYTE_BYTE_LEN, 0},
	{0x94,0x7f, BYTE_BYTE_LEN, 0},
	{0x95,0x7f, BYTE_BYTE_LEN, 0},
	{0x96,0x7f, BYTE_BYTE_LEN, 0},
	{0x97,0x7f, BYTE_BYTE_LEN, 0},
	{0x98,0x7b, BYTE_BYTE_LEN, 0},
	{0x99,0x74, BYTE_BYTE_LEN, 0},

	{0x9a,0x6e, BYTE_BYTE_LEN, 0},
	
	//Dgain
	{0xa0, 0x81, BYTE_BYTE_LEN, 0}, //Manual WB gain enable
	{0xa1, 0x00, BYTE_BYTE_LEN, 0},
	
	{0xa2, 0x68, BYTE_BYTE_LEN, 0}, //R_dgain_byr
	{0xa3, 0x70, BYTE_BYTE_LEN, 0}, //B_dgain_byr
	
	{0xa6, 0xa0, BYTE_BYTE_LEN, 0}, //r max
	{0xa8, 0xa0, BYTE_BYTE_LEN, 0}, //b max
			// Pre WB gain setting(after AWB setting)
	{0xF0, 0x09, BYTE_BYTE_LEN, 0},//Pre WB gain enable Gain resolution_2x
	{0xF1, 0x80, BYTE_BYTE_LEN, 0},
	{0xF2, 0x80, BYTE_BYTE_LEN, 0},
	{0xF3, 0x80, BYTE_BYTE_LEN, 0},
	{0xF4, 0x80, BYTE_BYTE_LEN, 0}, 
	///////////////////////////////////////////
	// 17 Page Gamma
	///////////////////////////////////////////
	{0x03, 0x17, BYTE_BYTE_LEN, 0},
	{0x10, 0x01, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// 18 Page Histogram
	///////////////////////////////////////////
	{0x03, 0x18, BYTE_BYTE_LEN, 0},
	{0x10, 0x00, BYTE_BYTE_LEN, 0},
	{0xc0, 0x01, BYTE_BYTE_LEN, 0},


  	{0xC4, 0x42, BYTE_BYTE_LEN, 0}, //FLK200 
  	{0xC5, 0x37, BYTE_BYTE_LEN, 0}, //FLK240 
	
	///////////////////////////////////////////
	// 20 Page AE
	/////////////////////////////////////////// 			
	{0x03, 0x20, BYTE_BYTE_LEN, 0},
	{0x10, 0x9f, BYTE_BYTE_LEN, 0}, //M 50hz//0xdf, BYTE_BYTE_LEN, 0},//auto 50hz flicker select
	{0x12, 0x2d, BYTE_BYTE_LEN, 0}, // STEVE Dgain off
	{0x17, 0xa0, BYTE_BYTE_LEN, 0},
	{0x1f, 0x1f, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x20, BYTE_BYTE_LEN, 0}, //Page 20
{0x20, 0x00, BYTE_BYTE_LEN, 0}, //EXP Normal 33.33 fps
{0x21, 0x0a, BYTE_BYTE_LEN, 0},
{0x22, 0xfc, BYTE_BYTE_LEN, 0},
{0x23, 0x38, BYTE_BYTE_LEN, 0},
{0x24, 0x00, BYTE_BYTE_LEN, 0}, //EXP Max 8.33 fps
{0x25, 0x2b, BYTE_BYTE_LEN, 0},
{0x26, 0xf0, BYTE_BYTE_LEN, 0},
{0x27, 0xe0, BYTE_BYTE_LEN, 0},
	{0x28, 0x00, BYTE_BYTE_LEN, 0}, //EXPMin 13201.32 fps
	{0x29, 0x07, BYTE_BYTE_LEN, 0}, 
	{0x2a, 0x1a, BYTE_BYTE_LEN, 0}, 
	{0x30, 0x03, BYTE_BYTE_LEN, 0}, //EXP100 
	{0x31, 0xa9, BYTE_BYTE_LEN, 0}, 
	{0x32, 0x68, BYTE_BYTE_LEN, 0}, 
	{0x33, 0x03, BYTE_BYTE_LEN, 0}, //EXP120 
	{0x34, 0x0d, BYTE_BYTE_LEN, 0}, 
	{0x35, 0x2c, BYTE_BYTE_LEN, 0}, 
	{0x36, 0x00, BYTE_BYTE_LEN, 0}, //EXP Unit 
	{0x37, 0x07, BYTE_BYTE_LEN, 0}, 
	{0x38, 0x1a, BYTE_BYTE_LEN, 0}, 
	
	{0x40, 0x00, BYTE_BYTE_LEN, 0}, //exp 120
	{0x41, 0x03, BYTE_BYTE_LEN, 0}, 
	{0x42, 0xa9, BYTE_BYTE_LEN, 0}, 
	
{0x43, 0x04, BYTE_BYTE_LEN, 0}, //pga time th

{0x51, 0xb0, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //pga_max_total
	{0x52, 0x28, BYTE_BYTE_LEN, 0}, //pga_min_total
	
	{0x71, 0x80, BYTE_BYTE_LEN, 0}, //DG MAX 
	{0x72, 0x80, BYTE_BYTE_LEN, 0}, //DG MIN
	
  	{0x80, 0x34, BYTE_BYTE_LEN, 0}, //ae target
	
	///////////////////////////////////////////
	// Preview Setting
	///////////////////////////////////////////
	
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x10, 0x13, BYTE_BYTE_LEN, 0}, //Pre2
	
	{0x20, 0x00, BYTE_BYTE_LEN, 0},
	{0x21, 0x01, BYTE_BYTE_LEN, 0}, //preview row start set.
	
	{0x03, 0x15, BYTE_BYTE_LEN, 0},  //Shading
	{0x10, 0x81, BYTE_BYTE_LEN, 0},  //
	{0x20, 0x04, BYTE_BYTE_LEN, 0},  //Shading Width 2048
	{0x21, 0x00, BYTE_BYTE_LEN, 0},
	{0x22, 0x03, BYTE_BYTE_LEN, 0},  //Shading Height 768
	{0x23, 0x00, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x19, BYTE_BYTE_LEN, 0}, 
	{0x10, 0x87, BYTE_BYTE_LEN, 0},//MODE_ZOOM			 
	{0x11, 0x00, BYTE_BYTE_LEN, 0},//MODE_ZOOM2 		 
	{0x12, 0x06, BYTE_BYTE_LEN, 0},//ZOOM_CONFIG		 
	{0x13, 0x01, BYTE_BYTE_LEN, 0},//Test Setting		 
	{0x20, 0x02, BYTE_BYTE_LEN, 0},//ZOOM_DST_WIDTH_H	 
	{0x21, 0x80, BYTE_BYTE_LEN, 0},//ZOOM_DST_WIDTH_L	 
	{0x22, 0x01, BYTE_BYTE_LEN, 0},//ZOOM_DST_HEIGHT_H	 
	{0x23, 0xe0, BYTE_BYTE_LEN, 0},//ZOOM_DST_HEIGHT_L	 
	{0x24, 0x00, BYTE_BYTE_LEN, 0},//ZOOM_WIN_STX_H 	 
	{0x25, 0x01, BYTE_BYTE_LEN, 0},//ZOOM_WIN_STX_L 	 // STEVE00 3-) 1
	{0x26, 0x00, BYTE_BYTE_LEN, 0},//ZOOM_WIN_STY_H 	 
	{0x27, 0x01, BYTE_BYTE_LEN, 0},//ZOOM_WIN_STY_L 	 
	{0x28, 0x02, BYTE_BYTE_LEN, 0},//ZOOM_WIN_ENX_H 	 
	{0x29, 0x81, BYTE_BYTE_LEN, 0},//ZOOM_WIN_ENX_L 	 //STEVE00 83 -) 81
	{0x2a, 0x01, BYTE_BYTE_LEN, 0},//ZOOM_WIN_ENY_H 	 
	{0x2b, 0xe1, BYTE_BYTE_LEN, 0},//ZOOM_WIN_ENY_L 	 
	{0x2c, 0x0c, BYTE_BYTE_LEN, 0},//ZOOM_VER_STEP_H	 
	{0x2d, 0xcc, BYTE_BYTE_LEN, 0},//ZOOM_VER_STEP_L	 
	{0x2e, 0x0c, BYTE_BYTE_LEN, 0},//ZOOM_HOR_STEP_H	 
	{0x2f, 0xcc, BYTE_BYTE_LEN, 0},//ZOOM_HOR_STEP_L	 
	{0x30, 0x42, BYTE_BYTE_LEN, 0},//ZOOM_FIFO_DELAY	 

	
	///////////////////////////////////////////
	// 30 Page DMA address set
	///////////////////////////////////////////
	{0x03, 0x30, BYTE_BYTE_LEN, 0}, //DMA
	{0x7c, 0x2c, BYTE_BYTE_LEN, 0}, //Extra str
	{0x7d, 0xce, BYTE_BYTE_LEN, 0},
	{0x7e, 0x2c, BYTE_BYTE_LEN, 0}, //Extra end
	{0x7f, 0xd1, BYTE_BYTE_LEN, 0},
	{0x80, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor str
	{0x81, 0x70, BYTE_BYTE_LEN, 0}, 
	{0x82, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor end 
	{0x83, 0x73, BYTE_BYTE_LEN, 0},  
	{0x84, 0x21, BYTE_BYTE_LEN, 0}, //Indoor str 
	{0x85, 0xa6, BYTE_BYTE_LEN, 0},  
	{0x86, 0x21, BYTE_BYTE_LEN, 0}, //Indoor end 
	{0x87, 0xa9, BYTE_BYTE_LEN, 0}, 
	{0x88, 0x27, BYTE_BYTE_LEN, 0}, //Dark1 str
	{0x89, 0x3a, BYTE_BYTE_LEN, 0},  
	{0x8a, 0x27, BYTE_BYTE_LEN, 0}, //Dark1 end  
	{0x8b, 0x3d, BYTE_BYTE_LEN, 0}, 
	{0x8c, 0x2a, BYTE_BYTE_LEN, 0}, //Dark2 str 
	{0x8d, 0x04, BYTE_BYTE_LEN, 0},  
	{0x8e, 0x2a, BYTE_BYTE_LEN, 0}, //Dark2 end 
	{0x8f, 0x07, BYTE_BYTE_LEN, 0},
		   
	{0x03, 0xC0, BYTE_BYTE_LEN, 0},
	{0x2F, 0xf0, BYTE_BYTE_LEN, 0}, //DMA busy flag check
	{0x31, 0x20, BYTE_BYTE_LEN, 0}, //Delay before DMA write_setting
	{0x33, 0x20, BYTE_BYTE_LEN, 0}, //DMA full stuck mode
	{0x32, 0x01, BYTE_BYTE_LEN, 0}, //DMA on first 
	
	{0x03, 0xC0, BYTE_BYTE_LEN, 0},
	{0x2F, 0xf0, BYTE_BYTE_LEN, 0}, //DMA busy flag check
	{0x31, 0x20, BYTE_BYTE_LEN, 0}, //Delay before DMA write_setting
	{0x33, 0x20, BYTE_BYTE_LEN, 0},
	{0x32, 0x01, BYTE_BYTE_LEN, 0}, //DMA on second
	
	
	{0x03, 0xC0, BYTE_BYTE_LEN, 0},
	{0xe1, 0x80, BYTE_BYTE_LEN, 0},// PCON Enable option
	{0xe1, 0x80, BYTE_BYTE_LEN, 0},// PCON MODE ON
	
	//MCU Set
	{0x03, 0x30, BYTE_BYTE_LEN, 0},
	{0x12, 0x00, BYTE_BYTE_LEN, 0},
	{0x20, 0x08, BYTE_BYTE_LEN, 0},
	{0x50, 0x00, BYTE_BYTE_LEN, 0},
	{0xE0, 0x02, BYTE_BYTE_LEN, 0},
	{0xF0, 0x00, BYTE_BYTE_LEN, 0},
	{0x11, 0x05, BYTE_BYTE_LEN, 0},// M2i Hold
	{0x03, 0xc0, BYTE_BYTE_LEN, 0},
	{0xe4, 0x14, BYTE_BYTE_LEN, 0},//jktest 0210 400khz//0xa0, BYTE_BYTE_LEN, 0}, //0x14, BYTE_BYTE_LEN, 0},//0x64, BYTE_BYTE_LEN, 0},//I2C 400k//0xa0, BYTE_BYTE_LEN, 0}, //I2C 100k//delay 64 -) a0
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x01, 0xF0, BYTE_BYTE_LEN, 0}, // sleep off
	
	///////////////////////////////////////////
	// CD Page Adaptive Mode(Color ratio)
	///////////////////////////////////////////
	{0x03, 0xCD, BYTE_BYTE_LEN, 0},
	{0x47, 0x00, BYTE_BYTE_LEN, 0},
	{0x12, 0x80, BYTE_BYTE_LEN, 0},
	{0x13, 0x80, BYTE_BYTE_LEN, 0}, //Ratio WB R gain min
	{0x14, 0x90, BYTE_BYTE_LEN, 0}, //Ratio WB R gain max
	{0x15, 0x80, BYTE_BYTE_LEN, 0}, //Ratio WB B gain min
	{0x16, 0x90, BYTE_BYTE_LEN, 0}, //Ratio WB B gain max
	{0x10, 0x38, BYTE_BYTE_LEN, 0}, // STEVE b9 -) 38 Disable
	
	///////////////////////////////////////////
	// 1F Page SSD
	///////////////////////////////////////////
	{0x03, 0x1f, BYTE_BYTE_LEN, 0}, //1F page
	{0x11, 0x00, BYTE_BYTE_LEN, 0}, //bit[5:4]: debug mode
	{0x12, 0x60, BYTE_BYTE_LEN, 0},
	{0x13, 0x14, BYTE_BYTE_LEN, 0},
	{0x14, 0x10, BYTE_BYTE_LEN, 0},
	{0x15, 0x00, BYTE_BYTE_LEN, 0},
	{0x20, 0x18, BYTE_BYTE_LEN, 0}, //ssd_x_start_pos
	{0x21, 0x14, BYTE_BYTE_LEN, 0}, //ssd_y_start_pos
	{0x22, 0x8C, BYTE_BYTE_LEN, 0}, //ssd_blk_width
	{0x23, 0x9c, BYTE_BYTE_LEN, 0}, //ssd_blk_height 60 -) 9c
	{0x28, 0x18, BYTE_BYTE_LEN, 0},
	{0x29, 0x02, BYTE_BYTE_LEN, 0},
	{0x3B, 0x18, BYTE_BYTE_LEN, 0},
	{0x3C, 0x8C, BYTE_BYTE_LEN, 0},
	{0x10, 0x19, BYTE_BYTE_LEN, 0}, //SSD enable
	
	///////////////////////////////////////////
	
	///////////////////////////////////////////
	// C4 Page MCU AE
	///////////////////////////////////////////
	{0x03, 0xc4, BYTE_BYTE_LEN, 0},
	{0x11, 0x30, BYTE_BYTE_LEN, 0}, // ae speed B[7:6] 0 (SLOW) ~ 3 (FAST), 0x70 - 0x30   
	{0x12, 0x10, BYTE_BYTE_LEN, 0}, 		
	{0x19, 0x40, BYTE_BYTE_LEN, 0}, //0x30, BYTE_BYTE_LEN, 0}, //0x50, BYTE_BYTE_LEN, 0}, //17th//0x30, BYTE_BYTE_LEN, 0}, // band0 gain 40fps 0x2d
	{0x1a, 0x4d, BYTE_BYTE_LEN, 0}, //0x38, BYTE_BYTE_LEN, 0}, //0x58, BYTE_BYTE_LEN, 0}, //17th//0x38, BYTE_BYTE_LEN, 0}, // band1 gain 20fps STEVE 
	{0x1b, 0x55, BYTE_BYTE_LEN, 0}, //0x4c, BYTE_BYTE_LEN, 0}, //0x60, BYTE_BYTE_LEN, 0}, //0x6c, BYTE_BYTE_LEN, 0}, //17th//0x4c, BYTE_BYTE_LEN, 0}, // band2 gain 12fps
	{0x1c, 0x04, BYTE_BYTE_LEN, 0}, 
	{0x1d, 0x80, BYTE_BYTE_LEN, 0},

{0x1e, 0x00, BYTE_BYTE_LEN, 0}, // band1 min exposure time	1/33.33s // correction point
{0x1f, 0x0a, BYTE_BYTE_LEN, 0},
{0x20, 0xfc, BYTE_BYTE_LEN, 0},
{0x21, 0x38, BYTE_BYTE_LEN, 0},

	{0x22, 0x00, BYTE_BYTE_LEN, 0}, // band2 min exposure time	1/20s
{0x23, 0x12, BYTE_BYTE_LEN, 0},
{0x24, 0x4f, BYTE_BYTE_LEN, 0},
{0x25, 0x08, BYTE_BYTE_LEN, 0},
	           
{0x26, 0x00, BYTE_BYTE_LEN, 0},// band3 min exposure time  1/12.5s
{0x27, 0x1d, BYTE_BYTE_LEN, 0},
{0x28, 0x4b, BYTE_BYTE_LEN, 0},
{0x29, 0x40, BYTE_BYTE_LEN, 0},
	
	{0x36, 0x22, BYTE_BYTE_LEN, 0}, // AE Yth 변경
	
	{0x66, 0x00, BYTE_BYTE_LEN, 0}, //jktest 0131//manual 50hz
	{0x69, 0x00, BYTE_BYTE_LEN, 0}, //jktest 0131//manual 50hz //auto off
	
	{0x03, 0x20, BYTE_BYTE_LEN, 0},
	{0x12, 0x2d, BYTE_BYTE_LEN, 0}, // STEVE 2d (AE digital gain OFF)
	
	///////////////////////////////////////////
	// c3 Page MCU AE Weight
	///////////////////////////////////////////
	{0x03, 0xc3, BYTE_BYTE_LEN, 0},
	{0x10, 0x00, BYTE_BYTE_LEN, 0},
	{0x38, 0xFF, BYTE_BYTE_LEN, 0},
	{0x39, 0xFF, BYTE_BYTE_LEN, 0},

	//AE_CenterWeighted
	{0x70, 0x00, BYTE_BYTE_LEN, 0},
	{0x71, 0x00, BYTE_BYTE_LEN, 0},
	{0x72, 0x00, BYTE_BYTE_LEN, 0},
	{0x73, 0x00, BYTE_BYTE_LEN, 0},
	{0x74, 0x00, BYTE_BYTE_LEN, 0},
	{0x75, 0x00, BYTE_BYTE_LEN, 0},
	{0x76, 0x00, BYTE_BYTE_LEN, 0},
	{0x77, 0x00, BYTE_BYTE_LEN, 0},
	{0x78, 0x00, BYTE_BYTE_LEN, 0},
	{0x79, 0x00, BYTE_BYTE_LEN, 0},
	{0x7A, 0x00, BYTE_BYTE_LEN, 0},
	{0x7B, 0x00, BYTE_BYTE_LEN, 0},
	{0x7C, 0x11, BYTE_BYTE_LEN, 0},
	{0x7D, 0x11, BYTE_BYTE_LEN, 0},
	{0x7E, 0x11, BYTE_BYTE_LEN, 0},
	{0x7F, 0x11, BYTE_BYTE_LEN, 0},
	{0x80, 0x11, BYTE_BYTE_LEN, 0},
	{0x81, 0x11, BYTE_BYTE_LEN, 0},
	{0x82, 0x11, BYTE_BYTE_LEN, 0},
	{0x83, 0x21, BYTE_BYTE_LEN, 0},
	{0x84, 0x44, BYTE_BYTE_LEN, 0},
	{0x85, 0x44, BYTE_BYTE_LEN, 0},
	{0x86, 0x12, BYTE_BYTE_LEN, 0},
	{0x87, 0x11, BYTE_BYTE_LEN, 0},
	{0x88, 0x11, BYTE_BYTE_LEN, 0},
	{0x89, 0x22, BYTE_BYTE_LEN, 0},
	{0x8A, 0x64, BYTE_BYTE_LEN, 0},
	{0x8B, 0x46, BYTE_BYTE_LEN, 0},
	{0x8C, 0x22, BYTE_BYTE_LEN, 0},
	{0x8D, 0x11, BYTE_BYTE_LEN, 0},
	{0x8E, 0x21, BYTE_BYTE_LEN, 0},
	{0x8F, 0x33, BYTE_BYTE_LEN, 0},
	{0x90, 0x64, BYTE_BYTE_LEN, 0},
	{0x91, 0x46, BYTE_BYTE_LEN, 0},
	{0x92, 0x33, BYTE_BYTE_LEN, 0},
	{0x93, 0x12, BYTE_BYTE_LEN, 0},
	{0x94, 0x21, BYTE_BYTE_LEN, 0},
	{0x95, 0x33, BYTE_BYTE_LEN, 0},
	{0x96, 0x44, BYTE_BYTE_LEN, 0},
	{0x97, 0x44, BYTE_BYTE_LEN, 0},
	{0x98, 0x33, BYTE_BYTE_LEN, 0},
	{0x99, 0x12, BYTE_BYTE_LEN, 0},
	{0x9A, 0x21, BYTE_BYTE_LEN, 0},
	{0x9B, 0x33, BYTE_BYTE_LEN, 0},
	{0x9C, 0x33, BYTE_BYTE_LEN, 0},
	{0x9D, 0x33, BYTE_BYTE_LEN, 0},
	{0x9E, 0x33, BYTE_BYTE_LEN, 0},
	{0x9F, 0x12, BYTE_BYTE_LEN, 0},
	{0xA0, 0x11, BYTE_BYTE_LEN, 0},
	{0xA1, 0x11, BYTE_BYTE_LEN, 0},
	{0xA2, 0x11, BYTE_BYTE_LEN, 0},
	{0xA3, 0x11, BYTE_BYTE_LEN, 0},
	{0xA4, 0x11, BYTE_BYTE_LEN, 0},
	{0xA5, 0x11, BYTE_BYTE_LEN, 0},

	{0xE1, 0x25, BYTE_BYTE_LEN, 0}, //Outdoor AG Max
	{0xE2, 0x03, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// Capture Setting
	///////////////////////////////////////////
				
	{0x03, 0xd5, BYTE_BYTE_LEN, 0},
	{0x11, 0xb9, BYTE_BYTE_LEN, 0}, //manual sleep onoff	
	{0x14, 0xfd, BYTE_BYTE_LEN, 0}, // STEVE EXPMIN x2
	{0x1e, 0x02, BYTE_BYTE_LEN, 0}, //capture clock set
	{0x86, 0x02, BYTE_BYTE_LEN, 0}, //preview clock set
	
{0x1f, 0x01, BYTE_BYTE_LEN, 0},
{0x20, 0x40, BYTE_BYTE_LEN, 0}, // Capture Hblank 320 -) one line 2500
	
	{0x21, 0x09, BYTE_BYTE_LEN, 0},
{0x22, 0xc4, BYTE_BYTE_LEN, 0},// Capture Line unit 2180+320 = 2500
	
	///////////////////////////////////////////
	// Capture Mode option D6
	///////////////////////////////////////////   
	{0x03, 0xd6, BYTE_BYTE_LEN, 0}, 	 
	
	{0x03, 0xd6, BYTE_BYTE_LEN, 0}, 
	{0x10, 0x28, BYTE_BYTE_LEN, 0}, // ISO 100
	{0x11, 0x38, BYTE_BYTE_LEN, 0}, // ISO 200
	{0x12, 0x78, BYTE_BYTE_LEN, 0}, // ISO 400
	{0x13, 0xa0, BYTE_BYTE_LEN, 0}, // ISO 800
	{0x14, 0xe0, BYTE_BYTE_LEN, 0}, // ISO 1600
	{0x15, 0xf0, BYTE_BYTE_LEN, 0}, // ISO 3200
	///////////////////////////////////////////
	// C0 Page Firmware system
	///////////////////////////////////////////
	{0x03, 0xc0, BYTE_BYTE_LEN, 0},
	{0x16, 0x81, BYTE_BYTE_LEN, 0}, //MCU main roof holding on
	
///////////////////////////////////////////
// DAWB control : Page Mode = 0xC5 ~ 0xC9
///////////////////////////////////////////               
{0x03,0xc5, BYTE_BYTE_LEN, 0},
 
{0x10,0x30, BYTE_BYTE_LEN, 0},
{0x11,0xa1, BYTE_BYTE_LEN, 0},
{0x12,0x17, BYTE_BYTE_LEN, 0},
{0x13,0x0d, BYTE_BYTE_LEN, 0},// high temp. option off
{0x14,0x13, BYTE_BYTE_LEN, 0},
{0x15,0x04, BYTE_BYTE_LEN, 0},
{0x16,0x0a, BYTE_BYTE_LEN, 0},
{0x17,0x08, BYTE_BYTE_LEN, 0},
 
{0x18,0x0a, BYTE_BYTE_LEN, 0},
{0x19,0x03, BYTE_BYTE_LEN, 0},
{0x1a,0xa0, BYTE_BYTE_LEN, 0},//awb max ylvl
{0x1b,0x18, BYTE_BYTE_LEN, 0},//awb min ylvl
{0x1c,0x0a, BYTE_BYTE_LEN, 0},//awb frame skip when min max
{0x1d,0x40, BYTE_BYTE_LEN, 0},
{0x1e,0x00, BYTE_BYTE_LEN, 0},
{0x1f,0xF2, BYTE_BYTE_LEN, 0},//sky limit
 
{0x20,0x00, BYTE_BYTE_LEN, 0},
{0x21,0xa0, BYTE_BYTE_LEN, 0},
{0x22,0x00, BYTE_BYTE_LEN, 0},
{0x23,0xF2, BYTE_BYTE_LEN, 0},//sky limit
{0x24,0x00, BYTE_BYTE_LEN, 0},
{0x25,0xa0, BYTE_BYTE_LEN, 0},
{0x26,0x01, BYTE_BYTE_LEN, 0},
{0x27,0x09, BYTE_BYTE_LEN, 0},
 
{0x28,0x00, BYTE_BYTE_LEN, 0},
{0x29,0x7d, BYTE_BYTE_LEN, 0}, //jktest 0131//0x64, BYTE_BYTE_LEN, 0},
{0x2a,0x00, BYTE_BYTE_LEN, 0},
{0x2b,0xe6, BYTE_BYTE_LEN, 0},
{0x2c,0x00, BYTE_BYTE_LEN, 0},
{0x2d,0x82, BYTE_BYTE_LEN, 0}, // dark angle max

{0x2e,0x00, BYTE_BYTE_LEN, 0},
{0x2f,0x00, BYTE_BYTE_LEN, 0},
{0x30,0x66, BYTE_BYTE_LEN, 0},
{0x31,0x8a, BYTE_BYTE_LEN, 0},

{0x32,0x00, BYTE_BYTE_LEN, 0},
{0x33,0x00, BYTE_BYTE_LEN, 0},
{0x34,0x8b, BYTE_BYTE_LEN, 0},
{0x35,0x29, BYTE_BYTE_LEN, 0},

{0x36,0x00, BYTE_BYTE_LEN, 0},
{0x37,0x06, BYTE_BYTE_LEN, 0},
{0x38,0xdd, BYTE_BYTE_LEN, 0},
{0x39,0xd0, BYTE_BYTE_LEN, 0},

{0x3a,0x00, BYTE_BYTE_LEN, 0},
{0x3b,0x0d, BYTE_BYTE_LEN, 0},
{0x3c,0xbb, BYTE_BYTE_LEN, 0},
{0x3d,0xa0, BYTE_BYTE_LEN, 0},

{0x3e,0x00, BYTE_BYTE_LEN, 0},
{0x3f,0xdb, BYTE_BYTE_LEN, 0},
{0x40,0xba, BYTE_BYTE_LEN, 0},
{0x41,0x00, BYTE_BYTE_LEN, 0},

{0x42,0x00, BYTE_BYTE_LEN, 0},
{0x43,0xe9, BYTE_BYTE_LEN, 0},
{0x44,0x75, BYTE_BYTE_LEN, 0},
{0x45,0xa0, BYTE_BYTE_LEN, 0},

{0x46,0x00, BYTE_BYTE_LEN, 0},
{0x47,0x04, BYTE_BYTE_LEN, 0},
{0x48,0x93, BYTE_BYTE_LEN, 0},
{0x49,0xE0, BYTE_BYTE_LEN, 0},

{0x4a,0x00, BYTE_BYTE_LEN, 0},
{0x4b,0x05, BYTE_BYTE_LEN, 0},
{0x4c,0x57, BYTE_BYTE_LEN, 0},
{0x4d,0x30, BYTE_BYTE_LEN, 0},

{0x4e,0x03, BYTE_BYTE_LEN, 0}, //jktest 0216 for TL//0x02, BYTE_BYTE_LEN, 0},//wb slop offset r
{0x4f,0x03, BYTE_BYTE_LEN, 0}, //jktest 0216 for TL//0x02, BYTE_BYTE_LEN, 0},//wb slop offset b
 
{0x50,0x55, BYTE_BYTE_LEN, 0},
{0x51,0x55, BYTE_BYTE_LEN, 0},
{0x52,0x55, BYTE_BYTE_LEN, 0},
{0x53,0x55, BYTE_BYTE_LEN, 0},
{0x54,0x55, BYTE_BYTE_LEN, 0},
{0x55,0x55, BYTE_BYTE_LEN, 0},
{0x56,0x55, BYTE_BYTE_LEN, 0},
{0x57,0x55, BYTE_BYTE_LEN, 0},
 
{0x58,0x55, BYTE_BYTE_LEN, 0},
{0x59,0x55, BYTE_BYTE_LEN, 0},
{0x5a,0x55, BYTE_BYTE_LEN, 0},
{0x5b,0x55, BYTE_BYTE_LEN, 0},
{0x5c,0x55, BYTE_BYTE_LEN, 0},
{0x5d,0x55, BYTE_BYTE_LEN, 0},
{0x5e,0x55, BYTE_BYTE_LEN, 0},
{0x5f,0x55, BYTE_BYTE_LEN, 0},
 
{0x60,0x55, BYTE_BYTE_LEN, 0},
{0x61,0x55, BYTE_BYTE_LEN, 0},
{0x62,0x55, BYTE_BYTE_LEN, 0},
{0x63,0x55, BYTE_BYTE_LEN, 0},
{0x64,0x55, BYTE_BYTE_LEN, 0},
{0x65,0x55, BYTE_BYTE_LEN, 0},
{0x66,0x55, BYTE_BYTE_LEN, 0},
{0x67,0x55, BYTE_BYTE_LEN, 0},
 
{0x68,0x55, BYTE_BYTE_LEN, 0},
{0x69,0x55, BYTE_BYTE_LEN, 0},
{0x6a,0x55, BYTE_BYTE_LEN, 0},
{0x6b,0x1f, BYTE_BYTE_LEN, 0},
{0x6c,0x26, BYTE_BYTE_LEN, 0},
{0x6d,0x2c, BYTE_BYTE_LEN, 0},
{0x6e,0x33, BYTE_BYTE_LEN, 0},
{0x6f,0x3c, BYTE_BYTE_LEN, 0},
 
{0x70,0x49, BYTE_BYTE_LEN, 0},
{0x71,0x53, BYTE_BYTE_LEN, 0},
{0x72,0x5c, BYTE_BYTE_LEN, 0},
{0x73,0x67, BYTE_BYTE_LEN, 0},
{0x74,0x72, BYTE_BYTE_LEN, 0},
{0x75,0x7d, BYTE_BYTE_LEN, 0},
{0x76,0x86, BYTE_BYTE_LEN, 0},
{0x77,0x72, BYTE_BYTE_LEN, 0},
 
{0x78,0x62, BYTE_BYTE_LEN, 0},
{0x79,0x54, BYTE_BYTE_LEN, 0},
{0x7a,0x46, BYTE_BYTE_LEN, 0},
{0x7b,0x3e, BYTE_BYTE_LEN, 0},
{0x7c,0x3b, BYTE_BYTE_LEN, 0},
{0x7d,0x38, BYTE_BYTE_LEN, 0},
{0x7e,0x36, BYTE_BYTE_LEN, 0},
{0x7f,0x35, BYTE_BYTE_LEN, 0},
 
{0x80,0x34, BYTE_BYTE_LEN, 0},
{0x81,0x99, BYTE_BYTE_LEN, 0},
{0x82,0x90, BYTE_BYTE_LEN, 0},
{0x83,0x86, BYTE_BYTE_LEN, 0},
{0x84,0x7a, BYTE_BYTE_LEN, 0},
{0x85,0x6c, BYTE_BYTE_LEN, 0},
{0x86,0x5e, BYTE_BYTE_LEN, 0},
{0x87,0x52, BYTE_BYTE_LEN, 0},
 
{0x88,0x4c, BYTE_BYTE_LEN, 0},
{0x89,0x48, BYTE_BYTE_LEN, 0},
{0x8a,0x46, BYTE_BYTE_LEN, 0},
{0x8b,0x45, BYTE_BYTE_LEN, 0},
{0x8c,0x1f, BYTE_BYTE_LEN, 0},
{0x8d,0x26, BYTE_BYTE_LEN, 0},
{0x8e,0x2b, BYTE_BYTE_LEN, 0},
{0x8f,0x30, BYTE_BYTE_LEN, 0},
 
{0x90,0x36, BYTE_BYTE_LEN, 0},
{0x91,0x3c, BYTE_BYTE_LEN, 0},
{0x92,0x47, BYTE_BYTE_LEN, 0},
{0x93,0x52, BYTE_BYTE_LEN, 0},
{0x94,0x5f, BYTE_BYTE_LEN, 0},
{0x95,0x6d, BYTE_BYTE_LEN, 0},
{0x96,0x7d, BYTE_BYTE_LEN, 0},
{0x97,0x8f, BYTE_BYTE_LEN, 0},
 
{0x98,0x81, BYTE_BYTE_LEN, 0},
{0x99,0x78, BYTE_BYTE_LEN, 0},
{0x9a,0x6d, BYTE_BYTE_LEN, 0},
{0x9b,0x61, BYTE_BYTE_LEN, 0},
{0x9c,0x55, BYTE_BYTE_LEN, 0},
{0x9d,0x4b, BYTE_BYTE_LEN, 0},
{0x9e,0x44, BYTE_BYTE_LEN, 0},
{0x9f,0x41, BYTE_BYTE_LEN, 0},
 
{0xa0,0x3e, BYTE_BYTE_LEN, 0},
{0xa1,0x3c, BYTE_BYTE_LEN, 0},
{0xa2,0x28, BYTE_BYTE_LEN, 0},
{0xa3,0x2d, BYTE_BYTE_LEN, 0},
{0xa4,0x37, BYTE_BYTE_LEN, 0},
{0xa5,0x46, BYTE_BYTE_LEN, 0},
{0xa6,0x50, BYTE_BYTE_LEN, 0},
{0xa7,0x55, BYTE_BYTE_LEN, 0},
 
{0xa8,0x69, BYTE_BYTE_LEN, 0},//205degree b gain 8 code up
{0xa9,0x82, BYTE_BYTE_LEN, 0},
{0xaa,0x00, BYTE_BYTE_LEN, 0},
{0xab,0x00, BYTE_BYTE_LEN, 0},
{0xac,0x8a, BYTE_BYTE_LEN, 0}, //jktest 0131//0x00, BYTE_BYTE_LEN, 0},
{0xad,0x8a, BYTE_BYTE_LEN, 0}, //jktest 0131//0x00, BYTE_BYTE_LEN, 0},
{0xae,0x00, BYTE_BYTE_LEN, 0},//awb indoor taget r
{0xaf,0x00, BYTE_BYTE_LEN, 0},//awb indoor taget r
 
{0xb0,0x00, BYTE_BYTE_LEN, 0},
{0xb1,0x00, BYTE_BYTE_LEN, 0},
{0xb2,0x00, BYTE_BYTE_LEN, 0},
{0xb3,0x00, BYTE_BYTE_LEN, 0},
{0xb4,0x05, BYTE_BYTE_LEN, 0}, //jktest 0131//0x00, BYTE_BYTE_LEN, 0},
{0xb5,0x05, BYTE_BYTE_LEN, 0}, //jktest 0131//0x00, BYTE_BYTE_LEN, 0},
{0xb6,0x00, BYTE_BYTE_LEN, 0},//awb indoor taget b
{0xb7,0x00, BYTE_BYTE_LEN, 0},//awb indoor taget b
 
{0xb8,0x00, BYTE_BYTE_LEN, 0}, //205degree [ indoor ]b gain 6 code up
{0xb9,0x00, BYTE_BYTE_LEN, 0},
{0xba,0x00, BYTE_BYTE_LEN, 0},
{0xbb,0x00, BYTE_BYTE_LEN, 0},
{0xbc,0x00, BYTE_BYTE_LEN, 0},
{0xbd,0x00, BYTE_BYTE_LEN, 0},
{0xbe,0x00, BYTE_BYTE_LEN, 0},
{0xbf,0x00, BYTE_BYTE_LEN, 0},
 
{0xc0,0x00, BYTE_BYTE_LEN, 0},
{0xc1,0x00, BYTE_BYTE_LEN, 0},
{0xc2,0x00, BYTE_BYTE_LEN, 0},
{0xc3,0x00, BYTE_BYTE_LEN, 0},
{0xc4,0x00, BYTE_BYTE_LEN, 0},
{0xc5,0x00, BYTE_BYTE_LEN, 0},
{0xc6,0x00, BYTE_BYTE_LEN, 0},
{0xc7,0x00, BYTE_BYTE_LEN, 0},
 
{0xc8,0x00, BYTE_BYTE_LEN, 0},
{0xc9,0x00, BYTE_BYTE_LEN, 0},
{0xca,0x00, BYTE_BYTE_LEN, 0},
{0xcb,0x00, BYTE_BYTE_LEN, 0},
{0xcc,0x00, BYTE_BYTE_LEN, 0},
{0xcd,0x00, BYTE_BYTE_LEN, 0},
{0xce,0x00, BYTE_BYTE_LEN, 0},
{0xcf,0x00, BYTE_BYTE_LEN, 0},
 
{0xd0,0x00, BYTE_BYTE_LEN, 0},
{0xd1,0x00, BYTE_BYTE_LEN, 0},
{0xd2,0x0a, BYTE_BYTE_LEN, 0},
{0xd3,0x0e, BYTE_BYTE_LEN, 0},
{0xd4,0x14, BYTE_BYTE_LEN, 0},
{0xd5,0x1e, BYTE_BYTE_LEN, 0},
{0xd6,0x28, BYTE_BYTE_LEN, 0},
{0xd7,0x22, BYTE_BYTE_LEN, 0},
 
{0xd8,0x1e, BYTE_BYTE_LEN, 0},
{0xd9,0x1b, BYTE_BYTE_LEN, 0},
{0xda,0x18, BYTE_BYTE_LEN, 0},
{0xdb,0x14, BYTE_BYTE_LEN, 0},
{0xdc,0x10, BYTE_BYTE_LEN, 0},
{0xdd,0x0d, BYTE_BYTE_LEN, 0},
{0xde,0x0a, BYTE_BYTE_LEN, 0},
{0xdf,0x0a, BYTE_BYTE_LEN, 0},
 
{0xe0,0x0a, BYTE_BYTE_LEN, 0},
{0xe1,0x0a, BYTE_BYTE_LEN, 0},
{0xe2,0x28, BYTE_BYTE_LEN, 0},
{0xe3,0x28, BYTE_BYTE_LEN, 0},
{0xe4,0x28, BYTE_BYTE_LEN, 0},
{0xe5,0x28, BYTE_BYTE_LEN, 0},
{0xe6,0x28, BYTE_BYTE_LEN, 0},
{0xe7,0x24, BYTE_BYTE_LEN, 0},
 
{0xe8,0x20, BYTE_BYTE_LEN, 0},
{0xe9,0x1c, BYTE_BYTE_LEN, 0},
{0xea,0x18, BYTE_BYTE_LEN, 0},
{0xeb,0x14, BYTE_BYTE_LEN, 0},
{0xec,0x14, BYTE_BYTE_LEN, 0},
{0xed,0x0a, BYTE_BYTE_LEN, 0},
{0xee,0x0a, BYTE_BYTE_LEN, 0},
{0xef,0x0a, BYTE_BYTE_LEN, 0},
 
{0xf0,0x0a, BYTE_BYTE_LEN, 0},
{0xf1,0x09, BYTE_BYTE_LEN, 0},
{0xf2,0x08, BYTE_BYTE_LEN, 0},
{0xf3,0x07, BYTE_BYTE_LEN, 0},
{0xf4,0x07, BYTE_BYTE_LEN, 0},
{0xf5,0x06, BYTE_BYTE_LEN, 0},
{0xf6,0x06, BYTE_BYTE_LEN, 0},
{0xf7,0x05, BYTE_BYTE_LEN, 0},
 
{0xf8,0x64, BYTE_BYTE_LEN, 0},
{0xf9,0x05, BYTE_BYTE_LEN, 0},
{0xfa,0x32, BYTE_BYTE_LEN, 0},
{0xfb,0x28, BYTE_BYTE_LEN, 0},
{0xfc,0x64, BYTE_BYTE_LEN, 0},
{0xfd,0x14, BYTE_BYTE_LEN, 0},//indoor angle offset 26->14
     
{0x03,0xc6, BYTE_BYTE_LEN, 0},
 
{0x10,0x1e, BYTE_BYTE_LEN, 0},
{0x11,0x50, BYTE_BYTE_LEN, 0},
{0x12,0x1e, BYTE_BYTE_LEN, 0},// spec temp 1e -> 32 50%
{0x13,0x14, BYTE_BYTE_LEN, 0},
{0x14,0x64, BYTE_BYTE_LEN, 0}, //96 ->64
{0x15,0x1e, BYTE_BYTE_LEN, 0},
{0x16,0x04, BYTE_BYTE_LEN, 0},
{0x17,0xf8, BYTE_BYTE_LEN, 0},
 
{0x18,0x40, BYTE_BYTE_LEN, 0},
{0x19,0xf0, BYTE_BYTE_LEN, 0},
{0x1a,0x40, BYTE_BYTE_LEN, 0},
{0x1b,0xf0, BYTE_BYTE_LEN, 0},
{0x1c,0x08, BYTE_BYTE_LEN, 0},
{0x1d,0x00, BYTE_BYTE_LEN, 0},
{0x1e,0x34, BYTE_BYTE_LEN, 0},
{0x1f,0x39, BYTE_BYTE_LEN, 0},
 
{0x20,0x3F, BYTE_BYTE_LEN, 0},
{0x21,0x46, BYTE_BYTE_LEN, 0},
{0x22,0x4B, BYTE_BYTE_LEN, 0},
{0x23,0x52, BYTE_BYTE_LEN, 0},
{0x24,0x58, BYTE_BYTE_LEN, 0},
{0x25,0x5F, BYTE_BYTE_LEN, 0},
{0x26,0x66, BYTE_BYTE_LEN, 0},
{0x27,0x6D, BYTE_BYTE_LEN, 0},
 
{0x28,0x79, BYTE_BYTE_LEN, 0},
{0x29,0x5F, BYTE_BYTE_LEN, 0},
{0x2a,0x55, BYTE_BYTE_LEN, 0},
{0x2b,0x4E, BYTE_BYTE_LEN, 0},
{0x2c,0x47, BYTE_BYTE_LEN, 0},
{0x2d,0x43, BYTE_BYTE_LEN, 0},
{0x2e,0x40, BYTE_BYTE_LEN, 0},
{0x2f,0x3D, BYTE_BYTE_LEN, 0},
 
{0x30,0x3C, BYTE_BYTE_LEN, 0},
{0x31,0x3B, BYTE_BYTE_LEN, 0},
{0x32,0x3A, BYTE_BYTE_LEN, 0},
{0x33,0x39, BYTE_BYTE_LEN, 0},
{0x34,0x70, BYTE_BYTE_LEN, 0},
{0x35,0x6B, BYTE_BYTE_LEN, 0},
{0x36,0x67, BYTE_BYTE_LEN, 0},
{0x37,0x64, BYTE_BYTE_LEN, 0},
 
{0x38,0x60, BYTE_BYTE_LEN, 0},
{0x39,0x5C, BYTE_BYTE_LEN, 0},
{0x3a,0x58, BYTE_BYTE_LEN, 0},
{0x3b,0x54, BYTE_BYTE_LEN, 0},
{0x3c,0x51, BYTE_BYTE_LEN, 0},
{0x3d,0x4C, BYTE_BYTE_LEN, 0},
{0x3e,0x4A, BYTE_BYTE_LEN, 0},
{0x3f,0x34, BYTE_BYTE_LEN, 0},
 
{0x40,0x3B, BYTE_BYTE_LEN, 0},
{0x41,0x41, BYTE_BYTE_LEN, 0},
{0x42,0x45, BYTE_BYTE_LEN, 0},
{0x43,0x4B, BYTE_BYTE_LEN, 0},
{0x44,0x51, BYTE_BYTE_LEN, 0},
{0x45,0x57, BYTE_BYTE_LEN, 0},
{0x46,0x60, BYTE_BYTE_LEN, 0},
{0x47,0x6A, BYTE_BYTE_LEN, 0},
 
{0x48,0x72, BYTE_BYTE_LEN, 0},
{0x49,0x79, BYTE_BYTE_LEN, 0},
{0x4a,0x67, BYTE_BYTE_LEN, 0},
{0x4b,0x60, BYTE_BYTE_LEN, 0},
{0x4c,0x59, BYTE_BYTE_LEN, 0},
{0x4d,0x55, BYTE_BYTE_LEN, 0},
{0x4e,0x50, BYTE_BYTE_LEN, 0},
{0x4f,0x4D, BYTE_BYTE_LEN, 0},
 
{0x50,0x4A, BYTE_BYTE_LEN, 0},
{0x51,0x46, BYTE_BYTE_LEN, 0},
{0x52,0x44, BYTE_BYTE_LEN, 0},
{0x53,0x42, BYTE_BYTE_LEN, 0},
{0x54,0x41, BYTE_BYTE_LEN, 0},
{0x55,0x50, BYTE_BYTE_LEN, 0},
{0x56,0x5a, BYTE_BYTE_LEN, 0},
{0x57,0x64, BYTE_BYTE_LEN, 0},
 
{0x58,0x6e, BYTE_BYTE_LEN, 0},
{0x59,0x78, BYTE_BYTE_LEN, 0},
{0x5a,0x82, BYTE_BYTE_LEN, 0},
{0x5b,0x8c, BYTE_BYTE_LEN, 0},
{0x5c,0x96, BYTE_BYTE_LEN, 0},
{0x5d,0x00, BYTE_BYTE_LEN, 0},
{0x5e,0x00, BYTE_BYTE_LEN, 0},
{0x5f,0x00, BYTE_BYTE_LEN, 0},
 
{0x60,0x00, BYTE_BYTE_LEN, 0},
{0x61,0x00, BYTE_BYTE_LEN, 0},
{0x62,0x00, BYTE_BYTE_LEN, 0},
{0x63,0x00, BYTE_BYTE_LEN, 0},
{0x64,0x00, BYTE_BYTE_LEN, 0},
{0x65,0x00, BYTE_BYTE_LEN, 0},
{0x66,0x00, BYTE_BYTE_LEN, 0},
{0x67,0x00, BYTE_BYTE_LEN, 0},
 
{0x68,0x00, BYTE_BYTE_LEN, 0},
{0x69,0x00, BYTE_BYTE_LEN, 0},
{0x6a,0x00, BYTE_BYTE_LEN, 0},
{0x6b,0x00, BYTE_BYTE_LEN, 0},
{0x6c,0x00, BYTE_BYTE_LEN, 0},
{0x6d,0x00, BYTE_BYTE_LEN, 0},
{0x6e,0x00, BYTE_BYTE_LEN, 0},
{0x6f,0x00, BYTE_BYTE_LEN, 0},
 
{0x70,0x00, BYTE_BYTE_LEN, 0},
{0x71,0x00, BYTE_BYTE_LEN, 0},
{0x72,0x00, BYTE_BYTE_LEN, 0},
{0x73,0x00, BYTE_BYTE_LEN, 0},
{0x74,0x00, BYTE_BYTE_LEN, 0},
{0x75,0x00, BYTE_BYTE_LEN, 0},
{0x76,0x00, BYTE_BYTE_LEN, 0},
{0x77,0x00, BYTE_BYTE_LEN, 0},
 
{0x78,0x00, BYTE_BYTE_LEN, 0},
{0x79,0x00, BYTE_BYTE_LEN, 0},
{0x7a,0x00, BYTE_BYTE_LEN, 0},
{0x7b,0x00, BYTE_BYTE_LEN, 0},
{0x7c,0x00, BYTE_BYTE_LEN, 0},
{0x7d,0x00, BYTE_BYTE_LEN, 0},
{0x7e,0x00, BYTE_BYTE_LEN, 0},
{0x7f,0x00, BYTE_BYTE_LEN, 0},
 
{0x80,0x00, BYTE_BYTE_LEN, 0},
{0x81,0x00, BYTE_BYTE_LEN, 0},
{0x82,0x00, BYTE_BYTE_LEN, 0},
{0x83,0x00, BYTE_BYTE_LEN, 0},
{0x84,0x00, BYTE_BYTE_LEN, 0},
{0x85,0x0a, BYTE_BYTE_LEN, 0},
{0x86,0x0a, BYTE_BYTE_LEN, 0},
{0x87,0x0a, BYTE_BYTE_LEN, 0},
 
{0x88,0x0a, BYTE_BYTE_LEN, 0},
{0x89,0x0a, BYTE_BYTE_LEN, 0},
{0x8a,0x0a, BYTE_BYTE_LEN, 0},
{0x8b,0x0a, BYTE_BYTE_LEN, 0},
{0x8c,0x28, BYTE_BYTE_LEN, 0},
{0x8d,0x28, BYTE_BYTE_LEN, 0},
{0x8e,0x28, BYTE_BYTE_LEN, 0},
{0x8f,0x28, BYTE_BYTE_LEN, 0},
 
{0x90,0x1e, BYTE_BYTE_LEN, 0},
{0x91,0x1e, BYTE_BYTE_LEN, 0},
{0x92,0x0a, BYTE_BYTE_LEN, 0},
{0x93,0x0a, BYTE_BYTE_LEN, 0},
{0x94,0x0a, BYTE_BYTE_LEN, 0},
{0x95,0x20, BYTE_BYTE_LEN, 0},
{0x96,0x1e, BYTE_BYTE_LEN, 0},
{0x97,0x1c, BYTE_BYTE_LEN, 0},
 
{0x98,0x1a, BYTE_BYTE_LEN, 0},
{0x99,0x18, BYTE_BYTE_LEN, 0},
{0x9a,0x16, BYTE_BYTE_LEN, 0},
{0x9b,0x14, BYTE_BYTE_LEN, 0},
{0x9c,0x14, BYTE_BYTE_LEN, 0},
{0x9d,0x14, BYTE_BYTE_LEN, 0},
{0x9e,0x12, BYTE_BYTE_LEN, 0},
{0x9f,0x12, BYTE_BYTE_LEN, 0},
 
{0xa0,0x08, BYTE_BYTE_LEN, 0},
{0xa1,0x08, BYTE_BYTE_LEN, 0},
{0xa2,0x08, BYTE_BYTE_LEN, 0},
{0xa3,0x07, BYTE_BYTE_LEN, 0},
{0xa4,0x07, BYTE_BYTE_LEN, 0},
{0xa5,0x07, BYTE_BYTE_LEN, 0},
{0xa6,0x06, BYTE_BYTE_LEN, 0},
{0xa7,0x06, BYTE_BYTE_LEN, 0},
 
{0xa8,0x05, BYTE_BYTE_LEN, 0},
{0xa9,0x05, BYTE_BYTE_LEN, 0},
{0xaa,0x04, BYTE_BYTE_LEN, 0},
{0xab,0x64, BYTE_BYTE_LEN, 0},
{0xac,0x01, BYTE_BYTE_LEN, 0},
{0xad,0x14, BYTE_BYTE_LEN, 0},
{0xae,0x19, BYTE_BYTE_LEN, 0},
{0xaf,0x64, BYTE_BYTE_LEN, 0},//kjh out limit 64 -> 76 sky
 
{0xb0,0x14, BYTE_BYTE_LEN, 0},
{0xb1,0x1e, BYTE_BYTE_LEN, 0},
{0xb2,0x20, BYTE_BYTE_LEN, 0}, //50 -> 20 sky outdoor 
{0xb3,0x32, BYTE_BYTE_LEN, 0}, //1e -> 32(50%)
{0xb4,0x14, BYTE_BYTE_LEN, 0},
{0xb5,0x3c, BYTE_BYTE_LEN, 0},
{0xb6,0x1e, BYTE_BYTE_LEN, 0},
{0xb7,0x08, BYTE_BYTE_LEN, 0},
 
{0xb8,0xd2, BYTE_BYTE_LEN, 0},
{0xb9,0x40, BYTE_BYTE_LEN, 0},
{0xba,0xf0, BYTE_BYTE_LEN, 0},
{0xbb,0x40, BYTE_BYTE_LEN, 0},
{0xbc,0xf0, BYTE_BYTE_LEN, 0},
{0xbd,0x04, BYTE_BYTE_LEN, 0},
{0xbe,0x00, BYTE_BYTE_LEN, 0},
{0xbf,0xcd, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// CD Page (Color ratio)
	///////////////////////////////////////////
	{0x03, 0xCD, BYTE_BYTE_LEN, 0},
	{0x47, 0x06, BYTE_BYTE_LEN, 0},
	{0x10, 0x38, BYTE_BYTE_LEN, 0}, //STEVE B8 -) 38 disable
	
	///////////////////////////////////////////
	//Adaptive mode : Page Mode = 0xCF
	///////////////////////////////////////////
	{0x03, 0xcf, BYTE_BYTE_LEN, 0},
	
	{0x10, 0x00, BYTE_BYTE_LEN, 0},
	{0x11, 0x84, BYTE_BYTE_LEN, 0}, // STEVE 04 -) 84  //cmc + - , adaptive lsc
	{0x12, 0x01, BYTE_BYTE_LEN, 0},
	           
{0x13, 0x00, BYTE_BYTE_LEN, 0}, //Y_LUM_MAX
{0x14, 0xf1, BYTE_BYTE_LEN, 0},
{0x15, 0xac, BYTE_BYTE_LEN, 0},
{0x16, 0xd0, BYTE_BYTE_LEN, 0},
	           
{0x17, 0x00, BYTE_BYTE_LEN, 0},  //Y_LUM middle 1
{0x18, 0x5c, BYTE_BYTE_LEN, 0},
{0x19, 0x75, BYTE_BYTE_LEN, 0},
{0x1a, 0x82, BYTE_BYTE_LEN, 0},
	           
{0x1b, 0x00, BYTE_BYTE_LEN, 0},  //Y_LUM middle 2
{0x1c, 0x04, BYTE_BYTE_LEN, 0},
{0x1d, 0x93, BYTE_BYTE_LEN, 0},
{0x1e, 0xc2, BYTE_BYTE_LEN, 0},
	           
{0x1f, 0x00, BYTE_BYTE_LEN, 0},  //Y_LUM min
	{0x20, 0x00, BYTE_BYTE_LEN, 0},
{0x21, 0x08, BYTE_BYTE_LEN, 0},
{0x22, 0xe0, BYTE_BYTE_LEN, 0},
	           
	{0x23, 0x9a, BYTE_BYTE_LEN, 0},  //CTEM high
	{0x24, 0x54, BYTE_BYTE_LEN, 0},  //ctemp middler
	{0x25, 0x35, BYTE_BYTE_LEN, 0},  //CTEM low 
	           
	{0x26, 0x50, BYTE_BYTE_LEN, 0},  //YCON high   
	{0x27, 0x20, BYTE_BYTE_LEN, 0},  //YCON middle 
	{0x28, 0x01, BYTE_BYTE_LEN, 0},  //YCON low    
	           
	{0x29, 0x00, BYTE_BYTE_LEN, 0}, //Y_LUM max_TH 
	{0x2a, 0x00, BYTE_BYTE_LEN, 0},
	{0x2b, 0x00, BYTE_BYTE_LEN, 0},
	{0x2c, 0x00, BYTE_BYTE_LEN, 0},
	           
	{0x2d, 0x00, BYTE_BYTE_LEN, 0},  //Y_LUM middle1_TH
	{0x2e, 0x00, BYTE_BYTE_LEN, 0},
	{0x2f, 0x00, BYTE_BYTE_LEN, 0},
	{0x30, 0x00, BYTE_BYTE_LEN, 0},
	           
	{0x31, 0x00, BYTE_BYTE_LEN, 0},  //Y_LUM middle_TH
	{0x32, 0x00, BYTE_BYTE_LEN, 0},
	{0x33, 0x00, BYTE_BYTE_LEN, 0},
	{0x34, 0x00, BYTE_BYTE_LEN, 0},
	           
	{0x35, 0x00, BYTE_BYTE_LEN, 0}, //Y_LUM min_TH
	{0x36, 0x00, BYTE_BYTE_LEN, 0},
	{0x37, 0x00, BYTE_BYTE_LEN, 0},
	{0x38, 0x00, BYTE_BYTE_LEN, 0},
	           
	{0x39, 0x00, BYTE_BYTE_LEN, 0},  //CTEM high_TH  
	{0x3a, 0x00, BYTE_BYTE_LEN, 0},  //CTEM middle_TH
	{0x3b, 0x00, BYTE_BYTE_LEN, 0}, //CTEM low_TH
	           
	{0x3c, 0x00, BYTE_BYTE_LEN, 0}, //YCON high_TH
	{0x3d, 0x00, BYTE_BYTE_LEN, 0}, //YCON middle_TH
	{0x3e, 0x00, BYTE_BYTE_LEN, 0}, //YCON low_TH
	           
////////////////////////////////////////////
	// CF Page Adaptive Y Target
///////////////////////////////////////////
	           
{0x3f, 0x30, BYTE_BYTE_LEN, 0}, //YLVL_00
{0x40, 0x30, BYTE_BYTE_LEN, 0}, //YLVL_01
{0x41, 0x30, BYTE_BYTE_LEN, 0}, //YLVL_02
{0x42, 0x30, BYTE_BYTE_LEN, 0}, //YLVL_03
{0x43, 0x30, BYTE_BYTE_LEN, 0}, //YLVL_04
{0x44, 0x30, BYTE_BYTE_LEN, 0}, //YLVL_05
{0x45, 0x38, BYTE_BYTE_LEN, 0}, //YLVL_06
{0x46, 0x38, BYTE_BYTE_LEN, 0}, //YLVL_07
{0x47, 0x38, BYTE_BYTE_LEN, 0}, //YLVL_08
{0x48, 0x38, BYTE_BYTE_LEN, 0}, //YLVL_09
{0x49, 0x38, BYTE_BYTE_LEN, 0}, //YLVL_10
{0x4a, 0x38, BYTE_BYTE_LEN, 0}, //YLVL_11
	           
///////////////////////////////////////////
// CF Page Adaptive Y Contrast (4b~56)
///////////////////////////////////////////
	{0x4b, 0x80, BYTE_BYTE_LEN, 0},  //YCON_00
	{0x4c, 0x80, BYTE_BYTE_LEN, 0},  //YCON_01
	{0x4d, 0x80, BYTE_BYTE_LEN, 0},  //YCON_02
	{0x4e, 0x80, BYTE_BYTE_LEN, 0},  //Contrast 3 
	{0x4f, 0x80, BYTE_BYTE_LEN, 0},  //Contrast 4 
	{0x50, 0x80, BYTE_BYTE_LEN, 0},  //Contrast 5 
	{0x51, 0x80, BYTE_BYTE_LEN, 0},  //Contrast 6 
	{0x52, 0x80, BYTE_BYTE_LEN, 0},  //Contrast 7 
	{0x53, 0x80, BYTE_BYTE_LEN, 0},  //Contrast 8 
	{0x54, 0x80, BYTE_BYTE_LEN, 0},  //Contrast 9 
	{0x55, 0x80, BYTE_BYTE_LEN, 0},  //Contrast 10
	{0x56, 0x80, BYTE_BYTE_LEN, 0},  //Contrast 11
	           
///////////////////////////////////////////
// CF Page Adaptive Y OFFSET (57~62)
///////////////////////////////////////////
{0x57, 0x00, BYTE_BYTE_LEN, 0}, //jktest//0x08, BYTE_BYTE_LEN, 0}, // dark offset for noise
{0x58, 0x00, BYTE_BYTE_LEN, 0}, //jktest//0x08, BYTE_BYTE_LEN, 0},
{0x59, 0x00, BYTE_BYTE_LEN, 0}, //jktest//0x08, BYTE_BYTE_LEN, 0},
	{0x5a, 0x00, BYTE_BYTE_LEN, 0},
	{0x5b, 0x00, BYTE_BYTE_LEN, 0},
	{0x5c, 0x00, BYTE_BYTE_LEN, 0},
	{0x5d, 0x00, BYTE_BYTE_LEN, 0},
	{0x5e, 0x00, BYTE_BYTE_LEN, 0},
	{0x5f, 0x00, BYTE_BYTE_LEN, 0},
	{0x60, 0x80, BYTE_BYTE_LEN, 0},
	{0x61, 0x80, BYTE_BYTE_LEN, 0},
	{0x62, 0x80, BYTE_BYTE_LEN, 0},  
	/////////////2//////////////////////
	// CF~D0~D1 P2tive GAMMA
	/////////////2//////////////////////
	           
	           
	{0x63, 0x08, BYTE_BYTE_LEN, 0}, //0x0d, BYTE_BYTE_LEN, 0}, //28 set//0x10, BYTE_BYTE_LEN, 0}, //0x00, BYTE_BYTE_LEN, 0},//0x06, BYTE_BYTE_LEN, 0},//0x03, BYTE_BYTE_LEN, 0},//0x1d, BYTE_BYTE_LEN, 0}, //noise test//0x03, BYTE_BYTE_LEN, 0},//0220//0x04, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x00, BYTE_BYTE_LEN, 0},//GMA00
	{0x64, 0x0e, BYTE_BYTE_LEN, 0}, //0x15, BYTE_BYTE_LEN, 0}, //28 set//0x1a, BYTE_BYTE_LEN, 0}, //0x0f, BYTE_BYTE_LEN, 0},//0x0c, BYTE_BYTE_LEN, 0},//0x0c, BYTE_BYTE_LEN, 0},//0x21, BYTE_BYTE_LEN, 0}, //noise test//0x0c, BYTE_BYTE_LEN, 0},//0220//0x09, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x07, BYTE_BYTE_LEN, 0},
	{0x65, 0x13, BYTE_BYTE_LEN, 0}, //0x1b, BYTE_BYTE_LEN, 0}, //28 set//0x20, BYTE_BYTE_LEN, 0}, //0x18, BYTE_BYTE_LEN, 0},//0x15, BYTE_BYTE_LEN, 0},//0x15, BYTE_BYTE_LEN, 0},//0x24, BYTE_BYTE_LEN, 0}, //noise test//0x15, BYTE_BYTE_LEN, 0},//0220//0x0e, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x0E, BYTE_BYTE_LEN, 0},
	{0x66, 0x1c, BYTE_BYTE_LEN, 0}, //0x23, BYTE_BYTE_LEN, 0}, //28 set//0x27, BYTE_BYTE_LEN, 0}, //0x21, BYTE_BYTE_LEN, 0},//0x1b, BYTE_BYTE_LEN, 0},//0x1c, BYTE_BYTE_LEN, 0},//0x29, BYTE_BYTE_LEN, 0}, //noise test//0x1c, BYTE_BYTE_LEN, 0},//0220//0x16, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x19, BYTE_BYTE_LEN, 0},
	{0x67, 0x25, BYTE_BYTE_LEN, 0}, //0x2a, BYTE_BYTE_LEN, 0}, //28 set//0x32, BYTE_BYTE_LEN, 0}, //0x29, BYTE_BYTE_LEN, 0},//0x24, BYTE_BYTE_LEN, 0},//0x25, BYTE_BYTE_LEN, 0},//0x2e, BYTE_BYTE_LEN, 0}, //noise test//0x25, BYTE_BYTE_LEN, 0},//0220//0x21, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x20, BYTE_BYTE_LEN, 0},
	{0x68, 0x31, BYTE_BYTE_LEN, 0}, //0x36, BYTE_BYTE_LEN, 0}, //28 set//0x3a, BYTE_BYTE_LEN, 0}, //0x39, BYTE_BYTE_LEN, 0},//0x2E, BYTE_BYTE_LEN, 0},//0x2e, BYTE_BYTE_LEN, 0},//0x35, BYTE_BYTE_LEN, 0}, //noise test//0x2e, BYTE_BYTE_LEN, 0},//0220//0x30, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x2E, BYTE_BYTE_LEN, 0},
	{0x69, 0x3c, BYTE_BYTE_LEN, 0}, //0x3e, BYTE_BYTE_LEN, 0}, //28 set//0x43, BYTE_BYTE_LEN, 0}, //0x47, BYTE_BYTE_LEN, 0},//0x3b, BYTE_BYTE_LEN, 0},//0x3b, BYTE_BYTE_LEN, 0}, //noise test//0x3b, BYTE_BYTE_LEN, 0},//0220//0x3f, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x3B, BYTE_BYTE_LEN, 0},
	{0x6a, 0x43, BYTE_BYTE_LEN, 0}, //0x48, BYTE_BYTE_LEN, 0}, //28 set//0x4e, BYTE_BYTE_LEN, 0}, //0x54, BYTE_BYTE_LEN, 0},//0220//0x4e, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x45, BYTE_BYTE_LEN, 0},
	{0x6b, 0x4d, BYTE_BYTE_LEN, 0}, //0x51, BYTE_BYTE_LEN, 0}, //28 set//0x59, BYTE_BYTE_LEN, 0}, //0x61, BYTE_BYTE_LEN, 0},//0220//0x5a, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x4D, BYTE_BYTE_LEN, 0},
	{0x6c, 0x58, BYTE_BYTE_LEN, 0}, //0x5a, BYTE_BYTE_LEN, 0}, //28 set//0x61, BYTE_BYTE_LEN, 0}, //0x6c, BYTE_BYTE_LEN, 0},//0220//0x67, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x54, BYTE_BYTE_LEN, 0},
	{0x6d, 0x62, BYTE_BYTE_LEN, 0}, //0x61, BYTE_BYTE_LEN, 0}, //28 set//0x68, BYTE_BYTE_LEN, 0}, //0x77, BYTE_BYTE_LEN, 0},//0x5a, BYTE_BYTE_LEN, 0},//0220//0x72, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x5A, BYTE_BYTE_LEN, 0},
	{0x6e, 0x6a, BYTE_BYTE_LEN, 0}, //0x6b, BYTE_BYTE_LEN, 0}, //28 set//0x70, BYTE_BYTE_LEN, 0}, //0x81, BYTE_BYTE_LEN, 0},//0220//0x7b, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x63, BYTE_BYTE_LEN, 0},
	{0x6f, 0x71, BYTE_BYTE_LEN, 0}, //28 set//0x76, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0},//0220//0x82, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x69, BYTE_BYTE_LEN, 0},
	{0x70, 0x7e, BYTE_BYTE_LEN, 0}, //0x93, BYTE_BYTE_LEN, 0},//0220//0x89, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x70, BYTE_BYTE_LEN, 0},
	{0x71, 0x82, BYTE_BYTE_LEN, 0}, //0x9a, BYTE_BYTE_LEN, 0},//0220//0x8e, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x76, BYTE_BYTE_LEN, 0},
	{0x72, 0x86, BYTE_BYTE_LEN, 0}, //0xa2, BYTE_BYTE_LEN, 0},//0220//0x92, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x7C, BYTE_BYTE_LEN, 0},
	{0x73, 0x8c, BYTE_BYTE_LEN, 0}, //0xa9, BYTE_BYTE_LEN, 0},//0220//0x96, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x81, BYTE_BYTE_LEN, 0},
	{0x74, 0x93, BYTE_BYTE_LEN, 0}, //0xaf, BYTE_BYTE_LEN, 0},//0220//0x9b, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x86, BYTE_BYTE_LEN, 0},
	{0x75, 0x97, BYTE_BYTE_LEN, 0}, //0xb5, BYTE_BYTE_LEN, 0},//0220//0x9f, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x8C, BYTE_BYTE_LEN, 0},
	{0x76, 0x9e, BYTE_BYTE_LEN, 0}, //0xbb, BYTE_BYTE_LEN, 0},//0220//0xa2, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x90, BYTE_BYTE_LEN, 0},
	{0x77, 0xa4, BYTE_BYTE_LEN, 0}, //0xc0, BYTE_BYTE_LEN, 0},//0220//0xa7, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x95, BYTE_BYTE_LEN, 0},
	{0x78, 0xab, BYTE_BYTE_LEN, 0}, //0xc9, BYTE_BYTE_LEN, 0},//0220//0xac, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x9E, BYTE_BYTE_LEN, 0},
	{0x79, 0xb4, BYTE_BYTE_LEN, 0}, //0xd1, BYTE_BYTE_LEN, 0},//0220//0xb6, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xA7, BYTE_BYTE_LEN, 0},
	{0x7a, 0xbc, BYTE_BYTE_LEN, 0}, //0xd8, BYTE_BYTE_LEN, 0},//0220//0xbc, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xAF, BYTE_BYTE_LEN, 0},
	{0x7b, 0xc8, BYTE_BYTE_LEN, 0}, //0xe2, BYTE_BYTE_LEN, 0},//0220//0xc6, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xBD, BYTE_BYTE_LEN, 0},
	{0x7c, 0xd2, BYTE_BYTE_LEN, 0}, //0xea, BYTE_BYTE_LEN, 0},//0220//0xd1, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xC9, BYTE_BYTE_LEN, 0},
	{0x7d, 0xda, BYTE_BYTE_LEN, 0}, //0xf0, BYTE_BYTE_LEN, 0},//0220//0xdb, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xD4, BYTE_BYTE_LEN, 0},
	{0x7e, 0xdf, BYTE_BYTE_LEN, 0}, //0xf5, BYTE_BYTE_LEN, 0},//0220//0xe2, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xDD, BYTE_BYTE_LEN, 0},
	{0x7f, 0xe5, BYTE_BYTE_LEN, 0}, //0xf8, BYTE_BYTE_LEN, 0},//0220//0xe9, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xE5, BYTE_BYTE_LEN, 0},
	{0x80, 0xeb, BYTE_BYTE_LEN, 0}, //0xfa, BYTE_BYTE_LEN, 0},//0220//0xf0, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xEB, BYTE_BYTE_LEN, 0},
	{0x81, 0xef, BYTE_BYTE_LEN, 0}, //0xfc, BYTE_BYTE_LEN, 0},//0220//0xf6, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xF1, BYTE_BYTE_LEN, 0},
	{0x82, 0xf2, BYTE_BYTE_LEN, 0}, //0xfe, BYTE_BYTE_LEN, 0},//0220//0xfa, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xF6, BYTE_BYTE_LEN, 0},
	{0x83, 0xf3, BYTE_BYTE_LEN, 0}, //0xff, BYTE_BYTE_LEN, 0},//0220//0xfe, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xFB, BYTE_BYTE_LEN, 0},
	{0x84, 0xf3, BYTE_BYTE_LEN, 0}, //0xff, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xFF, BYTE_BYTE_LEN, 0},
	           
	{0x85, 0x08, BYTE_BYTE_LEN, 0}, //0x0d, BYTE_BYTE_LEN, 0}, //28 set//0x10, BYTE_BYTE_LEN, 0}, //0x00, BYTE_BYTE_LEN, 0},//0x06, BYTE_BYTE_LEN, 0},//0x03, BYTE_BYTE_LEN, 0},//0x1d, BYTE_BYTE_LEN, 0}, //noise test//0x03, BYTE_BYTE_LEN, 0},//0220//0x04, BYTE_BYTE_LEN, 0},0x04, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x00, BYTE_BYTE_LEN, 0},//GMA01
	{0x86, 0x0e, BYTE_BYTE_LEN, 0}, //0x15, BYTE_BYTE_LEN, 0}, //28 set//0x1a, BYTE_BYTE_LEN, 0}, //0x0f, BYTE_BYTE_LEN, 0},//0x0c, BYTE_BYTE_LEN, 0},//0x0c, BYTE_BYTE_LEN, 0},//0x21, BYTE_BYTE_LEN, 0}, //noise test//0x0c, BYTE_BYTE_LEN, 0},//0220//0x09, BYTE_BYTE_LEN, 0},0x09, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x07, BYTE_BYTE_LEN, 0},
	{0x87, 0x13, BYTE_BYTE_LEN, 0}, //0x1b, BYTE_BYTE_LEN, 0}, //28 set//0x20, BYTE_BYTE_LEN, 0}, //0x18, BYTE_BYTE_LEN, 0},//0x15, BYTE_BYTE_LEN, 0},//0x15, BYTE_BYTE_LEN, 0},//0x24, BYTE_BYTE_LEN, 0}, //noise test//0x15, BYTE_BYTE_LEN, 0},//0220//0x0e, BYTE_BYTE_LEN, 0},0x0e, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x0E, BYTE_BYTE_LEN, 0},
	{0x88, 0x1c, BYTE_BYTE_LEN, 0}, //0x23, BYTE_BYTE_LEN, 0}, //28 set//0x27, BYTE_BYTE_LEN, 0}, //0x21, BYTE_BYTE_LEN, 0},//0x1b, BYTE_BYTE_LEN, 0},//0x1c, BYTE_BYTE_LEN, 0},//0x29, BYTE_BYTE_LEN, 0}, //noise test//0x1c, BYTE_BYTE_LEN, 0},//0220//0x16, BYTE_BYTE_LEN, 0},0x16, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x19, BYTE_BYTE_LEN, 0},
	{0x89, 0x25, BYTE_BYTE_LEN, 0}, //0x2a, BYTE_BYTE_LEN, 0}, //28 set//0x32, BYTE_BYTE_LEN, 0}, //0x29, BYTE_BYTE_LEN, 0},//0x24, BYTE_BYTE_LEN, 0},//0x25, BYTE_BYTE_LEN, 0},//0x2e, BYTE_BYTE_LEN, 0}, //noise test//0x25, BYTE_BYTE_LEN, 0},//0220//0x21, BYTE_BYTE_LEN, 0},0x21, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x20, BYTE_BYTE_LEN, 0},
	{0x8a, 0x31, BYTE_BYTE_LEN, 0}, //0x36, BYTE_BYTE_LEN, 0}, //28 set//0x3a, BYTE_BYTE_LEN, 0}, //0x39, BYTE_BYTE_LEN, 0},//0x2E, BYTE_BYTE_LEN, 0},//0x2e, BYTE_BYTE_LEN, 0},//0x35, BYTE_BYTE_LEN, 0}, //noise test//0x2e, BYTE_BYTE_LEN, 0},//0220//0x30, BYTE_BYTE_LEN, 0},0x30, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x2E, BYTE_BYTE_LEN, 0},
	{0x8b, 0x3c, BYTE_BYTE_LEN, 0}, //0x3e, BYTE_BYTE_LEN, 0}, //28 set//0x43, BYTE_BYTE_LEN, 0}, //0x47, BYTE_BYTE_LEN, 0},//0x3b, BYTE_BYTE_LEN, 0},//0x3b, BYTE_BYTE_LEN, 0}, //noise test//0x3b, BYTE_BYTE_LEN, 0},//0220//0x3f, BYTE_BYTE_LEN, 0},0x3f, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x3B, BYTE_BYTE_LEN, 0},
	{0x8c, 0x43, BYTE_BYTE_LEN, 0}, //0x48, BYTE_BYTE_LEN, 0}, //28 set//0x4e, BYTE_BYTE_LEN, 0}, //0x54, BYTE_BYTE_LEN, 0},//0220//0x4e, BYTE_BYTE_LEN, 0},0x4e, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x45, BYTE_BYTE_LEN, 0},
	{0x8d, 0x4d, BYTE_BYTE_LEN, 0}, //0x51, BYTE_BYTE_LEN, 0}, //28 set//0x59, BYTE_BYTE_LEN, 0}, //0x61, BYTE_BYTE_LEN, 0},//0220//0x5a, BYTE_BYTE_LEN, 0},0x5a, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x4D, BYTE_BYTE_LEN, 0},
	{0x8e, 0x58, BYTE_BYTE_LEN, 0}, //0x5a, BYTE_BYTE_LEN, 0}, //28 set//0x61, BYTE_BYTE_LEN, 0}, //0x6c, BYTE_BYTE_LEN, 0},//0220//0x67, BYTE_BYTE_LEN, 0},0x67, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x54, BYTE_BYTE_LEN, 0},
	{0x8f, 0x62, BYTE_BYTE_LEN, 0}, //0x61, BYTE_BYTE_LEN, 0}, //28 set//0x68, BYTE_BYTE_LEN, 0}, //0x77, BYTE_BYTE_LEN, 0},//0x5a, BYTE_BYTE_LEN, 0},//0220//0x72, BYTE_BYTE_LEN, 0},0x72, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x5A, BYTE_BYTE_LEN, 0},
	{0x90, 0x6a, BYTE_BYTE_LEN, 0}, //0x6b, BYTE_BYTE_LEN, 0}, //28 set//0x70, BYTE_BYTE_LEN, 0}, //0x81, BYTE_BYTE_LEN, 0},//0220//0x7b, BYTE_BYTE_LEN, 0},0x7b, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x63, BYTE_BYTE_LEN, 0},
	{0x91, 0x71, BYTE_BYTE_LEN, 0}, //28 set//0x76, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0},//0220//0x82, BYTE_BYTE_LEN, 0},0x82, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x69, BYTE_BYTE_LEN, 0},
	{0x92, 0x7e, BYTE_BYTE_LEN, 0}, //0x93, BYTE_BYTE_LEN, 0},//0220//0x89, BYTE_BYTE_LEN, 0},0x89, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x70, BYTE_BYTE_LEN, 0},
	{0x93, 0x82, BYTE_BYTE_LEN, 0}, //0x9a, BYTE_BYTE_LEN, 0},//0220//0x8e, BYTE_BYTE_LEN, 0},0x8e, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x76, BYTE_BYTE_LEN, 0},
	{0x94, 0x86, BYTE_BYTE_LEN, 0}, //0xa2, BYTE_BYTE_LEN, 0},//0220//0x92, BYTE_BYTE_LEN, 0},0x92, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x7C, BYTE_BYTE_LEN, 0},
	{0x95, 0x8c, BYTE_BYTE_LEN, 0}, //0xa9, BYTE_BYTE_LEN, 0},//0220//0x96, BYTE_BYTE_LEN, 0},0x96, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x81, BYTE_BYTE_LEN, 0},
	{0x96, 0x93, BYTE_BYTE_LEN, 0}, //0xaf, BYTE_BYTE_LEN, 0},//0220//0x9b, BYTE_BYTE_LEN, 0},0x9b, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x86, BYTE_BYTE_LEN, 0},
	{0x97, 0x97, BYTE_BYTE_LEN, 0}, //0xb5, BYTE_BYTE_LEN, 0},//0220//0x9f, BYTE_BYTE_LEN, 0},0x9f, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x8C, BYTE_BYTE_LEN, 0},
	{0x98, 0x9e, BYTE_BYTE_LEN, 0}, //0xbb, BYTE_BYTE_LEN, 0},//0220//0xa2, BYTE_BYTE_LEN, 0},0xa2, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x90, BYTE_BYTE_LEN, 0},
	{0x99, 0xa4, BYTE_BYTE_LEN, 0}, //0xc0, BYTE_BYTE_LEN, 0},//0220//0xa7, BYTE_BYTE_LEN, 0},0xa7, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x95, BYTE_BYTE_LEN, 0},
	{0x9a, 0xab, BYTE_BYTE_LEN, 0}, //0xc9, BYTE_BYTE_LEN, 0},//0220//0xac, BYTE_BYTE_LEN, 0},0xac, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x9E, BYTE_BYTE_LEN, 0},
	{0x9b, 0xb4, BYTE_BYTE_LEN, 0}, //0xd1, BYTE_BYTE_LEN, 0},//0220//0xb6, BYTE_BYTE_LEN, 0},0xb6, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xA7, BYTE_BYTE_LEN, 0},
	{0x9c, 0xbc, BYTE_BYTE_LEN, 0}, //0xd8, BYTE_BYTE_LEN, 0},//0220//0xbc, BYTE_BYTE_LEN, 0},0xbc, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xAF, BYTE_BYTE_LEN, 0},
	{0x9d, 0xc8, BYTE_BYTE_LEN, 0}, //0xe2, BYTE_BYTE_LEN, 0},//0220//0xc6, BYTE_BYTE_LEN, 0},0xc6, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xBD, BYTE_BYTE_LEN, 0},
	{0x9e, 0xd2, BYTE_BYTE_LEN, 0}, //0xea, BYTE_BYTE_LEN, 0},//0220//0xd1, BYTE_BYTE_LEN, 0},0xd1, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xC9, BYTE_BYTE_LEN, 0},
	{0x9f, 0xda, BYTE_BYTE_LEN, 0}, //0xf0, BYTE_BYTE_LEN, 0},//0220//0xdb, BYTE_BYTE_LEN, 0},0xdb, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xD4, BYTE_BYTE_LEN, 0},
	{0xa0, 0xdf, BYTE_BYTE_LEN, 0}, //0xf5, BYTE_BYTE_LEN, 0},//0220//0xe2, BYTE_BYTE_LEN, 0},0xe2, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xDD, BYTE_BYTE_LEN, 0},
	{0xa1, 0xe5, BYTE_BYTE_LEN, 0}, //0xf8, BYTE_BYTE_LEN, 0},//0220//0xe9, BYTE_BYTE_LEN, 0},0xe9, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xE5, BYTE_BYTE_LEN, 0},
	{0xa2, 0xeb, BYTE_BYTE_LEN, 0}, //0xfa, BYTE_BYTE_LEN, 0},//0220//0xf0, BYTE_BYTE_LEN, 0},0xf0, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xEB, BYTE_BYTE_LEN, 0},
	{0xa3, 0xef, BYTE_BYTE_LEN, 0}, //0xfc, BYTE_BYTE_LEN, 0},//0220//0xf6, BYTE_BYTE_LEN, 0},0xf6, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xF1, BYTE_BYTE_LEN, 0},
	{0xa4, 0xf2, BYTE_BYTE_LEN, 0}, //0xfe, BYTE_BYTE_LEN, 0},//0220//0xfa, BYTE_BYTE_LEN, 0},0xfa, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xF6, BYTE_BYTE_LEN, 0},
	{0xa5, 0xf3, BYTE_BYTE_LEN, 0}, //0xff, BYTE_BYTE_LEN, 0},//0220//0xfe, BYTE_BYTE_LEN, 0},0xfe, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xFB, BYTE_BYTE_LEN, 0},
	{0xa6, 0xf3, BYTE_BYTE_LEN, 0}, //0xff, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xFF, BYTE_BYTE_LEN, 0},
	           
	{0xa7, 0x08, BYTE_BYTE_LEN, 0}, //0x0d, BYTE_BYTE_LEN, 0}, //28 set//0x10, BYTE_BYTE_LEN, 0}, //0x00, BYTE_BYTE_LEN, 0},//0x06, BYTE_BYTE_LEN, 0},//0x03, BYTE_BYTE_LEN, 0},//0x1d, BYTE_BYTE_LEN, 0}, //noise test//0x03, BYTE_BYTE_LEN, 0},//0220//0x04, BYTE_BYTE_LEN, 0},0x04, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x00, BYTE_BYTE_LEN, 0},//GMA02
	{0xa8, 0x0e, BYTE_BYTE_LEN, 0}, //0x15, BYTE_BYTE_LEN, 0}, //28 set//0x1a, BYTE_BYTE_LEN, 0}, //0x0f, BYTE_BYTE_LEN, 0},//0x0c, BYTE_BYTE_LEN, 0},//0x0c, BYTE_BYTE_LEN, 0},//0x21, BYTE_BYTE_LEN, 0}, //noise test//0x0c, BYTE_BYTE_LEN, 0},//0220//0x09, BYTE_BYTE_LEN, 0},0x09, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x07, BYTE_BYTE_LEN, 0},
	{0xa9, 0x13, BYTE_BYTE_LEN, 0}, //0x1b, BYTE_BYTE_LEN, 0}, //28 set//0x20, BYTE_BYTE_LEN, 0}, //0x18, BYTE_BYTE_LEN, 0},//0x15, BYTE_BYTE_LEN, 0},//0x15, BYTE_BYTE_LEN, 0},//0x24, BYTE_BYTE_LEN, 0}, //noise test//0x15, BYTE_BYTE_LEN, 0},//0220//0x0e, BYTE_BYTE_LEN, 0},0x0e, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x0E, BYTE_BYTE_LEN, 0},
	{0xaa, 0x1c, BYTE_BYTE_LEN, 0}, //0x23, BYTE_BYTE_LEN, 0}, //28 set//0x27, BYTE_BYTE_LEN, 0}, //0x21, BYTE_BYTE_LEN, 0},//0x1b, BYTE_BYTE_LEN, 0},//0x1c, BYTE_BYTE_LEN, 0},//0x29, BYTE_BYTE_LEN, 0}, //noise test//0x1c, BYTE_BYTE_LEN, 0},//0220//0x16, BYTE_BYTE_LEN, 0},0x16, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x19, BYTE_BYTE_LEN, 0},
	{0xab, 0x25, BYTE_BYTE_LEN, 0}, //0x2a, BYTE_BYTE_LEN, 0}, //28 set//0x32, BYTE_BYTE_LEN, 0}, //0x29, BYTE_BYTE_LEN, 0},//0x24, BYTE_BYTE_LEN, 0},//0x25, BYTE_BYTE_LEN, 0},//0x2e, BYTE_BYTE_LEN, 0}, //noise test//0x25, BYTE_BYTE_LEN, 0},//0220//0x21, BYTE_BYTE_LEN, 0},0x21, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x20, BYTE_BYTE_LEN, 0},
	{0xac, 0x31, BYTE_BYTE_LEN, 0}, //0x36, BYTE_BYTE_LEN, 0}, //28 set//0x3a, BYTE_BYTE_LEN, 0}, //0x39, BYTE_BYTE_LEN, 0},//0x2E, BYTE_BYTE_LEN, 0},//0x2e, BYTE_BYTE_LEN, 0},//0x35, BYTE_BYTE_LEN, 0}, //noise test//0x2e, BYTE_BYTE_LEN, 0},//0220//0x30, BYTE_BYTE_LEN, 0},0x30, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x2E, BYTE_BYTE_LEN, 0},
	{0xad, 0x3c, BYTE_BYTE_LEN, 0}, //0x3e, BYTE_BYTE_LEN, 0}, //28 set//0x43, BYTE_BYTE_LEN, 0}, //0x47, BYTE_BYTE_LEN, 0},//0x3b, BYTE_BYTE_LEN, 0},//0x3b, BYTE_BYTE_LEN, 0}, //noise test//0x3b, BYTE_BYTE_LEN, 0},//0220//0x3f, BYTE_BYTE_LEN, 0},0x3f, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x3B, BYTE_BYTE_LEN, 0},
	{0xae, 0x43, BYTE_BYTE_LEN, 0}, //0x48, BYTE_BYTE_LEN, 0}, //28 set//0x4e, BYTE_BYTE_LEN, 0}, //0x54, BYTE_BYTE_LEN, 0},//0220//0x4e, BYTE_BYTE_LEN, 0},0x4e, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x45, BYTE_BYTE_LEN, 0},
	{0xaf, 0x4d, BYTE_BYTE_LEN, 0}, //0x51, BYTE_BYTE_LEN, 0}, //28 set//0x59, BYTE_BYTE_LEN, 0}, //0x61, BYTE_BYTE_LEN, 0},//0220//0x5a, BYTE_BYTE_LEN, 0},0x5a, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x4D, BYTE_BYTE_LEN, 0},
	{0xb0, 0x58, BYTE_BYTE_LEN, 0}, //0x5a, BYTE_BYTE_LEN, 0}, //28 set//0x61, BYTE_BYTE_LEN, 0}, //0x6c, BYTE_BYTE_LEN, 0},//0220//0x67, BYTE_BYTE_LEN, 0},0x67, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x54, BYTE_BYTE_LEN, 0},
	{0xb1, 0x62, BYTE_BYTE_LEN, 0}, //0x61, BYTE_BYTE_LEN, 0}, //28 set//0x68, BYTE_BYTE_LEN, 0}, //0x77, BYTE_BYTE_LEN, 0},//0x5a, BYTE_BYTE_LEN, 0},//0220//0x72, BYTE_BYTE_LEN, 0},0x72, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x5A, BYTE_BYTE_LEN, 0},
	{0xb2, 0x6a, BYTE_BYTE_LEN, 0}, //0x6b, BYTE_BYTE_LEN, 0}, //28 set//0x70, BYTE_BYTE_LEN, 0}, //0x81, BYTE_BYTE_LEN, 0},//0220//0x7b, BYTE_BYTE_LEN, 0},0x7b, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x63, BYTE_BYTE_LEN, 0},
	{0xb3, 0x71, BYTE_BYTE_LEN, 0}, //28 set//0x76, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0},//0220//0x82, BYTE_BYTE_LEN, 0},0x82, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x69, BYTE_BYTE_LEN, 0},
	{0xb4, 0x7e, BYTE_BYTE_LEN, 0}, //0x93, BYTE_BYTE_LEN, 0},//0220//0x89, BYTE_BYTE_LEN, 0},0x89, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x70, BYTE_BYTE_LEN, 0},
	{0xb5, 0x82, BYTE_BYTE_LEN, 0}, //0x9a, BYTE_BYTE_LEN, 0},//0220//0x8e, BYTE_BYTE_LEN, 0},0x8e, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x76, BYTE_BYTE_LEN, 0},
	{0xb6, 0x86, BYTE_BYTE_LEN, 0}, //0xa2, BYTE_BYTE_LEN, 0},//0220//0x92, BYTE_BYTE_LEN, 0},0x92, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x7C, BYTE_BYTE_LEN, 0},
	{0xb7, 0x8c, BYTE_BYTE_LEN, 0}, //0xa9, BYTE_BYTE_LEN, 0},//0220//0x96, BYTE_BYTE_LEN, 0},0x96, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x81, BYTE_BYTE_LEN, 0},
	{0xb8, 0x93, BYTE_BYTE_LEN, 0}, //0xaf, BYTE_BYTE_LEN, 0},//0220//0x9b, BYTE_BYTE_LEN, 0},0x9b, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x86, BYTE_BYTE_LEN, 0},
	{0xb9, 0x97, BYTE_BYTE_LEN, 0}, //0xb5, BYTE_BYTE_LEN, 0},//0220//0x9f, BYTE_BYTE_LEN, 0},0x9f, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x8C, BYTE_BYTE_LEN, 0},
	{0xba, 0x9e, BYTE_BYTE_LEN, 0}, //0xbb, BYTE_BYTE_LEN, 0},//0220//0xa2, BYTE_BYTE_LEN, 0},0xa2, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x90, BYTE_BYTE_LEN, 0},
	{0xbb, 0xa4, BYTE_BYTE_LEN, 0}, //0xc0, BYTE_BYTE_LEN, 0},//0220//0xa7, BYTE_BYTE_LEN, 0},0xa7, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x95, BYTE_BYTE_LEN, 0},
	{0xbc, 0xab, BYTE_BYTE_LEN, 0}, //0xc9, BYTE_BYTE_LEN, 0},//0220//0xac, BYTE_BYTE_LEN, 0},0xac, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0x9E, BYTE_BYTE_LEN, 0},
	{0xbd, 0xb4, BYTE_BYTE_LEN, 0}, //0xd1, BYTE_BYTE_LEN, 0},//0220//0xb6, BYTE_BYTE_LEN, 0},0xb6, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xA7, BYTE_BYTE_LEN, 0},
	{0xbe, 0xbc, BYTE_BYTE_LEN, 0}, //0xd8, BYTE_BYTE_LEN, 0},//0220//0xbc, BYTE_BYTE_LEN, 0},0xbc, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xAF, BYTE_BYTE_LEN, 0},
	{0xbf, 0xc8, BYTE_BYTE_LEN, 0}, //0xe2, BYTE_BYTE_LEN, 0},//0220//0xc6, BYTE_BYTE_LEN, 0},0xc6, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xBD, BYTE_BYTE_LEN, 0},
	{0xc0, 0xd2, BYTE_BYTE_LEN, 0}, //0xea, BYTE_BYTE_LEN, 0},//0220//0xd1, BYTE_BYTE_LEN, 0},0xd1, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xC9, BYTE_BYTE_LEN, 0},
	{0xc1, 0xda, BYTE_BYTE_LEN, 0}, //0xf0, BYTE_BYTE_LEN, 0},//0220//0xdb, BYTE_BYTE_LEN, 0},0xdb, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xD4, BYTE_BYTE_LEN, 0},
	{0xc2, 0xdf, BYTE_BYTE_LEN, 0}, //0xf5, BYTE_BYTE_LEN, 0},//0220//0xe2, BYTE_BYTE_LEN, 0},0xe2, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xDD, BYTE_BYTE_LEN, 0},
	{0xc3, 0xe5, BYTE_BYTE_LEN, 0}, //0xf8, BYTE_BYTE_LEN, 0},//0220//0xe9, BYTE_BYTE_LEN, 0},0xe9, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xE5, BYTE_BYTE_LEN, 0},
	{0xc4, 0xeb, BYTE_BYTE_LEN, 0}, //0xfa, BYTE_BYTE_LEN, 0},//0220//0xf0, BYTE_BYTE_LEN, 0},0xf0, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xEB, BYTE_BYTE_LEN, 0},
	{0xc5, 0xef, BYTE_BYTE_LEN, 0}, //0xfc, BYTE_BYTE_LEN, 0},//0220//0xf6, BYTE_BYTE_LEN, 0},0xf6, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xF1, BYTE_BYTE_LEN, 0},
	{0xc6, 0xf2, BYTE_BYTE_LEN, 0}, //0xfe, BYTE_BYTE_LEN, 0},//0220//0xfa, BYTE_BYTE_LEN, 0},0xfa, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xF6, BYTE_BYTE_LEN, 0},
	{0xc7, 0xf3, BYTE_BYTE_LEN, 0}, //0xff, BYTE_BYTE_LEN, 0},//0220//0xfe, BYTE_BYTE_LEN, 0},0xfe, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xFB, BYTE_BYTE_LEN, 0},
	{0xc8, 0xf3, BYTE_BYTE_LEN, 0}, //0xff, BYTE_BYTE_LEN, 0}, //jktest kyocera 0216//0xFF, BYTE_BYTE_LEN, 0},
	           
	{0xc9, 0x00, BYTE_BYTE_LEN, 0},//GMA03
	{0xca, 0x03, BYTE_BYTE_LEN, 0},       
	{0xcb, 0x08, BYTE_BYTE_LEN, 0},       
	{0xcc, 0x12, BYTE_BYTE_LEN, 0},       
	{0xcd, 0x19, BYTE_BYTE_LEN, 0},       
	{0xce, 0x25, BYTE_BYTE_LEN, 0},       
	{0xcf, 0x32, BYTE_BYTE_LEN, 0},       
	{0xd0, 0x3E, BYTE_BYTE_LEN, 0},       
	{0xd1, 0x4B, BYTE_BYTE_LEN, 0},       
	{0xd2, 0x56, BYTE_BYTE_LEN, 0},       
	{0xd3, 0x62, BYTE_BYTE_LEN, 0},       
	{0xd4, 0x6A, BYTE_BYTE_LEN, 0},       
	{0xd5, 0x71, BYTE_BYTE_LEN, 0},       
	{0xd6, 0x78, BYTE_BYTE_LEN, 0},       
	{0xd7, 0x7F, BYTE_BYTE_LEN, 0},       
	{0xd8, 0x85, BYTE_BYTE_LEN, 0},       
	{0xd9, 0x8A, BYTE_BYTE_LEN, 0},       
	{0xda, 0x90, BYTE_BYTE_LEN, 0},       
	{0xdb, 0x95, BYTE_BYTE_LEN, 0},       
	{0xdc, 0x9A, BYTE_BYTE_LEN, 0},       
	{0xdd, 0x9F, BYTE_BYTE_LEN, 0},       
	{0xde, 0xA9, BYTE_BYTE_LEN, 0},       
	{0xdf, 0xb1, BYTE_BYTE_LEN, 0}, //0217 jk//0xB1, BYTE_BYTE_LEN, 0},       
	{0xe0, 0xb9, BYTE_BYTE_LEN, 0}, //0217 jk//0xB9, BYTE_BYTE_LEN, 0},       
	{0xe1, 0xc3, BYTE_BYTE_LEN, 0}, //0217 jk//0xC6, BYTE_BYTE_LEN, 0},       
	{0xe2, 0xcb, BYTE_BYTE_LEN, 0}, //0217 jk//0xD0, BYTE_BYTE_LEN, 0},       
	{0xe3, 0xd4, BYTE_BYTE_LEN, 0}, //0217 jk//0xD8, BYTE_BYTE_LEN, 0},       
	{0xe4, 0xda, BYTE_BYTE_LEN, 0}, //0217 jk//0xDF, BYTE_BYTE_LEN, 0},       
	{0xe5, 0xe1, BYTE_BYTE_LEN, 0}, //0217 jk//0xE6, BYTE_BYTE_LEN, 0},       
	{0xe6, 0xe7, BYTE_BYTE_LEN, 0}, //0217 jk//0xEC, BYTE_BYTE_LEN, 0},       
	{0xe7, 0xec, BYTE_BYTE_LEN, 0}, //0217 jk//0xF1, BYTE_BYTE_LEN, 0},       
	{0xe8, 0xf0, BYTE_BYTE_LEN, 0}, //0217 jk//0xF7, BYTE_BYTE_LEN, 0},       
	{0xe9, 0xf3, BYTE_BYTE_LEN, 0}, //0217 jk//0xFB, BYTE_BYTE_LEN, 0},       
	{0xea, 0xf5, BYTE_BYTE_LEN, 0}, //0217 jk//0xFF, BYTE_BYTE_LEN, 0},       
	           
	{0xeb, 0x00, BYTE_BYTE_LEN, 0},//GMA04   
	{0xec, 0x03, BYTE_BYTE_LEN, 0},          
	{0xed, 0x08, BYTE_BYTE_LEN, 0},          
	{0xee, 0x12, BYTE_BYTE_LEN, 0},          
	{0xef, 0x19, BYTE_BYTE_LEN, 0},          
	{0xf0, 0x25, BYTE_BYTE_LEN, 0},          
	{0xf1, 0x32, BYTE_BYTE_LEN, 0},          
	{0xf2, 0x3E, BYTE_BYTE_LEN, 0},          
	{0xf3, 0x4B, BYTE_BYTE_LEN, 0},          
	{0xf4, 0x56, BYTE_BYTE_LEN, 0},          
	{0xf5, 0x62, BYTE_BYTE_LEN, 0},          
	{0xf6, 0x6A, BYTE_BYTE_LEN, 0},          
	{0xf7, 0x71, BYTE_BYTE_LEN, 0},          
	{0xf8, 0x78, BYTE_BYTE_LEN, 0},          
	{0xf9, 0x7F, BYTE_BYTE_LEN, 0},          
	{0xfa, 0x85, BYTE_BYTE_LEN, 0},          
	{0xfb, 0x8A, BYTE_BYTE_LEN, 0},          
	{0xfc, 0x90, BYTE_BYTE_LEN, 0},          
	{0xfd, 0x95, BYTE_BYTE_LEN, 0},          
	{0x03, 0xd0, BYTE_BYTE_LEN, 0},//Page d0 
	{0x10, 0x9A, BYTE_BYTE_LEN, 0},          
	{0x11, 0x9F, BYTE_BYTE_LEN, 0},          
	{0x12, 0xA9, BYTE_BYTE_LEN, 0},          
	{0x13, 0xb1, BYTE_BYTE_LEN, 0}, //0217 jk//0xB1, BYTE_BYTE_LEN, 0},          
	{0x14, 0xb9, BYTE_BYTE_LEN, 0}, //0217 jk//0xB9, BYTE_BYTE_LEN, 0},          
	{0x15, 0xc3, BYTE_BYTE_LEN, 0}, //0217 jk//0xC6, BYTE_BYTE_LEN, 0},          
	{0x16, 0xcb, BYTE_BYTE_LEN, 0}, //0217 jk//0xD0, BYTE_BYTE_LEN, 0},          
	{0x17, 0xd4, BYTE_BYTE_LEN, 0}, //0217 jk//0xD8, BYTE_BYTE_LEN, 0},          
	{0x18, 0xda, BYTE_BYTE_LEN, 0}, //0217 jk//0xDF, BYTE_BYTE_LEN, 0},          
	{0x19, 0xe1, BYTE_BYTE_LEN, 0}, //0217 jk//0xE6, BYTE_BYTE_LEN, 0},          
	{0x1a, 0xe7, BYTE_BYTE_LEN, 0}, //0217 jk//0xEC, BYTE_BYTE_LEN, 0},          
	{0x1b, 0xec, BYTE_BYTE_LEN, 0}, //0217 jk//0xF1, BYTE_BYTE_LEN, 0},          
	{0x1c, 0xf0, BYTE_BYTE_LEN, 0}, //0217 jk//0xF7, BYTE_BYTE_LEN, 0},          
	{0x1d, 0xf3, BYTE_BYTE_LEN, 0}, //0217 jk//0xFB, BYTE_BYTE_LEN, 0},          
	{0x1e, 0xf5, BYTE_BYTE_LEN, 0}, //0217 jk//0xFF, BYTE_BYTE_LEN, 0},
	           
	{0x1f, 0x00, BYTE_BYTE_LEN, 0},//GMA05
	{0x20, 0x03, BYTE_BYTE_LEN, 0},
	{0x21, 0x08, BYTE_BYTE_LEN, 0},
	{0x22, 0x12, BYTE_BYTE_LEN, 0},
	{0x23, 0x19, BYTE_BYTE_LEN, 0},
	{0x24, 0x25, BYTE_BYTE_LEN, 0},
	{0x25, 0x32, BYTE_BYTE_LEN, 0},
	{0x26, 0x3E, BYTE_BYTE_LEN, 0},
	{0x27, 0x4B, BYTE_BYTE_LEN, 0},
	{0x28, 0x56, BYTE_BYTE_LEN, 0},
	{0x29, 0x62, BYTE_BYTE_LEN, 0},
	{0x2a, 0x6A, BYTE_BYTE_LEN, 0},
	{0x2b, 0x71, BYTE_BYTE_LEN, 0},
	{0x2c, 0x78, BYTE_BYTE_LEN, 0},
	{0x2d, 0x7F, BYTE_BYTE_LEN, 0},
	{0x2e, 0x85, BYTE_BYTE_LEN, 0},
	{0x2f, 0x8A, BYTE_BYTE_LEN, 0},
	{0x30, 0x90, BYTE_BYTE_LEN, 0},
	{0x31, 0x95, BYTE_BYTE_LEN, 0},
	{0x32, 0x9A, BYTE_BYTE_LEN, 0},
	{0x33, 0x9F, BYTE_BYTE_LEN, 0},
	{0x34, 0xA9, BYTE_BYTE_LEN, 0},
	{0x35, 0xb1, BYTE_BYTE_LEN, 0}, //0217 jk//0xB1, BYTE_BYTE_LEN, 0},
	{0x36, 0xb9, BYTE_BYTE_LEN, 0}, //0217 jk//0xB9, BYTE_BYTE_LEN, 0},
	{0x37, 0xc3, BYTE_BYTE_LEN, 0}, //0217 jk//0xC6, BYTE_BYTE_LEN, 0},
	{0x38, 0xcb, BYTE_BYTE_LEN, 0}, //0217 jk//0xD0, BYTE_BYTE_LEN, 0},
	{0x39, 0xd4, BYTE_BYTE_LEN, 0}, //0217 jk//0xD8, BYTE_BYTE_LEN, 0},
	{0x3a, 0xda, BYTE_BYTE_LEN, 0}, //0217 jk//0xDF, BYTE_BYTE_LEN, 0},
	{0x3b, 0xe1, BYTE_BYTE_LEN, 0}, //0217 jk//0xE6, BYTE_BYTE_LEN, 0},
	{0x3c, 0xe7, BYTE_BYTE_LEN, 0}, //0217 jk//0xEC, BYTE_BYTE_LEN, 0},
	{0x3d, 0xec, BYTE_BYTE_LEN, 0}, //0217 jk//0xF1, BYTE_BYTE_LEN, 0},
	{0x3e, 0xf0, BYTE_BYTE_LEN, 0}, //0217 jk//0xF7, BYTE_BYTE_LEN, 0},
	{0x3f, 0xf3, BYTE_BYTE_LEN, 0}, //0217 jk//0xFB, BYTE_BYTE_LEN, 0},
	{0x40, 0xf5, BYTE_BYTE_LEN, 0}, //0217 jk//0xFF, BYTE_BYTE_LEN, 0},
	           
	{0x41, 0x00, BYTE_BYTE_LEN, 0},//GMA06
	{0x42, 0x03, BYTE_BYTE_LEN, 0},
	{0x43, 0x08, BYTE_BYTE_LEN, 0},
	{0x44, 0x12, BYTE_BYTE_LEN, 0},
	{0x45, 0x19, BYTE_BYTE_LEN, 0},
	{0x46, 0x25, BYTE_BYTE_LEN, 0},
	{0x47, 0x32, BYTE_BYTE_LEN, 0},
	{0x48, 0x3E, BYTE_BYTE_LEN, 0},
	{0x49, 0x4B, BYTE_BYTE_LEN, 0},
	{0x4a, 0x56, BYTE_BYTE_LEN, 0},
	{0x4b, 0x62, BYTE_BYTE_LEN, 0},
	{0x4c, 0x6A, BYTE_BYTE_LEN, 0},
	{0x4d, 0x71, BYTE_BYTE_LEN, 0},
	{0x4e, 0x78, BYTE_BYTE_LEN, 0},
	{0x4f, 0x7F, BYTE_BYTE_LEN, 0},
	{0x50, 0x85, BYTE_BYTE_LEN, 0},
	{0x51, 0x8A, BYTE_BYTE_LEN, 0},
	{0x52, 0x90, BYTE_BYTE_LEN, 0},
	{0x53, 0x95, BYTE_BYTE_LEN, 0},
	{0x54, 0x9A, BYTE_BYTE_LEN, 0},
	{0x55, 0x9F, BYTE_BYTE_LEN, 0},
	{0x56, 0xA9, BYTE_BYTE_LEN, 0},
	{0x57, 0xB1, BYTE_BYTE_LEN, 0},
	{0x58, 0xB9, BYTE_BYTE_LEN, 0},
	{0x59, 0xC6, BYTE_BYTE_LEN, 0},
	{0x5a, 0xD0, BYTE_BYTE_LEN, 0},
	{0x5b, 0xD8, BYTE_BYTE_LEN, 0},
	{0x5c, 0xDF, BYTE_BYTE_LEN, 0},
	{0x5d, 0xE6, BYTE_BYTE_LEN, 0},
	{0x5e, 0xEC, BYTE_BYTE_LEN, 0},
	{0x5f, 0xF1, BYTE_BYTE_LEN, 0},
	{0x60, 0xF7, BYTE_BYTE_LEN, 0},
	{0x61, 0xFB, BYTE_BYTE_LEN, 0},
	{0x62, 0xFF, BYTE_BYTE_LEN, 0},
	           
	{0x63, 0x00, BYTE_BYTE_LEN, 0},//GMA07
	{0x64, 0x03, BYTE_BYTE_LEN, 0},
	{0x65, 0x08, BYTE_BYTE_LEN, 0},
	{0x66, 0x12, BYTE_BYTE_LEN, 0},
	{0x67, 0x19, BYTE_BYTE_LEN, 0},
	{0x68, 0x25, BYTE_BYTE_LEN, 0},
	{0x69, 0x32, BYTE_BYTE_LEN, 0},
	{0x6a, 0x3E, BYTE_BYTE_LEN, 0},
	{0x6b, 0x4B, BYTE_BYTE_LEN, 0},
	{0x6c, 0x56, BYTE_BYTE_LEN, 0},
	{0x6d, 0x62, BYTE_BYTE_LEN, 0},
	{0x6e, 0x6A, BYTE_BYTE_LEN, 0},
	{0x6f, 0x71, BYTE_BYTE_LEN, 0},
	{0x70, 0x78, BYTE_BYTE_LEN, 0},
	{0x71, 0x7F, BYTE_BYTE_LEN, 0},
	{0x72, 0x85, BYTE_BYTE_LEN, 0},
	{0x73, 0x8A, BYTE_BYTE_LEN, 0},
	{0x74, 0x90, BYTE_BYTE_LEN, 0},
	{0x75, 0x95, BYTE_BYTE_LEN, 0},
	{0x76, 0x9A, BYTE_BYTE_LEN, 0},
	{0x77, 0x9F, BYTE_BYTE_LEN, 0},
	{0x78, 0xA9, BYTE_BYTE_LEN, 0},
	{0x79, 0xB1, BYTE_BYTE_LEN, 0},
	{0x7a, 0xB9, BYTE_BYTE_LEN, 0},
	{0x7b, 0xC6, BYTE_BYTE_LEN, 0},
	{0x7c, 0xD0, BYTE_BYTE_LEN, 0},
	{0x7d, 0xD8, BYTE_BYTE_LEN, 0},
	{0x7e, 0xDF, BYTE_BYTE_LEN, 0},
	{0x7f, 0xE6, BYTE_BYTE_LEN, 0},
	{0x80, 0xEC, BYTE_BYTE_LEN, 0},
	{0x81, 0xF1, BYTE_BYTE_LEN, 0},
	{0x82, 0xF7, BYTE_BYTE_LEN, 0},
	{0x83, 0xFB, BYTE_BYTE_LEN, 0},
	{0x84, 0xFF, BYTE_BYTE_LEN, 0},
	           
	{0x85, 0x00, BYTE_BYTE_LEN, 0},//GMA08
	{0x86, 0x03, BYTE_BYTE_LEN, 0},
	{0x87, 0x08, BYTE_BYTE_LEN, 0},
	{0x88, 0x12, BYTE_BYTE_LEN, 0},
	{0x89, 0x19, BYTE_BYTE_LEN, 0},
	{0x8a, 0x25, BYTE_BYTE_LEN, 0},
	{0x8b, 0x32, BYTE_BYTE_LEN, 0},
	{0x8c, 0x3E, BYTE_BYTE_LEN, 0},
	{0x8d, 0x4B, BYTE_BYTE_LEN, 0},
	{0x8e, 0x56, BYTE_BYTE_LEN, 0},
	{0x8f, 0x62, BYTE_BYTE_LEN, 0},
	{0x90, 0x6A, BYTE_BYTE_LEN, 0},
	{0x91, 0x71, BYTE_BYTE_LEN, 0},
	{0x92, 0x78, BYTE_BYTE_LEN, 0},
	{0x93, 0x7F, BYTE_BYTE_LEN, 0},
	{0x94, 0x85, BYTE_BYTE_LEN, 0},
	{0x95, 0x8A, BYTE_BYTE_LEN, 0},
	{0x96, 0x90, BYTE_BYTE_LEN, 0},
	{0x97, 0x95, BYTE_BYTE_LEN, 0},
	{0x98, 0x9A, BYTE_BYTE_LEN, 0},
	{0x99, 0x9F, BYTE_BYTE_LEN, 0},
	{0x9a, 0xA9, BYTE_BYTE_LEN, 0},
	{0x9b, 0xB1, BYTE_BYTE_LEN, 0},
	{0x9c, 0xB9, BYTE_BYTE_LEN, 0},
	{0x9d, 0xC6, BYTE_BYTE_LEN, 0},
	{0x9e, 0xD0, BYTE_BYTE_LEN, 0},
	{0x9f, 0xD8, BYTE_BYTE_LEN, 0},
	{0xa0, 0xDF, BYTE_BYTE_LEN, 0},
	{0xa1, 0xE6, BYTE_BYTE_LEN, 0},
	{0xa2, 0xEC, BYTE_BYTE_LEN, 0},
	{0xa3, 0xF1, BYTE_BYTE_LEN, 0},
	{0xa4, 0xF7, BYTE_BYTE_LEN, 0},
	{0xa5, 0xFB, BYTE_BYTE_LEN, 0},
	{0xa6, 0xFF, BYTE_BYTE_LEN, 0},
	           
	{0xa7, 0x00, BYTE_BYTE_LEN, 0},//GMA09
	{0xa8, 0x03, BYTE_BYTE_LEN, 0},
	{0xa9, 0x08, BYTE_BYTE_LEN, 0},
	{0xaa, 0x12, BYTE_BYTE_LEN, 0},
	{0xab, 0x19, BYTE_BYTE_LEN, 0},
	{0xac, 0x25, BYTE_BYTE_LEN, 0},
	{0xad, 0x32, BYTE_BYTE_LEN, 0},
	{0xae, 0x3E, BYTE_BYTE_LEN, 0},
	{0xaf, 0x4B, BYTE_BYTE_LEN, 0},
	{0xb0, 0x56, BYTE_BYTE_LEN, 0},
	{0xb1, 0x62, BYTE_BYTE_LEN, 0},
	{0xb2, 0x6A, BYTE_BYTE_LEN, 0},
	{0xb3, 0x71, BYTE_BYTE_LEN, 0},
	{0xb4, 0x78, BYTE_BYTE_LEN, 0},
	{0xb5, 0x7F, BYTE_BYTE_LEN, 0},
	{0xb6, 0x85, BYTE_BYTE_LEN, 0},
	{0xb7, 0x8A, BYTE_BYTE_LEN, 0},
	{0xb8, 0x90, BYTE_BYTE_LEN, 0},
	{0xb9, 0x95, BYTE_BYTE_LEN, 0},
	{0xba, 0x9A, BYTE_BYTE_LEN, 0},
	{0xbb, 0x9F, BYTE_BYTE_LEN, 0},
	{0xbc, 0xA9, BYTE_BYTE_LEN, 0},
	{0xbd, 0xB1, BYTE_BYTE_LEN, 0},
	{0xbe, 0xB9, BYTE_BYTE_LEN, 0},
	{0xbf, 0xC6, BYTE_BYTE_LEN, 0},
	{0xc0, 0xD0, BYTE_BYTE_LEN, 0},
	{0xc1, 0xD8, BYTE_BYTE_LEN, 0},
	{0xc2, 0xDF, BYTE_BYTE_LEN, 0},
	{0xc3, 0xE6, BYTE_BYTE_LEN, 0},
	{0xc4, 0xEC, BYTE_BYTE_LEN, 0},
	{0xc5, 0xF1, BYTE_BYTE_LEN, 0},
	{0xc6, 0xF7, BYTE_BYTE_LEN, 0},
	{0xc7, 0xFB, BYTE_BYTE_LEN, 0},
	{0xc8, 0xFF, BYTE_BYTE_LEN, 0},
	           
	{0xc9, 0x00, BYTE_BYTE_LEN, 0},//GMA10
	{0xca, 0x03, BYTE_BYTE_LEN, 0},
	{0xcb, 0x08, BYTE_BYTE_LEN, 0},
	{0xcc, 0x12, BYTE_BYTE_LEN, 0},
	{0xcd, 0x19, BYTE_BYTE_LEN, 0},
	{0xce, 0x25, BYTE_BYTE_LEN, 0},
	{0xcf, 0x32, BYTE_BYTE_LEN, 0},
	{0xd0, 0x3E, BYTE_BYTE_LEN, 0},
	{0xd1, 0x4B, BYTE_BYTE_LEN, 0},
	{0xd2, 0x56, BYTE_BYTE_LEN, 0},
	{0xd3, 0x62, BYTE_BYTE_LEN, 0},
	{0xd4, 0x6A, BYTE_BYTE_LEN, 0},
	{0xd5, 0x71, BYTE_BYTE_LEN, 0},
	{0xd6, 0x78, BYTE_BYTE_LEN, 0},
	{0xd7, 0x7F, BYTE_BYTE_LEN, 0},
	{0xd8, 0x85, BYTE_BYTE_LEN, 0},
	{0xd9, 0x8A, BYTE_BYTE_LEN, 0},
	{0xda, 0x90, BYTE_BYTE_LEN, 0},
	{0xdb, 0x95, BYTE_BYTE_LEN, 0},
	{0xdc, 0x9A, BYTE_BYTE_LEN, 0},
	{0xdd, 0x9F, BYTE_BYTE_LEN, 0},
	{0xde, 0xA9, BYTE_BYTE_LEN, 0},
	{0xdf, 0xB1, BYTE_BYTE_LEN, 0},
	{0xe0, 0xB9, BYTE_BYTE_LEN, 0},
	{0xe1, 0xC6, BYTE_BYTE_LEN, 0},
	{0xe2, 0xD0, BYTE_BYTE_LEN, 0},
	{0xe3, 0xD8, BYTE_BYTE_LEN, 0},
	{0xe4, 0xDF, BYTE_BYTE_LEN, 0},
	{0xe5, 0xE6, BYTE_BYTE_LEN, 0},
	{0xe6, 0xEC, BYTE_BYTE_LEN, 0},
	{0xe7, 0xF1, BYTE_BYTE_LEN, 0},
	{0xe8, 0xF7, BYTE_BYTE_LEN, 0},
	{0xe9, 0xFB, BYTE_BYTE_LEN, 0},
	{0xea, 0xFF, BYTE_BYTE_LEN, 0},
	           
	{0xeb, 0x00, BYTE_BYTE_LEN, 0},//GMA11  
	{0xec, 0x03, BYTE_BYTE_LEN, 0},         
	{0xed, 0x08, BYTE_BYTE_LEN, 0},         
	{0xee, 0x12, BYTE_BYTE_LEN, 0},         
	{0xef, 0x19, BYTE_BYTE_LEN, 0},         
	{0xf0, 0x25, BYTE_BYTE_LEN, 0},         
	{0xf1, 0x32, BYTE_BYTE_LEN, 0},         
	{0xf2, 0x3E, BYTE_BYTE_LEN, 0},         
	{0xf3, 0x4B, BYTE_BYTE_LEN, 0},         
	{0xf4, 0x56, BYTE_BYTE_LEN, 0},         
	{0xf5, 0x62, BYTE_BYTE_LEN, 0},         
	{0xf6, 0x6A, BYTE_BYTE_LEN, 0},         
	{0xf7, 0x71, BYTE_BYTE_LEN, 0},         
	{0xf8, 0x78, BYTE_BYTE_LEN, 0},         
	{0xf9, 0x7F, BYTE_BYTE_LEN, 0},         
	{0xfa, 0x85, BYTE_BYTE_LEN, 0},         
	{0xfb, 0x8A, BYTE_BYTE_LEN, 0},         
	{0xfc, 0x90, BYTE_BYTE_LEN, 0},         
	{0xfd, 0x95, BYTE_BYTE_LEN, 0},         
	{0x03, 0xd1, BYTE_BYTE_LEN, 0},//Page d1
	{0x10, 0x9A, BYTE_BYTE_LEN, 0},         
	{0x11, 0x9F, BYTE_BYTE_LEN, 0},         
	{0x12, 0xA9, BYTE_BYTE_LEN, 0},         
	{0x13, 0xB1, BYTE_BYTE_LEN, 0},         
	{0x14, 0xB9, BYTE_BYTE_LEN, 0},         
	{0x15, 0xC6, BYTE_BYTE_LEN, 0},         
	{0x16, 0xD0, BYTE_BYTE_LEN, 0},         
	{0x17, 0xD8, BYTE_BYTE_LEN, 0},         
	{0x18, 0xDF, BYTE_BYTE_LEN, 0},         
	{0x19, 0xE6, BYTE_BYTE_LEN, 0},         
	{0x1a, 0xEC, BYTE_BYTE_LEN, 0},         
	{0x1b, 0xF1, BYTE_BYTE_LEN, 0},         
	{0x1c, 0xF7, BYTE_BYTE_LEN, 0},         
	{0x1d, 0xFB, BYTE_BYTE_LEN, 0},         
	{0x1e, 0xFF, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// D1 Page Adaptive Y Target delta
	///////////////////////////////////////////
	{0x1f, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 0 
	{0x20, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 1
	{0x21, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 2
	{0x22, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 3
	{0x23, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 4
	{0x24, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 5
	{0x25, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 6
	{0x26, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 7
	{0x27, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 8
	{0x28, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 9
	{0x29, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 10
	{0x2a, 0x80, BYTE_BYTE_LEN, 0},//Y target delta 11
	///////////////////////////////////////////
	// D1 Page Adaptive R/B saturation
	///////////////////////////////////////////
	{0x2b, 0x80, BYTE_BYTE_LEN, 0}, //0x7a, BYTE_BYTE_LEN, 0}, //0x80, BYTE_BYTE_LEN, 0}, //28 set//0x90, BYTE_BYTE_LEN, 0}, //0xa0, BYTE_BYTE_LEN, 0},//0x80, BYTE_BYTE_LEN, 0}, //0220//0x90, BYTE_BYTE_LEN, 0},//SATB_00
	{0x2c, 0x80, BYTE_BYTE_LEN, 0}, //0x7a, BYTE_BYTE_LEN, 0}, //0x80, BYTE_BYTE_LEN, 0}, //28 set//0x90, BYTE_BYTE_LEN, 0}, //0xa0, BYTE_BYTE_LEN, 0},//0x80, BYTE_BYTE_LEN, 0}, //0220//0x90, BYTE_BYTE_LEN, 0},//SATB_01
	{0x2d, 0x80, BYTE_BYTE_LEN, 0}, //0x7a, BYTE_BYTE_LEN, 0}, //0x80, BYTE_BYTE_LEN, 0}, //28 set//0x90, BYTE_BYTE_LEN, 0}, //0xa0, BYTE_BYTE_LEN, 0},//0x80, BYTE_BYTE_LEN, 0}, //0220//0x90, BYTE_BYTE_LEN, 0},//SATB_02
	{0x2e, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0}, //jk0130//0x90, BYTE_BYTE_LEN, 0},//SATB_03
	{0x2f, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0}, //jk0130//0x90, BYTE_BYTE_LEN, 0},//SATB_04
	{0x30, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0}, //jk0130//0x90, BYTE_BYTE_LEN, 0},//SATB_05
	{0x31, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//120106 for QC//0x88, BYTE_BYTE_LEN, 0},//SATB_06
	{0x32, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//120106 for QC//0x88, BYTE_BYTE_LEN, 0},//SATB_07
	{0x33, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//120106 for QC//0x88, BYTE_BYTE_LEN, 0},//SATB_08
	{0x34, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//SATB_09
	{0x35, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//SATB_10
	{0x36, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//SATB_11 
	
	//Cr		

	{0x37, 0x80, BYTE_BYTE_LEN, 0}, //0x7a, BYTE_BYTE_LEN, 0}, //0x80, BYTE_BYTE_LEN, 0}, //28 set//0x90, BYTE_BYTE_LEN, 0}, //0xa0, BYTE_BYTE_LEN, 0},//0x80, BYTE_BYTE_LEN, 0}, //0220//0x90, BYTE_BYTE_LEN, 0},//SATR_00
	{0x38, 0x80, BYTE_BYTE_LEN, 0}, //0x7a, BYTE_BYTE_LEN, 0}, //0x80, BYTE_BYTE_LEN, 0}, //28 set//0x90, BYTE_BYTE_LEN, 0}, //0xa0, BYTE_BYTE_LEN, 0},//0x80, BYTE_BYTE_LEN, 0}, //0220//0x90, BYTE_BYTE_LEN, 0},//SATR_01
	{0x39, 0x80, BYTE_BYTE_LEN, 0}, //0x7a, BYTE_BYTE_LEN, 0}, //0x80, BYTE_BYTE_LEN, 0}, //28 set//0x90, BYTE_BYTE_LEN, 0}, //0xa0, BYTE_BYTE_LEN, 0},//0x80, BYTE_BYTE_LEN, 0}, //0220//0x90, BYTE_BYTE_LEN, 0},//SATR_02
	{0x3a, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0}, //jk0130//0x90, BYTE_BYTE_LEN, 0},//SATR_03
	{0x3b, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0}, //jk0130//0x90, BYTE_BYTE_LEN, 0},//SATR_04
	{0x3c, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0}, //jk0130//0x90, BYTE_BYTE_LEN, 0},//SATR_05
	{0x3d, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//120106 for QC//0x88, BYTE_BYTE_LEN, 0},//SATR_06
	{0x3e, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//120106 for QC//0x88, BYTE_BYTE_LEN, 0},//SATR_07
	{0x3f, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//120106 for QC//0x88, BYTE_BYTE_LEN, 0},//SATR_08
	{0x40, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//SATR_09
	{0x41, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//SATR_10
	{0x42, 0x90, BYTE_BYTE_LEN, 0}, //0x8a, BYTE_BYTE_LEN, 0}, //0x90, BYTE_BYTE_LEN, 0}, //28 set//0xb0, BYTE_BYTE_LEN, 0},//SATR_11

	
	///////////////////////////////////////////
	// D1 Page Adaptive CMC
	///////////////////////////////////////////
	
	{0x43, 0x2f, BYTE_BYTE_LEN, 0},//CMC_00
	{0x44, 0x74, BYTE_BYTE_LEN, 0},
	{0x45, 0x3f, BYTE_BYTE_LEN, 0},
	{0x46, 0x0b, BYTE_BYTE_LEN, 0},
	{0x47, 0x1c, BYTE_BYTE_LEN, 0},
	{0x48, 0x76, BYTE_BYTE_LEN, 0},
	{0x49, 0x1a, BYTE_BYTE_LEN, 0},
	{0x4a, 0x04, BYTE_BYTE_LEN, 0},
	{0x4b, 0x2f, BYTE_BYTE_LEN, 0},
	{0x4c, 0x73, BYTE_BYTE_LEN, 0},
	           
	{0x4d, 0x2f, BYTE_BYTE_LEN, 0},//CMC_01
	{0x4e, 0x74, BYTE_BYTE_LEN, 0},
	{0x4f, 0x3f, BYTE_BYTE_LEN, 0},
	{0x50, 0x0b, BYTE_BYTE_LEN, 0},
	{0x51, 0x1c, BYTE_BYTE_LEN, 0},
	{0x52, 0x76, BYTE_BYTE_LEN, 0},
	{0x53, 0x1a, BYTE_BYTE_LEN, 0},
	{0x54, 0x04, BYTE_BYTE_LEN, 0},
	{0x55, 0x2f, BYTE_BYTE_LEN, 0},
	{0x56, 0x73, BYTE_BYTE_LEN, 0},
	           
	{0x57, 0x2f, BYTE_BYTE_LEN, 0},//CMC_02
	{0x58, 0x74, BYTE_BYTE_LEN, 0},
	{0x59, 0x3f, BYTE_BYTE_LEN, 0},
	{0x5a, 0x0b, BYTE_BYTE_LEN, 0},
	{0x5b, 0x1c, BYTE_BYTE_LEN, 0},
	{0x5c, 0x76, BYTE_BYTE_LEN, 0},
	{0x5d, 0x1a, BYTE_BYTE_LEN, 0},
	{0x5e, 0x04, BYTE_BYTE_LEN, 0},
	{0x5f, 0x2f, BYTE_BYTE_LEN, 0},
	{0x60, 0x73, BYTE_BYTE_LEN, 0},
	           
	{0x61, 0x2f, BYTE_BYTE_LEN, 0},//CMC_03
	{0x62, 0x6a, BYTE_BYTE_LEN, 0},
	{0x63, 0x32, BYTE_BYTE_LEN, 0},
	{0x64, 0x08, BYTE_BYTE_LEN, 0},
	{0x65, 0x1a, BYTE_BYTE_LEN, 0},
	{0x66, 0x6c, BYTE_BYTE_LEN, 0},
	{0x67, 0x12, BYTE_BYTE_LEN, 0},
	{0x68, 0x03, BYTE_BYTE_LEN, 0},
	{0x69, 0x30, BYTE_BYTE_LEN, 0},
	{0x6a, 0x73, BYTE_BYTE_LEN, 0},
	           
	{0x6b, 0x2f, BYTE_BYTE_LEN, 0},//CMC_04
	{0x6c, 0x65, BYTE_BYTE_LEN, 0},
	{0x6d, 0x2b, BYTE_BYTE_LEN, 0},
	{0x6e, 0x06, BYTE_BYTE_LEN, 0},
	{0x6f, 0x19, BYTE_BYTE_LEN, 0},
	{0x70, 0x6c, BYTE_BYTE_LEN, 0},
	{0x71, 0x13, BYTE_BYTE_LEN, 0},
	{0x72, 0x09, BYTE_BYTE_LEN, 0},
	{0x73, 0x2a, BYTE_BYTE_LEN, 0},
	{0x74, 0x73, BYTE_BYTE_LEN, 0},
	           
	{0x75, 0x2f, BYTE_BYTE_LEN, 0},//CMC_05
	{0x76, 0x65, BYTE_BYTE_LEN, 0},
	{0x77, 0x2b, BYTE_BYTE_LEN, 0},
	{0x78, 0x06, BYTE_BYTE_LEN, 0},
	{0x79, 0x19, BYTE_BYTE_LEN, 0},
	{0x7a, 0x6c, BYTE_BYTE_LEN, 0},
	{0x7b, 0x13, BYTE_BYTE_LEN, 0},
	{0x7c, 0x09, BYTE_BYTE_LEN, 0},
	{0x7d, 0x2a, BYTE_BYTE_LEN, 0},
	{0x7e, 0x73, BYTE_BYTE_LEN, 0},
	           
	{0x7f, 0x2f, BYTE_BYTE_LEN, 0},//CMC_06
	{0x80, 0x6a, BYTE_BYTE_LEN, 0},
	{0x81, 0x32, BYTE_BYTE_LEN, 0},
	{0x82, 0x08, BYTE_BYTE_LEN, 0},
	{0x83, 0x1a, BYTE_BYTE_LEN, 0},
	{0x84, 0x6c, BYTE_BYTE_LEN, 0},
	{0x85, 0x12, BYTE_BYTE_LEN, 0},
	{0x86, 0x03, BYTE_BYTE_LEN, 0},
	{0x87, 0x30, BYTE_BYTE_LEN, 0},
	{0x88, 0x73, BYTE_BYTE_LEN, 0},
	           
	{0x89, 0x2f, BYTE_BYTE_LEN, 0},//CMC_07
	{0x8a, 0x6a, BYTE_BYTE_LEN, 0},
	{0x8b, 0x32, BYTE_BYTE_LEN, 0},
	{0x8c, 0x08, BYTE_BYTE_LEN, 0},
	{0x8d, 0x1a, BYTE_BYTE_LEN, 0},
	{0x8e, 0x6c, BYTE_BYTE_LEN, 0},
	{0x8f, 0x12, BYTE_BYTE_LEN, 0},
	{0x90, 0x03, BYTE_BYTE_LEN, 0},
	{0x91, 0x30, BYTE_BYTE_LEN, 0},
	{0x92, 0x73, BYTE_BYTE_LEN, 0},
	           

	{0x93, 0x2f, BYTE_BYTE_LEN, 0},//CMC_08
	{0x94, 0x6a, BYTE_BYTE_LEN, 0},
	{0x95, 0x32, BYTE_BYTE_LEN, 0},
	{0x96, 0x08, BYTE_BYTE_LEN, 0},
	{0x97, 0x1a, BYTE_BYTE_LEN, 0},
	{0x98, 0x6c, BYTE_BYTE_LEN, 0},
	{0x99, 0x12, BYTE_BYTE_LEN, 0},
	{0x9a, 0x03, BYTE_BYTE_LEN, 0},
	{0x9b, 0x30, BYTE_BYTE_LEN, 0},
	{0x9c, 0x73, BYTE_BYTE_LEN, 0},

	           
	{0x9d, 0x2f, BYTE_BYTE_LEN, 0},//CMC_09
	{0x9e, 0x6a, BYTE_BYTE_LEN, 0},
	{0x9f, 0x32, BYTE_BYTE_LEN, 0},
	{0xa0, 0x08, BYTE_BYTE_LEN, 0},
	{0xa1, 0x1a, BYTE_BYTE_LEN, 0},
	{0xa2, 0x6c, BYTE_BYTE_LEN, 0},
	{0xa3, 0x12, BYTE_BYTE_LEN, 0},
	{0xa4, 0x03, BYTE_BYTE_LEN, 0},
	{0xa5, 0x30, BYTE_BYTE_LEN, 0},
	{0xa6, 0x73, BYTE_BYTE_LEN, 0},
	           
	{0xa7, 0x2f, BYTE_BYTE_LEN, 0},//CMC_10
	{0xa8, 0x6a, BYTE_BYTE_LEN, 0},
	{0xa9, 0x32, BYTE_BYTE_LEN, 0},
	{0xaa, 0x08, BYTE_BYTE_LEN, 0},
	{0xab, 0x1a, BYTE_BYTE_LEN, 0},
	{0xac, 0x6c, BYTE_BYTE_LEN, 0},
	{0xad, 0x12, BYTE_BYTE_LEN, 0},
	{0xae, 0x03, BYTE_BYTE_LEN, 0},
	{0xaf, 0x30, BYTE_BYTE_LEN, 0},
	{0xb0, 0x73, BYTE_BYTE_LEN, 0},
	           
	{0xb1, 0x2f, BYTE_BYTE_LEN, 0},//CMC_11
	{0xb2, 0x6a, BYTE_BYTE_LEN, 0},
	{0xb3, 0x32, BYTE_BYTE_LEN, 0},
	{0xb4, 0x08, BYTE_BYTE_LEN, 0},
	{0xb5, 0x1a, BYTE_BYTE_LEN, 0},
	{0xb6, 0x6c, BYTE_BYTE_LEN, 0},
	{0xb7, 0x12, BYTE_BYTE_LEN, 0},
	{0xb8, 0x03, BYTE_BYTE_LEN, 0},
	{0xb9, 0x30, BYTE_BYTE_LEN, 0},
	{0xba, 0x73, BYTE_BYTE_LEN, 0},
	
		///////////////////////////////////////////
	// D1~D2~D3 Page Adaptive Multi-CMC
	/////////////////////////////////////////// 		  
	//MCMC_00
	{0xbb, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0xbc, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE  
	{0xbd, 0x73, BYTE_BYTE_LEN, 0},//0_GAIN   //115 //R1 //Red
	{0xbe, 0x81, BYTE_BYTE_LEN, 0},//0_HUE	  //-1
	{0xbf, 0x35, BYTE_BYTE_LEN, 0},//0_CENTER //53
	{0xc0, 0x1a, BYTE_BYTE_LEN, 0},//0_DELTA  //26
	{0xc1, 0x6c, BYTE_BYTE_LEN, 0},//1_GAIN   //108 //r2 //Green
	{0xc2, 0x12, BYTE_BYTE_LEN, 0},//1_HUE	  //+18 
	{0xc3, 0x74, BYTE_BYTE_LEN, 0}, //jktest 0202//116//0x76, BYTE_BYTE_LEN, 0},//1_CENTER //118 
	{0xc4, 0x21, BYTE_BYTE_LEN, 0}, //jktest 0202//33//0x34, BYTE_BYTE_LEN, 0},//1_DELTA  //52
	{0xc5, 0x88, BYTE_BYTE_LEN, 0},//2_GAIN   //136 //r3 //Blue
	{0xc6, 0x8c, BYTE_BYTE_LEN, 0},//2_HUE	  //-12
	{0xc7, 0xa9, BYTE_BYTE_LEN, 0},//2_CENTER //169
	{0xc8, 0x10, BYTE_BYTE_LEN, 0},//2_DELTA  //16
	{0xc9, 0x90, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0},//3_GAIN   //87 //r4 //yellow
	{0xca, 0x20, BYTE_BYTE_LEN, 0},//0219//0x03, BYTE_BYTE_LEN, 0},//0x88, BYTE_BYTE_LEN, 0},//3_HUE	  //-8
	{0xcb, 0x4a, BYTE_BYTE_LEN, 0}, //jktest 0202//74//0x52, BYTE_BYTE_LEN, 0},//3_CENTER //82
	{0xcc, 0x0b, BYTE_BYTE_LEN, 0}, //jktest 0202//11//0x16, BYTE_BYTE_LEN, 0},//3_DELTA  //22
	{0xcd, 0x80, BYTE_BYTE_LEN, 0}, //0219//0x90, BYTE_BYTE_LEN, 0},//0x8a, BYTE_BYTE_LEN, 0},//t//0x80, BYTE_BYTE_LEN, 0},//4_GAIN   //128 //r5 //tree green indoor
	{0xce, 0x0c, BYTE_BYTE_LEN, 0}, //0219//0x65, BYTE_BYTE_LEN, 0},//t//0x30, BYTE_BYTE_LEN, 0},//t//0x20, BYTE_BYTE_LEN, 0},//t//0x0c, BYTE_BYTE_LEN, 0},//4_HUE	  //12
	{0xcf, 0x5b, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0}, //jktest 0202//87//0x76, BYTE_BYTE_LEN, 0},//4_CENTER //118
	{0xd0, 0x16, BYTE_BYTE_LEN, 0},//t//0x0d, BYTE_BYTE_LEN, 0}, //jktest 0202//12//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA  //28
	{0xd1, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN   //178 //r6 //No use
	{0xd2, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE	  //-10
	{0xd3, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER //82
	{0xd4, 0x1c, BYTE_BYTE_LEN, 0},//5_DELTA  //28
	//MCMC_01  
	{0xd5, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0xd6, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0xd7, 0x66, BYTE_BYTE_LEN, 0},//0_GAIN  //102////r1
	{0xd8, 0x00, BYTE_BYTE_LEN, 0},//0_HUE   //00
	{0xd9, 0x35, BYTE_BYTE_LEN, 0},//0_CENTER //53
	{0xda, 0x13, BYTE_BYTE_LEN, 0},//0_DELTA //19
	{0xdb, 0x67, BYTE_BYTE_LEN, 0},//1_GAIN  //103 //r2
	{0xdc, 0x04, BYTE_BYTE_LEN, 0},//1_HUE	 //04
	{0xdd, 0x6e, BYTE_BYTE_LEN, 0},//1_CENTER //110
	{0xde, 0x16, BYTE_BYTE_LEN, 0},//jktest 0202//22//0x1c, BYTE_BYTE_LEN, 0},//1_DELTA //28
	{0xdf, 0x67, BYTE_BYTE_LEN, 0},//2_GAIN  //103 //r3
	{0xe0, 0x8f, BYTE_BYTE_LEN, 0},//2_HUE	 //-15
	{0xe1, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER //175
	{0xe2, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA //28
	{0xe3, 0x9a, BYTE_BYTE_LEN, 0},//3_GAIN  //154//r4
	{0xe4, 0x86, BYTE_BYTE_LEN, 0},//3_HUE	 //-6 
	{0xe5, 0x4a, BYTE_BYTE_LEN, 0},//jktest 0202//74//0x52, BYTE_BYTE_LEN, 0},//3_CENTER //82
	{0xe6, 0x0a, BYTE_BYTE_LEN, 0},//jktest 0202//10//0x1c, BYTE_BYTE_LEN, 0},//3_DELTA //28
	{0xe7, 0x90, BYTE_BYTE_LEN, 0},//4_GAIN  //144//r5	   
	{0xe8, 0x20, BYTE_BYTE_LEN, 0},//t//0x94, BYTE_BYTE_LEN, 0},//4_HUE	 //-20
	{0xe9, 0x5b, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0},//jktest 0202//87//0x93, BYTE_BYTE_LEN, 0},//4_CENTER //147
	{0xea, 0x16, BYTE_BYTE_LEN, 0},//t//0x0c, BYTE_BYTE_LEN, 0},//jktest 0202//12//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA //28
	{0xeb, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN  //178//r6
	{0xec, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE	 //-10
	{0xed, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER//82
	{0xee, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA //29
	//MCMC_02  
	{0xef, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0xf0, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0xf1, 0x66, BYTE_BYTE_LEN, 0},//0_GAIN  
	{0xf2, 0x00, BYTE_BYTE_LEN, 0},//0_HUE
	{0xf3, 0x35, BYTE_BYTE_LEN, 0},//0_CENTER
	{0xf4, 0x13, BYTE_BYTE_LEN, 0},//0_DELTA
	{0xf5, 0x67, BYTE_BYTE_LEN, 0},//1_GAIN  
	{0xf6, 0x04, BYTE_BYTE_LEN, 0},//1_HUE	 
	{0xf7, 0x6e, BYTE_BYTE_LEN, 0},//1_CENTER
	{0xf8, 0x1c, BYTE_BYTE_LEN, 0},//1_DELTA
	{0xf9, 0x67, BYTE_BYTE_LEN, 0},//2_GAIN  
	{0xfa, 0x92, BYTE_BYTE_LEN, 0},//2_HUE	 
	{0xfb, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0xfc, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0xfd, 0x9a, BYTE_BYTE_LEN, 0},//3_GAIN  

	{0x03, 0xd2, BYTE_BYTE_LEN, 0},//Page d2
	{0x10, 0x86, BYTE_BYTE_LEN, 0},//3_HUE	 
	{0x11, 0x35, BYTE_BYTE_LEN, 0},//jktest 0202//0x52, BYTE_BYTE_LEN, 0},//3_CENTER //R4
	{0x12, 0x13, BYTE_BYTE_LEN, 0},//jktest 0202//0x1c, BYTE_BYTE_LEN, 0},//3_DELTA
	{0x13, 0x90, BYTE_BYTE_LEN, 0},//4_GAIN  
	{0x14, 0x94, BYTE_BYTE_LEN, 0},//4_HUE	 
	{0x15, 0x5b, BYTE_BYTE_LEN, 0},//t//0x6e, BYTE_BYTE_LEN, 0},//jktest 0202//0x93, BYTE_BYTE_LEN, 0},//4_CENTER //R5
	{0x16, 0x16, BYTE_BYTE_LEN, 0},//t//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0x17, 0x35, BYTE_BYTE_LEN, 0},//jktest 0202//0xb2, BYTE_BYTE_LEN, 0},//5_GAIN  
	{0x18, 0x13, BYTE_BYTE_LEN, 0},//jktest 0202//0x8a, BYTE_BYTE_LEN, 0},//5_HUE	 
	{0x19, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0x1a, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	//MCMC_03  
	{0x1b, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN                                  {0x1b, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0x1c, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE                                   {0x1c, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0x1d, 0x73, BYTE_BYTE_LEN, 0},//0_GAIN   //115 //R1                       {0x1d, 0x70, BYTE_BYTE_LEN, 0},//0_GAIN
	{0x1e, 0x81, BYTE_BYTE_LEN, 0},//0_HUE	  //-1                             {0x1e, 0x04, BYTE_BYTE_LEN, 0},//0_HUE
	{0x1f, 0x35, BYTE_BYTE_LEN, 0},//0_CENTER //53                             {0x1f, 0x36, BYTE_BYTE_LEN, 0},//0_CENTER
	{0x20, 0x1a, BYTE_BYTE_LEN, 0},//0_DELTA  //26                             {0x20, 0x0d, BYTE_BYTE_LEN, 0},//0_DELTA
	{0x21, 0x6c, BYTE_BYTE_LEN, 0},//1_GAIN   //108 //r2                       {0x21, 0xa0, BYTE_BYTE_LEN, 0},//1_GAIN
	{0x22, 0x12, BYTE_BYTE_LEN, 0},//1_HUE	  //+18                            {0x22, 0x08, BYTE_BYTE_LEN, 0},//1_HUE
	{0x23, 0x74, BYTE_BYTE_LEN, 0}, //jktest 0202//116//0x76, BYTE_BYTE_LEN, 0},//1_CENTER //118 {0x23, 0x76, BYTE_BYTE_LEN, 0},//1_CENTER
	{0x24, 0x21, BYTE_BYTE_LEN, 0}, //jktest 0202//33//0x34, BYTE_BYTE_LEN, 0},//1_DELTA  //52   {0x24, 0x10, BYTE_BYTE_LEN, 0},//1_DELTA
	{0x25, 0x88, BYTE_BYTE_LEN, 0},//2_GAIN   //136 //r3                       {0x25, 0x80, BYTE_BYTE_LEN, 0},//2_GAIN
	{0x26, 0x8c, BYTE_BYTE_LEN, 0},//2_HUE	  //-12                            {0x26, 0x00, BYTE_BYTE_LEN, 0},//2_HUE
	{0x27, 0xa9, BYTE_BYTE_LEN, 0},//2_CENTER //169                            {0x27, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0x28, 0x10, BYTE_BYTE_LEN, 0},//2_DELTA  //16                             {0x28, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0x29, 0x90, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0},//3_GAIN   //87 //r4                        {0x29, 0x80, BYTE_BYTE_LEN, 0},//3_GAIN
	{0x2a, 0x20, BYTE_BYTE_LEN, 0},//0219//0x30, BYTE_BYTE_LEN, 0},//t//0x88, BYTE_BYTE_LEN, 0},//3_HUE	  //-8                             {0x2a, 0x87, BYTE_BYTE_LEN, 0},//3_HUE
	{0x2b, 0x4a, BYTE_BYTE_LEN, 0}, //jktest 0202//74//0x52, BYTE_BYTE_LEN, 0},//3_CENTER //82   {0x2b, 0x51, BYTE_BYTE_LEN, 0},//3_CENTER
	{0x2c, 0x0b, BYTE_BYTE_LEN, 0}, //jktest 0202//11//0x16, BYTE_BYTE_LEN, 0},//3_DELTA  //22   {0x2c, 0x1c, BYTE_BYTE_LEN, 0},//3_DELTA
	{0x2d, 0x80, BYTE_BYTE_LEN, 0},//0219//0x90, BYTE_BYTE_LEN, 0},//t//0x8a, BYTE_BYTE_LEN, 0},//t//0x80, BYTE_BYTE_LEN, 0},//4_GAIN   //128 //r5                       {0x2d, 0x80, BYTE_BYTE_LEN, 0},//4_GAIN
	{0x2e, 0x0c, BYTE_BYTE_LEN, 0},//0219//0x65, BYTE_BYTE_LEN, 0},//t//0x30, BYTE_BYTE_LEN, 0},//t//0x20, BYTE_BYTE_LEN, 0},//t//0x0c, BYTE_BYTE_LEN, 0},//4_HUE	  //12                             {0x2e, 0x0c, BYTE_BYTE_LEN, 0},//4_HUE
	{0x2f, 0x5b, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0}, //jktest 0202//87//0x76, BYTE_BYTE_LEN, 0},//4_CENTER //118  {0x2f, 0x76, BYTE_BYTE_LEN, 0},//4_CENTER
	{0x30, 0x16, BYTE_BYTE_LEN, 0},//t//0x0d, BYTE_BYTE_LEN, 0}, //jktest 0202//12//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA  //28   {0x30, 0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0x31, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN   //178 //r6                       {0x31, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN
	{0x32, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE	  //-10                            {0x32, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE
	{0x33, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER //82                             {0x33, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0x34, 0x1c, BYTE_BYTE_LEN, 0},//5_DELTA  //28                             {0x34, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	//MCMC_04  
	{0x35, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0x36, 0x82, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0x37, 0x80, BYTE_BYTE_LEN, 0},//0_GAIN //r1
	{0x38, 0x80, BYTE_BYTE_LEN, 0},//0_HUE
	{0x39, 0x36, BYTE_BYTE_LEN, 0},//0_CENTER
	{0x3a, 0x0a, BYTE_BYTE_LEN, 0},//0_DELTA
	{0x3b, 0x80, BYTE_BYTE_LEN, 0},//1_GAIN //r2
	{0x3c, 0x10, BYTE_BYTE_LEN, 0},//1_HUE
	{0x3d, 0x80, BYTE_BYTE_LEN, 0},//1_CENTER
	{0x3e, 0x16, BYTE_BYTE_LEN, 0},//jktest 0202//22//0x0a, BYTE_BYTE_LEN, 0},//1_DELTA
	{0x3f, 0x80, BYTE_BYTE_LEN, 0},//2_GAIN //r3
	{0x40, 0x00, BYTE_BYTE_LEN, 0},//2_HUE
	{0x41, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0x42, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0x43, 0x80, BYTE_BYTE_LEN, 0},//3_GAIN //r4
	{0x44, 0x88, BYTE_BYTE_LEN, 0},//3_HUE
	{0x45, 0x4a, BYTE_BYTE_LEN, 0},//jktest 0202//74//0x51, BYTE_BYTE_LEN, 0},//3_CENTER
	{0x46, 0x0a, BYTE_BYTE_LEN, 0},//jktest 0202//10//0x1c, BYTE_BYTE_LEN, 0},//3_DELTA
	{0x47, 0x90, BYTE_BYTE_LEN, 0},//t//0x80, BYTE_BYTE_LEN, 0},//4_GAIN //r5
	{0x48, 0x0c, BYTE_BYTE_LEN, 0},//0219 tt//0x60, BYTE_BYTE_LEN, 0}, //0219 tt//0x20, BYTE_BYTE_LEN, 0},//t//0x0c, BYTE_BYTE_LEN, 0},//4_HUE
	{0x49, 0x5b, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0},//jktest 0202//87//0x76, BYTE_BYTE_LEN, 0},//4_CENTER
	{0x4a, 0x16, BYTE_BYTE_LEN, 0},//t//0x0c, BYTE_BYTE_LEN, 0},//jktest 0202//12//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0x4b, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN //r6
	{0x4c, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE
	{0x4d, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0x4e, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	//MCMC_05  
	{0x4f, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0x50, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0x51, 0x80, BYTE_BYTE_LEN, 0},//0_GAIN  //R1
	{0x52, 0x80, BYTE_BYTE_LEN, 0},//0_HUE
	{0x53, 0x36, BYTE_BYTE_LEN, 0},//0_CENTER
	{0x54, 0x0a, BYTE_BYTE_LEN, 0},//0_DELTA
	{0x55, 0x80, BYTE_BYTE_LEN, 0},//1_GAIN  //R2
	{0x56, 0x10, BYTE_BYTE_LEN, 0},//1_HUE
	{0x57, 0x80, BYTE_BYTE_LEN, 0},//1_CENTER
	{0x58, 0x0a, BYTE_BYTE_LEN, 0},//1_DELTA
	{0x59, 0x80, BYTE_BYTE_LEN, 0},//2_GAIN  //R3
	{0x5a, 0x00, BYTE_BYTE_LEN, 0},//2_HUE
	{0x5b, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0x5c, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0x5d, 0x80, BYTE_BYTE_LEN, 0},//3_GAIN  //R4
	{0x5e, 0x88, BYTE_BYTE_LEN, 0},//3_HUE
	{0x5f, 0x35, BYTE_BYTE_LEN, 0},//jktest 0202//0x51, BYTE_BYTE_LEN, 0},//3_CENTER
	{0x60, 0x13, BYTE_BYTE_LEN, 0},//jktest 0202//0x1c, BYTE_BYTE_LEN, 0},//3_DELTA
	{0x61, 0x80, BYTE_BYTE_LEN, 0},//4_GAIN  //R5
	{0x62, 0x88, BYTE_BYTE_LEN, 0},//jktest 0202//0x0c, BYTE_BYTE_LEN, 0},//4_HUE
	{0x63, 0x5b, BYTE_BYTE_LEN, 0},//t//0x6e, BYTE_BYTE_LEN, 0},//jktest 0202//0x76, BYTE_BYTE_LEN, 0},//4_CENTER
	{0x64, 0x16, BYTE_BYTE_LEN, 0},//t//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0x65, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN  //R6
	{0x66, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE
	{0x67, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0x68, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	//MCMC_06  
	{0x69, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN                                  {0x69, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0x6a, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE                                   {0x6a, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0x6b, 0x73, BYTE_BYTE_LEN, 0},//0_GAIN   //115 //R1                       {0x6b, 0x80, BYTE_BYTE_LEN, 0},//0_GAIN
	{0x6c, 0x81, BYTE_BYTE_LEN, 0},//0_HUE	  //-1                             {0x6c, 0x04, BYTE_BYTE_LEN, 0},//0_HUE
	{0x6d, 0x35, BYTE_BYTE_LEN, 0},//0_CENTER //53                             {0x6d, 0x36, BYTE_BYTE_LEN, 0},//0_CENTER
	{0x6e, 0x1a, BYTE_BYTE_LEN, 0},//0_DELTA  //26                             {0x6e, 0x13, BYTE_BYTE_LEN, 0},//0_DELTA
	{0x6f, 0x6c, BYTE_BYTE_LEN, 0},//1_GAIN   //108 //r2                       {0x6f, 0x80, BYTE_BYTE_LEN, 0},//1_GAIN   
	{0x70, 0x12, BYTE_BYTE_LEN, 0},//1_HUE	  //+18                            {0x70, 0x0c, BYTE_BYTE_LEN, 0},//1_HUE	  
	{0x71, 0x74, BYTE_BYTE_LEN, 0}, //jktest 0202//116//0x76, BYTE_BYTE_LEN, 0},//1_CENTER //118 {0x71, 0x62, BYTE_BYTE_LEN, 0},//1_CENTER 
	{0x72, 0x21, BYTE_BYTE_LEN, 0}, //jktest 0202//33//0x34, BYTE_BYTE_LEN, 0},//1_DELTA  //52   {0x72, 0x10, BYTE_BYTE_LEN, 0},//1_DELTA			 
	{0x73, 0x88, BYTE_BYTE_LEN, 0},//2_GAIN   //136 //r3                       {0x73, 0x73, BYTE_BYTE_LEN, 0},//2_GAIN
	{0x74, 0x8c, BYTE_BYTE_LEN, 0},//2_HUE	  //-12                            {0x74, 0x8a, BYTE_BYTE_LEN, 0},//2_HUE
	{0x75, 0xa9, BYTE_BYTE_LEN, 0},//2_CENTER //169                            {0x75, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0x76, 0x10, BYTE_BYTE_LEN, 0},//2_DELTA  //16                             {0x76, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0x77, 0x90, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0},//3_GAIN   //87 //r4                        {0x77, 0x80, BYTE_BYTE_LEN, 0},//3_GAIN
	{0x78, 0x20, BYTE_BYTE_LEN, 0},//0219//0x03, BYTE_BYTE_LEN, 0},//t//0x88, BYTE_BYTE_LEN, 0},//3_HUE	  //-8                             {0x78, 0x86, BYTE_BYTE_LEN, 0},//3_HUE	  
	{0x79, 0x4a, BYTE_BYTE_LEN, 0}, //jktest 0202//74//0x52, BYTE_BYTE_LEN, 0},//3_CENTER //82   {0x79, 0x51, BYTE_BYTE_LEN, 0},//3_CENTER
	{0x7a, 0x0b, BYTE_BYTE_LEN, 0}, //jktest 0202//11//0x16, BYTE_BYTE_LEN, 0},//3_DELTA  //22   {0x7a, 0x14, BYTE_BYTE_LEN, 0},//3_DELTA  
	{0x7b, 0x80, BYTE_BYTE_LEN, 0},//0219//0x90, BYTE_BYTE_LEN, 0},//t//0x80, BYTE_BYTE_LEN, 0},//4_GAIN   //128 //r5                       {0x7b, 0x80, BYTE_BYTE_LEN, 0},//4_GAIN
	{0x7c, 0x0c, BYTE_BYTE_LEN, 0},//0219//0x65, BYTE_BYTE_LEN, 0},//t//0x30, BYTE_BYTE_LEN, 0},//t//0x0c, BYTE_BYTE_LEN, 0},//4_HUE	  //12                             {0x7c, 0x0c, BYTE_BYTE_LEN, 0},//4_HUE
	{0x7d, 0x5b, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0}, //jktest 0202//87//0x76, BYTE_BYTE_LEN, 0},//4_CENTER //118  {0x7d, 0x76, BYTE_BYTE_LEN, 0},//4_CENTER
	{0x7e, 0x16, BYTE_BYTE_LEN, 0},//t//0x0d, BYTE_BYTE_LEN, 0}, //jktest 0202//12//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA  //28   {0x7e, 0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0x7f, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN   //178 //r6                       {0x7f, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN
	{0x80, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE	  //-10                            {0x80, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE
	{0x81, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER //82                             {0x81, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0x82, 0x1c, BYTE_BYTE_LEN, 0},//5_DELTA  //28                             {0x82, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	//MCMC_07  
	{0x83, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0x84, 0x82, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0x85, 0x80, BYTE_BYTE_LEN, 0},//0_GAIN
	{0x86, 0x04, BYTE_BYTE_LEN, 0},//0_HUE
	{0x87, 0x36, BYTE_BYTE_LEN, 0},//0_CENTER
	{0x88, 0x13, BYTE_BYTE_LEN, 0},//0_DELTA
	{0x89, 0x80, BYTE_BYTE_LEN, 0},//1_GAIN  
	{0x8a, 0x0c, BYTE_BYTE_LEN, 0},//1_HUE	 
	{0x8b, 0x62, BYTE_BYTE_LEN, 0},//1_CENTER
	{0x8c, 0x16, BYTE_BYTE_LEN, 0},//jktest 0202//22//0x10, BYTE_BYTE_LEN, 0},//1_DELTA 
	{0x8d, 0x73, BYTE_BYTE_LEN, 0},//2_GAIN
	{0x8e, 0x8a, BYTE_BYTE_LEN, 0},//2_HUE
	{0x8f, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0x90, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0x91, 0x80, BYTE_BYTE_LEN, 0},//3_GAIN
	{0x92, 0x86, BYTE_BYTE_LEN, 0},//3_HUE	 
	{0x93, 0x4a, BYTE_BYTE_LEN, 0},//jktest 0202//74//0x51, BYTE_BYTE_LEN, 0},//3_CENTER
	{0x94, 0x0a, BYTE_BYTE_LEN, 0},//jktest 0202//10//0x14, BYTE_BYTE_LEN, 0},//3_DELTA 
	{0x95, 0x80, BYTE_BYTE_LEN, 0},//4_GAIN
	{0x96, 0x0c, BYTE_BYTE_LEN, 0},//0x00, BYTE_BYTE_LEN, 0},//0219 tt//0x0c, BYTE_BYTE_LEN, 0},//4_HUE
	{0x97, 0x5b, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0},//jktest 0202//87//0x76, BYTE_BYTE_LEN, 0},//4_CENTER
	{0x98, 0x16, BYTE_BYTE_LEN, 0},//t//0x0c, BYTE_BYTE_LEN, 0},//jktest 0202//12//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0x99, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN
	{0x9a, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE
	{0x9b, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0x9c, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	//MCMC_08  
	{0x9d, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0x9e, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0x9f, 0x80, BYTE_BYTE_LEN, 0},//0_GAIN
	{0xa0, 0x04, BYTE_BYTE_LEN, 0},//0_HUE
	{0xa1, 0x36, BYTE_BYTE_LEN, 0},//0_CENTER
	{0xa2, 0x13, BYTE_BYTE_LEN, 0},//0_DELTA
	{0xa3, 0x80, BYTE_BYTE_LEN, 0},//1_GAIN  
	{0xa4, 0x0c, BYTE_BYTE_LEN, 0},//1_HUE	 
	{0xa5, 0x62, BYTE_BYTE_LEN, 0},//1_CENTER
	{0xa6, 0x10, BYTE_BYTE_LEN, 0},//1_DELTA 
	{0xa7, 0x73, BYTE_BYTE_LEN, 0},//2_GAIN
	{0xa8, 0x8a, BYTE_BYTE_LEN, 0},//2_HUE
	{0xa9, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0xaa, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0xab, 0x80, BYTE_BYTE_LEN, 0},//3_GAIN
	{0xac, 0x86, BYTE_BYTE_LEN, 0},//3_HUE	 
	{0xad, 0x35, BYTE_BYTE_LEN, 0},//jktest 0202//0x51, BYTE_BYTE_LEN, 0},//3_CENTER
	{0xae, 0x13, BYTE_BYTE_LEN, 0},//jktest 0202//0x14, BYTE_BYTE_LEN, 0},//3_DELTA 
	{0xaf, 0x80, BYTE_BYTE_LEN, 0},//4_GAIN
	{0xb0, 0x86, BYTE_BYTE_LEN, 0},//jktest 0202//0x0c, BYTE_BYTE_LEN, 0},//4_HUE
	{0xb1, 0x5b, BYTE_BYTE_LEN, 0},//0x6e, BYTE_BYTE_LEN, 0},//jktest 0202//0x76, BYTE_BYTE_LEN, 0},//4_CENTER
	{0xb2, 0x16, BYTE_BYTE_LEN, 0},//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0xb3, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN
	{0xb4, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE
	{0xb5, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0xb6, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	//MCMC_09  
	{0xb7, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN                                  {0xb7, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0xb8, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE                                   {0xb8, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0xb9, 0x73, BYTE_BYTE_LEN, 0},//0_GAIN   //115 //R1                       {0xb9, 0x80, BYTE_BYTE_LEN, 0},//0_GAIN
	{0xba, 0x81, BYTE_BYTE_LEN, 0},//0_HUE	  //-1                             {0xba, 0x04, BYTE_BYTE_LEN, 0},//0_HUE
	{0xbb, 0x35, BYTE_BYTE_LEN, 0},//0_CENTER //53                             {0xbb, 0x36, BYTE_BYTE_LEN, 0},//0_CENTER
	{0xbc, 0x1a, BYTE_BYTE_LEN, 0},//0_DELTA  //26                             {0xbc, 0x13, BYTE_BYTE_LEN, 0},//0_DELTA
	{0xbd, 0x6c, BYTE_BYTE_LEN, 0},//1_GAIN   //108 //r2                       {0xbd, 0x80, BYTE_BYTE_LEN, 0},//1_GAIN   
	{0xbe, 0x12, BYTE_BYTE_LEN, 0},//1_HUE	  //+18                            {0xbe, 0x0c, BYTE_BYTE_LEN, 0},//1_HUE	  
	{0xbf, 0x74, BYTE_BYTE_LEN, 0}, //jktest 0202//116//0x76, BYTE_BYTE_LEN, 0},//1_CENTER //118 {0xbf, 0x62, BYTE_BYTE_LEN, 0},//1_CENTER 
	{0xc0, 0x21, BYTE_BYTE_LEN, 0}, //jktest 0202//33//0x34, BYTE_BYTE_LEN, 0},//1_DELTA  //52   {0xc0, 0x10, BYTE_BYTE_LEN, 0},//1_DELTA  
	{0xc1, 0x88, BYTE_BYTE_LEN, 0},//2_GAIN   //136 //r3                       {0xc1, 0x73, BYTE_BYTE_LEN, 0},//2_GAIN
	{0xc2, 0x8c, BYTE_BYTE_LEN, 0},//2_HUE	  //-12                            {0xc2, 0x8a, BYTE_BYTE_LEN, 0},//2_HUE
	{0xc3, 0xa9, BYTE_BYTE_LEN, 0},//2_CENTER //169                            {0xc3, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0xc4, 0x10, BYTE_BYTE_LEN, 0},//2_DELTA  //16                             {0xc4, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0xc5, 0x90, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0},//3_GAIN   //87 //r4                        {0xc5, 0x80, BYTE_BYTE_LEN, 0},//3_GAIN
	{0xc6, 0x20, BYTE_BYTE_LEN, 0},//0219//0x03, BYTE_BYTE_LEN, 0},//t//0x88, BYTE_BYTE_LEN, 0},//3_HUE	  //-8                             {0xc6, 0x86, BYTE_BYTE_LEN, 0},//3_HUE	  
	{0xc7, 0x4a, BYTE_BYTE_LEN, 0}, //jktest 0202//74//0x52, BYTE_BYTE_LEN, 0},//3_CENTER //82   {0xc7, 0x51, BYTE_BYTE_LEN, 0},//3_CENTER
	{0xc8, 0x0b, BYTE_BYTE_LEN, 0}, //jktest 0202//11//0x16, BYTE_BYTE_LEN, 0},//3_DELTA  //22   {0xc8, 0x14, BYTE_BYTE_LEN, 0},//3_DELTA  
	{0xc9, 0x80, BYTE_BYTE_LEN, 0},//0219//0x90, BYTE_BYTE_LEN, 0},//t//0x80, BYTE_BYTE_LEN, 0},//4_GAIN   //128 //r5                       {0xc9, 0x80, BYTE_BYTE_LEN, 0},//4_GAIN
	{0xca, 0x0c, BYTE_BYTE_LEN, 0},//0219//0x65, BYTE_BYTE_LEN, 0},//t//0x30, BYTE_BYTE_LEN, 0},//t//0x0c, BYTE_BYTE_LEN, 0},//4_HUE	  //12                             {0xca, 0x0c, BYTE_BYTE_LEN, 0},//4_HUE
	{0xcb, 0x5b, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0}, //jktest 0202//87//0x76, BYTE_BYTE_LEN, 0},//4_CENTER //118  {0xcb, 0x76, BYTE_BYTE_LEN, 0},//4_CENTER
	{0xcc, 0x16, BYTE_BYTE_LEN, 0},//t//0x0d, BYTE_BYTE_LEN, 0}, //jktest 0202//12//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA  //28   {0xcc, 0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0xcd, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN   //178 //r6                       {0xcd, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN
	{0xce, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE	  //-10                            {0xce, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE
	{0xcf, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER //82                             {0xcf, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0xd0, 0x1c, BYTE_BYTE_LEN, 0},//5_DELTA  //28                             {0xd0, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	//MCMC_10  
	{0xd1, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0xd2, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0xd3, 0x80, BYTE_BYTE_LEN, 0},//0_GAIN
	{0xd4, 0x04, BYTE_BYTE_LEN, 0},//0_HUE
	{0xd5, 0x36, BYTE_BYTE_LEN, 0},//0_CENTER
	{0xd6, 0x13, BYTE_BYTE_LEN, 0},//0_DELTA
	{0xd7, 0x80, BYTE_BYTE_LEN, 0},//1_GAIN  
	{0xd8, 0x0c, BYTE_BYTE_LEN, 0},//1_HUE	 
	{0xd9, 0x62, BYTE_BYTE_LEN, 0},//1_CENTER
	{0xda, 0x10, BYTE_BYTE_LEN, 0},//1_DELTA 
	{0xdb, 0x73, BYTE_BYTE_LEN, 0},//2_GAIN
	{0xdc, 0x8a, BYTE_BYTE_LEN, 0},//2_HUE
	{0xdd, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0xde, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0xdf, 0x80, BYTE_BYTE_LEN, 0},//3_GAIN
	{0xe0, 0x86, BYTE_BYTE_LEN, 0},//3_HUE	 
	{0xe1, 0x4a, BYTE_BYTE_LEN, 0},//jktest 0202//74//0x51, BYTE_BYTE_LEN, 0},//3_CENTER
	{0xe2, 0x0a, BYTE_BYTE_LEN, 0},//jktest 0202//10//0x14, BYTE_BYTE_LEN, 0},//3_DELTA 
	{0xe3, 0x80, BYTE_BYTE_LEN, 0},//4_GAIN
	{0xe4, 0x0c, BYTE_BYTE_LEN, 0},//4_HUE
	{0xe5, 0x5b, BYTE_BYTE_LEN, 0},//t//0x57, BYTE_BYTE_LEN, 0},//jktest 0202//87//0x76, BYTE_BYTE_LEN, 0},//4_CENTER
	{0xe6, 0x16, BYTE_BYTE_LEN, 0},//t//0x0c, BYTE_BYTE_LEN, 0},//jktest 0202//12//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0xe7, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN
	{0xe8, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE
	{0xe9, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0xea, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	//MCMC_11  
	{0xeb, 0x80, BYTE_BYTE_LEN, 0},//GLB_GAIN
	{0xec, 0x01, BYTE_BYTE_LEN, 0},//GLB_HUE
	{0xed, 0x80, BYTE_BYTE_LEN, 0},//0_GAIN
	{0xee, 0x04, BYTE_BYTE_LEN, 0},//0_HUE
	{0xef, 0x36, BYTE_BYTE_LEN, 0},//0_CENTER
	{0xf0, 0x13, BYTE_BYTE_LEN, 0},//0_DELTA
	{0xf1, 0x80, BYTE_BYTE_LEN, 0},//1_GAIN  
	{0xf2, 0x0c, BYTE_BYTE_LEN, 0},//1_HUE	 
	{0xf3, 0x62, BYTE_BYTE_LEN, 0},//1_CENTER
	{0xf4, 0x10, BYTE_BYTE_LEN, 0},//1_DELTA 
	{0xf5, 0x73, BYTE_BYTE_LEN, 0},//2_GAIN
	{0xf6, 0x8a, BYTE_BYTE_LEN, 0},//2_HUE
	{0xf7, 0xaf, BYTE_BYTE_LEN, 0},//2_CENTER
	{0xf8, 0x1c, BYTE_BYTE_LEN, 0},//2_DELTA
	{0xf9, 0x80, BYTE_BYTE_LEN, 0},//3_GAIN
	{0xfa, 0x86, BYTE_BYTE_LEN, 0},//3_HUE	 
	{0xfb, 0x35, BYTE_BYTE_LEN, 0},//jktest 0202//0x51, BYTE_BYTE_LEN, 0},//3_CENTER
	{0xfc, 0x13, BYTE_BYTE_LEN, 0},//jktest 0202//0x14, BYTE_BYTE_LEN, 0},//3_DELTA 
	{0xfd, 0x80, BYTE_BYTE_LEN, 0},//4_GAIN
	           
	{0x03, 0xd3, BYTE_BYTE_LEN, 0},//Page d3	
	{0x10, 0x86, BYTE_BYTE_LEN, 0},//4_HUE
	{0x11, 0x5b, BYTE_BYTE_LEN, 0},//t//0x6e, BYTE_BYTE_LEN, 0},//jktest 0202//0x76, BYTE_BYTE_LEN, 0},//4_CENTER
	{0x12, 0x16, BYTE_BYTE_LEN, 0},//t//0x1c, BYTE_BYTE_LEN, 0},//4_DELTA
	{0x13, 0xb2, BYTE_BYTE_LEN, 0},//5_GAIN
	{0x14, 0x8a, BYTE_BYTE_LEN, 0},//5_HUE
	{0x15, 0x52, BYTE_BYTE_LEN, 0},//5_CENTER
	{0x16, 0x1d, BYTE_BYTE_LEN, 0},//5_DELTA
	
	///////////////////////////////////////////
	// D3 Page Adaptive LSC
	///////////////////////////////////////////
	
	{0x17, 0x00, BYTE_BYTE_LEN, 0}, //LSC 00 ofs GB //00
	{0x18, 0x00, BYTE_BYTE_LEN, 0}, //LSC 00 ofs B
	{0x19, 0x02, BYTE_BYTE_LEN, 0}, //LSC 00 ofs R
	{0x1a, 0x00, BYTE_BYTE_LEN, 0}, //LSC 00 ofs GR
	           
	{0x1b, 0x00, BYTE_BYTE_LEN, 0}, //0x40, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0},//LSC 00 Gain GB
	{0x1c, 0x00, BYTE_BYTE_LEN, 0}, //0x40, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0},//LSC 00 Gain B 
	{0x1d, 0x00, BYTE_BYTE_LEN, 0}, //0x45, BYTE_BYTE_LEN, 0}, //28set//0x14, BYTE_BYTE_LEN, 0}, //LSC 00 Gain R
	{0x1e, 0x00, BYTE_BYTE_LEN, 0}, //0x40, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0},//LSC 00 Gain GR
	           
	{0x1f, 0x00, BYTE_BYTE_LEN, 0}, //LSC 01 ofs GB //01
	{0x20, 0x00, BYTE_BYTE_LEN, 0}, //LSC 01 ofs B	
	{0x21, 0x02, BYTE_BYTE_LEN, 0}, //LSC 01 ofs R	
	{0x22, 0x00, BYTE_BYTE_LEN, 0}, //LSC 01 ofs GR 
	{0x23, 0x00, BYTE_BYTE_LEN, 0}, //0x40, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0},//LSC 01 Gain GB
	{0x24, 0x00, BYTE_BYTE_LEN, 0}, //0x40, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0},//LSC 01 Gain B 
	{0x25, 0x00, BYTE_BYTE_LEN, 0}, //0x45, BYTE_BYTE_LEN, 0}, //28set//0x14, BYTE_BYTE_LEN, 0}, //LSC 01 Gain R 
	{0x26, 0x00, BYTE_BYTE_LEN, 0}, //0x40, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0},//LSC 01 Gain GR
	           
	{0x27, 0x00, BYTE_BYTE_LEN, 0}, //LSC 02 ofs GB //02
	{0x28, 0x00, BYTE_BYTE_LEN, 0}, //LSC 02 ofs B	
	{0x29, 0x00, BYTE_BYTE_LEN, 0}, //LSC 02 ofs R	
	{0x2a, 0x00, BYTE_BYTE_LEN, 0}, //LSC 02 ofs GR 
	{0x2b, 0x00, BYTE_BYTE_LEN, 0}, //0x40, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0},//LSC 02 Gain GB
	{0x2c, 0x00, BYTE_BYTE_LEN, 0}, //0x40, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0},//LSC 02 Gain B 
	{0x2d, 0x00, BYTE_BYTE_LEN, 0}, //0x45, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0}, //LSC 02 Gain R 
	{0x2e, 0x00, BYTE_BYTE_LEN, 0}, //0x40, BYTE_BYTE_LEN, 0}, //28set//0x20, BYTE_BYTE_LEN, 0},//LSC 02 Gain GR
	           
	{0x2f, 0x00, BYTE_BYTE_LEN, 0}, //LSC 03 ofs GB //03
	{0x30, 0x00, BYTE_BYTE_LEN, 0}, //LSC 03 ofs B	
	{0x31, 0x00, BYTE_BYTE_LEN, 0}, //0308//0x04, BYTE_BYTE_LEN, 0}, //LSC 03 ofs R	
	{0x32, 0x00, BYTE_BYTE_LEN, 0}, //LSC 03 ofs GR 
	{0x33, 0x40, BYTE_BYTE_LEN, 0}, //LSC 03 Gain GB
	{0x34, 0x40, BYTE_BYTE_LEN, 0}, //LSC 03 Gain B 
	{0x35, 0x40, BYTE_BYTE_LEN, 0}, //0308//0x34, BYTE_BYTE_LEN, 0}, //LSC 03 Gain R 
	{0x36, 0x40, BYTE_BYTE_LEN, 0}, //LSC 03 Gain GR
	           
	{0x37, 0x00, BYTE_BYTE_LEN, 0}, //LSC 04 ofs GB //04
	{0x38, 0x00, BYTE_BYTE_LEN, 0}, //LSC 04 ofs B	
	{0x39, 0x00, BYTE_BYTE_LEN, 0}, //0308//0x04, BYTE_BYTE_LEN, 0}, //LSC 04 ofs R	
	{0x3a, 0x00, BYTE_BYTE_LEN, 0}, //LSC 04 ofs GR 
	{0x3b, 0x40, BYTE_BYTE_LEN, 0}, //LSC 04 Gain GB
	{0x3c, 0x40, BYTE_BYTE_LEN, 0}, //LSC 04 Gain B 
	{0x3d, 0x40, BYTE_BYTE_LEN, 0}, //0308//0x34, BYTE_BYTE_LEN, 0}, //LSC 04 Gain R 
	{0x3e, 0x40, BYTE_BYTE_LEN, 0}, //LSC 04 Gain GR
	           
	{0x3f, 0x00, BYTE_BYTE_LEN, 0}, //LSC 05 ofs GB //05
	{0x40, 0x00, BYTE_BYTE_LEN, 0}, //LSC 05 ofs B	
	{0x41, 0x00, BYTE_BYTE_LEN, 0}, //LSC 05 ofs R	
	{0x42, 0x00, BYTE_BYTE_LEN, 0}, //LSC 05 ofs GR 
	{0x43, 0x40, BYTE_BYTE_LEN, 0},//LSC 05 Gain GB
	{0x44, 0x40, BYTE_BYTE_LEN, 0},//LSC 05 Gain B 
	{0x45, 0x40, BYTE_BYTE_LEN, 0}, //LSC 05 Gain R 
	{0x46, 0x40, BYTE_BYTE_LEN, 0},//LSC 05 Gain GR
	           
	{0x47, 0x00, BYTE_BYTE_LEN, 0}, //LSC 06 ofs GB //06
	{0x48, 0x00, BYTE_BYTE_LEN, 0}, //LSC 06 ofs B	
	{0x49, 0x00, BYTE_BYTE_LEN, 0}, //0308//0x04, BYTE_BYTE_LEN, 0}, //LSC 06 ofs R	
	{0x4a, 0x00, BYTE_BYTE_LEN, 0}, //LSC 06 ofs GR 
	{0x4b, 0x40, BYTE_BYTE_LEN, 0},//78 LSC 06 Gain GB
	{0x4c, 0x40, BYTE_BYTE_LEN, 0},//7c LSC 06 Gain B 
	{0x4d, 0x40, BYTE_BYTE_LEN, 0}, //0308//0x34, BYTE_BYTE_LEN, 0},//80 LSC 06 Gain R 
	{0x4e, 0x40, BYTE_BYTE_LEN, 0},//78 LSC 06 Gain GR
	           
	{0x4f, 0x00, BYTE_BYTE_LEN, 0}, //LSC 07 ofs GB //07
	{0x50, 0x00, BYTE_BYTE_LEN, 0}, //LSC 07 ofs B	
	{0x51, 0x00, BYTE_BYTE_LEN, 0}, //0308//0x04, BYTE_BYTE_LEN, 0}, //LSC 07 ofs R	
	{0x52, 0x00, BYTE_BYTE_LEN, 0}, //LSC 07 ofs GR 
	{0x53, 0x40, BYTE_BYTE_LEN, 0},//78 LSC 07 Gain GB 
	{0x54, 0x40, BYTE_BYTE_LEN, 0},//7c LSC 07 Gain B  
	{0x55, 0x40, BYTE_BYTE_LEN, 0}, //0308//0x34, BYTE_BYTE_LEN, 0},//80 LSC 07 Gain R  
	{0x56, 0x40, BYTE_BYTE_LEN, 0},//78 LSC 07 Gain GR 
	           
	{0x57, 0x00, BYTE_BYTE_LEN, 0}, //LSC 08 ofs GB //08
	{0x58, 0x00, BYTE_BYTE_LEN, 0}, //LSC 08 ofs B	
	{0x59, 0x00, BYTE_BYTE_LEN, 0}, //LSC 08 ofs R	
	{0x5a, 0x00, BYTE_BYTE_LEN, 0}, //LSC 08 ofs GR 
	{0x5b, 0x40, BYTE_BYTE_LEN, 0}, //78 LSC 08 Gain GB
	{0x5c, 0x40, BYTE_BYTE_LEN, 0}, //7c LSC 08 Gain B 
	{0x5d, 0x40, BYTE_BYTE_LEN, 0}, //80 LSC 08 Gain R 
	{0x5e, 0x40, BYTE_BYTE_LEN, 0}, //78 LSC 08 Gain GR
	           
	{0x5f, 0x00, BYTE_BYTE_LEN, 0}, //LSC 09 ofs GB //09
	{0x60, 0x00, BYTE_BYTE_LEN, 0}, //LSC 09 ofs B	
	{0x61, 0x00, BYTE_BYTE_LEN, 0}, //LSC 09 ofs R	
	{0x62, 0x00, BYTE_BYTE_LEN, 0}, //LSC 09 ofs GR 
	{0x63, 0x40, BYTE_BYTE_LEN, 0}, //78 LSC 09 Gain GB
	{0x64, 0x40, BYTE_BYTE_LEN, 0}, //7c LSC 09 Gain B 
	{0x65, 0x40, BYTE_BYTE_LEN, 0}, //80 LSC 09 Gain R 
	{0x66, 0x40, BYTE_BYTE_LEN, 0}, //78 LSC 09 Gain GR
	           
	{0x67, 0x00, BYTE_BYTE_LEN, 0}, //LSC 10 ofs GB //10
	{0x68, 0x00, BYTE_BYTE_LEN, 0}, //LSC 10 ofs B	
	{0x69, 0x00, BYTE_BYTE_LEN, 0}, //LSC 10 ofs R	
	{0x6a, 0x00, BYTE_BYTE_LEN, 0}, //LSC 10 ofs GR 
	{0x6b, 0x40, BYTE_BYTE_LEN, 0}, //78 LSC 10 Gain GB
	{0x6c, 0x40, BYTE_BYTE_LEN, 0}, //7c LSC 10 Gain B 
	{0x6d, 0x40, BYTE_BYTE_LEN, 0}, //80 LSC 10 Gain R 
	{0x6e, 0x40, BYTE_BYTE_LEN, 0}, //78 LSC 10 Gain GR
	           
	{0x6f, 0x00, BYTE_BYTE_LEN, 0}, //LSC 11 ofs GB //11
	{0x70, 0x00, BYTE_BYTE_LEN, 0}, //LSC 11 ofs B	
	{0x71, 0x00, BYTE_BYTE_LEN, 0}, //LSC 11 ofs R	
	{0x72, 0x00, BYTE_BYTE_LEN, 0}, //LSC 11 ofs GR 
	{0x73, 0x40, BYTE_BYTE_LEN, 0}, //78 LSC 11 Gain GB
	{0x74, 0x40, BYTE_BYTE_LEN, 0}, //7c LSC 11 Gain B 
	{0x75, 0x40, BYTE_BYTE_LEN, 0}, //80 LSC 11 Gain R 
	{0x76, 0x40, BYTE_BYTE_LEN, 0}, //78 LSC 11 Gain GR
	///////////////////////////////////////////
	// D3 Page OTP, ROM Select TH
	///////////////////////////////////////////
	{0x77, 0x60, BYTE_BYTE_LEN, 0}, //2 ROM High
	{0x78, 0x20, BYTE_BYTE_LEN, 0}, //2 ROM Low
	{0x79, 0x60, BYTE_BYTE_LEN, 0}, //3 OTP High
	{0x7a, 0x40, BYTE_BYTE_LEN, 0}, //3 OTP Mid
	{0x7b, 0x20, BYTE_BYTE_LEN, 0}, //3 OTP Low
	///////////////////////////////////////////
	// D3 Page Adaptive DNP
	///////////////////////////////////////////
	{0x7c, 0x00, BYTE_BYTE_LEN, 0}, //LSC EV max
	{0x7d, 0x00, BYTE_BYTE_LEN, 0},
	{0x7e, 0x07, BYTE_BYTE_LEN, 0},
	{0x7f, 0xf1, BYTE_BYTE_LEN, 0},
	           
	{0x80, 0x00, BYTE_BYTE_LEN, 0}, //LSC EV min
	{0x81, 0x00, BYTE_BYTE_LEN, 0},
	{0x82, 0x07, BYTE_BYTE_LEN, 0},
	{0x83, 0xf1, BYTE_BYTE_LEN, 0},
	{0x84, 0x20, BYTE_BYTE_LEN, 0}, //CTEM max
	{0x85, 0x20, BYTE_BYTE_LEN, 0}, //CTEM min
	{0x86, 0x20, BYTE_BYTE_LEN, 0}, //Y STD max
	{0x87, 0x20, BYTE_BYTE_LEN, 0}, //Y STD min
	           
	{0x88, 0x00, BYTE_BYTE_LEN, 0}, //LSC offset
	{0x89, 0x00, BYTE_BYTE_LEN, 0},
	{0x8a, 0x00, BYTE_BYTE_LEN, 0},
	{0x8b, 0x00, BYTE_BYTE_LEN, 0},
	{0x8c, 0x80, BYTE_BYTE_LEN, 0}, //LSC gain
	{0x8d, 0x80, BYTE_BYTE_LEN, 0},
	{0x8e, 0x80, BYTE_BYTE_LEN, 0},
	{0x8f, 0x80, BYTE_BYTE_LEN, 0},
	           
	{0x90, 0x80, BYTE_BYTE_LEN, 0}, //DNP CB
	{0x91, 0x80, BYTE_BYTE_LEN, 0}, //DNP CR
	
	///////////////////////////////////
	//Page 0xD9 DMA EXTRA
	///////////////////////////////////
	
	{0x03, 0xd9, BYTE_BYTE_LEN, 0},
	
	{0x10, 0x03, BYTE_BYTE_LEN, 0},
	{0x11, 0x10, BYTE_BYTE_LEN, 0},//Page 10
	{0x12, 0x61, BYTE_BYTE_LEN, 0},//sat B
	{0x13, 0x80, BYTE_BYTE_LEN, 0},
	{0x14, 0x62, BYTE_BYTE_LEN, 0},//sat R
	{0x15, 0x80, BYTE_BYTE_LEN, 0},
	{0x16, 0x03, BYTE_BYTE_LEN, 0}, //jktest 0131//0x40, BYTE_BYTE_LEN, 0}, //dyoffset
	{0x17, 0x00, BYTE_BYTE_LEN, 0}, //0x05, BYTE_BYTE_LEN, 0}, //17th//0x07, BYTE_BYTE_LEN, 0}, //0220 test dyoffset//0x00, BYTE_BYTE_LEN, 0},
	           
	{0x18, 0x03, BYTE_BYTE_LEN, 0}, //jktest 0131//0x40, BYTE_BYTE_LEN, 0}, //dyoffset
	{0x19, 0x00, BYTE_BYTE_LEN, 0}, //0x05, BYTE_BYTE_LEN, 0}, //0x00, BYTE_BYTE_LEN, 0},
	{0x1a, 0x03, BYTE_BYTE_LEN, 0},//Page 16 CMC
	{0x1b, 0x16, BYTE_BYTE_LEN, 0},
	{0x1c, 0x30, BYTE_BYTE_LEN, 0},//CMC11
	{0x1d, 0x7f, BYTE_BYTE_LEN, 0},
	{0x1e, 0x31, BYTE_BYTE_LEN, 0},//CMC12
	{0x1f, 0x42, BYTE_BYTE_LEN, 0},
	           
	{0x20, 0x32, BYTE_BYTE_LEN, 0},//CMC13
	{0x21, 0x03, BYTE_BYTE_LEN, 0},
	{0x22, 0x33, BYTE_BYTE_LEN, 0},//CMC21
	{0x23, 0x22, BYTE_BYTE_LEN, 0},
	{0x24, 0x34, BYTE_BYTE_LEN, 0},//CMC22
	{0x25, 0x7b, BYTE_BYTE_LEN, 0},
	{0x26, 0x35, BYTE_BYTE_LEN, 0},//CMC23
	{0x27, 0x19, BYTE_BYTE_LEN, 0},           
	{0x28, 0x36, BYTE_BYTE_LEN, 0},//CMC31
	{0x29, 0x01, BYTE_BYTE_LEN, 0},
	{0x2a, 0x37, BYTE_BYTE_LEN, 0},//CMC32
	{0x2b, 0x43, BYTE_BYTE_LEN, 0},
	{0x2c, 0x38, BYTE_BYTE_LEN, 0},//CMC33
	{0x2d, 0x84, BYTE_BYTE_LEN, 0},
	{0x2e, 0x70, BYTE_BYTE_LEN, 0},//GLBSATGAIN
	{0x2f, 0x80, BYTE_BYTE_LEN, 0},
	           
	{0x30, 0x71, BYTE_BYTE_LEN, 0},//GLBROT
	{0x31, 0x00, BYTE_BYTE_LEN, 0},
	{0x32, 0x72, BYTE_BYTE_LEN, 0},//R1SATGAIN
	{0x33, 0x9b, BYTE_BYTE_LEN, 0},
	{0x34, 0x73, BYTE_BYTE_LEN, 0},//R1ROT
	{0x35, 0x05, BYTE_BYTE_LEN, 0},
	{0x36, 0x74, BYTE_BYTE_LEN, 0},//R1CENTERANG
	{0x37, 0x34, BYTE_BYTE_LEN, 0},
	           
	{0x38, 0x75, BYTE_BYTE_LEN, 0},//R1DELTAANG
	{0x39, 0x1e, BYTE_BYTE_LEN, 0},
	{0x3a, 0x76, BYTE_BYTE_LEN, 0},//R2SATGAIN
	{0x3b, 0xa6, BYTE_BYTE_LEN, 0},
	{0x3c, 0x77, BYTE_BYTE_LEN, 0},//R2ROT
	{0x3d, 0x10, BYTE_BYTE_LEN, 0},
	{0x3e, 0x78, BYTE_BYTE_LEN, 0},//R2CENTERANG
	{0x3f, 0x69, BYTE_BYTE_LEN, 0},
	           
	{0x40, 0x79, BYTE_BYTE_LEN, 0},//R2DELTAANG
	{0x41, 0x1e, BYTE_BYTE_LEN, 0},
	{0x42, 0x7a, BYTE_BYTE_LEN, 0},//R3SATGAIN
	{0x43, 0x80, BYTE_BYTE_LEN, 0},
	{0x44, 0x7b, BYTE_BYTE_LEN, 0},//R3ROT
	{0x45, 0x80, BYTE_BYTE_LEN, 0},
	{0x46, 0x7c, BYTE_BYTE_LEN, 0},//R3CENTERANG
	{0x47, 0xad, BYTE_BYTE_LEN, 0},
	           
	{0x48, 0x7d, BYTE_BYTE_LEN, 0},//R3DELTAANG
	{0x49, 0x1e, BYTE_BYTE_LEN, 0},
	{0x4a, 0x7e, BYTE_BYTE_LEN, 0},//R4SATGAIN
	{0x4b, 0x98, BYTE_BYTE_LEN, 0},
	{0x4c, 0x7f, BYTE_BYTE_LEN, 0},//R4ROT
	{0x4d, 0x80, BYTE_BYTE_LEN, 0},
	{0x4e, 0x80, BYTE_BYTE_LEN, 0},//R4CENTERANG
	{0x4f, 0x51, BYTE_BYTE_LEN, 0},
	           
	{0x50, 0x81, BYTE_BYTE_LEN, 0},//R4DELTANAG
	{0x51, 0x1e, BYTE_BYTE_LEN, 0},
	{0x52, 0x82, BYTE_BYTE_LEN, 0},//R5SATGAIN
	{0x53, 0x80, BYTE_BYTE_LEN, 0},
	{0x54, 0x83, BYTE_BYTE_LEN, 0},//R5ROT
	{0x55, 0x0c, BYTE_BYTE_LEN, 0},
	{0x56, 0x84, BYTE_BYTE_LEN, 0},//R5CENTERANG
	{0x57, 0x23, BYTE_BYTE_LEN, 0},
	           
	{0x58, 0x85, BYTE_BYTE_LEN, 0},//R5DELTAANG
	{0x59, 0x1e, BYTE_BYTE_LEN, 0},
	{0x5a, 0x86, BYTE_BYTE_LEN, 0},//R6SATGAIN
	{0x5b, 0xb3, BYTE_BYTE_LEN, 0},
	{0x5c, 0x87, BYTE_BYTE_LEN, 0},//R6ROT
	{0x5d, 0x8a, BYTE_BYTE_LEN, 0},
	{0x5e, 0x88, BYTE_BYTE_LEN, 0},//R6CENTERANG
	{0x5f, 0x52, BYTE_BYTE_LEN, 0},
	           
	{0x60, 0x89, BYTE_BYTE_LEN, 0},//R6DELTAANG
	{0x61, 0x1e, BYTE_BYTE_LEN, 0},
	{0x62, 0x03, BYTE_BYTE_LEN, 0},//Page 17 Gamma
	{0x63, 0x17, BYTE_BYTE_LEN, 0},
	{0x64, 0x20, BYTE_BYTE_LEN, 0},//GMMA0 
	{0x65, 0x00, BYTE_BYTE_LEN, 0},
	{0x66, 0x21, BYTE_BYTE_LEN, 0},
	{0x67, 0x02, BYTE_BYTE_LEN, 0},
	           
	{0x68, 0x22, BYTE_BYTE_LEN, 0},
	{0x69, 0x04, BYTE_BYTE_LEN, 0},
	{0x6a, 0x23, BYTE_BYTE_LEN, 0},
	{0x6b, 0x09, BYTE_BYTE_LEN, 0},
	{0x6c, 0x24, BYTE_BYTE_LEN, 0},
	{0x6d, 0x12, BYTE_BYTE_LEN, 0},
	{0x6e, 0x25, BYTE_BYTE_LEN, 0},
	{0x6f, 0x23, BYTE_BYTE_LEN, 0},
	           
	{0x70, 0x26, BYTE_BYTE_LEN, 0},//GMMA6
	{0x71, 0x37, BYTE_BYTE_LEN, 0},
	{0x72, 0x27, BYTE_BYTE_LEN, 0},
	{0x73, 0x47, BYTE_BYTE_LEN, 0},
	{0x74, 0x28, BYTE_BYTE_LEN, 0},
	{0x75, 0x57, BYTE_BYTE_LEN, 0},
	{0x76, 0x29, BYTE_BYTE_LEN, 0},
	{0x77, 0x61, BYTE_BYTE_LEN, 0},
	           
	{0x78, 0x2a, BYTE_BYTE_LEN, 0},
	{0x79, 0x6b, BYTE_BYTE_LEN, 0},
	{0x7a, 0x2b, BYTE_BYTE_LEN, 0},
	{0x7b, 0x71, BYTE_BYTE_LEN, 0},
	{0x7c, 0x2c, BYTE_BYTE_LEN, 0},
	{0x7d, 0x76, BYTE_BYTE_LEN, 0},
	{0x7e, 0x2d, BYTE_BYTE_LEN, 0},//GMMA13
	{0x7f, 0x7a, BYTE_BYTE_LEN, 0},
	           
	{0x80, 0x2e, BYTE_BYTE_LEN, 0},
	{0x81, 0x7f, BYTE_BYTE_LEN, 0},
	{0x82, 0x2f, BYTE_BYTE_LEN, 0},
	{0x83, 0x84, BYTE_BYTE_LEN, 0},
	{0x84, 0x30, BYTE_BYTE_LEN, 0},
	{0x85, 0x88, BYTE_BYTE_LEN, 0},
	{0x86, 0x31, BYTE_BYTE_LEN, 0},
	{0x87, 0x8c, BYTE_BYTE_LEN, 0},
	           
	{0x88, 0x32, BYTE_BYTE_LEN, 0},
	{0x89, 0x91, BYTE_BYTE_LEN, 0},
	{0x8a, 0x33, BYTE_BYTE_LEN, 0},
	{0x8b, 0x94, BYTE_BYTE_LEN, 0},
	{0x8c, 0x34, BYTE_BYTE_LEN, 0},
	{0x8d, 0x98, BYTE_BYTE_LEN, 0},
	{0x8e, 0x35, BYTE_BYTE_LEN, 0},
	{0x8f, 0x9f, BYTE_BYTE_LEN, 0},
	           
	{0x90, 0x36, BYTE_BYTE_LEN, 0},
	{0x91, 0xa6, BYTE_BYTE_LEN, 0},
	{0x92, 0x37, BYTE_BYTE_LEN, 0},
	{0x93, 0xae, BYTE_BYTE_LEN, 0},
	{0x94, 0x38, BYTE_BYTE_LEN, 0},
	{0x95, 0xbb, BYTE_BYTE_LEN, 0},
	{0x96, 0x39, BYTE_BYTE_LEN, 0},
	{0x97, 0xc9, BYTE_BYTE_LEN, 0},
	           
	{0x98, 0x3a, BYTE_BYTE_LEN, 0},
	{0x99, 0xd3, BYTE_BYTE_LEN, 0},
	{0x9a, 0x3b, BYTE_BYTE_LEN, 0},
	{0x9b, 0xdc, BYTE_BYTE_LEN, 0},
	{0x9c, 0x3c, BYTE_BYTE_LEN, 0},
	{0x9d, 0xe2, BYTE_BYTE_LEN, 0},
	{0x9e, 0x3d, BYTE_BYTE_LEN, 0},
	{0x9f, 0xe8, BYTE_BYTE_LEN, 0},
	           
	{0xa0, 0x3e, BYTE_BYTE_LEN, 0},
	{0xa1, 0xed, BYTE_BYTE_LEN, 0},
	{0xa2, 0x3f, BYTE_BYTE_LEN, 0},
	{0xa3, 0xf4, BYTE_BYTE_LEN, 0},
	{0xa4, 0x40, BYTE_BYTE_LEN, 0},
	{0xa5, 0xfa, BYTE_BYTE_LEN, 0},
	{0xa6, 0x41, BYTE_BYTE_LEN, 0},//GMMA33
	{0xa7, 0xff, BYTE_BYTE_LEN, 0},
	           
	{0xa8, 0x03, BYTE_BYTE_LEN, 0},//page 20 AE
	{0xa9, 0x20, BYTE_BYTE_LEN, 0},
	{0xaa, 0x39, BYTE_BYTE_LEN, 0},//??
	{0xab, 0x40, BYTE_BYTE_LEN, 0},
	{0xac, 0x03, BYTE_BYTE_LEN, 0},//Page 15 SHD
	{0xad, 0x15, BYTE_BYTE_LEN, 0},
	{0xae, 0x24, BYTE_BYTE_LEN, 0},//LSC OFFSET GB
	{0xaf, 0x00, BYTE_BYTE_LEN, 0},
	           
	{0xb0, 0x25, BYTE_BYTE_LEN, 0},//LSC OFFSET B
	{0xb1, 0x00, BYTE_BYTE_LEN, 0},
	{0xb2, 0x26, BYTE_BYTE_LEN, 0},//LSC OFFSET R
	{0xb3, 0x00, BYTE_BYTE_LEN, 0},
	{0xb4, 0x27, BYTE_BYTE_LEN, 0},//LSC OFFSET GR
	{0xb5, 0x00, BYTE_BYTE_LEN, 0},
	{0xb6, 0x28, BYTE_BYTE_LEN, 0},//LSC GAIN GB
	{0xb7, 0x80, BYTE_BYTE_LEN, 0},
	           
	{0xb8, 0x29, BYTE_BYTE_LEN, 0},//LSC GAIN B
	{0xb9, 0x80, BYTE_BYTE_LEN, 0},
	{0xba, 0x2a, BYTE_BYTE_LEN, 0},//LSC GAIN R
	{0xbb, 0x7a, BYTE_BYTE_LEN, 0},
	{0xbc, 0x2b, BYTE_BYTE_LEN, 0},//LSC GAIN GR
	{0xbd, 0x80, BYTE_BYTE_LEN, 0},
	{0xbe, 0x11, BYTE_BYTE_LEN, 0},//LSC CTL2 
	{0xbf, 0x40, BYTE_BYTE_LEN, 0},
	///////////////////////////////////
	// Page 0xDA(DMA Outdoor)
	///////////////////////////////////
	{0x03, 0xda, BYTE_BYTE_LEN, 0},
	
	{0x10, 0x03, BYTE_BYTE_LEN, 0},//11 page Noise Reduction
	{0x11, 0x11, BYTE_BYTE_LEN, 0},
	{0x12, 0x10, BYTE_BYTE_LEN, 0},//Dlpf ctl
	{0x13, 0x13, BYTE_BYTE_LEN, 0}, //Outdoor 1110
	{0x14, 0x11, BYTE_BYTE_LEN, 0},
	{0x15, 0x25, BYTE_BYTE_LEN, 0}, //Outdoor 1111
	{0x16, 0x12, BYTE_BYTE_LEN, 0},
	{0x17, 0x22, BYTE_BYTE_LEN, 0}, //Outdoor 1112
				
	{0x18, 0x13, BYTE_BYTE_LEN, 0},
	{0x19, 0x11, BYTE_BYTE_LEN, 0}, //Outdoor 1113
	{0x1a, 0x14, BYTE_BYTE_LEN, 0},
	{0x1b, 0x21, BYTE_BYTE_LEN, 0}, //Outdoor 1114
	{0x1c, 0x30, BYTE_BYTE_LEN, 0},
	{0x1d, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 1130
	{0x1e, 0x31, BYTE_BYTE_LEN, 0},
	{0x1f, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 1131
				
	{0x20, 0x32, BYTE_BYTE_LEN, 0},
	{0x21, 0x70, BYTE_BYTE_LEN, 0}, //Outdoor 1132
	{0x22, 0x33, BYTE_BYTE_LEN, 0},
	{0x23, 0x4a, BYTE_BYTE_LEN, 0}, //Outdoor 1133
	{0x24, 0x34, BYTE_BYTE_LEN, 0},
	{0x25, 0x2a, BYTE_BYTE_LEN, 0}, //Outdoor 1134
	{0x26, 0x35, BYTE_BYTE_LEN, 0},
	{0x27, 0x2b, BYTE_BYTE_LEN, 0}, //Outdoor 1135
	{0x28, 0x36, BYTE_BYTE_LEN, 0},
	{0x29, 0x16, BYTE_BYTE_LEN, 0}, //Outdoor 1136
	{0x2a, 0x37, BYTE_BYTE_LEN, 0},
	{0x2b, 0x1b, BYTE_BYTE_LEN, 0}, //Outdoor 1137
	{0x2c, 0x38, BYTE_BYTE_LEN, 0},
	{0x2d, 0x14, BYTE_BYTE_LEN, 0}, //Outdoor 1138

	{0x2e, 0x39, BYTE_BYTE_LEN, 0},
	{0x2f, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1139 : R2 Lum Lv 01 NR Gain
	{0x30, 0x3a, BYTE_BYTE_LEN, 0},
	{0x31, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 113a : R2 Lum Lv 02 NR Gain
	{0x32, 0x3b, BYTE_BYTE_LEN, 0},
	{0x33, 0x2c, BYTE_BYTE_LEN, 0}, //Outdoor 113b : R2 Lum Lv 03 NR Gain
	{0x34, 0x3c, BYTE_BYTE_LEN, 0},
	{0x35, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 113c : R2 Lum Lv 04 NR Gain
	{0x36, 0x3d, BYTE_BYTE_LEN, 0},
	{0x37, 0x38, BYTE_BYTE_LEN, 0}, //Outdoor 113d : R2 Lum Lv 05 NR Gain
				
	{0x38, 0x3e, BYTE_BYTE_LEN, 0},
	{0x39, 0x68, BYTE_BYTE_LEN, 0}, //Outdoor 113e : R2 Lum Lv 06 NR Gain
	{0x3a, 0x3f, BYTE_BYTE_LEN, 0},
	{0x3b, 0x68, BYTE_BYTE_LEN, 0}, //Outdoor 113f : R2 Lum Lv 07 NR Gain
	{0x3c, 0x40, BYTE_BYTE_LEN, 0},
	{0x3d, 0x68, BYTE_BYTE_LEN, 0}, //Outdoor 1140 : R2 Lum Lv 08 NR Gain
	{0x3e, 0x41, BYTE_BYTE_LEN, 0},
	{0x3f, 0x1a, BYTE_BYTE_LEN, 0}, //Outdoor 1141 : R2 Lum Lv 01 NR offset
				
	{0x40, 0x42, BYTE_BYTE_LEN, 0},
	{0x41, 0x1a, BYTE_BYTE_LEN, 0}, //Outdoor 1142 : R2 Lum Lv 02 NR offset
	{0x42, 0x43, BYTE_BYTE_LEN, 0},
	{0x43, 0x1a, BYTE_BYTE_LEN, 0}, //Outdoor 1143 : R2 Lum Lv 03 NR offset
	{0x44, 0x44, BYTE_BYTE_LEN, 0},
	{0x45, 0x30, BYTE_BYTE_LEN, 0}, //Outdoor 1144 : R2 Lum Lv 04 NR offset
	{0x46, 0x45, BYTE_BYTE_LEN, 0},
	{0x47, 0x30, BYTE_BYTE_LEN, 0}, //Outdoor 1145 : R2 Lum Lv 05 NR offset
				
	{0x48, 0x46, BYTE_BYTE_LEN, 0},
	{0x49, 0x40, BYTE_BYTE_LEN, 0}, //Outdoor 1146 : R2 Lum Lv 06 NR offset
	{0x4a, 0x47, BYTE_BYTE_LEN, 0},
	{0x4b, 0x40, BYTE_BYTE_LEN, 0}, //Outdoor 1147 : R2 Lum Lv 07 NR offset
	{0x4c, 0x48, BYTE_BYTE_LEN, 0},
	{0x4d, 0x40, BYTE_BYTE_LEN, 0}, //Outdoor 1148 : R2 Lum Lv 08 NR offset
	{0x4e, 0x49, BYTE_BYTE_LEN, 0},
	{0x4f, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 1149
				
	{0x50, 0x4a, BYTE_BYTE_LEN, 0},
	{0x51, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 114a
	{0x52, 0x4b, BYTE_BYTE_LEN, 0},
	{0x53, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 114b
	{0x54, 0x4c, BYTE_BYTE_LEN, 0},
	{0x55, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 114c
	{0x56, 0x4d, BYTE_BYTE_LEN, 0},
	{0x57, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 114d
				
	{0x58, 0x4e, BYTE_BYTE_LEN, 0},
	{0x59, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 114e
	{0x5a, 0x4f, BYTE_BYTE_LEN, 0},
	{0x5b, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 114f
	{0x5c, 0x50, BYTE_BYTE_LEN, 0},
	{0x5d, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 1150
	{0x5e, 0x51, BYTE_BYTE_LEN, 0},
	{0x5f, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1151
				
	{0x60, 0x52, BYTE_BYTE_LEN, 0},
	{0x61, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1152
	{0x62, 0x53, BYTE_BYTE_LEN, 0},
	{0x63, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1153
	{0x64, 0x54, BYTE_BYTE_LEN, 0},
	{0x65, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1154
	{0x66, 0x55, BYTE_BYTE_LEN, 0},
	{0x67, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1155
				
	{0x68, 0x56, BYTE_BYTE_LEN, 0},
	{0x69, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1156
	{0x6a, 0x57, BYTE_BYTE_LEN, 0},
	{0x6b, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1157
	{0x6c, 0x58, BYTE_BYTE_LEN, 0},
	{0x6d, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1158
	{0x6e, 0x59, BYTE_BYTE_LEN, 0},
	{0x6f, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 1159
				
	{0x70, 0x5a, BYTE_BYTE_LEN, 0},
	{0x71, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 115a
	{0x72, 0x5b, BYTE_BYTE_LEN, 0},
	{0x73, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 115b
	{0x74, 0x5c, BYTE_BYTE_LEN, 0},
	{0x75, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 115c
	{0x76, 0x5d, BYTE_BYTE_LEN, 0},
	{0x77, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 115d
				
	{0x78, 0x5e, BYTE_BYTE_LEN, 0},
	{0x79, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 115e
	{0x7a, 0x5f, BYTE_BYTE_LEN, 0},
	{0x7b, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 115f
	{0x7c, 0x60, BYTE_BYTE_LEN, 0},
	{0x7d, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 1160
	{0x7e, 0x61, BYTE_BYTE_LEN, 0},
	{0x7f, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 1161
				
	{0x80, 0x62, BYTE_BYTE_LEN, 0},
	{0x81, 0xfc, BYTE_BYTE_LEN, 0}, //Outdoor 1162
	{0x82, 0x63, BYTE_BYTE_LEN, 0},
	{0x83, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1163
	{0x84, 0x64, BYTE_BYTE_LEN, 0},
	{0x85, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 1164
	{0x86, 0x65, BYTE_BYTE_LEN, 0},
	{0x87, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 1165 Imp Lv1 Hi Gain
				
	{0x88, 0x66, BYTE_BYTE_LEN, 0},
	{0x89, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 1166 Imp Lv1 Middle Gain
	{0x8a, 0x67, BYTE_BYTE_LEN, 0},
	{0x8b, 0x1a, BYTE_BYTE_LEN, 0}, //Outdoor 1167 Imp Lv1 Low Gain
	{0x8c, 0x68, BYTE_BYTE_LEN, 0},
	{0x8d, 0x5a, BYTE_BYTE_LEN, 0}, //Outdoor 1168
	{0x8e, 0x69, BYTE_BYTE_LEN, 0},
	{0x8f, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 1169
				
	{0x90, 0x6a, BYTE_BYTE_LEN, 0},
	{0x91, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 116a Imp Lv2 Hi Gain
	{0x92, 0x6b, BYTE_BYTE_LEN, 0},
	{0x93, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 116b Imp Lv2 Middle Gain
	{0x94, 0x6c, BYTE_BYTE_LEN, 0},
	{0x95, 0x1a, BYTE_BYTE_LEN, 0}, //Outdoor 116c Imp Lv2 Low Gain
	{0x96, 0x6d, BYTE_BYTE_LEN, 0},
	{0x97, 0x5c, BYTE_BYTE_LEN, 0}, //Outdoor 116d
				
	{0x98, 0x6e, BYTE_BYTE_LEN, 0},
	{0x99, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 116e
	{0x9a, 0x6f, BYTE_BYTE_LEN, 0},
	{0x9b, 0x26, BYTE_BYTE_LEN, 0}, //Outdoor 116f Imp Lv3 Hi Gain
	{0x9c, 0x70, BYTE_BYTE_LEN, 0},
	{0x9d, 0x26, BYTE_BYTE_LEN, 0}, //Outdoor 1170 Imp Lv3 Middle Gain
	{0x9e, 0x71, BYTE_BYTE_LEN, 0},
	{0x9f, 0x30, BYTE_BYTE_LEN, 0}, //Outdoor 1171 Imp Lv3 Low Gain
				
	{0xa0, 0x72, BYTE_BYTE_LEN, 0},
	{0xa1, 0x5c, BYTE_BYTE_LEN, 0}, //Outdoor 1172
	{0xa2, 0x73, BYTE_BYTE_LEN, 0},
	{0xa3, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 1173
	{0xa4, 0x74, BYTE_BYTE_LEN, 0},
	{0xa5, 0x90, BYTE_BYTE_LEN, 0}, //Outdoor 1174 Imp Lv4 Hi Gain
	{0xa6, 0x75, BYTE_BYTE_LEN, 0},
	{0xa7, 0x90, BYTE_BYTE_LEN, 0}, //Outdoor 1175 Imp Lv4 Middle Gain
				
	{0xa8, 0x76, BYTE_BYTE_LEN, 0},
	{0xa9, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 1176 Imp Lv4 Low Gain
	{0xaa, 0x77, BYTE_BYTE_LEN, 0},
	{0xab, 0x40, BYTE_BYTE_LEN, 0}, //Outdoor 1177
	{0xac, 0x78, BYTE_BYTE_LEN, 0},
	{0xad, 0x26, BYTE_BYTE_LEN, 0}, //Outdoor 1178
	{0xae, 0x79, BYTE_BYTE_LEN, 0},
	{0xaf, 0x90, BYTE_BYTE_LEN, 0}, //Outdoor 1179 Imp Lv5 Hi Gain
				
	{0xb0, 0x7a, BYTE_BYTE_LEN, 0},
	{0xb1, 0x90, BYTE_BYTE_LEN, 0}, //Outdoor 117a Imp Lv5 Middle Gain
	{0xb2, 0x7b, BYTE_BYTE_LEN, 0},
	{0xb3, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 117b Imp Lv5 Low Gain
	{0xb4, 0x7c, BYTE_BYTE_LEN, 0},
	{0xb5, 0x38, BYTE_BYTE_LEN, 0}, //Outdoor 117c
	{0xb6, 0x7d, BYTE_BYTE_LEN, 0},
	{0xb7, 0x1c, BYTE_BYTE_LEN, 0}, //Outdoor 117d
				
	{0xb8, 0x7e, BYTE_BYTE_LEN, 0},
	{0xb9, 0x68, BYTE_BYTE_LEN, 0}, //Outdoor 117e Imp Lv6 Hi Gain
	{0xba, 0x7f, BYTE_BYTE_LEN, 0},
	{0xbb, 0x58, BYTE_BYTE_LEN, 0}, //Outdoor 117f Imp Lv6 Middle Gain
	{0xbc, 0x80, BYTE_BYTE_LEN, 0},
	{0xbd, 0x48, BYTE_BYTE_LEN, 0}, //Outdoor 1180 Imp Lv6 Low Gain
	{0xbe, 0x81, BYTE_BYTE_LEN, 0},
	{0xbf, 0x32, BYTE_BYTE_LEN, 0}, //Outdoor 1181
				
	{0xc0, 0x82, BYTE_BYTE_LEN, 0},
	{0xc1, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1182
	{0xc2, 0x83, BYTE_BYTE_LEN, 0},
	{0xc3, 0x68, BYTE_BYTE_LEN, 0}, //Outdoor 1183 Imp Lv7 Hi Gain
	{0xc4, 0x84, BYTE_BYTE_LEN, 0},
	{0xc5, 0x58, BYTE_BYTE_LEN, 0}, //Outdoor 1184 Imp Lv7 Middle Gain
	{0xc6, 0x85, BYTE_BYTE_LEN, 0},
	{0xc7, 0x48, BYTE_BYTE_LEN, 0}, //Outdoor 1185 Imp Lv7 Low Gain
				
	{0xc8, 0x86, BYTE_BYTE_LEN, 0},
	{0xc9, 0x1c, BYTE_BYTE_LEN, 0}, //Outdoor 1186
	{0xca, 0x87, BYTE_BYTE_LEN, 0},
	{0xcb, 0x08, BYTE_BYTE_LEN, 0}, //Outdoor 1187
	{0xcc, 0x88, BYTE_BYTE_LEN, 0},
	{0xcd, 0x64, BYTE_BYTE_LEN, 0}, //Outdoor 1188
	{0xce, 0x89, BYTE_BYTE_LEN, 0},
	{0xcf, 0x64, BYTE_BYTE_LEN, 0}, //Outdoor 1189
				
	{0xd0, 0x8a, BYTE_BYTE_LEN, 0},
	{0xd1, 0x48, BYTE_BYTE_LEN, 0}, //Outdoor 118a
	{0xd2, 0x90, BYTE_BYTE_LEN, 0},
	{0xd3, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 1190
	{0xd4, 0x91, BYTE_BYTE_LEN, 0},
	{0xd5, 0x48, BYTE_BYTE_LEN, 0}, //Outdoor 1191
	{0xd6, 0x92, BYTE_BYTE_LEN, 0},
	{0xd7, 0x00, BYTE_BYTE_LEN, 0}, //Outdoor 1192
				
	{0xd8, 0x93, BYTE_BYTE_LEN, 0},
	{0xd9, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 1193
	{0xda, 0x94, BYTE_BYTE_LEN, 0},
	{0xdb, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 1194
	{0xdc, 0x95, BYTE_BYTE_LEN, 0},
	{0xdd, 0x64, BYTE_BYTE_LEN, 0}, //Outdoor 1195
	{0xde, 0x96, BYTE_BYTE_LEN, 0},
	{0xdf, 0x14, BYTE_BYTE_LEN, 0}, //Outdoor 1196
				
	{0xe0, 0x97, BYTE_BYTE_LEN, 0},
	{0xe1, 0x90, BYTE_BYTE_LEN, 0}, //Outdoor 1197
	{0xe2, 0xb0, BYTE_BYTE_LEN, 0},
	{0xe3, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 11b0
	{0xe4, 0xb1, BYTE_BYTE_LEN, 0},
	{0xe5, 0x90, BYTE_BYTE_LEN, 0}, //Outdoor 11b1
	{0xe6, 0xb2, BYTE_BYTE_LEN, 0},
	{0xe7, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 11b2
				
	{0xe8, 0xb3, BYTE_BYTE_LEN, 0},
	{0xe9, 0x08, BYTE_BYTE_LEN, 0}, //Outdoor 11b3
	{0xea, 0xb4, BYTE_BYTE_LEN, 0},
	{0xeb, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 11b4
	{0xec, 0x03, BYTE_BYTE_LEN, 0},
	{0xed, 0x12, BYTE_BYTE_LEN, 0},
	{0xee, 0x10, BYTE_BYTE_LEN, 0},
	{0xef, 0x03, BYTE_BYTE_LEN, 0}, //Outdoor 1210
				
	{0xf0, 0x11, BYTE_BYTE_LEN, 0},
	{0xf1, 0x29, BYTE_BYTE_LEN, 0}, //Outdoor 1211
	{0xf2, 0x12, BYTE_BYTE_LEN, 0},
	{0xf3, 0x30, BYTE_BYTE_LEN, 0}, //Outdoor 1212
	{0xf4, 0x40, BYTE_BYTE_LEN, 0},
	{0xf5, 0x37, BYTE_BYTE_LEN, 0}, //Outdoor 1240
	{0xf6, 0x41, BYTE_BYTE_LEN, 0},
	{0xf7, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 1241
				
	{0xf8, 0x42, BYTE_BYTE_LEN, 0},
	{0xf9, 0x00, BYTE_BYTE_LEN, 0}, //Outdoor 1242
	{0xfa, 0x43, BYTE_BYTE_LEN, 0},
	{0xfb, 0x62, BYTE_BYTE_LEN, 0}, //Outdoor 1243
	{0xfc, 0x44, BYTE_BYTE_LEN, 0},
	{0xfd, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 1244


	// Page 0xdb		  
	{0x03, 0xdb, BYTE_BYTE_LEN, 0},				
	{0x10, 0x45, BYTE_BYTE_LEN, 0},
	{0x11, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 1245
	{0x12, 0x46, BYTE_BYTE_LEN, 0},
	{0x13, 0x40, BYTE_BYTE_LEN, 0}, //Outdoor 1246
	{0x14, 0x60, BYTE_BYTE_LEN, 0},
	{0x15, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 1260
	{0x16, 0x61, BYTE_BYTE_LEN, 0},
	{0x17, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 1261
				
	{0x18, 0x62, BYTE_BYTE_LEN, 0},
	{0x19, 0x4b, BYTE_BYTE_LEN, 0}, //Outdoor 1262
	{0x1a, 0x63, BYTE_BYTE_LEN, 0},
	{0x1b, 0x41, BYTE_BYTE_LEN, 0}, //Outdoor 1263
	{0x1c, 0x64, BYTE_BYTE_LEN, 0},
	{0x1d, 0x14, BYTE_BYTE_LEN, 0}, //Outdoor 1264
	{0x1e, 0x65, BYTE_BYTE_LEN, 0},
	{0x1f, 0x00, BYTE_BYTE_LEN, 0}, //Outdoor 1265
				
	{0x20, 0x68, BYTE_BYTE_LEN, 0},
	{0x21, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 1268
	{0x22, 0x69, BYTE_BYTE_LEN, 0},
	{0x23, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 1269
	{0x24, 0x6a, BYTE_BYTE_LEN, 0},
	{0x25, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 126a
	{0x26, 0x6b, BYTE_BYTE_LEN, 0},
	{0x27, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 126b
				
	{0x28, 0x6c, BYTE_BYTE_LEN, 0},
	{0x29, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 126c
	{0x2a, 0x6d, BYTE_BYTE_LEN, 0},
	{0x2b, 0x01, BYTE_BYTE_LEN, 0}, //Outdoor 126d
	{0x2c, 0x70, BYTE_BYTE_LEN, 0},
	{0x2d, 0x01, BYTE_BYTE_LEN, 0}, //Outdoor 1270
	{0x2e, 0x71, BYTE_BYTE_LEN, 0},
	{0x2f, 0x3d, BYTE_BYTE_LEN, 0}, //Outdoor 1271
				
	{0x30, 0x80, BYTE_BYTE_LEN, 0},
	{0x31, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 1280
	{0x32, 0x81, BYTE_BYTE_LEN, 0},
	{0x33, 0x8a, BYTE_BYTE_LEN, 0}, //Outdoor 1281
	{0x34, 0x82, BYTE_BYTE_LEN, 0},
	{0x35, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 1282
	{0x36, 0x83, BYTE_BYTE_LEN, 0},
	{0x37, 0x12, BYTE_BYTE_LEN, 0}, //Outdoor 1283
				
	{0x38, 0x84, BYTE_BYTE_LEN, 0},
	{0x39, 0xc8, BYTE_BYTE_LEN, 0}, //Outdoor 1284
	{0x3a, 0x85, BYTE_BYTE_LEN, 0},
	{0x3b, 0x92, BYTE_BYTE_LEN, 0}, //Outdoor 1285
	{0x3c, 0x86, BYTE_BYTE_LEN, 0},
	{0x3d, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 1286
	{0x3e, 0x87, BYTE_BYTE_LEN, 0},
	{0x3f, 0x00, BYTE_BYTE_LEN, 0}, //Outdoor 1287
				
	{0x40, 0x88, BYTE_BYTE_LEN, 0},
	{0x41, 0x70, BYTE_BYTE_LEN, 0}, //Outdoor 1288 lmt center
	{0x42, 0x89, BYTE_BYTE_LEN, 0},
	{0x43, 0xaa, BYTE_BYTE_LEN, 0}, //Outdoor 1289
	{0x44, 0x8a, BYTE_BYTE_LEN, 0},
	{0x45, 0x50, BYTE_BYTE_LEN, 0}, //Outdoor 128a
	{0x46, 0x8b, BYTE_BYTE_LEN, 0},
	{0x47, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 128b
				
	{0x48, 0x8c, BYTE_BYTE_LEN, 0},
	{0x49, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 128c
	{0x4a, 0x8d, BYTE_BYTE_LEN, 0},
	{0x4b, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 128d
	{0x4c, 0xe6, BYTE_BYTE_LEN, 0},
	{0x4d, 0xff, BYTE_BYTE_LEN, 0}, //Outdoor 12e6
	{0x4e, 0xe7, BYTE_BYTE_LEN, 0},
	{0x4f, 0x18, BYTE_BYTE_LEN, 0}, //Outdoor 12e7
				
	{0x50, 0xe8, BYTE_BYTE_LEN, 0},
	{0x51, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 12e8
	{0x52, 0xe9, BYTE_BYTE_LEN, 0},
	{0x53, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 12e9
	{0x54, 0x03, BYTE_BYTE_LEN, 0},
	{0x55, 0x13, BYTE_BYTE_LEN, 0},
	{0x56, 0x10, BYTE_BYTE_LEN, 0},
	{0x57, 0x3f, BYTE_BYTE_LEN, 0}, //Outdoor 1310 Skin Color On
				
	{0x58, 0x20, BYTE_BYTE_LEN, 0},
	{0x59, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 1320
	{0x5a, 0x21, BYTE_BYTE_LEN, 0},
	{0x5b, 0x30, BYTE_BYTE_LEN, 0}, //Outdoor 1321
	{0x5c, 0x22, BYTE_BYTE_LEN, 0},
	{0x5d, 0x36, BYTE_BYTE_LEN, 0}, //Outdoor 1322
	{0x5e, 0x23, BYTE_BYTE_LEN, 0},
	{0x5f, 0x6a, BYTE_BYTE_LEN, 0}, //Outdoor 1323
				
	{0x60, 0x24, BYTE_BYTE_LEN, 0},
	{0x61, 0xa0, BYTE_BYTE_LEN, 0}, //Outdoor 1324
	{0x62, 0x25, BYTE_BYTE_LEN, 0},
	{0x63, 0xc0, BYTE_BYTE_LEN, 0}, //Outdoor 1325
	{0x64, 0x26, BYTE_BYTE_LEN, 0},
	{0x65, 0xe0, BYTE_BYTE_LEN, 0}, //Outdoor 1326
	{0x66, 0x27, BYTE_BYTE_LEN, 0},
	{0x67, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 1327
				
	{0x68, 0x28, BYTE_BYTE_LEN, 0},
	{0x69, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 1328
	{0x6a, 0x29, BYTE_BYTE_LEN, 0},
	{0x6b, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 1329
	{0x6c, 0x2a, BYTE_BYTE_LEN, 0},
	{0x6d, 0x08, BYTE_BYTE_LEN, 0}, //Outdoor 132a
	{0x6e, 0x2b, BYTE_BYTE_LEN, 0},
	{0x6f, 0x06, BYTE_BYTE_LEN, 0}, //Outdoor 132b
				
	{0x70, 0x2c, BYTE_BYTE_LEN, 0},
	{0x71, 0x05, BYTE_BYTE_LEN, 0}, //Outdoor 132c
	{0x72, 0x2d, BYTE_BYTE_LEN, 0},
	{0x73, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 132d
	{0x74, 0x2e, BYTE_BYTE_LEN, 0},
	{0x75, 0x03, BYTE_BYTE_LEN, 0}, //Outdoor 132e
	{0x76, 0x2f, BYTE_BYTE_LEN, 0},
	{0x77, 0x01, BYTE_BYTE_LEN, 0}, //Outdoor 132f weight skin
				
	{0x78, 0x30, BYTE_BYTE_LEN, 0},
	{0x79, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 1330 weight blue
	{0x7a, 0x31, BYTE_BYTE_LEN, 0},
	{0x7b, 0x78, BYTE_BYTE_LEN, 0}, //Outdoor 1331 weight green
	{0x7c, 0x32, BYTE_BYTE_LEN, 0},
	{0x7d, 0x01, BYTE_BYTE_LEN, 0}, //Outdoor 1332 weight strong color
	{0x7e, 0x33, BYTE_BYTE_LEN, 0},
	{0x7f, 0x80, BYTE_BYTE_LEN, 0}, //Outdoor 1333
				
	{0x80, 0x34, BYTE_BYTE_LEN, 0},
	{0x81, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 1334
	{0x82, 0x35, BYTE_BYTE_LEN, 0},
	{0x83, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1335
	{0x84, 0x36, BYTE_BYTE_LEN, 0},
	{0x85, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 1336
	{0x86, 0xa0, BYTE_BYTE_LEN, 0},
	{0x87, 0x07, BYTE_BYTE_LEN, 0}, //Outdoor 13a0
				
	{0x88, 0xa8, BYTE_BYTE_LEN, 0},
	{0x89, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 13a8 Outdoor Cb-filter
	{0x8a, 0xa9, BYTE_BYTE_LEN, 0},
	{0x8b, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 13a9 Outdoor Cr-filter
	{0x8c, 0xaa, BYTE_BYTE_LEN, 0},
	{0x8d, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 13aa
	{0x8e, 0xab, BYTE_BYTE_LEN, 0},
	{0x8f, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 13ab
				
	{0x90, 0xc0, BYTE_BYTE_LEN, 0},
	{0x91, 0x27, BYTE_BYTE_LEN, 0}, //Outdoor 13c0
	{0x92, 0xc2, BYTE_BYTE_LEN, 0},
	{0x93, 0x08, BYTE_BYTE_LEN, 0}, //Outdoor 13c2
	{0x94, 0xc3, BYTE_BYTE_LEN, 0},
	{0x95, 0x08, BYTE_BYTE_LEN, 0}, //Outdoor 13c3
	{0x96, 0xc4, BYTE_BYTE_LEN, 0},
	{0x97, 0x40, BYTE_BYTE_LEN, 0}, //Outdoor 13c4 Green Detect
				
	{0x98, 0xc5, BYTE_BYTE_LEN, 0},
	{0x99, 0x78, BYTE_BYTE_LEN, 0}, //Outdoor 13c5
	{0x9a, 0xc6, BYTE_BYTE_LEN, 0},
	{0x9b, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 13c6
	{0x9c, 0xc7, BYTE_BYTE_LEN, 0},
	{0x9d, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 13c7
	{0x9e, 0xc8, BYTE_BYTE_LEN, 0},
	{0x9f, 0x44, BYTE_BYTE_LEN, 0}, //Outdoor 13c8
				
	{0xa0, 0xc9, BYTE_BYTE_LEN, 0},
	{0xa1, 0x87, BYTE_BYTE_LEN, 0}, //Outdoor 13c9
	{0xa2, 0xca, BYTE_BYTE_LEN, 0},
	{0xa3, 0xff, BYTE_BYTE_LEN, 0}, //Outdoor 13ca
	{0xa4, 0xcb, BYTE_BYTE_LEN, 0},
	{0xa5, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 13cb
	{0xa6, 0xcc, BYTE_BYTE_LEN, 0},
	{0xa7, 0x61, BYTE_BYTE_LEN, 0}, //Outdoor 13cc skin range_cb_l
				
	{0xa8, 0xcd, BYTE_BYTE_LEN, 0},
	{0xa9, 0x87, BYTE_BYTE_LEN, 0}, //Outdoor 13cd skin range_cb_h
	{0xaa, 0xce, BYTE_BYTE_LEN, 0},
	{0xab, 0x8a, BYTE_BYTE_LEN, 0}, //Outdoor 13ce skin range_cr_l
	{0xac, 0xcf, BYTE_BYTE_LEN, 0},
	{0xad, 0xa5, BYTE_BYTE_LEN, 0}, //Outdoor 13cf skin range_cr_h
	{0xae, 0x03, BYTE_BYTE_LEN, 0},
	{0xaf, 0x14, BYTE_BYTE_LEN, 0},
				
	{0xb0, 0x11, BYTE_BYTE_LEN, 0},
	{0xb1, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 1410
	{0xb2, 0x11, BYTE_BYTE_LEN, 0},
	{0xb3, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 1411
	{0xb4, 0x12, BYTE_BYTE_LEN, 0},
	{0xb5, 0x60, BYTE_BYTE_LEN, 0}, //Outdoor 1412 Top H_Clip
	{0xb6, 0x13, BYTE_BYTE_LEN, 0},
	{0xb7, 0x62, BYTE_BYTE_LEN, 0}, //Outdoor 1413
	{0xb8, 0x14, BYTE_BYTE_LEN, 0},
	{0xb9, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 1414
	{0xba, 0x15, BYTE_BYTE_LEN, 0},
	{0xbb, 0x30, BYTE_BYTE_LEN, 0}, //Outdoor 1415 sharp positive ya
	{0xbc, 0x16, BYTE_BYTE_LEN, 0},
	{0xbd, 0x30, BYTE_BYTE_LEN, 0}, //Outdoor 1416 sharp positive mi
	{0xbe, 0x17, BYTE_BYTE_LEN, 0},
	{0xbf, 0x30, BYTE_BYTE_LEN, 0}, //Outdoor 1417 sharp positive low
	{0xc0, 0x18, BYTE_BYTE_LEN, 0},
	{0xc1, 0x54, BYTE_BYTE_LEN, 0}, //Outdoor 1418 sharp negative ya
	{0xc2, 0x19, BYTE_BYTE_LEN, 0},
	{0xc3, 0x54, BYTE_BYTE_LEN, 0}, //Outdoor 1419 sharp negative mi
	{0xc4, 0x1a, BYTE_BYTE_LEN, 0},
	{0xc5, 0x40, BYTE_BYTE_LEN, 0}, //Outdoor 141a sharp negative low
	{0xc6, 0x20, BYTE_BYTE_LEN, 0},
	{0xc7, 0xd2, BYTE_BYTE_LEN, 0}, //Outdoor 1420
	{0xc8, 0x21, BYTE_BYTE_LEN, 0},
	{0xc9, 0x03, BYTE_BYTE_LEN, 0}, //Outdoor 1421
	{0xca, 0x22, BYTE_BYTE_LEN, 0},
	{0xcb, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 1422
	{0xcc, 0x23, BYTE_BYTE_LEN, 0},
	{0xcd, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 1423
	{0xce, 0x24, BYTE_BYTE_LEN, 0},
	{0xcf, 0x05, BYTE_BYTE_LEN, 0}, //Outdoor 1424
	{0xd0, 0x25, BYTE_BYTE_LEN, 0},
	{0xd1, 0x32, BYTE_BYTE_LEN, 0}, //Outdoor 1425
	{0xd2, 0x26, BYTE_BYTE_LEN, 0},
	{0xd3, 0x30, BYTE_BYTE_LEN, 0}, //Outdoor 1426
	{0xd4, 0x27, BYTE_BYTE_LEN, 0},
	{0xd5, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 1427
	{0xd6, 0x28, BYTE_BYTE_LEN, 0},
	{0xd7, 0x3c, BYTE_BYTE_LEN, 0}, //Outdoor 1428
	{0xd8, 0x29, BYTE_BYTE_LEN, 0},
	{0xd9, 0x00, BYTE_BYTE_LEN, 0}, //Outdoor 1429
	{0xda, 0x2a, BYTE_BYTE_LEN, 0},
	{0xdb, 0x16, BYTE_BYTE_LEN, 0}, //Outdoor 142a S_Diff Hi Dr_Gain
	{0xdc, 0x2b, BYTE_BYTE_LEN, 0},
	{0xdd, 0x16, BYTE_BYTE_LEN, 0}, //Outdoor 142b S_Diff Middle Dr_Gain
	{0xde, 0x2c, BYTE_BYTE_LEN, 0},
	{0xdf, 0x16, BYTE_BYTE_LEN, 0}, //Outdoor 142c S_Diff Low Dr_Gain
	{0xe0, 0x2d, BYTE_BYTE_LEN, 0},
	{0xe1, 0x54, BYTE_BYTE_LEN, 0}, //Outdoor 142d
	{0xe2, 0x2e, BYTE_BYTE_LEN, 0},
	{0xe3, 0x54, BYTE_BYTE_LEN, 0}, //Outdoor 142e
	{0xe4, 0x2f, BYTE_BYTE_LEN, 0},
	{0xe5, 0x54, BYTE_BYTE_LEN, 0}, //Outdoor 142f
	{0xe6, 0x30, BYTE_BYTE_LEN, 0},
	{0xe7, 0x82, BYTE_BYTE_LEN, 0}, //Outdoor 1430
	{0xe8, 0x31, BYTE_BYTE_LEN, 0},
	{0xe9, 0x02, BYTE_BYTE_LEN, 0}, //Outdoor 1431
	{0xea, 0x32, BYTE_BYTE_LEN, 0},
	{0xeb, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 1432
	{0xec, 0x33, BYTE_BYTE_LEN, 0},
	{0xed, 0x04, BYTE_BYTE_LEN, 0}, //Outdoor 1433
	{0xee, 0x34, BYTE_BYTE_LEN, 0},
	{0xef, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 1434
	{0xf0, 0x35, BYTE_BYTE_LEN, 0},
	{0xf1, 0x46, BYTE_BYTE_LEN, 0}, //Outdoor 1435
	{0xf2, 0x36, BYTE_BYTE_LEN, 0},
	{0xf3, 0x32, BYTE_BYTE_LEN, 0}, //Outdoor 1436
	{0xf4, 0x37, BYTE_BYTE_LEN, 0},
	{0xf5, 0x28, BYTE_BYTE_LEN, 0}, //Outdoor 1437
	{0xf6, 0x38, BYTE_BYTE_LEN, 0},
	{0xf7, 0x12, BYTE_BYTE_LEN, 0}, //Outdoor 1438
	{0xf8, 0x39, BYTE_BYTE_LEN, 0},
	{0xf9, 0x00, BYTE_BYTE_LEN, 0}, //Outdoor 1439
	{0xfa, 0x3a, BYTE_BYTE_LEN, 0},
	{0xfb, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 143a L_Diff Hi Dr_Gain
	{0xfc, 0x3b, BYTE_BYTE_LEN, 0},
	{0xfd, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 143b L_Diff Middle Dr_Gain
	   
	{0x03, 0xdc, BYTE_BYTE_LEN, 0},
	{0x10, 0x3c, BYTE_BYTE_LEN, 0},
	{0x11, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 143c L_Diff Low Dr_Gain
	{0x12, 0x3d, BYTE_BYTE_LEN, 0},
	{0x13, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 143d L_Diff Hi_Normal Gain
	{0x14, 0x3e, BYTE_BYTE_LEN, 0},
	{0x15, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 143e L_Diff Middle_Normal Gain
	{0x16, 0x3f, BYTE_BYTE_LEN, 0},
	{0x17, 0x14, BYTE_BYTE_LEN, 0}, //Outdoor 143f L_Diff Low_Normal Gain
	{0x18, 0x40, BYTE_BYTE_LEN, 0},
	{0x19, 0x82, BYTE_BYTE_LEN, 0}, //Outdoor 1440
	{0x1a, 0x41, BYTE_BYTE_LEN, 0},
	{0x1b, 0x1a, BYTE_BYTE_LEN, 0}, //Outdoor 1441 H Clip
	{0x1c, 0x42, BYTE_BYTE_LEN, 0},
	{0x1d, 0x70, BYTE_BYTE_LEN, 0}, //Outdoor 1442
	{0x1e, 0x43, BYTE_BYTE_LEN, 0},
	{0x1f, 0x20, BYTE_BYTE_LEN, 0}, //Outdoor 1443
	{0x20, 0x44, BYTE_BYTE_LEN, 0},
	{0x21, 0x26, BYTE_BYTE_LEN, 0}, //Outdoor 1444 M_Diff Hi Dr_Gain
	{0x22, 0x45, BYTE_BYTE_LEN, 0},
	{0x23, 0x1e, BYTE_BYTE_LEN, 0}, //Outdoor 1445 M_Diff Low Dr_Gain
	{0x24, 0x46, BYTE_BYTE_LEN, 0},
	{0x25, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 1446 M_Diff Middle Dr_Gain
	{0x26, 0x47, BYTE_BYTE_LEN, 0},
	{0x27, 0x18, BYTE_BYTE_LEN, 0}, //Outdoor 1447
	{0x28, 0x48, BYTE_BYTE_LEN, 0},
	{0x29, 0x0a, BYTE_BYTE_LEN, 0}, //Outdoor 1448
	{0x2a, 0x49, BYTE_BYTE_LEN, 0},
	{0x2b, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1449
	{0x2c, 0x50, BYTE_BYTE_LEN, 0},
	{0x2d, 0x82, BYTE_BYTE_LEN, 0}, //Outdoor 1450
	{0x2e, 0x51, BYTE_BYTE_LEN, 0},
	{0x2f, 0x38, BYTE_BYTE_LEN, 0}, //Outdoor 1451
	{0x30, 0x52, BYTE_BYTE_LEN, 0},
	{0x31, 0xf0, BYTE_BYTE_LEN, 0}, //Outdoor 1452
	{0x32, 0x53, BYTE_BYTE_LEN, 0},
	{0x33, 0x78, BYTE_BYTE_LEN, 0}, //Outdoor 1453
	{0x34, 0x54, BYTE_BYTE_LEN, 0},
	{0x35, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 1454
	{0x36, 0x55, BYTE_BYTE_LEN, 0},
	{0x37, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 1455
	{0x38, 0x56, BYTE_BYTE_LEN, 0},
	{0x39, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor 1456
	{0x3a, 0x57, BYTE_BYTE_LEN, 0},
	{0x3b, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1457
	{0x3c, 0x58, BYTE_BYTE_LEN, 0},
	{0x3d, 0x14, BYTE_BYTE_LEN, 0}, //Outdoor 1458
	{0x3e, 0x59, BYTE_BYTE_LEN, 0},
	{0x3f, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1459
				
	{0x40, 0x60, BYTE_BYTE_LEN, 0},
	{0x41, 0x01, BYTE_BYTE_LEN, 0}, //Outdoor 1460 skin opt
	{0x42, 0x61, BYTE_BYTE_LEN, 0},
	{0x43, 0xa0, BYTE_BYTE_LEN, 0}, //Outdoor 1461
	{0x44, 0x62, BYTE_BYTE_LEN, 0},
	{0x45, 0x98, BYTE_BYTE_LEN, 0}, //Outdoor 1462
	{0x46, 0x63, BYTE_BYTE_LEN, 0},
	{0x47, 0xe4, BYTE_BYTE_LEN, 0}, //Outdoor 1463 skin_std_th_h
				
	{0x48, 0x64, BYTE_BYTE_LEN, 0},
	{0x49, 0xa4, BYTE_BYTE_LEN, 0}, //Outdoor 1464 skin_std_th_l
	{0x4a, 0x65, BYTE_BYTE_LEN, 0},
	{0x4b, 0x7d, BYTE_BYTE_LEN, 0}, //Outdoor 1465 sharp_std_th_h
	{0x4c, 0x66, BYTE_BYTE_LEN, 0},
	{0x4d, 0x4b, BYTE_BYTE_LEN, 0}, //Outdoor 1466 sharp_std_th_l
	{0x4e, 0x70, BYTE_BYTE_LEN, 0},
	{0x4f, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1470
	       
	{0x50, 0x71, BYTE_BYTE_LEN, 0},
	{0x51, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1471
	{0x52, 0x72, BYTE_BYTE_LEN, 0},
	{0x53, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1472
	{0x54, 0x73, BYTE_BYTE_LEN, 0},
	{0x55, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1473
	{0x56, 0x74, BYTE_BYTE_LEN, 0},
	{0x57, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1474
				
	{0x58, 0x75, BYTE_BYTE_LEN, 0},
	{0x59, 0x10, BYTE_BYTE_LEN, 0}, //Outdoor 1475
	{0x5a, 0x76, BYTE_BYTE_LEN, 0},
	{0x5b, 0x38, BYTE_BYTE_LEN, 0}, //Outdoor 1476 green sharp pos High
	{0x5c, 0x77, BYTE_BYTE_LEN, 0},
	{0x5d, 0x38, BYTE_BYTE_LEN, 0}, //Outdoor 1477 green sharp pos Middle
	{0x5e, 0x78, BYTE_BYTE_LEN, 0},
	{0x5f, 0x38, BYTE_BYTE_LEN, 0}, //Outdoor 1478 green sharp pos Low
				
	{0x60, 0x79, BYTE_BYTE_LEN, 0},
	{0x61, 0x58, BYTE_BYTE_LEN, 0}, //Outdoor 1479 green sharp nega High
	{0x62, 0x7a, BYTE_BYTE_LEN, 0},
	{0x63, 0x58, BYTE_BYTE_LEN, 0}, //Outdoor 147a green sharp nega Middle
	{0x64, 0x7b, BYTE_BYTE_LEN, 0},
	{0x65, 0x58, BYTE_BYTE_LEN, 0}, //Outdoor 147b green sharp nega Low


	//////////////////
	// dd Page (DMA Indoor)
	//////////////////
	{0x03, 0xdd, BYTE_BYTE_LEN, 0},
	{0x10, 0x03, BYTE_BYTE_LEN, 0},//Indoor Page11 D-LPF
	{0x11, 0x11, BYTE_BYTE_LEN, 0},
	{0x12, 0x10, BYTE_BYTE_LEN, 0},//Indoor 0x1110
	{0x13, 0x13, BYTE_BYTE_LEN, 0},
	{0x14, 0x11, BYTE_BYTE_LEN, 0},//Indoor 0x1111
	{0x15, 0x0c, BYTE_BYTE_LEN, 0},
	{0x16, 0x12, BYTE_BYTE_LEN, 0},//Indoor 0x1112
	{0x17, 0x22, BYTE_BYTE_LEN, 0},
	{0x18, 0x13, BYTE_BYTE_LEN, 0},//Indoor 0x1113
	{0x19, 0x22, BYTE_BYTE_LEN, 0},
	{0x1a, 0x14, BYTE_BYTE_LEN, 0},//Indoor 0x1114
	{0x1b, 0x3a, BYTE_BYTE_LEN, 0},
	{0x1c, 0x30, BYTE_BYTE_LEN, 0},//Indoor 0x1130
	{0x1d, 0x20, BYTE_BYTE_LEN, 0},
	{0x1e, 0x31, BYTE_BYTE_LEN, 0},//Indoor 0x1131
	{0x1f, 0x20, BYTE_BYTE_LEN, 0},

	{0x20, 0x32, BYTE_BYTE_LEN, 0}, //Indoor 0x1132 //STEVE Lum. Level. in DLPF                    
	{0x21, 0x8b, BYTE_BYTE_LEN, 0}, //52                                                          
	{0x22, 0x33, BYTE_BYTE_LEN, 0}, //Indoor 0x1133                                               
	{0x23, 0x54, BYTE_BYTE_LEN, 0}, //3b                                                          
	{0x24, 0x34, BYTE_BYTE_LEN, 0}, //Indoor 0x1134                                               
	{0x25, 0x2c, BYTE_BYTE_LEN, 0}, //1d                                                          
	{0x26, 0x35, BYTE_BYTE_LEN, 0}, //Indoor 0x1135                                               
	{0x27, 0x29, BYTE_BYTE_LEN, 0}, //21                                                          
	{0x28, 0x36, BYTE_BYTE_LEN, 0}, //Indoor 0x1136                                               
	{0x29, 0x18, BYTE_BYTE_LEN, 0}, //1b                                                          
	{0x2a, 0x37, BYTE_BYTE_LEN, 0}, //Indoor 0x1137                                               
	{0x2b, 0x1e, BYTE_BYTE_LEN, 0}, //21                                                          
	{0x2c, 0x38, BYTE_BYTE_LEN, 0}, //Indoor 0x1138                                               
	{0x2d, 0x17, BYTE_BYTE_LEN, 0}, //18                                                          
	
	{0x2e, 0x39, BYTE_BYTE_LEN, 0},//Indoor 0x1139 gain 1           
	{0x2f, 0x34, BYTE_BYTE_LEN, 0},    //r2 1
	{0x30, 0x3a, BYTE_BYTE_LEN, 0},//Indoor 0x113a
	{0x31, 0x38, BYTE_BYTE_LEN, 0},
	{0x32, 0x3b, BYTE_BYTE_LEN, 0},//Indoor 0x113b
	{0x33, 0x3a, BYTE_BYTE_LEN, 0},
	{0x34, 0x3c, BYTE_BYTE_LEN, 0},//Indoor 0x113c
	{0x35, 0x38, BYTE_BYTE_LEN, 0},   //18
	{0x36, 0x3d, BYTE_BYTE_LEN, 0},//Indoor 0x113d
	{0x37, 0x2a, BYTE_BYTE_LEN, 0},   //18
	{0x38, 0x3e, BYTE_BYTE_LEN, 0},//Indoor 0x113e
	{0x39, 0x26, BYTE_BYTE_LEN, 0},   //18
	{0x3a, 0x3f, BYTE_BYTE_LEN, 0},//Indoor 0x113f
	{0x3b, 0x22, BYTE_BYTE_LEN, 0},
	{0x3c, 0x40, BYTE_BYTE_LEN, 0},//Indoor 0x1140 gain 8
	{0x3d, 0x20, BYTE_BYTE_LEN, 0},
	{0x3e, 0x41, BYTE_BYTE_LEN, 0},//Indoor 0x1141
	{0x3f, 0x20, BYTE_BYTE_LEN, 0}, //50, BYTE_BYTE_LEN, 0},
	{0x40, 0x42, BYTE_BYTE_LEN, 0},//Indoor 0x1142
	{0x41, 0x20, BYTE_BYTE_LEN, 0}, //50, BYTE_BYTE_LEN, 0},
	{0x42, 0x43, BYTE_BYTE_LEN, 0},//Indoor 0x1143
	{0x43, 0x20, BYTE_BYTE_LEN, 0}, //50, BYTE_BYTE_LEN, 0},
	{0x44, 0x44, BYTE_BYTE_LEN, 0},//Indoor 0x1144
	{0x45, 0x20, BYTE_BYTE_LEN, 0}, //80, BYTE_BYTE_LEN, 0},
	{0x46, 0x45, BYTE_BYTE_LEN, 0},//Indoor 0x1145
	{0x47, 0x20, BYTE_BYTE_LEN, 0}, //80, BYTE_BYTE_LEN, 0},
	{0x48, 0x46, BYTE_BYTE_LEN, 0},//Indoor 0x1146
	{0x49, 0x20, BYTE_BYTE_LEN, 0}, //80, BYTE_BYTE_LEN, 0},
	{0x4a, 0x47, BYTE_BYTE_LEN, 0},//Indoor 0x1147
	{0x4b, 0x20, BYTE_BYTE_LEN, 0}, //80, BYTE_BYTE_LEN, 0},
	{0x4c, 0x48, BYTE_BYTE_LEN, 0},//Indoor 0x1148
	{0x4d, 0x20, BYTE_BYTE_LEN, 0}, //80, BYTE_BYTE_LEN, 0},
	{0x4e, 0x49, BYTE_BYTE_LEN, 0},//Indoor 0x1149
	{0x4f, 0xf0, BYTE_BYTE_LEN, 0}, //fc, BYTE_BYTE_LEN, 0}, //high_clip_start
	{0x50, 0x4a, BYTE_BYTE_LEN, 0},//Indoor 0x114a
	{0x51, 0xf0, BYTE_BYTE_LEN, 0}, //fc, BYTE_BYTE_LEN, 0},
	{0x52, 0x4b, BYTE_BYTE_LEN, 0},//Indoor 0x114b
	{0x53, 0xf0, BYTE_BYTE_LEN, 0}, //fc, BYTE_BYTE_LEN, 0},
	{0x54, 0x4c, BYTE_BYTE_LEN, 0},//Indoor 0x114c //NR Gain MAx Limit
	{0x55, 0xf0, BYTE_BYTE_LEN, 0},//0224 jk//0xfc, BYTE_BYTE_LEN, 0},
	{0x56, 0x4d, BYTE_BYTE_LEN, 0},//Indoor 0x114d
	{0x57, 0xf0, BYTE_BYTE_LEN, 0}, //fc, BYTE_BYTE_LEN, 0},
	{0x58, 0x4e, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//Indoor 0x114e
	{0x59, 0xf0, BYTE_BYTE_LEN, 0},   //Lv 6 h_clip
	{0x5a, 0x4f, BYTE_BYTE_LEN, 0},//Indoor 0x114f
	{0x5b, 0xf0, BYTE_BYTE_LEN, 0},   //Lv 7 h_clip 
	{0x5c, 0x50, BYTE_BYTE_LEN, 0},//Indoor 0x1150 clip 8
	{0x5d, 0xf0, BYTE_BYTE_LEN, 0},
	{0x5e, 0x51, BYTE_BYTE_LEN, 0},//Indoor 0x1151
	{0x5f, 0x68, BYTE_BYTE_LEN, 0}, //test//0x08, BYTE_BYTE_LEN, 0}, //color gain start
	{0x60, 0x52, BYTE_BYTE_LEN, 0},//Indoor 0x1152
	{0x61, 0x68, BYTE_BYTE_LEN, 0}, //test//0x08, BYTE_BYTE_LEN, 0},
	{0x62, 0x53, BYTE_BYTE_LEN, 0},//Indoor 0x1153
	{0x63, 0x68, BYTE_BYTE_LEN, 0}, //test//0x08, BYTE_BYTE_LEN, 0},
	{0x64, 0x54, BYTE_BYTE_LEN, 0},//Indoor 0x1154
	{0x65, 0x68, BYTE_BYTE_LEN, 0}, //test//0x08, BYTE_BYTE_LEN, 0},
	{0x66, 0x55, BYTE_BYTE_LEN, 0},//Indoor 0x1155
	{0x67, 0x68, BYTE_BYTE_LEN, 0}, //test//0x08, BYTE_BYTE_LEN, 0},
	{0x68, 0x56, BYTE_BYTE_LEN, 0},//Indoor 0x1156
	{0x69, 0x68, BYTE_BYTE_LEN, 0}, //test//0x08, BYTE_BYTE_LEN, 0},
	{0x6a, 0x57, BYTE_BYTE_LEN, 0},//Indoor 0x1157
	{0x6b, 0x68, BYTE_BYTE_LEN, 0}, //test//0x08, BYTE_BYTE_LEN, 0},
	{0x6c, 0x58, BYTE_BYTE_LEN, 0},//Indoor 0x1158
	{0x6d, 0x68, BYTE_BYTE_LEN, 0}, //test//0x08, BYTE_BYTE_LEN, 0}, //color gain end
	{0x6e, 0x59, BYTE_BYTE_LEN, 0},//Indoor 0x1159
	{0x6f, 0x80, BYTE_BYTE_LEN, 0}, //test//0x10, BYTE_BYTE_LEN, 0}, //color ofs lmt start
	{0x70, 0x5a, BYTE_BYTE_LEN, 0},//Indoor 0x115a
	{0x71, 0x80, BYTE_BYTE_LEN, 0}, //test//0x10, BYTE_BYTE_LEN, 0},
	{0x72, 0x5b, BYTE_BYTE_LEN, 0},//Indoor 0x115b
	{0x73, 0x80, BYTE_BYTE_LEN, 0}, //test//0x10, BYTE_BYTE_LEN, 0},
	{0x74, 0x5c, BYTE_BYTE_LEN, 0},//Indoor 0x115c
	{0x75, 0x80, BYTE_BYTE_LEN, 0}, //test//0x10, BYTE_BYTE_LEN, 0},
	{0x76, 0x5d, BYTE_BYTE_LEN, 0},//Indoor 0x115d
	{0x77, 0x80, BYTE_BYTE_LEN, 0}, //test//0x10, BYTE_BYTE_LEN, 0},
	{0x78, 0x5e, BYTE_BYTE_LEN, 0},//Indoor 0x115e
	{0x79, 0x80, BYTE_BYTE_LEN, 0}, //test//0x10, BYTE_BYTE_LEN, 0},
	{0x7a, 0x5f, BYTE_BYTE_LEN, 0},//Indoor 0x115f
	{0x7b, 0x80, BYTE_BYTE_LEN, 0}, //test//0x10, BYTE_BYTE_LEN, 0},
	{0x7c, 0x60, BYTE_BYTE_LEN, 0},//Indoor 0x1160
	{0x7d, 0x80, BYTE_BYTE_LEN, 0}, //test//0x10, BYTE_BYTE_LEN, 0},//color ofs lmt end
	{0x7e, 0x61, BYTE_BYTE_LEN, 0},//Indoor 0x1161
	{0x7f, 0xc0, BYTE_BYTE_LEN, 0},
	{0x80, 0x62, BYTE_BYTE_LEN, 0},//Indoor 0x1162
	{0x81, 0xf0, BYTE_BYTE_LEN, 0},
	{0x82, 0x63, BYTE_BYTE_LEN, 0},//Indoor 0x1163
	{0x83, 0x80, BYTE_BYTE_LEN, 0},
	{0x84, 0x64, BYTE_BYTE_LEN, 0},//Indoor 0x1164
	{0x85, 0x40, BYTE_BYTE_LEN, 0},
	{0x86, 0x65, BYTE_BYTE_LEN, 0},//Indoor 0x1165
	{0x87, 0x60, BYTE_BYTE_LEN, 0},
	{0x88, 0x66, BYTE_BYTE_LEN, 0},//Indoor 0x1166
	{0x89, 0x60, BYTE_BYTE_LEN, 0},
	{0x8a, 0x67, BYTE_BYTE_LEN, 0},//Indoor 0x1167
	{0x8b, 0x60, BYTE_BYTE_LEN, 0},
	{0x8c, 0x68, BYTE_BYTE_LEN, 0},//Indoor 0x1168
	{0x8d, 0x80, BYTE_BYTE_LEN, 0},
	{0x8e, 0x69, BYTE_BYTE_LEN, 0},//Indoor 0x1169
	{0x8f, 0x40, BYTE_BYTE_LEN, 0},
	{0x90, 0x6a, BYTE_BYTE_LEN, 0},//Indoor 0x116a	 //Imp Lv2 High Gain
	{0x91, 0x60, BYTE_BYTE_LEN, 0}, //20, BYTE_BYTE_LEN, 0},//0224 jk//0x60, BYTE_BYTE_LEN, 0},
	{0x92, 0x6b, BYTE_BYTE_LEN, 0},//Indoor 0x116b	 //Imp Lv2 Middle Gain
	{0x93, 0x60, BYTE_BYTE_LEN, 0}, //20, BYTE_BYTE_LEN, 0},//0224 jk//0x60, BYTE_BYTE_LEN, 0},
	{0x94, 0x6c, BYTE_BYTE_LEN, 0},//Indoor 0x116c	 //Imp Lv2 Low Gain
	{0x95, 0x60, BYTE_BYTE_LEN, 0}, //20, BYTE_BYTE_LEN, 0},//0224 jk//0x60, BYTE_BYTE_LEN, 0},
	{0x96, 0x6d, BYTE_BYTE_LEN, 0},//Indoor 0x116d
	{0x97, 0x80, BYTE_BYTE_LEN, 0},
	{0x98, 0x6e, BYTE_BYTE_LEN, 0},//Indoor 0x116e
	{0x99, 0x40, BYTE_BYTE_LEN, 0},
	{0x9a, 0x6f, BYTE_BYTE_LEN, 0},//Indoor 0x116f	//Imp Lv3 Hi Gain
	{0x9b, 0x60, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x60, BYTE_BYTE_LEN, 0},
	{0x9c, 0x70, BYTE_BYTE_LEN, 0},//Indoor 0x1170	//Imp Lv3 Middle Gain
	{0x9d, 0x60, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x60, BYTE_BYTE_LEN, 0},
	{0x9e, 0x71, BYTE_BYTE_LEN, 0},//Indoor 0x1171	//Imp Lv3 Low Gain
	{0x9f, 0x40, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x60, BYTE_BYTE_LEN, 0},
	{0xa0, 0x72, BYTE_BYTE_LEN, 0},//Indoor 0x1172
	{0xa1, 0x6e, BYTE_BYTE_LEN, 0},
	{0xa2, 0x73, BYTE_BYTE_LEN, 0},//Indoor 0x1173
	{0xa3, 0x3a, BYTE_BYTE_LEN, 0},
	{0xa4, 0x74, BYTE_BYTE_LEN, 0},//Indoor 0x1174	//Imp Lv4 Hi Gain
	{0xa5, 0x60, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x60, BYTE_BYTE_LEN, 0},
	{0xa6, 0x75, BYTE_BYTE_LEN, 0},//Indoor 0x1175	//Imp Lv4 Middle Gain
	{0xa7, 0x60, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x60, BYTE_BYTE_LEN, 0},
	{0xa8, 0x76, BYTE_BYTE_LEN, 0},//Indoor 0x1176	//Imp Lv4 Low Gain
	{0xa9, 0x40, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x60, BYTE_BYTE_LEN, 0},//18
	{0xaa, 0x77, BYTE_BYTE_LEN, 0},//Indoor 0x1177	//Imp Lv5 Hi Th
	{0xab, 0x6e, BYTE_BYTE_LEN, 0},
	{0xac, 0x78, BYTE_BYTE_LEN, 0},//Indoor 0x1178	//Imp Lv5 Middle Th
	{0xad, 0x66, BYTE_BYTE_LEN, 0},
	{0xae, 0x79, BYTE_BYTE_LEN, 0},//Indoor 0x1179	//Imp Lv5 Hi Gain
	{0xaf, 0x60, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x50, BYTE_BYTE_LEN, 0},
	{0xb0, 0x7a, BYTE_BYTE_LEN, 0},//Indoor 0x117a	//Imp Lv5 Middle Gain
	{0xb1, 0x40, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x50, BYTE_BYTE_LEN, 0},
	{0xb2, 0x7b, BYTE_BYTE_LEN, 0},//Indoor 0x117b	//Imp Lv5 Low Gain
	{0xb3, 0x20, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x50, BYTE_BYTE_LEN, 0},
	{0xb4, 0x7c, BYTE_BYTE_LEN, 0},//Indoor 0x117c	//Imp Lv6 Hi Th
	{0xb5, 0x5c, BYTE_BYTE_LEN, 0},
	{0xb6, 0x7d, BYTE_BYTE_LEN, 0},//Indoor 0x117d	//Imp Lv6 Middle Th
	{0xb7, 0x30, BYTE_BYTE_LEN, 0},
	{0xb8, 0x7e, BYTE_BYTE_LEN, 0},//Indoor 0x117e	//Imp Lv6 Hi Gain
	{0xb9, 0x10, BYTE_BYTE_LEN, 0},//0224 jk//0x44, BYTE_BYTE_LEN, 0},
	{0xba, 0x7f, BYTE_BYTE_LEN, 0},//Indoor 0x117f	//Imp Lv6 Middle Gain
	{0xbb, 0x10, BYTE_BYTE_LEN, 0},//0224 jk//0x44, BYTE_BYTE_LEN, 0},
	{0xbc, 0x80, BYTE_BYTE_LEN, 0},//Indoor 0x1180	//Imp Lv6 Low Gain
	{0xbd, 0x10, BYTE_BYTE_LEN, 0},//0224 jk//0x44, BYTE_BYTE_LEN, 0}, 
	{0xbe, 0x81, BYTE_BYTE_LEN, 0},//Indoor 0x1181
	{0xbf, 0x62, BYTE_BYTE_LEN, 0},
	{0xc0, 0x82, BYTE_BYTE_LEN, 0},//Indoor 0x1182
	{0xc1, 0x26, BYTE_BYTE_LEN, 0},
	{0xc2, 0x83, BYTE_BYTE_LEN, 0},//Indoor 0x1183	//Imp Lv7 Hi Gain
	{0xc3, 0x40, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x3e, BYTE_BYTE_LEN, 0},
	{0xc4, 0x84, BYTE_BYTE_LEN, 0},//Indoor 0x1184	//Imp Lv7 Middle Gain
	{0xc5, 0x40, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x3e, BYTE_BYTE_LEN, 0},
	{0xc6, 0x85, BYTE_BYTE_LEN, 0},//Indoor 0x1185	//Imp Lv7 Low Gain
	{0xc7, 0x20, BYTE_BYTE_LEN, 0}, //10, BYTE_BYTE_LEN, 0},//0224 jk//0x3e, BYTE_BYTE_LEN, 0},
	{0xc8, 0x86, BYTE_BYTE_LEN, 0},//Indoor 0x1186
	{0xc9, 0x62, BYTE_BYTE_LEN, 0},
	{0xca, 0x87, BYTE_BYTE_LEN, 0},//Indoor 0x1187
	{0xcb, 0x26, BYTE_BYTE_LEN, 0},
	{0xcc, 0x88, BYTE_BYTE_LEN, 0},//Indoor 0x1188
	{0xcd, 0x30, BYTE_BYTE_LEN, 0},
	{0xce, 0x89, BYTE_BYTE_LEN, 0},//Indoor 0x1189
	{0xcf, 0x30, BYTE_BYTE_LEN, 0},
	{0xd0, 0x8a, BYTE_BYTE_LEN, 0},//Indoor 0x118a
	{0xd1, 0x30, BYTE_BYTE_LEN, 0},
	{0xd2, 0x90, BYTE_BYTE_LEN, 0},//Indoor 0x1190
	{0xd3, 0x00, BYTE_BYTE_LEN, 0},
	{0xd4, 0x91, BYTE_BYTE_LEN, 0},//Indoor 0x1191
	{0xd5, 0x4e, BYTE_BYTE_LEN, 0},
	{0xd6, 0x92, BYTE_BYTE_LEN, 0},//Indoor 0x1192
	{0xd7, 0x00, BYTE_BYTE_LEN, 0},
	{0xd8, 0x93, BYTE_BYTE_LEN, 0},//Indoor 0x1193
	{0xd9, 0x16, BYTE_BYTE_LEN, 0},
	{0xda, 0x94, BYTE_BYTE_LEN, 0},//Indoor 0x1194
	{0xdb, 0x01, BYTE_BYTE_LEN, 0},
	{0xdc, 0x95, BYTE_BYTE_LEN, 0},//Indoor 0x1195
	{0xdd, 0x80, BYTE_BYTE_LEN, 0},
	{0xde, 0x96, BYTE_BYTE_LEN, 0},//Indoor 0x1196
	{0xdf, 0x55, BYTE_BYTE_LEN, 0},
	{0xe0, 0x97, BYTE_BYTE_LEN, 0},//Indoor 0x1197
	{0xe1, 0x8d, BYTE_BYTE_LEN, 0},
	{0xe2, 0xb0, BYTE_BYTE_LEN, 0},//Indoor 0x11b0
	{0xe3, 0x60, BYTE_BYTE_LEN, 0},
	{0xe4, 0xb1, BYTE_BYTE_LEN, 0},//Indoor 0x11b1
	{0xe5, 0x99, BYTE_BYTE_LEN, 0},
	{0xe6, 0xb2, BYTE_BYTE_LEN, 0},//Indoor 0x11b2
	{0xe7, 0x19, BYTE_BYTE_LEN, 0},
	{0xe8, 0xb3, BYTE_BYTE_LEN, 0},//Indoor 0x11b3
	{0xe9, 0x00, BYTE_BYTE_LEN, 0},
	{0xea, 0xb4, BYTE_BYTE_LEN, 0},//Indoor 0x11b4
	{0xeb, 0x00, BYTE_BYTE_LEN, 0},
	{0xec, 0x03, BYTE_BYTE_LEN, 0}, //12 page
	{0xed, 0x12, BYTE_BYTE_LEN, 0},
	{0xee, 0x10, BYTE_BYTE_LEN, 0}, //Indoor 0x1210
	{0xef, 0x03, BYTE_BYTE_LEN, 0},
	{0xf0, 0x11, BYTE_BYTE_LEN, 0}, //Indoor 0x1211
	{0xf1, 0x29, BYTE_BYTE_LEN, 0},
	{0xf2, 0x12, BYTE_BYTE_LEN, 0}, //Indoor 0x1212
	{0xf3, 0x08, BYTE_BYTE_LEN, 0},
	{0xf4, 0x40, BYTE_BYTE_LEN, 0},//Indoor 0x1240
	{0xf5, 0x33, BYTE_BYTE_LEN, 0}, //07
	{0xf6, 0x41, BYTE_BYTE_LEN, 0},//Indoor 0x1241
	{0xf7, 0x0a, BYTE_BYTE_LEN, 0}, //32
	{0xf8, 0x42, BYTE_BYTE_LEN, 0},//Indoor 0x1242
	{0xf9, 0x6a, BYTE_BYTE_LEN, 0}, //8c
	{0xfa, 0x43, BYTE_BYTE_LEN, 0},//Indoor 0x1243
	{0xfb, 0x80, BYTE_BYTE_LEN, 0},
	{0xfc, 0x44, BYTE_BYTE_LEN, 0}, //Indoor 0x1244
	{0xfd, 0x02, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xde, BYTE_BYTE_LEN, 0},
	{0x10, 0x45, BYTE_BYTE_LEN, 0}, //Indoor 0x1245
	{0x11, 0x0a, BYTE_BYTE_LEN, 0},
	{0x12, 0x46, BYTE_BYTE_LEN, 0}, //Indoor 0x1246
	{0x13, 0x80, BYTE_BYTE_LEN, 0},
	{0x14, 0x60, BYTE_BYTE_LEN, 0}, //Indoor 0x1260
	{0x15, 0x02, BYTE_BYTE_LEN, 0},
	{0x16, 0x61, BYTE_BYTE_LEN, 0}, //Indoor 0x1261
	{0x17, 0x04, BYTE_BYTE_LEN, 0},
	{0x18, 0x62, BYTE_BYTE_LEN, 0}, //Indoor 0x1262
	{0x19, 0x4b, BYTE_BYTE_LEN, 0},
	{0x1a, 0x63, BYTE_BYTE_LEN, 0}, //Indoor 0x1263
	{0x1b, 0x41, BYTE_BYTE_LEN, 0},
	{0x1c, 0x64, BYTE_BYTE_LEN, 0}, //Indoor 0x1264
	{0x1d, 0x14, BYTE_BYTE_LEN, 0},
	{0x1e, 0x65, BYTE_BYTE_LEN, 0}, //Indoor 0x1265
	{0x1f, 0x00, BYTE_BYTE_LEN, 0},
	{0x20, 0x68, BYTE_BYTE_LEN, 0}, //Indoor 0x1268
	{0x21, 0x0a, BYTE_BYTE_LEN, 0},
	{0x22, 0x69, BYTE_BYTE_LEN, 0}, //Indoor 0x1269
	{0x23, 0x04, BYTE_BYTE_LEN, 0},
	{0x24, 0x6a, BYTE_BYTE_LEN, 0}, //Indoor 0x126a
	{0x25, 0x0a, BYTE_BYTE_LEN, 0},
	{0x26, 0x6b, BYTE_BYTE_LEN, 0}, //Indoor 0x126b
	{0x27, 0x0a, BYTE_BYTE_LEN, 0},
	{0x28, 0x6c, BYTE_BYTE_LEN, 0}, //Indoor 0x126c
	{0x29, 0x24, BYTE_BYTE_LEN, 0},
	{0x2a, 0x6d, BYTE_BYTE_LEN, 0}, //Indoor 0x126d
	{0x2b, 0x01, BYTE_BYTE_LEN, 0},
	{0x2c, 0x70, BYTE_BYTE_LEN, 0}, //Indoor 0x1270
	{0x2d, 0x25, BYTE_BYTE_LEN, 0},
	{0x2e, 0x71, BYTE_BYTE_LEN, 0},//Indoor 0x1271
	{0x2f, 0x7f, BYTE_BYTE_LEN, 0},
	{0x30, 0x80, BYTE_BYTE_LEN, 0},//Indoor 0x1280
	{0x31, 0x82, BYTE_BYTE_LEN, 0},//88 			
	{0x32, 0x81, BYTE_BYTE_LEN, 0},//Indoor 0x1281
	{0x33, 0x86, BYTE_BYTE_LEN, 0}, //05			
	{0x34, 0x82, BYTE_BYTE_LEN, 0},//Indoor 0x1282
	{0x35, 0x06, BYTE_BYTE_LEN, 0},//13 			
	{0x36, 0x83, BYTE_BYTE_LEN, 0},//Indoor 0x1283
	{0x37, 0x04, BYTE_BYTE_LEN, 0},//40 			
	{0x38, 0x84, BYTE_BYTE_LEN, 0},//Indoor 0x1284
	{0x39, 0x10, BYTE_BYTE_LEN, 0},
	{0x3a, 0x85, BYTE_BYTE_LEN, 0},//Indoor 0x1285
	{0x3b, 0x86, BYTE_BYTE_LEN, 0},
	{0x3c, 0x86, BYTE_BYTE_LEN, 0},//Indoor 0x1286
	{0x3d, 0x90, BYTE_BYTE_LEN, 0},//15 			
	{0x3e, 0x87, BYTE_BYTE_LEN, 0},//Indoor 0x1287
	{0x3f, 0x10, BYTE_BYTE_LEN, 0},
	{0x40, 0x88, BYTE_BYTE_LEN, 0},//Indoor 0x1288
	{0x41, 0x3a, BYTE_BYTE_LEN, 0},
	{0x42, 0x89, BYTE_BYTE_LEN, 0},//Indoor 0x1289
	{0x43, 0x80, BYTE_BYTE_LEN, 0},//c0
	{0x44, 0x8a, BYTE_BYTE_LEN, 0},//Indoor 0x128a
	{0x45, 0xa0, BYTE_BYTE_LEN, 0},//18
	{0x46, 0x8b, BYTE_BYTE_LEN, 0}, //Indoor 0x128b
	{0x47, 0x03, BYTE_BYTE_LEN, 0},//05
	{0x48, 0x8c, BYTE_BYTE_LEN, 0}, //Indoor 0x128c
	{0x49, 0x02, BYTE_BYTE_LEN, 0},
	{0x4a, 0x8d, BYTE_BYTE_LEN, 0}, //Indoor 0x128d
	{0x4b, 0x02, BYTE_BYTE_LEN, 0},
	{0x4c, 0xe6, BYTE_BYTE_LEN, 0}, //Indoor 0x12e6
	{0x4d, 0xff, BYTE_BYTE_LEN, 0},
	{0x4e, 0xe7, BYTE_BYTE_LEN, 0}, //Indoor 0x12e7
	{0x4f, 0x18, BYTE_BYTE_LEN, 0},
	{0x50, 0xe8, BYTE_BYTE_LEN, 0}, //Indoor 0x12e8
	{0x51, 0x0a, BYTE_BYTE_LEN, 0},
	{0x52, 0xe9, BYTE_BYTE_LEN, 0}, //Indoor 0x12e9
	{0x53, 0x04, BYTE_BYTE_LEN, 0},
	{0x54, 0x03, BYTE_BYTE_LEN, 0},//Indoor Page13
	{0x55, 0x13, BYTE_BYTE_LEN, 0},
	{0x56, 0x10, BYTE_BYTE_LEN, 0},//Indoor 0x1310
	{0x57, 0x33, BYTE_BYTE_LEN, 0},
	{0x58, 0x20, BYTE_BYTE_LEN, 0},//Indoor 0x1320
	{0x59, 0x20, BYTE_BYTE_LEN, 0},
	{0x5a, 0x21, BYTE_BYTE_LEN, 0},//Indoor 0x1321
	{0x5b, 0x30, BYTE_BYTE_LEN, 0},
	{0x5c, 0x22, BYTE_BYTE_LEN, 0},//Indoor 0x1322
	{0x5d, 0x36, BYTE_BYTE_LEN, 0},
	{0x5e, 0x23, BYTE_BYTE_LEN, 0},//Indoor 0x1323
	{0x5f, 0x6a, BYTE_BYTE_LEN, 0},
	{0x60, 0x24, BYTE_BYTE_LEN, 0},//Indoor 0x1324
	{0x61, 0xa0, BYTE_BYTE_LEN, 0},
	{0x62, 0x25, BYTE_BYTE_LEN, 0},//Indoor 0x1325
	{0x63, 0xc0, BYTE_BYTE_LEN, 0},
	{0x64, 0x26, BYTE_BYTE_LEN, 0},//Indoor 0x1326
	{0x65, 0xe0, BYTE_BYTE_LEN, 0},
	{0x66, 0x27, BYTE_BYTE_LEN, 0},//Indoor 0x1327
	{0x67, 0x02, BYTE_BYTE_LEN, 0},
	{0x68, 0x28, BYTE_BYTE_LEN, 0},//Indoor 0x1328
	{0x69, 0x00, BYTE_BYTE_LEN, 0}, //test//0x03, BYTE_BYTE_LEN, 0},
	{0x6a, 0x29, BYTE_BYTE_LEN, 0},//Indoor 0x1329
	{0x6b, 0x00, BYTE_BYTE_LEN, 0}, //test//0x03, BYTE_BYTE_LEN, 0},
	{0x6c, 0x2a, BYTE_BYTE_LEN, 0},//Indoor 0x132a
	{0x6d, 0x00, BYTE_BYTE_LEN, 0}, //test//0x03, BYTE_BYTE_LEN, 0},
	{0x6e, 0x2b, BYTE_BYTE_LEN, 0},//Indoor 0x132b
	{0x6f, 0x00, BYTE_BYTE_LEN, 0}, //test//0x03, BYTE_BYTE_LEN, 0},
	{0x70, 0x2c, BYTE_BYTE_LEN, 0},//Indoor 0x132c
	{0x71, 0x03, BYTE_BYTE_LEN, 0},
	{0x72, 0x2d, BYTE_BYTE_LEN, 0},//Indoor 0x132d
	{0x73, 0x03, BYTE_BYTE_LEN, 0},
	{0x74, 0x2e, BYTE_BYTE_LEN, 0},//Indoor 0x132e
	{0x75, 0x03, BYTE_BYTE_LEN, 0},
	{0x76, 0x2f, BYTE_BYTE_LEN, 0},//Indoor 0x132f
	{0x77, 0x03, BYTE_BYTE_LEN, 0},
	{0x78, 0x30, BYTE_BYTE_LEN, 0},//Indoor 0x1330
	{0x79, 0x03, BYTE_BYTE_LEN, 0},
	{0x7a, 0x31, BYTE_BYTE_LEN, 0},//Indoor 0x1331
	{0x7b, 0x03, BYTE_BYTE_LEN, 0},
	{0x7c, 0x32, BYTE_BYTE_LEN, 0},//Indoor 0x1332
	{0x7d, 0x03, BYTE_BYTE_LEN, 0},
	{0x7e, 0x33, BYTE_BYTE_LEN, 0},//Indoor 0x1333
	{0x7f, 0x40, BYTE_BYTE_LEN, 0},
	{0x80, 0x34, BYTE_BYTE_LEN, 0},//Indoor 0x1334
	{0x81, 0x10, BYTE_BYTE_LEN, 0}, //test//0x80, BYTE_BYTE_LEN, 0},
	{0x82, 0x35, BYTE_BYTE_LEN, 0},//Indoor 0x1335
	{0x83, 0x00, BYTE_BYTE_LEN, 0},
	{0x84, 0x36, BYTE_BYTE_LEN, 0},//Indoor 0x1336
	{0x85, 0xf0, BYTE_BYTE_LEN, 0},
	{0x86, 0xa0, BYTE_BYTE_LEN, 0},//Indoor 0x13a0
	{0x87, 0x0f, BYTE_BYTE_LEN, 0},
	{0x88, 0xa8, BYTE_BYTE_LEN, 0},//Indoor 0x13a8
	{0x89, 0x10, BYTE_BYTE_LEN, 0},
	{0x8a, 0xa9, BYTE_BYTE_LEN, 0},//Indoor 0x13a9
	{0x8b, 0x16, BYTE_BYTE_LEN, 0},
	{0x8c, 0xaa, BYTE_BYTE_LEN, 0},//Indoor 0x13aa
	{0x8d, 0x0a, BYTE_BYTE_LEN, 0},
	{0x8e, 0xab, BYTE_BYTE_LEN, 0},//Indoor 0x13ab
	{0x8f, 0x02, BYTE_BYTE_LEN, 0},
	{0x90, 0xc0, BYTE_BYTE_LEN, 0},//Indoor 0x13c0
	{0x91, 0x27, BYTE_BYTE_LEN, 0},
	{0x92, 0xc2, BYTE_BYTE_LEN, 0},//Indoor 0x13c2
	{0x93, 0x08, BYTE_BYTE_LEN, 0},
	{0x94, 0xc3, BYTE_BYTE_LEN, 0},//Indoor 0x13c3
	{0x95, 0x08, BYTE_BYTE_LEN, 0},
	{0x96, 0xc4, BYTE_BYTE_LEN, 0},//Indoor 0x13c4
	{0x97, 0x40, BYTE_BYTE_LEN, 0},
	{0x98, 0xc5, BYTE_BYTE_LEN, 0},//Indoor 0x13c5
	{0x99, 0x38, BYTE_BYTE_LEN, 0},
	{0x9a, 0xc6, BYTE_BYTE_LEN, 0},//Indoor 0x13c6
	{0x9b, 0xf0, BYTE_BYTE_LEN, 0},
	{0x9c, 0xc7, BYTE_BYTE_LEN, 0},//Indoor 0x13c7
	{0x9d, 0x10, BYTE_BYTE_LEN, 0},
	{0x9e, 0xc8, BYTE_BYTE_LEN, 0},//Indoor 0x13c8
	{0x9f, 0x44, BYTE_BYTE_LEN, 0},
	{0xa0, 0xc9, BYTE_BYTE_LEN, 0},//Indoor 0x13c9
	{0xa1, 0x87, BYTE_BYTE_LEN, 0},
	{0xa2, 0xca, BYTE_BYTE_LEN, 0},//Indoor 0x13ca
	{0xa3, 0xff, BYTE_BYTE_LEN, 0},
	{0xa4, 0xcb, BYTE_BYTE_LEN, 0},//Indoor 0x13cb
	{0xa5, 0x20, BYTE_BYTE_LEN, 0},
	{0xa6, 0xcc, BYTE_BYTE_LEN, 0},//Indoor 0x13cc
	{0xa7, 0x61, BYTE_BYTE_LEN, 0},
	{0xa8, 0xcd, BYTE_BYTE_LEN, 0},//Indoor 0x13cd
	{0xa9, 0x87, BYTE_BYTE_LEN, 0},
	{0xaa, 0xce, BYTE_BYTE_LEN, 0},//Indoor 0x13ce
	{0xab, 0x8a, BYTE_BYTE_LEN, 0},//07
	{0xac, 0xcf, BYTE_BYTE_LEN, 0},//Indoor 0x13cf
	{0xad, 0xa5, BYTE_BYTE_LEN, 0},//07
	{0xae, 0x03, BYTE_BYTE_LEN, 0},//Indoor Page14
	{0xaf, 0x14, BYTE_BYTE_LEN, 0},
	{0xb0, 0x10, BYTE_BYTE_LEN, 0},//Indoor 0x1410
	{0xb1, 0x27, BYTE_BYTE_LEN, 0},
	{0xb2, 0x11, BYTE_BYTE_LEN, 0},//Indoor 0x1411
	{0xb3, 0x02, BYTE_BYTE_LEN, 0},
	{0xb4, 0x12, BYTE_BYTE_LEN, 0},//Indoor 0x1412
	{0xb5, 0x40, BYTE_BYTE_LEN, 0},
	{0xb6, 0x13, BYTE_BYTE_LEN, 0},//Indoor 0x1413
	{0xb7, 0x98, BYTE_BYTE_LEN, 0},
	{0xb8, 0x14, BYTE_BYTE_LEN, 0},//Indoor 0x1414
	{0xb9, 0x3a, BYTE_BYTE_LEN, 0},
	{0xba, 0x15, BYTE_BYTE_LEN, 0},//Indoor 0x1415
	{0xbb, 0x24, BYTE_BYTE_LEN, 0},
	{0xbc, 0x16, BYTE_BYTE_LEN, 0},//Indoor 0x1416
	{0xbd, 0x1a, BYTE_BYTE_LEN, 0},
	{0xbe, 0x17, BYTE_BYTE_LEN, 0},//Indoor 0x1417
	{0xbf, 0x1a, BYTE_BYTE_LEN, 0},
	{0xc0, 0x18, BYTE_BYTE_LEN, 0},//Indoor 0x1418	Negative High Gain
	{0xc1, 0x60, BYTE_BYTE_LEN, 0},//3a
	{0xc2, 0x19, BYTE_BYTE_LEN, 0},//Indoor 0x1419	Negative Middle Gain
	{0xc3, 0x68, BYTE_BYTE_LEN, 0},//3a
	{0xc4, 0x1a, BYTE_BYTE_LEN, 0},//Indoor 0x141a	Negative Low Gain
	{0xc5, 0x68, BYTE_BYTE_LEN, 0}, //
	{0xc6, 0x20, BYTE_BYTE_LEN, 0},//Indoor 0x1420
	{0xc7, 0x82, BYTE_BYTE_LEN, 0},  // s_diff L_clip
	{0xc8, 0x21, BYTE_BYTE_LEN, 0},//Indoor 0x1421
	{0xc9, 0x03, BYTE_BYTE_LEN, 0},
	{0xca, 0x22, BYTE_BYTE_LEN, 0},//Indoor 0x1422
	{0xcb, 0x05, BYTE_BYTE_LEN, 0},
	{0xcc, 0x23, BYTE_BYTE_LEN, 0},//Indoor 0x1423
	{0xcd, 0x07, BYTE_BYTE_LEN, 0},
	{0xce, 0x24, BYTE_BYTE_LEN, 0},//Indoor 0x1424
	{0xcf, 0x0a, BYTE_BYTE_LEN, 0},
	{0xd0, 0x25, BYTE_BYTE_LEN, 0},//Indoor 0x1425
	{0xd1, 0x46, BYTE_BYTE_LEN, 0}, //19
	{0xd2, 0x26, BYTE_BYTE_LEN, 0},//Indoor 0x1426
	{0xd3, 0x32, BYTE_BYTE_LEN, 0},
	{0xd4, 0x27, BYTE_BYTE_LEN, 0},//Indoor 0x1427
	{0xd5, 0x1e, BYTE_BYTE_LEN, 0},
	{0xd6, 0x28, BYTE_BYTE_LEN, 0},//Indoor 0x1428
	{0xd7, 0x10, BYTE_BYTE_LEN, 0},
	{0xd8, 0x29, BYTE_BYTE_LEN, 0},//Indoor 0x1429
	{0xd9, 0x00, BYTE_BYTE_LEN, 0},
	{0xda, 0x2a, BYTE_BYTE_LEN, 0},//Indoor 0x142a
	{0xdb, 0x18, BYTE_BYTE_LEN, 0},//40
	{0xdc, 0x2b, BYTE_BYTE_LEN, 0},//Indoor 0x142b
	{0xdd, 0x18, BYTE_BYTE_LEN, 0},
	{0xde, 0x2c, BYTE_BYTE_LEN, 0},//Indoor 0x142c
	{0xdf, 0x18, BYTE_BYTE_LEN, 0},
	{0xe0, 0x2d, BYTE_BYTE_LEN, 0},//Indoor 0x142d
	{0xe1, 0x30, BYTE_BYTE_LEN, 0},
	{0xe2, 0x2e, BYTE_BYTE_LEN, 0},//Indoor 0x142e
	{0xe3, 0x30, BYTE_BYTE_LEN, 0},
	{0xe4, 0x2f, BYTE_BYTE_LEN, 0},//Indoor 0x142f
	{0xe5, 0x30, BYTE_BYTE_LEN, 0},
	{0xe6, 0x30, BYTE_BYTE_LEN, 0},//Indoor 0x1430
	{0xe7, 0x87, BYTE_BYTE_LEN, 0}, //test//0x82, BYTE_BYTE_LEN, 0},   //Ldiff_L_cip
	{0xe8, 0x31, BYTE_BYTE_LEN, 0},//Indoor 0x1431
	{0xe9, 0x02, BYTE_BYTE_LEN, 0},
	{0xea, 0x32, BYTE_BYTE_LEN, 0},//Indoor 0x1432
	{0xeb, 0x04, BYTE_BYTE_LEN, 0},
	{0xec, 0x33, BYTE_BYTE_LEN, 0},//Indoor 0x1433
	{0xed, 0x04, BYTE_BYTE_LEN, 0},
	{0xee, 0x34, BYTE_BYTE_LEN, 0},//Indoor 0x1434
	{0xef, 0x0a, BYTE_BYTE_LEN, 0},
	{0xf0, 0x35, BYTE_BYTE_LEN, 0},//Indoor 0x1435
	{0xf1, 0x46, BYTE_BYTE_LEN, 0},//12
	{0xf2, 0x36, BYTE_BYTE_LEN, 0},//Indoor 0x1436
	{0xf3, 0x15, BYTE_BYTE_LEN, 0}, //test//0x32, BYTE_BYTE_LEN, 0},
	{0xf4, 0x37, BYTE_BYTE_LEN, 0},//Indoor 0x1437
	{0xf5, 0x32, BYTE_BYTE_LEN, 0},
	{0xf6, 0x38, BYTE_BYTE_LEN, 0},//Indoor 0x1438
	{0xf7, 0x18, BYTE_BYTE_LEN, 0}, //test//0x22, BYTE_BYTE_LEN, 0},
	{0xf8, 0x39, BYTE_BYTE_LEN, 0},//Indoor 0x1439
	{0xf9, 0x00, BYTE_BYTE_LEN, 0},
	{0xfa, 0x3a, BYTE_BYTE_LEN, 0},//Indoor 0x143a
	{0xfb, 0x48, BYTE_BYTE_LEN, 0},
	{0xfc, 0x3b, BYTE_BYTE_LEN, 0},//Indoor 0x143b
	{0xfd, 0x19, BYTE_BYTE_LEN, 0}, //test//0x30, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xdf, BYTE_BYTE_LEN, 0},
	{0x10, 0x3c, BYTE_BYTE_LEN, 0},//Indoor 0x143c
	{0x11, 0x19, BYTE_BYTE_LEN, 0}, //test//0x30, BYTE_BYTE_LEN, 0},
	{0x12, 0x3d, BYTE_BYTE_LEN, 0},//Indoor 0x143d
	{0x13, 0x20, BYTE_BYTE_LEN, 0},
	{0x14, 0x3e, BYTE_BYTE_LEN, 0},//Indoor 0x143e
	{0x15, 0x15, BYTE_BYTE_LEN, 0}, //test//0x22, BYTE_BYTE_LEN, 0},//12
	{0x16, 0x3f, BYTE_BYTE_LEN, 0},//Indoor 0x143f
	{0x17, 0x0d, BYTE_BYTE_LEN, 0}, //test//0x10, BYTE_BYTE_LEN, 0},
	{0x18, 0x40, BYTE_BYTE_LEN, 0},//Indoor 0x1440
	{0x19, 0x84, BYTE_BYTE_LEN, 0},
	{0x1a, 0x41, BYTE_BYTE_LEN, 0},//Indoor 0x1441
	{0x1b, 0x10, BYTE_BYTE_LEN, 0},//20
	{0x1c, 0x42, BYTE_BYTE_LEN, 0},//Indoor 0x1442
	{0x1d, 0xb0, BYTE_BYTE_LEN, 0},//20
	{0x1e, 0x43, BYTE_BYTE_LEN, 0},//Indoor 0x1443
	{0x1f, 0x40, BYTE_BYTE_LEN, 0},//20
	{0x20, 0x44, BYTE_BYTE_LEN, 0},//Indoor 0x1444
	{0x21, 0x14, BYTE_BYTE_LEN, 0},
	{0x22, 0x45, BYTE_BYTE_LEN, 0},//Indoor 0x1445
	{0x23, 0x10, BYTE_BYTE_LEN, 0},
	{0x24, 0x46, BYTE_BYTE_LEN, 0},//Indoor 0x1446
	{0x25, 0x14, BYTE_BYTE_LEN, 0},
	{0x26, 0x47, BYTE_BYTE_LEN, 0},//Indoor 0x1447
	{0x27, 0x04, BYTE_BYTE_LEN, 0},
	{0x28, 0x48, BYTE_BYTE_LEN, 0},//Indoor 0x1448
	{0x29, 0x04, BYTE_BYTE_LEN, 0},
	{0x2a, 0x49, BYTE_BYTE_LEN, 0},//Indoor 0x1449
	{0x2b, 0x04, BYTE_BYTE_LEN, 0},
	{0x2c, 0x50, BYTE_BYTE_LEN, 0},//Indoor 0x1450
	{0x2d, 0x84, BYTE_BYTE_LEN, 0},//19
	{0x2e, 0x51, BYTE_BYTE_LEN, 0},//Indoor 0x1451
	{0x2f, 0x30, BYTE_BYTE_LEN, 0},//60
	{0x30, 0x52, BYTE_BYTE_LEN, 0},//Indoor 0x1452
	{0x31, 0xb0, BYTE_BYTE_LEN, 0},
	{0x32, 0x53, BYTE_BYTE_LEN, 0},//Indoor 0x1453
	{0x33, 0x37, BYTE_BYTE_LEN, 0},//58
	{0x34, 0x54, BYTE_BYTE_LEN, 0},//Indoor 0x1454
	{0x35, 0x44, BYTE_BYTE_LEN, 0},
	{0x36, 0x55, BYTE_BYTE_LEN, 0},//Indoor 0x1455
	{0x37, 0x44, BYTE_BYTE_LEN, 0},
	{0x38, 0x56, BYTE_BYTE_LEN, 0},//Indoor 0x1456
	{0x39, 0x44, BYTE_BYTE_LEN, 0},
	{0x3a, 0x57, BYTE_BYTE_LEN, 0},//Indoor 0x1457
	{0x3b, 0x10, BYTE_BYTE_LEN, 0},//03
	{0x3c, 0x58, BYTE_BYTE_LEN, 0},//Indoor 0x1458
	{0x3d, 0x14, BYTE_BYTE_LEN, 0},
	{0x3e, 0x59, BYTE_BYTE_LEN, 0},//Indoor 0x1459
	{0x3f, 0x14, BYTE_BYTE_LEN, 0},
	{0x40, 0x60, BYTE_BYTE_LEN, 0},//Indoor 0x1460
	{0x41, 0x02, BYTE_BYTE_LEN, 0},
	{0x42, 0x61, BYTE_BYTE_LEN, 0},//Indoor 0x1461
	{0x43, 0xa0, BYTE_BYTE_LEN, 0},
	{0x44, 0x62, BYTE_BYTE_LEN, 0},//Indoor 0x1462
	{0x45, 0x98, BYTE_BYTE_LEN, 0},
	{0x46, 0x63, BYTE_BYTE_LEN, 0},//Indoor 0x1463
	{0x47, 0xe4, BYTE_BYTE_LEN, 0},
	{0x48, 0x64, BYTE_BYTE_LEN, 0},//Indoor 0x1464
	{0x49, 0xa4, BYTE_BYTE_LEN, 0},
	{0x4a, 0x65, BYTE_BYTE_LEN, 0},//Indoor 0x1465
	{0x4b, 0x7d, BYTE_BYTE_LEN, 0},
	{0x4c, 0x66, BYTE_BYTE_LEN, 0},//Indoor 0x1466
	{0x4d, 0x4b, BYTE_BYTE_LEN, 0},
	{0x4e, 0x70, BYTE_BYTE_LEN, 0},//Indoor 0x1470
	{0x4f, 0x10, BYTE_BYTE_LEN, 0},
	{0x50, 0x71, BYTE_BYTE_LEN, 0},//Indoor 0x1471
	{0x51, 0x10, BYTE_BYTE_LEN, 0},
	{0x52, 0x72, BYTE_BYTE_LEN, 0},//Indoor 0x1472
	{0x53, 0x10, BYTE_BYTE_LEN, 0},
	{0x54, 0x73, BYTE_BYTE_LEN, 0},//Indoor 0x1473
	{0x55, 0x10, BYTE_BYTE_LEN, 0},
	{0x56, 0x74, BYTE_BYTE_LEN, 0},//Indoor 0x1474
	{0x57, 0x10, BYTE_BYTE_LEN, 0},
	{0x58, 0x75, BYTE_BYTE_LEN, 0},//Indoor 0x1475
	{0x59, 0x10, BYTE_BYTE_LEN, 0},
	{0x5a, 0x76, BYTE_BYTE_LEN, 0},//Indoor 0x1476	  //green sharp pos High		  
	{0x5b, 0x10, BYTE_BYTE_LEN, 0}, 												   
	{0x5c, 0x77, BYTE_BYTE_LEN, 0},//Indoor 0x1477	  //green sharp pos Middle		  
	{0x5d, 0x20, BYTE_BYTE_LEN, 0}, 												   
	{0x5e, 0x78, BYTE_BYTE_LEN, 0},//Indoor 0x1478	  //green sharp pos Low 		  
	{0x5f, 0x18, BYTE_BYTE_LEN, 0}, 												   
	{0x60, 0x79, BYTE_BYTE_LEN, 0},//Indoor 0x1479	   //green sharp nega High		  
	{0x61, 0x60, BYTE_BYTE_LEN, 0}, 												   
	{0x62, 0x7a, BYTE_BYTE_LEN, 0},//Indoor 0x147a	   //green sharp nega Middle	  
	{0x63, 0x60, BYTE_BYTE_LEN, 0}, 												   
	{0x64, 0x7b, BYTE_BYTE_LEN, 0},//Indoor 0x147b	   //green sharp nega Low		  
	{0x65, 0x60, BYTE_BYTE_LEN, 0},
	

	
	//////////////////
	// e0 Page (DMA Dark1)
	//////////////////
	
	//Page 0xe0
	{0x03, 0xe0, BYTE_BYTE_LEN, 0},
	{0x10, 0x03, BYTE_BYTE_LEN, 0},
	{0x11, 0x11, BYTE_BYTE_LEN, 0}, //11 page
	{0x12, 0x10, BYTE_BYTE_LEN, 0}, //Dark1 0x1110
	{0x13, 0x1f, BYTE_BYTE_LEN, 0}, 
	{0x14, 0x11, BYTE_BYTE_LEN, 0}, //Dark1 0x1111
	{0x15, 0x2a, BYTE_BYTE_LEN, 0},
	{0x16, 0x12, BYTE_BYTE_LEN, 0}, //Dark1 0x1112
	{0x17, 0x32, BYTE_BYTE_LEN, 0},
	{0x18, 0x13, BYTE_BYTE_LEN, 0}, //Dark1 0x1113
	{0x19, 0x21, BYTE_BYTE_LEN, 0},
	{0x1a, 0x14, BYTE_BYTE_LEN, 0}, //Dark1 0x1114
	{0x1b, 0x3a, BYTE_BYTE_LEN, 0},
	{0x1c, 0x30, BYTE_BYTE_LEN, 0}, //Dark1 0x1130
	{0x1d, 0x20, BYTE_BYTE_LEN, 0}, //20
	{0x1e, 0x31, BYTE_BYTE_LEN, 0}, //Dark1 0x1131
	{0x1f, 0x20, BYTE_BYTE_LEN, 0}, //20
	
	{0x20, 0x32, BYTE_BYTE_LEN, 0}, //Dark1 0x1132 //STEVE Lum. Level. in DLPF
	{0x21, 0x8b, BYTE_BYTE_LEN, 0}, //52                                      
	{0x22, 0x33, BYTE_BYTE_LEN, 0}, //Dark1 0x1133                            
	{0x23, 0x54, BYTE_BYTE_LEN, 0}, //3b                                      
	{0x24, 0x34, BYTE_BYTE_LEN, 0}, //Dark1 0x1134                            
	{0x25, 0x2c, BYTE_BYTE_LEN, 0}, //1d                                      
	{0x26, 0x35, BYTE_BYTE_LEN, 0}, //Dark1 0x1135                            
	{0x27, 0x29, BYTE_BYTE_LEN, 0}, //21                                      
	{0x28, 0x36, BYTE_BYTE_LEN, 0}, //Dark1 0x1136                            
	{0x29, 0x18, BYTE_BYTE_LEN, 0}, //1b                                      
	{0x2a, 0x37, BYTE_BYTE_LEN, 0}, //Dark1 0x1137                            
	{0x2b, 0x1e, BYTE_BYTE_LEN, 0}, //21                                      
	{0x2c, 0x38, BYTE_BYTE_LEN, 0}, //Dark1 0x1138                            
	{0x2d, 0x17, BYTE_BYTE_LEN, 0}, //18                                      
	                                        
	{0x2e, 0x39, BYTE_BYTE_LEN, 0}, //Dark1 0x1139
	{0x2f, 0x8a, BYTE_BYTE_LEN, 0},
	{0x30, 0x3a, BYTE_BYTE_LEN, 0}, //Dark1 0x113a
	{0x31, 0x8a, BYTE_BYTE_LEN, 0},
	{0x32, 0x3b, BYTE_BYTE_LEN, 0}, //Dark1 0x113b
	{0x33, 0x8a, BYTE_BYTE_LEN, 0},
	{0x34, 0x3c, BYTE_BYTE_LEN, 0}, //Dark1 0x113c
	{0x35, 0x8a, BYTE_BYTE_LEN, 0},
	{0x36, 0x3d, BYTE_BYTE_LEN, 0}, //Dark1 0x113d
	{0x37, 0x8a, BYTE_BYTE_LEN, 0},
	{0x38, 0x3e, BYTE_BYTE_LEN, 0}, //Dark1 0x113e
	{0x39, 0x8a, BYTE_BYTE_LEN, 0},
	{0x3a, 0x3f, BYTE_BYTE_LEN, 0}, //Dark1 0x113f
	{0x3b, 0x8a, BYTE_BYTE_LEN, 0},
	{0x3c, 0x40, BYTE_BYTE_LEN, 0}, //Dark1 0x1140
	{0x3d, 0x8a, BYTE_BYTE_LEN, 0},
	{0x3e, 0x41, BYTE_BYTE_LEN, 0}, //Dark1 0x1141
	{0x3f, 0x40, BYTE_BYTE_LEN, 0},
	{0x40, 0x42, BYTE_BYTE_LEN, 0}, //Dark1 0x1142
	{0x41, 0x10, BYTE_BYTE_LEN, 0},
	{0x42, 0x43, BYTE_BYTE_LEN, 0}, //Dark1 0x1143
	{0x43, 0x10, BYTE_BYTE_LEN, 0},
	{0x44, 0x44, BYTE_BYTE_LEN, 0}, //Dark1 0x1144
	{0x45, 0x10, BYTE_BYTE_LEN, 0},
	{0x46, 0x45, BYTE_BYTE_LEN, 0}, //Dark1 0x1145
	{0x47, 0x10, BYTE_BYTE_LEN, 0},
	{0x48, 0x46, BYTE_BYTE_LEN, 0}, //Dark1 0x1146
	{0x49, 0x10, BYTE_BYTE_LEN, 0},
	{0x4a, 0x47, BYTE_BYTE_LEN, 0}, //Dark1 0x1147
	{0x4b, 0x10, BYTE_BYTE_LEN, 0},
	{0x4c, 0x48, BYTE_BYTE_LEN, 0}, //Dark1 0x1148
	{0x4d, 0x10, BYTE_BYTE_LEN, 0},
	{0x4e, 0x49, BYTE_BYTE_LEN, 0}, //Dark1 0x1149
	{0x4f, 0x80, BYTE_BYTE_LEN, 0},
	{0x50, 0x4a, BYTE_BYTE_LEN, 0}, //Dark1 0x114a
	{0x51, 0x80, BYTE_BYTE_LEN, 0},
	{0x52, 0x4b, BYTE_BYTE_LEN, 0}, //Dark1 0x114b
	{0x53, 0x80, BYTE_BYTE_LEN, 0},
	{0x54, 0x4c, BYTE_BYTE_LEN, 0}, //Dark1 0x114c
	{0x55, 0x80, BYTE_BYTE_LEN, 0},
	{0x56, 0x4d, BYTE_BYTE_LEN, 0}, //Dark1 0x114d
	{0x57, 0x80, BYTE_BYTE_LEN, 0},
	{0x58, 0x4e, BYTE_BYTE_LEN, 0}, //Dark1 0x114e
	{0x59, 0x80, BYTE_BYTE_LEN, 0},
	{0x5a, 0x4f, BYTE_BYTE_LEN, 0}, //Dark1 0x114f
	{0x5b, 0x80, BYTE_BYTE_LEN, 0},
	{0x5c, 0x50, BYTE_BYTE_LEN, 0}, //Dark1 0x1150
	{0x5d, 0x80, BYTE_BYTE_LEN, 0},
	{0x5e, 0x51, BYTE_BYTE_LEN, 0}, //Dark1 0x1151
	{0x5f, 0xd8, BYTE_BYTE_LEN, 0},
	{0x60, 0x52, BYTE_BYTE_LEN, 0}, //Dark1 0x1152
	{0x61, 0xd8, BYTE_BYTE_LEN, 0},
	{0x62, 0x53, BYTE_BYTE_LEN, 0}, //Dark1 0x1153
	{0x63, 0xd8, BYTE_BYTE_LEN, 0},
	{0x64, 0x54, BYTE_BYTE_LEN, 0}, //Dark1 0x1154
	{0x65, 0xd0, BYTE_BYTE_LEN, 0},
	{0x66, 0x55, BYTE_BYTE_LEN, 0}, //Dark1 0x1155
	{0x67, 0xd0, BYTE_BYTE_LEN, 0},
	{0x68, 0x56, BYTE_BYTE_LEN, 0}, //Dark1 0x1156
	{0x69, 0xc8, BYTE_BYTE_LEN, 0},
	{0x6a, 0x57, BYTE_BYTE_LEN, 0}, //Dark1 0x1157
	{0x6b, 0xc0, BYTE_BYTE_LEN, 0},
	{0x6c, 0x58, BYTE_BYTE_LEN, 0}, //Dark1 0x1158
	{0x6d, 0xc0, BYTE_BYTE_LEN, 0},
	{0x6e, 0x59, BYTE_BYTE_LEN, 0}, //Dark1 0x1159
	{0x6f, 0xf0, BYTE_BYTE_LEN, 0},
	{0x70, 0x5a, BYTE_BYTE_LEN, 0}, //Dark1 0x115a
	{0x71, 0xf0, BYTE_BYTE_LEN, 0},
	{0x72, 0x5b, BYTE_BYTE_LEN, 0}, //Dark1 0x115b
	{0x73, 0xf0, BYTE_BYTE_LEN, 0},
	{0x74, 0x5c, BYTE_BYTE_LEN, 0}, //Dark1 0x115c
	{0x75, 0xe8, BYTE_BYTE_LEN, 0},
	{0x76, 0x5d, BYTE_BYTE_LEN, 0}, //Dark1 0x115d
	{0x77, 0xe8, BYTE_BYTE_LEN, 0},
	{0x78, 0x5e, BYTE_BYTE_LEN, 0}, //Dark1 0x115e
	{0x79, 0xe0, BYTE_BYTE_LEN, 0},
	{0x7a, 0x5f, BYTE_BYTE_LEN, 0}, //Dark1 0x115f
	{0x7b, 0xe0, BYTE_BYTE_LEN, 0},
	{0x7c, 0x60, BYTE_BYTE_LEN, 0}, //Dark1 0x1160
	{0x7d, 0xe0, BYTE_BYTE_LEN, 0},
	{0x7e, 0x61, BYTE_BYTE_LEN, 0}, //Dark1 0x1161
	{0x7f, 0xf0, BYTE_BYTE_LEN, 0},
	{0x80, 0x62, BYTE_BYTE_LEN, 0}, //Dark1 0x1162
	{0x81, 0xf0, BYTE_BYTE_LEN, 0},
	{0x82, 0x63, BYTE_BYTE_LEN, 0}, //Dark1 0x1163
	{0x83, 0x80, BYTE_BYTE_LEN, 0},
	{0x84, 0x64, BYTE_BYTE_LEN, 0}, //Dark1 0x1164
	{0x85, 0x40, BYTE_BYTE_LEN, 0},
	{0x86, 0x65, BYTE_BYTE_LEN, 0}, //Dark1 0x1165
	{0x87, 0x02, BYTE_BYTE_LEN, 0},//0224 jk//0x08, BYTE_BYTE_LEN, 0},0x08, BYTE_BYTE_LEN, 0},
	{0x88, 0x66, BYTE_BYTE_LEN, 0}, //Dark1 0x1166
	{0x89, 0x02, BYTE_BYTE_LEN, 0},//0224 jk//0x08, BYTE_BYTE_LEN, 0},0x08, BYTE_BYTE_LEN, 0},
	{0x8a, 0x67, BYTE_BYTE_LEN, 0}, //Dark1 0x1167
	{0x8b, 0x02, BYTE_BYTE_LEN, 0},//0224 jk//0x08, BYTE_BYTE_LEN, 0},0x08, BYTE_BYTE_LEN, 0},
	{0x8c, 0x68, BYTE_BYTE_LEN, 0}, //Dark1 0x1168
	{0x8d, 0x80, BYTE_BYTE_LEN, 0},
	{0x8e, 0x69, BYTE_BYTE_LEN, 0}, //Dark1 0x1169
	{0x8f, 0x40, BYTE_BYTE_LEN, 0},
	{0x90, 0x6a, BYTE_BYTE_LEN, 0}, //Dark1 0x116a
	{0x91, 0x02, BYTE_BYTE_LEN, 0},//0224 jk//0x08, BYTE_BYTE_LEN, 0},0x08, BYTE_BYTE_LEN, 0},
	{0x92, 0x6b, BYTE_BYTE_LEN, 0}, //Dark1 0x116b
	{0x93, 0x02, BYTE_BYTE_LEN, 0},//0224 jk//0x08, BYTE_BYTE_LEN, 0},0x08, BYTE_BYTE_LEN, 0},
	{0x94, 0x6c, BYTE_BYTE_LEN, 0}, //Dark1 0x116c
	{0x95, 0x02, BYTE_BYTE_LEN, 0},//0224 jk//0x08, BYTE_BYTE_LEN, 0},
	{0x96, 0x6d, BYTE_BYTE_LEN, 0}, //Dark1 0x116d
	{0x97, 0x80, BYTE_BYTE_LEN, 0},
	{0x98, 0x6e, BYTE_BYTE_LEN, 0}, //Dark1 0x116e
	{0x99, 0x40, BYTE_BYTE_LEN, 0},
	{0x9a, 0x6f, BYTE_BYTE_LEN, 0}, //Dark1 0x116f
	{0x9b, 0x02, BYTE_BYTE_LEN, 0},
	{0x9c, 0x70, BYTE_BYTE_LEN, 0}, //Dark1 0x1170
	{0x9d, 0x02, BYTE_BYTE_LEN, 0},
	{0x9e, 0x71, BYTE_BYTE_LEN, 0}, //Dark1 0x1171
	{0x9f, 0x02, BYTE_BYTE_LEN, 0},
	{0xa0, 0x72, BYTE_BYTE_LEN, 0}, //Dark1 0x1172
	{0xa1, 0x6e, BYTE_BYTE_LEN, 0},
	{0xa2, 0x73, BYTE_BYTE_LEN, 0}, //Dark1 0x1173
	{0xa3, 0x3a, BYTE_BYTE_LEN, 0},
	{0xa4, 0x74, BYTE_BYTE_LEN, 0}, //Dark1 0x1174
	{0xa5, 0x02, BYTE_BYTE_LEN, 0},
	{0xa6, 0x75, BYTE_BYTE_LEN, 0}, //Dark1 0x1175
	{0xa7, 0x02, BYTE_BYTE_LEN, 0},
	{0xa8, 0x76, BYTE_BYTE_LEN, 0}, //Dark1 0x1176
	{0xa9, 0x02, BYTE_BYTE_LEN, 0},
	{0xaa, 0x77, BYTE_BYTE_LEN, 0}, //Dark1 0x1177
	{0xab, 0x6e, BYTE_BYTE_LEN, 0},
	{0xac, 0x78, BYTE_BYTE_LEN, 0}, //Dark1 0x1178
	{0xad, 0x3a, BYTE_BYTE_LEN, 0},
	{0xae, 0x79, BYTE_BYTE_LEN, 0}, //Dark1 0x1179
	{0xaf, 0x02, BYTE_BYTE_LEN, 0},
	{0xb0, 0x7a, BYTE_BYTE_LEN, 0}, //Dark1 0x117a
	{0xb1, 0x02, BYTE_BYTE_LEN, 0},
	{0xb2, 0x7b, BYTE_BYTE_LEN, 0}, //Dark1 0x117b
	{0xb3, 0x02, BYTE_BYTE_LEN, 0},
	{0xb4, 0x7c, BYTE_BYTE_LEN, 0}, //Dark1 0x117c
	{0xb5, 0x5c, BYTE_BYTE_LEN, 0},
	{0xb6, 0x7d, BYTE_BYTE_LEN, 0}, //Dark1 0x117d
	{0xb7, 0x30, BYTE_BYTE_LEN, 0},
	{0xb8, 0x7e, BYTE_BYTE_LEN, 0}, //Dark1 0x117e
	{0xb9, 0x02, BYTE_BYTE_LEN, 0},
	{0xba, 0x7f, BYTE_BYTE_LEN, 0}, //Dark1 0x117f
	{0xbb, 0x02, BYTE_BYTE_LEN, 0},
	{0xbc, 0x80, BYTE_BYTE_LEN, 0}, //Dark1 0x1180
	{0xbd, 0x02, BYTE_BYTE_LEN, 0},
	{0xbe, 0x81, BYTE_BYTE_LEN, 0}, //Dark1 0x1181
	{0xbf, 0x62, BYTE_BYTE_LEN, 0},
	{0xc0, 0x82, BYTE_BYTE_LEN, 0}, //Dark1 0x1182
	{0xc1, 0x26, BYTE_BYTE_LEN, 0},
	{0xc2, 0x83, BYTE_BYTE_LEN, 0}, //Dark1 0x1183
	{0xc3, 0x02, BYTE_BYTE_LEN, 0},
	{0xc4, 0x84, BYTE_BYTE_LEN, 0}, //Dark1 0x1184
	{0xc5, 0x02, BYTE_BYTE_LEN, 0},
	{0xc6, 0x85, BYTE_BYTE_LEN, 0}, //Dark1 0x1185
	{0xc7, 0x02, BYTE_BYTE_LEN, 0},
	{0xc8, 0x86, BYTE_BYTE_LEN, 0}, //Dark1 0x1186
	{0xc9, 0x62, BYTE_BYTE_LEN, 0},
	{0xca, 0x87, BYTE_BYTE_LEN, 0}, //Dark1 0x1187
	{0xcb, 0x26, BYTE_BYTE_LEN, 0},
	{0xcc, 0x88, BYTE_BYTE_LEN, 0}, //Dark1 0x1188
	{0xcd, 0x02, BYTE_BYTE_LEN, 0},
	{0xce, 0x89, BYTE_BYTE_LEN, 0}, //Dark1 0x1189
	{0xcf, 0x02, BYTE_BYTE_LEN, 0},
	{0xd0, 0x8a, BYTE_BYTE_LEN, 0}, //Dark1 0x118a
	{0xd1, 0x02, BYTE_BYTE_LEN, 0},
	{0xd2, 0x90, BYTE_BYTE_LEN, 0}, //Dark1 0x1190
	{0xd3, 0x03, BYTE_BYTE_LEN, 0},
	{0xd4, 0x91, BYTE_BYTE_LEN, 0}, //Dark1 0x1191
	{0xd5, 0xff, BYTE_BYTE_LEN, 0},
	{0xd6, 0x92, BYTE_BYTE_LEN, 0}, //Dark1 0x1192
	{0xd7, 0x0a, BYTE_BYTE_LEN, 0},
	{0xd8, 0x93, BYTE_BYTE_LEN, 0}, //Dark1 0x1193
	{0xd9, 0x80, BYTE_BYTE_LEN, 0},
	{0xda, 0x94, BYTE_BYTE_LEN, 0}, //Dark1 0x1194
	{0xdb, 0x03, BYTE_BYTE_LEN, 0},
	{0xdc, 0x95, BYTE_BYTE_LEN, 0}, //Dark1 0x1195
	{0xdd, 0x64, BYTE_BYTE_LEN, 0},
	{0xde, 0x96, BYTE_BYTE_LEN, 0}, //Dark1 0x1196
	{0xdf, 0x90, BYTE_BYTE_LEN, 0},
	{0xe0, 0x97, BYTE_BYTE_LEN, 0}, //Dark1 0x1197
	{0xe1, 0xa0, BYTE_BYTE_LEN, 0},
	{0xe2, 0xb0, BYTE_BYTE_LEN, 0}, //Dark1 0x11b0
	{0xe3, 0x64, BYTE_BYTE_LEN, 0},
	{0xe4, 0xb1, BYTE_BYTE_LEN, 0}, //Dark1 0x11b1
	{0xe5, 0xd8, BYTE_BYTE_LEN, 0},
	{0xe6, 0xb2, BYTE_BYTE_LEN, 0}, //Dark1 0x11b2
	{0xe7, 0x50, BYTE_BYTE_LEN, 0},
	{0xe8, 0xb3, BYTE_BYTE_LEN, 0}, //Dark1 0x11b3
	{0xe9, 0x10, BYTE_BYTE_LEN, 0},
	{0xea, 0xb4, BYTE_BYTE_LEN, 0}, //Dark1 0x11b4
	{0xeb, 0x03, BYTE_BYTE_LEN, 0},
	           
	{0xec, 0x03, BYTE_BYTE_LEN, 0},
	{0xed, 0x12, BYTE_BYTE_LEN, 0},//12 page
	{0xee, 0x10, BYTE_BYTE_LEN, 0}, //Dark1 0x1210
	{0xef, 0x03, BYTE_BYTE_LEN, 0},
	{0xf0, 0x11, BYTE_BYTE_LEN, 0}, //Dark1 0x1211
	{0xf1, 0x29, BYTE_BYTE_LEN, 0},
	{0xf2, 0x12, BYTE_BYTE_LEN, 0}, //Dark1 0x1212
	{0xf3, 0x08, BYTE_BYTE_LEN, 0},
	{0xf4, 0x40, BYTE_BYTE_LEN, 0}, //Dark1 0x1240
	{0xf5, 0x33, BYTE_BYTE_LEN, 0}, //07
	{0xf6, 0x41, BYTE_BYTE_LEN, 0}, //Dark1 0x1241
	{0xf7, 0x0a, BYTE_BYTE_LEN, 0}, //32
	{0xf8, 0x42, BYTE_BYTE_LEN, 0}, //Dark1 0x1242
	{0xf9, 0x6a, BYTE_BYTE_LEN, 0}, //8c
	{0xfa, 0x43, BYTE_BYTE_LEN, 0}, //Dark1 0x1243
	{0xfb, 0x80, BYTE_BYTE_LEN, 0},
	{0xfc, 0x44, BYTE_BYTE_LEN, 0}, //Dark1 0x1244
	{0xfd, 0x02, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xe1, BYTE_BYTE_LEN, 0},
	{0x10, 0x45, BYTE_BYTE_LEN, 0}, //Dark1 0x1245
	{0x11, 0x0a, BYTE_BYTE_LEN, 0},
	{0x12, 0x46, BYTE_BYTE_LEN, 0}, //Dark1 0x1246
	{0x13, 0x80, BYTE_BYTE_LEN, 0},
	{0x14, 0x60, BYTE_BYTE_LEN, 0}, //Dark1 0x1260
	{0x15, 0x02, BYTE_BYTE_LEN, 0},
	{0x16, 0x61, BYTE_BYTE_LEN, 0}, //Dark1 0x1261
	{0x17, 0x04, BYTE_BYTE_LEN, 0},
	{0x18, 0x62, BYTE_BYTE_LEN, 0}, //Dark1 0x1262
	{0x19, 0x4b, BYTE_BYTE_LEN, 0},
	{0x1a, 0x63, BYTE_BYTE_LEN, 0}, //Dark1 0x1263
	{0x1b, 0x41, BYTE_BYTE_LEN, 0},
	{0x1c, 0x64, BYTE_BYTE_LEN, 0}, //Dark1 0x1264
	{0x1d, 0x14, BYTE_BYTE_LEN, 0},
	{0x1e, 0x65, BYTE_BYTE_LEN, 0}, //Dark1 0x1265
	{0x1f, 0x00, BYTE_BYTE_LEN, 0},
	{0x20, 0x68, BYTE_BYTE_LEN, 0}, //Dark1 0x1268
	{0x21, 0x0a, BYTE_BYTE_LEN, 0},
	{0x22, 0x69, BYTE_BYTE_LEN, 0}, //Dark1 0x1269
	{0x23, 0x04, BYTE_BYTE_LEN, 0},
	{0x24, 0x6a, BYTE_BYTE_LEN, 0}, //Dark1 0x126a
	{0x25, 0x0a, BYTE_BYTE_LEN, 0},
	{0x26, 0x6b, BYTE_BYTE_LEN, 0}, //Dark1 0x126b
	{0x27, 0x0a, BYTE_BYTE_LEN, 0},
	{0x28, 0x6c, BYTE_BYTE_LEN, 0}, //Dark1 0x126c
	{0x29, 0x24, BYTE_BYTE_LEN, 0},
	{0x2a, 0x6d, BYTE_BYTE_LEN, 0}, //Dark1 0x126d
	{0x2b, 0x01, BYTE_BYTE_LEN, 0},
	{0x2c, 0x70, BYTE_BYTE_LEN, 0}, //Dark1 0x1270
	{0x2d, 0x00, BYTE_BYTE_LEN, 0},
	{0x2e, 0x71, BYTE_BYTE_LEN, 0}, //Dark1 0x1271
	{0x2f, 0xbf, BYTE_BYTE_LEN, 0},
	{0x30, 0x80, BYTE_BYTE_LEN, 0}, //Dark1 0x1280
	{0x31, 0x64, BYTE_BYTE_LEN, 0},
	{0x32, 0x81, BYTE_BYTE_LEN, 0}, //Dark1 0x1281
	{0x33, 0xb1, BYTE_BYTE_LEN, 0},
	{0x34, 0x82, BYTE_BYTE_LEN, 0}, //Dark1 0x1282
	{0x35, 0x2c, BYTE_BYTE_LEN, 0},
	{0x36, 0x83, BYTE_BYTE_LEN, 0}, //Dark1 0x1283
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x38, 0x84, BYTE_BYTE_LEN, 0}, //Dark1 0x1284
	{0x39, 0x30, BYTE_BYTE_LEN, 0},
	{0x3a, 0x85, BYTE_BYTE_LEN, 0}, //Dark1 0x1285
	{0x3b, 0x90, BYTE_BYTE_LEN, 0},
	{0x3c, 0x86, BYTE_BYTE_LEN, 0}, //Dark1 0x1286
	{0x3d, 0x10, BYTE_BYTE_LEN, 0},
	{0x3e, 0x87, BYTE_BYTE_LEN, 0}, //Dark1 0x1287
	{0x3f, 0x01, BYTE_BYTE_LEN, 0},
	{0x40, 0x88, BYTE_BYTE_LEN, 0}, //Dark1 0x1288
	{0x41, 0x3a, BYTE_BYTE_LEN, 0},
	{0x42, 0x89, BYTE_BYTE_LEN, 0}, //Dark1 0x1289
	{0x43, 0x90, BYTE_BYTE_LEN, 0},
	{0x44, 0x8a, BYTE_BYTE_LEN, 0}, //Dark1 0x128a
	{0x45, 0x0e, BYTE_BYTE_LEN, 0},
	{0x46, 0x8b, BYTE_BYTE_LEN, 0}, //Dark1 0x128b
	{0x47, 0x0c, BYTE_BYTE_LEN, 0},
	{0x48, 0x8c, BYTE_BYTE_LEN, 0}, //Dark1 0x128c
	{0x49, 0x05, BYTE_BYTE_LEN, 0},
	{0x4a, 0x8d, BYTE_BYTE_LEN, 0}, //Dark1 0x128d
	{0x4b, 0x03, BYTE_BYTE_LEN, 0},
	{0x4c, 0xe6, BYTE_BYTE_LEN, 0}, //Dark1 0x12e6
	{0x4d, 0xff, BYTE_BYTE_LEN, 0},
	{0x4e, 0xe7, BYTE_BYTE_LEN, 0}, //Dark1 0x12e7
	{0x4f, 0x18, BYTE_BYTE_LEN, 0},
	{0x50, 0xe8, BYTE_BYTE_LEN, 0}, //Dark1 0x12e8
	{0x51, 0x0a, BYTE_BYTE_LEN, 0},
	{0x52, 0xe9, BYTE_BYTE_LEN, 0}, //Dark1 0x12e9
	{0x53, 0x04, BYTE_BYTE_LEN, 0},
	{0x54, 0x03, BYTE_BYTE_LEN, 0},
	{0x55, 0x13, BYTE_BYTE_LEN, 0},//13 page
	{0x56, 0x10, BYTE_BYTE_LEN, 0}, //Dark1 0x1310
	{0x57, 0x3f, BYTE_BYTE_LEN, 0},
	{0x58, 0x20, BYTE_BYTE_LEN, 0}, //Dark1 0x1320
	{0x59, 0x20, BYTE_BYTE_LEN, 0},
	{0x5a, 0x21, BYTE_BYTE_LEN, 0}, //Dark1 0x1321
	{0x5b, 0x30, BYTE_BYTE_LEN, 0},
	{0x5c, 0x22, BYTE_BYTE_LEN, 0}, //Dark1 0x1322
	{0x5d, 0x36, BYTE_BYTE_LEN, 0},
	{0x5e, 0x23, BYTE_BYTE_LEN, 0}, //Dark1 0x1323
	{0x5f, 0x6a, BYTE_BYTE_LEN, 0},
	{0x60, 0x24, BYTE_BYTE_LEN, 0}, //Dark1 0x1324
	{0x61, 0xa0, BYTE_BYTE_LEN, 0},
	{0x62, 0x25, BYTE_BYTE_LEN, 0}, //Dark1 0x1325
	{0x63, 0xc0, BYTE_BYTE_LEN, 0},
	{0x64, 0x26, BYTE_BYTE_LEN, 0}, //Dark1 0x1326
	{0x65, 0xe0, BYTE_BYTE_LEN, 0},
	{0x66, 0x27, BYTE_BYTE_LEN, 0}, //Dark1 0x1327
	{0x67, 0x04, BYTE_BYTE_LEN, 0},
	{0x68, 0x28, BYTE_BYTE_LEN, 0}, //Dark1 0x1328
	{0x69, 0x05, BYTE_BYTE_LEN, 0},
	{0x6a, 0x29, BYTE_BYTE_LEN, 0}, //Dark1 0x1329
	{0x6b, 0x06, BYTE_BYTE_LEN, 0},
	{0x6c, 0x2a, BYTE_BYTE_LEN, 0}, //Dark1 0x132a
	{0x6d, 0x08, BYTE_BYTE_LEN, 0},
	{0x6e, 0x2b, BYTE_BYTE_LEN, 0}, //Dark1 0x132b
	{0x6f, 0x0a, BYTE_BYTE_LEN, 0},
	{0x70, 0x2c, BYTE_BYTE_LEN, 0}, //Dark1 0x132c
	{0x71, 0x0c, BYTE_BYTE_LEN, 0},
	{0x72, 0x2d, BYTE_BYTE_LEN, 0}, //Dark1 0x132d
	{0x73, 0x12, BYTE_BYTE_LEN, 0},
	{0x74, 0x2e, BYTE_BYTE_LEN, 0}, //Dark1 0x132e
	{0x75, 0x16, BYTE_BYTE_LEN, 0},
	{0x76, 0x2f, BYTE_BYTE_LEN, 0}, //Dark1 0x132f	   //weight skin
	{0x77, 0x04, BYTE_BYTE_LEN, 0},
	{0x78, 0x30, BYTE_BYTE_LEN, 0}, //Dark1 0x1330	   //weight blue
	{0x79, 0x04, BYTE_BYTE_LEN, 0},
	{0x7a, 0x31, BYTE_BYTE_LEN, 0}, //Dark1 0x1331	   //weight green
	{0x7b, 0x04, BYTE_BYTE_LEN, 0},
	{0x7c, 0x32, BYTE_BYTE_LEN, 0}, //Dark1 0x1332	   //weight strong color
	{0x7d, 0x04, BYTE_BYTE_LEN, 0},
	{0x7e, 0x33, BYTE_BYTE_LEN, 0}, //Dark1 0x1333
	{0x7f, 0x40, BYTE_BYTE_LEN, 0},
	{0x80, 0x34, BYTE_BYTE_LEN, 0}, //Dark1 0x1334
	{0x81, 0x80, BYTE_BYTE_LEN, 0},
	{0x82, 0x35, BYTE_BYTE_LEN, 0}, //Dark1 0x1335
	{0x83, 0x00, BYTE_BYTE_LEN, 0},
	{0x84, 0x36, BYTE_BYTE_LEN, 0}, //Dark1 0x1336
	{0x85, 0x80, BYTE_BYTE_LEN, 0},
	{0x86, 0xa0, BYTE_BYTE_LEN, 0}, //Dark1 0x13a0
	{0x87, 0x07, BYTE_BYTE_LEN, 0},
	{0x88, 0xa8, BYTE_BYTE_LEN, 0}, //Dark1 0x13a8	   //Dark1 Cb-filter 0x20
	{0x89, 0x30, BYTE_BYTE_LEN, 0},
	{0x8a, 0xa9, BYTE_BYTE_LEN, 0}, //Dark1 0x13a9	   //Dark1 Cr-filter 0x20
	{0x8b, 0x30, BYTE_BYTE_LEN, 0},
	{0x8c, 0xaa, BYTE_BYTE_LEN, 0}, //Dark1 0x13aa
	{0x8d, 0x30, BYTE_BYTE_LEN, 0},
	{0x8e, 0xab, BYTE_BYTE_LEN, 0}, //Dark1 0x13ab
	{0x8f, 0x02, BYTE_BYTE_LEN, 0},
	{0x90, 0xc0, BYTE_BYTE_LEN, 0}, //Dark1 0x13c0
	{0x91, 0x27, BYTE_BYTE_LEN, 0},
	{0x92, 0xc2, BYTE_BYTE_LEN, 0}, //Dark1 0x13c2
	{0x93, 0x08, BYTE_BYTE_LEN, 0},
	{0x94, 0xc3, BYTE_BYTE_LEN, 0}, //Dark1 0x13c3
	{0x95, 0x08, BYTE_BYTE_LEN, 0},
	{0x96, 0xc4, BYTE_BYTE_LEN, 0}, //Dark1 0x13c4
	{0x97, 0x46, BYTE_BYTE_LEN, 0},
	{0x98, 0xc5, BYTE_BYTE_LEN, 0}, //Dark1 0x13c5
	{0x99, 0x78, BYTE_BYTE_LEN, 0},
	{0x9a, 0xc6, BYTE_BYTE_LEN, 0}, //Dark1 0x13c6
	{0x9b, 0xf0, BYTE_BYTE_LEN, 0},
	{0x9c, 0xc7, BYTE_BYTE_LEN, 0}, //Dark1 0x13c7
	{0x9d, 0x10, BYTE_BYTE_LEN, 0},
	{0x9e, 0xc8, BYTE_BYTE_LEN, 0}, //Dark1 0x13c8
	{0x9f, 0x44, BYTE_BYTE_LEN, 0},
	{0xa0, 0xc9, BYTE_BYTE_LEN, 0}, //Dark1 0x13c9
	{0xa1, 0x87, BYTE_BYTE_LEN, 0},
	{0xa2, 0xca, BYTE_BYTE_LEN, 0}, //Dark1 0x13ca
	{0xa3, 0xff, BYTE_BYTE_LEN, 0},
	{0xa4, 0xcb, BYTE_BYTE_LEN, 0}, //Dark1 0x13cb
	{0xa5, 0x20, BYTE_BYTE_LEN, 0},
	{0xa6, 0xcc, BYTE_BYTE_LEN, 0}, //Dark1 0x13cc	   //skin range_cb_l
	{0xa7, 0x61, BYTE_BYTE_LEN, 0},
	{0xa8, 0xcd, BYTE_BYTE_LEN, 0}, //Dark1 0x13cd	   //skin range_cb_h
	{0xa9, 0x87, BYTE_BYTE_LEN, 0},
	{0xaa, 0xce, BYTE_BYTE_LEN, 0}, //Dark1 0x13ce	   //skin range_cr_l
	{0xab, 0x8a, BYTE_BYTE_LEN, 0},
	{0xac, 0xcf, BYTE_BYTE_LEN, 0}, //Dark1 0x13cf	   //skin range_cr_h
	{0xad, 0xa5, BYTE_BYTE_LEN, 0},
	{0xae, 0x03, BYTE_BYTE_LEN, 0}, //14 page
	{0xaf, 0x14, BYTE_BYTE_LEN, 0},
	{0xb0, 0x10, BYTE_BYTE_LEN, 0}, //Dark1 0x1410
	{0xb1, 0x00, BYTE_BYTE_LEN, 0},
	{0xb2, 0x11, BYTE_BYTE_LEN, 0}, //Dark1 0x1411
	{0xb3, 0x00, BYTE_BYTE_LEN, 0},
	{0xb4, 0x12, BYTE_BYTE_LEN, 0}, //Dark1 0x1412
	{0xb5, 0x40, BYTE_BYTE_LEN, 0}, //Top H_Clip
	{0xb6, 0x13, BYTE_BYTE_LEN, 0}, //Dark1 0x1413
	{0xb7, 0xc8, BYTE_BYTE_LEN, 0},
	{0xb8, 0x14, BYTE_BYTE_LEN, 0}, //Dark1 0x1414
	{0xb9, 0x50, BYTE_BYTE_LEN, 0},
	{0xba, 0x15, BYTE_BYTE_LEN, 0}, //Dark1 0x1415	   //sharp positive hi
	{0xbb, 0x10, BYTE_BYTE_LEN, 0},
	{0xbc, 0x16, BYTE_BYTE_LEN, 0}, //Dark1 0x1416	   //sharp positive mi
	{0xbd, 0x10, BYTE_BYTE_LEN, 0},
	{0xbe, 0x17, BYTE_BYTE_LEN, 0}, //Dark1 0x1417	   //sharp positive low
	{0xbf, 0x10, BYTE_BYTE_LEN, 0},
	{0xc0, 0x18, BYTE_BYTE_LEN, 0}, //Dark1 0x1418	   //sharp negative hi
	{0xc1, 0x28, BYTE_BYTE_LEN, 0},
	{0xc2, 0x19, BYTE_BYTE_LEN, 0}, //Dark1 0x1419	   //sharp negative mi
	{0xc3, 0x28, BYTE_BYTE_LEN, 0},
	{0xc4, 0x1a, BYTE_BYTE_LEN, 0}, //Dark1 0x141a	   //sharp negative low
	{0xc5, 0x28, BYTE_BYTE_LEN, 0},
	{0xc6, 0x20, BYTE_BYTE_LEN, 0}, //Dark1 0x1420
	{0xc7, 0x80, BYTE_BYTE_LEN, 0},
	{0xc8, 0x21, BYTE_BYTE_LEN, 0}, //Dark1 0x1421
	{0xc9, 0x03, BYTE_BYTE_LEN, 0},
	{0xca, 0x22, BYTE_BYTE_LEN, 0}, //Dark1 0x1422
	{0xcb, 0x05, BYTE_BYTE_LEN, 0},
	{0xcc, 0x23, BYTE_BYTE_LEN, 0}, //Dark1 0x1423
	{0xcd, 0x07, BYTE_BYTE_LEN, 0},
	{0xce, 0x24, BYTE_BYTE_LEN, 0}, //Dark1 0x1424
	{0xcf, 0x0a, BYTE_BYTE_LEN, 0},
	{0xd0, 0x25, BYTE_BYTE_LEN, 0}, //Dark1 0x1425
	{0xd1, 0x46, BYTE_BYTE_LEN, 0},
	{0xd2, 0x26, BYTE_BYTE_LEN, 0}, //Dark1 0x1426
	{0xd3, 0x32, BYTE_BYTE_LEN, 0},
	{0xd4, 0x27, BYTE_BYTE_LEN, 0}, //Dark1 0x1427
	{0xd5, 0x1e, BYTE_BYTE_LEN, 0},
	{0xd6, 0x28, BYTE_BYTE_LEN, 0}, //Dark1 0x1428
	{0xd7, 0x19, BYTE_BYTE_LEN, 0},
	{0xd8, 0x29, BYTE_BYTE_LEN, 0}, //Dark1 0x1429
	{0xd9, 0x00, BYTE_BYTE_LEN, 0},
	{0xda, 0x2a, BYTE_BYTE_LEN, 0}, //Dark1 0x142a
	{0xdb, 0x10, BYTE_BYTE_LEN, 0},
	{0xdc, 0x2b, BYTE_BYTE_LEN, 0}, //Dark1 0x142b
	{0xdd, 0x10, BYTE_BYTE_LEN, 0},
	{0xde, 0x2c, BYTE_BYTE_LEN, 0}, //Dark1 0x142c
	{0xdf, 0x10, BYTE_BYTE_LEN, 0},
	{0xe0, 0x2d, BYTE_BYTE_LEN, 0}, //Dark1 0x142d
	{0xe1, 0x80, BYTE_BYTE_LEN, 0},
	{0xe2, 0x2e, BYTE_BYTE_LEN, 0}, //Dark1 0x142e
	{0xe3, 0x80, BYTE_BYTE_LEN, 0},
	{0xe4, 0x2f, BYTE_BYTE_LEN, 0}, //Dark1 0x142f
	{0xe5, 0x80, BYTE_BYTE_LEN, 0},
	{0xe6, 0x30, BYTE_BYTE_LEN, 0}, //Dark1 0x1430
	{0xe7, 0x80, BYTE_BYTE_LEN, 0},
	{0xe8, 0x31, BYTE_BYTE_LEN, 0}, //Dark1 0x1431
	{0xe9, 0x02, BYTE_BYTE_LEN, 0},
	{0xea, 0x32, BYTE_BYTE_LEN, 0}, //Dark1 0x1432
	{0xeb, 0x04, BYTE_BYTE_LEN, 0},
	{0xec, 0x33, BYTE_BYTE_LEN, 0}, //Dark1 0x1433
	{0xed, 0x04, BYTE_BYTE_LEN, 0},
	{0xee, 0x34, BYTE_BYTE_LEN, 0}, //Dark1 0x1434
	{0xef, 0x0a, BYTE_BYTE_LEN, 0},
	{0xf0, 0x35, BYTE_BYTE_LEN, 0}, //Dark1 0x1435
	{0xf1, 0x46, BYTE_BYTE_LEN, 0},
	{0xf2, 0x36, BYTE_BYTE_LEN, 0}, //Dark1 0x1436
	{0xf3, 0x32, BYTE_BYTE_LEN, 0},
	{0xf4, 0x37, BYTE_BYTE_LEN, 0}, //Dark1 0x1437
	{0xf5, 0x28, BYTE_BYTE_LEN, 0},
	{0xf6, 0x38, BYTE_BYTE_LEN, 0}, //Dark1 0x1438
	{0xf7, 0x12, BYTE_BYTE_LEN, 0},//2d
	{0xf8, 0x39, BYTE_BYTE_LEN, 0}, //Dark1 0x1439
	{0xf9, 0x00, BYTE_BYTE_LEN, 0},//23
	{0xfa, 0x3a, BYTE_BYTE_LEN, 0}, //Dark1 0x143a
	{0xfb, 0x18, BYTE_BYTE_LEN, 0}, //dr gain
	{0xfc, 0x3b, BYTE_BYTE_LEN, 0}, //Dark1 0x143b
	{0xfd, 0x20, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xe2, BYTE_BYTE_LEN, 0},
	{0x10, 0x3c, BYTE_BYTE_LEN, 0}, //Dark1 0x143c
	{0x11, 0x18, BYTE_BYTE_LEN, 0},
	{0x12, 0x3d, BYTE_BYTE_LEN, 0}, //Dark1 0x143d
	{0x13, 0x20, BYTE_BYTE_LEN, 0}, //nor gain
	{0x14, 0x3e, BYTE_BYTE_LEN, 0}, //Dark1 0x143e
	{0x15, 0x22, BYTE_BYTE_LEN, 0},
	{0x16, 0x3f, BYTE_BYTE_LEN, 0}, //Dark1 0x143f
	{0x17, 0x10, BYTE_BYTE_LEN, 0},
	{0x18, 0x40, BYTE_BYTE_LEN, 0}, //Dark1 0x1440
	{0x19, 0x80, BYTE_BYTE_LEN, 0},
	{0x1a, 0x41, BYTE_BYTE_LEN, 0}, //Dark1 0x1441
	{0x1b, 0x12, BYTE_BYTE_LEN, 0},
	{0x1c, 0x42, BYTE_BYTE_LEN, 0}, //Dark1 0x1442
	{0x1d, 0xb0, BYTE_BYTE_LEN, 0},
	{0x1e, 0x43, BYTE_BYTE_LEN, 0}, //Dark1 0x1443
	{0x1f, 0x20, BYTE_BYTE_LEN, 0},
	{0x20, 0x44, BYTE_BYTE_LEN, 0}, //Dark1 0x1444
	{0x21, 0x20, BYTE_BYTE_LEN, 0},
	{0x22, 0x45, BYTE_BYTE_LEN, 0}, //Dark1 0x1445
	{0x23, 0x20, BYTE_BYTE_LEN, 0},
	{0x24, 0x46, BYTE_BYTE_LEN, 0}, //Dark1 0x1446
	{0x25, 0x20, BYTE_BYTE_LEN, 0},
	{0x26, 0x47, BYTE_BYTE_LEN, 0}, //Dark1 0x1447
	{0x27, 0x08, BYTE_BYTE_LEN, 0},
	{0x28, 0x48, BYTE_BYTE_LEN, 0}, //Dark1 0x1448
	{0x29, 0x08, BYTE_BYTE_LEN, 0},
	{0x2a, 0x49, BYTE_BYTE_LEN, 0}, //Dark1 0x1449
	{0x2b, 0x08, BYTE_BYTE_LEN, 0},
	{0x2c, 0x50, BYTE_BYTE_LEN, 0}, //Dark1 0x1450
	{0x2d, 0x80, BYTE_BYTE_LEN, 0},
	{0x2e, 0x51, BYTE_BYTE_LEN, 0}, //Dark1 0x1451
	{0x2f, 0x32, BYTE_BYTE_LEN, 0}, //
	{0x30, 0x52, BYTE_BYTE_LEN, 0}, //Dark1 0x1452
	{0x31, 0x40, BYTE_BYTE_LEN, 0},
	{0x32, 0x53, BYTE_BYTE_LEN, 0}, //Dark1 0x1453
	{0x33, 0x19, BYTE_BYTE_LEN, 0},
	{0x34, 0x54, BYTE_BYTE_LEN, 0}, //Dark1 0x1454
	{0x35, 0x60, BYTE_BYTE_LEN, 0},
	{0x36, 0x55, BYTE_BYTE_LEN, 0}, //Dark1 0x1455
	{0x37, 0x60, BYTE_BYTE_LEN, 0},
	{0x38, 0x56, BYTE_BYTE_LEN, 0}, //Dark1 0x1456
	{0x39, 0x60, BYTE_BYTE_LEN, 0},
	{0x3a, 0x57, BYTE_BYTE_LEN, 0}, //Dark1 0x1457
	{0x3b, 0x20, BYTE_BYTE_LEN, 0},
	{0x3c, 0x58, BYTE_BYTE_LEN, 0}, //Dark1 0x1458
	{0x3d, 0x20, BYTE_BYTE_LEN, 0},
	{0x3e, 0x59, BYTE_BYTE_LEN, 0}, //Dark1 0x1459
	{0x3f, 0x20, BYTE_BYTE_LEN, 0},
	{0x40, 0x60, BYTE_BYTE_LEN, 0}, //Dark1 0x1460
	{0x41, 0x03, BYTE_BYTE_LEN, 0}, //skin opt en
	{0x42, 0x61, BYTE_BYTE_LEN, 0}, //Dark1 0x1461
	{0x43, 0xa0, BYTE_BYTE_LEN, 0},
	{0x44, 0x62, BYTE_BYTE_LEN, 0}, //Dark1 0x1462
	{0x45, 0x98, BYTE_BYTE_LEN, 0},
	{0x46, 0x63, BYTE_BYTE_LEN, 0}, //Dark1 0x1463
	{0x47, 0xe4, BYTE_BYTE_LEN, 0}, //skin_std_th_h
	{0x48, 0x64, BYTE_BYTE_LEN, 0}, //Dark1 0x1464
	{0x49, 0xa4, BYTE_BYTE_LEN, 0}, //skin_std_th_l
	{0x4a, 0x65, BYTE_BYTE_LEN, 0}, //Dark1 0x1465
	{0x4b, 0x7d, BYTE_BYTE_LEN, 0}, //sharp_std_th_h
	{0x4c, 0x66, BYTE_BYTE_LEN, 0}, //Dark1 0x1466
	{0x4d, 0x4b, BYTE_BYTE_LEN, 0}, //sharp_std_th_l
	{0x4e, 0x70, BYTE_BYTE_LEN, 0}, //Dark1 0x1470
	{0x4f, 0x10, BYTE_BYTE_LEN, 0},
	{0x50, 0x71, BYTE_BYTE_LEN, 0}, //Dark1 0x1471
	{0x51, 0x10, BYTE_BYTE_LEN, 0},
	{0x52, 0x72, BYTE_BYTE_LEN, 0}, //Dark1 0x1472
	{0x53, 0x10, BYTE_BYTE_LEN, 0},
	{0x54, 0x73, BYTE_BYTE_LEN, 0}, //Dark1 0x1473
	{0x55, 0x10, BYTE_BYTE_LEN, 0},
	{0x56, 0x74, BYTE_BYTE_LEN, 0}, //Dark1 0x1474
	{0x57, 0x10, BYTE_BYTE_LEN, 0},
	{0x58, 0x75, BYTE_BYTE_LEN, 0}, //Dark1 0x1475
	{0x59, 0x10, BYTE_BYTE_LEN, 0},
	{0x5a, 0x76, BYTE_BYTE_LEN, 0}, //Dark1 0x1476
	{0x5b, 0x28, BYTE_BYTE_LEN, 0},
	{0x5c, 0x77, BYTE_BYTE_LEN, 0}, //Dark1 0x1477
	{0x5d, 0x28, BYTE_BYTE_LEN, 0},
	{0x5e, 0x78, BYTE_BYTE_LEN, 0}, //Dark1 0x1478
	{0x5f, 0x28, BYTE_BYTE_LEN, 0},
	{0x60, 0x79, BYTE_BYTE_LEN, 0}, //Dark1 0x1479
	{0x61, 0x28, BYTE_BYTE_LEN, 0},
	{0x62, 0x7a, BYTE_BYTE_LEN, 0}, //Dark1 0x147a
	{0x63, 0x28, BYTE_BYTE_LEN, 0},
	{0x64, 0x7b, BYTE_BYTE_LEN, 0}, //Dark1 0x147b
	{0x65, 0x28, BYTE_BYTE_LEN, 0},
	  
	
	//////////////////
	// e3 Page (DMA Dark2)
	//////////////////
	
	{0x03, 0xe3, BYTE_BYTE_LEN, 0},
	{0x10, 0x03, BYTE_BYTE_LEN, 0},//Dark2 Page11
	{0x11, 0x11, BYTE_BYTE_LEN, 0},
	{0x12, 0x10, BYTE_BYTE_LEN, 0},//Dark2 0x1110
	{0x13, 0x1f, BYTE_BYTE_LEN, 0}, 
	{0x14, 0x11, BYTE_BYTE_LEN, 0},//Dark2 0x1111
	{0x15, 0x2a, BYTE_BYTE_LEN, 0},
	{0x16, 0x12, BYTE_BYTE_LEN, 0},//Dark2 0x1112
	{0x17, 0x32, BYTE_BYTE_LEN, 0},
	{0x18, 0x13, BYTE_BYTE_LEN, 0},//Dark2 0x1113
	{0x19, 0x21, BYTE_BYTE_LEN, 0},
	{0x1a, 0x14, BYTE_BYTE_LEN, 0},//Dark2 0x1114
	{0x1b, 0x3a, BYTE_BYTE_LEN, 0},
	{0x1c, 0x30, BYTE_BYTE_LEN, 0},//Dark2 0x1130
	{0x1d, 0x20, BYTE_BYTE_LEN, 0},
	{0x1e, 0x31, BYTE_BYTE_LEN, 0},//Dark2 0x1131
	{0x1f, 0x20, BYTE_BYTE_LEN, 0},
	
	{0x20, 0x32, BYTE_BYTE_LEN, 0},  //Dark2 0x1132 //STEVE Lum. Level. in DLPF               
	{0x21, 0x8b, BYTE_BYTE_LEN, 0},  //52                    82                               
	{0x22, 0x33, BYTE_BYTE_LEN, 0},  //Dark2 0x1133                                           
	{0x23, 0x54, BYTE_BYTE_LEN, 0},  //3b                    5d                               
	{0x24, 0x34, BYTE_BYTE_LEN, 0},  //Dark2 0x1134                                           
	{0x25, 0x2c, BYTE_BYTE_LEN, 0},  //1d                    37                               
	{0x26, 0x35, BYTE_BYTE_LEN, 0},  //Dark2 0x1135                                           
	{0x27, 0x29, BYTE_BYTE_LEN, 0},  //21                    30                               
	{0x28, 0x36, BYTE_BYTE_LEN, 0},  //Dark2 0x1136                                           
	{0x29, 0x18, BYTE_BYTE_LEN, 0},  //1b                    18                               
	{0x2a, 0x37, BYTE_BYTE_LEN, 0},  //Dark2 0x1137                                           
	{0x2b, 0x1e, BYTE_BYTE_LEN, 0},  //21                    24                               
	{0x2c, 0x38, BYTE_BYTE_LEN, 0},  //Dark2 0x1138                                           
	{0x2d, 0x17, BYTE_BYTE_LEN, 0},  //18                    18                               
	                                        
	{0x2e, 0x39, BYTE_BYTE_LEN, 0},//Dark2 0x1139 gain 1
	{0x2f, 0x8a, BYTE_BYTE_LEN, 0},    //r2 1
	{0x30, 0x3a, BYTE_BYTE_LEN, 0},//Dark2 0x113a
	{0x31, 0x8a, BYTE_BYTE_LEN, 0},
	{0x32, 0x3b, BYTE_BYTE_LEN, 0},//Dark2 0x113b
	{0x33, 0x8a, BYTE_BYTE_LEN, 0},
	{0x34, 0x3c, BYTE_BYTE_LEN, 0},//Dark2 0x113c
	{0x35, 0x8a, BYTE_BYTE_LEN, 0},   //18
	{0x36, 0x3d, BYTE_BYTE_LEN, 0},//Dark2 0x113d
	{0x37, 0x8a, BYTE_BYTE_LEN, 0},   //18
	{0x38, 0x3e, BYTE_BYTE_LEN, 0},//Dark2 0x113e
	{0x39, 0x8a, BYTE_BYTE_LEN, 0},   //18
	{0x3a, 0x3f, BYTE_BYTE_LEN, 0},//Dark2 0x113f
	{0x3b, 0x8a, BYTE_BYTE_LEN, 0},
	{0x3c, 0x40, BYTE_BYTE_LEN, 0},//Dark2 0x1140 gain 8
	{0x3d, 0x8a, BYTE_BYTE_LEN, 0},
	{0x3e, 0x41, BYTE_BYTE_LEN, 0},//Dark2 0x1141
	{0x3f, 0x40, BYTE_BYTE_LEN, 0},
	{0x40, 0x42, BYTE_BYTE_LEN, 0},//Dark2 0x1142
	{0x41, 0x10, BYTE_BYTE_LEN, 0},
	{0x42, 0x43, BYTE_BYTE_LEN, 0},//Dark2 0x1143
	{0x43, 0x10, BYTE_BYTE_LEN, 0},
	{0x44, 0x44, BYTE_BYTE_LEN, 0},//Dark2 0x1144
	{0x45, 0x10, BYTE_BYTE_LEN, 0},
	{0x46, 0x45, BYTE_BYTE_LEN, 0},//Dark2 0x1145
	{0x47, 0x10, BYTE_BYTE_LEN, 0},
	{0x48, 0x46, BYTE_BYTE_LEN, 0},//Dark2 0x1146
	{0x49, 0x10, BYTE_BYTE_LEN, 0},
	{0x4a, 0x47, BYTE_BYTE_LEN, 0},//Dark2 0x1147
	{0x4b, 0x10, BYTE_BYTE_LEN, 0},
	{0x4c, 0x48, BYTE_BYTE_LEN, 0},//Dark2 0x1148
	{0x4d, 0x10, BYTE_BYTE_LEN, 0},
	{0x4e, 0x49, BYTE_BYTE_LEN, 0},//Dark2 0x1149
	{0x4f, 0x00, BYTE_BYTE_LEN, 0}, //high_clip_start
	{0x50, 0x4a, BYTE_BYTE_LEN, 0},//Dark2 0x114a
	{0x51, 0x00, BYTE_BYTE_LEN, 0},
	{0x52, 0x4b, BYTE_BYTE_LEN, 0},//Dark2 0x114b
	{0x53, 0x00, BYTE_BYTE_LEN, 0},
	{0x54, 0x4c, BYTE_BYTE_LEN, 0},//Dark2 0x114c
	{0x55, 0x00, BYTE_BYTE_LEN, 0},
	{0x56, 0x4d, BYTE_BYTE_LEN, 0},//Dark2 0x114d
	{0x57, 0x00, BYTE_BYTE_LEN, 0},
	{0x58, 0x4e, BYTE_BYTE_LEN, 0},//Dark2 0x114e
	{0x59, 0x00, BYTE_BYTE_LEN, 0},   //Lv 6 h_clip
	{0x5a, 0x4f, BYTE_BYTE_LEN, 0},//Dark2 0x114f
	{0x5b, 0x00, BYTE_BYTE_LEN, 0},   //Lv 7 h_clip 
	{0x5c, 0x50, BYTE_BYTE_LEN, 0},//Dark2 0x1150 clip 8
	{0x5d, 0x00, BYTE_BYTE_LEN, 0},
	{0x5e, 0x51, BYTE_BYTE_LEN, 0},//Dark2 0x1151
	{0x5f, 0xd8, BYTE_BYTE_LEN, 0}, //color gain start
	{0x60, 0x52, BYTE_BYTE_LEN, 0},//Dark2 0x1152
	{0x61, 0xd8, BYTE_BYTE_LEN, 0},
	{0x62, 0x53, BYTE_BYTE_LEN, 0},//Dark2 0x1153
	{0x63, 0xd8, BYTE_BYTE_LEN, 0},
	{0x64, 0x54, BYTE_BYTE_LEN, 0},//Dark2 0x1154
	{0x65, 0xd0, BYTE_BYTE_LEN, 0},
	{0x66, 0x55, BYTE_BYTE_LEN, 0},//Dark2 0x1155
	{0x67, 0xd0, BYTE_BYTE_LEN, 0},
	{0x68, 0x56, BYTE_BYTE_LEN, 0},//Dark2 0x1156
	{0x69, 0xc8, BYTE_BYTE_LEN, 0},
	{0x6a, 0x57, BYTE_BYTE_LEN, 0},//Dark2 0x1157
	{0x6b, 0xc0, BYTE_BYTE_LEN, 0},
	{0x6c, 0x58, BYTE_BYTE_LEN, 0},//Dark2 0x1158
	{0x6d, 0xc0, BYTE_BYTE_LEN, 0}, //color gain end
	{0x6e, 0x59, BYTE_BYTE_LEN, 0},//Dark2 0x1159
	{0x6f, 0xf0, BYTE_BYTE_LEN, 0}, //color ofs lmt start
	{0x70, 0x5a, BYTE_BYTE_LEN, 0},//Dark2 0x115a
	{0x71, 0xf0, BYTE_BYTE_LEN, 0},
	{0x72, 0x5b, BYTE_BYTE_LEN, 0},//Dark2 0x115b
	{0x73, 0xf0, BYTE_BYTE_LEN, 0},
	{0x74, 0x5c, BYTE_BYTE_LEN, 0},//Dark2 0x115c
	{0x75, 0xe8, BYTE_BYTE_LEN, 0},
	{0x76, 0x5d, BYTE_BYTE_LEN, 0},//Dark2 0x115d
	{0x77, 0xe8, BYTE_BYTE_LEN, 0},
	{0x78, 0x5e, BYTE_BYTE_LEN, 0},//Dark2 0x115e
	{0x79, 0xe0, BYTE_BYTE_LEN, 0},
	{0x7a, 0x5f, BYTE_BYTE_LEN, 0},//Dark2 0x115f
	{0x7b, 0xe0, BYTE_BYTE_LEN, 0},
	{0x7c, 0x60, BYTE_BYTE_LEN, 0},//Dark2 0x1160
	{0x7d, 0xe0, BYTE_BYTE_LEN, 0},//color ofs lmt end
	{0x7e, 0x61, BYTE_BYTE_LEN, 0},//Dark2 0x1161
	{0x7f, 0xf0, BYTE_BYTE_LEN, 0},
	{0x80, 0x62, BYTE_BYTE_LEN, 0},//Dark2 0x1162
	{0x81, 0xf0, BYTE_BYTE_LEN, 0},
	{0x82, 0x63, BYTE_BYTE_LEN, 0},//Dark2 0x1163
	{0x83, 0x80, BYTE_BYTE_LEN, 0},
	{0x84, 0x64, BYTE_BYTE_LEN, 0},//Dark2 0x1164
	{0x85, 0x40, BYTE_BYTE_LEN, 0},
	{0x86, 0x65, BYTE_BYTE_LEN, 0},//Dark2 0x1165
	{0x87, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0x88, 0x66, BYTE_BYTE_LEN, 0},//Dark2 0x1166
	{0x89, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0x8a, 0x67, BYTE_BYTE_LEN, 0},//Dark2 0x1167
	{0x8b, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0x8c, 0x68, BYTE_BYTE_LEN, 0},//Dark2 0x1168
	{0x8d, 0x80, BYTE_BYTE_LEN, 0},
	{0x8e, 0x69, BYTE_BYTE_LEN, 0},//Dark2 0x1169
	{0x8f, 0x40, BYTE_BYTE_LEN, 0},
	{0x90, 0x6a, BYTE_BYTE_LEN, 0},//Dark2 0x116a
	{0x91, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0x92, 0x6b, BYTE_BYTE_LEN, 0},//Dark2 0x116b
	{0x93, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0x94, 0x6c, BYTE_BYTE_LEN, 0},//Dark2 0x116c
	{0x95, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0x96, 0x6d, BYTE_BYTE_LEN, 0},//Dark2 0x116d
	{0x97, 0x80, BYTE_BYTE_LEN, 0},
	{0x98, 0x6e, BYTE_BYTE_LEN, 0},//Dark2 0x116e
	{0x99, 0x40, BYTE_BYTE_LEN, 0},
	{0x9a, 0x6f, BYTE_BYTE_LEN, 0},//Dark2 0x116f
	{0x9b, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0x9c, 0x70, BYTE_BYTE_LEN, 0},//Dark2 0x1170
	{0x9d, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0x9e, 0x71, BYTE_BYTE_LEN, 0},//Dark2 0x1171
	{0x9f, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xa0, 0x72, BYTE_BYTE_LEN, 0},//Dark2 0x1172
	{0xa1, 0x6e, BYTE_BYTE_LEN, 0},
	{0xa2, 0x73, BYTE_BYTE_LEN, 0},//Dark2 0x1173
	{0xa3, 0x3a, BYTE_BYTE_LEN, 0},
	{0xa4, 0x74, BYTE_BYTE_LEN, 0},//Dark2 0x1174
	{0xa5, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xa6, 0x75, BYTE_BYTE_LEN, 0},//Dark2 0x1175
	{0xa7, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xa8, 0x76, BYTE_BYTE_LEN, 0},//Dark2 0x1176
	{0xa9, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},//18
	{0xaa, 0x77, BYTE_BYTE_LEN, 0},//Dark2 0x1177
	{0xab, 0x6e, BYTE_BYTE_LEN, 0},
	{0xac, 0x78, BYTE_BYTE_LEN, 0},//Dark2 0x1178
	{0xad, 0x3a, BYTE_BYTE_LEN, 0},
	{0xae, 0x79, BYTE_BYTE_LEN, 0},//Dark2 0x1179
	{0xaf, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xb0, 0x7a, BYTE_BYTE_LEN, 0},//Dark2 0x117a
	{0xb1, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xb2, 0x7b, BYTE_BYTE_LEN, 0},//Dark2 0x117b
	{0xb3, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xb4, 0x7c, BYTE_BYTE_LEN, 0},//Dark2 0x117c
	{0xb5, 0x5c, BYTE_BYTE_LEN, 0},
	{0xb6, 0x7d, BYTE_BYTE_LEN, 0},//Dark2 0x117d
	{0xb7, 0x30, BYTE_BYTE_LEN, 0},
	{0xb8, 0x7e, BYTE_BYTE_LEN, 0},//Dark2 0x117e
	{0xb9, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xba, 0x7f, BYTE_BYTE_LEN, 0},//Dark2 0x117f
	{0xbb, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xbc, 0x80, BYTE_BYTE_LEN, 0},//Dark2 0x1180
	{0xbd, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xbe, 0x81, BYTE_BYTE_LEN, 0},//Dark2 0x1181
	{0xbf, 0x62, BYTE_BYTE_LEN, 0},
	{0xc0, 0x82, BYTE_BYTE_LEN, 0},//Dark2 0x1182
	{0xc1, 0x26, BYTE_BYTE_LEN, 0},
	{0xc2, 0x83, BYTE_BYTE_LEN, 0},//Dark2 0x1183
	{0xc3, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xc4, 0x84, BYTE_BYTE_LEN, 0},//Dark2 0x1184
	{0xc5, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xc6, 0x85, BYTE_BYTE_LEN, 0},//Dark2 0x1185
	{0xc7, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xc8, 0x86, BYTE_BYTE_LEN, 0},//Dark2 0x1186
	{0xc9, 0x62, BYTE_BYTE_LEN, 0},
	{0xca, 0x87, BYTE_BYTE_LEN, 0},//Dark2 0x1187
	{0xcb, 0x26, BYTE_BYTE_LEN, 0},
	{0xcc, 0x88, BYTE_BYTE_LEN, 0},//Dark2 0x1188
	{0xcd, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xce, 0x89, BYTE_BYTE_LEN, 0},//Dark2 0x1189
	{0xcf, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xd0, 0x8a, BYTE_BYTE_LEN, 0},//Dark2 0x118a
	{0xd1, 0x01, BYTE_BYTE_LEN, 0},//0224 jk//0x04, BYTE_BYTE_LEN, 0},
	{0xd2, 0x90, BYTE_BYTE_LEN, 0},//Dark2 0x1190
	{0xd3, 0x03, BYTE_BYTE_LEN, 0},
	{0xd4, 0x91, BYTE_BYTE_LEN, 0},//Dark2 0x1191
	{0xd5, 0xff, BYTE_BYTE_LEN, 0},
	{0xd6, 0x92, BYTE_BYTE_LEN, 0},//Dark2 0x1192
	{0xd7, 0x00, BYTE_BYTE_LEN, 0},
	{0xd8, 0x93, BYTE_BYTE_LEN, 0},//Dark2 0x1193
	{0xd9, 0x00, BYTE_BYTE_LEN, 0},
	{0xda, 0x94, BYTE_BYTE_LEN, 0},//Dark2 0x1194
	{0xdb, 0x03, BYTE_BYTE_LEN, 0},
	{0xdc, 0x95, BYTE_BYTE_LEN, 0},//Dark2 0x1195
	{0xdd, 0x64, BYTE_BYTE_LEN, 0},
	{0xde, 0x96, BYTE_BYTE_LEN, 0},//Dark2 0x1196
	{0xdf, 0x00, BYTE_BYTE_LEN, 0},
	{0xe0, 0x97, BYTE_BYTE_LEN, 0},//Dark2 0x1197
	{0xe1, 0x00, BYTE_BYTE_LEN, 0},
	{0xe2, 0xb0, BYTE_BYTE_LEN, 0},//Dark2 0x11b0
	{0xe3, 0x64, BYTE_BYTE_LEN, 0},
	{0xe4, 0xb1, BYTE_BYTE_LEN, 0},//Dark2 0x11b1
	{0xe5, 0x80, BYTE_BYTE_LEN, 0},
	{0xe6, 0xb2, BYTE_BYTE_LEN, 0},//Dark2 0x11b2
	{0xe7, 0x00, BYTE_BYTE_LEN, 0},
	{0xe8, 0xb3, BYTE_BYTE_LEN, 0},//Dark2 0x11b3
	{0xe9, 0x00, BYTE_BYTE_LEN, 0},
	{0xea, 0xb4, BYTE_BYTE_LEN, 0},//Dark2 0x11b4
	{0xeb, 0x03, BYTE_BYTE_LEN, 0},
	           
	{0xec, 0x03, BYTE_BYTE_LEN, 0},
	{0xed, 0x12, BYTE_BYTE_LEN, 0},//12 page
	{0xee, 0x10, BYTE_BYTE_LEN, 0}, //Dark2 0x1210
	{0xef, 0x03, BYTE_BYTE_LEN, 0},
	{0xf0, 0x11, BYTE_BYTE_LEN, 0}, //Dark2 0x1211
	{0xf1, 0x29, BYTE_BYTE_LEN, 0},
	{0xf2, 0x12, BYTE_BYTE_LEN, 0}, //Dark2 0x1212
	{0xf3, 0x08, BYTE_BYTE_LEN, 0},
	{0xf4, 0x40, BYTE_BYTE_LEN, 0}, //Dark2 0x1240
	{0xf5, 0x33, BYTE_BYTE_LEN, 0}, //07
	{0xf6, 0x41, BYTE_BYTE_LEN, 0}, //Dark2 0x1241
	{0xf7, 0x0a, BYTE_BYTE_LEN, 0}, //32
	{0xf8, 0x42, BYTE_BYTE_LEN, 0}, //Dark2 0x1242
	{0xf9, 0x6a, BYTE_BYTE_LEN, 0}, //8c
	{0xfa, 0x43, BYTE_BYTE_LEN, 0}, //Dark2 0x1243
	{0xfb, 0x80, BYTE_BYTE_LEN, 0},
	{0xfc, 0x44, BYTE_BYTE_LEN, 0}, //Dark2 0x1244
	{0xfd, 0x02, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xe4, BYTE_BYTE_LEN, 0},
	{0x10, 0x45, BYTE_BYTE_LEN, 0}, //Dark2 0x1245
	{0x11, 0x0a, BYTE_BYTE_LEN, 0},
	{0x12, 0x46, BYTE_BYTE_LEN, 0}, //Dark2 0x1246
	{0x13, 0x80, BYTE_BYTE_LEN, 0},
	{0x14, 0x60, BYTE_BYTE_LEN, 0}, //Dark2 0x1260
	{0x15, 0x02, BYTE_BYTE_LEN, 0},
	{0x16, 0x61, BYTE_BYTE_LEN, 0}, //Dark2 0x1261
	{0x17, 0x04, BYTE_BYTE_LEN, 0},
	{0x18, 0x62, BYTE_BYTE_LEN, 0}, //Dark2 0x1262
	{0x19, 0x4b, BYTE_BYTE_LEN, 0},
	{0x1a, 0x63, BYTE_BYTE_LEN, 0}, //Dark2 0x1263
	{0x1b, 0x41, BYTE_BYTE_LEN, 0},
	{0x1c, 0x64, BYTE_BYTE_LEN, 0}, //Dark2 0x1264
	{0x1d, 0x14, BYTE_BYTE_LEN, 0},
	{0x1e, 0x65, BYTE_BYTE_LEN, 0}, //Dark2 0x1265
	{0x1f, 0x00, BYTE_BYTE_LEN, 0},
	{0x20, 0x68, BYTE_BYTE_LEN, 0}, //Dark2 0x1268
	{0x21, 0x0a, BYTE_BYTE_LEN, 0},
	{0x22, 0x69, BYTE_BYTE_LEN, 0}, //Dark2 0x1269
	{0x23, 0x04, BYTE_BYTE_LEN, 0},
	{0x24, 0x6a, BYTE_BYTE_LEN, 0}, //Dark2 0x126a
	{0x25, 0x0a, BYTE_BYTE_LEN, 0},
	{0x26, 0x6b, BYTE_BYTE_LEN, 0}, //Dark2 0x126b
	{0x27, 0x0a, BYTE_BYTE_LEN, 0},
	{0x28, 0x6c, BYTE_BYTE_LEN, 0}, //Dark2 0x126c
	{0x29, 0x24, BYTE_BYTE_LEN, 0},
	{0x2a, 0x6d, BYTE_BYTE_LEN, 0}, //Dark2 0x126d
	{0x2b, 0x01, BYTE_BYTE_LEN, 0},
	{0x2c, 0x70, BYTE_BYTE_LEN, 0}, //Dark2 0x1270
	{0x2d, 0x00, BYTE_BYTE_LEN, 0}, 
	{0x2e, 0x71, BYTE_BYTE_LEN, 0}, //Dark2 0x1271
	{0x2f, 0xbf, BYTE_BYTE_LEN, 0},
	{0x30, 0x80, BYTE_BYTE_LEN, 0}, //Dark2 0x1280
	{0x31, 0x64, BYTE_BYTE_LEN, 0},
	{0x32, 0x81, BYTE_BYTE_LEN, 0}, //Dark2 0x1281
	{0x33, 0x80, BYTE_BYTE_LEN, 0},
	{0x34, 0x82, BYTE_BYTE_LEN, 0}, //Dark2 0x1282
	{0x35, 0x80, BYTE_BYTE_LEN, 0},
	{0x36, 0x83, BYTE_BYTE_LEN, 0}, //Dark2 0x1283
	{0x37, 0x02, BYTE_BYTE_LEN, 0},
	{0x38, 0x84, BYTE_BYTE_LEN, 0}, //Dark2 0x1284
	{0x39, 0x30, BYTE_BYTE_LEN, 0},
	{0x3a, 0x85, BYTE_BYTE_LEN, 0}, //Dark2 0x1285
	{0x3b, 0x80, BYTE_BYTE_LEN, 0},
	{0x3c, 0x86, BYTE_BYTE_LEN, 0}, //Dark2 0x1286
	{0x3d, 0x80, BYTE_BYTE_LEN, 0},
	{0x3e, 0x87, BYTE_BYTE_LEN, 0}, //Dark2 0x1287
	{0x3f, 0x01, BYTE_BYTE_LEN, 0},
	{0x40, 0x88, BYTE_BYTE_LEN, 0}, //Dark2 0x1288
	{0x41, 0x3a, BYTE_BYTE_LEN, 0},
	{0x42, 0x89, BYTE_BYTE_LEN, 0}, //Dark2 0x1289
	{0x43, 0x80, BYTE_BYTE_LEN, 0},
	{0x44, 0x8a, BYTE_BYTE_LEN, 0}, //Dark2 0x128a
	{0x45, 0x80, BYTE_BYTE_LEN, 0},
	{0x46, 0x8b, BYTE_BYTE_LEN, 0}, //Dark2 0x128b
	{0x47, 0x0c, BYTE_BYTE_LEN, 0},
	{0x48, 0x8c, BYTE_BYTE_LEN, 0}, //Dark2 0x128c
	{0x49, 0x05, BYTE_BYTE_LEN, 0},
	{0x4a, 0x8d, BYTE_BYTE_LEN, 0}, //Dark2 0x128d
	{0x4b, 0x03, BYTE_BYTE_LEN, 0},
	{0x4c, 0xe6, BYTE_BYTE_LEN, 0}, //Dark2 0x12e6
	{0x4d, 0xff, BYTE_BYTE_LEN, 0},
	{0x4e, 0xe7, BYTE_BYTE_LEN, 0}, //Dark2 0x12e7
	{0x4f, 0x18, BYTE_BYTE_LEN, 0},
	{0x50, 0xe8, BYTE_BYTE_LEN, 0}, //Dark2 0x12e8
	{0x51, 0x0a, BYTE_BYTE_LEN, 0},
	{0x52, 0xe9, BYTE_BYTE_LEN, 0}, //Dark2 0x12e9
	{0x53, 0x04, BYTE_BYTE_LEN, 0},
	{0x54, 0x03, BYTE_BYTE_LEN, 0},
	{0x55, 0x13, BYTE_BYTE_LEN, 0},//13 page
	{0x56, 0x10, BYTE_BYTE_LEN, 0}, //Dark2 0x1310
	{0x57, 0x3f, BYTE_BYTE_LEN, 0},
	{0x58, 0x20, BYTE_BYTE_LEN, 0}, //Dark2 0x1320
	{0x59, 0x20, BYTE_BYTE_LEN, 0},
	{0x5a, 0x21, BYTE_BYTE_LEN, 0}, //Dark2 0x1321
	{0x5b, 0x30, BYTE_BYTE_LEN, 0},
	{0x5c, 0x22, BYTE_BYTE_LEN, 0}, //Dark2 0x1322
	{0x5d, 0x36, BYTE_BYTE_LEN, 0},
	{0x5e, 0x23, BYTE_BYTE_LEN, 0}, //Dark2 0x1323
	{0x5f, 0x6a, BYTE_BYTE_LEN, 0},
	{0x60, 0x24, BYTE_BYTE_LEN, 0}, //Dark2 0x1324
	{0x61, 0xa0, BYTE_BYTE_LEN, 0},
	{0x62, 0x25, BYTE_BYTE_LEN, 0}, //Dark2 0x1325
	{0x63, 0xc0, BYTE_BYTE_LEN, 0},
	{0x64, 0x26, BYTE_BYTE_LEN, 0}, //Dark2 0x1326
	{0x65, 0xe0, BYTE_BYTE_LEN, 0},
	{0x66, 0x27, BYTE_BYTE_LEN, 0}, //Dark2 0x1327 lum 0
	{0x67, 0x04, BYTE_BYTE_LEN, 0},
	{0x68, 0x28, BYTE_BYTE_LEN, 0}, //Dark2 0x1328
	{0x69, 0x00, BYTE_BYTE_LEN, 0},
	{0x6a, 0x29, BYTE_BYTE_LEN, 0}, //Dark2 0x1329
	{0x6b, 0x00, BYTE_BYTE_LEN, 0},
	{0x6c, 0x2a, BYTE_BYTE_LEN, 0}, //Dark2 0x132a
	{0x6d, 0x00, BYTE_BYTE_LEN, 0},
	{0x6e, 0x2b, BYTE_BYTE_LEN, 0}, //Dark2 0x132b
	{0x6f, 0x00, BYTE_BYTE_LEN, 0},
	{0x70, 0x2c, BYTE_BYTE_LEN, 0}, //Dark2 0x132c
	{0x71, 0x00, BYTE_BYTE_LEN, 0},
	{0x72, 0x2d, BYTE_BYTE_LEN, 0}, //Dark2 0x132d
	{0x73, 0x00, BYTE_BYTE_LEN, 0},
	{0x74, 0x2e, BYTE_BYTE_LEN, 0}, //Dark2 0x132e lum7
	{0x75, 0x00, BYTE_BYTE_LEN, 0},
	{0x76, 0x2f, BYTE_BYTE_LEN, 0}, //Dark2 0x132f	   //weight skin
	{0x77, 0x00, BYTE_BYTE_LEN, 0},
	{0x78, 0x30, BYTE_BYTE_LEN, 0}, //Dark2 0x1330	   //weight blue
	{0x79, 0x00, BYTE_BYTE_LEN, 0},
	{0x7a, 0x31, BYTE_BYTE_LEN, 0}, //Dark2 0x1331	   //weight green
	{0x7b, 0x00, BYTE_BYTE_LEN, 0},
	{0x7c, 0x32, BYTE_BYTE_LEN, 0}, //Dark2 0x1332	   //weight strong color
	{0x7d, 0x00, BYTE_BYTE_LEN, 0},
	{0x7e, 0x33, BYTE_BYTE_LEN, 0}, //Dark2 0x1333
	{0x7f, 0x00, BYTE_BYTE_LEN, 0},
	{0x80, 0x34, BYTE_BYTE_LEN, 0}, //Dark2 0x1334
	{0x81, 0x00, BYTE_BYTE_LEN, 0},
	{0x82, 0x35, BYTE_BYTE_LEN, 0}, //Dark2 0x1335
	{0x83, 0x00, BYTE_BYTE_LEN, 0},
	{0x84, 0x36, BYTE_BYTE_LEN, 0}, //Dark2 0x1336
	{0x85, 0x00, BYTE_BYTE_LEN, 0},
	{0x86, 0xa0, BYTE_BYTE_LEN, 0}, //Dark2 0x13a0
	{0x87, 0x07, BYTE_BYTE_LEN, 0},
	{0x88, 0xa8, BYTE_BYTE_LEN, 0}, //Dark2 0x13a8	   //Dark2 Cb-filter 0x20
	{0x89, 0x30, BYTE_BYTE_LEN, 0},
	{0x8a, 0xa9, BYTE_BYTE_LEN, 0}, //Dark2 0x13a9	   //Dark2 Cr-filter 0x20
	{0x8b, 0x30, BYTE_BYTE_LEN, 0},
	{0x8c, 0xaa, BYTE_BYTE_LEN, 0}, //Dark2 0x13aa
	{0x8d, 0x30, BYTE_BYTE_LEN, 0},
	{0x8e, 0xab, BYTE_BYTE_LEN, 0}, //Dark2 0x13ab
	{0x8f, 0x02, BYTE_BYTE_LEN, 0},
	{0x90, 0xc0, BYTE_BYTE_LEN, 0}, //Dark2 0x13c0
	{0x91, 0x27, BYTE_BYTE_LEN, 0},
	{0x92, 0xc2, BYTE_BYTE_LEN, 0}, //Dark2 0x13c2
	{0x93, 0x08, BYTE_BYTE_LEN, 0},
	{0x94, 0xc3, BYTE_BYTE_LEN, 0}, //Dark2 0x13c3
	{0x95, 0x08, BYTE_BYTE_LEN, 0},
	{0x96, 0xc4, BYTE_BYTE_LEN, 0}, //Dark2 0x13c4
	{0x97, 0x46, BYTE_BYTE_LEN, 0},
	{0x98, 0xc5, BYTE_BYTE_LEN, 0}, //Dark2 0x13c5
	{0x99, 0x78, BYTE_BYTE_LEN, 0},
	{0x9a, 0xc6, BYTE_BYTE_LEN, 0}, //Dark2 0x13c6
	{0x9b, 0xf0, BYTE_BYTE_LEN, 0},
	{0x9c, 0xc7, BYTE_BYTE_LEN, 0}, //Dark2 0x13c7
	{0x9d, 0x10, BYTE_BYTE_LEN, 0},
	{0x9e, 0xc8, BYTE_BYTE_LEN, 0}, //Dark2 0x13c8
	{0x9f, 0x44, BYTE_BYTE_LEN, 0},
	{0xa0, 0xc9, BYTE_BYTE_LEN, 0}, //Dark2 0x13c9
	{0xa1, 0x87, BYTE_BYTE_LEN, 0},
	{0xa2, 0xca, BYTE_BYTE_LEN, 0}, //Dark2 0x13ca
	{0xa3, 0xff, BYTE_BYTE_LEN, 0},
	{0xa4, 0xcb, BYTE_BYTE_LEN, 0}, //Dark2 0x13cb
	{0xa5, 0x20, BYTE_BYTE_LEN, 0},
	{0xa6, 0xcc, BYTE_BYTE_LEN, 0}, //Dark2 0x13cc	   //skin range_cb_l
	{0xa7, 0x61, BYTE_BYTE_LEN, 0},
	{0xa8, 0xcd, BYTE_BYTE_LEN, 0}, //Dark2 0x13cd	   //skin range_cb_h
	{0xa9, 0x87, BYTE_BYTE_LEN, 0},
	{0xaa, 0xce, BYTE_BYTE_LEN, 0}, //Dark2 0x13ce	   //skin range_cr_l
	{0xab, 0x8a, BYTE_BYTE_LEN, 0},
	{0xac, 0xcf, BYTE_BYTE_LEN, 0}, //Dark2 0x13cf	   //skin range_cr_h
	{0xad, 0xa5, BYTE_BYTE_LEN, 0},
	{0xae, 0x03, BYTE_BYTE_LEN, 0}, //14 page
	{0xaf, 0x14, BYTE_BYTE_LEN, 0},
	{0xb0, 0x10, BYTE_BYTE_LEN, 0}, //Dark2 0x1410
	{0xb1, 0x00, BYTE_BYTE_LEN, 0},
	{0xb2, 0x11, BYTE_BYTE_LEN, 0}, //Dark2 0x1411
	{0xb3, 0x00, BYTE_BYTE_LEN, 0},
	{0xb4, 0x12, BYTE_BYTE_LEN, 0}, //Dark2 0x1412
	{0xb5, 0x40, BYTE_BYTE_LEN, 0}, //Top H_Clip
	{0xb6, 0x13, BYTE_BYTE_LEN, 0}, //Dark2 0x1413
	{0xb7, 0xc8, BYTE_BYTE_LEN, 0},
	{0xb8, 0x14, BYTE_BYTE_LEN, 0}, //Dark2 0x1414
	{0xb9, 0x50, BYTE_BYTE_LEN, 0},
	{0xba, 0x15, BYTE_BYTE_LEN, 0}, //Dark2 0x1415	   //sharp positive hi
	{0xbb, 0x10, BYTE_BYTE_LEN, 0},
	{0xbc, 0x16, BYTE_BYTE_LEN, 0}, //Dark2 0x1416	   //sharp positive mi
	{0xbd, 0x10, BYTE_BYTE_LEN, 0},
	{0xbe, 0x17, BYTE_BYTE_LEN, 0}, //Dark2 0x1417	   //sharp positive low
	{0xbf, 0x10, BYTE_BYTE_LEN, 0},
	{0xc0, 0x18, BYTE_BYTE_LEN, 0}, //Dark2 0x1418	   //sharp negative hi
	{0xc1, 0x28, BYTE_BYTE_LEN, 0},
	{0xc2, 0x19, BYTE_BYTE_LEN, 0}, //Dark2 0x1419	   //sharp negative mi
	{0xc3, 0x28, BYTE_BYTE_LEN, 0},
	{0xc4, 0x1a, BYTE_BYTE_LEN, 0}, //Dark2 0x141a	   //sharp negative low
	{0xc5, 0x28, BYTE_BYTE_LEN, 0},
	{0xc6, 0x20, BYTE_BYTE_LEN, 0}, //Dark2 0x1420
	{0xc7, 0x80, BYTE_BYTE_LEN, 0},
	{0xc8, 0x21, BYTE_BYTE_LEN, 0}, //Dark2 0x1421
	{0xc9, 0x03, BYTE_BYTE_LEN, 0},
	{0xca, 0x22, BYTE_BYTE_LEN, 0}, //Dark2 0x1422
	{0xcb, 0x05, BYTE_BYTE_LEN, 0},
	{0xcc, 0x23, BYTE_BYTE_LEN, 0}, //Dark2 0x1423
	{0xcd, 0x07, BYTE_BYTE_LEN, 0},
	{0xce, 0x24, BYTE_BYTE_LEN, 0}, //Dark2 0x1424
	{0xcf, 0x0a, BYTE_BYTE_LEN, 0},
	{0xd0, 0x25, BYTE_BYTE_LEN, 0}, //Dark2 0x1425
	{0xd1, 0x46, BYTE_BYTE_LEN, 0},
	{0xd2, 0x26, BYTE_BYTE_LEN, 0}, //Dark2 0x1426
	{0xd3, 0x32, BYTE_BYTE_LEN, 0},
	{0xd4, 0x27, BYTE_BYTE_LEN, 0}, //Dark2 0x1427
	{0xd5, 0x1e, BYTE_BYTE_LEN, 0},
	{0xd6, 0x28, BYTE_BYTE_LEN, 0}, //Dark2 0x1428
	{0xd7, 0x19, BYTE_BYTE_LEN, 0},
	{0xd8, 0x29, BYTE_BYTE_LEN, 0}, //Dark2 0x1429
	{0xd9, 0x00, BYTE_BYTE_LEN, 0},
	{0xda, 0x2a, BYTE_BYTE_LEN, 0}, //Dark2 0x142a
	{0xdb, 0x10, BYTE_BYTE_LEN, 0},
	{0xdc, 0x2b, BYTE_BYTE_LEN, 0}, //Dark2 0x142b
	{0xdd, 0x10, BYTE_BYTE_LEN, 0},
	{0xde, 0x2c, BYTE_BYTE_LEN, 0}, //Dark2 0x142c
	{0xdf, 0x10, BYTE_BYTE_LEN, 0},
	{0xe0, 0x2d, BYTE_BYTE_LEN, 0}, //Dark2 0x142d
	{0xe1, 0x80, BYTE_BYTE_LEN, 0},
	{0xe2, 0x2e, BYTE_BYTE_LEN, 0}, //Dark2 0x142e
	{0xe3, 0x80, BYTE_BYTE_LEN, 0},
	{0xe4, 0x2f, BYTE_BYTE_LEN, 0}, //Dark2 0x142f
	{0xe5, 0x80, BYTE_BYTE_LEN, 0},
	{0xe6, 0x30, BYTE_BYTE_LEN, 0}, //Dark2 0x1430
	{0xe7, 0x80, BYTE_BYTE_LEN, 0},
	{0xe8, 0x31, BYTE_BYTE_LEN, 0}, //Dark2 0x1431
	{0xe9, 0x02, BYTE_BYTE_LEN, 0},
	{0xea, 0x32, BYTE_BYTE_LEN, 0}, //Dark2 0x1432
	{0xeb, 0x04, BYTE_BYTE_LEN, 0},
	{0xec, 0x33, BYTE_BYTE_LEN, 0}, //Dark2 0x1433
	{0xed, 0x04, BYTE_BYTE_LEN, 0},
	{0xee, 0x34, BYTE_BYTE_LEN, 0}, //Dark2 0x1434
	{0xef, 0x0a, BYTE_BYTE_LEN, 0},
	{0xf0, 0x35, BYTE_BYTE_LEN, 0}, //Dark2 0x1435
	{0xf1, 0x46, BYTE_BYTE_LEN, 0},
	{0xf2, 0x36, BYTE_BYTE_LEN, 0}, //Dark2 0x1436
	{0xf3, 0x32, BYTE_BYTE_LEN, 0},
	{0xf4, 0x37, BYTE_BYTE_LEN, 0}, //Dark2 0x1437
	{0xf5, 0x28, BYTE_BYTE_LEN, 0},
	{0xf6, 0x38, BYTE_BYTE_LEN, 0}, //Dark2 0x1438
	{0xf7, 0x12, BYTE_BYTE_LEN, 0},//2d
	{0xf8, 0x39, BYTE_BYTE_LEN, 0}, //Dark2 0x1439
	{0xf9, 0x00, BYTE_BYTE_LEN, 0},//23
	{0xfa, 0x3a, BYTE_BYTE_LEN, 0}, //Dark2 0x143a
	{0xfb, 0x18, BYTE_BYTE_LEN, 0}, //dr gain
	{0xfc, 0x3b, BYTE_BYTE_LEN, 0}, //Dark2 0x143b
	{0xfd, 0x20, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xe5, BYTE_BYTE_LEN, 0},
	{0x10, 0x3c, BYTE_BYTE_LEN, 0}, //Dark2 0x143c
	{0x11, 0x18, BYTE_BYTE_LEN, 0},
	{0x12, 0x3d, BYTE_BYTE_LEN, 0}, //Dark2 0x143d
	{0x13, 0x20, BYTE_BYTE_LEN, 0}, //nor gain
	{0x14, 0x3e, BYTE_BYTE_LEN, 0}, //Dark2 0x143e
	{0x15, 0x22, BYTE_BYTE_LEN, 0},
	{0x16, 0x3f, BYTE_BYTE_LEN, 0}, //Dark2 0x143f
	{0x17, 0x10, BYTE_BYTE_LEN, 0},
	{0x18, 0x40, BYTE_BYTE_LEN, 0}, //Dark2 0x1440
	{0x19, 0x80, BYTE_BYTE_LEN, 0},
	{0x1a, 0x41, BYTE_BYTE_LEN, 0}, //Dark2 0x1441
	{0x1b, 0x12, BYTE_BYTE_LEN, 0},
	{0x1c, 0x42, BYTE_BYTE_LEN, 0}, //Dark2 0x1442
	{0x1d, 0xb0, BYTE_BYTE_LEN, 0},
	{0x1e, 0x43, BYTE_BYTE_LEN, 0}, //Dark2 0x1443
	{0x1f, 0x20, BYTE_BYTE_LEN, 0},
	{0x20, 0x44, BYTE_BYTE_LEN, 0}, //Dark2 0x1444
	{0x21, 0x20, BYTE_BYTE_LEN, 0},
	{0x22, 0x45, BYTE_BYTE_LEN, 0}, //Dark2 0x1445
	{0x23, 0x20, BYTE_BYTE_LEN, 0},
	{0x24, 0x46, BYTE_BYTE_LEN, 0}, //Dark2 0x1446
	{0x25, 0x20, BYTE_BYTE_LEN, 0},
	{0x26, 0x47, BYTE_BYTE_LEN, 0}, //Dark2 0x1447
	{0x27, 0x08, BYTE_BYTE_LEN, 0},
	{0x28, 0x48, BYTE_BYTE_LEN, 0}, //Dark2 0x1448
	{0x29, 0x08, BYTE_BYTE_LEN, 0},
	{0x2a, 0x49, BYTE_BYTE_LEN, 0}, //Dark2 0x1449
	{0x2b, 0x08, BYTE_BYTE_LEN, 0},
	{0x2c, 0x50, BYTE_BYTE_LEN, 0}, //Dark2 0x1450
	{0x2d, 0x80, BYTE_BYTE_LEN, 0},
	{0x2e, 0x51, BYTE_BYTE_LEN, 0}, //Dark2 0x1451
	{0x2f, 0x32, BYTE_BYTE_LEN, 0}, //
	{0x30, 0x52, BYTE_BYTE_LEN, 0}, //Dark2 0x1452
	{0x31, 0x40, BYTE_BYTE_LEN, 0},
	{0x32, 0x53, BYTE_BYTE_LEN, 0}, //Dark2 0x1453
	{0x33, 0x19, BYTE_BYTE_LEN, 0},
	{0x34, 0x54, BYTE_BYTE_LEN, 0}, //Dark2 0x1454
	{0x35, 0x60, BYTE_BYTE_LEN, 0},
	{0x36, 0x55, BYTE_BYTE_LEN, 0}, //Dark2 0x1455
	{0x37, 0x60, BYTE_BYTE_LEN, 0},
	{0x38, 0x56, BYTE_BYTE_LEN, 0}, //Dark2 0x1456
	{0x39, 0x60, BYTE_BYTE_LEN, 0},
	{0x3a, 0x57, BYTE_BYTE_LEN, 0}, //Dark2 0x1457
	{0x3b, 0x20, BYTE_BYTE_LEN, 0},
	{0x3c, 0x58, BYTE_BYTE_LEN, 0}, //Dark2 0x1458
	{0x3d, 0x20, BYTE_BYTE_LEN, 0},
	{0x3e, 0x59, BYTE_BYTE_LEN, 0}, //Dark2 0x1459
	{0x3f, 0x20, BYTE_BYTE_LEN, 0},
	{0x40, 0x60, BYTE_BYTE_LEN, 0}, //Dark2 0x1460
	{0x41, 0x03, BYTE_BYTE_LEN, 0}, //skin opt en
	{0x42, 0x61, BYTE_BYTE_LEN, 0}, //Dark2 0x1461
	{0x43, 0xa0, BYTE_BYTE_LEN, 0},
	{0x44, 0x62, BYTE_BYTE_LEN, 0}, //Dark2 0x1462
	{0x45, 0x98, BYTE_BYTE_LEN, 0},
	{0x46, 0x63, BYTE_BYTE_LEN, 0}, //Dark2 0x1463
	{0x47, 0xe4, BYTE_BYTE_LEN, 0}, //skin_std_th_h
	{0x48, 0x64, BYTE_BYTE_LEN, 0}, //Dark2 0x1464
	{0x49, 0xa4, BYTE_BYTE_LEN, 0}, //skin_std_th_l
	{0x4a, 0x65, BYTE_BYTE_LEN, 0}, //Dark2 0x1465
	{0x4b, 0x7d, BYTE_BYTE_LEN, 0}, //sharp_std_th_h
	{0x4c, 0x66, BYTE_BYTE_LEN, 0}, //Dark2 0x1466
	{0x4d, 0x4b, BYTE_BYTE_LEN, 0}, //sharp_std_th_l
	{0x4e, 0x70, BYTE_BYTE_LEN, 0}, //Dark2 0x1470
	{0x4f, 0x10, BYTE_BYTE_LEN, 0},
	{0x50, 0x71, BYTE_BYTE_LEN, 0}, //Dark2 0x1471
	{0x51, 0x10, BYTE_BYTE_LEN, 0},
	{0x52, 0x72, BYTE_BYTE_LEN, 0}, //Dark2 0x1472
	{0x53, 0x10, BYTE_BYTE_LEN, 0},
	{0x54, 0x73, BYTE_BYTE_LEN, 0}, //Dark2 0x1473
	{0x55, 0x10, BYTE_BYTE_LEN, 0},
	{0x56, 0x74, BYTE_BYTE_LEN, 0}, //Dark2 0x1474
	{0x57, 0x10, BYTE_BYTE_LEN, 0},
	{0x58, 0x75, BYTE_BYTE_LEN, 0}, //Dark2 0x1475
	{0x59, 0x10, BYTE_BYTE_LEN, 0},
	{0x5a, 0x76, BYTE_BYTE_LEN, 0}, //Dark2 0x1476
	{0x5b, 0x28, BYTE_BYTE_LEN, 0},
	{0x5c, 0x77, BYTE_BYTE_LEN, 0}, //Dark2 0x1477
	{0x5d, 0x28, BYTE_BYTE_LEN, 0},
	{0x5e, 0x78, BYTE_BYTE_LEN, 0}, //Dark2 0x1478
	{0x5f, 0x28, BYTE_BYTE_LEN, 0},
	{0x60, 0x79, BYTE_BYTE_LEN, 0}, //Dark2 0x1479
	{0x61, 0x28, BYTE_BYTE_LEN, 0},
	{0x62, 0x7a, BYTE_BYTE_LEN, 0}, //Dark2 0x147a
	{0x63, 0x28, BYTE_BYTE_LEN, 0},
	{0x64, 0x7b, BYTE_BYTE_LEN, 0}, //Dark2 0x147b
	{0x65, 0x28, BYTE_BYTE_LEN, 0},


	
	// DMA END
	
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x01, 0xF1, BYTE_BYTE_LEN, 0}, //Sleep mode on
	
	{0x03, 0xc0, BYTE_BYTE_LEN, 0},
	{0x16, 0x80, BYTE_BYTE_LEN, 0}, //MCU main roof holding off
	
	{0x03, 0xC0, BYTE_BYTE_LEN, 0},
	{0x33, 0x01, BYTE_BYTE_LEN, 0}, //DMA hand shake mode set
	{0x32, 0x01, BYTE_BYTE_LEN, 0}, //DMA off
	{0x03, 0x30, BYTE_BYTE_LEN, 0},
	{0x11, 0x04, BYTE_BYTE_LEN, 0}, //Bit[0]: MCU hold off
	
	{0x03, 0xc0, BYTE_BYTE_LEN, 0},
	{0xe1, 0x00, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x30, BYTE_BYTE_LEN, 0},
	{0x25, 0x0e, BYTE_BYTE_LEN, 0},
	{0x25, 0x1e, BYTE_BYTE_LEN, 0},
	///////////////////////////////////////////
	// 1F Page SSD
	///////////////////////////////////////////
	{0x03, 0x1f, BYTE_BYTE_LEN, 0}, //1F page
	{0x11, 0x00, BYTE_BYTE_LEN, 0}, //bit[5:4]: debug mode
	{0x12, 0x60, BYTE_BYTE_LEN, 0}, 
	{0x13, 0x14, BYTE_BYTE_LEN, 0}, 
	{0x14, 0x10, BYTE_BYTE_LEN, 0}, 
	{0x15, 0x00, BYTE_BYTE_LEN, 0}, 
	{0x20, 0x18, BYTE_BYTE_LEN, 0}, //ssd_x_start_pos
	{0x21, 0x14, BYTE_BYTE_LEN, 0}, //ssd_y_start_pos
	{0x22, 0x8C, BYTE_BYTE_LEN, 0}, //ssd_blk_width
	{0x23, 0x9C, BYTE_BYTE_LEN, 0}, //ssd_blk_height
	{0x28, 0x18, BYTE_BYTE_LEN, 0},
	{0x29, 0x02, BYTE_BYTE_LEN, 0},
	{0x3B, 0x18, BYTE_BYTE_LEN, 0},
	{0x3C, 0x8C, BYTE_BYTE_LEN, 0},
	{0x10, 0x19, BYTE_BYTE_LEN, 0}, //SSD enable
	
	{0x03, 0xc4, BYTE_BYTE_LEN, 0}, //AE en
	{0x10, 0xeb, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xc3, BYTE_BYTE_LEN, 0}, //AE Static en
	{0x10, 0x84, BYTE_BYTE_LEN, 0},
	
	///////////////////////////////////////////
	// 30 Page DMA address set
	///////////////////////////////////////////
	{0x03, 0x30, BYTE_BYTE_LEN, 0}, //DMA
	{0x7c, 0x2c, BYTE_BYTE_LEN, 0}, //Extra str
	{0x7d, 0xce, BYTE_BYTE_LEN, 0},
	{0x7e, 0x2d, BYTE_BYTE_LEN, 0}, //Extra end
	{0x7f, 0xbb, BYTE_BYTE_LEN, 0},
	{0x80, 0x24, BYTE_BYTE_LEN, 0}, //Outdoor str
	{0x81, 0x70, BYTE_BYTE_LEN, 0},
	{0x82, 0x27, BYTE_BYTE_LEN, 0}, //Outdoor end
	{0x83, 0x39, BYTE_BYTE_LEN, 0},
	{0x84, 0x21, BYTE_BYTE_LEN, 0}, //Indoor str
	{0x85, 0xa6, BYTE_BYTE_LEN, 0},
	{0x86, 0x24, BYTE_BYTE_LEN, 0}, //Indoor end
	{0x87, 0x6f, BYTE_BYTE_LEN, 0},
	{0x88, 0x27, BYTE_BYTE_LEN, 0}, //Dark1 str
	{0x89, 0x3a, BYTE_BYTE_LEN, 0},
	{0x8a, 0x2a, BYTE_BYTE_LEN, 0}, //Dark1 end
	{0x8b, 0x03, BYTE_BYTE_LEN, 0},
	{0x8c, 0x2a, BYTE_BYTE_LEN, 0}, //Dark2 str
	{0x8d, 0x04, BYTE_BYTE_LEN, 0},
	{0x8e, 0x2c, BYTE_BYTE_LEN, 0}, //Dark2 end
	{0x8f, 0xcd, BYTE_BYTE_LEN, 0},
	
	
	
	///////////////////////////////////////////
	// CD Page (Color ratio)
	///////////////////////////////////////////
	{0x03, 0xCD, BYTE_BYTE_LEN, 0},
	{0x10, 0x38, BYTE_BYTE_LEN, 0}, //ColorRatio disable 시킴
	
	
	{0x03, 0xc9, BYTE_BYTE_LEN, 0}, //AWB Start Point
	{0x2a, 0x00, BYTE_BYTE_LEN, 0},
	{0x2b, 0xb2, BYTE_BYTE_LEN, 0},
	{0x2c, 0x00, BYTE_BYTE_LEN, 0},
	{0x2d, 0x82, BYTE_BYTE_LEN, 0},
	{0x2e, 0x00, BYTE_BYTE_LEN, 0},
	{0x2f, 0xb2, BYTE_BYTE_LEN, 0},
	{0x30, 0x00, BYTE_BYTE_LEN, 0},
	{0x31, 0x82, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xc5, BYTE_BYTE_LEN, 0}, //AWB en
	{0x10, 0xb1, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xcf, BYTE_BYTE_LEN, 0}, //Adative en
	{0x10, 0xaf, BYTE_BYTE_LEN, 0}, //yoffset off//0xef, BYTE_BYTE_LEN, 0}, //0220 yoffon//0xaf, BYTE_BYTE_LEN, 0}, //jk0130//0xaf, BYTE_BYTE_LEN, 0}, // STEVE 8f -) af ON :Ytar, Gam, CCM, Sat, LSC, MCMC / OFF: Yoffs, Contrast
	
	///////////////////////////////////////////
	// 48 Page MIPI setting
	///////////////////////////////////////////
	{0x03, 0x48, BYTE_BYTE_LEN, 0},
	{0x09, 0xa2, BYTE_BYTE_LEN, 0}, //MIPI CLK
	{0x10, 0x1C, BYTE_BYTE_LEN, 0}, //MIPI ON
	{0x11, 0x00, BYTE_BYTE_LEN, 0}, //Normal Mode //[4] '1' contenous, '0'non-contenous
	{0x14, 0x50, BYTE_BYTE_LEN, 0}, //Skew
	{0x16, 0x04, BYTE_BYTE_LEN, 0}, 
	
	{0x1a, 0x11, BYTE_BYTE_LEN, 0},
	{0x1b, 0x0d, BYTE_BYTE_LEN, 0}, //Short Packet
	{0x1c, 0x0a, BYTE_BYTE_LEN, 0}, //Control DP
	{0x1d, 0x0f, BYTE_BYTE_LEN, 0}, //Control DN
	{0x1e, 0x09, BYTE_BYTE_LEN, 0},
	{0x1f, 0x04, BYTE_BYTE_LEN, 0}, // 20120305 LSW 0x07, BYTE_BYTE_LEN, 0},
	{0x20, 0x00, BYTE_BYTE_LEN, 0},
	{0x24, 0x1e, BYTE_BYTE_LEN, 0}, //Bayer8 : 2a, Bayer10 : 2b, YUV : 1e
	
	{0x30, 0x00, BYTE_BYTE_LEN, 0}, //2048*2
	{0x31, 0x05, BYTE_BYTE_LEN, 0},
	{0x32, 0x0f, BYTE_BYTE_LEN, 0}, // Tclk zero
	{0x34, 0x03, BYTE_BYTE_LEN, 0}, //120302 jk//0x06}, // Tclk prepare
	{0x35, 0x04, BYTE_BYTE_LEN, 0}, //20120305 LSW 0x06(default)
	
	{0x39, 0x03, BYTE_BYTE_LEN, 0}, //Drivability 00
	
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x0c, 0xf0, BYTE_BYTE_LEN, 0}, //Parallel Line Off
	
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x11, 0x83, BYTE_BYTE_LEN, 0}, // STEVE 0frame skip, XY flip 
	{0x01, 0xf0, BYTE_BYTE_LEN, 0}, //sleep off
	
	{0x03, 0xC0, BYTE_BYTE_LEN, 0},
	{0x33, 0x00, BYTE_BYTE_LEN, 0},
	{0x32, 0x01, BYTE_BYTE_LEN, 0}, //DMA on
	
	//////////////////////////////////////////////
	// Delay
	//////////////////////////////////////////////
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},

};




static const struct hi351_i2c_reg_conf const cam_set_wb_tbl_daylight[] = {
//CAMERA_WB_DAYLIGHT          //5
  {0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0x30, BYTE_BYTE_LEN, 0}, //AWB Off
	{0x03, 0xc6, BYTE_BYTE_LEN, 0},
	{0x18, 0x66, BYTE_BYTE_LEN, 0}, //bInRgainMin_a00_n00 
	{0x19, 0x7e, BYTE_BYTE_LEN, 0}, //bInRgainMax_a00_n00 

	{0x1a, 0x4c, BYTE_BYTE_LEN, 0}, //bInBgainMin_a00_n00 
	{0x1b, 0x66, BYTE_BYTE_LEN, 0}, //bInBgainMax_a00_n00 
	
	{0xb9, 0x6a, BYTE_BYTE_LEN, 0}, //bOutRgainMin_a00_n00  
	{0xba, 0x7e, BYTE_BYTE_LEN, 0}, //bOutRgainMax_a00_n00  
			
	{0xbb, 0x4c, BYTE_BYTE_LEN, 0}, //bOutBgainMin_a00_n00  
	{0xbc, 0x60, BYTE_BYTE_LEN, 0}, //bOutBgainMax_a00_n00  
	{0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0xb1, BYTE_BYTE_LEN, 0}, //AWB On
};

static const struct hi351_i2c_reg_conf const cam_set_wb_tbl_auto[] = {
//CAMERA_WB_AUTO                //1
  {0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0x30, BYTE_BYTE_LEN, 0}, //AWB Off
	{0x03, 0xc6, BYTE_BYTE_LEN, 0},
	{0x18, 0x40, BYTE_BYTE_LEN, 0}, //bInRgainMin_a00_n00 
	{0x19, 0xf0, BYTE_BYTE_LEN, 0}, //bInRgainMax_a00_n00 
	{0x1a, 0x40, BYTE_BYTE_LEN, 0}, //bInBgainMin_a00_n00 
	{0x1b, 0xf0, BYTE_BYTE_LEN, 0}, //bInBgainMax_a00_n00 
	{0xb9, 0x40, BYTE_BYTE_LEN, 0}, //bOutRgainMin_a00_n00	
	{0xba, 0xf0, BYTE_BYTE_LEN, 0}, //bOutRgainMax_a00_n00	
	{0xbb, 0x40, BYTE_BYTE_LEN, 0}, //bOutBgainMin_a00_n00	
	{0xbc, 0xf0, BYTE_BYTE_LEN, 0}, //bOutBgainMax_a00_n00	
	{0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0xb1, BYTE_BYTE_LEN, 0}, //AWB On
};

static const struct hi351_i2c_reg_conf const cam_set_wb_tbl_cloudy[] = {
//CAMERA_WB_CLOUDY_DAYLIGHT   //6
  {0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0x30, BYTE_BYTE_LEN, 0},
	{0x03, 0xc6, BYTE_BYTE_LEN, 0},
	{0x18, 0x78, BYTE_BYTE_LEN, 0}, //bInRgainMin_a00_n00 
	{0x19, 0x8C, BYTE_BYTE_LEN, 0}, //bInRgainMax_a00_n00 

	{0x1a, 0x40, BYTE_BYTE_LEN, 0}, //bInBgainMin_a00_n00 
	{0x1b, 0x50, BYTE_BYTE_LEN, 0}, //bInBgainMax_a00_n00 
	
	{0xb9, 0x78, BYTE_BYTE_LEN, 0}, //bOutRgainMin_a00_n00  
	{0xba, 0x8C, BYTE_BYTE_LEN, 0}, //bOutRgainMax_a00_n00  
			
	{0xbb, 0x40, BYTE_BYTE_LEN, 0}, //bOutBgainMin_a00_n00  
	{0xbc, 0x50, BYTE_BYTE_LEN, 0}, //bOutBgainMax_a00_n00  
	{0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0xb1, BYTE_BYTE_LEN, 0},
};

static const struct hi351_i2c_reg_conf const cam_set_wb_tbl_flourescent[] = {
//CAMERA_WB_FLUORESCENT       //4
{0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0x30, BYTE_BYTE_LEN, 0}, //AWB Off
	{0x03, 0xc6, BYTE_BYTE_LEN, 0},
	{0x18, 0x49, BYTE_BYTE_LEN, 0}, //bInRgainMin_a00_n00 
	{0x19, 0x5d, BYTE_BYTE_LEN, 0}, //bInRgainMax_a00_n00 

	{0x1a, 0x6c, BYTE_BYTE_LEN, 0}, //bInBgainMin_a00_n00 
	{0x1b, 0x80, BYTE_BYTE_LEN, 0}, //bInBgainMax_a00_n00 
	
	{0xb9, 0x49, BYTE_BYTE_LEN, 0}, //bOutRgainMin_a00_n00  
	{0xba, 0x5d, BYTE_BYTE_LEN, 0}, //bOutRgainMax_a00_n00  
			
	{0xbb, 0x6c, BYTE_BYTE_LEN, 0}, //bOutBgainMin_a00_n00  
	{0xbc, 0x80, BYTE_BYTE_LEN, 0}, //bOutBgainMax_a00_n00  
	{0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0xb1, BYTE_BYTE_LEN, 0}, //AWB On
};

static const struct hi351_i2c_reg_conf const cam_set_wb_tbl_incandescent[] = {
//CAMERA_WB_INCANDESCENT       //3
  {0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0x30, BYTE_BYTE_LEN, 0}, //AWB Off
	{0x03, 0xc6, BYTE_BYTE_LEN, 0},
	{0x18, 0x46, BYTE_BYTE_LEN, 0}, //bInRgainMin_a00_n00 
	{0x19, 0x5a, BYTE_BYTE_LEN, 0}, //bInRgainMax_a00_n00 
			
	{0x1a, 0x78, BYTE_BYTE_LEN, 0}, //bInBgainMin_a00_n00 
	{0x1b, 0x8c, BYTE_BYTE_LEN, 0}, //bInBgainMax_a00_n00 
	
	{0xb9, 0x46, BYTE_BYTE_LEN, 0}, //bOutRgainMin_a00_n00  
	{0xba, 0x5a, BYTE_BYTE_LEN, 0}, //bOutRgainMax_a00_n00  
			
	{0xbb, 0x78, BYTE_BYTE_LEN, 0}, //bOutBgainMin_a00_n00  
	{0xbc, 0x8c, BYTE_BYTE_LEN, 0}, //bOutBgainMax_a00_n00  
	{0x03, 0xc5, BYTE_BYTE_LEN, 0},
	{0x10, 0xb1, BYTE_BYTE_LEN, 0}, //AWB On
};
/* -------- end of wb   ------------------*/

/* -------- start of iso ------------------ */
static const struct hi351_i2c_reg_conf const cam_set_iso_tbl_auto[] = {
//ISO Auto
{ 0x03, 0xD5, BYTE_BYTE_LEN, 0},				  
{ 0x10, 0x06, BYTE_BYTE_LEN, 0}, // ISO Mode Select B[6:4] 000 Auto,001 ISO1,010 ISO2,011 ISO3,100 ISO4,101 ISO5,110 ISO6
};
static const struct hi351_i2c_reg_conf const cam_set_iso_tbl_100[] = {
//ISO 100
{ 0x03, 0xD5, BYTE_BYTE_LEN, 0},				  
{ 0x10, 0x16, BYTE_BYTE_LEN, 0}, // ISO 1 control 
};
static const struct hi351_i2c_reg_conf const cam_set_iso_tbl_200[] = {
//ISO 200
{ 0x03, 0xD5, BYTE_BYTE_LEN, 0},				  	
{ 0x10, 0x26, BYTE_BYTE_LEN, 0}, // ISO 2 control 
};
static const struct hi351_i2c_reg_conf const cam_set_iso_tbl_400[] = {
//ISO 400
{ 0x03, 0xD5, BYTE_BYTE_LEN, 0},				  
{ 0x10, 0x36, BYTE_BYTE_LEN, 0}, // ISO 3 control
};
static const struct hi351_i2c_reg_conf const cam_set_iso_tbl_800[] = {
};
/* -------- end of iso -------------------*/

/* -------- start of exposure  ------------------ */
static const struct hi351_i2c_reg_conf const cam_set_exposure_tbl_averg[] = {
//AE OFF
{0x03, 0xc4, BYTE_BYTE_LEN, 0},
{0x10, 0x60, BYTE_BYTE_LEN, 0},

//AWB OFF
{0x03, 0xc5, BYTE_BYTE_LEN, 0},
{0x10, 0x30, BYTE_BYTE_LEN, 0},

{0x03, 0xc3, BYTE_BYTE_LEN, 0},
{0x10, 0x80, BYTE_BYTE_LEN, 0},
{0x3A, 0x11, BYTE_BYTE_LEN, 0},
{0x3B, 0x11, BYTE_BYTE_LEN, 0},
{0x3C, 0x11, BYTE_BYTE_LEN, 0},
{0x3D, 0x11, BYTE_BYTE_LEN, 0},
{0x3E, 0x11, BYTE_BYTE_LEN, 0},
{0x3F, 0x11, BYTE_BYTE_LEN, 0},
{0x40, 0x11, BYTE_BYTE_LEN, 0},
{0x41, 0x11, BYTE_BYTE_LEN, 0},
{0x42, 0x11, BYTE_BYTE_LEN, 0},
{0x43, 0x11, BYTE_BYTE_LEN, 0},
{0x44, 0x11, BYTE_BYTE_LEN, 0},
{0x45, 0x11, BYTE_BYTE_LEN, 0},
{0x46, 0x11, BYTE_BYTE_LEN, 0},
{0x47, 0x11, BYTE_BYTE_LEN, 0},
{0x48, 0x11, BYTE_BYTE_LEN, 0},
{0x49, 0x11, BYTE_BYTE_LEN, 0},
{0x4A, 0x11, BYTE_BYTE_LEN, 0},
{0x4B, 0x11, BYTE_BYTE_LEN, 0},
{0x4C, 0x11, BYTE_BYTE_LEN, 0},
{0x4D, 0x11, BYTE_BYTE_LEN, 0},
{0x4E, 0x11, BYTE_BYTE_LEN, 0},
{0x4F, 0x11, BYTE_BYTE_LEN, 0},
{0x50, 0x11, BYTE_BYTE_LEN, 0},
{0x51, 0x11, BYTE_BYTE_LEN, 0},
{0x52, 0x11, BYTE_BYTE_LEN, 0},
{0x53, 0x11, BYTE_BYTE_LEN, 0},
{0x54, 0x11, BYTE_BYTE_LEN, 0},
{0x55, 0x11, BYTE_BYTE_LEN, 0},
{0x56, 0x11, BYTE_BYTE_LEN, 0},
{0x57, 0x11, BYTE_BYTE_LEN, 0},
{0x58, 0x11, BYTE_BYTE_LEN, 0},
{0x59, 0x11, BYTE_BYTE_LEN, 0},
{0x5A, 0x11, BYTE_BYTE_LEN, 0},
{0x5B, 0x11, BYTE_BYTE_LEN, 0},
{0x5C, 0x11, BYTE_BYTE_LEN, 0},
{0x5D, 0x11, BYTE_BYTE_LEN, 0},
{0x5E, 0x11, BYTE_BYTE_LEN, 0},
{0x5F, 0x11, BYTE_BYTE_LEN, 0},
{0x60, 0x11, BYTE_BYTE_LEN, 0},
{0x61, 0x11, BYTE_BYTE_LEN, 0},
{0x62, 0x11, BYTE_BYTE_LEN, 0},
{0x63, 0x11, BYTE_BYTE_LEN, 0},
{0x64, 0x11, BYTE_BYTE_LEN, 0},
{0x65, 0x11, BYTE_BYTE_LEN, 0},
{0x66, 0x11, BYTE_BYTE_LEN, 0},
{0x67, 0x11, BYTE_BYTE_LEN, 0},
{0x68, 0x11, BYTE_BYTE_LEN, 0},
{0x69, 0x11, BYTE_BYTE_LEN, 0},
{0x6A, 0x11, BYTE_BYTE_LEN, 0},
{0x6B, 0x11, BYTE_BYTE_LEN, 0},
{0x6C, 0x11, BYTE_BYTE_LEN, 0},
{0x6D, 0x11, BYTE_BYTE_LEN, 0},
{0x6E, 0x11, BYTE_BYTE_LEN, 0},
{0x6F, 0x11, BYTE_BYTE_LEN, 0},
{0x10, 0x84, BYTE_BYTE_LEN, 0},

//AE On
{0x03, 0xc4, BYTE_BYTE_LEN, 0},
{0x10, 0xe2, BYTE_BYTE_LEN, 10},
 //Delay 10ms
	
//AWB On
{0x03, 0xc5, BYTE_BYTE_LEN, 0},
{0x10, 0xb1, BYTE_BYTE_LEN, 0},
};
static const struct hi351_i2c_reg_conf const cam_set_exposure_tbl_center[] = {
//AE OFF
{0x03, 0xc4, BYTE_BYTE_LEN, 0},
{0x10, 0x60, BYTE_BYTE_LEN, 0},

//AWB OFF
{0x03, 0xc5, BYTE_BYTE_LEN, 0},
{0x10, 0x30, BYTE_BYTE_LEN, 0},

{0x03, 0xc3, BYTE_BYTE_LEN, 0},
{0x10, 0x80, BYTE_BYTE_LEN, 0},
{0x3A, 0x11, BYTE_BYTE_LEN, 0},
{0x3B, 0x11, BYTE_BYTE_LEN, 0},
{0x3C, 0x11, BYTE_BYTE_LEN, 0},
{0x3D, 0x11, BYTE_BYTE_LEN, 0},
{0x3E, 0x11, BYTE_BYTE_LEN, 0},
{0x3F, 0x11, BYTE_BYTE_LEN, 0},
{0x40, 0x11, BYTE_BYTE_LEN, 0},
{0x41, 0x11, BYTE_BYTE_LEN, 0},
{0x42, 0x11, BYTE_BYTE_LEN, 0},
{0x43, 0x11, BYTE_BYTE_LEN, 0},
{0x44, 0x11, BYTE_BYTE_LEN, 0},
{0x45, 0x11, BYTE_BYTE_LEN, 0},
{0x46, 0x11, BYTE_BYTE_LEN, 0},
{0x47, 0x11, BYTE_BYTE_LEN, 0},
{0x48, 0x11, BYTE_BYTE_LEN, 0},
{0x49, 0x11, BYTE_BYTE_LEN, 0},
{0x4A, 0x11, BYTE_BYTE_LEN, 0},
{0x4B, 0x11, BYTE_BYTE_LEN, 0},
{0x4C, 0x11, BYTE_BYTE_LEN, 0},
{0x4D, 0x31, BYTE_BYTE_LEN, 0},
{0x4E, 0x77, BYTE_BYTE_LEN, 0},
{0x4F, 0x77, BYTE_BYTE_LEN, 0},
{0x50, 0x13, BYTE_BYTE_LEN, 0},
{0x51, 0x11, BYTE_BYTE_LEN, 0},
{0x52, 0x11, BYTE_BYTE_LEN, 0},
{0x53, 0x32, BYTE_BYTE_LEN, 0},
{0x54, 0x77, BYTE_BYTE_LEN, 0},
{0x55, 0x77, BYTE_BYTE_LEN, 0},
{0x56, 0x23, BYTE_BYTE_LEN, 0},
{0x57, 0x11, BYTE_BYTE_LEN, 0},
{0x58, 0x21, BYTE_BYTE_LEN, 0},
{0x59, 0x66, BYTE_BYTE_LEN, 0},
{0x5A, 0x76, BYTE_BYTE_LEN, 0},
{0x5B, 0x67, BYTE_BYTE_LEN, 0},
{0x5C, 0x66, BYTE_BYTE_LEN, 0},
{0x5D, 0x12, BYTE_BYTE_LEN, 0},
{0x5E, 0x21, BYTE_BYTE_LEN, 0},
{0x5F, 0x66, BYTE_BYTE_LEN, 0},
{0x60, 0x76, BYTE_BYTE_LEN, 0},
{0x61, 0x67, BYTE_BYTE_LEN, 0},
{0x62, 0x66, BYTE_BYTE_LEN, 0},
{0x63, 0x12, BYTE_BYTE_LEN, 0},
{0x64, 0x21, BYTE_BYTE_LEN, 0},
{0x65, 0x55, BYTE_BYTE_LEN, 0},
{0x66, 0x55, BYTE_BYTE_LEN, 0},
{0x67, 0x55, BYTE_BYTE_LEN, 0},
{0x68, 0x55, BYTE_BYTE_LEN, 0},
{0x69, 0x12, BYTE_BYTE_LEN, 0},
{0x6A, 0x11, BYTE_BYTE_LEN, 0},
{0x6B, 0x11, BYTE_BYTE_LEN, 0},
{0x6C, 0x11, BYTE_BYTE_LEN, 0},
{0x6D, 0x11, BYTE_BYTE_LEN, 0},
{0x6E, 0x11, BYTE_BYTE_LEN, 0},
{0x6F, 0x11, BYTE_BYTE_LEN, 0},
{0x10, 0x84, BYTE_BYTE_LEN, 0},

//AE On
{0x03, 0xc4, BYTE_BYTE_LEN, 0},
{0x10, 0xe2, BYTE_BYTE_LEN, 10},
 //Delay 10ms
	
//AWB On
{0x03, 0xc5, BYTE_BYTE_LEN, 0},
{0x10, 0xb1, BYTE_BYTE_LEN, 0},
};
static const struct hi351_i2c_reg_conf const cam_set_exposure_tbl_spot[] = {
//AE OFF
{0x03, 0xc4, BYTE_BYTE_LEN, 0},
{0x10, 0x60, BYTE_BYTE_LEN, 0},

//AWB OFF
{0x03, 0xc5, BYTE_BYTE_LEN, 0},
{0x10, 0x30, BYTE_BYTE_LEN, 0},

{0x03, 0xc3, BYTE_BYTE_LEN, 0},
{0x10, 0x80, BYTE_BYTE_LEN, 0},
{0x3A, 0x00, BYTE_BYTE_LEN, 0},
{0x3B, 0x00, BYTE_BYTE_LEN, 0},
{0x3C, 0x00, BYTE_BYTE_LEN, 0},
{0x3D, 0x00, BYTE_BYTE_LEN, 0},
{0x3E, 0x00, BYTE_BYTE_LEN, 0},
{0x3F, 0x00, BYTE_BYTE_LEN, 0},
{0x40, 0x00, BYTE_BYTE_LEN, 0},
{0x41, 0x00, BYTE_BYTE_LEN, 0},
{0x42, 0x00, BYTE_BYTE_LEN, 0},
{0x43, 0x00, BYTE_BYTE_LEN, 0},
{0x44, 0x00, BYTE_BYTE_LEN, 0},
{0x45, 0x00, BYTE_BYTE_LEN, 0},
{0x46, 0x00, BYTE_BYTE_LEN, 0},
{0x47, 0x00, BYTE_BYTE_LEN, 0},
{0x48, 0x00, BYTE_BYTE_LEN, 0},
{0x49, 0x00, BYTE_BYTE_LEN, 0},
{0x4A, 0x00, BYTE_BYTE_LEN, 0},
{0x4B, 0x00, BYTE_BYTE_LEN, 0},
{0x4C, 0x00, BYTE_BYTE_LEN, 0},
{0x4D, 0x00, BYTE_BYTE_LEN, 0},
{0x4E, 0x44, BYTE_BYTE_LEN, 0},
{0x4F, 0x44, BYTE_BYTE_LEN, 0},
{0x50, 0x00, BYTE_BYTE_LEN, 0},
{0x51, 0x00, BYTE_BYTE_LEN, 0},
{0x52, 0x00, BYTE_BYTE_LEN, 0},
{0x53, 0x00, BYTE_BYTE_LEN, 0},
{0x54, 0x44, BYTE_BYTE_LEN, 0},
{0x55, 0x44, BYTE_BYTE_LEN, 0},
{0x56, 0x00, BYTE_BYTE_LEN, 0},
{0x57, 0x00, BYTE_BYTE_LEN, 0},
{0x58, 0x00, BYTE_BYTE_LEN, 0},
{0x59, 0x00, BYTE_BYTE_LEN, 0},
{0x5A, 0x44, BYTE_BYTE_LEN, 0},
{0x5B, 0x44, BYTE_BYTE_LEN, 0},
{0x5C, 0x00, BYTE_BYTE_LEN, 0},
{0x5D, 0x00, BYTE_BYTE_LEN, 0},
{0x5E, 0x00, BYTE_BYTE_LEN, 0},
{0x5F, 0x00, BYTE_BYTE_LEN, 0},
{0x60, 0x00, BYTE_BYTE_LEN, 0},
{0x61, 0x00, BYTE_BYTE_LEN, 0},
{0x62, 0x00, BYTE_BYTE_LEN, 0},
{0x63, 0x00, BYTE_BYTE_LEN, 0},
{0x64, 0x00, BYTE_BYTE_LEN, 0},
{0x65, 0x00, BYTE_BYTE_LEN, 0},
{0x66, 0x00, BYTE_BYTE_LEN, 0},
{0x67, 0x00, BYTE_BYTE_LEN, 0},
{0x68, 0x00, BYTE_BYTE_LEN, 0},
{0x69, 0x00, BYTE_BYTE_LEN, 0},
{0x6A, 0x00, BYTE_BYTE_LEN, 0},
{0x6B, 0x00, BYTE_BYTE_LEN, 0},
{0x6C, 0x00, BYTE_BYTE_LEN, 0},
{0x6D, 0x00, BYTE_BYTE_LEN, 0},
{0x6E, 0x00, BYTE_BYTE_LEN, 0},
{0x6F, 0x00, BYTE_BYTE_LEN, 0},
{0x10, 0x84, BYTE_BYTE_LEN, 0},

//AE On
{0x03, 0xc4, BYTE_BYTE_LEN, 0},
{0x10, 0xe2, BYTE_BYTE_LEN, 10},
 //Delay 10ms
	
//AWB On
{0x03, 0xc5, BYTE_BYTE_LEN, 0},
{0x10, 0xb1, BYTE_BYTE_LEN, 0},
};
/* -------- end of exposure ------------------ */

/* -------- start of brightness ------------------ */
static const struct hi351_i2c_reg_conf const cam_set_brightness_tbl_0[] = {
//Brightness -2
//{0x03, 0xd9, BYTE_BYTE_LEN, 0},
//{0x17, 0xb0, BYTE_BYTE_LEN, 0},
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x13, 0x0A, BYTE_BYTE_LEN, 0},
{0x4A, 0x40, BYTE_BYTE_LEN, 0},
};
static const struct hi351_i2c_reg_conf const cam_set_brightness_tbl_1[] = {
//Brightness -1
//{0x03, 0xd9, BYTE_BYTE_LEN, 0},
//{0x17, 0xa0, BYTE_BYTE_LEN, 0},
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x13, 0x0A, BYTE_BYTE_LEN, 0},
{0x4A, 0x60, BYTE_BYTE_LEN, 0},
};
static const struct hi351_i2c_reg_conf const cam_set_brightness_tbl_2[] = {
//Brightness 0 (Default)
//{0x03, 0xd9, BYTE_BYTE_LEN, 0},
//{0x17, 0x00, BYTE_BYTE_LEN, 0},
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x13, 0x0A, BYTE_BYTE_LEN, 0},
{0x4A, 0x80, BYTE_BYTE_LEN, 0},
};
static const struct hi351_i2c_reg_conf const cam_set_brightness_tbl_3[] = {
//Brightness +1
//{0x03, 0xd9, BYTE_BYTE_LEN, 0},
//{0x17, 0x14, BYTE_BYTE_LEN, 0},
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x13, 0x0A, BYTE_BYTE_LEN, 0},
{0x4A, 0xA0, BYTE_BYTE_LEN, 0},
};
static const struct hi351_i2c_reg_conf const cam_set_brightness_tbl_4[] = {
//Brightness +2
//{0x03, 0xd9, BYTE_BYTE_LEN, 0},
//{0x17, 0x28, BYTE_BYTE_LEN, 0},
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x13, 0x0A, BYTE_BYTE_LEN, 0},
{0x4A, 0xC0, BYTE_BYTE_LEN, 0},
};
/* -------- end of brightness ------------------ */

/* -------- start of saturation ------------------ */
static const struct hi351_i2c_reg_conf const cam_set_saturation_tbl_0[] = {
//Sat x0.5

  {0x03,0x30, BYTE_BYTE_LEN, 0}, //DMA & Adaptive Off
	{0x36,0xa3, BYTE_BYTE_LEN, 0},
	{0x03,0xd1, BYTE_BYTE_LEN, 10}, //Page d1/*page D1(Adaptive)*/
  	//delay 10ms
	{0x2b,0x5a, BYTE_BYTE_LEN, 0},
	{0x2c,0x5a, BYTE_BYTE_LEN, 0},
	{0x2d,0x5a, BYTE_BYTE_LEN, 0},
	{0x2e,0x6a, BYTE_BYTE_LEN, 0}, 
	{0x2f,0x6a, BYTE_BYTE_LEN, 0}, 
	{0x30,0x6a, BYTE_BYTE_LEN, 0}, 
	{0x31,0x6a, BYTE_BYTE_LEN, 0},
	{0x32,0x6a, BYTE_BYTE_LEN, 0},
	{0x33,0x6a, BYTE_BYTE_LEN, 0},
	{0x34,0x6a, BYTE_BYTE_LEN, 0},
	{0x35,0x6a, BYTE_BYTE_LEN, 0},
	{0x36,0x6a, BYTE_BYTE_LEN, 0},
	
	//Cr		

	{0x37,0x5a, BYTE_BYTE_LEN, 0},
	{0x38,0x5a, BYTE_BYTE_LEN, 0},
	{0x39,0x5a, BYTE_BYTE_LEN, 0},
	{0x3a,0x6a, BYTE_BYTE_LEN, 0},
	{0x3b,0x6a, BYTE_BYTE_LEN, 0}, 
	{0x3c,0x6a, BYTE_BYTE_LEN, 0},
	{0x3d,0x6a, BYTE_BYTE_LEN, 0},
	{0x3e,0x6a, BYTE_BYTE_LEN, 0},
	{0x3f,0x6a, BYTE_BYTE_LEN, 0},
	{0x40,0x6a, BYTE_BYTE_LEN, 0},
	{0x41,0x6a, BYTE_BYTE_LEN, 0},
	{0x42,0x6a, BYTE_BYTE_LEN, 0},

	
	{0x03,0xcf, BYTE_BYTE_LEN, 0}, //Adaptive On
	{0x10,0xaf, BYTE_BYTE_LEN, 0},
	         
	{0x03,0xc0, BYTE_BYTE_LEN, 0}, 
  {0x33,0x00, BYTE_BYTE_LEN, 0},
	{0x32,0x01, BYTE_BYTE_LEN, 0}, //DMA On
};
static const struct hi351_i2c_reg_conf const cam_set_saturation_tbl_1[] = {
//Saturation x0.75
 {0x03,0x30, BYTE_BYTE_LEN, 0}, //DMA & Adaptive Off
	{0x36,0xa3, BYTE_BYTE_LEN, 0},
	{0x03,0xd1, BYTE_BYTE_LEN, 10}, //Page d1/*page D1(Adaptive)*/
  	//delay 10ms
	{0x2b,0x6a, BYTE_BYTE_LEN, 0},
	{0x2c,0x6a, BYTE_BYTE_LEN, 0},
	{0x2d,0x6a, BYTE_BYTE_LEN, 0},
	{0x2e,0x7a, BYTE_BYTE_LEN, 0}, 
	{0x2f,0x7a, BYTE_BYTE_LEN, 0}, 
	{0x30,0x7a, BYTE_BYTE_LEN, 0}, 
	{0x31,0x7a, BYTE_BYTE_LEN, 0},
	{0x32,0x7a, BYTE_BYTE_LEN, 0},
	{0x33,0x7a, BYTE_BYTE_LEN, 0},
	{0x34,0x7a, BYTE_BYTE_LEN, 0},
	{0x35,0x7a, BYTE_BYTE_LEN, 0},
	{0x36,0x7a, BYTE_BYTE_LEN, 0},
	
	//Cr		

	{0x37,0x6a, BYTE_BYTE_LEN, 0},
	{0x38,0x6a, BYTE_BYTE_LEN, 0},
	{0x39,0x6a, BYTE_BYTE_LEN, 0},
	{0x3a,0x7a, BYTE_BYTE_LEN, 0},
	{0x3b,0x7a, BYTE_BYTE_LEN, 0}, 
	{0x3c,0x7a, BYTE_BYTE_LEN, 0},
	{0x3d,0x7a, BYTE_BYTE_LEN, 0},
	{0x3e,0x7a, BYTE_BYTE_LEN, 0},
	{0x3f,0x7a, BYTE_BYTE_LEN, 0},
	{0x40,0x7a, BYTE_BYTE_LEN, 0},
	{0x41,0x7a, BYTE_BYTE_LEN, 0},
	{0x42,0x7a, BYTE_BYTE_LEN, 0},

	
	{0x03,0xcf, BYTE_BYTE_LEN, 0}, //Adaptive On
	{0x10,0xaf, BYTE_BYTE_LEN, 0},
	         
	{0x03,0xc0, BYTE_BYTE_LEN, 0}, 
  {0x33,0x00, BYTE_BYTE_LEN, 0},
	{0x32,0x01, BYTE_BYTE_LEN, 0}, //DMA On
};
static const struct hi351_i2c_reg_conf const cam_set_saturation_tbl_2[] = {
//Saturation x1 (Default)
  {0x03,0x30, BYTE_BYTE_LEN, 0}, //DMA & Adaptive Off
	{0x36,0xa3, BYTE_BYTE_LEN, 0},
	{0x03,0xd1, BYTE_BYTE_LEN, 10}, //Page d1/*page D1(Adaptive)*/
  	//delay 10ms
	{0x2b,0x80, BYTE_BYTE_LEN, 0},
	{0x2c,0x80, BYTE_BYTE_LEN, 0},
	{0x2d,0x90, BYTE_BYTE_LEN, 0},
	{0x2e,0x90, BYTE_BYTE_LEN, 0}, 
	{0x2f,0x90, BYTE_BYTE_LEN, 0}, 
	{0x30,0x90, BYTE_BYTE_LEN, 0}, 
	{0x31,0x90, BYTE_BYTE_LEN, 0},
	{0x32,0x90, BYTE_BYTE_LEN, 0},
	{0x33,0x90, BYTE_BYTE_LEN, 0},
	{0x34,0x90, BYTE_BYTE_LEN, 0},
	{0x35,0x90, BYTE_BYTE_LEN, 0},
	{0x36,0x90, BYTE_BYTE_LEN, 0},
	
	//Cr		

	{0x37,0x80, BYTE_BYTE_LEN, 0},
	{0x38,0x80, BYTE_BYTE_LEN, 0},
	{0x39,0x80, BYTE_BYTE_LEN, 0},
	{0x3a,0x90, BYTE_BYTE_LEN, 0},
	{0x3b,0x90, BYTE_BYTE_LEN, 0}, 
	{0x3c,0x90, BYTE_BYTE_LEN, 0},
	{0x3d,0x90, BYTE_BYTE_LEN, 0},
	{0x3e,0x90, BYTE_BYTE_LEN, 0},
	{0x3f,0x90, BYTE_BYTE_LEN, 0},
	{0x40,0x90, BYTE_BYTE_LEN, 0},
	{0x41,0x90, BYTE_BYTE_LEN, 0},
	{0x42,0x90, BYTE_BYTE_LEN, 0},
	
	{0x03,0xcf, BYTE_BYTE_LEN, 0}, //Adaptive On
	{0x10,0xaf, BYTE_BYTE_LEN, 0},
	         
	{0x03,0xc0, BYTE_BYTE_LEN, 0}, 
  {0x33,0x00, BYTE_BYTE_LEN, 0},
	{0x32,0x01, BYTE_BYTE_LEN, 0}, //DMA On
};
static const struct hi351_i2c_reg_conf const cam_set_saturation_tbl_3[] = {
//Saturation x1.25
   {0x03,0x30, BYTE_BYTE_LEN, 0}, //DMA & Adaptive Off
	{0x36,0xa3, BYTE_BYTE_LEN, 0},
	{0x03,0xd1, BYTE_BYTE_LEN, 10}, //Page d1/*page D1(Adaptive)*/
  	//delay 10ms
	{0x2b,0x8a, BYTE_BYTE_LEN, 0},
	{0x2c,0x8a, BYTE_BYTE_LEN, 0},
	{0x2d,0x8a, BYTE_BYTE_LEN, 0},
	{0x2e,0x9a, BYTE_BYTE_LEN, 0}, 
	{0x2f,0x9a, BYTE_BYTE_LEN, 0}, 
	{0x30,0x9a, BYTE_BYTE_LEN, 0}, 
	{0x31,0x9a, BYTE_BYTE_LEN, 0},
	{0x32,0x9a, BYTE_BYTE_LEN, 0},
	{0x33,0x9a, BYTE_BYTE_LEN, 0},
	{0x34,0x9a, BYTE_BYTE_LEN, 0},
	{0x35,0x9a, BYTE_BYTE_LEN, 0},
	{0x36,0x9a, BYTE_BYTE_LEN, 0},
	
	//Cr		

	{0x37,0x8a, BYTE_BYTE_LEN, 0},
	{0x38,0x8a, BYTE_BYTE_LEN, 0},
	{0x39,0x8a, BYTE_BYTE_LEN, 0},
	{0x3a,0x9a, BYTE_BYTE_LEN, 0},
	{0x3b,0x9a, BYTE_BYTE_LEN, 0}, 
	{0x3c,0x9a, BYTE_BYTE_LEN, 0},
	{0x3d,0x9a, BYTE_BYTE_LEN, 0},
	{0x3e,0x9a, BYTE_BYTE_LEN, 0},
	{0x3f,0x9a, BYTE_BYTE_LEN, 0},
	{0x40,0x9a, BYTE_BYTE_LEN, 0},
	{0x41,0x9a, BYTE_BYTE_LEN, 0},
	{0x42,0x9a, BYTE_BYTE_LEN, 0},

	
	{0x03,0xcf, BYTE_BYTE_LEN, 0}, //Adaptive On
	{0x10,0xaf, BYTE_BYTE_LEN, 0},
	         
	{0x03,0xc0, BYTE_BYTE_LEN, 0}, 
  {0x33,0x00, BYTE_BYTE_LEN, 0},
	{0x32,0x01, BYTE_BYTE_LEN, 0}, //DMA On
};
static const struct hi351_i2c_reg_conf const cam_set_saturation_tbl_4[] = {
//Sat x1.5
  {0x03,0x30, BYTE_BYTE_LEN, 0}, //DMA & Adaptive Off
	{0x36,0xa3, BYTE_BYTE_LEN, 0},
	{0x03,0xd1, BYTE_BYTE_LEN, 10}, //Page d1/*page D1(Adaptive)*/
  	//delay 10ms
	{0x2b,0x9a, BYTE_BYTE_LEN, 0},
	{0x2c,0x9a, BYTE_BYTE_LEN, 0},
	{0x2d,0x9a, BYTE_BYTE_LEN, 0},
	{0x2e,0xaa, BYTE_BYTE_LEN, 0}, 
	{0x2f,0xaa, BYTE_BYTE_LEN, 0}, 
	{0x30,0xaa, BYTE_BYTE_LEN, 0}, 
	{0x31,0xaa, BYTE_BYTE_LEN, 0},
	{0x32,0xaa, BYTE_BYTE_LEN, 0},
	{0x33,0xaa, BYTE_BYTE_LEN, 0},
	{0x34,0xaa, BYTE_BYTE_LEN, 0},
	{0x35,0xaa, BYTE_BYTE_LEN, 0},
	{0x36,0xaa, BYTE_BYTE_LEN, 0},
	
	//Cr		

	{0x37,0x9a, BYTE_BYTE_LEN, 0},
	{0x38,0x9a, BYTE_BYTE_LEN, 0},
	{0x39,0x9a, BYTE_BYTE_LEN, 0},
	{0x3a,0xaa, BYTE_BYTE_LEN, 0},
	{0x3b,0xaa, BYTE_BYTE_LEN, 0}, 
	{0x3c,0xaa, BYTE_BYTE_LEN, 0},
	{0x3d,0xaa, BYTE_BYTE_LEN, 0},
	{0x3e,0xaa, BYTE_BYTE_LEN, 0},
	{0x3f,0xaa, BYTE_BYTE_LEN, 0},
	{0x40,0xaa, BYTE_BYTE_LEN, 0},
	{0x41,0xaa, BYTE_BYTE_LEN, 0},
	{0x42,0xaa, BYTE_BYTE_LEN, 0},

	{0x03,0xcf, BYTE_BYTE_LEN, 0}, //Adaptive On
	{0x10,0xaf, BYTE_BYTE_LEN, 0},
	         
	{0x03,0xc0, BYTE_BYTE_LEN, 0}, 
  {0x33,0x00, BYTE_BYTE_LEN, 0},
	{0x32,0x01, BYTE_BYTE_LEN, 0}, //DMA On
};
/* -------- end of saturation ------------------ */

/* -------- start of sharpness ------------------ */
static const struct hi351_i2c_reg_conf const cam_set_sharpness_tbl_0[] = {
//Sharpness 0
{ 0x03, 0x11, BYTE_BYTE_LEN, 0}, 
{ 0xf0, 0x0d, BYTE_BYTE_LEN, 0}, //DMA dark2
};
static const struct hi351_i2c_reg_conf const cam_set_sharpness_tbl_1[] = {
//Sharpness 1
{ 0x03, 0x11, BYTE_BYTE_LEN, 0}, 
{ 0xf0, 0x0c, BYTE_BYTE_LEN, 0}, //DMA dark1
};
static const struct hi351_i2c_reg_conf const cam_set_sharpness_tbl_2[] = {
//Sharpness_Auto (Default)
{ 0x03, 0x11, BYTE_BYTE_LEN, 0}, 
{ 0xf0, 0x20, BYTE_BYTE_LEN, 0}, //DMA Auto
};
static const struct hi351_i2c_reg_conf const cam_set_sharpness_tbl_3[] = {
//Sharpness 3
{ 0x03, 0x11, BYTE_BYTE_LEN, 0}, 
{ 0xf0, 0x0b, BYTE_BYTE_LEN, 0}, //DMA in
};
static const struct hi351_i2c_reg_conf const cam_set_sharpness_tbl_4[] = {
//Sharpness 4
{ 0x03, 0x11, BYTE_BYTE_LEN, 0}, 
{ 0xf0, 0x09, BYTE_BYTE_LEN, 0}, //DMA out
};
/* -------- end of sharpness ------------------ */

/* -------- start of contrast	------------------*/
static const struct hi351_i2c_reg_conf const cam_set_contrast_tbl_0[] = {
//Contrast -2
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x48, 0x60, BYTE_BYTE_LEN, 0},
};

static const struct hi351_i2c_reg_conf const cam_set_contrast_tbl_1[] = {
//Contrast -1
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x48, 0x70, BYTE_BYTE_LEN, 0},
};

static const struct hi351_i2c_reg_conf const cam_set_contrast_tbl_2[] = {
//Contrast (Default)
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x48, 0x98, BYTE_BYTE_LEN, 0},
};

static const struct hi351_i2c_reg_conf const cam_set_contrast_tbl_3[] = {
//Contrast +1
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x48, 0xa0, BYTE_BYTE_LEN, 0},
};

static const struct hi351_i2c_reg_conf const cam_set_contrast_tbl_4[] = {
//Contrast +2 
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x48, 0xb0, BYTE_BYTE_LEN, 0},
};
/* -------- end of contrast   ------------------*/
/* -------- start of effect ------------------ */
static const struct hi351_i2c_reg_conf const cam_set_effect_off_tbl[] = {
//CAMERA_EFFECT_OFF           0
	{ 0x03, 0x10, BYTE_BYTE_LEN, 0},
	{ 0x11, 0x03, BYTE_BYTE_LEN, 0},
	{ 0x12, 0xf0, BYTE_BYTE_LEN, 0}, 
	{ 0x13, 0x0a, BYTE_BYTE_LEN, 0},
	{ 0x42, 0x00, BYTE_BYTE_LEN, 0},
	{ 0x43, 0x00, BYTE_BYTE_LEN, 0},
	{ 0x44, 0x80, BYTE_BYTE_LEN, 0},
	{ 0x45, 0x80, BYTE_BYTE_LEN, 0},
};
static const struct hi351_i2c_reg_conf const cam_set_effect_mono_tbl[] = {
//CAMERA_EFFECT_MONO          1
  { 0x03, 0x10, BYTE_BYTE_LEN, 0},
	{ 0x11, 0x03, BYTE_BYTE_LEN, 0},
	{ 0x12, 0x33, BYTE_BYTE_LEN, 0},
	{ 0x13, 0x00, BYTE_BYTE_LEN, 0},
	{ 0x40, 0x00, BYTE_BYTE_LEN, 0},
	{ 0x44, 0x80, BYTE_BYTE_LEN, 0},
	{ 0x45, 0x80, BYTE_BYTE_LEN, 0},
};
static const struct hi351_i2c_reg_conf const cam_set_effect_negative_tbl[] = {
//CAMERA_EFFECT_NEGATIVE      2
  { 0x03, 0x10, BYTE_BYTE_LEN, 0},
	{ 0x11, 0x03, BYTE_BYTE_LEN, 0},
	{ 0x12, 0x18, BYTE_BYTE_LEN, 0},
	{ 0x13, 0x0a, BYTE_BYTE_LEN, 0},
	{ 0x42, 0x00, BYTE_BYTE_LEN, 0},
	{ 0x43, 0x00, BYTE_BYTE_LEN, 0},
	{ 0x44, 0x80, BYTE_BYTE_LEN, 0},
	{ 0x45, 0x80, BYTE_BYTE_LEN, 0},
};
static const struct hi351_i2c_reg_conf const cam_set_effect_solarize_tbl[] = {

};
static const struct hi351_i2c_reg_conf const cam_set_effect_sepia_tbl[] = {
//CAMERA_EFFECT_SEPIA         4
  { 0x03, 0x10, BYTE_BYTE_LEN, 0},
	{ 0x11, 0x03, BYTE_BYTE_LEN, 0},
	{ 0x12, 0x13, BYTE_BYTE_LEN, 0},
	{ 0x13, 0x0a, BYTE_BYTE_LEN, 0},
	{ 0x42, 0x00, BYTE_BYTE_LEN, 0},
	{ 0x43, 0x00, BYTE_BYTE_LEN, 0},
	{ 0x44, 0x60, BYTE_BYTE_LEN, 0},
	{ 0x45, 0xa3, BYTE_BYTE_LEN, 0},
};
/* -------- end of effect ------------------ */
static const struct hi351_i2c_reg_conf const cam_set_mode_preview_tbl[] = {
//[Go to preview]

	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x01, 0xf1, BYTE_BYTE_LEN, 0}, //Sleep on

  {0x03, 0x30, BYTE_BYTE_LEN, 0}, //DMA&Adaptive Off
	{0x36, 0xa3, BYTE_BYTE_LEN, 10},
	//delay 10ms
	
	{0x03, 0xc4, BYTE_BYTE_LEN, 0}, //AE off
	{0x10, 0x60, BYTE_BYTE_LEN, 0},
	{0x03, 0xc5, BYTE_BYTE_LEN, 0}, //AWB off
	{0x10, 0x30, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x19, BYTE_BYTE_LEN, 0}, //Scaler Setting
	{0x10, 0x87, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x10, 0x13, BYTE_BYTE_LEN, 0}, //Sub1/2 + Pre2
	//{0x11, 0x83, BYTE_BYTE_LEN, 0}, // STEVE 0 skip Fix Frame Off, XY Flip
	{0x13, 0x80, BYTE_BYTE_LEN, 0}, //Fix AE Set Off
	{0x14, 0x70, BYTE_BYTE_LEN, 0}, // for Pre2mode
	
	{0x03, 0xFE, BYTE_BYTE_LEN, 0},
	{0xFE, 0x2c, BYTE_BYTE_LEN, 10},  //Delay 10ms //important

	
	{0x03, 0x10, BYTE_BYTE_LEN, 0}, //jktest 0131
	{0x40, 0x03, BYTE_BYTE_LEN, 0}, //jktest 0131//yoffset
	
	//{0x03, 0xFE, BYTE_BYTE_LEN, 0},
	//{0xFE, 0x2c, BYTE_BYTE_LEN, 0}, //Delay 10ms

	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x20, 0x00, BYTE_BYTE_LEN, 0},
	{0x21, 0x01, BYTE_BYTE_LEN, 0}, //preview row start set.
	{0x03, 0x15, BYTE_BYTE_LEN, 0},  //Shading
	{0x10, 0x81, BYTE_BYTE_LEN, 0},
	
	{0x20, 0x04, BYTE_BYTE_LEN, 0},  //Shading Width 1024 (pre2)
	{0x21, 0x00, BYTE_BYTE_LEN, 0},
	{0x22, 0x03, BYTE_BYTE_LEN, 0},  //Shading Height 768
	{0x23, 0x00, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x48, BYTE_BYTE_LEN, 0},
	{0x10, 0x1C, BYTE_BYTE_LEN, 0}, //MIPI On
	{0x16, 0x04, BYTE_BYTE_LEN, 0},
	{0x30, 0x00, BYTE_BYTE_LEN, 0}, //640 * 2
	{0x31, 0x05, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x30, BYTE_BYTE_LEN, 0},
	{0x36, 0x28, BYTE_BYTE_LEN, 0}, //Preview set //important
	
	{0x03, 0xFE, BYTE_BYTE_LEN, 0},
	{0xFE, 0x2c, BYTE_BYTE_LEN, 10}, //Delay 10ms
	
	{0x03, 0x11, BYTE_BYTE_LEN, 0},
	{0xf0, 0x20, BYTE_BYTE_LEN, 0}, //jktest 0203//for longhceer aw551//0x0d, BYTE_BYTE_LEN, 0}, // STEVE Dark mode for Sawtooth
	
	{0x03, 0xc4, BYTE_BYTE_LEN, 0}, //AE en
	{0x10, 0xeb, BYTE_BYTE_LEN, 0}, //JH guide 0xea, BYTE_BYTE_LEN, 0}, //50hz//0xe1, BYTE_BYTE_LEN, 0},//auto
	
	{0x03, 0xFE, BYTE_BYTE_LEN, 0},
	{0xFE, 0x2c, BYTE_BYTE_LEN, 20}, //Delay 20ms
	
	{0x03, 0xc5, BYTE_BYTE_LEN, 0}, //AWB en
	{0x10, 0xb1, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x01, 0xf0, BYTE_BYTE_LEN, 0}, //sleep off
		
	{0x03, 0xcf, BYTE_BYTE_LEN, 0}, //Adaptive On
	{0x10, 0xaf, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xc0, BYTE_BYTE_LEN, 0}, 
	{0x33, 0x00, BYTE_BYTE_LEN, 0},
	{0x32, 0x01, BYTE_BYTE_LEN, 0}, //DMA On
	


};
static const struct hi351_i2c_reg_conf const cam_release_tbl[] = {
//[Go to preview]
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x01, 0xf1, BYTE_BYTE_LEN, 0}, //Sleep on

 	{0x03, 0x30, BYTE_BYTE_LEN, 0}, //DMA&Adaptive Off
	{0x36, 0xa3, BYTE_BYTE_LEN, 0},
	//delay 10ms
	
	{0x03, 0xc4, BYTE_BYTE_LEN, 0}, //AE off
	{0x10, 0x60, BYTE_BYTE_LEN, 0},
	{0x03, 0xc5, BYTE_BYTE_LEN, 0}, //AWB off
	{0x10, 0x30, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x19, BYTE_BYTE_LEN, 0}, //Scaler Setting
	{0x10, 0x87, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x10, 0x13, BYTE_BYTE_LEN, 0}, //Sub1/2 + Pre2
	//{0x11, 0x83, BYTE_BYTE_LEN, 0}, // STEVE 0 skip Fix Frame Off, XY Flip
	{0x13, 0x80, BYTE_BYTE_LEN, 0}, //Fix AE Set Off
	{0x14, 0x70, BYTE_BYTE_LEN, 0}, // for Pre2mode
	
	{0x03, 0x10, BYTE_BYTE_LEN, 0}, //jktest 0131
	{0x40, 0x03, BYTE_BYTE_LEN, 0}, //jktest 0131//yoffset
	
	{0x03, 0xFE, BYTE_BYTE_LEN, 0},
	{0xFE, 0x2c, BYTE_BYTE_LEN, 10}, //Delay 10ms

	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x20, 0x00, BYTE_BYTE_LEN, 0},
	{0x21, 0x01, BYTE_BYTE_LEN, 0}, //preview row start set.
	{0x03, 0x15, BYTE_BYTE_LEN, 0},  //Shading
	{0x10, 0x81, BYTE_BYTE_LEN, 0},
	
	{0x20, 0x04, BYTE_BYTE_LEN, 0},  //Shading Width 1024 (pre2)
	{0x21, 0x00, BYTE_BYTE_LEN, 0},
	{0x22, 0x03, BYTE_BYTE_LEN, 0},  //Shading Height 768
	{0x23, 0x00, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x48, BYTE_BYTE_LEN, 0},
	{0x10, 0x1C, BYTE_BYTE_LEN, 0}, //MIPI On
	{0x16, 0x04, BYTE_BYTE_LEN, 0},
	{0x30, 0x00, BYTE_BYTE_LEN, 0}, //640 * 2
	{0x31, 0x05, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x30, BYTE_BYTE_LEN, 0},
	{0x36, 0x28, BYTE_BYTE_LEN, 0}, //Preview set //important
	
	{0x03, 0xFE, BYTE_BYTE_LEN, 0},
	{0xFE, 0x2c, BYTE_BYTE_LEN, 10}, //Delay 10ms
	
	{0x03, 0x11, BYTE_BYTE_LEN, 0},
	{0xf0, 0x0d, BYTE_BYTE_LEN, 0}, // STEVE Dark mode for Sawtooth
	
	{0x03, 0xc4, BYTE_BYTE_LEN, 0}, //AE en
	{0x10, 0xe1, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xFE, BYTE_BYTE_LEN, 0},
	{0xFE, 0x2c, BYTE_BYTE_LEN, 20}, //Delay 20ms
	
	{0x03, 0xc5, BYTE_BYTE_LEN, 0}, //AWB en
	{0x10, 0xb1, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x01, 0xf0, BYTE_BYTE_LEN, 0}, //sleep off
		
	{0x03, 0xcf, BYTE_BYTE_LEN, 0}, //Adaptive On
	{0x10, 0xaf, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xc0, BYTE_BYTE_LEN, 0}, 
	{0x33, 0x00, BYTE_BYTE_LEN, 0},
	{0x32, 0x01, BYTE_BYTE_LEN, 0}, //DMA On
};	
static const struct hi351_i2c_reg_conf const cam_set_mode_snapshot_tbl[] = {
//[Go to capture]
  {0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x01, 0xf1, BYTE_BYTE_LEN, 0}, //Sleep on
	
  {0x03, 0x30, BYTE_BYTE_LEN, 0}, //DMA & Adaptive Off
	{0x36, 0xa3, BYTE_BYTE_LEN, 10},
   //delay 10ms
	{0x03, 0xc4, BYTE_BYTE_LEN, 0}, //AE off
	{0x10, 0x68, BYTE_BYTE_LEN, 0},
	{0x03, 0xc5, BYTE_BYTE_LEN, 0}, //AWB off
	{0x10, 0x30, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x19, BYTE_BYTE_LEN, 0}, //Scaler Off
	{0x10, 0x00, BYTE_BYTE_LEN, 0},
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x10, 0x00, BYTE_BYTE_LEN, 0}, //Full
	{0x14, 0x20, BYTE_BYTE_LEN, 0}, //for Cap mode
	
	{0x03, 0x10, BYTE_BYTE_LEN, 0}, //jktest 0131
	{0x40, 0x00, BYTE_BYTE_LEN, 0}, //jktest 0131
	
	{0x03, 0xFE, BYTE_BYTE_LEN, 0},
	{0xFE, 0x2c, BYTE_BYTE_LEN, 10}, //Delay 10ms
	
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
  	{0x20, 0x00, BYTE_BYTE_LEN, 0},
	{0x21, 0x03, BYTE_BYTE_LEN, 0}, //preview row start set.
	
	
	{0x03, 0x15, BYTE_BYTE_LEN, 0}, //Shading
	{0x10, 0x83, BYTE_BYTE_LEN, 0}, // 00 - 83 LSC ON
	{0x20, 0x07, BYTE_BYTE_LEN, 0},
	{0x21, 0xf8, BYTE_BYTE_LEN, 0},
	{0x22, 0x05, BYTE_BYTE_LEN, 0},
	{0x23, 0xf8, BYTE_BYTE_LEN, 0},
	
	{0x03, 0x11, BYTE_BYTE_LEN, 0},
	{0xf0, 0x00, BYTE_BYTE_LEN, 0}, // STEVE Dark mode for Sawtooth
	
	{0x03, 0x48, BYTE_BYTE_LEN, 0}, //MIPI Setting
	{0x10, 0x1C, BYTE_BYTE_LEN, 0},
	{0x16, 0x04, BYTE_BYTE_LEN, 0},
	{0x30, 0x00, BYTE_BYTE_LEN, 0}, //2048 * 2
	{0x31, 0x10, BYTE_BYTE_LEN, 0},
	
	
	{0x03, 0x30, BYTE_BYTE_LEN, 0},
	{0x36, 0x29, BYTE_BYTE_LEN, 0}, //Capture
	
	{0x03, 0xFE, BYTE_BYTE_LEN, 0},
	{0xFE, 0x2c, BYTE_BYTE_LEN, 20}, //Delay 20ms
	
	{0x03, 0x00, BYTE_BYTE_LEN, 0},
	{0x01, 0xf0, BYTE_BYTE_LEN, 0}, //sleep off
	
	{0x03, 0xcf, BYTE_BYTE_LEN, 0}, //Adaptive On
	{0x10, 0xaf, BYTE_BYTE_LEN, 0},
	
	{0x03, 0xc0, BYTE_BYTE_LEN, 0}, 
  {0x33, 0x00, BYTE_BYTE_LEN, 0},
	{0x32, 0x01, BYTE_BYTE_LEN, 0}, //DMA On
};
static const struct hi351_i2c_reg_conf const cam_set_mode_rawsnapshot_tbl[] = {
//	{ 0x098E, 0x8410, WORD_LEN, 0}, 	// LOGICAL_ADDRESS_ACCE
};

struct hi351_reg hi351_regs = {
	.cambl = cam_init_tbl,
	.cambl_size = ARRAY_SIZE(cam_init_tbl),

	.cambl_otp = cam_init_tbl_otp,
	.cambl_otp_size = ARRAY_SIZE(cam_init_tbl_otp),

	.cambl_otp2 = cam_init_tbl_otp2,
	.cambl_otp2_size = ARRAY_SIZE(cam_init_tbl_otp2),

	.wbtbl_auto = cam_set_wb_tbl_auto , 
	.wbtbl_size_auto = ARRAY_SIZE(cam_set_wb_tbl_auto),
	.wbtbl_daylight =  cam_set_wb_tbl_daylight, 
	.wbtbl_size_daylight = ARRAY_SIZE(cam_set_wb_tbl_daylight),
	.wbtbl_cloudy =  cam_set_wb_tbl_cloudy, 
	.wbtbl_size_cloudy = ARRAY_SIZE(cam_set_wb_tbl_cloudy),
	.wbtbl_flourescent = cam_set_wb_tbl_flourescent , 
	.wbtbl_size_flourescent = ARRAY_SIZE(cam_set_wb_tbl_flourescent),
	.wbtbl_incandescent = cam_set_wb_tbl_incandescent,
	.wbtbl_size_incandescent =ARRAY_SIZE(cam_set_wb_tbl_incandescent),

	.isotbl_auto = cam_set_iso_tbl_auto, 
	.isotbl_size_auto = ARRAY_SIZE(cam_set_wb_tbl_auto),
	.isotbl_100 = cam_set_iso_tbl_100, 
	.isotbl_size_100 = ARRAY_SIZE(cam_set_iso_tbl_100),
	.isotbl_200 = cam_set_iso_tbl_200, 
	.isotbl_size_200 = ARRAY_SIZE(cam_set_iso_tbl_200),
	.isotbl_400 = cam_set_iso_tbl_400, 
	.isotbl_size_400 = ARRAY_SIZE(cam_set_iso_tbl_400),
	.isotbl_800 = cam_set_iso_tbl_800, 
	.isotbl_size_800 = ARRAY_SIZE(cam_set_iso_tbl_800),


	.exposuretbl_averg = cam_set_exposure_tbl_averg,
	.exposuretbl_size_averg = ARRAY_SIZE(cam_set_exposure_tbl_averg),
	.exposuretbl_center = cam_set_exposure_tbl_center,
	.exposuretbl_size_center = ARRAY_SIZE(cam_set_exposure_tbl_center),
	.exposuretbl_spot = cam_set_exposure_tbl_spot,
	.exposuretbl_size_spot = ARRAY_SIZE(cam_set_exposure_tbl_spot),

	.brightnesstbl_0 = cam_set_brightness_tbl_0,
	.brightnesstbl_size_0 = ARRAY_SIZE(cam_set_brightness_tbl_0),
	.brightnesstbl_1 = cam_set_brightness_tbl_1,
	.brightnesstbl_size_1 = ARRAY_SIZE(cam_set_brightness_tbl_1),
	.brightnesstbl_2 = cam_set_brightness_tbl_2,
	.brightnesstbl_size_2 = ARRAY_SIZE(cam_set_brightness_tbl_2),
	.brightnesstbl_3 = cam_set_brightness_tbl_3,
	.brightnesstbl_size_3 = ARRAY_SIZE(cam_set_brightness_tbl_3),
	.brightnesstbl_4 = cam_set_brightness_tbl_4,
	.brightnesstbl_size_4 = ARRAY_SIZE(cam_set_brightness_tbl_4),

	.sharpnesstbl_0 = cam_set_sharpness_tbl_0,
	.sharpnesstbl_size_0 = ARRAY_SIZE(cam_set_sharpness_tbl_0),
	.sharpnesstbl_1 = cam_set_sharpness_tbl_1,
	.sharpnesstbl_size_1 = ARRAY_SIZE(cam_set_sharpness_tbl_1),
	.sharpnesstbl_2 = cam_set_sharpness_tbl_2,
	.sharpnesstbl_size_2 = ARRAY_SIZE(cam_set_sharpness_tbl_2),
	.sharpnesstbl_3 = cam_set_sharpness_tbl_3,
	.sharpnesstbl_size_3 = ARRAY_SIZE(cam_set_sharpness_tbl_3),
	.sharpnesstbl_4 = cam_set_sharpness_tbl_4,
	.sharpnesstbl_size_4 = ARRAY_SIZE(cam_set_sharpness_tbl_4),

	.saturationtbl_0 = cam_set_saturation_tbl_0,
	.saturationtbl_size_0 = ARRAY_SIZE(cam_set_saturation_tbl_0),
	.saturationtbl_1 = cam_set_saturation_tbl_1,
	.saturationtbl_size_1 = ARRAY_SIZE(cam_set_saturation_tbl_1),
	.saturationtbl_2 = cam_set_saturation_tbl_2,
	.saturationtbl_size_2 = ARRAY_SIZE(cam_set_saturation_tbl_2),
	.saturationtbl_3 = cam_set_saturation_tbl_3,
	.saturationtbl_size_3 = ARRAY_SIZE(cam_set_saturation_tbl_3),
	.saturationtbl_4 = cam_set_saturation_tbl_4,
	.saturationtbl_size_4 = ARRAY_SIZE(cam_set_saturation_tbl_4),

	.contrasttbl_0 = cam_set_contrast_tbl_0,
	.contrasttbl_size_0 = ARRAY_SIZE(cam_set_contrast_tbl_0),
	.contrasttbl_1 = cam_set_contrast_tbl_1,
	.contrasttbl_size_1 = ARRAY_SIZE(cam_set_contrast_tbl_1),
	.contrasttbl_2 = cam_set_contrast_tbl_2,
	.contrasttbl_size_2 = ARRAY_SIZE(cam_set_contrast_tbl_2),
	.contrasttbl_3 = cam_set_contrast_tbl_3,
	.contrasttbl_size_3 = ARRAY_SIZE(cam_set_contrast_tbl_3),
	.contrasttbl_4 = cam_set_contrast_tbl_4,
	.contrasttbl_size_4 = ARRAY_SIZE(cam_set_contrast_tbl_4),

	.effecttbl_off = cam_set_effect_off_tbl,
	.effecttbl_size_off = ARRAY_SIZE(cam_set_effect_off_tbl),
	.effecttbl_mono = cam_set_effect_mono_tbl,
	.effecttbl_size_mono = ARRAY_SIZE(cam_set_effect_mono_tbl),
	.effecttbl_negative = cam_set_effect_negative_tbl,
	.effecttbl_size_negative = ARRAY_SIZE(cam_set_effect_negative_tbl),
	.effecttbl_solarize = cam_set_effect_solarize_tbl,
	.effecttbl_size_solarize = ARRAY_SIZE(cam_set_effect_solarize_tbl),
	.effecttbl_sepia = cam_set_effect_sepia_tbl,
	.effecttbl_size_sepia = ARRAY_SIZE(cam_set_effect_sepia_tbl),

	.previewtbl = cam_set_mode_preview_tbl,
	.previewtbl_size = ARRAY_SIZE(cam_set_mode_preview_tbl),
	.snapshottbl = cam_set_mode_snapshot_tbl,
	.snapshottbl_size = ARRAY_SIZE(cam_set_mode_snapshot_tbl),
	.rawsnapshottbl = cam_set_mode_rawsnapshot_tbl,
	.rawsnapshottbl_size = ARRAY_SIZE(cam_set_mode_rawsnapshot_tbl),
	.releasetbl = cam_release_tbl,
	.releasetbl_size = ARRAY_SIZE(cam_release_tbl),
};

#endif /* CAMSENSOR_hi351 */
