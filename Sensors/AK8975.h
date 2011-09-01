/*
             Bluetooth Robot
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _SENSORS_AK8975_H_
#define _SENSORS_AK8975_H_

	/* Includes: */
		#include "SensorsCommon.h"

	/* Macros: */
		#define AK8975_ADDRESS                  (0x0C << 1)
		#define AK8975_DEVICE_ID                0x48		 

		#define AK8975_REG_WIA                  0x00
		#define AK8975_REG_INFO                 0x01

		#define AK8975_REG_ST1                  0x02
		#define AK8975_REG_ST1_DRDY_MASK        (1 << 0)

		#define AK8975_REG_HXL                  0x03
		#define AK8975_REG_HXH                  0x04
		#define AK8975_REG_HYL                  0x05
		#define AK8975_REG_HYH                  0x06
		#define AK8975_REG_HZL                  0x07
		#define AK8975_REG_HZH                  0x08
		#define AK8975_REG_ST2                  0x09
		#define AK8975_REG_ST2_DERR_MASK        (1 << 2)

		#define AK8975_REG_ST2_HOFL_MASK        (1 << 3)

		#define AK8975_REG_CNTL                 0x0A
		#define AK8975_REG_CNTL_MODE_POWER_DOWN (0  << 0)
		#define AK8975_REG_CNTL_MODE_ONCE       (1  << 0)
		#define AK8975_REG_CNTL_MODE_SELF_TEST  (8  << 0)
		#define AK8975_REG_CNTL_MODE_FUSE_ROM   (15 << 0)

		#define AK8975_REG_RSVC                 0x0B
		#define AK8975_REG_ASTC                 0x0C
		#define AK8975_REG_TS1                  0x0D
		#define AK8975_REG_TS2                  0x0E
		#define AK8975_REG_I2CDIS               0x0F
		#define AK8975_REG_ASAX                 0x10
		#define AK8975_REG_ASAY                 0x11
		#define AK8975_REG_ASAZ                 0x12

	/* Function Prototypes: */
		void AK8975_Init(SensorData_t* const CompassSensorInfo);
		void AK8975_ZeroCalibrate(void);
		void AK8975_Update(SensorData_t* const CompassSensorInfo);

#endif

