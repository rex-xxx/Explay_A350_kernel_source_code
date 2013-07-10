/* Copyright (c) 2009,2011, Code Aurora Forum. All rights reserved.
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


#ifndef __MSM_BATTERY_H__
#define __MSM_BATTERY_H__

#define AC_CHG     0x00000001
#define USB_CHG    0x00000002

#define LCT_CHARGE_BATTERY_PM 1 //lct lusongbai
struct msm_psy_batt_pdata {
	u32 voltage_max_design;
	u32 voltage_min_design;
	u32 avail_chg_sources;
	u32 batt_technology;
	u32 (*calculate_capacity)(u32 voltage);
	//zhaozy porting start
	u32 (*powerup_reason)(void);
	u32 (*powerup_reason_real)(void);
	//zhaozy porting end
};
//zhaozy porting start
struct msm_appsbl_interface
{
	/* Stores boot mode reason for apps */
	u32 apps_boot_reason;
};
//zhaozy porting end

enum {
	BATTERY_REGISTRATION_SUCCESSFUL = 0,
	BATTERY_DEREGISTRATION_SUCCESSFUL = BATTERY_REGISTRATION_SUCCESSFUL,
	BATTERY_MODIFICATION_SUCCESSFUL = BATTERY_REGISTRATION_SUCCESSFUL,
	BATTERY_INTERROGATION_SUCCESSFUL = BATTERY_REGISTRATION_SUCCESSFUL,
	BATTERY_CLIENT_TABLE_FULL = 1,
	BATTERY_REG_PARAMS_WRONG = 2,
	BATTERY_DEREGISTRATION_FAILED = 4,
	BATTERY_MODIFICATION_FAILED = 8,
	BATTERY_INTERROGATION_FAILED = 16,
	/* Client's filter could not be set because perhaps it does not exist */
	BATTERY_SET_FILTER_FAILED         = 32,
	/* Client's could not be found for enabling or disabling the individual
	 * client */
	BATTERY_ENABLE_DISABLE_INDIVIDUAL_CLIENT_FAILED  = 64,
	BATTERY_LAST_ERROR = 128,
};

enum {
	BATTERY_VOLTAGE_UP = 0,
	BATTERY_VOLTAGE_DOWN,
	BATTERY_VOLTAGE_ABOVE_THIS_LEVEL,
	BATTERY_VOLTAGE_BELOW_THIS_LEVEL,
	BATTERY_VOLTAGE_LEVEL,
	BATTERY_ALL_ACTIVITY,
	VBATT_CHG_EVENTS,
	BATTERY_VOLTAGE_UNKNOWN,
};

/*
 * This enum contains defintions of the charger hardware status
 */
enum chg_charger_status_type {
	/* The charger is good      */
	CHARGER_STATUS_GOOD,
	/* The charger is bad       */
	CHARGER_STATUS_BAD,
	/* The charger is weak      */
	CHARGER_STATUS_WEAK,
	/* Invalid charger status.  */
	CHARGER_STATUS_INVALID
};

/*
 *This enum contains defintions of the charger hardware type
 */
enum chg_charger_hardware_type {
	/* The charger is removed                 */
	CHARGER_TYPE_NONE,
	/* The charger is a regular wall charger   */
	CHARGER_TYPE_WALL,
	/* The charger is a PC USB                 */
	CHARGER_TYPE_USB_PC,
	/* The charger is a wall USB charger       */
	CHARGER_TYPE_USB_WALL,
	/* The charger is a USB carkit             */
	CHARGER_TYPE_USB_CARKIT,
	/* Invalid charger hardware status.        */
	CHARGER_TYPE_INVALID
};
#ifdef LCT_CHARGE_BATTERY_PM
typedef enum 
{
    /*! Charger state machine entry point.                            */
    CHG_AUTO__INIT_ST,     
    /*! Power on from the charger state.                              */
    CHG_AUTO__PWR_ON_CHGR_ST,
    /*! Auto rickle charging state.                                   */
    CHG_AUTO__AUTO_TRICKLE_ST,  
    /*!Power on from the battery state.                               */
    CHG_AUTO__PWR_ON_BATT_ST,
    /*! Auto trickle charging has failed state.                       */
    CHG_AUTO__AUTO_TRICKLE_FAILED_ST,
    /*! Transition state, gives time to analog circuitry to settle.   */
    CHG_AUTO__TRANS_ST,
    /*! Power on from battery and charger after auto trickle charing. */                          
    CHG_AUTO__POST_AUTO_TRICKLE_ST,
    /*! Fast charging state.                                          */
    CHG_AUTO__FAST_ST,   
    /*! Trickle charging state.                                       */
    CHG_AUTO__TRICKLE_ST,
    /*! Charging failed state.                                        */
    CHG_AUTO__FAILED_ST,
    /*! End-of-charging state -- charging succeed.                    */
    CHG_AUTO__DONE_ST,
    /*! Temperary state: Low current liming charging                  */
    CHG_AUTO__LOW_ILIM_ST, 
    /*! USB suspend mode.                                             */
    CHG_AUTO__USB_SUSP_ST,
    /*! Paused auto trickle charging                                  */
    CHG_AUTO__PAUSED_AUTO_TRICKLE_ST,
    /*! Paused fast charging                                          */
    CHG_AUTO__PAUSED_FAST_ST,
    /*! Paused trickle charging                                       */
    CHG_AUTO__PAUSED_TRICKLE_ST,
    /*! Invalid state.                                                */
    CHG_AUTO__INVALID_ST           
} pm_auto_chg_state_name_type;
#endif
/*
 *  This enum contains defintions of the battery status
 */
enum chg_battery_status_type {
	/* The battery is good        */
	BATTERY_STATUS_GOOD,
	/* The battery is cold/hot    */
	BATTERY_STATUS_BAD_TEMP,
	/* The battery is bad         */
	BATTERY_STATUS_BAD,
	/* The battery is removed     */
	BATTERY_STATUS_REMOVED,		/* on v2.2 only */
	BATTERY_STATUS_INVALID_v1 = BATTERY_STATUS_REMOVED,
	/* Invalid battery status.    */
	BATTERY_STATUS_INVALID
};

/*
 *This enum contains defintions of the battery voltage level
 */
enum chg_battery_level_type {
	/* The battery voltage is dead/very low (less than 3.2V) */
	BATTERY_LEVEL_DEAD,
	/* The battery voltage is weak/low (between 3.2V and 3.4V) */
	BATTERY_LEVEL_WEAK,
	/* The battery voltage is good/normal(between 3.4V and 4.2V) */
	BATTERY_LEVEL_GOOD,
	/* The battery voltage is up to full (close to 4.2V) */
	BATTERY_LEVEL_FULL,
	/* Invalid battery voltage level. */
	BATTERY_LEVEL_INVALID
};

struct msm_batt_gauge {
	int (*get_battery_mvolts) (void);
	int (*get_battery_temperature) (void);
	int (*is_battery_present) (void);
	int (*is_battery_temp_within_range) (void);
	int (*is_battery_id_valid) (void);
	int (*get_battery_status)(void);
	int (*get_charger_status)(void);
	int (*get_charger_type)(void);
	int (*get_batt_remaining_capacity) (void);
};

void msm_batt_update_psy_status(void);
int msm_batt_fuel_register(struct msm_batt_gauge* batt);
void msm_batt_fuel_unregister(struct msm_batt_gauge* batt);
#endif
