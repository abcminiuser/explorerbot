/*
             Bluetooth Robot
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.com
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

/** \file
 *
 *  Header file for BMA150.c.
 */

#ifndef _SENSORS_BMA150_H_
#define _SENSORS_BMA150_H_

	/* Includes: */
		#include "SensorsCommon.h"

	/* Macros: */
		/** Address of the BMA150 sensor on the I2C bus. */
		#define BMA150_ADDRESS                 (0x38 << 1)
		
		/** Device ID returned by the sensor when the CHIP ID register is queried. */
		#define BMA150_CHIP_ID                 0x02

		/** \name Register addresses for the BMA150 sensor. */
		//@{
		#define BMA150_CHIP_ID_REG             0x00
		#define BMA150_VERSION_REG             0x01
		#define BMA150_X_AXIS_LSB_REG          0x02
		#define BMA150_X_AXIS_MSB_REG          0x03
		#define BMA150_Y_AXIS_LSB_REG          0x04
		#define BMA150_Y_AXIS_MSB_REG          0x05
		#define BMA150_Z_AXIS_LSB_REG          0x06
		#define BMA150_Z_AXIS_MSB_REG          0x07
		#define BMA150_TEMP_RD_REG             0x08
		#define BMA150_SMB150_STATUS_REG       0x09
		#define BMA150_SMB150_CTRL_REG         0x0A
		#define BMA150_SMB150_CONF1_REG        0x0B
		#define BMA150_LG_THRESHOLD_REG        0x0C
		#define BMA150_LG_DURATION_REG         0x0D
		#define BMA150_HG_THRESHOLD_REG        0x0E
		#define BMA150_HG_DURATION_REG         0x0F
		#define BMA150_MOTION_THRS_REG         0x10
		#define BMA150_HYSTERESIS_REG          0x11
		#define BMA150_CUSTOMER1_REG           0x12
		#define BMA150_CUSTOMER2_REG           0x13
		#define BMA150_RANGE_BWIDTH_REG        0x14
		#define BMA150_SMB150_CONF2_REG        0x15
		#define BMA150_OFFS_GAIN_X_REG         0x16
		#define BMA150_OFFS_GAIN_Y_REG         0x17
		#define BMA150_OFFS_GAIN_Z_REG         0x18
		#define BMA150_OFFS_GAIN_T_REG         0x19
		#define BMA150_OFFSET_X_REG            0x1A
		#define BMA150_OFFSET_Y_REG            0x1B
		#define BMA150_OFFSET_Z_REG            0x1C
		#define BMA150_OFFSET_T_REG            0x1D
		//@}
		
	/* Function Prototypes: */
		void BMA150_Init(SensorData_t* const AccelSensorInfo);
		void BMA150_Update(SensorData_t* const AccelSensorInfo);

#endif

