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
		#include <avr/pgmspace.h>
	
		#include <stdint.h>
		#include <stdbool.h>

		#include <LUFA/Drivers/Peripheral/TWI.h>

	/* Macros: */
		#define SENSOR_BUS_TIMEOUT_MS   10

	/* Type Defines: */
		typedef struct
		{
			bool  Connected;
			const char* Name;
			
			bool SingleAxis;
			
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
			SensorData_t Temperature;
		} Sensor_t;
	
	/* Inline Functions */
		static inline uint8_t Sensor_WriteBytes(const uint8_t SensorTWIAddress,
		                                        const uint8_t RegisterAddress,
		                                        const uint8_t* const PacketBuffer,
		                                        const uint8_t Length)
		{
			return TWI_WritePacket(SensorTWIAddress, SENSOR_BUS_TIMEOUT_MS, &RegisterAddress, sizeof(RegisterAddress), PacketBuffer, Length);
		}

		static inline uint8_t Sensor_ReadBytes(const uint8_t SensorTWIAddress,
		                                       const uint8_t RegisterAddress,
		                                       uint8_t* const PacketBuffer,
		                                       const uint8_t Length)
		{
			return TWI_ReadPacket(SensorTWIAddress, SENSOR_BUS_TIMEOUT_MS, &RegisterAddress, sizeof(RegisterAddress), PacketBuffer, Length);
		}
	
	/* External Variables: */
		extern Sensor_t Sensors;
		
#endif
