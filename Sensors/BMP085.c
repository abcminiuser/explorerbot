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

#include "BMP085.h"

static void BMP085_StartConversion(SensorData_t* const PressureSensorInfo)
{
	uint8_t RegisterAddress;
	uint8_t PacketBuffer[1];

	/* Abort if Compass sensor not connected and initialized */
	if (!(PressureSensorInfo->Connected))
	  return;

	/* Write to the control register to initiate the next single conversion */
	RegisterAddress = BMP085_CONTROL_REG;
	PacketBuffer[0] = BMP085_CONTROL_CMD_PRESSURE;
	TWI_WritePacket(BMP085_ADDRESS, 100, &RegisterAddress, 1, PacketBuffer, 1);
}

void BMP085_Init(SensorData_t* const PressureSensorInfo)
{
	uint8_t RegisterAddress;
	uint8_t PacketBuffer[1];

	/* Sensor considered not connected until it has been sucessfully initialized */
	PressureSensorInfo->Connected = false;

	/* Attempt to read the sensor's ID register, return error if sensor cannot be communicated with */
	RegisterAddress = BMP085_CHIP_ID_REG;
	if (TWI_ReadPacket(BMP085_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	/* Verify the returned sensor ID against the expected sensor ID */
	PressureSensorInfo->Connected = (PacketBuffer[0] == BMP085_CHIP_ID);

	/* Abort if sensor not connected and initialized */
	if (!(PressureSensorInfo->Connected))
	  return;

	/* Try to trigger the first conversion in the sensor */
	BMP085_StartConversion(PressureSensorInfo);
}

void BMP085_Update(SensorData_t* const PressureSensorInfo)
{
	uint8_t PacketBuffer[3];
	uint8_t RegisterAddress;

	/* Abort if sensor not connected and initialized */
	if (!(PressureSensorInfo->Connected))
	  return;

	/* Wait for sensor interrupt line to go high to signal end of conversion */
	while (!(PIND & (1 << 2)));
	
	/* Read the converted sensor data as a block packet */
	RegisterAddress = BMP085_CONVERSION_REG_MSB;
	if (TWI_ReadPacket(BMP085_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 3) != TWI_ERROR_NoError)
	  return;

	/* Save updated sensor data */
	PressureSensorInfo->Data.Single = (((uint32_t)PacketBuffer[0] << 16) | ((uint32_t)PacketBuffer[1] << 8) | PacketBuffer[0]);
		
	/* Start next sensor data conversion */
	BMP085_StartConversion(PressureSensorInfo);
}

