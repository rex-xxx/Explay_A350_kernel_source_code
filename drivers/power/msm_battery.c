/* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
 * this needs to be before <linux/kernel.h> is loaded,
 * and <linux/sched.h> loads <linux/kernel.h>
 */
#define DEBUG 1

#include <linux/slab.h>
#include <linux/earlysuspend.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/workqueue.h>

#include <asm/atomic.h>
#include <linux/wakelock.h>
#include <mach/msm_rpcrouter.h>
#include <mach/msm_battery.h>

#include <mach/msm_smsm.h>
static struct wake_lock  msm_chg_wake_lock;

#define BATTERY_RPC_PROG	0x30000089
#define BATTERY_RPC_VER_1_1	0x00010001
#define BATTERY_RPC_VER_2_1	0x00020001
#define BATTERY_RPC_VER_4_1     0x00040001
#define BATTERY_RPC_VER_5_1     0x00050001

#define BATTERY_RPC_CB_PROG	(BATTERY_RPC_PROG | 0x01000000)

#define CHG_RPC_PROG		0x3000001a
#define CHG_RPC_VER_1_1		0x00010001
#define CHG_RPC_VER_1_3		0x00010003
#define CHG_RPC_VER_2_2		0x00020002
#define CHG_RPC_VER_3_1         0x00030001
#define CHG_RPC_VER_4_1         0x00040001

#define BATTERY_REGISTER_PROC				2
#define BATTERY_MODIFY_CLIENT_PROC			4
#define BATTERY_DEREGISTER_CLIENT_PROC			5
#define BATTERY_READ_MV_PROC				12
#define BATTERY_ENABLE_DISABLE_FILTER_PROC		14

#define VBATT_FILTER			2

#define BATTERY_CB_TYPE_PROC		1
#define BATTERY_CB_ID_ALL_ACTIV		1
#define BATTERY_CB_ID_LOW_VOL		2
#define BATTERY_CB_ID_CHG_EVT		3
#if 0 //lct lusongbai
#define BATTERY_LOW		3200
#define BATTERY_HIGH		4300
#else
#define BATTERY_LOW		3420
#define BATTERY_HIGH		4300
#endif
#ifdef LCT_CHARGE_BATTERY_PM
/*
0格	<10%	                     V<3.68	                  <5%	 V<3.60	
1格	10%~30%	3.68≤V<3.77	5%~15%	3.63≤     V<3.69	
2格	30%~50%	3.77≤V<3.82	15%~40%	3.69≤V<3.84	
3格	50%~70%	3.82≤V<3.92	40%~65%	3.84≤V<3.99	
4格	70%~90%	3.92≤V<4.06	65%~100%	3.99≤V	
5格	90%~100%	4.06≤V	/	/
*/
#ifdef CONFIG_LCT_AW550_DW
#define MSM_BATTERY_LEVEL0			3380        /*0%*/
#define MSM_BATTERY_LEVEL1			3615        /*5%*/
#define MSM_BATTERY_LEVEL2			3664        /*15%*/
#define MSM_BATTERY_LEVEL3			3714        /*30%*/
#define MSM_BATTERY_LEVEL4			3781        /*50%*/
#define MSM_BATTERY_LEVEL5			3880        /*70%*/
#define MSM_BATTERY_LEVEL6			4055        /*90%*/
#define MSM_BATTERY_FULL			4170        /*100%*/
#else
#define MSM_BATTERY_LEVEL0			3380        /*0%*/
#define MSM_BATTERY_LEVEL1			3658        /*10%*/
#define MSM_BATTERY_LEVEL2			3737        /*30%*/
#define MSM_BATTERY_LEVEL3			3782        /*50%*/
#define MSM_BATTERY_LEVEL4			3900        /*70%*/
#define MSM_BATTERY_FULL			4180        /*100%*/
#endif
#define BATTERY_OFFSET				10
#define BATT_VOLT_TABLE_MAX 5
static u8 voltage_fixup_enable;
static u8 batt_voltage_cnt=0; 
static u32 batt_voltage_table[BATT_VOLT_TABLE_MAX]={0};
static u8 voltage_changed_callback=0;



#endif
#define ONCRPC_CHG_GET_GENERAL_STATUS_PROC	12
#define ONCRPC_CHARGER_API_VERSIONS_PROC	0xffffffff

#define BATT_RPC_TIMEOUT    5000	/* 5 sec */

#define INVALID_BATT_HANDLE    -1

#define RPC_TYPE_REQ     0
#define RPC_TYPE_REPLY   1
#define RPC_REQ_REPLY_COMMON_HEADER_SIZE   (3 * sizeof(uint32_t))

#define MSM_BATT_POLLING_TIME	(3 * HZ)
#define  MSM_BATT_LOW_LEVEL_POLLING_TIME    (10*HZ)
#define  MSM_BATT_HIGH_LEVLE_POLLING_TIME   (3*60*HZ)

#if DEBUG
#define DBG_LIMIT(x...) do {if (printk_ratelimit()) pr_debug(x); } while (0)
#else
#define DBG_LIMIT(x...) do {} while (0)
#endif



#ifndef CONFIG_BATTERY_MSM_FAKE
struct rpc_reply_batt_chg_v1 {
	struct rpc_reply_hdr hdr;
	u32 	more_data;

	u32	charger_status;
	u32	charger_type;
	u32	battery_status;
	u32	battery_level;
	u32     battery_voltage;
	u32	battery_temp;
	#ifdef LCT_CHARGE_BATTERY_PM
	u32	charger_voltage;
       u32	charger_current;
       u32	chg_state;//auto charge state
       #endif
};

struct rpc_reply_batt_chg_v2 {
	struct rpc_reply_batt_chg_v1	v1;

	u32	is_charger_valid;
	u32	is_charging;
	u32	is_battery_valid;
	u32	ui_event;
};

union rpc_reply_batt_chg {
	struct rpc_reply_batt_chg_v1	v1;
	struct rpc_reply_batt_chg_v2	v2;
};

static union rpc_reply_batt_chg rep_batt_chg;
#endif

struct msm_battery_info {
	u32 voltage_max_design;
	u32 voltage_min_design;
	u32 chg_api_version;
	u32 batt_technology;
	u32 batt_api_version;

	u32 avail_chg_sources;
	u32 current_chg_source;

	u32 batt_status;
	u32 batt_health;
	u32 charger_valid;
	u32 batt_valid;
	u32 batt_capacity; /* in percentage */

	u32 charger_status;
	u32 charger_type;
	u32 battery_status;
	u32 battery_level;
	u32 battery_voltage; /* in millie volts */
	u32 battery_temp;  /* in celsius */

      #ifdef LCT_CHARGE_BATTERY_PM
	u32 charger_voltage;
	u32 charger_current;
	u32 chg_state;
      #endif


	u32 powerup_reason_real;

	u32(*calculate_capacity) (u32 voltage);

        int fuel_gauge;
        int (*get_battery_mvolts) (void);
        int (*get_battery_temperature) (void);
        int (*is_battery_present) (void);
        int (*is_battery_temp_within_range) (void);
        int (*is_battery_id_valid) (void);
        int (*get_battery_status)(void);
        int (*get_batt_remaining_capacity) (void);

	s32 batt_handle;
	s32 chg_handle;

	struct power_supply *msm_psy_ac;
	struct power_supply *msm_psy_usb;
	struct power_supply *msm_psy_batt;
	struct power_supply *current_ps;

	struct msm_rpc_client *batt_client;
	struct msm_rpc_endpoint *chg_ep;

#ifdef CONFIG_HAS_EARLYSUSPEND
	bool is_suspended;
	struct mutex sus_lock;
#endif

	struct workqueue_struct	*batt_queue;
	struct delayed_work batt_work;
	wait_queue_head_t wait_q;

	u32 vbatt_modify_reply_avail;

	struct early_suspend early_suspend;
};

static struct msm_battery_info msm_batt_info = {
	.batt_handle = INVALID_BATT_HANDLE,
	.charger_status = CHARGER_STATUS_BAD,
	.charger_type = CHARGER_TYPE_INVALID,
	.battery_status = BATTERY_STATUS_GOOD,
	.battery_level = BATTERY_LEVEL_FULL,
	.battery_voltage = BATTERY_HIGH,
	.batt_capacity = 70,
	.batt_status = POWER_SUPPLY_STATUS_DISCHARGING,
	.batt_health = POWER_SUPPLY_HEALTH_GOOD,
	.batt_valid  = 1,
	.battery_temp = 23,

	#ifdef LCT_CHARGE_BATTERY_PM
       .charger_voltage=0,
	.charger_current=0,
	.chg_state=0,
	#endif

	.powerup_reason_real = 0,

	.vbatt_modify_reply_avail = 0,
};

static enum power_supply_property msm_power_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static char *msm_power_supplied_to[] = {
	"battery",
};

static int msm_power_get_property(struct power_supply *psy,
				  enum power_supply_property psp,
				  union power_supply_propval *val)
{
	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		if (psy->type == POWER_SUPPLY_TYPE_MAINS) {
			val->intval = msm_batt_info.current_chg_source & AC_CHG
			    ? 1 : 0;
		}
		if (psy->type == POWER_SUPPLY_TYPE_USB) {
			val->intval = msm_batt_info.current_chg_source & USB_CHG
			    ? 1 : 0;
		}
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static struct power_supply msm_psy_ac = {
	.name = "ac",
	.type = POWER_SUPPLY_TYPE_MAINS,
	.supplied_to = msm_power_supplied_to,
	.num_supplicants = ARRAY_SIZE(msm_power_supplied_to),
	.properties = msm_power_props,
	.num_properties = ARRAY_SIZE(msm_power_props),
	.get_property = msm_power_get_property,
};

static struct power_supply msm_psy_usb = {
	.name = "usb",
	.type = POWER_SUPPLY_TYPE_USB,
	.supplied_to = msm_power_supplied_to,
	.num_supplicants = ARRAY_SIZE(msm_power_supplied_to),
	.properties = msm_power_props,
	.num_properties = ARRAY_SIZE(msm_power_props),
	.get_property = msm_power_get_property,
};

static enum power_supply_property msm_batt_power_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CHARGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_POWERUP_REASON_REAL,
	POWER_SUPPLY_PROP_TEMP,
};


//wgz add ,get the powerup true reason
static u32 get_powerup_reason_real(void)
{

        u32 *start_reason;
        start_reason = smem_alloc(SMEM_POWER_ON_STATUS_INFO,
                                sizeof(*start_reason));

        return *start_reason;
}
//wgz add end


static int msm_batt_power_get_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       union power_supply_propval *val)
{
	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		/*if(msm_batt_info.batt_status == POWER_SUPPLY_STATUS_CHARGING  &&
		   msm_batt_info.batt_capacity == 100) {
			val->intval = POWER_SUPPLY_STATUS_FULL;
		} else {*/
			val->intval = msm_batt_info.batt_status;
		//}
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = msm_batt_info.batt_health;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = msm_batt_info.batt_valid;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = msm_batt_info.batt_technology;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
		val->intval = msm_batt_info.voltage_max_design;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
		val->intval = msm_batt_info.voltage_min_design;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = msm_batt_info.battery_voltage;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = msm_batt_info.batt_capacity;
		break;
	case POWER_SUPPLY_PROP_POWERUP_REASON_REAL:
		val->intval = get_powerup_reason_real();
		break;
//lct.yujun [2011-12-13]
	case  POWER_SUPPLY_PROP_CHARGE_NOW:
		val->intval = msm_batt_info.charger_voltage;
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		val->intval = msm_batt_info.charger_current;
		break;

	case  POWER_SUPPLY_PROP_TEMP:
		val->intval = msm_batt_info.battery_temp;
		break;

//~lct.yujun [2011-12-13]
	default:
		return -EINVAL;
	}
	return 0;
}

static struct power_supply msm_psy_batt = {
	.name = "battery",
	.type = POWER_SUPPLY_TYPE_BATTERY,
	.properties = msm_batt_power_props,
	.num_properties = ARRAY_SIZE(msm_batt_power_props),
	.get_property = msm_batt_power_get_property,
};

#ifndef CONFIG_BATTERY_MSM_FAKE
struct msm_batt_get_volt_ret_data {
	u32 battery_voltage;
};

static int msm_batt_get_volt_ret_func(struct msm_rpc_client *batt_client,
				       void *buf, void *data)
{
	struct msm_batt_get_volt_ret_data *data_ptr, *buf_ptr;

	data_ptr = (struct msm_batt_get_volt_ret_data *)data;
	buf_ptr = (struct msm_batt_get_volt_ret_data *)buf;

	data_ptr->battery_voltage = be32_to_cpu(buf_ptr->battery_voltage);

	return 0;
}

static u32 msm_batt_get_vbatt_voltage(void)
{
	int rc;

	struct msm_batt_get_volt_ret_data rep;

	rc = msm_rpc_client_req(msm_batt_info.batt_client,
			BATTERY_READ_MV_PROC,
			NULL, NULL,
			msm_batt_get_volt_ret_func, &rep,
			msecs_to_jiffies(BATT_RPC_TIMEOUT));

	if (rc < 0) {
		pr_err("%s: FAIL: vbatt get volt. rc=%d\n", __func__, rc);
		return 0;
	}

	return rep.battery_voltage;
}

#define	be32_to_cpu_self(v)	(v = be32_to_cpu(v))

static int msm_batt_get_batt_chg_status(void)
{
	int rc;

	struct rpc_req_batt_chg {
		struct rpc_request_hdr hdr;
		u32 more_data;
	} req_batt_chg;
	struct rpc_reply_batt_chg_v1 *v1p;

	req_batt_chg.more_data = cpu_to_be32(1);

	memset(&rep_batt_chg, 0, sizeof(rep_batt_chg));

	v1p = &rep_batt_chg.v1;
	rc = msm_rpc_call_reply(msm_batt_info.chg_ep,
				ONCRPC_CHG_GET_GENERAL_STATUS_PROC,
				&req_batt_chg, sizeof(req_batt_chg),
				&rep_batt_chg, sizeof(rep_batt_chg),
				msecs_to_jiffies(BATT_RPC_TIMEOUT));

	if (rc < 0) {
		pr_err("%s: ERROR. msm_rpc_call_reply failed! proc=%d rc=%d\n",
		       __func__, ONCRPC_CHG_GET_GENERAL_STATUS_PROC, rc);
		return rc;
	} else if (be32_to_cpu(v1p->more_data)) {
		be32_to_cpu_self(v1p->charger_status);
		be32_to_cpu_self(v1p->charger_type);
		be32_to_cpu_self(v1p->battery_status);
		be32_to_cpu_self(v1p->battery_level);
		be32_to_cpu_self(v1p->battery_voltage);
		be32_to_cpu_self(v1p->battery_temp);
		#ifdef LCT_CHARGE_BATTERY_PM
		be32_to_cpu_self(v1p->charger_voltage);
		be32_to_cpu_self(v1p->charger_current);
		be32_to_cpu_self(v1p->chg_state);
		#endif
	} else {
		pr_err("%s: No battery/charger data in RPC reply\n", __func__);
		return -EIO;
	}

	return 0;

}

void msm_batt_update_psy_status(void)
{
	static u32 unnecessary_event_count;
	static u32 last_capacity = -1;
    static u32 samecapacitycount = 0;
	u32	charger_status;
	u32	charger_type;
	u32	battery_status;
	u32	battery_level;
	u32     battery_voltage;
	u32	battery_temp;
	#ifdef LCT_CHARGE_BATTERY_PM
       u32	charger_voltage=0;
	u32  charger_current=0;
	u32	chg_state=0;
	#endif
	struct	power_supply	*supp;
	bool	isWake = true;

#ifdef CONFIG_HAS_EARLYSUSPEND
	isWake = !msm_batt_info.is_suspended;
#endif

	if (msm_batt_get_batt_chg_status())
		return;

	if(msm_batt_info.fuel_gauge)
	{
		DBG_LIMIT("BATT: rcvd: msm_batt_info.fuel_gauge is not NULL\n");
		charger_status = rep_batt_chg.v1.charger_status;
		charger_type = rep_batt_chg.v1.charger_type;
		battery_status = msm_batt_info.get_battery_status();
		battery_level =  rep_batt_chg.v1.battery_level;
		battery_voltage = msm_batt_info.get_battery_mvolts();
		battery_temp = msm_batt_info.get_battery_temperature();
	}
	else
	{
		DBG_LIMIT("BATT: rcvd: msm_batt_info.fuel_gauge  NULL\n");
		charger_status = rep_batt_chg.v1.charger_status;
		charger_type = rep_batt_chg.v1.charger_type;
		battery_status = rep_batt_chg.v1.battery_status;
		battery_level = rep_batt_chg.v1.battery_level;
		battery_voltage = rep_batt_chg.v1.battery_voltage;//msm_batt_get_vbatt_voltage();
		battery_temp = rep_batt_chg.v1.battery_temp;
		#ifdef LCT_CHARGE_BATTERY_PM
              charger_voltage = rep_batt_chg.v1.charger_voltage;
		charger_current = rep_batt_chg.v1.charger_current;
		chg_state = rep_batt_chg.v1.chg_state;
		#endif
	}

           #ifdef LCT_CHARGE_BATTERY_PM
             batt_voltage_table[batt_voltage_cnt++]=battery_voltage;
	     if(batt_voltage_cnt>=BATT_VOLT_TABLE_MAX){
	 	 batt_voltage_cnt=0;
		 voltage_fixup_enable=1;
	 	}
          #endif

	DBG_LIMIT("BATT: rcvd: %d, %d, %d,%d;volt=%d, %d;;chg_v=%d,chg_i=%d,state=%d\n",
		 charger_status, charger_type, battery_status,
		 battery_level, battery_voltage, battery_temp,
		 charger_voltage,charger_current,chg_state);
	/* Make correction for battery status */
	if (battery_status == BATTERY_STATUS_INVALID_v1) {
		if (msm_batt_info.chg_api_version < CHG_RPC_VER_3_1)
			battery_status = BATTERY_STATUS_INVALID;
			DBG_LIMIT("BATT: Reset battery status to invalid\n");
	}

	if (charger_status == msm_batt_info.charger_status &&
	    charger_type == msm_batt_info.charger_type &&
	    battery_status == msm_batt_info.battery_status &&
	    battery_level == msm_batt_info.battery_level &&
	    battery_voltage == msm_batt_info.battery_voltage &&
	    battery_temp == msm_batt_info.battery_temp 
	  #ifdef LCT_CHARGE_BATTERY_PM
              && charger_voltage==msm_batt_info.charger_voltage &&
		charger_current==msm_batt_info.charger_current &&
		chg_state==msm_batt_info.chg_state 
	  #endif
	) {
		/* Got unnecessary event from Modem PMIC VBATT driver.
		 * Nothing changed in Battery or charger status.
		 */
		unnecessary_event_count++;
		if ((unnecessary_event_count % 20) == 1)
		{
			DBG_LIMIT("BATT: rcvd: %d, %d, %d, %d; %d, %d\n",
                 charger_status, charger_type, battery_status,
                 battery_level, battery_voltage, battery_temp);
			DBG_LIMIT("BATT: same event count = %u\n",
				 unnecessary_event_count);
		}
		if(isWake) {
			queue_delayed_work(msm_batt_info.batt_queue, &msm_batt_info.batt_work, MSM_BATT_POLLING_TIME);
		}
		return;
	}

	unnecessary_event_count = 0;

	if (battery_status == BATTERY_STATUS_INVALID &&
	    battery_level != BATTERY_LEVEL_INVALID) {
		DBG_LIMIT("BATT: change status(%d) to (%d) for level=%d\n",
			 battery_status, BATTERY_STATUS_GOOD, battery_level);
		battery_status = BATTERY_STATUS_GOOD;
	}

	if (msm_batt_info.charger_type != charger_type) {
		if (charger_type == CHARGER_TYPE_USB_PC) {
			DBG_LIMIT("BATT: USB charger plugged in\n");
            wake_lock(&msm_chg_wake_lock);
			msm_batt_info.current_chg_source = USB_CHG;
			supp = &msm_psy_usb;
		} else if (charger_type == CHARGER_TYPE_USB_WALL ||
			   charger_type == CHARGER_TYPE_WALL ||
			   charger_type == CHARGER_TYPE_USB_CARKIT) {
			DBG_LIMIT("BATT: AC Wall changer plugged in\n");
            wake_lock(&msm_chg_wake_lock);
			msm_batt_info.current_chg_source = AC_CHG;
			supp = &msm_psy_ac;
		} else {
			if (msm_batt_info.current_chg_source & AC_CHG)
				DBG_LIMIT("BATT: AC Wall charger removed\n");
			else if (msm_batt_info.current_chg_source & USB_CHG)
				DBG_LIMIT("BATT: USB charger removed\n");
			else
				DBG_LIMIT("BATT: No charger present\n");
            wake_unlock(&msm_chg_wake_lock);
			msm_batt_info.current_chg_source = 0;
			supp = &msm_psy_batt;

			/* Correct charger status */
			if (charger_status != CHARGER_STATUS_INVALID) {
				DBG_LIMIT("BATT: No charging!\n");
				charger_status = CHARGER_STATUS_INVALID;
				msm_batt_info.batt_status =
					POWER_SUPPLY_STATUS_DISCHARGING;
			}
		}
	} else
		supp = NULL;

	if (msm_batt_info.charger_status != charger_status) {
		if (charger_status == CHARGER_STATUS_GOOD ||
		    charger_status == CHARGER_STATUS_WEAK) {
			if (msm_batt_info.current_chg_source) {
				DBG_LIMIT("BATT: Charging.\n");
				msm_batt_info.batt_status =
					POWER_SUPPLY_STATUS_CHARGING;

				/* Correct when supp==NULL */
				if (msm_batt_info.current_chg_source & AC_CHG)
					supp = &msm_psy_ac;
				else
					supp = &msm_psy_usb;
			}
		} else {
			DBG_LIMIT("BATT: No charging.\n");
			msm_batt_info.batt_status =
				POWER_SUPPLY_STATUS_DISCHARGING;
			supp = &msm_psy_batt;
		}
	} else {
		/* Correct charger status */
		if (charger_type != CHARGER_TYPE_INVALID &&
		    charger_type != CHARGER_TYPE_NONE &&
		    charger_status == CHARGER_STATUS_GOOD) {
			DBG_LIMIT("BATT: In charging\n");
			msm_batt_info.batt_status =
				POWER_SUPPLY_STATUS_CHARGING;
		}
	}

	/* Correct battery voltage and status */
	if (!battery_voltage) {
		if (charger_status == CHARGER_STATUS_INVALID) {
			DBG_LIMIT("BATT: Read VBATT\n");
			if(msm_batt_info.fuel_gauge)
				battery_voltage = msm_batt_info.get_battery_mvolts();
			else
				battery_voltage = msm_batt_get_vbatt_voltage();
		} else
			/* Use previous */
			battery_voltage = msm_batt_info.battery_voltage;
		if(!battery_voltage)
			battery_voltage = msm_batt_info.battery_voltage;
	}
	if (battery_status == BATTERY_STATUS_INVALID) {
		if (battery_voltage >= msm_batt_info.voltage_min_design &&
		    battery_voltage <= msm_batt_info.voltage_max_design) {
			DBG_LIMIT("BATT: Battery valid\n");
			msm_batt_info.batt_valid = 1;
			battery_status = BATTERY_STATUS_GOOD;
		}
	}

	if (msm_batt_info.battery_status != battery_status) {
		if (battery_status != BATTERY_STATUS_INVALID) {
			msm_batt_info.batt_valid = 1;

			if (battery_status == BATTERY_STATUS_BAD) {
				DBG_LIMIT("BATT: Battery bad.\n");
				msm_batt_info.batt_health =
					POWER_SUPPLY_HEALTH_DEAD;
			} else if (battery_status == BATTERY_STATUS_BAD_TEMP) {
				DBG_LIMIT("BATT: Battery overheat.\n");
				msm_batt_info.batt_health =
					POWER_SUPPLY_HEALTH_OVERHEAT;
			} else {
				DBG_LIMIT("BATT: Battery good.\n");
				msm_batt_info.batt_health =
					POWER_SUPPLY_HEALTH_GOOD;
			}
		} else {
			msm_batt_info.batt_valid = 0;
			DBG_LIMIT("BATT: Battery invalid.\n");
			msm_batt_info.batt_health = POWER_SUPPLY_HEALTH_UNKNOWN;
		}

		if (msm_batt_info.batt_status != POWER_SUPPLY_STATUS_CHARGING) {
			if (battery_status == BATTERY_STATUS_INVALID) {
				DBG_LIMIT("BATT: Battery -> unknown\n");
				msm_batt_info.batt_status =
					POWER_SUPPLY_STATUS_UNKNOWN;
			} else {
				DBG_LIMIT("BATT: Battery -> discharging\n");
				msm_batt_info.batt_status =
					POWER_SUPPLY_STATUS_DISCHARGING;
			}
		}

		if (!supp) {
			if (msm_batt_info.current_chg_source) {
				if (msm_batt_info.current_chg_source & AC_CHG)
					supp = &msm_psy_ac;
				else
					supp = &msm_psy_usb;
			} else
				supp = &msm_psy_batt;
		}
	}

	if(msm_batt_info.batt_status !=POWER_SUPPLY_STATUS_CHARGING)
	{
		charger_voltage = 0;
		charger_current = 0;
	}
	if(msm_batt_info.batt_status ==
                                        POWER_SUPPLY_STATUS_UNKNOWN) {
		msm_batt_info.batt_status = POWER_SUPPLY_STATUS_DISCHARGING;
	}
	msm_batt_info.charger_status 	= charger_status;
	msm_batt_info.charger_type 	= charger_type;
	msm_batt_info.battery_status 	= battery_status;
	msm_batt_info.battery_level 	= battery_level;
	msm_batt_info.battery_temp 	= battery_temp;
     #ifdef LCT_CHARGE_BATTERY_PM
       //msm_batt_info.charger_voltage =charger_voltage;
       msm_batt_info.charger_current =charger_current;
	msm_batt_info.chg_state =chg_state;
     #endif
	if (msm_batt_info.battery_voltage != battery_voltage) {
		msm_batt_info.battery_voltage  	= battery_voltage;
		if(msm_batt_info.fuel_gauge)
			 msm_batt_info.batt_capacity = msm_batt_info.get_batt_remaining_capacity();
		else msm_batt_info.batt_capacity =
			msm_batt_info.calculate_capacity(battery_voltage);
		DBG_LIMIT("BATT: voltage = %u mV [capacity = %d%%]\n",
			 battery_voltage, msm_batt_info.batt_capacity);
		/*If the current capacity is larger than the last one, 
		and no charger is connected, replace it*/
		if((last_capacity != -1 ) 
			&& (msm_batt_info.batt_capacity > last_capacity) 
			&& (msm_batt_info.batt_status != POWER_SUPPLY_STATUS_CHARGING)){
			msm_batt_info.batt_capacity = last_capacity;
		}
        if((last_capacity != -1 )  
			&& (msm_batt_info.batt_status == POWER_SUPPLY_STATUS_CHARGING)){
			samecapacitycount = 0;
			if(msm_batt_info.batt_capacity < last_capacity)
			 msm_batt_info.batt_capacity = last_capacity;
		}
		last_capacity = msm_batt_info.batt_capacity;

		if (!supp)
			supp = msm_batt_info.current_ps;
	}
    else{
        if((last_capacity != -1 ) 
            &&(msm_batt_info.batt_capacity >95)
			&& (msm_batt_info.batt_status == POWER_SUPPLY_STATUS_CHARGING))
                samecapacitycount++;
            if ((samecapacitycount % 30) == 29){
            DBG_LIMIT("BATT: guodandan5rcvd: msm_batt_info.batt_capacity = %d\n",
                 msm_batt_info.batt_capacity);
                msm_batt_info.batt_capacity = 100;
            
            DBG_LIMIT("BATT: guodandan6rcvd: msm_batt_info.batt_capacity = %d\n",
                 msm_batt_info.batt_capacity);
			DBG_LIMIT("BATT: samecapacitycount = %u\n",
				 samecapacitycount);
            last_capacity = msm_batt_info.batt_capacity;

		    if (!supp)
			  supp = msm_batt_info.current_ps;
            }
	}
    DBG_LIMIT("BATT: msm_batt_info.charger_voltage = %d,charger_voltage = %d\n", msm_batt_info.charger_voltage,charger_voltage);
    if(msm_batt_info.charger_voltage != charger_voltage){
        msm_batt_info.charger_voltage = charger_voltage;
        if (!supp)
			supp = msm_batt_info.current_ps;
        DBG_LIMIT("BATT: aaaaabbbbbbbbSupply = %s\n", supp->name);
    }
	if (supp) {
		msm_batt_info.current_ps = supp;
		DBG_LIMIT("BATT: Supply = %s\n", supp->name);
		power_supply_changed(supp);
	}

	if(isWake) {
		queue_delayed_work(msm_batt_info.batt_queue, &msm_batt_info.batt_work, MSM_BATT_POLLING_TIME);
	}
}
EXPORT_SYMBOL(msm_batt_update_psy_status);

#ifdef CONFIG_HAS_EARLYSUSPEND
struct batt_modify_client_req {

	u32 client_handle;

	/* The voltage at which callback (CB) should be called. */
	u32 desired_batt_voltage;

	/* The direction when the CB should be called. */
	u32 voltage_direction;

	/* The registered callback to be called when voltage and
	 * direction specs are met. */
	u32 batt_cb_id;

	/* The call back data */
	u32 cb_data;
};

struct batt_modify_client_rep {
	u32 result;
};

static int msm_batt_modify_client_arg_func(struct msm_rpc_client *batt_client,
				       void *buf, void *data)
{
	struct batt_modify_client_req *batt_modify_client_req =
		(struct batt_modify_client_req *)data;
	u32 *req = (u32 *)buf;
	int size = 0;

	*req = cpu_to_be32(batt_modify_client_req->client_handle);
	size += sizeof(u32);
	req++;

	*req = cpu_to_be32(batt_modify_client_req->desired_batt_voltage);
	size += sizeof(u32);
	req++;

	*req = cpu_to_be32(batt_modify_client_req->voltage_direction);
	size += sizeof(u32);
	req++;

	*req = cpu_to_be32(batt_modify_client_req->batt_cb_id);
	size += sizeof(u32);
	req++;

	*req = cpu_to_be32(batt_modify_client_req->cb_data);
	size += sizeof(u32);

	return size;
}

static int msm_batt_modify_client_ret_func(struct msm_rpc_client *batt_client,
				       void *buf, void *data)
{
	struct  batt_modify_client_rep *data_ptr, *buf_ptr;

	data_ptr = (struct batt_modify_client_rep *)data;
	buf_ptr = (struct batt_modify_client_rep *)buf;

	data_ptr->result = be32_to_cpu(buf_ptr->result);

	return 0;
}

static int msm_batt_modify_client(u32 client_handle, u32 desired_batt_voltage,
	     u32 voltage_direction, u32 batt_cb_id, u32 cb_data)
{
	int rc;

	struct batt_modify_client_req  req;
	struct batt_modify_client_rep rep;

	req.client_handle = client_handle;
	req.desired_batt_voltage = desired_batt_voltage;
	req.voltage_direction = voltage_direction;
	req.batt_cb_id = batt_cb_id;
	req.cb_data = cb_data;

	rc = msm_rpc_client_req(msm_batt_info.batt_client,
			BATTERY_MODIFY_CLIENT_PROC,
			msm_batt_modify_client_arg_func, &req,
			msm_batt_modify_client_ret_func, &rep,
			msecs_to_jiffies(BATT_RPC_TIMEOUT));

	if (rc < 0) {
		pr_err("%s: ERROR. failed to modify  Vbatt client\n",
		       __func__);
		return rc;
	}

	if (rep.result != BATTERY_MODIFICATION_SUCCESSFUL) {
		pr_err("%s: ERROR. modify client failed. result = %u\n",
		       __func__, rep.result);
		return -EIO;
	}

	return 0;
}

void msm_batt_early_suspend(struct early_suspend *h)
{
	int rc;

	pr_debug("%s: enter\n", __func__);

	mutex_lock(&msm_batt_info.sus_lock);
	if(msm_batt_info.is_suspended) {
		mutex_unlock(&msm_batt_info.sus_lock);
		pr_debug("%s: is already suspended!\n", __func__);
		return;
	}
	msm_batt_info.is_suspended = true;
	mutex_unlock(&msm_batt_info.sus_lock);

	flush_workqueue(msm_batt_info.batt_queue);

	if (msm_batt_info.batt_handle != INVALID_BATT_HANDLE) {
		rc = msm_batt_modify_client(msm_batt_info.batt_handle,
				BATTERY_LOW, BATTERY_VOLTAGE_BELOW_THIS_LEVEL,
				BATTERY_CB_ID_LOW_VOL, BATTERY_LOW);

		if (rc < 0) {
			pr_err("%s: msm_batt_modify_client. rc=%d\n",
			       __func__, rc);
			return;
		}
	} else {
		pr_err("%s: ERROR. invalid batt_handle\n", __func__);
		return;
	}

	pr_debug("%s: exit\n", __func__);
}

void msm_batt_late_resume(struct early_suspend *h)
{
	int rc;

	pr_debug("%s: enter\n", __func__);

	mutex_lock(&msm_batt_info.sus_lock);
	if(!msm_batt_info.is_suspended) {
		mutex_unlock(&msm_batt_info.sus_lock);
		pr_debug("%s: is already resumed!\n", __func__);
		return;
	}
	msm_batt_info.is_suspended = false;
	mutex_unlock(&msm_batt_info.sus_lock);

	if (msm_batt_info.batt_handle != INVALID_BATT_HANDLE) {
		rc = msm_batt_modify_client(msm_batt_info.batt_handle,
				BATTERY_LOW, BATTERY_ALL_ACTIVITY,
			       BATTERY_CB_ID_ALL_ACTIV, BATTERY_ALL_ACTIVITY);
		if (rc < 0) {
			pr_err("%s: msm_batt_modify_client FAIL rc=%d\n",
			       __func__, rc);
			return;
		}
	} else {
		pr_err("%s: ERROR. invalid batt_handle\n", __func__);
		return;
	}

	queue_delayed_work(msm_batt_info.batt_queue, &msm_batt_info.batt_work, 0);
	pr_debug("%s: exit\n", __func__);
}
#endif

//lct.yujun [2011-12-19] 
static int msm_batt_resume(struct platform_device *dev)
{
	int rc;

	pr_debug("%s: enter\n", __func__);

	mutex_lock(&msm_batt_info.sus_lock);
	if(!msm_batt_info.is_suspended) {
		mutex_unlock(&msm_batt_info.sus_lock);
		pr_debug("%s: is already resumed!\n", __func__);
		return 0;
	}
	msm_batt_info.is_suspended = false;
	mutex_unlock(&msm_batt_info.sus_lock);

	if (msm_batt_info.batt_handle != INVALID_BATT_HANDLE) {
		rc = msm_batt_modify_client(msm_batt_info.batt_handle,
				BATTERY_LOW, BATTERY_ALL_ACTIVITY,
			       BATTERY_CB_ID_ALL_ACTIV, BATTERY_ALL_ACTIVITY);
		if (rc < 0) {
			pr_err("%s: msm_batt_modify_client FAIL rc=%d\n",
			       __func__, rc);
			return -1;
		}
	} else {
		pr_err("%s: ERROR. invalid batt_handle\n", __func__);
		return -1;
	}

	queue_delayed_work(msm_batt_info.batt_queue, &msm_batt_info.batt_work, 0);
	pr_debug("%s: exit\n", __func__);

	return 0;
}


static int msm_batt_suspend(struct platform_device *dev, pm_message_t state)
{
	int rc;

	pr_debug("%s: enter\n", __func__);

	mutex_lock(&msm_batt_info.sus_lock);
	if(msm_batt_info.is_suspended) {
		mutex_unlock(&msm_batt_info.sus_lock);
		pr_debug("%s: is already suspended!\n", __func__);
		return 0;
	}
	msm_batt_info.is_suspended = true;
	mutex_unlock(&msm_batt_info.sus_lock);


	flush_workqueue(msm_batt_info.batt_queue);

	if (msm_batt_info.batt_handle != INVALID_BATT_HANDLE) {
		rc = msm_batt_modify_client(msm_batt_info.batt_handle,
				BATTERY_LOW, BATTERY_VOLTAGE_BELOW_THIS_LEVEL,
				BATTERY_CB_ID_LOW_VOL, BATTERY_LOW);

		if (rc < 0) {
			pr_err("%s: msm_batt_modify_client. rc=%d\n",
			       __func__, rc);
			return -1;
		}
	} else {
		pr_err("%s: ERROR. invalid batt_handle\n", __func__);
		return -1;
	}




	pr_debug("%s: exit\n", __func__);

	return 0;
}


//~lct.yujun [2011-12-19] 

struct msm_batt_vbatt_filter_req {
	u32 batt_handle;
	u32 enable_filter;
	u32 vbatt_filter;
};

struct msm_batt_vbatt_filter_rep {
	u32 result;
};

static int msm_batt_filter_arg_func(struct msm_rpc_client *batt_client,

		void *buf, void *data)
{
	struct msm_batt_vbatt_filter_req *vbatt_filter_req =
		(struct msm_batt_vbatt_filter_req *)data;
	u32 *req = (u32 *)buf;
	int size = 0;

	*req = cpu_to_be32(vbatt_filter_req->batt_handle);
	size += sizeof(u32);
	req++;

	*req = cpu_to_be32(vbatt_filter_req->enable_filter);
	size += sizeof(u32);
	req++;

	*req = cpu_to_be32(vbatt_filter_req->vbatt_filter);
	size += sizeof(u32);
	return size;
}

static int msm_batt_filter_ret_func(struct msm_rpc_client *batt_client,
				       void *buf, void *data)
{

	struct msm_batt_vbatt_filter_rep *data_ptr, *buf_ptr;

	data_ptr = (struct msm_batt_vbatt_filter_rep *)data;
	buf_ptr = (struct msm_batt_vbatt_filter_rep *)buf;

	data_ptr->result = be32_to_cpu(buf_ptr->result);
	return 0;
}

static int msm_batt_enable_filter(u32 vbatt_filter)
{
	int rc;
	struct  msm_batt_vbatt_filter_req  vbatt_filter_req;
	struct  msm_batt_vbatt_filter_rep  vbatt_filter_rep;

	vbatt_filter_req.batt_handle = msm_batt_info.batt_handle;
	vbatt_filter_req.enable_filter = 1;
	vbatt_filter_req.vbatt_filter = vbatt_filter;

	rc = msm_rpc_client_req(msm_batt_info.batt_client,
			BATTERY_ENABLE_DISABLE_FILTER_PROC,
			msm_batt_filter_arg_func, &vbatt_filter_req,
			msm_batt_filter_ret_func, &vbatt_filter_rep,
			msecs_to_jiffies(BATT_RPC_TIMEOUT));

	if (rc < 0) {
		pr_err("%s: FAIL: enable vbatt filter. rc=%d\n",
		       __func__, rc);
		return rc;
	}

	if (vbatt_filter_rep.result != BATTERY_DEREGISTRATION_SUCCESSFUL) {
		pr_err("%s: FAIL: enable vbatt filter: result=%d\n",
		       __func__, vbatt_filter_rep.result);
		return -EIO;
	}

	pr_debug("%s: enable vbatt filter: OK\n", __func__);
	return rc;
}

struct batt_client_registration_req {
	/* The voltage at which callback (CB) should be called. */
	u32 desired_batt_voltage;

	/* The direction when the CB should be called. */
	u32 voltage_direction;

	/* The registered callback to be called when voltage and
	 * direction specs are met. */
	u32 batt_cb_id;

	/* The call back data */
	u32 cb_data;
	u32 more_data;
	u32 batt_error;
};

struct batt_client_registration_req_4_1 {
	/* The voltage at which callback (CB) should be called. */
	u32 desired_batt_voltage;

	/* The direction when the CB should be called. */
	u32 voltage_direction;

	/* The registered callback to be called when voltage and
	 * direction specs are met. */
	u32 batt_cb_id;

	/* The call back data */
	u32 cb_data;
	u32 batt_error;
};

struct batt_client_registration_rep {
	u32 batt_handle;
};

struct batt_client_registration_rep_4_1 {
	u32 batt_handle;
	u32 more_data;
	u32 err;
};

static int msm_batt_register_arg_func(struct msm_rpc_client *batt_client,
				       void *buf, void *data)
{
	struct batt_client_registration_req *batt_reg_req =
		(struct batt_client_registration_req *)data;

	u32 *req = (u32 *)buf;
	int size = 0;


	if (msm_batt_info.batt_api_version == BATTERY_RPC_VER_4_1) {
		*req = cpu_to_be32(batt_reg_req->desired_batt_voltage);
		size += sizeof(u32);
		req++;

		*req = cpu_to_be32(batt_reg_req->voltage_direction);
		size += sizeof(u32);
		req++;

		*req = cpu_to_be32(batt_reg_req->batt_cb_id);
		size += sizeof(u32);
		req++;

		*req = cpu_to_be32(batt_reg_req->cb_data);
		size += sizeof(u32);
		req++;

		*req = cpu_to_be32(batt_reg_req->batt_error);
		size += sizeof(u32);

		return size;
	} else {
		*req = cpu_to_be32(batt_reg_req->desired_batt_voltage);
		size += sizeof(u32);
		req++;

		*req = cpu_to_be32(batt_reg_req->voltage_direction);
		size += sizeof(u32);
		req++;

		*req = cpu_to_be32(batt_reg_req->batt_cb_id);
		size += sizeof(u32);
		req++;

		*req = cpu_to_be32(batt_reg_req->cb_data);
		size += sizeof(u32);
		req++;

		*req = cpu_to_be32(batt_reg_req->more_data);
		size += sizeof(u32);
		req++;

		*req = cpu_to_be32(batt_reg_req->batt_error);
		size += sizeof(u32);

		return size;
	}

}

static int msm_batt_register_ret_func(struct msm_rpc_client *batt_client,
				       void *buf, void *data)
{
	struct batt_client_registration_rep *data_ptr, *buf_ptr;
	struct batt_client_registration_rep_4_1 *data_ptr_4_1, *buf_ptr_4_1;

	if (msm_batt_info.batt_api_version == BATTERY_RPC_VER_4_1) {
		data_ptr_4_1 = (struct batt_client_registration_rep_4_1 *)data;
		buf_ptr_4_1 = (struct batt_client_registration_rep_4_1 *)buf;

		data_ptr_4_1->batt_handle
			= be32_to_cpu(buf_ptr_4_1->batt_handle);
		data_ptr_4_1->more_data
			= be32_to_cpu(buf_ptr_4_1->more_data);
		data_ptr_4_1->err = be32_to_cpu(buf_ptr_4_1->err);
		return 0;
	} else {
		data_ptr = (struct batt_client_registration_rep *)data;
		buf_ptr = (struct batt_client_registration_rep *)buf;

		data_ptr->batt_handle = be32_to_cpu(buf_ptr->batt_handle);
		return 0;
	}
}

static int msm_batt_register(u32 desired_batt_voltage, u32 voltage_direction,
			     u32 batt_cb_id, u32 cb_data, s32 *handle)
{
	struct batt_client_registration_req batt_reg_req;
	struct batt_client_registration_req_4_1 batt_reg_req_4_1;
	struct batt_client_registration_rep batt_reg_rep;
	struct batt_client_registration_rep_4_1 batt_reg_rep_4_1;
	void *request;
	void *reply;
	int rc;

	if (msm_batt_info.batt_api_version == BATTERY_RPC_VER_4_1) {
		batt_reg_req_4_1.desired_batt_voltage = desired_batt_voltage;
		batt_reg_req_4_1.voltage_direction = voltage_direction;
		batt_reg_req_4_1.batt_cb_id = batt_cb_id;
		batt_reg_req_4_1.cb_data = cb_data;
		batt_reg_req_4_1.batt_error = 1;
		request = &batt_reg_req_4_1;
	} else {
		batt_reg_req.desired_batt_voltage = desired_batt_voltage;
		batt_reg_req.voltage_direction = voltage_direction;
		batt_reg_req.batt_cb_id = batt_cb_id;
		batt_reg_req.cb_data = cb_data;
		batt_reg_req.more_data = 1;
		batt_reg_req.batt_error = 0;
		request = &batt_reg_req;
	}

	if (msm_batt_info.batt_api_version == BATTERY_RPC_VER_4_1)
		reply = &batt_reg_rep_4_1;
	else
		reply = &batt_reg_rep;

	rc = msm_rpc_client_req(msm_batt_info.batt_client,
			BATTERY_REGISTER_PROC,
			msm_batt_register_arg_func, request,
			msm_batt_register_ret_func, reply,
			msecs_to_jiffies(BATT_RPC_TIMEOUT));

	if (rc < 0) {
		pr_err("%s: FAIL: vbatt register. rc=%d\n", __func__, rc);
		return rc;
	}

	if (msm_batt_info.batt_api_version == BATTERY_RPC_VER_4_1) {
		if (batt_reg_rep_4_1.more_data != 0
			&& batt_reg_rep_4_1.err
				!= BATTERY_REGISTRATION_SUCCESSFUL) {
			pr_err("%s: vBatt Registration Failed proc_num=%d\n"
					, __func__, BATTERY_REGISTER_PROC);
			return -EIO;
		}
		*handle = batt_reg_rep_4_1.batt_handle;
	} else
		*handle = batt_reg_rep.batt_handle;

	return 0;
}

struct batt_client_deregister_req {
	u32 batt_handle;
};

struct batt_client_deregister_rep {
	u32 batt_error;
};

static int msm_batt_deregister_arg_func(struct msm_rpc_client *batt_client,
				       void *buf, void *data)
{
	struct batt_client_deregister_req *deregister_req =
		(struct  batt_client_deregister_req *)data;
	u32 *req = (u32 *)buf;
	int size = 0;

	*req = cpu_to_be32(deregister_req->batt_handle);
	size += sizeof(u32);

	return size;
}

static int msm_batt_deregister_ret_func(struct msm_rpc_client *batt_client,
				       void *buf, void *data)
{
	struct batt_client_deregister_rep *data_ptr, *buf_ptr;

	data_ptr = (struct batt_client_deregister_rep *)data;
	buf_ptr = (struct batt_client_deregister_rep *)buf;

	data_ptr->batt_error = be32_to_cpu(buf_ptr->batt_error);

	return 0;
}

static int msm_batt_deregister(u32 batt_handle)
{
	int rc;
	struct batt_client_deregister_req req;
	struct batt_client_deregister_rep rep;

	req.batt_handle = batt_handle;

	rc = msm_rpc_client_req(msm_batt_info.batt_client,
			BATTERY_DEREGISTER_CLIENT_PROC,
			msm_batt_deregister_arg_func, &req,
			msm_batt_deregister_ret_func, &rep,
			msecs_to_jiffies(BATT_RPC_TIMEOUT));

	if (rc < 0) {
		pr_err("%s: FAIL: vbatt deregister. rc=%d\n", __func__, rc);
		return rc;
	}

	if (rep.batt_error != BATTERY_DEREGISTRATION_SUCCESSFUL) {
		pr_err("%s: vbatt deregistration FAIL. error=%d, handle=%d\n",
		       __func__, rep.batt_error, batt_handle);
		return -EIO;
	}

	return 0;
}
#endif  /* CONFIG_BATTERY_MSM_FAKE */

static int msm_batt_cleanup(void)
{
	int rc = 0;

#ifndef CONFIG_BATTERY_MSM_FAKE
	if (msm_batt_info.batt_handle != INVALID_BATT_HANDLE) {

		rc = msm_batt_deregister(msm_batt_info.batt_handle);
		if (rc < 0)
			pr_err("%s: FAIL: msm_batt_deregister. rc=%d\n",
			       __func__, rc);
	}

	if (msm_batt_info.chg_handle != INVALID_BATT_HANDLE) {
		rc = msm_batt_deregister(msm_batt_info.chg_handle);
		if (rc < 0)
			pr_err("%s: FAIL: msm_batt_deregister. rc=%d\n",
			       __func__, rc);
	}

	msm_batt_info.batt_handle = INVALID_BATT_HANDLE;

	if (msm_batt_info.batt_client)
		msm_rpc_unregister_client(msm_batt_info.batt_client);
#endif  /* CONFIG_BATTERY_MSM_FAKE */

	if (msm_batt_info.msm_psy_ac)
		power_supply_unregister(msm_batt_info.msm_psy_ac);

	if (msm_batt_info.msm_psy_usb)
		power_supply_unregister(msm_batt_info.msm_psy_usb);
	if (msm_batt_info.msm_psy_batt)
		power_supply_unregister(msm_batt_info.msm_psy_batt);

#ifndef CONFIG_BATTERY_MSM_FAKE
	if (msm_batt_info.chg_ep) {
		rc = msm_rpc_close(msm_batt_info.chg_ep);
		if (rc < 0) {
			pr_err("%s: FAIL. msm_rpc_close(chg_ep). rc=%d\n",
			       __func__, rc);
		}
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	if (msm_batt_info.early_suspend.suspend == msm_batt_early_suspend)
		unregister_early_suspend(&msm_batt_info.early_suspend);
#endif
	if(msm_batt_info.batt_queue)
		destroy_workqueue(msm_batt_info.batt_queue);
#endif
    wake_lock_destroy(&msm_chg_wake_lock);
	return rc;
}

#ifdef LCT_CHARGE_BATTERY_PM
static u32 last_voltage;
//static u32 maxvolt_times,lowvolt_times;
static int fixup_voltage(u32 voltage)
{
    int i=0;
    u32 current_batt_volt;
	
    if(voltage_fixup_enable==0){
        return voltage;
	}
	
    for(i=0;i<BATT_VOLT_TABLE_MAX;i++){
          current_batt_volt +=batt_voltage_table[i];
	   //printk("fixup_voltage batt_voltage_table[%d]=%d \n",i,batt_voltage_table[i]);
	 }
      current_batt_volt=current_batt_volt /BATT_VOLT_TABLE_MAX;

     printk("BATT: fixup_voltage , result for table ,current_voltage=%d,last_voltage=%d,batt_status=%d \n",
	 	current_batt_volt,last_voltage,msm_batt_info.batt_status);
	 
     //check voltage  valid
     if(msm_batt_info.batt_status==POWER_SUPPLY_STATUS_CHARGING){
         if(current_batt_volt<last_voltage)
                  current_batt_volt=last_voltage;
     	}else{
           if(current_batt_volt>last_voltage)
                    current_batt_volt=last_voltage;     
	}
      pr_info("\n BATT fixup_voltage  after fixup current_voltage=%d\n\n", current_batt_volt);

   return current_batt_volt;
}

static u32 calculate_capacity(int voltage, int high, int low, int base, int ratio) 
{
	u32 capacity;
	
	if(voltage >= high) {
		capacity = base + ratio;
	}else if(voltage <= low) {
		capacity = base;
	}else {
		capacity = (voltage - low) * 100 
					/ (high - low);
		capacity = capacity * ratio / 100;
		capacity += base;
	}
	return capacity;
	
}
#endif

static u32 msm_batt_capacity(u32 current_voltage)
{
#ifdef LCT_CHARGE_BATTERY_PM
	int capacity = -1;
	static int last_capacity = -1;
    static int highChgCapacity = -1;
	static int last_batt_status = -1;
	static u32 start_voltage = 0;
	static int last_level = 0;
	//static int last_batt_status = -1;
	u32 batt_status = msm_batt_info.batt_status;
	
	if(start_voltage == 0) {
		start_voltage = current_voltage;
	}
	pr_info("msm_batt_capacity: %d,%d\n", current_voltage, voltage_changed_callback);
	if(voltage_changed_callback==0){
	         current_voltage = fixup_voltage(current_voltage);
		}
        voltage_changed_callback=0;
	#ifdef CONFIG_LCT_AW550_DW
    if(current_voltage >=MSM_BATTERY_FULL) {
		last_level = MSM_BATTERY_FULL;
		capacity = 100;
	}else if(current_voltage > MSM_BATTERY_LEVEL6) {
		last_level = MSM_BATTERY_LEVEL6;
		capacity = calculate_capacity(current_voltage, MSM_BATTERY_FULL, 
										MSM_BATTERY_LEVEL6, 90, 10);
    }else if(current_voltage > MSM_BATTERY_LEVEL5) {
		last_level = MSM_BATTERY_LEVEL5;
		capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL6, 
										MSM_BATTERY_LEVEL5, 70, 20);
	}else if(current_voltage > MSM_BATTERY_LEVEL4) {
	
	    last_level = MSM_BATTERY_LEVEL4;
  	    capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL5, 
							            MSM_BATTERY_LEVEL4, 50, 20);	
    }else if(current_voltage > MSM_BATTERY_LEVEL3) {
	    last_level = MSM_BATTERY_LEVEL3;
	    capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL4, 
									MSM_BATTERY_LEVEL3, 30, 20);
    }else if(current_voltage > MSM_BATTERY_LEVEL2) {
	    last_level = MSM_BATTERY_LEVEL2;
	    capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL3, 
									MSM_BATTERY_LEVEL2, 15, 15);	
    }else if(current_voltage > MSM_BATTERY_LEVEL1) {
	    last_level = MSM_BATTERY_LEVEL1;
	    capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL2, 
									MSM_BATTERY_LEVEL1, 5, 10);	
    }else if(current_voltage > MSM_BATTERY_LEVEL0) {
	    last_level = MSM_BATTERY_LEVEL0;
	    capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL1, 
						            MSM_BATTERY_LEVEL0, 0, 5);
    }else if(current_voltage <= MSM_BATTERY_LEVEL0) {
	     last_level = MSM_BATTERY_LEVEL0;
	     capacity = 0;
    }
#else
	if(current_voltage >=MSM_BATTERY_FULL) {
		last_level = MSM_BATTERY_FULL;
		capacity = 100;
	}else if(current_voltage > MSM_BATTERY_LEVEL4) {
		last_level = MSM_BATTERY_LEVEL4;
		capacity = calculate_capacity(current_voltage, MSM_BATTERY_FULL, 
										MSM_BATTERY_LEVEL4, 70, 30);
	}else if(current_voltage > MSM_BATTERY_LEVEL3) {
		last_level = MSM_BATTERY_LEVEL3;
		capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL4, 
								MSM_BATTERY_LEVEL3, 50, 20);
	}else if(current_voltage > MSM_BATTERY_LEVEL2) {
		last_level = MSM_BATTERY_LEVEL2;
		capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL3, 
										MSM_BATTERY_LEVEL2, 30, 20);

	}else if(current_voltage > MSM_BATTERY_LEVEL1) {
		last_level = MSM_BATTERY_LEVEL1;
		capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL2, 
										MSM_BATTERY_LEVEL1, 10, 20);
	}else if(current_voltage > MSM_BATTERY_LEVEL0) {
		last_level = MSM_BATTERY_LEVEL0;
		capacity = calculate_capacity(current_voltage, MSM_BATTERY_LEVEL1, 
							MSM_BATTERY_LEVEL0, 0, 10);
	}else if(current_voltage <= MSM_BATTERY_LEVEL0) {
		last_level = MSM_BATTERY_LEVEL0;
		capacity = 0;
	
} 
#endif	  
//done:
       last_voltage=current_voltage;
	last_batt_status = batt_status;
	last_capacity = capacity;
	//msm_batt_info.battery_voltage = current_voltage;
      // pr_info("\n BATT msm_batt_capacity  after fixup current_voltage=%d\n\n", current_voltage);
//if charger complete 10 == CHG_AUTO__DONE_ST
	if(msm_batt_info.chg_state == 10 &&
		msm_batt_info.batt_status ==POWER_SUPPLY_STATUS_CHARGING)
	{
		capacity = 100;
	}
    
	if(msm_batt_info.batt_status !=POWER_SUPPLY_STATUS_CHARGING)
	{
		if(capacity >=90) {
            capacity = 100;
            if(highChgCapacity < capacity && highChgCapacity != -1)
                capacity = highChgCapacity;
        }
	}
    
    
         pr_info("[BATT]  msm_batt_capacity   capacity=%d\n\n", capacity);

    if(msm_batt_info.batt_status ==POWER_SUPPLY_STATUS_CHARGING){
        if(capacity > highChgCapacity)
            highChgCapacity = capacity;
    }else {
        highChgCapacity = -1;
    }     
	return capacity;
	
#else//MSM,by default
	u32 low_voltage = msm_batt_info.voltage_min_design;
	u32 high_voltage = msm_batt_info.voltage_max_design;

	if (current_voltage <= low_voltage)
		return 0;
	else if (current_voltage >= high_voltage)
		return 100;
	else
		return (current_voltage - low_voltage) * 100
			/ (high_voltage - low_voltage);
#endif
}

#ifndef CONFIG_BATTERY_MSM_FAKE
int msm_batt_get_charger_api_version(void)
{
	int rc ;
	struct rpc_reply_hdr *reply;

	struct rpc_req_chg_api_ver {
		struct rpc_request_hdr hdr;
		u32 more_data;
	} req_chg_api_ver;

	struct rpc_rep_chg_api_ver {
		struct rpc_reply_hdr hdr;
		u32 num_of_chg_api_versions;
		u32 *chg_api_versions;
	};

	u32 num_of_versions;

	struct rpc_rep_chg_api_ver *rep_chg_api_ver;


	req_chg_api_ver.more_data = cpu_to_be32(1);

	msm_rpc_setup_req(&req_chg_api_ver.hdr, CHG_RPC_PROG, CHG_RPC_VER_1_1,
			  ONCRPC_CHARGER_API_VERSIONS_PROC);

	rc = msm_rpc_write(msm_batt_info.chg_ep, &req_chg_api_ver,
			sizeof(req_chg_api_ver));
	if (rc < 0) {
		pr_err("%s: FAIL: msm_rpc_write. proc=0x%08x, rc=%d\n",
		       __func__, ONCRPC_CHARGER_API_VERSIONS_PROC, rc);
		return rc;
	}

	for (;;) {
		rc = msm_rpc_read(msm_batt_info.chg_ep, (void *) &reply, -1,
				BATT_RPC_TIMEOUT);
		if (rc < 0)
			return rc;
		if (rc < RPC_REQ_REPLY_COMMON_HEADER_SIZE) {
			pr_err("%s: LENGTH ERR: msm_rpc_read. rc=%d (<%d)\n",
			       __func__, rc, RPC_REQ_REPLY_COMMON_HEADER_SIZE);

			rc = -EIO;
			break;
		}
		/* we should not get RPC REQ or call packets -- ignore them */
		if (reply->type == RPC_TYPE_REQ) {
			pr_err("%s: TYPE ERR: type=%d (!=%d)\n",
			       __func__, reply->type, RPC_TYPE_REQ);
			kfree(reply);
			continue;
		}

		/* If an earlier call timed out, we could get the (no
		 * longer wanted) reply for it.	 Ignore replies that
		 * we don't expect
		 */
		if (reply->xid != req_chg_api_ver.hdr.xid) {
			pr_err("%s: XID ERR: xid=%d (!=%d)\n", __func__,
			       reply->xid, req_chg_api_ver.hdr.xid);
			kfree(reply);
			continue;
		}
		if (reply->reply_stat != RPCMSG_REPLYSTAT_ACCEPTED) {
			rc = -EPERM;
			break;
		}
		if (reply->data.acc_hdr.accept_stat !=
				RPC_ACCEPTSTAT_SUCCESS) {
			rc = -EINVAL;
			break;
		}

		rep_chg_api_ver = (struct rpc_rep_chg_api_ver *)reply;

		num_of_versions =
			be32_to_cpu(rep_chg_api_ver->num_of_chg_api_versions);

		rep_chg_api_ver->chg_api_versions =  (u32 *)
			((u8 *) reply + sizeof(struct rpc_reply_hdr) +
			sizeof(rep_chg_api_ver->num_of_chg_api_versions));

		rc = be32_to_cpu(
			rep_chg_api_ver->chg_api_versions[num_of_versions - 1]);

		pr_debug("%s: num_of_chg_api_versions = %u. "
			"The chg api version = 0x%08x\n", __func__,
			num_of_versions, rc);
		break;
	}
	kfree(reply);
	return rc;
}

static int msm_batt_cb_func(struct msm_rpc_client *client,
			    void *buffer, int in_size)
{
	int rc = 0;
	struct rpc_request_hdr *req;
	u32 procedure;
	u32 accept_status;

//	printk("msm_batt_cb_func is called\n");

	req = (struct rpc_request_hdr *)buffer;
	procedure = be32_to_cpu(req->procedure);

	switch (procedure) {
	case BATTERY_CB_TYPE_PROC:
		accept_status = RPC_ACCEPTSTAT_SUCCESS;
		break;

	default:
		accept_status = RPC_ACCEPTSTAT_PROC_UNAVAIL;
		pr_err("%s: ERROR. procedure (%d) not supported\n",
		       __func__, procedure);
		break;
	}

	msm_rpc_start_accepted_reply(msm_batt_info.batt_client,
			be32_to_cpu(req->xid), accept_status);

	rc = msm_rpc_send_accepted_reply(msm_batt_info.batt_client, 0);
	if (rc)
		pr_err("%s: FAIL: sending reply. rc=%d\n", __func__, rc);

	if (accept_status == RPC_ACCEPTSTAT_SUCCESS)
		#ifdef LCT_CHARGE_BATTERY_PM
		voltage_changed_callback=1;
              #endif
		queue_delayed_work(msm_batt_info.batt_queue, &msm_batt_info.batt_work, 0);

	return rc;
}
#endif  /* CONFIG_BATTERY_MSM_FAKE */

int msm_batt_fuel_register(struct msm_batt_gauge* batt)
{
	int rc = 0;
	if(!batt)
	{
		pr_err("%s: battery fuel gauge is no exist\n" ,__func__);
		return EIO;
	}
	msm_batt_info.fuel_gauge = 1;
	msm_batt_info.get_battery_mvolts = batt->get_battery_mvolts;
	msm_batt_info.get_battery_temperature = batt->get_battery_temperature;
	msm_batt_info.is_battery_present = batt->is_battery_present;
	msm_batt_info.is_battery_temp_within_range = batt->is_battery_temp_within_range;
	msm_batt_info.is_battery_id_valid = batt->is_battery_id_valid;
	msm_batt_info.get_battery_status = batt->get_battery_status;
	msm_batt_info.get_batt_remaining_capacity = batt->get_batt_remaining_capacity;

	return rc;

}
EXPORT_SYMBOL(msm_batt_fuel_register);

void msm_batt_fuel_unregister(struct msm_batt_gauge* batt)
{
	if(!batt)
	{
		pr_err("%s: battery fuel gauge is no exist\n", __func__);
		return;
	}
	msm_batt_info.fuel_gauge = 0;
	msm_batt_info.get_battery_mvolts = NULL;
	msm_batt_info.get_battery_temperature = NULL;
	msm_batt_info.is_battery_present = NULL;
	msm_batt_info.is_battery_temp_within_range = NULL;
	msm_batt_info.is_battery_id_valid = NULL;
	msm_batt_info.get_battery_status = NULL;
	msm_batt_info.get_batt_remaining_capacity = NULL;

	return;
}
EXPORT_SYMBOL(msm_batt_fuel_unregister);

static void msm_batt_worker(struct work_struct *work)
{
	msm_batt_update_psy_status();
}

static int __devinit msm_batt_probe(struct platform_device *pdev)
{
	int rc;
	struct msm_psy_batt_pdata *pdata = pdev->dev.platform_data;

	if (pdev->id != -1) {
		dev_err(&pdev->dev,
			"%s: MSM chipsets Can only support one"
			" battery ", __func__);
		return -EINVAL;
	}

#ifndef CONFIG_BATTERY_MSM_FAKE
	if (pdata->avail_chg_sources & AC_CHG) {
#else
	{
#endif
		rc = power_supply_register(&pdev->dev, &msm_psy_ac);
		if (rc < 0) {
			dev_err(&pdev->dev,
				"%s: power_supply_register failed"
				" rc = %d\n", __func__, rc);
			msm_batt_cleanup();
			return rc;
		}
		msm_batt_info.msm_psy_ac = &msm_psy_ac;
		msm_batt_info.avail_chg_sources |= AC_CHG;
	}

	if (pdata->avail_chg_sources & USB_CHG) {
		rc = power_supply_register(&pdev->dev, &msm_psy_usb);
		if (rc < 0) {
			dev_err(&pdev->dev,
				"%s: power_supply_register failed"
				" rc = %d\n", __func__, rc);
			msm_batt_cleanup();
			return rc;
		}
		msm_batt_info.msm_psy_usb = &msm_psy_usb;
		msm_batt_info.avail_chg_sources |= USB_CHG;
	}

	if (!msm_batt_info.msm_psy_ac && !msm_batt_info.msm_psy_usb) {

		dev_err(&pdev->dev,
			"%s: No external Power supply(AC or USB)"
			"is avilable\n", __func__);
		msm_batt_cleanup();
		return -ENODEV;
	}

	msm_batt_info.voltage_max_design = pdata->voltage_max_design;
	msm_batt_info.voltage_min_design = pdata->voltage_min_design;
	msm_batt_info.batt_technology = pdata->batt_technology;
	if(!msm_batt_info.fuel_gauge)
		msm_batt_info.calculate_capacity = pdata->calculate_capacity;

	if (!msm_batt_info.voltage_min_design)
		msm_batt_info.voltage_min_design = BATTERY_LOW;
	if (!msm_batt_info.voltage_max_design)
		msm_batt_info.voltage_max_design = BATTERY_HIGH;

	if (msm_batt_info.batt_technology == POWER_SUPPLY_TECHNOLOGY_UNKNOWN)
		msm_batt_info.batt_technology = POWER_SUPPLY_TECHNOLOGY_LION;
#ifndef LCT_CHARGE_BATTERY_PM
	if (!msm_batt_info.calculate_capacity)
#endif
		msm_batt_info.calculate_capacity = msm_batt_capacity;

	rc = power_supply_register(&pdev->dev, &msm_psy_batt);
	if (rc < 0) {
		dev_err(&pdev->dev, "%s: power_supply_register failed"
			" rc=%d\n", __func__, rc);
		msm_batt_cleanup();
		return rc;
	}
	msm_batt_info.msm_psy_batt = &msm_psy_batt;

#ifndef CONFIG_BATTERY_MSM_FAKE
	rc = msm_batt_register(BATTERY_LOW, BATTERY_ALL_ACTIVITY,
			       BATTERY_CB_ID_ALL_ACTIV, BATTERY_ALL_ACTIVITY,
			       &msm_batt_info.batt_handle);
	if (rc < 0) {
		dev_err(&pdev->dev,
			"%s: msm_batt_register failed rc = %d\n", __func__, rc);
		msm_batt_cleanup();
		return rc;
	}

	rc =  msm_batt_enable_filter(VBATT_FILTER);

	if (rc < 0) {
		dev_err(&pdev->dev,
			"%s: msm_batt_enable_filter failed rc = %d\n",
			__func__, rc);
		msm_batt_cleanup();
		return rc;
	}

	rc = msm_batt_register(BATTERY_LOW, VBATT_CHG_EVENTS,
			       BATTERY_CB_ID_CHG_EVT, VBATT_CHG_EVENTS,
			       &msm_batt_info.chg_handle);
	if (rc < 0) {
		dev_err(&pdev->dev,
			"%s: msm_batt_register failed rc = %d\n", __func__, rc);
		msm_batt_cleanup();
		return rc;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	msm_batt_info.early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	msm_batt_info.early_suspend.suspend = msm_batt_early_suspend;
	msm_batt_info.early_suspend.resume = msm_batt_late_resume;
	msm_batt_info.is_suspended = false;
	mutex_init(&msm_batt_info.sus_lock);
	//register_early_suspend(&msm_batt_info.early_suspend);
#endif

	/* == Create work queue to do the polling update status== */
	msm_batt_info.batt_queue = create_singlethread_workqueue("battery_queue");
	if (!msm_batt_info.batt_queue) {
		dev_err(&pdev->dev,
			"%s: Create work queue ERROR! \n",
			__func__);

		msm_batt_cleanup();
		return -ENOMEM;
	}

	INIT_DELAYED_WORK(&msm_batt_info.batt_work, msm_batt_worker);
	queue_delayed_work(msm_batt_info.batt_queue, &msm_batt_info.batt_work, MSM_BATT_POLLING_TIME);

#else
	power_supply_changed(&msm_psy_ac);
#endif  /* CONFIG_BATTERY_MSM_FAKE */

        wake_lock_init(&msm_chg_wake_lock, WAKE_LOCK_SUSPEND, "msm_chg");

	if(pdata->powerup_reason_real != NULL)
		msm_batt_info.powerup_reason_real = pdata->powerup_reason_real();

	return 0;
}

static int __devexit msm_batt_remove(struct platform_device *pdev)
{
	int rc;
	rc = msm_batt_cleanup();

	if (rc < 0) {
		dev_err(&pdev->dev,
			"%s: msm_batt_cleanup  failed rc=%d\n", __func__, rc);
		return rc;
	}
	return 0;
}

static struct platform_driver msm_batt_driver = {
	.probe = msm_batt_probe,
	.remove = __devexit_p(msm_batt_remove),
	
	.suspend = msm_batt_suspend,
	.resume = msm_batt_resume,
	
	.driver = {
		   .name = "msm-battery",
		   .owner = THIS_MODULE,
		   },
};

static int __devinit msm_batt_init_rpc(void)
{
	int rc;

#ifdef CONFIG_BATTERY_MSM_FAKE
	pr_info("Faking MSM battery\n");
#else

	msm_batt_info.chg_ep =
		msm_rpc_connect_compatible(CHG_RPC_PROG, CHG_RPC_VER_4_1, 0);
	msm_batt_info.chg_api_version =  CHG_RPC_VER_4_1;
	if (msm_batt_info.chg_ep == NULL) {
		pr_err("%s: rpc connect CHG_RPC_PROG = NULL\n", __func__);
		return -ENODEV;
	}

	if (IS_ERR(msm_batt_info.chg_ep)) {
		msm_batt_info.chg_ep = msm_rpc_connect_compatible(
				CHG_RPC_PROG, CHG_RPC_VER_3_1, 0);
		msm_batt_info.chg_api_version =  CHG_RPC_VER_3_1;
	}
	if (IS_ERR(msm_batt_info.chg_ep)) {
		msm_batt_info.chg_ep = msm_rpc_connect_compatible(
				CHG_RPC_PROG, CHG_RPC_VER_1_1, 0);
		msm_batt_info.chg_api_version =  CHG_RPC_VER_1_1;
	}
	if (IS_ERR(msm_batt_info.chg_ep)) {
		msm_batt_info.chg_ep = msm_rpc_connect_compatible(
				CHG_RPC_PROG, CHG_RPC_VER_1_3, 0);
		msm_batt_info.chg_api_version =  CHG_RPC_VER_1_3;
	}
	if (IS_ERR(msm_batt_info.chg_ep)) {
		msm_batt_info.chg_ep = msm_rpc_connect_compatible(
				CHG_RPC_PROG, CHG_RPC_VER_2_2, 0);
		msm_batt_info.chg_api_version =  CHG_RPC_VER_2_2;
	}
	if (IS_ERR(msm_batt_info.chg_ep)) {
		rc = PTR_ERR(msm_batt_info.chg_ep);
		pr_err("%s: FAIL: rpc connect for CHG_RPC_PROG. rc=%d\n",
		       __func__, rc);
		msm_batt_info.chg_ep = NULL;
		return rc;
	}

	/* Get the real 1.x version */
	if (msm_batt_info.chg_api_version == CHG_RPC_VER_1_1)
		msm_batt_info.chg_api_version =
			msm_batt_get_charger_api_version();

	/* Fall back to 1.1 for default */
	if (msm_batt_info.chg_api_version < 0)
		msm_batt_info.chg_api_version = CHG_RPC_VER_1_1;
	msm_batt_info.batt_api_version =  BATTERY_RPC_VER_4_1;

	msm_batt_info.batt_client =
		msm_rpc_register_client("battery", BATTERY_RPC_PROG,
					BATTERY_RPC_VER_4_1,
					1, msm_batt_cb_func);

	if (msm_batt_info.batt_client == NULL) {
		pr_err("%s: FAIL: rpc_register_client. batt_client=NULL\n",
		       __func__);
		return -ENODEV;
	}
	if (IS_ERR(msm_batt_info.batt_client)) {
		msm_batt_info.batt_client =
			msm_rpc_register_client("battery", BATTERY_RPC_PROG,
						BATTERY_RPC_VER_1_1,
						1, msm_batt_cb_func);
		msm_batt_info.batt_api_version =  BATTERY_RPC_VER_1_1;
	}
	if (IS_ERR(msm_batt_info.batt_client)) {
		msm_batt_info.batt_client =
			msm_rpc_register_client("battery", BATTERY_RPC_PROG,
						BATTERY_RPC_VER_2_1,
						1, msm_batt_cb_func);
		msm_batt_info.batt_api_version =  BATTERY_RPC_VER_2_1;
	}
	if (IS_ERR(msm_batt_info.batt_client)) {
		msm_batt_info.batt_client =
			msm_rpc_register_client("battery", BATTERY_RPC_PROG,
						BATTERY_RPC_VER_5_1,
						1, msm_batt_cb_func);
		msm_batt_info.batt_api_version =  BATTERY_RPC_VER_5_1;
	}
	if (IS_ERR(msm_batt_info.batt_client)) {
		rc = PTR_ERR(msm_batt_info.batt_client);
		pr_err("%s: ERROR: rpc_register_client: rc = %d\n ",
		       __func__, rc);
		msm_batt_info.batt_client = NULL;
		return rc;
	}
#endif  /* CONFIG_BATTERY_MSM_FAKE */

	rc = platform_driver_register(&msm_batt_driver);

	if (rc < 0)
		pr_err("%s: FAIL: platform_driver_register. rc = %d\n",
		       __func__, rc);

	return rc;
}

static int __init msm_batt_init(void)
{
	int rc;

	pr_debug("%s: enter\n", __func__);

	rc = msm_batt_init_rpc();

	if (rc < 0) {
		pr_err("%s: FAIL: msm_batt_init_rpc.  rc=%d\n", __func__, rc);
		msm_batt_cleanup();
		return rc;
	}

	pr_info("%s: Charger/Battery = 0x%08x/0x%08x (RPC version)\n",
		__func__, msm_batt_info.chg_api_version,
		msm_batt_info.batt_api_version);

	return 0;
}

static void __exit msm_batt_exit(void)
{
	platform_driver_unregister(&msm_batt_driver);
}

module_init(msm_batt_init);
module_exit(msm_batt_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Kiran Kandi, Qualcomm Innovation Center, Inc.");
MODULE_DESCRIPTION("Battery driver for Qualcomm MSM chipsets.");
MODULE_VERSION("1.0");
MODULE_ALIAS("platform:msm_battery");
