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

static void BMP085_StartConversion(SensorData_t* const SensorInfo)
{
	uint8_t RegisterAddress;
	uint8_t PacketBuffer[1];

	/* Abort if Compass sensor not connected and initialized */
	if (!(SensorInfo->Connected))
	  return;
	
	/* TODO */
}

void BMP085_Init(SensorData_t* const SensorInfo)
{
	uint8_t RegisterAddress;
	uint8_t PacketBuffer[1];

	/* Sensor considered not connected until it has been sucessfully initialized */
	SensorInfo->Connected = false;

	/* Attempt to read the sensor's ID register, return error if sensor cannot be communicated with */
	RegisterAddress = BMP085_CHIP_ID_REG;
	if (TWI_ReadPacket(BMP085_ADDRESS, 100, &RegisterAddress, sizeof(uint8_t), PacketBuffer, 1) != TWI_ERROR_NoError)
	  return;

	/* Verify the returned sensor ID against the expected sensor ID */
	SensorInfo->Connected = (PacketBuffer[0] == BMP085_CHIP_ID);

	/* Try to trigger the first conversion in the sensor */
	BMP085_StartConversion(SensorInfo);
}

void BMP085_Update(SensorData_t* const SensorInfo)
{
	uint8_t PacketBuffer[6];
	uint8_t RegisterAddress;

	/* Abort if Compass sensor not connected and initialized */
	if (!(SensorInfo->Connected))
	  return;

	/* TODO */
}

