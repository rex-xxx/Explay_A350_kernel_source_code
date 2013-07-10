#include "mipi_backlight.h"
#include <asm/system.h>
#include <linux/spinlock.h>

DEFINE_SPINLOCK(backlight_pulse_lock);

static int old_target_level=0;
static char backlight_state=0;//0-off, 1-on
/*
parameters:
level		----0~max_level

return:0~max_level step pulse num
 */
static int get_backlight_step_level(int target_level, int old_level,int max_level)
{
	int step_level=-1;
	if(target_level == old_level) return -2;
	if((target_level < 0) ||(target_level > max_level)) return -3;
	if(target_level == 0) return 0;
	
	/* step pulse number calulate */
	if(old_level < target_level) ///old backlight current is greater than target current
	{
		step_level = target_level - old_level;
	}
	else  ///old backlight current is smaller than target current
	{
		step_level = target_level + max_level - old_level;
	}
	
	return step_level;

}

static char gpio_backlight_set_brightness(int level, int old_level)
{
	int pulse_count = level;
	int i=0;
	
	//pr_info("gpio_backlight_set_brightness level=%d\r\n", level);	
	if(pulse_count==0)
	{	
		gpio_set_value(GPIO_BACKLIGHT_EN,0);//need to wait for 3 ms, this in arm11
		mdelay(3);
		return 0;
	}
	
	spin_lock(&backlight_pulse_lock);
	if(old_level == 0)
	{
		gpio_set_value(GPIO_BACKLIGHT_EN,0);
		udelay(2);
		gpio_set_value(GPIO_BACKLIGHT_EN,1);
		udelay(35);
		pulse_count --;
	}	
	
	for(i=0; i<pulse_count; i++)
	{
		gpio_set_value(GPIO_BACKLIGHT_EN,0);
		udelay(2);
		gpio_set_value(GPIO_BACKLIGHT_EN,1);
		udelay(2);
	}
	spin_unlock(&backlight_pulse_lock);
	mdelay(2);
	return 1;
}

void mipi_backlight_control(struct msm_fb_data_type *mfd)
{
	int bl_level = mfd->bl_level;
	int max_level = mfd->panel_info.bl_max;
	int target_level=0;
	int step_level=0;
	int percent_level=0;//use to control arm9 pmic gpio

        if(!mfd->panel_power_on) { //the UI invoke bl first when calling.  lct.huliangsheng
            printk(KERN_WARNING "lcd hasn't been initialized,shouldn't turn on bkl!\n");
            return;  
        }
	MSM_FB_INFO("bl_level= %d\n", bl_level);
	if(backlight_state == 0)
	{
		gpio_set_value(GPIO_BACKLIGHT_EN,0);//need to wait for 3 ms, this in arm11
		pmapp_disp_backlight_set_brightness(0);//this in arm9 pmic
		mdelay(3);
		old_target_level = 0;
	}
	//check parameter
	if(bl_level < 0) bl_level = 0;
	if(bl_level > max_level) bl_level = max_level;
	//calulate pulses
	if(bl_level == 0) target_level = 0;
	else target_level = max_level + 1 - bl_level;
	//no change
	if(target_level ==  old_target_level) return;
	step_level = get_backlight_step_level(target_level, old_target_level,max_level);//get step_pulse_num
	if(step_level < 0) return;
	backlight_state = gpio_backlight_set_brightness(step_level, old_target_level);//arm11 gpio	
	if(target_level == 0) percent_level = 0;
	else percent_level = (int)((max_level + 1 - target_level)*100/max_level);
	pmapp_disp_backlight_set_brightness(percent_level);//arm9 pmic gpio
	old_target_level = target_level;
	
	
}

void mipi_backlight_turnoff(void)
{
	gpio_backlight_set_brightness(0,0);
	pmapp_disp_backlight_set_brightness(0);
}
