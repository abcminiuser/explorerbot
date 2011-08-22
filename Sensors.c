/*
             Bluetooth Robot
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.org
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

#include "Sensors.h"

void Sensors_Init(void)
{
	DDRB &= ~((1 << 0) | (1 << 1) | (1 << 2));
	DDRB |=  (1 << 3);
	DDRD &= ~(1 << 2);
	
	PORTB |= (1 << 3);
	
	TWI_Init(TWI_BIT_PRESCALE_4, (F_CPU / 4 / 10000) / 2);
}

uint16_t Sensors_CheckSensors(void)
{
	uint8_t ReturnedID;
	const struct
	{
		uint8_t Address;
		uint8_t IDRegister;
		uint8_t ExpectedID;
	} SensorInfo[] =
		{
			{SENSOR_Pressure,      0xD0, 0x55},
			{SENSOR_Compass,       0x00, 0x48},
			{SENSOR_Accelerometer, 0x00, 0x02},
			{SENSOR_Gyroscope,     0x00, 0x69},
		};
	
	Delay_MS(100);
	
	for (uint8_t i = 0; i < (sizeof(SensorInfo) / sizeof(SensorInfo[0])); i++)
	{
		/* Attempt to read the current sensor's ID register, return error if sensor cannot be communicated with */
		if (TWI_ReadPacket(SensorInfo[i].Address, 100, &SensorInfo[i].IDRegister, sizeof(uint8_t), &ReturnedID, sizeof(ReturnedID)) != TWI_ERROR_NoError)
		  return SensorInfo[i].Address;
		
		/* Verify the returned sensor ID against the expected sensor ID */
		if (ReturnedID != SensorInfo[i].ExpectedID)
		  return (SENSOR_ERROR_ID | SensorInfo[i].Address);
	}
	
	return 0;
}
