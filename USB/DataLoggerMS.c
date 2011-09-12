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

#include "DataloggerMS.h"

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all Mass Storage Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_MS_Host_t Datalogger_MS_Interface =
	{
		.Config =
			{
				.DataINPipeNumber       = 1,
				.DataINPipeDoubleBank   = false,

				.DataOUTPipeNumber      = 2,
				.DataOUTPipeDoubleBank  = false,
			},
	};

/** FAT Fs structure to hold the internal state of the FAT driver for the Dataflash contents. */
static FATFS DiskFATState;

/** FAT Fs structure to hold a FAT file handle for the log data write destination. */
static FIL DiskLogFile;


/** Attempts to configure the system pipes for the attached Mass Storage device.
 *
 *  \param[in] DeviceDescriptor      Pointer to the Device Descriptor read from the attached device
 *  \param[in] ConfigDescriptorSize  Size of the retrieved Configuration Descriptor from the device
 *  \param[in] ConfigDescriptorData  Pointer to the Configuration Descriptor read from the attached device
 *
 *  \return  Boolean true if a valid Mass Storage interface was found, false otherwise.
 */
bool Datalogger_ConfigurePipes(USB_Descriptor_Device_t* DeviceDescriptor,
                               uint16_t ConfigDescriptorSize,
                               void* ConfigDescriptorData)
{
	/* Attempt to bind to the attached device as a Mass Storage class interface */
	return (MS_Host_ConfigurePipes(&Datalogger_MS_Interface, ConfigDescriptorSize, ConfigDescriptorData) == HID_ENUMERROR_NoError);
}

/** Performs any post configuration tasks after the attached Mass Storage device has been successfully enumerated.
 *
 *  \return Boolean true if no Mass Storage device attached or if all post-configuration tasks complete without error, false otherwise
 */
bool Datalogger_PostConfiguration(void)
{
	if (!(Datalogger_MS_Interface.State.IsActive))
	  return true;

	uint8_t ErrorCode;
	
	f_mount(0, &DiskFATState);
	ErrorCode = f_open(&DiskLogFile, DATALOG_FILENAME, (FA_CREATE_NEW | FA_WRITE));

	/* See if the existing log was created sucessfully */
	if (ErrorCode == FR_OK)
	{
		uint16_t      BytesWritten = 0;
		SensorData_t* CurrSensor;
	
		/* Log file created, print out sensor names */
		CurrSensor = (SensorData_t*)&Sensors;
		for (uint8_t SensorIndex = 0; SensorIndex < (sizeof(Sensors) / sizeof(SensorData_t)); SensorIndex++)
		{
			/* Add seperator between sensor names as they are written to the log file */
			if (SensorIndex)
			  f_write(&DiskLogFile, ",", strlen(","), &BytesWritten);

			/* Output sensor name to the log file */
			f_write(&DiskLogFile, CurrSensor->Name, strlen(CurrSensor->Name), &BytesWritten);
			
			/* If the sensor is a triplicate, need to add in seperators to keep values aligned properly */
			if (!(CurrSensor->SingleAxis))
			  f_write(&DiskLogFile, ",,", strlen(",,"), &BytesWritten);
			
			/* Advance pointer to next sensor entry in the sensor structure */
			CurrSensor++;
		}

		/* Add end of line terminator */
		f_write(&DiskLogFile, "\r\n", strlen("\r\n"), &BytesWritten);

		/* Print out sensor axis */
		CurrSensor = (SensorData_t*)&Sensors;
		for (uint8_t SensorIndex = 0; SensorIndex < (sizeof(Sensors) / sizeof(SensorData_t)); SensorIndex++)
		{
			/* Add seperator between sensor names as they are written to the log file */
			if (SensorIndex)
			  f_write(&DiskLogFile, ",", strlen(","), &BytesWritten);

			/* If the sensor is a triplicate, need to add in seperators to keep values aligned properly */
			if (!(CurrSensor->SingleAxis))
			  f_write(&DiskLogFile, "X,Y,Z", strlen("X,Y,Z"), &BytesWritten);
			
			/* Advance pointer to next sensor entry in the sensor structure */
			CurrSensor++;
		}
		
		/* Add end of line terminator */
		f_write(&DiskLogFile, "\r\n", strlen("\r\n"), &BytesWritten);
	}
	else if (ErrorCode == FR_EXIST)
	{
		/* Open the already existing file on the disk */
		f_open(&DiskLogFile, DATALOG_FILENAME, (FA_OPEN_EXISTING | FA_WRITE));
	
		/* Seek to the end of the existing log file */
		f_lseek(&DiskLogFile, DiskLogFile.fsize);	
	}
	else
	{
		/* Return disk error */
		return false;
	}
	
	RGB_SetColour(RGB_ALIAS_Connected);
	return true;
}

/** Task to manage an enumerated Mass Storage device once connected. */
void Datalogger_USBTask(void)
{
	if ((USB_HostState != HOST_STATE_Configured) || !(Datalogger_MS_Interface.State.IsActive))
	  return;
	
	/* Run the Mass Storage Class Driver service task on the Mass Storage interface */
	MS_Host_USBTask(&Datalogger_MS_Interface);
}

/** Writes the current sensor values out to the attached Mass Storage device. */
void Datalogger_LogSensors(void)
{
	if ((USB_HostState != HOST_STATE_Configured) || !(Datalogger_MS_Interface.State.IsActive))
	  return;

	uint16_t      BytesWritten = 0;
	SensorData_t* CurrSensor   = (SensorData_t*)&Sensors;

	for (uint8_t SensorIndex = 0; SensorIndex < (sizeof(Sensors) / sizeof(SensorData_t)); SensorIndex++)
	{
		/* Add seperator between sensor names as they are written to the log file */
		if (SensorIndex)
		  f_write(&DiskLogFile, ", ", strlen(", "), &BytesWritten);

		char    TempBuffer[25];
		uint8_t TempBufferLen;
		
		/* Print the current sensor data into the temporary buffer */
		if (CurrSensor->SingleAxis)
		  TempBufferLen = sprintf(TempBuffer, "%ld", CurrSensor->Data.Single);
		else
		  TempBufferLen = sprintf(TempBuffer, "%d, %d, %d", CurrSensor->Data.Triplicate.X, CurrSensor->Data.Triplicate.Y, CurrSensor->Data.Triplicate.Z);

		/* Output sensor name to the log file */
		f_write(&DiskLogFile, TempBuffer, TempBufferLen, &BytesWritten);			
		
		/* Advance pointer to next sensor entry in the sensor structure */
		CurrSensor++;
	}

	f_write(&DiskLogFile, "\r\n", strlen("\r\n"), &BytesWritten);
	f_sync(&DiskLogFile);
}
