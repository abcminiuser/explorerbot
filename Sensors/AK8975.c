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

#include "AK8975.h"

static void AK8975_StartConversion(SensorData_t* const SensorInfo)
{
	uint8_t RegisterAddress;
	uint8_t PacketBuffer[1];

	if (!(SensorInfo->Connected))
	  return;

	RegisterAddress = AK8975_REG_CNTL;
	PacketBuffer[0] = AK8975_REG_CNTL_MODE_ONCE;
	TWI_WritePacket(AK8975_ADDRESS, 100, &RegisterAddress, 1, PacketBuffer, 1);
}

void AK8975_Init(SensorData_t* const SensorInfo)
{
	uint8_t RegisterAddress;
	uint8_t PacketBuffer[1];

	// Sensor considered not connected until it has been sucessfully initialized
	SensorInfo->Connected = false;

	// Attempt to read the sensor's ID register, return error if sensor cannot be communicated with
	RegisterAddress = AK8975_REG_WIA;
	if (TWI_ReadPacket(AK8975_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	// Verify the returned sensor ID against the expected sensor ID
	SensorInfo->Connected = (PacketBuffer[0] == AK8975_DEVICE_ID);

	// Try to trigger the first conversion in the sensor
	AK8975_StartConversion(SensorInfo);
}

void AK8975_Update(SensorData_t* const SensorInfo)
{
	uint8_t PacketBuffer[6];
	uint8_t RegisterAddress;

	if (!(SensorInfo->Connected))
	  return;
	
	// Read the Compass sensor's status register
	RegisterAddress = AK8975_REG_ST1;
	if (TWI_ReadPacket(AK8975_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;
	
	// Check if new data is ready
	if (PacketBuffer[0] & AK8975_REG_ST1_DRDY_MASK)
	{
		// Read the converted sensor data
		RegisterAddress = AK8975_REG_HXL;
		
		for (uint8_t i = 0; i < 6; i++)
		{
			if (TWI_ReadPacket(AK8975_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), &PacketBuffer[i], 1) != TWI_ERROR_NoError)
			  return;
		
			RegisterAddress++;
		}

#if 0
		// Read the converted sensor data
		RegisterAddress = AK8975_REG_HXL;
		if (TWI_ReadPacket(AK8975_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 6) != TWI_ERROR_NoError)
		  return;		  
#endif

		// Save updated sensor data
		SensorInfo->Data.Triplicate.X = (((uint16_t)PacketBuffer[1] << 8) | PacketBuffer[0]);
		SensorInfo->Data.Triplicate.Y = (((uint16_t)PacketBuffer[3] << 8) | PacketBuffer[2]);
		SensorInfo->Data.Triplicate.Z = (((uint16_t)PacketBuffer[5] << 8) | PacketBuffer[4]);
		
		// Start next conversion
		AK8975_StartConversion(SensorInfo);
	}
}

