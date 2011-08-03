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

#include "JoystickControl.h"

/** Processed HID report descriptor items structure, containing information on each HID report element */
static HID_ReportInfo_t HIDReportInfo;

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

/** Attempts to configure the system pipes for the attached HID Joystick.
 *
 *  \param[in] DeviceDescriptor      Pointer to the Device Descriptor read from the attached device
 *  \param[in] ConfigDescriptorSize  Size of the retrieved Configuration Descriptor from the device
 *  \param[in] ConfigDescriptorData  Pointer to the Configuration Descriptor read from the attached device
 *
 *  \return  Boolean true if a valid HID Joystick interface was found, false otherwise.
 */
bool Joystick_ConfigurePipes(const USB_Descriptor_Device_t* DeviceDescriptor,
                             const uint16_t ConfigDescriptorSize,
                             uint8_t* ConfigDescriptorData)
{
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

	return (HID_Host_SetReportProtocol(&Joystick_HID_Interface) == 0);
}

/** Task to manage an enumerated USB joystick once connected, to display movement data as it is received. */
void Joystick_USBTask(void)
{
	if (USB_HostState != HOST_STATE_Configured)
	  return;

	if (HID_Host_IsReportReceived(&Joystick_HID_Interface))
	{
		uint8_t JoystickReport[Joystick_HID_Interface.State.LargestReportSize];
		HID_Host_ReceiveReport(&Joystick_HID_Interface, &JoystickReport);

		for (uint8_t ReportNumber = 0; ReportNumber < HIDReportInfo.TotalReportItems; ReportNumber++)
		{
			HID_ReportItem_t* ReportItem = &HIDReportInfo.ReportItems[ReportNumber];

			/* Update the report item value if it is contained within the current report */
			if (!(USB_GetHIDReportItemInfo(JoystickReport, ReportItem)))
			  continue;

			/* Determine what report item is being tested, process updated value as needed */
			if ((ReportItem->Attributes.Usage.Page        == USAGE_PAGE_BUTTON) &&
			    (ReportItem->ItemType                     == HID_REPORT_ITEM_In))
			{
				Headlights_SetState(ReportItem->Value != 0);
			}
			else if ((ReportItem->Attributes.Usage.Page   == USAGE_PAGE_GENERIC_DCTRL) &&
			         ((ReportItem->Attributes.Usage.Usage == USAGE_X)                  ||
			          (ReportItem->Attributes.Usage.Usage == USAGE_Y))                 &&
			         (ReportItem->ItemType                == HID_REPORT_ITEM_In))
			{
				static int16_t MotorSpeed   = 0;
				static int16_t MotorBalance = 0;
				
				int16_t JoystickPosition = HID_ALIGN_DATA(ReportItem, int16_t);

				if (ReportItem->Attributes.Usage.Usage == USAGE_X)
				  MotorBalance = JoystickPosition;
				else if  (ReportItem->Attributes.Usage.Usage == USAGE_Y)
				  MotorSpeed   = JoystickPosition;

				Motors_SetChannelSpeed(MOTOR_CHANNEL_Left,  (MotorBalance <  100) ? MotorSpeed : 0);
				Motors_SetChannelSpeed(MOTOR_CHANNEL_Right, (MotorBalance > -100) ? MotorSpeed : 0);
			}
		}
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
	 * Joystick Usage is found - this prevents Mice, which use identical descriptors except for the Joystick usage
	 * parent node, from being erroneously treated as a joystick by the demo
	 */
	for (HID_CollectionPath_t* CurrPath = CurrentItem->CollectionPath; CurrPath != NULL; CurrPath = CurrPath->Parent)
	{
		if ((CurrPath->Usage.Page  == USAGE_PAGE_GENERIC_DCTRL) &&
		    (CurrPath->Usage.Usage == USAGE_JOYSTICK))
		{
			IsJoystick = true;
			break;
		}
	}

#if !defined(ALLOW_MOUSE_CONTROL)
	/* If a collection with the joystick usage was not found, indicate that we are not interested in this item */
	if (!IsJoystick)
	  return false;
#endif

	RGB_SetColour(RGB_ALIAS_Connected);

	/* Check the attributes of the current item - see if we are interested in it or not;
	 * only store BUTTON and GENERIC_DESKTOP_CONTROL items into the Processed HID Report
	 * structure to save RAM and ignore the rest
	 */
	return ((CurrentItem->Attributes.Usage.Page == USAGE_PAGE_BUTTON) ||
	        (CurrentItem->Attributes.Usage.Page == USAGE_PAGE_GENERIC_DCTRL));
}

