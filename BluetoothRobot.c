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

#include "BluetoothRobot.h"

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
	SetupHardware();
	//StartupSequence();
	//CheckSensors();

	EVENT_USB_Host_DeviceUnattached();
	sei();

	for (;;)
	{
		uint8_t ButtonStatus = Buttons_GetStateMask();

		/* Mode action button */
		if (ButtonStatus & BUTTON1_MASK)
		{
			if (Disk_MS_Interface.State.IsActive)
			{
				MassStorage_SensorLoggingEnabled = true;
			
				LCD_Clear();
				LCD_WriteString_P(PSTR("  Disk Sensor\n"
				                       "Logging Enabled"));
			}
			else if (BluetoothAdapter_IsActive)
			{
				static BT_HCI_Connection_t* RemoteConnection = NULL;
				
				if (!(RemoteConnection) || (RemoteConnection->State == HCI_CONSTATE_Closed))
				{
					RemoteConnection = BluetoothAdapter_ConnectToRemoteDevice();
					
					LCD_Clear();
					LCD_WriteString_P(PSTR("Connecting to:\n"));
					LCD_WriteBDADDR(RemoteConnection->RemoteBDADDR);
					
					Speaker_PlaySequence(SPEAKER_SEQUENCE_Connecting);
				}
			}
		}
		  
		/* System reset button */
		if (ButtonStatus & BUTTON2_MASK)
		{
			Motors_SetChannelSpeed(0, 0);
			USB_Disable();
			
			wdt_enable(WDTO_15MS);
			for(;;);
		}

		/* Check if the system update interval has elapsed */
		if (TIFR3 & (1 << OCF3A))
		{
			/* Clear the timer compare flag */
			TIFR3 |= (1 << OCF3A);
		
			static uint8_t SensorTicksElapsed = 0;
			
			/* Update sensors every 250ms and log to disk */
			if (SensorTicksElapsed++ == 25)
			{
				SensorTicksElapsed = 0;
				
				/* Update all connected sensors' values */
				Sensors_Update();
				
				char    LineBuffer[200];
				uint8_t LineLength;
				
				/* Construct the next sensor CSV line from the current sensor data */
				Sensors_WriteSensorDataCSV(LineBuffer);
				LineLength = strlen(LineBuffer);
				
				/* Write the sensor data to the attached Mass Storage disk (if available and logging enabled) */
				if (Disk_MS_Interface.State.IsActive && MassStorage_SensorLoggingEnabled)
				{
					uint16_t BytesWritten;
					f_write(&MassStorage_DiskLogFile, LineBuffer, LineLength, &BytesWritten);
					f_sync(&MassStorage_DiskLogFile);
				}

				/* Write the sensor data to the attached wireless serial port (if available) */
				if (RFCOMM_SensorStream)
				  RFCOMM_SendData(RFCOMM_SensorStream, LineLength, LineBuffer);				
			}
			
			/* Update the currently playing tone through the speaker (if one is playing) */
			Speaker_NextSequenceTone();
		}
		
		BluetoothAdapter_USBTask();
		Joystick_USBTask();
		MassStorage_USBTask();
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Disable JTAG to allow RGB LEDs to work */
	JTAG_DISABLE();
	
	/* Disable unused peripheral modules */
	PRR0 = ((1 << PRADC)  | (1 << PRSPI));
	PRR1 = (1 << PRUSART1);
	
	/* Enable system update tick timer (10ms) */
	TCCR3B = ((1 << WGM32) | (1 << CS31) | (1 << CS30));
	OCR3A  = ((F_CPU / 64) / 100);

	/* Hardware Initialization */
	ExternalSRAM_Init();
	Buttons_Init();
	Headlights_Init();
	LCD_Init();
	Motors_Init();
	RGB_Init();
	Sensors_Init();
	Speaker_Init();
	USB_Init();
}

/** System startup sequence, to test system hardware and display welcome/startup message to the user. */
void StartupSequence(void)
{
	const uint8_t ColourMap[] = {RGB_COLOUR_Green, RGB_COLOUR_Cyan,   RGB_COLOUR_Blue,  RGB_COLOUR_Magenta,
	                             RGB_COLOUR_Red,   RGB_COLOUR_Yellow, RGB_COLOUR_White, RGB_COLOUR_Off};

	LCD_Clear();
	LCD_WriteString_P(PSTR("Bluetooth  Robot\n"
	                       " By Dean Camera "));

	/* Fade in LCD backlight to half maximum brightness to save power */
	for (uint8_t i = 0; i < (0xFF / 2); i++)
	{
		LCD_SetBacklight(i);
		Delay_MS(5);
	}
	
	/* Cycle through the RGB status LED colours */
	for (uint8_t i = 0; i < sizeof(ColourMap); i++)
	{
		RGB_SetColour(ColourMap[i]);
		Delay_MS(150);
	}
}

/** Check the board sensors to ensure that they are attached and operating correctly. */
void CheckSensors(void)
{
	static const uint8_t LCDIcon_Cross[] PROGMEM = {0x00, 0x1B, 0x0E, 0x04, 0x0E, 0x1B, 0x00, 0x00};
	static const uint8_t LCDIcon_Tick[]  PROGMEM = {0x00, 0x01, 0x03, 0x16, 0x1C, 0x08, 0x00, 0x00};
	
	LCD_SetCustomChar('\1', LCDIcon_Cross);
	LCD_SetCustomChar('\2', LCDIcon_Tick);
	
	LCD_Clear();
	LCD_WriteFormattedString_P(PSTR(" CMP %c   ACC %c\n"
	                                " GYR %c   PRS %c"), (Sensors.Direction.Connected    ? '\2' : '\1'),
	                                                     (Sensors.Acceleration.Connected ? '\2' : '\1'),
	                                                     (Sensors.Orientation.Connected  ? '\2' : '\1'),
	                                                     (Sensors.Pressure.Connected     ? '\2' : '\1'));
												 
	Delay_MS(1500);
}

/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(void)
{
	LCD_Clear();
	LCD_WriteString_P(PSTR("* Enumerating *"));
	RGB_SetColour(RGB_ALIAS_Enumerating);
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(void)
{
	Motors_SetChannelSpeed(0, 0);

	LCD_Clear();
	LCD_WriteString_P(PSTR(" * Insert USB *"));
	RGB_SetColour(RGB_ALIAS_Disconnected);
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(void)
{
	USB_Descriptor_Device_t DeviceDescriptor;
	uint16_t ConfigDescriptorSize;
	uint8_t  ConfigDescriptorData[512];

	LCD_Clear();
	LCD_WriteString_P(PSTR("* Configuring *"));

	if (USB_Host_GetDeviceDescriptor(&DeviceDescriptor) != HOST_SENDCONTROL_Successful)
	{
		LCD_Clear();
		LCD_WriteString_P(PSTR("ERR: Dev Desc"));
		RGB_SetColour(RGB_ALIAS_Error);
		return;
	}

	if (USB_Host_GetDeviceConfigDescriptor(1, &ConfigDescriptorSize, ConfigDescriptorData,
	                                       sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful)
	{
		LCD_Clear();
		LCD_WriteString_P(PSTR("ERR: Config Desc"));
		RGB_SetColour(RGB_ALIAS_Error);
		return;
	}

	if (!(Joystick_ConfigurePipes(&DeviceDescriptor, ConfigDescriptorSize, ConfigDescriptorData)) &&
	    !(BluetoothAdapter_ConfigurePipes(&DeviceDescriptor, ConfigDescriptorSize, ConfigDescriptorData)) &&
	    !(MassStorage_ConfigurePipes(&DeviceDescriptor, ConfigDescriptorSize, ConfigDescriptorData)))
	{
		LCD_Clear();
		LCD_WriteString_P(PSTR("ERR: Unknown USB"));
		RGB_SetColour(RGB_ALIAS_Error);
		return;
	}

	if (USB_Host_SetDeviceConfiguration(1) != HOST_SENDCONTROL_Successful)
	{
		LCD_Clear();
		LCD_WriteString_P(PSTR("ERR: Set Config"));
		RGB_SetColour(RGB_ALIAS_Error);
		return;
	}
	
	if (!(Joystick_PostConfiguration()) || !(BluetoothAdapter_PostConfiguration()) || !(MassStorage_PostConfiguration()))
	{
		LCD_Clear();
		LCD_WriteString_P(PSTR("ERR: Post Config"));
		RGB_SetColour(RGB_ALIAS_Error);
		return;
	}
	
	LCD_Clear();
	LCD_WriteString_P(PSTR("* System Ready *"));
	LCD_SetCursor(2, 0);
	
	if (Joystick_HID_Interface.State.IsActive)
	  LCD_WriteString_P(PSTR("   (HID Mode)   "));
	else if (Disk_MS_Interface.State.IsActive)
	  LCD_WriteString_P(PSTR(" (MS Disk Mode) "));
	else if (BluetoothAdapter_IsActive)
	  LCD_WriteString_P(PSTR("(Bluetooth Mode)"));
	
	RGB_SetColour(RGB_ALIAS_Ready);
}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t ErrorCode)
{
	Motors_SetChannelSpeed(0, 0);
	USB_Disable();

	LCD_Clear();
	LCD_WriteString_P(PSTR("ERR: Host Error"));
	RGB_SetColour(RGB_ALIAS_Error);
	for(;;);
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t ErrorCode,
                                            const uint8_t SubErrorCode)
{
	LCD_Clear();
	LCD_WriteString_P(PSTR("ERR: Device Enum"));
	RGB_SetColour(RGB_ALIAS_Error);
}
