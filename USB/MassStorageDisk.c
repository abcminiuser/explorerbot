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
 *  USB transport management for USB Mass Storage devices.
 */

#include "MassStorageDisk.h"

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all Mass Storage Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_MS_Host_t Disk_MS_Interface =
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
static FATFS MassStorage_DiskFATState;

/** FAT Fs structure to hold a FAT file handle for the log data write destination. */
FIL MassStorage_DiskLogFile;

/** Flag to indicate if sensor logging to disk is currently enabled or not. */
bool MassStorage_SensorLoggingEnabled = false;


/** Opens the sensor log file on the attached Mass Storage disk, seeking to the end of the
 *  existing file (if found) or writing out the sensor log header if not.
 *
 *  \return Boolean true if an existing log file was opened (or a new one created), false otherwise.
 */
static bool MassStorage_OpenSensorLogFile(void)
{
	uint8_t ErrorCode;
	
	/* Create a new file on the disk to log the sensor data to, fail if one already exists */
	ErrorCode = f_open(&MassStorage_DiskLogFile, DATALOG_FILENAME, (FA_CREATE_NEW | FA_WRITE));

	/* See if the existing log was created sucessfully */
	if (ErrorCode == FR_OK)
	{
		/* Construct the sensor CSV header line(s) */
		char     LineBuffer[200];
		uint8_t  LineLength = Sensors_WriteSensorCSVHeader(LineBuffer);

		/* Write constructed CSV header to the attached mass storage disk */
		uint16_t BytesWritten = 0;
		f_write(&MassStorage_DiskLogFile, LineBuffer, LineLength, &BytesWritten);
	}
	else if (ErrorCode == FR_EXIST)
	{
		/* Open the already existing file on the disk */
		f_open(&MassStorage_DiskLogFile, DATALOG_FILENAME, (FA_OPEN_EXISTING | FA_WRITE));
	
		/* Seek to the end of the existing log file */
		f_lseek(&MassStorage_DiskLogFile, MassStorage_DiskLogFile.fsize);	
	}
	else
	{
		/* Return disk error */
		return false;
	}
	
	return true;
}

/** Opens and parses the existing Remote Bluetooth Device Address file stored on the attached Mass Storage
 *  device, if one is available. If not, a new file is created and populated with the currently stored
 *  address.
 *
 *  \return Boolean true if the file was read or created sucessfully, false otherwise.
 */
static bool MassStorage_ParseRemoteBDADDRFile(void)
{
	uint8_t ErrorCode;
	FIL     DiskRemAddrFile;

	ErrorCode = f_open(&DiskRemAddrFile, REMADDR_FILENAME, (FA_OPEN_EXISTING | FA_READ));

	if (ErrorCode == FR_OK)
	{
		uint16_t BytesRead = 0;
		char     LineBuffer[50];
		
		/* Fetch the remote device's BDADDR from the text file */
		f_read(&DiskRemAddrFile, LineBuffer, sizeof(LineBuffer), &BytesRead);
		
		/* Parse the file's hexadecimal data to retrieve the address octets */
		int RemoteBDADDR[BT_BDADDR_LEN];
		sscanf(LineBuffer, "%x:%x:%x:%x:%x:%x", &RemoteBDADDR[0], &RemoteBDADDR[1], &RemoteBDADDR[2], &RemoteBDADDR[3], &RemoteBDADDR[4], &RemoteBDADDR[5]);

		/* Save the remote device address to EEPROM for later use - note, must loop per address octet, to discard upper byte from each 16-bit integer */
		for (uint8_t i = 0; i < BT_BDADDR_LEN; i++)
		  eeprom_update_byte(&BluetoothAdapter_RemoteBDADDR[i], RemoteBDADDR[i]);

		f_close(&DiskRemAddrFile);
	}
	else if (ErrorCode == FR_NO_FILE)
	{
		uint8_t  RemoteBDADDR[BT_BDADDR_LEN];
		char     LineBuffer[50];
		uint16_t BytesWritten = 0;

		/* Retrieve the remote device's BDADDR saved in EEPROM, attempt a connection */
		eeprom_read_block(RemoteBDADDR, BluetoothAdapter_RemoteBDADDR, BT_BDADDR_LEN);

		ErrorCode = f_open(&DiskRemAddrFile, REMADDR_FILENAME, (FA_CREATE_NEW | FA_WRITE));
		
		/* Write the saved remote device address to the created file on the attached disk */
		sprintf(LineBuffer, "%02X:%02X:%02X:%02X:%02X:%02X", RemoteBDADDR[0], RemoteBDADDR[1], RemoteBDADDR[2], RemoteBDADDR[3], RemoteBDADDR[4], RemoteBDADDR[5]);		
		f_write(&DiskRemAddrFile, LineBuffer, strlen(LineBuffer), &BytesWritten);
		
		f_close(&DiskRemAddrFile);
	}
	else
	{
		return false;
	}
	
	return true;
}

/** Attempts to configure the system pipes for the attached Mass Storage device.
 *
 *  \param[in] DeviceDescriptor      Pointer to the Device Descriptor read from the attached device
 *  \param[in] ConfigDescriptorSize  Size of the retrieved Configuration Descriptor from the device
 *  \param[in] ConfigDescriptorData  Pointer to the Configuration Descriptor read from the attached device
 *
 *  \return  Boolean true if a valid Mass Storage interface was found, false otherwise.
 */
bool MassStorage_ConfigurePipes(USB_Descriptor_Device_t* const DeviceDescriptor,
                               uint16_t ConfigDescriptorSize,
                               void* ConfigDescriptorData)
{
	MassStorage_SensorLoggingEnabled = false;

	/* Attempt to bind to the attached device as a Mass Storage class interface */
	return (MS_Host_ConfigurePipes(&Disk_MS_Interface, ConfigDescriptorSize, ConfigDescriptorData) == HID_ENUMERROR_NoError);
}

/** Performs any post configuration tasks after the attached Mass Storage device has been successfully enumerated.
 *
 *  \return Boolean true if no Mass Storage device attached or if all post-configuration tasks complete without error, false otherwise
 */
bool MassStorage_PostConfiguration(void)
{
	if (!(Disk_MS_Interface.State.IsActive))
	  return true;

	f_mount(0, &MassStorage_DiskFATState);	

	/* Try to read in the data file containing the remote Bluetooth device address to connect to on demand */
	if (!(MassStorage_ParseRemoteBDADDRFile()))
	  return false;

	/* Abort if the sensor log file could not be opened */
	if (!(MassStorage_OpenSensorLogFile()))
	  return false;

	RGB_SetColour(RGB_ALIAS_Connected);
	return true;
}

/** Task to manage an enumerated Mass Storage device once connected. */
void MassStorage_USBTask(void)
{
	if ((USB_HostState != HOST_STATE_Configured) || !(Disk_MS_Interface.State.IsActive))
	  return;
	
	/* Run the Mass Storage Class Driver service task on the Mass Storage interface */
	MS_Host_USBTask(&Disk_MS_Interface);
}

