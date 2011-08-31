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

#ifndef _SENSORS_COMMON_H_
#define _SENSORS_COMMON_H_

	/* Includes: */
		#include <stdint.h>
		#include <stdbool.h>

		#include <LUFA/Drivers/Peripheral/TWI.h>

	/* Macros: */
		#define AK8975_ADDRESS          (0x0C << 1)
		#define ITG3200_ADDRESS         (0x68 << 1)
		#define BMA150_ADDRESS          (0x38 << 1)
		#define BMP085_ADDRESS          (0x77 << 1)
		
	/* Type Defines: */
		typedef struct
		{
			bool Connected;
			
			union
			{
				int32_t Single;
			
				struct
				{
					int16_t X;
					int16_t Y;
					int16_t Z;
				} Triplicate;
			} Data;
		} SensorData_t;
		
		typedef struct
		{
			SensorData_t Direction;
			SensorData_t Acceleration;
			SensorData_t Orientation;
			SensorData_t Pressure;
		} Sensor_t;
	
	/* External Variables: */
		extern Sensor_t Sensors;
		
#endif
