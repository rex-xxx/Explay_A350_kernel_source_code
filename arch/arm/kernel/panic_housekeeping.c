/* arch/arm/kernel/panic_housekeeping.c
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/notifier.h>

#include <asm/cacheflush.h>

static int panic_housekeeping(struct notifier_block *this, unsigned long event,
			void *ptr);

static struct notifier_block panic_blk = {
	.notifier_call	= panic_housekeeping,
};

static int panic_housekeeping(struct notifier_block *this, unsigned long event,
			void *ptr)
{
	unsigned long flags;
	
	local_irq_save(flags);
	__cpuc_flush_kern_all();
	__cpuc_flush_user_all();
	local_irq_restore(flags);
	
	return NOTIFY_DONE;
}

static int __init panic_housekeeping_init(void)
{
	atomic_notifier_chain_register(&panic_notifier_list, &panic_blk);
	printk(KERN_INFO "panic housekeeping initialized.\n");
	return 0;
}

arch_initcall(panic_housekeeping_init);
