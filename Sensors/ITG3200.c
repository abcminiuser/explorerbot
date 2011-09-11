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

#include "ITG3200.h"

static const char* SensorNameGyro PROGMEM = "Gyroscope";
static const char* SensorNameTemp PROGMEM = "Temperature";
static SensorData_t GyroZeroOffset;

void ITG3200_Init(SensorData_t* const GyroSensorInfo,
		          SensorData_t* const TempSensorInfo)
{
	uint8_t PacketBuffer[1];

	/* Sensor considered not connected until it has been sucessfully initialized */
	GyroSensorInfo->Connected = false;

	/* Configure sensor information settings */
	GyroSensorInfo->SingleAxis = false;
	GyroSensorInfo->Name       = SensorNameGyro;

	if (TempSensorInfo)
	{
		/* Temperature sensor connectivity should match the physical sensor connectivity */
		TempSensorInfo->Connected = false;

		/* Configure sensor information settings */
		TempSensorInfo->SingleAxis = true;
		TempSensorInfo->Name       = SensorNameTemp;
	}

	/* Attempt to read the sensor's ID register, return error if sensor cannot be communicated with */
	if (Sensor_ReadBytes(ITG3200_ADDRESS, ITG3200_WHOAMI_REG, PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	/* Verify the returned sensor ID against the expected sensor ID */
	GyroSensorInfo->Connected = (PacketBuffer[0] == ITG3200_CHIP_ID);
	
	/* Temperature sensor connectivity should match the physical sensor connectivity */
	if (TempSensorInfo)
	  TempSensorInfo->Connected = GyroSensorInfo->Connected;
	
	/* Abort if sensor not connected and initialized */
	if (!(GyroSensorInfo->Connected))
	  return;

	/* Force reset of the sensor */
	PacketBuffer[0] = 0x80;
	if (Sensor_WriteBytes(ITG3200_ADDRESS, ITG3200_PWR_M_REG, PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	/* Set the update rate to 100Hz at an internal sampling rate of 1KHz */
	PacketBuffer[0] = ((1000 / 100) - 1);
	if (Sensor_WriteBytes(ITG3200_ADDRESS, ITG3200_SMPLRT_REG, PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	/* Set Low Pass Filter to use 20Hz bandwidth */
	PacketBuffer[0] = ((3 << 3) | 5);
	if (Sensor_WriteBytes(ITG3200_ADDRESS, ITG3200_DLPF_FS_REG, PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	/* Enable data ready interrupt line notifications */
	PacketBuffer[0] = (ITG3200_CFG_RAWRDY | ITG3200_CFG_ANYRDCLR | ITG3200_CFG_LATCHINT);
	if (Sensor_WriteBytes(ITG3200_ADDRESS, ITG3200_INT_CFG_REG, PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	/* Set the clock reference to use the Gyroscope X axis PLL as the clock source */
	PacketBuffer[0] = 0x01;
	if (Sensor_WriteBytes(ITG3200_ADDRESS, ITG3200_PWR_M_REG, PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;
}

void ITG3200_ZeroCalibrate(SensorData_t* const GyroSensorInfo)
{
	/* Abort if sensor not connected and initialized */
	if (!(GyroSensorInfo->Connected))
	  return;

	memset(&GyroZeroOffset, 0x00, sizeof(GyroZeroOffset));
	GyroZeroOffset.Connected = true;
	
	ITG3200_Update(&GyroZeroOffset, NULL);
}

void ITG3200_Update(SensorData_t* const GyroSensorInfo,
		            SensorData_t* const TempSensorInfo)
{
	uint8_t PacketBuffer[8];

	/* Abort if sensor not connected and initialized */
	if (!(GyroSensorInfo->Connected))
	  return;

	/* Wait for sensor interrupt line to go high to signal end of conversion */
	while (!(PINB & (1 << 0)));

	/* Read the converted sensor data as a block packet */
	if (Sensor_ReadBytes(ITG3200_ADDRESS, ITG3200_TMP_H_REG, PacketBuffer, 8) != TWI_ERROR_NoError)
	  return;

	/* Update temperature sensor data */
	if (TempSensorInfo)
	  TempSensorInfo->Data.Single = (35 + ((13200 + (((int16_t)PacketBuffer[0] << 8) | PacketBuffer[1])) / 280));

	/* Save updated sensor data */
	GyroSensorInfo->Data.Triplicate.X = (((uint16_t)PacketBuffer[2] << 8) | PacketBuffer[3]) - GyroZeroOffset.Data.Triplicate.X;
	GyroSensorInfo->Data.Triplicate.Y = (((uint16_t)PacketBuffer[4] << 8) | PacketBuffer[5]) - GyroZeroOffset.Data.Triplicate.Y;
	GyroSensorInfo->Data.Triplicate.Z = (((uint16_t)PacketBuffer[6] << 8) | PacketBuffer[7]) - GyroZeroOffset.Data.Triplicate.Z;
}

