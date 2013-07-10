/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of Code Aurora Forum, Inc. nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef HI707_H
#define HI707_H
#include <linux/types.h>
#include <mach/board.h>

#define INVMASK(v)  (0xff-v)
#define hi707Core_WritePREG(pTbl)     hi707_WritePRegs(pTbl,sizeof(pTbl)/sizeof(pTbl[0]))

/* HI707 product ID register address */
#define REG_HI707_MODEL_ID                       0x04

/* HI707 product ID */
#define HI707_MODEL_ID                       0xB8

struct reg_addr_val_pair_struct {
	uint8_t	reg_addr;
	uint8_t	reg_val;
};

enum hi707_width{
	WORD_LEN,
	BYTE_LEN,
	BYTE_BYTE_LEN
};

enum hi707_test_mode_t {
	TEST_OFF,
	TEST_1,
	TEST_2,
	TEST_3
};

enum hi707_resolution_t {
	QTR_SIZE,
	FULL_SIZE,
	INVALID_SIZE
};

enum hi707_setting {
	RES_PREVIEW,
	RES_CAPTURE
};
enum hi707_reg_update {
	/* Sensor egisters that need to be updated during initialization */
	REG_INIT,
	/* Sensor egisters that needs periodic I2C writes */
	UPDATE_PERIODIC,
	/* All the sensor Registers will be updated */
	UPDATE_ALL,
	/* Not valid update */
	UPDATE_INVALID
};

struct hi707_i2c_reg_conf{
	unsigned short waddr;
	unsigned short wdata;
	enum hi707_width width;
	unsigned short mdelay_time;
};
/*OV SENSOR SCCB*/
typedef struct {
	uint8_t addr;
	uint8_t data;
	uint8_t mask;
} hi707_WREG, *Phi707_WREG;

/*  96MHz PCLK @ 24MHz MCLK */

struct hi707_reg {
/* -------------- sensor init -----------------*/
	const struct hi707_i2c_reg_conf *cambl;
	uint16_t cambl_size;
	
/* ------------ white balance --------------*/
	const struct hi707_i2c_reg_conf *wbtbl_auto;
	uint16_t wbtbl_size_auto;
	const struct hi707_i2c_reg_conf *wbtbl_daylight;
	uint16_t wbtbl_size_daylight;
	const struct hi707_i2c_reg_conf *wbtbl_cloudy;
	uint16_t wbtbl_size_cloudy;
	const struct hi707_i2c_reg_conf *wbtbl_flourescent;
	uint16_t wbtbl_size_flourescent;
	const struct hi707_i2c_reg_conf *wbtbl_incandescent;
	uint16_t wbtbl_size_incandescent;
/* ------------- iso --------------  */
	const struct hi707_i2c_reg_conf *isotbl_auto;
	uint16_t isotbl_size_auto;
	const struct hi707_i2c_reg_conf *isotbl_100;
	uint16_t isotbl_size_100;
	const struct hi707_i2c_reg_conf *isotbl_200;
	uint16_t isotbl_size_200;
	const struct hi707_i2c_reg_conf *isotbl_400;
	uint16_t isotbl_size_400;
	const struct hi707_i2c_reg_conf *isotbl_800;
	uint16_t isotbl_size_800;

/* ---------------- exposure -----------------*/
	const struct hi707_i2c_reg_conf *exposuretbl_averg;
	uint16_t exposuretbl_size_averg;
	const struct hi707_i2c_reg_conf *exposuretbl_center;
	uint16_t exposuretbl_size_center;
	const struct hi707_i2c_reg_conf *exposuretbl_spot;
	uint16_t exposuretbl_size_spot;

/* -------------------- brightness --------------*/
	const struct hi707_i2c_reg_conf *brightnesstbl_0;
	uint16_t brightnesstbl_size_0;
	const struct hi707_i2c_reg_conf *brightnesstbl_1;
	uint16_t brightnesstbl_size_1;
	const struct hi707_i2c_reg_conf *brightnesstbl_2;
	uint16_t brightnesstbl_size_2;
	const struct hi707_i2c_reg_conf *brightnesstbl_3;
	uint16_t brightnesstbl_size_3;
	const struct hi707_i2c_reg_conf *brightnesstbl_4;
	uint16_t brightnesstbl_size_4;

/* -------------------- sharpness ----------------- */
	const struct hi707_i2c_reg_conf *sharpnesstbl_0;
	uint16_t sharpnesstbl_size_0;
	const struct hi707_i2c_reg_conf *sharpnesstbl_1;
	uint16_t sharpnesstbl_size_1;
	const struct hi707_i2c_reg_conf *sharpnesstbl_2;
	uint16_t sharpnesstbl_size_2;
	const struct hi707_i2c_reg_conf *sharpnesstbl_3;
	uint16_t sharpnesstbl_size_3;
	const struct hi707_i2c_reg_conf *sharpnesstbl_4;
	uint16_t sharpnesstbl_size_4;
/* --------------------- saturation ------------------*/
	const struct hi707_i2c_reg_conf *saturationtbl_0;
	uint16_t saturationtbl_size_0;
	const struct hi707_i2c_reg_conf *saturationtbl_1;
	uint16_t saturationtbl_size_1;
	const struct hi707_i2c_reg_conf *saturationtbl_2;
	uint16_t saturationtbl_size_2;
	const struct hi707_i2c_reg_conf *saturationtbl_3;
	uint16_t saturationtbl_size_3;
	const struct hi707_i2c_reg_conf *saturationtbl_4;
	uint16_t saturationtbl_size_4;
/* ------------- contrast  ------------ */
	const struct hi707_i2c_reg_conf *contrasttbl_0;
	uint16_t contrasttbl_size_0;
	const struct hi707_i2c_reg_conf *contrasttbl_1;
	uint16_t contrasttbl_size_1;
	const struct hi707_i2c_reg_conf *contrasttbl_2;
	uint16_t contrasttbl_size_2;
	const struct hi707_i2c_reg_conf *contrasttbl_3;
	uint16_t contrasttbl_size_3;
	const struct hi707_i2c_reg_conf *contrasttbl_4;
	uint16_t contrasttbl_size_4;
/* ------------- effect -----------------*/
	const struct hi707_i2c_reg_conf *effecttbl_off;
	uint16_t effecttbl_size_off;
	const struct hi707_i2c_reg_conf *effecttbl_mono;
	uint16_t effecttbl_size_mono;
	const struct hi707_i2c_reg_conf *effecttbl_negative;
	uint16_t effecttbl_size_negative;
	const struct hi707_i2c_reg_conf *effecttbl_solarize;
	uint16_t effecttbl_size_solarize;
	const struct hi707_i2c_reg_conf *effecttbl_sepia;
	uint16_t effecttbl_size_sepia;
/* -------------- set mode --------------*/
	const struct hi707_i2c_reg_conf *previewtbl;
	uint16_t previewtbl_size;
	const struct hi707_i2c_reg_conf *snapshottbl;
	uint16_t snapshottbl_size;
	const struct hi707_i2c_reg_conf *rawsnapshottbl;
	uint16_t rawsnapshottbl_size;
	const struct hi707_i2c_reg_conf *releasetbl;
	uint16_t releasetbl_size;

};


static const struct hi707_i2c_reg_conf const cam_init_tbl[] =  {
{0x01, 0x71, BYTE_BYTE_LEN, 0},	// reset op.
{0x01, 0x73, BYTE_BYTE_LEN, 0},
{0x01, 0x71, BYTE_BYTE_LEN, 0},

///////////////////////////// Page 0	-  Image Size & Sync.
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x08, 0x0f, BYTE_BYTE_LEN, 0},                                     
{0x10, 0x00, BYTE_BYTE_LEN, 0},	// VDOCTL1 [5:4]subsample:1,1/2,1/4, [0]preview_en
{0x11, 0x93, BYTE_BYTE_LEN, 0},	// VDOCTL2 , 90 : FFR off, 94 : FFR on
{0x12, 0x00, BYTE_BYTE_LEN, 0},	// CLK_CTL
{0x14, 0x88, BYTE_BYTE_LEN, 0},	// [7]fix_frm_spd:prevent hounting, [3]fix_frm_opt:inc. exp.time

{0x20, 0x00, BYTE_BYTE_LEN, 0},	// WINROW
{0x21, 0x04, BYTE_BYTE_LEN, 0},	//  - VGA:04, QVGA,QQVGA:02
{0x22, 0x00, BYTE_BYTE_LEN, 0},	// WINCOL
{0x23, 0x04, BYTE_BYTE_LEN, 0},	//  - VGA,QVGA:04, QQVGA:02

{0x40, 0x00, BYTE_BYTE_LEN, 0},	// HBLANK 
{0x41, 0x90, BYTE_BYTE_LEN, 0},	//  - YUV422:0090, BAYER:0158
{0x42, 0x00, BYTE_BYTE_LEN, 0},	// VSYNCH
{0x43, 0x02, BYTE_BYTE_LEN, 0},	//  - YUV422:0002, BAYER:0014

{0x80, 0x2e, BYTE_BYTE_LEN, 0},	// BLCCTL
{0x81, 0x7e, BYTE_BYTE_LEN, 0},
{0x82, 0x90, BYTE_BYTE_LEN, 0},
{0x83, 0x30, BYTE_BYTE_LEN, 0},
{0x84, 0x2C, BYTE_BYTE_LEN, 0},
{0x85, 0x4b, BYTE_BYTE_LEN, 0}, 
{0x86, 0x01, BYTE_BYTE_LEN, 0},
{0x88, 0x47, BYTE_BYTE_LEN, 0},   //BLCLINE
{0x89, 0x48, BYTE_BYTE_LEN, 0},   //BLC hold
{0x90, 0x0b, BYTE_BYTE_LEN, 0},   //TIME_IN  12/100  _100318
{0x91, 0x0b, BYTE_BYTE_LEN, 0},   //TIME_OUT 12/100  _100318
{0x92, 0x48, BYTE_BYTE_LEN, 0},   //AG_IN
{0x93, 0x48, BYTE_BYTE_LEN, 0},   //AG_OUT
{0x98, 0x38, BYTE_BYTE_LEN, 0},
{0x99, 0x40, BYTE_BYTE_LEN, 0},   //40, BYTE_BYTE_LEN, 0},   //Out BLC LHC
{0xa0, 0x00, BYTE_BYTE_LEN, 0},   //Dark BLC
{0xa8, 0x40, BYTE_BYTE_LEN, 0},   //40, BYTE_BYTE_LEN, 0},   //Normal BLC LHC

{0xc0, 0x95, BYTE_BYTE_LEN, 0},	// PLL Mode                                    
{0xc1, 0x18, BYTE_BYTE_LEN, 0},                                     
{0xc2, 0x91, BYTE_BYTE_LEN, 0},	// [4]plldiv_en, [3:2]mipi4xclkdiv:bypass,1/2,1/4,1/8, [0]ispclkdiv:1/2,1/4
{0xc3, 0x00, BYTE_BYTE_LEN, 0},                                     
{0xc4, 0x01, BYTE_BYTE_LEN, 0},   

///////////////////////////// Page 2	-  Analog Circuit Control
{0x03, 0x02, BYTE_BYTE_LEN, 0},
{0x10, 0x00, BYTE_BYTE_LEN, 0},	// MODE_TEST
{0x11, 0x00, BYTE_BYTE_LEN, 0},	// MODE_DEAD_TEST
{0x13, 0x40, BYTE_BYTE_LEN, 0},	// MODE_ANA_TEST
{0x14, 0x04, BYTE_BYTE_LEN, 0},	// MODE_MEMORY

{0x18, 0x1c, BYTE_BYTE_LEN, 0},	// Analog mode
{0x19, 0x00, BYTE_BYTE_LEN, 0},	// [0]pmos_act
{0x1a, 0x00, BYTE_BYTE_LEN, 0}, 
{0x1b, 0x08, BYTE_BYTE_LEN, 0},

{0x1c, 0x9c, BYTE_BYTE_LEN, 0},	// DC-DC
{0x1d, 0x03, BYTE_BYTE_LEN, 0},

{0x20, 0x33, BYTE_BYTE_LEN, 0},	// PX bias
{0x21, 0x77, BYTE_BYTE_LEN, 0},	// ADC/ASP bias
{0x22, 0xa7, BYTE_BYTE_LEN, 0},	// Main bias
{0x23, 0x32, BYTE_BYTE_LEN, 0},	// Clamp 
{0x24, 0x33, BYTE_BYTE_LEN, 0},


{0x31, 0x99, BYTE_BYTE_LEN, 0},	// shared control
{0x32, 0x00, BYTE_BYTE_LEN, 0},
{0x33, 0x00, BYTE_BYTE_LEN, 0},
{0x34, 0x3c, BYTE_BYTE_LEN, 0},
{0x35, 0x0d, BYTE_BYTE_LEN, 0},
{0x3b, 0x80, BYTE_BYTE_LEN, 0},


                //timing control 1
{0x50, 0x21, BYTE_BYTE_LEN, 0},
{0x51, 0x1c, BYTE_BYTE_LEN, 0},
{0x52, 0xaa, BYTE_BYTE_LEN, 0},
{0x53, 0x5a, BYTE_BYTE_LEN, 0},
{0x54, 0x30, BYTE_BYTE_LEN, 0},
{0x55, 0x10, BYTE_BYTE_LEN, 0},
{0x56, 0x0c, BYTE_BYTE_LEN, 0},
{0x59, 0x0F, BYTE_BYTE_LEN, 0},
		// Row Timing Control
{0x60, 0x34, BYTE_BYTE_LEN, 0},	// addr_en - Exp.
{0x61, 0x3a, BYTE_BYTE_LEN, 0},
{0x62, 0x34, BYTE_BYTE_LEN, 0},	// rx1
{0x63, 0x39, BYTE_BYTE_LEN, 0},
{0x64, 0x34, BYTE_BYTE_LEN, 0},	// rx2
{0x65, 0x39, BYTE_BYTE_LEN, 0},

{0x72, 0x35, BYTE_BYTE_LEN, 0},	// tx1
{0x73, 0x38, BYTE_BYTE_LEN, 0},
{0x74, 0x35, BYTE_BYTE_LEN, 0},	// tx2
{0x75, 0x38, BYTE_BYTE_LEN, 0},
{0x80, 0x02, BYTE_BYTE_LEN, 0},	// addr_en - Read.
{0x81, 0x2e, BYTE_BYTE_LEN, 0},
{0x82, 0x0d, BYTE_BYTE_LEN, 0},	// rx1
{0x83, 0x10, BYTE_BYTE_LEN, 0},
{0x84, 0x0d, BYTE_BYTE_LEN, 0},	// rx2
{0x85, 0x10, BYTE_BYTE_LEN, 0},
{0x92, 0x1d, BYTE_BYTE_LEN, 0},	// tx1
{0x93, 0x20, BYTE_BYTE_LEN, 0},
{0x94, 0x1d, BYTE_BYTE_LEN, 0},	// tx2
{0x95, 0x20, BYTE_BYTE_LEN, 0},
{0xa0, 0x03, BYTE_BYTE_LEN, 0},	// sx
{0xa1, 0x2d, BYTE_BYTE_LEN, 0},
{0xa4, 0x2d, BYTE_BYTE_LEN, 0},	// sxb
{0xa5, 0x03, BYTE_BYTE_LEN, 0},
{0xa8, 0x12, BYTE_BYTE_LEN, 0},	// wrst
{0xa9, 0x1b, BYTE_BYTE_LEN, 0},
{0xaa, 0x22, BYTE_BYTE_LEN, 0},	// wsig
{0xab, 0x2b, BYTE_BYTE_LEN, 0},
{0xac, 0x10, BYTE_BYTE_LEN, 0},	// rx_off_rst
{0xad, 0x0e, BYTE_BYTE_LEN, 0},	// tx_off_rst
{0xb8, 0x33, BYTE_BYTE_LEN, 0},	// rx pwr - exp.
{0xb9, 0x35, BYTE_BYTE_LEN, 0},
{0xbc, 0x0c, BYTE_BYTE_LEN, 0},	// rx pwr - read
{0xbd, 0x0e, BYTE_BYTE_LEN, 0},
{0xc0, 0x3a, BYTE_BYTE_LEN, 0},	// addr_en1 - Fixed Exp.
{0xc1, 0x3f, BYTE_BYTE_LEN, 0},
{0xc2, 0x3a, BYTE_BYTE_LEN, 0},	// addr_en2
{0xc3, 0x3f, BYTE_BYTE_LEN, 0},
{0xc4, 0x3a, BYTE_BYTE_LEN, 0},	// sx1
{0xc5, 0x3e, BYTE_BYTE_LEN, 0},
{0xc6, 0x3a, BYTE_BYTE_LEN, 0},	// sx2
{0xc7, 0x3e, BYTE_BYTE_LEN, 0},
{0xc8, 0x3a, BYTE_BYTE_LEN, 0},	// rx1
{0xc9, 0x3e, BYTE_BYTE_LEN, 0},
{0xca, 0x3a, BYTE_BYTE_LEN, 0},	// rx2
{0xcb, 0x3e, BYTE_BYTE_LEN, 0},
{0xcc, 0x3b, BYTE_BYTE_LEN, 0},	// tx1
{0xcd, 0x3d, BYTE_BYTE_LEN, 0},
{0xce, 0x3b, BYTE_BYTE_LEN, 0},	// tx2
{0xcf, 0x3d, BYTE_BYTE_LEN, 0},

{0xd0, 0x33, BYTE_BYTE_LEN, 0},	// Exposure domain valid
{0xd1, 0x3f, BYTE_BYTE_LEN, 0},
{0x2b, 0x40, BYTE_BYTE_LEN, 0},	// Fixed frame counter end
{0x2d, 0x32, BYTE_BYTE_LEN, 0},	// Fixed frame counter start

                // Page 10 - Format, Image Effect
{0x03, 0x10, BYTE_BYTE_LEN, 0},
{0x10, 0x03, BYTE_BYTE_LEN, 0},	//ISPCTL - [7:4]0:YUV322, 6:BAYER, [1:0]VYUY, UYVY, YVYU, YUYV
{0x11, 0x43, BYTE_BYTE_LEN, 0},	// - [0x1010:1011]YUV422:0343, BAYER:6000
{0x12, 0x30, BYTE_BYTE_LEN, 0},   //Y offet, dy offseet enable
{0x40, 0x80, BYTE_BYTE_LEN, 0},
{0x41, 0x10, BYTE_BYTE_LEN, 0},   //00 DYOFS  00->10  _100318
{0x48, 0x84, BYTE_BYTE_LEN, 0},   //Contrast  88->84  _100318
{0x50, 0x48, BYTE_BYTE_LEN, 0},   //AGBRT
{0x60, 0x7f, BYTE_BYTE_LEN, 0},
{0x61, 0x00, BYTE_BYTE_LEN, 0},   //Use default
{0x62, 0xa0, BYTE_BYTE_LEN, 0},   //SATB  (1.4x)
{0x63, 0x90, BYTE_BYTE_LEN, 0},   //SATR  (1.2x)
{0x64, 0x48, BYTE_BYTE_LEN, 0},   //AGSAT
{0x66, 0x90, BYTE_BYTE_LEN, 0},   //wht_th2
{0x67, 0x36, BYTE_BYTE_LEN, 0},   //wht_gain  Dark (0.4x), Normal (0.75x)

///////////////////////////// Page 11	-  D,Z,W-LPF
{0x03, 0x11, BYTE_BYTE_LEN, 0},
{0x10, 0x25, BYTE_BYTE_LEN, 0},	//LPF_CTL1 //0x01
{0x11, 0x1f, BYTE_BYTE_LEN, 0},	//Test Setting
{0x20, 0x00, BYTE_BYTE_LEN, 0},	//LPF_AUTO_CTL
{0x21, 0x38, BYTE_BYTE_LEN, 0},	//LPF_PGA_TH
{0x23, 0x0a, BYTE_BYTE_LEN, 0},	//LPF_TIME_TH
{0x60, 0x10, BYTE_BYTE_LEN, 0},	//ZARA_SIGMA_TH //40->10
{0x61, 0x82, BYTE_BYTE_LEN, 0},
{0x62, 0x00, BYTE_BYTE_LEN, 0},	//ZARA_HLVL_CTL
{0x63, 0x83, BYTE_BYTE_LEN, 0},	//ZARA_LLVL_CTL
{0x64, 0x83, BYTE_BYTE_LEN, 0},	//ZARA_DY_CTL
{0x67, 0xF0, BYTE_BYTE_LEN, 0},	//*** Change 100402     //Dark
{0x68, 0x30, BYTE_BYTE_LEN, 0},	//*** Change 100402     //Middle
{0x69, 0x10, BYTE_BYTE_LEN, 0},	//High

		// Page 12 - 2D : YC1D,YC2D,DPC,Demosaic
{0x03, 0x12, BYTE_BYTE_LEN, 0},
{0x40, 0xe9, BYTE_BYTE_LEN, 0},	//YC2D_LPF_CTL1
{0x41, 0x09, BYTE_BYTE_LEN, 0},	//YC2D_LPF_CTL2
{0x50, 0x18, BYTE_BYTE_LEN, 0},	//Test Setting
{0x51, 0x24, BYTE_BYTE_LEN, 0},	//Test Setting
{0x70, 0x1f, BYTE_BYTE_LEN, 0},	//GBGR_CTL1 //0x1f
{0x71, 0x00, BYTE_BYTE_LEN, 0},	//Test Setting
{0x72, 0x00, BYTE_BYTE_LEN, 0},	//Test Setting
{0x73, 0x00, BYTE_BYTE_LEN, 0},	//Test Setting
{0x74, 0x10, BYTE_BYTE_LEN, 0},	//GBGR_G_UNIT_TH
{0x75, 0x10, BYTE_BYTE_LEN, 0},	//GBGR_RB_UNIT_TH
{0x76, 0x20, BYTE_BYTE_LEN, 0},	//GBGR_EDGE_TH
{0x77, 0x80, BYTE_BYTE_LEN, 0},	//GBGR_HLVL_TH
{0x78, 0x88, BYTE_BYTE_LEN, 0},	//GBGR_HLVL_COMP
{0x79, 0x18, BYTE_BYTE_LEN, 0},	//Test Setting
{0x90, 0x3d, BYTE_BYTE_LEN, 0},
{0x91, 0x34, BYTE_BYTE_LEN, 0},
{0x99, 0x28, BYTE_BYTE_LEN, 0},


{0x9c, 0x05, BYTE_BYTE_LEN, 0},
{0x9d, 0x08, BYTE_BYTE_LEN, 0},
{0x9e, 0x28, BYTE_BYTE_LEN, 0},
{0x9f, 0x28, BYTE_BYTE_LEN, 0},
{0xb0, 0x7d, BYTE_BYTE_LEN, 0},   //dpc
{0xb5, 0x44, BYTE_BYTE_LEN, 0},
{0xb6, 0x82, BYTE_BYTE_LEN, 0},
{0xb7, 0x52, BYTE_BYTE_LEN, 0},
{0xb8, 0x44, BYTE_BYTE_LEN, 0},
{0xb9, 0x15, BYTE_BYTE_LEN, 0},
	
	// Page 13 - Sharpness
{0x03, 0x13, BYTE_BYTE_LEN, 0},
{0x10, 0x01, BYTE_BYTE_LEN, 0},	
{0x11, 0x89, BYTE_BYTE_LEN, 0},	
{0x12, 0x14, BYTE_BYTE_LEN, 0},	
{0x13, 0x19, BYTE_BYTE_LEN, 0},	
{0x14, 0x08, BYTE_BYTE_LEN, 0},	//Test Setting
{0x20, 0x06, BYTE_BYTE_LEN, 0},	//SHARP_Negative
{0x21, 0x03, BYTE_BYTE_LEN, 0},	//SHARP_Positive
{0x23, 0x30, BYTE_BYTE_LEN, 0},	//SHARP_DY_CTL
{0x24, 0x33, BYTE_BYTE_LEN, 0},	//40->33
{0x25, 0x08, BYTE_BYTE_LEN, 0},	//SHARP_PGA_TH
{0x26, 0x18, BYTE_BYTE_LEN, 0},	//Test Setting
{0x27, 0x00, BYTE_BYTE_LEN, 0},	//Test Setting
{0x28, 0x08, BYTE_BYTE_LEN, 0},	//Test Setting
{0x29, 0x50, BYTE_BYTE_LEN, 0},	//AG_TH
{0x2a, 0xe0, BYTE_BYTE_LEN, 0},	//region ratio
{0x2b, 0x10, BYTE_BYTE_LEN, 0},	//Test Setting
{0x2c, 0x28, BYTE_BYTE_LEN, 0},	//Test Setting
{0x2d, 0x40, BYTE_BYTE_LEN, 0},	//Test Setting
{0x2e, 0x00, BYTE_BYTE_LEN, 0},	//Test Setting
{0x2f, 0x00, BYTE_BYTE_LEN, 0},	//Test Setting
{0x30, 0x11, BYTE_BYTE_LEN, 0},	//Test Setting
{0x80, 0x03, BYTE_BYTE_LEN, 0},	//SHARP2D_CTL
{0x81, 0x07, BYTE_BYTE_LEN, 0},	//Test Setting
{0x90, 0x04, BYTE_BYTE_LEN, 0},	//SHARP2D_SLOPE
{0x91, 0x02, BYTE_BYTE_LEN, 0},	//SHARP2D_DIFF_CTL
{0x92, 0x00, BYTE_BYTE_LEN, 0},	//SHARP2D_HI_CLIP
{0x93, 0x20, BYTE_BYTE_LEN, 0},	//SHARP2D_DY_CTL
{0x94, 0x42, BYTE_BYTE_LEN, 0},	//Test Setting
{0x95, 0x60, BYTE_BYTE_LEN, 0},	//Test Setting

		// Page 14 - Lens Shading Correction
{0x03, 0x14, BYTE_BYTE_LEN, 0},
{0x10, 0x01, BYTE_BYTE_LEN, 0},
{0x20, 0x90, BYTE_BYTE_LEN, 0},   //60, BYTE_BYTE_LEN, 0},   //XCEN LHC
{0x21, 0x80, BYTE_BYTE_LEN, 0},   //YCEN
{0x22, 0x7a, BYTE_BYTE_LEN, 0},    //7a->7e->7a//66, BYTE_BYTE_LEN, 0},   //76, 34, 2b   R
{0x23, 0x61, BYTE_BYTE_LEN, 0},    //6e->66->61//50, BYTE_BYTE_LEN, 0},   //4b, 15, 0d   G
{0x24, 0x63, BYTE_BYTE_LEN, 0},    //6e->63->63//44, BYTE_BYTE_LEN, 0},   //3b, 10, 0b   B


{0x27, 0xa0, BYTE_BYTE_LEN, 0},
{0x28, 0x60, BYTE_BYTE_LEN, 0},
{0x29, 0xa0, BYTE_BYTE_LEN, 0},
{0x2a, 0x60, BYTE_BYTE_LEN, 0},
{0x2b, 0xa0, BYTE_BYTE_LEN, 0},
{0x2c, 0x60, BYTE_BYTE_LEN, 0},


		// Page 15 - Color Matrix
{0x03, 0x15, BYTE_BYTE_LEN, 0}, 
{0x10, 0x01, BYTE_BYTE_LEN, 0},  //03, BYTE_BYTE_LEN, 0},
{0x14, 0x3c, BYTE_BYTE_LEN, 0},
{0x15, 0x3c, BYTE_BYTE_LEN, 0},
{0x16, 0x2c, BYTE_BYTE_LEN, 0},
{0x17, 0x2f, BYTE_BYTE_LEN, 0},

{0x30, 0xc1, BYTE_BYTE_LEN, 0},                     
{0x31, 0x50, BYTE_BYTE_LEN, 0},
{0x32, 0x0f, BYTE_BYTE_LEN, 0},
{0x33, 0x1e, BYTE_BYTE_LEN, 0},
{0x34, 0xa9, BYTE_BYTE_LEN, 0},
{0x35, 0x0b, BYTE_BYTE_LEN, 0},
{0x36, 0x15, BYTE_BYTE_LEN, 0},
{0x37, 0x45, BYTE_BYTE_LEN, 0},
{0x38, 0xda, BYTE_BYTE_LEN, 0},


{0x40, 0x87, BYTE_BYTE_LEN, 0},
{0x41, 0x18, BYTE_BYTE_LEN, 0},
{0x42, 0x91, BYTE_BYTE_LEN, 0},
{0x43, 0x94, BYTE_BYTE_LEN, 0},
{0x44, 0x9f, BYTE_BYTE_LEN, 0},
{0x45, 0x33, BYTE_BYTE_LEN, 0},
{0x46, 0x00, BYTE_BYTE_LEN, 0},
{0x47, 0x94, BYTE_BYTE_LEN, 0},
{0x48, 0x14, BYTE_BYTE_LEN, 0},

              // Page16 - Gamma Correction
{0x03,0x16, BYTE_BYTE_LEN, 0},
{0x30,0x00, BYTE_BYTE_LEN, 0},
{0x31,0x02, BYTE_BYTE_LEN, 0},
{0x32,0x0a, BYTE_BYTE_LEN, 0},
{0x33,0x2a, BYTE_BYTE_LEN, 0},
{0x34,0x5c, BYTE_BYTE_LEN, 0},
{0x35,0x79, BYTE_BYTE_LEN, 0},
{0x36,0x95, BYTE_BYTE_LEN, 0},
{0x37,0xa4, BYTE_BYTE_LEN, 0},
{0x38,0xb1, BYTE_BYTE_LEN, 0},
{0x39,0xbd, BYTE_BYTE_LEN, 0},
{0x3a,0xc8, BYTE_BYTE_LEN, 0},
{0x3b,0xd9, BYTE_BYTE_LEN, 0},
{0x3c,0xe8, BYTE_BYTE_LEN, 0},
{0x3d,0xf5, BYTE_BYTE_LEN, 0},
{0x3e,0xff, BYTE_BYTE_LEN, 0},
                // Page 17 - Findband
{0x03, 0x17, BYTE_BYTE_LEN, 0},
{0xc4, 0x3c, BYTE_BYTE_LEN, 0},
{0xc5, 0x32, BYTE_BYTE_LEN, 0},

///////////////////////////// Page 20	- Auto Exposure	
{0x03, 0x20, BYTE_BYTE_LEN, 0},
{0x10, 0x0c, BYTE_BYTE_LEN, 0},	// AECTL
{0x11, 0x04, BYTE_BYTE_LEN, 0},
{0x18, 0x30, BYTE_BYTE_LEN, 0},
{0x20, 0x01, BYTE_BYTE_LEN, 0},	// FrameCTL
{0x28, 0x27, BYTE_BYTE_LEN, 0},	// FineCTL
{0x29, 0xa5, BYTE_BYTE_LEN, 0},
{0x2a, 0xf0, BYTE_BYTE_LEN, 0},
{0x2b, 0x34, BYTE_BYTE_LEN, 0},
       
{0x30, 0xf8, BYTE_BYTE_LEN, 0},	// AE_Escape_CTL
{0x39, 0x22, BYTE_BYTE_LEN, 0},
{0x3a, 0xde, BYTE_BYTE_LEN, 0},
{0x3b, 0x23, BYTE_BYTE_LEN, 0},
{0x3c, 0xde, BYTE_BYTE_LEN, 0},

{0x60, 0x11, BYTE_BYTE_LEN, 0},   //95, BYTE_BYTE_LEN, 0},	// AE_WGT
{0x68, 0x3c, BYTE_BYTE_LEN, 0},	// AE_CEN
{0x69, 0x64, BYTE_BYTE_LEN, 0},
{0x6A, 0x28, BYTE_BYTE_LEN, 0},
{0x6B, 0xc8, BYTE_BYTE_LEN, 0},
{0x70, 0x3a, BYTE_BYTE_LEN, 0},   //42, BYTE_BYTE_LEN, 0},	// Y Target LHC
{0x76, 0x88, BYTE_BYTE_LEN, 0},   //22, BYTE_BYTE_LEN, 0},	// Unlock bnd1
{0x77, 0xfe, BYTE_BYTE_LEN, 0},   //02, BYTE_BYTE_LEN, 0},	// Unlock bnd2
{0x78, 0x22, BYTE_BYTE_LEN, 0},   //12, BYTE_BYTE_LEN, 0},	// Yth 1
{0x79, 0x26, BYTE_BYTE_LEN, 0},	// Yth 2
{0x7a, 0x23, BYTE_BYTE_LEN, 0},	// Yth 3
{0x7c, 0x1c, BYTE_BYTE_LEN, 0},	// Yth 2
{0x7d, 0x22, BYTE_BYTE_LEN, 0},	// Yth 4

{0x98, 0x8c, BYTE_BYTE_LEN, 0},	// BRIGHT_MEASURE_TH
{0x99, 0x23, BYTE_BYTE_LEN, 0},
{0xa0, 0x02, BYTE_BYTE_LEN, 0},	// EXP_MAX100
{0xa1, 0x49, BYTE_BYTE_LEN, 0},
{0xa2, 0xf0, BYTE_BYTE_LEN, 0},

{0xb0, 0x10, BYTE_BYTE_LEN, 0},	// Analog Gain = (0x021A[3]+1)x(0.5+0x20B0/32)
{0xb1, 0x14, BYTE_BYTE_LEN, 0},
{0xb2, 0x80, BYTE_BYTE_LEN, 0},   //50, BYTE_BYTE_LEN, 0},
{0xb4, 0x14, BYTE_BYTE_LEN, 0},
{0xb5, 0x38, BYTE_BYTE_LEN, 0},
{0xb6, 0x26, BYTE_BYTE_LEN, 0},
{0xb7, 0x20, BYTE_BYTE_LEN, 0},
{0xb8, 0x1d, BYTE_BYTE_LEN, 0},
{0xb9, 0x1b, BYTE_BYTE_LEN, 0},
{0xba, 0x1a, BYTE_BYTE_LEN, 0},
{0xbb, 0x19, BYTE_BYTE_LEN, 0},
{0xbc, 0x19, BYTE_BYTE_LEN, 0},
{0xbd, 0x18, BYTE_BYTE_LEN, 0},

{0xc0, 0x1a, BYTE_BYTE_LEN, 0},	// PGA_sky
{0xc3, 0x48, BYTE_BYTE_LEN, 0},	// PGA_dark_on
{0xc4, 0x48, BYTE_BYTE_LEN, 0},	// PGA_dark_off

                //Page 22 AWB
{0x03, 0x22, BYTE_BYTE_LEN, 0},
{0x10, 0xe2, BYTE_BYTE_LEN, 0},
{0x11, 0x26, BYTE_BYTE_LEN, 0},   //2E, BYTE_BYTE_LEN, 0}, //26, BYTE_BYTE_LEN, 0},
{0x21, 0x40, BYTE_BYTE_LEN, 0},
{0x30, 0x80, BYTE_BYTE_LEN, 0},
{0x31, 0x80, BYTE_BYTE_LEN, 0},
{0x38, 0x12, BYTE_BYTE_LEN, 0},
{0x39, 0x33, BYTE_BYTE_LEN, 0},
{0x40, 0xf0, BYTE_BYTE_LEN, 0},
{0x41, 0x44, BYTE_BYTE_LEN, 0},   //CDIFF
{0x42, 0x33, BYTE_BYTE_LEN, 0},   //CSUM
{0x43, 0xf3, BYTE_BYTE_LEN, 0},
{0x44, 0x55, BYTE_BYTE_LEN, 0},
{0x45, 0x44, BYTE_BYTE_LEN, 0},
{0x46, 0x02, BYTE_BYTE_LEN, 0},

{0x80, 0x40, BYTE_BYTE_LEN, 0}, //45, BYTE_BYTE_LEN, 0},
{0x81, 0x20, BYTE_BYTE_LEN, 0},
{0x82, 0x38, BYTE_BYTE_LEN, 0}, //48, BYTE_BYTE_LEN, 0},

{0x83, 0x52, BYTE_BYTE_LEN, 0}, //RMAX Default : 50 -> 48 -> 52 
{0x84, 0x08, BYTE_BYTE_LEN, 0}, //1b, BYTE_BYTE_LEN, 0}, //RMIN Default : 20
{0x85, 0x65, BYTE_BYTE_LEN, 0}, //50, BYTE_BYTE_LEN, 0}, //BMAX Default : 50, 5a -> 58 -> 55
{0x86, 0x25, BYTE_BYTE_LEN, 0}, //BMIN Default : 20
{0x87, 0x4d, BYTE_BYTE_LEN, 0}, //RMAXB Default : 50, 4d
{0x88, 0x38, BYTE_BYTE_LEN, 0}, //RMINB Default : 3e, 45 --> 42
{0x89, 0x3e, BYTE_BYTE_LEN, 0}, //BMAXB Default : 2e, 2d --> 30
{0x8a, 0x29, BYTE_BYTE_LEN, 0}, //BMINB Default : 20, 22 --> 26 --> 29
{0x8b, 0x02, BYTE_BYTE_LEN, 0}, //OUT TH
{0x8d, 0x22, BYTE_BYTE_LEN, 0},
{0x8e, 0x71, BYTE_BYTE_LEN, 0},
{0x8f, 0x63, BYTE_BYTE_LEN, 0},
{0x90, 0x60, BYTE_BYTE_LEN, 0},
{0x91, 0x5c, BYTE_BYTE_LEN, 0},
{0x92, 0x56, BYTE_BYTE_LEN, 0},
{0x93, 0x52, BYTE_BYTE_LEN, 0},
{0x94, 0x4c, BYTE_BYTE_LEN, 0},
{0x95, 0x36, BYTE_BYTE_LEN, 0},
{0x96, 0x31, BYTE_BYTE_LEN, 0},
{0x97, 0x2e, BYTE_BYTE_LEN, 0},
{0x98, 0x2a, BYTE_BYTE_LEN, 0},
{0x99, 0x29, BYTE_BYTE_LEN, 0},
{0x9a, 0x26, BYTE_BYTE_LEN, 0},
{0x9b, 0x09, BYTE_BYTE_LEN, 0},

////////////////////////////clock calculator//////////////////////////////

{0x03, 0x00, BYTE_BYTE_LEN, 0}, //Page 0
{0x40, 0x00, BYTE_BYTE_LEN, 0}, //Hblank 252
{0x41, 0xfc, BYTE_BYTE_LEN, 0}, 
{0x42, 0x00, BYTE_BYTE_LEN, 0}, //Vblank 28
{0x43, 0x1c, BYTE_BYTE_LEN, 0}, 

{0x03, 0x20, BYTE_BYTE_LEN, 0}, //Page 20
{0x83, 0x00, BYTE_BYTE_LEN, 0}, //EXP Normal 30.00 fps 
{0x84, 0xc3, BYTE_BYTE_LEN, 0}, 
{0x85, 0x14, BYTE_BYTE_LEN, 0}, 
{0x86, 0x00, BYTE_BYTE_LEN, 0}, //EXPMin 6607.93 fps
{0x87, 0xe3, BYTE_BYTE_LEN, 0}, 


{0x88, 0x01, BYTE_BYTE_LEN, 0}, //EXP Max 15.00 fps 
{0x89, 0x86, BYTE_BYTE_LEN, 0}, 
{0x8a, 0x28, BYTE_BYTE_LEN, 0}, 
{0x8B, 0x3a, BYTE_BYTE_LEN, 0}, //EXP100 
{0x8C, 0x86, BYTE_BYTE_LEN, 0}, 
{0x8D, 0x30, BYTE_BYTE_LEN, 0}, //EXP120 
{0x8E, 0xc5, BYTE_BYTE_LEN, 0}, 
{0x9c, 0x05, BYTE_BYTE_LEN, 0}, //EXP Limit 1101.32 fps 
{0x9d, 0x52, BYTE_BYTE_LEN, 0}, 
{0x9e, 0x00, BYTE_BYTE_LEN, 0}, //EXP Unit 
{0x9f, 0xe3, BYTE_BYTE_LEN, 0}, 

{0x03, 0x17, BYTE_BYTE_LEN, 0}, //Page 17 
{0xC4, 0x42, BYTE_BYTE_LEN, 0}, //FLK200 
{0xC5, 0x37, BYTE_BYTE_LEN, 0}, //FLK240 

//AntiBand Unlock
{0x03, 0x20, BYTE_BYTE_LEN, 0}, //Page 20 
{0x2b, 0x34, BYTE_BYTE_LEN, 0}, 
{0x30, 0x78, BYTE_BYTE_LEN, 0}, 

//BLC 
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //PAGE 0
{0x90, 0x06, BYTE_BYTE_LEN, 0}, //BLC_TIME_TH_ON
{0x91, 0x06, BYTE_BYTE_LEN, 0}, //BLC_TIME_TH_OFF 
{0x92, 0x48, BYTE_BYTE_LEN, 0}, //BLC_AG_TH_ON
{0x93, 0x40, BYTE_BYTE_LEN, 0}, //BLC_AG_TH_OFF
                  
///////////////////////////// Page 48	- MIPI
{0x03, 0x48, BYTE_BYTE_LEN, 0},                                     
{0x10, 0x05, BYTE_BYTE_LEN, 0},                                     
{0x11, 0x08, BYTE_BYTE_LEN, 0},	// async_fifo off                          
{0x16, 0xc4, BYTE_BYTE_LEN, 0},                                     
{0x1a, 0x00, BYTE_BYTE_LEN, 0},                                     
{0x1c, 0x02, BYTE_BYTE_LEN, 0},                                     
{0x1d, 0x04, BYTE_BYTE_LEN, 0},                                     
{0x1e, 0x07, BYTE_BYTE_LEN, 0},                                     
{0x1f, 0x06, BYTE_BYTE_LEN, 0},	// HS_TRAIL_TIME - VGA,QVGA:06, QQVGA:07
{0x20, 0x00, BYTE_BYTE_LEN, 0},                                     
{0x30, 0x05, BYTE_BYTE_LEN, 0},	// Long Packet Word Count - VGA:05, QVGA:02, QQVGA:01 
{0x31, 0x00, BYTE_BYTE_LEN, 0},                                     
{0x32, 0x06, BYTE_BYTE_LEN, 0},	// HS_CLK_ZERO_TIME
{0x34, 0x02, BYTE_BYTE_LEN, 0},	// CLK_POST_TIME - VGA:02, QVGA,QQVGA:01
{0x35, 0x03, BYTE_BYTE_LEN, 0},                                     
{0x36, 0x01, BYTE_BYTE_LEN, 0},                                     
{0x37, 0x03, BYTE_BYTE_LEN, 0},
{0x38, 0x02, BYTE_BYTE_LEN, 0},
{0x39, 0x4a, BYTE_BYTE_LEN, 0},                                     
{0x3c, 0x00, BYTE_BYTE_LEN, 0},                                     
{0x3d, 0xfa, BYTE_BYTE_LEN, 0},                                     
{0x3f, 0x10, BYTE_BYTE_LEN, 0},                                     
{0x40, 0x00, BYTE_BYTE_LEN, 0},                                     
{0x41, 0x20, BYTE_BYTE_LEN, 0},                                     
{0x42, 0x00, BYTE_BYTE_LEN, 0},                                     


//{0x10, 0x05, BYTE_BYTE_LEN, 0},                                     
//{0x11, 0x08, BYTE_BYTE_LEN, 0},                                     
//{0x1a, 0x00, BYTE_BYTE_LEN, 0},                                     
//{0x1f, 0x06, BYTE_BYTE_LEN, 0},                                     
//{0x24, 0x2a, BYTE_BYTE_LEN, 0},                                     
//{0x30, 0x02, BYTE_BYTE_LEN, 0},                                     


/////////////////////////////
{0x03, 0x22, BYTE_BYTE_LEN, 0},	// AWB on
{0x10, 0xfb, BYTE_BYTE_LEN, 0},
{0x03, 0x20, BYTE_BYTE_LEN, 0},	// AE on
{0x10, 0xCC, BYTE_BYTE_LEN, 0},   //9c, BYTE_BYTE_LEN, 0},
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy1
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy2
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy3
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy4
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy5
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy6
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy7
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy8
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy9
{0x03, 0x00, BYTE_BYTE_LEN, 0}, //dummy10
{0x03, 0x00, BYTE_BYTE_LEN, 0},
{0x01, 0xf0, BYTE_BYTE_LEN, 0},	// Sensor on


/////////////////////////////
// Page 48 MIPI /////////////
/////////////////////////////                                                     
                                                     
{0x03, 0x48, BYTE_BYTE_LEN, 0},                                     
                                                     
{0x10, 0x05, BYTE_BYTE_LEN, 0},                                     
{0x11, 0x88, BYTE_BYTE_LEN, 0}, 
{0x12, 0x00, BYTE_BYTE_LEN, 0},                                    
                                                     
{0x41, 0x20, BYTE_BYTE_LEN, 0},                                     
{0x42, 0x00, BYTE_BYTE_LEN, 0},                                     
{0x3c, 0x00, BYTE_BYTE_LEN, 0},                                     
{0x3d, 0xfa, BYTE_BYTE_LEN, 0},                                     
                                                     
{0x16, 0xc4, BYTE_BYTE_LEN, 0},                                     
                                                     
{0x35, 0x03, BYTE_BYTE_LEN, 0},                                     
{0x1f, 0x06, BYTE_BYTE_LEN, 0},                                     
{0x1a, 0x00, BYTE_BYTE_LEN, 0},                                     
{0x34, 0x02, BYTE_BYTE_LEN, 0},                                     
{0x1c, 0x02, BYTE_BYTE_LEN, 0},                                     
{0x1d, 0x04, BYTE_BYTE_LEN, 0},                                     
{0x36, 0x01, BYTE_BYTE_LEN, 0},                                     
{0x37, 0x03, BYTE_BYTE_LEN, 0},  
{0x38, 0x00, BYTE_BYTE_LEN, 0},
{0x39, 0x4a, BYTE_BYTE_LEN, 0},                                   
                                                     
{0x3f, 0x10, BYTE_BYTE_LEN, 0},                                     
{0x40, 0x00, BYTE_BYTE_LEN, 0},                                     
                                                     
                                                     
{0x1e, 0x07, BYTE_BYTE_LEN, 0},                                     
{0x32, 0x06, BYTE_BYTE_LEN, 0},                                     
{0x20, 0x00, BYTE_BYTE_LEN, 0},                                     
                                                     
{0x31, 0x00, BYTE_BYTE_LEN, 0},                                     
{0x30, 0x05, BYTE_BYTE_LEN, 0},                                     
                                                                                                   
//  {0x50, 0x81, BYTE_BYTE_LEN, 0},                                     
                                                                                          
{0x03, 0x22, BYTE_BYTE_LEN, 0},
{0x10, 0xfb, BYTE_BYTE_LEN, 0},

{0x03, 0x20, BYTE_BYTE_LEN, 0},
{0x10, 0x9c, BYTE_BYTE_LEN, 0},

{0x01, 0x70, BYTE_BYTE_LEN, 0},

// Page 48	- MIPI
{0x03, 0x48, BYTE_BYTE_LEN, 0},
{0x11, 0x08, BYTE_BYTE_LEN, 0},

#if 0
END
[END]
[USERSET_1]
DISP_NAME = "preview"
DISP_WIDTH = 640
DISP_HEIGHT = 480
DISP_FORMAT = YUV422
DISP_DATAORDER = YUYV
{0x03, 0x22, BYTE_BYTE_LEN, 0},
{0x10, 0xfb, BYTE_BYTE_LEN, 0},

{0x03, 0x20, BYTE_BYTE_LEN, 0},
{0x10, 0x9c, BYTE_BYTE_LEN, 0},

END
[END]
[USERSET_2]
DISP_NAME = "capture"
DISP_WIDTH = 640
DISP_HEIGHT = 480
DISP_FORMAT = YUV422
DISP_DATAORDER = YUYV
{0x03, 0x22, BYTE_BYTE_LEN, 0},
{0x10, 0x7b, BYTE_BYTE_LEN, 0},

{0x03, 0x20, BYTE_BYTE_LEN, 0},
{0x10, 0x1c, BYTE_BYTE_LEN, 0},

END
[END]
#endif

};




static const struct hi707_i2c_reg_conf const cam_set_wb_tbl_daylight[] = {};

static const struct hi707_i2c_reg_conf const cam_set_wb_tbl_auto[] = {};

static const struct hi707_i2c_reg_conf const cam_set_wb_tbl_cloudy[] = {};

static const struct hi707_i2c_reg_conf const cam_set_wb_tbl_flourescent[] = {};

static const struct hi707_i2c_reg_conf const cam_set_wb_tbl_incandescent[] = {};
/* -------- end of wb   ------------------*/

/* -------- start of iso ------------------ */
static const struct hi707_i2c_reg_conf const cam_set_iso_tbl_auto[] = {
//ISO Auto
};
static const struct hi707_i2c_reg_conf const cam_set_iso_tbl_100[] = {
//ISO 100
};
static const struct hi707_i2c_reg_conf const cam_set_iso_tbl_200[] = {
//ISO 200
};
static const struct hi707_i2c_reg_conf const cam_set_iso_tbl_400[] = {
//ISO 400
};
static const struct hi707_i2c_reg_conf const cam_set_iso_tbl_800[] = {
};
/* -------- end of iso -------------------*/

/* -------- start of exposure  ------------------ */
static const struct hi707_i2c_reg_conf const cam_set_exposure_tbl_averg[] = {};
static const struct hi707_i2c_reg_conf const cam_set_exposure_tbl_center[] = {};
static const struct hi707_i2c_reg_conf const cam_set_exposure_tbl_spot[] = {};
/* -------- end of exposure ------------------ */

/* -------- start of brightness ------------------ */
static const struct hi707_i2c_reg_conf const cam_set_brightness_tbl_0[] = {};
static const struct hi707_i2c_reg_conf const cam_set_brightness_tbl_1[] = {};
static const struct hi707_i2c_reg_conf const cam_set_brightness_tbl_2[] = {};
static const struct hi707_i2c_reg_conf const cam_set_brightness_tbl_3[] = {};
static const struct hi707_i2c_reg_conf const cam_set_brightness_tbl_4[] = {};
/* -------- end of brightness ------------------ */

/* -------- start of saturation ------------------ */
static const struct hi707_i2c_reg_conf const cam_set_saturation_tbl_0[] = {};
static const struct hi707_i2c_reg_conf const cam_set_saturation_tbl_1[] = {};
static const struct hi707_i2c_reg_conf const cam_set_saturation_tbl_2[] = {};
static const struct hi707_i2c_reg_conf const cam_set_saturation_tbl_3[] = {};
static const struct hi707_i2c_reg_conf const cam_set_saturation_tbl_4[] = {};
/* -------- end of saturation ------------------ */

/* -------- start of sharpness ------------------ */
static const struct hi707_i2c_reg_conf const cam_set_sharpness_tbl_0[] = {};
static const struct hi707_i2c_reg_conf const cam_set_sharpness_tbl_1[] = {};
static const struct hi707_i2c_reg_conf const cam_set_sharpness_tbl_2[] = {};
static const struct hi707_i2c_reg_conf const cam_set_sharpness_tbl_3[] = {};
static const struct hi707_i2c_reg_conf const cam_set_sharpness_tbl_4[] = {};
/* -------- end of sharpness ------------------ */

/* -------- start of contrast	------------------*/
static const struct hi707_i2c_reg_conf const cam_set_contrast_tbl_0[] = {};

static const struct hi707_i2c_reg_conf const cam_set_contrast_tbl_1[] = {};

static const struct hi707_i2c_reg_conf const cam_set_contrast_tbl_2[] = {};

static const struct hi707_i2c_reg_conf const cam_set_contrast_tbl_3[] = {};

static const struct hi707_i2c_reg_conf const cam_set_contrast_tbl_4[] = {};
/* -------- end of contrast   ------------------*/
/* -------- start of effect ------------------ */
static const struct hi707_i2c_reg_conf const cam_set_effect_off_tbl[] = {};
static const struct hi707_i2c_reg_conf const cam_set_effect_mono_tbl[] = {};
static const struct hi707_i2c_reg_conf const cam_set_effect_negative_tbl[] = {};
static const struct hi707_i2c_reg_conf const cam_set_effect_solarize_tbl[] = {};
static const struct hi707_i2c_reg_conf const cam_set_effect_sepia_tbl[] = {};
/* -------- end of effect ------------------ */
static const struct hi707_i2c_reg_conf const cam_set_mode_preview_tbl[] = {
{0x03, 0x22, BYTE_BYTE_LEN, 0},
{0x10, 0xfb, BYTE_BYTE_LEN, 0},

{0x03, 0x20, BYTE_BYTE_LEN, 0},
{0x10, 0x9c, BYTE_BYTE_LEN, 0},
};
static const struct hi707_i2c_reg_conf const cam_release_tbl[] = {};	
static const struct hi707_i2c_reg_conf const cam_set_mode_snapshot_tbl[] = {
{0x03, 0x22, BYTE_BYTE_LEN, 0},
{0x10, 0x7b, BYTE_BYTE_LEN, 0},

{0x03, 0x20, BYTE_BYTE_LEN, 0},
{0x10, 0x1c, BYTE_BYTE_LEN, 0},
};
static const struct hi707_i2c_reg_conf const cam_set_mode_rawsnapshot_tbl[] = {};

struct hi707_reg hi707_regs = {
	.cambl = cam_init_tbl,
	.cambl_size = ARRAY_SIZE(cam_init_tbl),

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


#endif

