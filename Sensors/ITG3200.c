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

void ITG3200_Init(SensorData_t* const SensorInfo)
{
	uint8_t RegisterAddress;
	uint8_t PacketBuffer[1];

	/* Sensor considered not connected until it has been sucessfully initialized */
	SensorInfo->Connected = false;

	/* Attempt to read the sensor's ID register, return error if sensor cannot be communicated with */
	RegisterAddress = ITG3200_WHOAMI_REG;
	if (TWI_ReadPacket(ITG3200_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;
	PacketBuffer[0] = ITG3200_CHIP_ID;

	/* Verify the returned sensor ID against the expected sensor ID */
	SensorInfo->Connected = (PacketBuffer[0] == ITG3200_CHIP_ID);
	
	if (!(SensorInfo->Connected))
	  return;

	/* Force reset of the sensor */
	RegisterAddress = ITG3200_PWR_M_REG;
	PacketBuffer[0] = 0x80;
	if (TWI_WritePacket(ITG3200_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	/* Set the update rate to 100Hz at an internal sampling rate of 1KHz */
	RegisterAddress = ITG3200_SMPLRT_REG;
	PacketBuffer[0] = ((1000 / 100) - 1);
	if (TWI_WritePacket(ITG3200_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	/* Set Low Pass Filter to use 20Hz bandwidth */
	RegisterAddress = ITG3200_DLPF_FS_REG;
	PacketBuffer[0] = ((3 << 3) | 5);
	if (TWI_WritePacket(ITG3200_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;	

	/* Set the clock reference to use the Gyroscope X axis PLL as the clock source */
	RegisterAddress = ITG3200_PWR_M_REG;
	PacketBuffer[0] = 0x01;
	if (TWI_WritePacket(ITG3200_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;
}

void ITG3200_Update(SensorData_t* const SensorInfo)
{
	uint8_t PacketBuffer[6];
	uint8_t RegisterAddress;

	/* Abort if Compass sensor not connected and initialized */
	if (!(SensorInfo->Connected))
	  return;

	/* Read the converted sensor data as a block packet */
	RegisterAddress = ITG3200_GX_H_REG;
	if (TWI_ReadPacket(ITG3200_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 6) != TWI_ERROR_NoError)
	  return;		  

	/* Save updated sensor data */
	SensorInfo->Data.Triplicate.X = (((uint16_t)PacketBuffer[0] << 8) | PacketBuffer[1]);
	SensorInfo->Data.Triplicate.Y = (((uint16_t)PacketBuffer[2] << 8) | PacketBuffer[3]);
	SensorInfo->Data.Triplicate.Z = (((uint16_t)PacketBuffer[4] << 8) | PacketBuffer[5]);
}

