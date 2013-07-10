/*
 * Copyright (C) 2010 MEMSIC, Inc.
 *
 * Initial Code:
 *	Robbie Cao
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

/*
 * Definitions for ECOMPASS magnetic sensor chip.
 */
#ifndef __SENSORS_CTL_H__
#define __SENSORS_CTL_H__

#include <linux/ioctl.h>

/* Use 'e' as magic number */
#define MIXED_SENSOR_IOM			'e'

/* IOCTLs for ECOMPASS device */
#define MIXED_SENSOR_IOC_SET_MODE		_IOW(MIXED_SENSOR_IOM, 0x00, short)
#define MIXED_SENSOR_IOC_SET_DELAY		_IOW(MIXED_SENSOR_IOM, 0x01, short)
#define MIXED_SENSOR_IOC_GET_DELAY		_IOR(MIXED_SENSOR_IOM, 0x02, short)

#define MIXED_SENSOR_IOC_SET_AFLAG		_IOW(MIXED_SENSOR_IOM, 0x10, short)
#define MIXED_SENSOR_IOC_GET_AFLAG		_IOR(MIXED_SENSOR_IOM, 0x11, short)
#define MIXED_SENSOR_IOC_SET_MFLAG		_IOW(MIXED_SENSOR_IOM, 0x12, short)
#define MIXED_SENSOR_IOC_GET_MFLAG		_IOR(MIXED_SENSOR_IOM, 0x13, short)
#define MIXED_SENSOR_IOC_SET_OFLAG		_IOW(MIXED_SENSOR_IOM, 0x14, short)
#define MIXED_SENSOR_IOC_GET_OFLAG		_IOR(MIXED_SENSOR_IOM, 0x15, short)
#define MIXED_SENSOR_IOC_SET_PFLAG		_IOW(MIXED_SENSOR_IOM, 0x16, short)
#define MIXED_SENSOR_IOC_GET_PFLAG		_IOR(MIXED_SENSOR_IOM, 0x17, short)
#define MIXED_SENSOR_IOC_SET_LFLAG		_IOW(MIXED_SENSOR_IOM, 0x18, short)
#define MIXED_SENSOR_IOC_GET_LFLAG		_IOR(MIXED_SENSOR_IOM, 0x19, short)

#define MIXED_SENSOR_IOC_SET_APARMS		_IOW(MIXED_SENSOR_IOM, 0x20, int[4])
#define MIXED_SENSOR_IOC_GET_APARMS		_IOR(MIXED_SENSOR_IOM, 0x21, int[4])
#define MIXED_SENSOR_IOC_SET_MPARMS		_IOW(MIXED_SENSOR_IOM, 0x22, int[4])
#define MIXED_SENSOR_IOC_GET_MPARMS		_IOR(MIXED_SENSOR_IOM, 0x23, int[4])
#define MIXED_SENSOR_IOC_SET_OPARMS_YAW		_IOW(MIXED_SENSOR_IOM, 0x24, int[4])
#define MIXED_SENSOR_IOC_GET_OPARMS_YAW		_IOR(MIXED_SENSOR_IOM, 0x25, int[4])
#define MIXED_SENSOR_IOC_SET_OPARMS_PITCH	_IOW(MIXED_SENSOR_IOM, 0x26, int[4])
#define MIXED_SENSOR_IOC_GET_OPARMS_PITCH	_IOR(MIXED_SENSOR_IOM, 0x27, int[4])
#define MIXED_SENSOR_IOC_SET_OPARMS_ROLL	_IOW(MIXED_SENSOR_IOM, 0x28, int[4])
#define MIXED_SENSOR_IOC_GET_OPARMS_ROLL	_IOR(MIXED_SENSOR_IOM, 0x29, int[4])

#define MIXED_SENSOR_IOC_SET_YPR		_IOW(MIXED_SENSOR_IOM, 0x30, int[14])
#define MIXED_SENSOR_IOC_GET_MODE		_IOR(MIXED_SENSOR_IOM, 0x31, short)


#endif /* __ECOMPASS_H__ */

