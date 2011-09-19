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

#include "JoystickHID.h"

/** Processed HID report descriptor items structure, containing information on each HID report element */
static HID_ReportInfo_t HIDReportInfo;

/** Indicates if the attached controller is a PS3 game controller device */
static bool Joystick_IsPS3Controller;

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Host_t Joystick_HID_Interface =
	{
		.Config =
			{
				.DataINPipeNumber       = 1,
				.DataINPipeDoubleBank   = false,

				.DataOUTPipeNumber      = 2,
				.DataOUTPipeDoubleBank  = false,

				.HIDInterfaceProtocol   = HID_CSCP_NonBootProtocol,

				.HIDParserData          = &HIDReportInfo
			},
	};

/** HID report item mappings from physical buttons to logical functions of the robot. */
static struct
{
	HID_ReportItem_t* Forward;
	HID_ReportItem_t* Left;
	HID_ReportItem_t* Right;
	HID_ReportItem_t* Backward;
	HID_ReportItem_t* Headlights;
	HID_ReportItem_t* Speaker;
} Joystick_HIDReportItemMappings;


/** Attempts to configure the system pipes for the attached HID Joystick.
 *
 *  \param[in] DeviceDescriptor      Pointer to the Device Descriptor read from the attached device
 *  \param[in] ConfigDescriptorSize  Size of the retrieved Configuration Descriptor from the device
 *  \param[in] ConfigDescriptorData  Pointer to the Configuration Descriptor read from the attached device
 *
 *  \return  Boolean true if a valid HID Joystick interface was found, false otherwise.
 */
bool Joystick_ConfigurePipes(USB_Descriptor_Device_t* DeviceDescriptor,
                             uint16_t ConfigDescriptorSize,
                             void* ConfigDescriptorData)
{
	/* Destroy any existing HID report item mappings */
	memset(&Joystick_HIDReportItemMappings, 0x00, sizeof(Joystick_HIDReportItemMappings));

	/* Detect PS3 Controllers seperately, pairing and other operations can be used on them */
	Joystick_IsPS3Controller = ((DeviceDescriptor->VendorID == PS3CONTROLLER_VID) && (DeviceDescriptor->ProductID == PS3CONTROLLER_PID));

	/* Attempt to bind to the attached device as a HID class interface */
	return (HID_Host_ConfigurePipes(&Joystick_HID_Interface, ConfigDescriptorSize, ConfigDescriptorData) == HID_ENUMERROR_NoError);
}

/** Performs any post configuration tasks after the attached HID joystick has been successfully enumerated.
 *
 *  \return Boolean true if no HID Joystick attached or if all post-configuration tasks complete without error, false otherwise
 */
bool Joystick_PostConfiguration(void)
{
	if (!(Joystick_HID_Interface.State.IsActive))
	  return true;

	/* Switch the attached HID device into report protocol mode and parse the HID report descriptor */
	if (HID_Host_SetReportProtocol(&Joystick_HID_Interface) || !(HIDReportInfo.TotalReportItems))
	  return false;

	/* Need to pair PS3 controllers to the last inserted Bluetooth adapter, and send a special packet to start reports */
	if (Joystick_IsPS3Controller)
	{
		/* Read out the latest inserted bluetooth adapter address stored in EEPROM */
		uint8_t TempAddress[BT_BDADDR_LEN];
		eeprom_read_block(TempAddress, BluetoothAdapter_LastLocalBDADDR, sizeof(TempAddress));

		/* Send PS3 bluetooth host pair request report to the adapter */
		uint8_t PS3AdapterPairRequest[]    = {0x01, 0x00, TempAddress[5], TempAddress[4], TempAddress[3], TempAddress[2], TempAddress[1], TempAddress[0]};
		HID_Host_SendReportByID(&Joystick_HID_Interface, 0xF5, HID_REPORT_ITEM_Feature, PS3AdapterPairRequest, sizeof(PS3AdapterPairRequest));

		/* Instruct the PS3 controller to send reports via the HID data IN endpoint */
		uint8_t PS3StartReportingRequest[] = {0x42, 0x0C, 0x00, 0x00};
		HID_Host_SendReportByID(&Joystick_HID_Interface, 0xF4, HID_REPORT_ITEM_Feature, PS3StartReportingRequest, sizeof(PS3StartReportingRequest));			
	}

	RGB_SetColour(RGB_ALIAS_Connected);
	return true;
}

/** Task to manage an enumerated USB joystick once connected. */
void Joystick_USBTask(void)
{
	if ((USB_HostState != HOST_STATE_Configured) || !(Joystick_HID_Interface.State.IsActive))
	  return;

	if (HID_Host_IsReportReceived(&Joystick_HID_Interface))
	{
		uint8_t JoystickReport[Joystick_HID_Interface.State.LargestReportSize];
		HID_Host_ReceiveReport(&Joystick_HID_Interface, &JoystickReport);

		/* Determine direction being pressed on the joystick */
		if (USB_GetHIDReportItemInfo(JoystickReport, Joystick_HIDReportItemMappings.Forward)       && Joystick_HIDReportItemMappings.Forward->Value)
		  Motors_SetChannelSpeed( MAX_MOTOR_POWER,  MAX_MOTOR_POWER);
		else if (USB_GetHIDReportItemInfo(JoystickReport, Joystick_HIDReportItemMappings.Backward) && Joystick_HIDReportItemMappings.Backward->Value)
		  Motors_SetChannelSpeed(-MAX_MOTOR_POWER, -MAX_MOTOR_POWER);
		else if (USB_GetHIDReportItemInfo(JoystickReport, Joystick_HIDReportItemMappings.Left)     && Joystick_HIDReportItemMappings.Left->Value)
		  Motors_SetChannelSpeed(-MAX_MOTOR_POWER,  MAX_MOTOR_POWER);
		else if (USB_GetHIDReportItemInfo(JoystickReport, Joystick_HIDReportItemMappings.Right)    && Joystick_HIDReportItemMappings.Right->Value)
		  Motors_SetChannelSpeed( MAX_MOTOR_POWER, -MAX_MOTOR_POWER);
		else
		  Motors_SetChannelSpeed(0, 0);
		  
		/* Determine if headlight button is currently pressed or not */
		if (USB_GetHIDReportItemInfo(JoystickReport, Joystick_HIDReportItemMappings.Headlights))
		  Headlights_SetState(Joystick_HIDReportItemMappings.Headlights->Value != 0);

		/* Determine if speaker button is currently pressed or not */
		if (USB_GetHIDReportItemInfo(JoystickReport, Joystick_HIDReportItemMappings.Speaker))
		  Speaker_Tone((Joystick_HIDReportItemMappings.Speaker->Value) ? 250 : 0);
	}

	/* Run the HID Class Driver service task on the HID Joystick interface */
	HID_Host_USBTask(&Joystick_HID_Interface);
}

/** Callback for the HID Report Parser. This function is called each time the HID report parser is about to store
 *  an IN, OUT or FEATURE item into the HIDReportInfo structure. To save on RAM, we are able to filter out items
 *  we aren't interested in (preventing us from being able to extract them later on, but saving on the RAM they would
 *  have occupied).
 *
 *  \param[in] CurrentItem  Pointer to the item the HID report parser is currently working with
 *
 *  \return Boolean true if the item should be stored into the HID report structure, false if it should be discarded
 */
bool CALLBACK_HIDParser_FilterHIDReportItem(HID_ReportItem_t* const CurrentItem)
{
	bool IsJoystick = false;

	/* Iterate through the item's collection path, until either the root collection node or a collection with the
	 * Joystick Usage is found - this distinguishes joystick HID devices from mouse devices
	 */
	for (HID_CollectionPath_t* CurrPath = CurrentItem->CollectionPath; CurrPath != NULL; CurrPath = CurrPath->Parent)
	{
		if (CurrPath->Usage.Usage == USAGE_JOYSTICK)
		{
			IsJoystick = true;
			break;
		}
	}

	/* Check the attributes of the current item - see if we are interested in it or not */
	if (CurrentItem->Attributes.Usage.Page == USAGE_PAGE_BUTTON)
	{
		/* Mappings are decided upon based on whether the attached device is a PS3 Controller or joystick */
		if (Joystick_IsPS3Controller)
		{
			/* Map button usages to functions suitable for a PS3 Controller */
			switch (CurrentItem->Attributes.Usage.Usage)
			{
				case PS3CONTROLLER_BUTTON_Up:
					Joystick_HIDReportItemMappings.Forward    = CurrentItem;
					return true;
				case PS3CONTROLLER_BUTTON_Right:
					Joystick_HIDReportItemMappings.Right      = CurrentItem;
					return true;
				case PS3CONTROLLER_BUTTON_Down:
					Joystick_HIDReportItemMappings.Backward   = CurrentItem;
					return true;
				case PS3CONTROLLER_BUTTON_Left:
					Joystick_HIDReportItemMappings.Left       = CurrentItem;
					return true;
				case PS3CONTROLLER_BUTTON_L1:
					Joystick_HIDReportItemMappings.Speaker    = CurrentItem;
					return true;
				case PS3CONTROLLER_BUTTON_R1:
					Joystick_HIDReportItemMappings.Headlights = CurrentItem;
					return true;
			}
		}
		else if (IsJoystick)
		{
			/* Map button usages to functions suitable for a generic joystick */
			switch (CurrentItem->Attributes.Usage.Usage)
			{
				case 1:
					Joystick_HIDReportItemMappings.Left       = CurrentItem;
					return true;
				case 2:
					Joystick_HIDReportItemMappings.Forward    = CurrentItem;
					return true;
				case 3:
					Joystick_HIDReportItemMappings.Backward   = CurrentItem;
					return true;
				case 4:
					Joystick_HIDReportItemMappings.Right      = CurrentItem;
					return true;
				case 5:
					Joystick_HIDReportItemMappings.Headlights = CurrentItem;
					return true;
				case 7:
					Joystick_HIDReportItemMappings.Speaker    = CurrentItem;
					return true;
			}
		}
	}

	return false;
}

