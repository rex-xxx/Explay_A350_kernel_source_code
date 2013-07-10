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

#ifndef CAMSENSOR_GC0339
#define CAMSENSOR_GC0339
#define GC0339_WriteSettingTbl(pTbl)     GC0339_WriteRegsTbl(pTbl,sizeof(pTbl)/sizeof(pTbl[0]))

enum gc0339_test_mode_t {
    TEST_OFF,
    TEST_1,
    TEST_2,
    TEST_3
};

enum gc0339_resolution_t {
  QTR_SIZE,
  FULL_SIZE,
  HFR_60FPS,
  HFR_90FPS,
  HFR_120FPS,
  INVALID_SIZE
};
/******************************************************************************
GC0339_WREG, *PGC0339_WREG Definition
******************************************************************************/
typedef struct {
    unsigned int addr;          /*Reg Addr :16Bit*/
    unsigned int data;          /*Reg Data :16Bit or 8Bit*/
    unsigned int data_format;   /*Reg Data Format:16Bit = 0,8Bit = 1*/
    unsigned int delay_ms;      /*Operation NOP time(ms)*/
} GC0339_WREG, *PGC0339_WREG;

/******************************************************************************
Initial Setting Table
******************************************************************************/
GC0339_WREG gc0339_init_tbl[] =
{
  {0xfc,0x10,1,0},//clockenable
  {0xfe,0x00,1,0},//###########systemreg########################
  {0xf6,0x07,1,0},//pllmode
  {0xf7,0x01,1,0},
  {0xf7,0x03,1,0},//
  {0xfc,0x16,1,0},
  {0x06,0x00,1,0},//rowstart//###########mipireg########################
  {0x08,0x04,1,0},//colstart2
  {0x09,0x01,1,0},//488-2
  {0x0a,0xe8,1,0},
  {0x0b,0x02,1,0},//648-4+8=
  {0x0c,0x88,1,0},
  {0x0f,0x02,1,0},//[7:4]vb,0x,0x[3:0]hb
  {0x14,0x20,1,0},//MormoOri0x20
  {0x1a,0x21,1,0},//clk_delay
  {0x1b,0x80,1,0},//MormoAdd
  {0x1c,0x49,1,0},//MormoOri0x41
  {0x61,0x2a,1,0},//RAW8->2ayuv->1e
  {0x62,0x8c,1,0},
  {0x63,0x02,1,0},//lwc//652*
  {0x32,0x00,1,0},//MormoOri0x00globaloffset10
  {0x3a,0x20,1,0},//MormoOri0x40
  {0x3b,0x20,1,0},
  {0x69,0x03,1,0},//phyenable
  {0x60,0x84,1,0},//mipienable//0x60,0x90//mipienableandlane_enalbe
  {0x65,0x10,1,0},//MormoOri0x20
  {0x6c,0xaa,1,0},//MormoTrig
  {0x6d,0x00,1,0},//MormoAdd
  {0x67,0x10,1,0},//
  {0x4a,0x40,1,0},
  {0x4b,0x40,1,0},//MormoOri0x46
  {0x4c,0x40,1,0},
  {0xe8,0x04,1,0},
  {0xe9,0xbb,1,0},
  {0x42,0x20,1,0},//bi_b
  {0x47,0x10,1,0},
  {0x50,0x40,1,0},//60//globalgain
  {0xd0,0x00,1,0},
  {0xd3,0x50,1,0},//target
  {0xf6,0x05,1,0},//05-192m;07-96m
  {0x01,0x6a,1,0},//MormoAddfa6aismin
  {0x02,0x0c,1,0},//MormoAdd0x0c0cismin
  {0x0f,0x00,1,0},
  {0x6a,0x11,1,0},//mipipaddriver
  {0x71,0x01,1,0},
  {0x72,0x02,1,0},
  {0x79,0x02,1,0},
  {0x73,0x01,1,0},
  {0x7a,0x01,1,0},
  {0x2e,0x30,1,0},//0->normal01->test[5]1pixel+8
  {0x2b,0x00,1,0},//fixdata
  {0x2c,0x03,1,0},//fixdata
  {0xd2,0x00,1,0},
  {0x20,0xb0,1,0},//enabledddn
  {0x60,0x94,1,0},
};

/******************************************************************************
Preview Setting Table 30Fps
******************************************************************************/
GC0339_WREG gc0339_preview_tbl_30fps[] =
{

};

/******************************************************************************
Preview Setting Table 60Fps
******************************************************************************/
GC0339_WREG gc0339_preview_tbl_60fps[] =
{

};

/******************************************************************************
Preview Setting Table 90Fps
******************************************************************************/
GC0339_WREG gc0339_preview_tbl_90fps[] =
{

};

/******************************************************************************
Capture Setting Table
******************************************************************************/
GC0339_WREG gc0339_capture_tbl[]=
{

};
#endif /* CAMSENSOR_GC0339 */



