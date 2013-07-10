/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
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
 *
 */

#ifndef __ASM__ARCH_OEM_RAPI_CLIENT_H
#define __ASM__ARCH_OEM_RAPI_CLIENT_H

/*
 * OEM RAPI CLIENT Driver header file
 */

#include <linux/types.h>
#include <mach/msm_rpcrouter.h>

enum {
	OEM_RAPI_CLIENT_EVENT_NONE = 0,

	/*
	 * list of oem rapi client events
	 */
	 //zhaozy porting start
	OEM_RAPI_CLIENT_EVENT_ARM9_CB_REGISTER,  
	OEM_RAPI_CLIENT_EVENT_ARM9_OUTVER,
	OEM_RAPI_CLIENT_EVENT_ARM9_INVER,
	OEM_RAPI_CLIENT_GET_USB_PID_INFO,//add by lct.fxl for get at+disk=8 info
	OEM_RAPI_CLIENT_GET_DIAG_INFO,//dd by lct.fxl for get at+disk=7 info
	//zhaozy porting end
	OEM_RAPI_CLIENT_EVENT_TRI_COLOR_LED_WORK = 21,
	OEM_RAPI_CLIENT_EVENT_MAX

};
//zhaozy porting end
/*
** This enum lists the events that the server can notify the client of.
*/
enum
{
  /* -------------------------------- */
  OEM_RAPI_SERVER_EVENT_NONE = 0,
  /* -------------------------------- */

  /* Enumerate OEM server events here
	 .
	 .
	 .
  */
  OEM_RAPI_SERVER_EVENT_ARM11_CB_EVENT,

  /* -------------------------------- */
  OEM_RAPI_SERVER_EVENT_MAX
  /* -------------------------------- */
} ;

#define LCT_DATA_ID_ERROR		   (0)
#define LCT_DATA_ID_COMMON_BASE    (100)
#define LCT_DATA_ID_APP_BASE	   (200)
#define LCT_DATA_ID_NET_BASE	   (300)
#define LCT_DATA_ID_DRIVER_BASE    (400)
#define LCT_DATA_ID_TEST_BASE	   (500)
#define LCT_DATA_ID_FUNCTION_BASE  (600)
#define LCT_DATA_ID_CONTROL_BASE   (700)
#define LCT_DATA_ID_PCSUIT_BASE    (800)
//common

//app
#define LCT_DATA_ID_APP_SET_CONSOLE 				 ((uint32_t)(LCT_DATA_ID_APP_BASE+0)) //lct_data_u32_type
#define LCT_DATA_ID_APP_SET_LCTFN				((uint32_t)(LCT_DATA_ID_APP_BASE+1)) //lct_data_lctfn_type
#define LCT_DATA_ID_APP_WRITE_MAC				((uint32_t)(LCT_DATA_ID_APP_BASE+2)) //lct_data_lctfn_type
#define LCT_DATA_ID_APP_GET_DIAG_VALUE		((uint32_t)(LCT_DATA_ID_APP_BASE+3)) //lct_data_lctfn_type
#define LCT_DATA_ID_APP_SET_DIAG_VALUE		((uint32_t)(LCT_DATA_ID_APP_BASE+4)) //lct_data_lctfn_type
#define LCT_DATA_ID_APP_GET_CDROM_VALUE 	((uint32_t)(LCT_DATA_ID_APP_BASE+5)) //lct_data_lctfn_type
#define LCT_DATA_ID_APP_SET_CDROM_VALUE 	((uint32_t)(LCT_DATA_ID_APP_BASE+6)) //lct_data_lctfn_type
#define LCT_DATA_ID_APP_GET_ARM11_VERSION	((uint32_t)(LCT_DATA_ID_APP_BASE+7)) //lct_data_lctfn_type
#define LCT_DATA_ID_APP_GET_BT_ADDR 				((uint32_t)(LCT_DATA_ID_APP_BASE+8)) //lct_data_lctfn_type
#define LCT_DATA_ID_APP_SET_BT_ADDR 			((uint32_t)(LCT_DATA_ID_APP_BASE+9)) //lct_data_lctfn_type

typedef union
{
		uint8_t 		nDiagValue;
	uint8_t 					   nCDROMValue;
}lct_data_type;

typedef struct
{
  uint32_t id;
  lct_data_type data;
}lct_data_trans_package_type;
//zhaozy porting end

struct oem_rapi_client_streaming_func_cb_arg {
	uint32_t  event;
	void      *handle;
	uint32_t  in_len;
	char      *input;
	uint32_t out_len_valid;
	uint32_t output_valid;
	uint32_t output_size;
};

struct oem_rapi_client_streaming_func_cb_ret {
	uint32_t *out_len;
	char *output;
};

struct oem_rapi_client_streaming_func_arg {
	uint32_t event;
	int (*cb_func)(struct oem_rapi_client_streaming_func_cb_arg *,
		       struct oem_rapi_client_streaming_func_cb_ret *);
	void *handle;
	uint32_t in_len;
	char *input;
	uint32_t out_len_valid;
	uint32_t output_valid;
	uint32_t output_size;
};

struct oem_rapi_client_streaming_func_ret {
	uint32_t *out_len;
	char *output;
};

int oem_rapi_client_streaming_function(
	struct msm_rpc_client *client,
	struct oem_rapi_client_streaming_func_arg *arg,
	struct oem_rapi_client_streaming_func_ret *ret);

int oem_rapi_client_close(void);

struct msm_rpc_client *oem_rapi_client_init(void);

#endif
