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
 *  Common definitions for the sensor driver platform.
 */

#ifndef _SENSORS_COMMON_H_
#define _SENSORS_COMMON_H_

	/* Includes: */
		#include <avr/pgmspace.h>
	
		#include <stdint.h>
		#include <stdbool.h>

		#include <LUFA/Drivers/Peripheral/TWI.h>

	/* Macros: */
		/** Timeout period in milliseconds allowable to capture a sensor on the I2C bus, before the packet is aborted. */
		#define SENSOR_BUS_TIMEOUT_MS   10
		
		/** Sign-extends a value from a given number of bits to the given type.
		 *
		 *  \param[in] Value     Value to sign-extend.
		 *  \param[in] FromBits  Number of bits of the signed input value.
		 *  \param[in] ToType    Name of the output signed integer type.
		 *
		 *  \return Sign-extended version of the input value.
		 */
		#define SIGN_EXTEND(Value, FromBits, ToType) (ToType)(((Value) & (1 << ((FromBits) - 1))) ? ((Value) | ((ToType)-1 & ~((1 << (FromBits)) - 1))) : (Value))

	/* Type Defines: */
		/** Type define for a sensor descriptor structure, passed to the individual sensor driver functions as needed to
		 *  get and set sensor data.
		 */
		typedef struct
		{
			bool  Connected; /**< Indicates if the current sensor is connected or not. */
			const char* Name; /**< Human-readable name of the sensor, stored in SRAM. */
			
			bool SingleAxis; /**< Indicates if the sensor data is represented as a single value, or a triplicate of three axis values. */
			
			union
			{
				int32_t Single; /**< Sensor data if the sensor outputs a single axis value. */
			
				struct
				{
					int16_t X; /**< X axis value of the triplicate. */
					int16_t Y; /**< Y axis value of the triplicate. */
					int16_t Z; /**< Z axis value of the triplicate. */
				} Triplicate; /**< Sensor data if the sensor outputs a triplicate of three axis values. */
			} Data; /**< Last retrieved data from the sensor. */
		} SensorData_t;
		
		/** Type define for a structure which contains all the possible sensor descriptors for the robot. */
		typedef struct
		{
			SensorData_t Direction; /**< Robot direction (compass) sensor descriptor. */
			SensorData_t Acceleration; /**< Robot acceleration sensor descriptor. */
			SensorData_t Orientation; /**< Robot orientation (gyroscope) sensor descriptor. */
			SensorData_t Pressure;  /**< Robot air pressure sensor descriptor. */
			SensorData_t Temperature; /**< Robot temperature sensor descriptor. */
		} ATTR_PACKED Sensor_t;
	
	/* Inline Functions */
		/** Writes a given buffer of bytes to the given sensor, over the I2C bus.
		 *
		 *  \param[in] SensorTWIAddress  Address of the sensor on the TWI bus.
		 *  \param[in] RegisterAddress   Address of the first register to write to within the device.
		 *  \param[in] PacketBuffer      Pointer to a buffer where the data to send is stored.
		 *  \param[in] Length            Length of the data to send, in bytes.
		 *
		 *  \return Error code from the TWI driver.
		 */
		static inline uint8_t Sensor_WriteBytes(const uint8_t SensorTWIAddress,
		                                        const uint8_t RegisterAddress,
		                                        const uint8_t* const PacketBuffer,
		                                        const uint8_t Length)
		{
			return TWI_WritePacket(SensorTWIAddress, SENSOR_BUS_TIMEOUT_MS, &RegisterAddress, sizeof(RegisterAddress), PacketBuffer, Length);
		}

		/** Reads a given buffer of bytes from the given sensor, over the I2C bus.
		 *
		 *  \param[in] SensorTWIAddress  Address of the sensor on the TWI bus.
		 *  \param[in] RegisterAddress   Address of the first register to read from within the device.
		 *  \param[in] PacketBuffer      Pointer to a buffer where the received data is to be stored.
		 *  \param[in] Length            Length of the data to read, in bytes.
		 *
		 *  \return Error code from the TWI driver.
		 */
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
