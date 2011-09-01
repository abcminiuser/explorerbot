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

#ifndef _SENSORS_BMP085_H_
#define _SENSORS_BMP085_H_

	/* Includes: */
		#include "SensorsCommon.h"

	/* Macros: */
		#define BMP085_ADDRESS               (0x77 << 1)
		#define BMP085_CHIP_ID               0x55
	
		#define BMP085_CONTROL_CMD_TEMP      0x2E
		#define BMP085_CONTROL_CMD_PRESSURE  0x34

		#define BMP085_CAL_AC1_REG           0xAA
		#define BMP085_CAL_AC2_REG           0xAC
		#define BMP085_CAL_AC3_REG           0xAE
		#define BMP085_CAL_AC4_REG           0xB0
		#define BMP085_CAL_AC5_REG           0xB2
		#define BMP085_CAL_AC6_REG           0xB4
		#define BMP085_CAL_B1_REG            0xB6
		#define BMP085_CAL_B2_REG            0xB8
		#define BMP085_CAL_MB_REG            0xBA
		#define BMP085_CAL_MC_REG            0xBC
		#define BMP085_CAL_MD_REG            0xBE

		#define BMP085_CHIP_ID_REG           0xD0
		#define BMP085_VERSION_REG           0xD1
		#define BMP085_CONTROL_REG           0xF4
		#define BMP085_CONVERSION_REG_MSB    0xF6
		#define BMP085_CONVERSION_REG_LSB    0xF7
		#define BMP085_CONVERSION_REG_XLSB   0xF8

	/* Function Prototypes: */
		void BMP085_Init(SensorData_t* const PressureSensorInfo);
		void BMP085_Update(SensorData_t* const PressureSensorInfo);
		
#endif

