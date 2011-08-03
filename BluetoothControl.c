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

#include "BluetoothControl.h"

/** Attempts to configure the system pipes for the attached Bluetooth adapter.
 *
 *  \param[in] DeviceDescriptor      Pointer to the Device Descriptor read from the attached device
 *  \param[in] ConfigDescriptorSize  Size of the retrieved Configuration Descriptor from the device
 *  \param[in] ConfigDescriptorData  Pointer to the Configuration Descriptor read from the attached device
 *
 *  \return  Boolean true if a valid Bluetooth interface was found, false otherwise.
 */
bool Bluetooth_ConfigurePipes(const USB_Descriptor_Device_t* DeviceDescriptor,
                              const uint16_t ConfigDescriptorSize,
                              uint8_t* ConfigDescriptorData)
{
	return false;
}

/** Performs any post configuration tasks after the attached Bluetooth adapter has been successfully enumerated.
 *
 *  \return Boolean true if no Bluetooth adapter attached or if all post-configuration tasks complete without error, false otherwise
 */
bool Bluetooth_PostConfiguration(void)
{
	return true;
}

/** Task to manage an enumerated USB Bluetooth adapter once connected, to display movement data as it is received. */
void Bluetooth_USBTask(void)
{
	if (USB_HostState != HOST_STATE_Configured)
	  return;
}

