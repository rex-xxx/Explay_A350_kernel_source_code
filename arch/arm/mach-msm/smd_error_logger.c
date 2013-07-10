/* arch/arm/mach-msm/smd_error_logger.c
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kmsg_dump.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/miscdevice.h>
#include <mach/msm_smsm.h>
#include <mach/msm_smd.h>
#include "proc_comm.h"

#define __LOG_BUF_LEN	(1 << CONFIG_LOG_BUF_SHIFT)
#define LOG_BUF_MASK	(__LOG_BUF_LEN - 1)
#ifndef MIN
# define MIN(x,y)	((x)>(y)?(y):(x))
#endif
typedef unsigned uint32;

static char *smd_logger_alloc(unsigned *size_ptr)
{
	char *x;
	unsigned size = 0;

	x = smem_get_entry(SMEM_RESET_LOG, &size);
	if (x == NULL) {
		size = __LOG_BUF_LEN;
		x = smem_alloc2(SMEM_RESET_LOG, size);
	}
	if (size_ptr)
		*size_ptr = size;
	return x;
}

static void smd_logger_notify(const char *ptr, uint32 channel, uint32 size)
{
	uint32 data1;
	uint32 data2;

	data1 = channel;
	data2 = size;
	msm_proc_comm(PCOM_OEM_CRASH_LOG_CMD, &data1, &data2);
}

static void smd_log_dump(struct kmsg_dumper *dumper, enum kmsg_dump_reason reason,
							const char *s1, unsigned long l1,
							const char *s2, unsigned long l2)
{
	char *x;
	unsigned size;
	int i, offset;
	if (reason != KMSG_DUMP_LCT_PRIVATE && reason != KMSG_DUMP_PANIC) {
		return;
	}

	x = smd_logger_alloc(&size);
	if (x == NULL || size <= 0) {
		pr_err("failed to alloc smd log dumper memory\n");
		return;
	}

	if (l1 == 0) {
		/* Loged Charactors more less than log_end
		 * Write log message until last char
		 */
		if ( l2 > size) {
			offset = l2 - size;
		} else {
			offset = 0;
		}
		size = MIN(size, l2);
		for (i = 0; i < size; i++) {
			x[i] = s2[offset + i];
		}
		x[size] = 0;
	} else {
		/* Loged Charactors are more than log_end
		 * Write log message until last char
		 * Use Loopback mode!
		 */
		if (size < __LOG_BUF_LEN) {
			l2 += __LOG_BUF_LEN - size;
		}
		for (i = 0; i < size; i++, l2++) {
			x[i] = s2[l2&LOG_BUF_MASK];
		}
		x[size] = 0;
	}
	smd_logger_notify(x, SMEM_RESET_LOG, (uint32)size);
}

static struct kmsg_dumper smd_log_dumper = {
	.dump = smd_log_dump,
	.registered = 0,
};

static int __init msm_smd_err_logger_init(void)
{
	return kmsg_dump_register(&smd_log_dumper);
}

static void __exit msm_smd_err_logger_exit(void)
{
	kmsg_dump_unregister(&smd_log_dumper);
}

module_init(msm_smd_err_logger_init);
module_exit(msm_smd_err_logger_exit);
MODULE_DESCRIPTION("MSM platform Kernel Panic Logger");
