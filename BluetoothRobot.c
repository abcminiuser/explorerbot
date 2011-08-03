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

#include "BluetoothRobot.h"

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
	SetupHardware();
	
	for (uint8_t i = 0; i < 0xFF; i++)
	{
		LCD_SetBacklight(i);
		Speaker_Tone(i);

		Delay_MS(3);
	}

	LCD_WriteString_P(PSTR("Bluetooth  Robot"));
	LCD_SetCursor(2, 0);
	LCD_WriteString_P(PSTR(" By Dean Camera "));
	
	Speaker_Tone(0);
	RGB_SetColour(RGB_ALIAS_Disconnected);
	sei();

	for (;;)
	{
		Joystick_USBTask();
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
	MCUCR |= (1 << JTD);
	MCUCR |= (1 << JTD);

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

/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(void)
{
	RGB_SetColour(RGB_ALIAS_Enumerating);
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(void)
{
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

	if (USB_Host_GetDeviceDescriptor(&DeviceDescriptor) != HOST_SENDCONTROL_Successful)
	{
		RGB_SetColour(RGB_ALIAS_Error);
		return;
	}

	if (USB_Host_GetDeviceConfigDescriptor(1, &ConfigDescriptorSize, ConfigDescriptorData,
	                                       sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful)
	{
		RGB_SetColour(RGB_ALIAS_Error);
		return;
	}

	if (!(Joystick_ConfigurePipes(&DeviceDescriptor, ConfigDescriptorSize, ConfigDescriptorData)) &&
	    !(Bluetooth_ConfigurePipes(&DeviceDescriptor, ConfigDescriptorSize, ConfigDescriptorData)))
	{
		RGB_SetColour(RGB_ALIAS_UnknownDevice);
		return;
	}

	if (USB_Host_SetDeviceConfiguration(1) != HOST_SENDCONTROL_Successful)
	{
		RGB_SetColour(RGB_ALIAS_Error);
		return;
	}
	
	if (!(Joystick_PostConfiguration()) || !(Bluetooth_PostConfiguration()))
	{
		RGB_SetColour(RGB_ALIAS_Error);
		return;
	}
	
	RGB_SetColour(RGB_ALIAS_Ready);
}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t ErrorCode)
{
	USB_Disable();
	RGB_SetColour(RGB_ALIAS_Error);

	for(;;);
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t ErrorCode,
                                            const uint8_t SubErrorCode)
{
	RGB_SetColour(RGB_ALIAS_Error);
}
