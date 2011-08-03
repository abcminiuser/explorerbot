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

bool IsActive;

/** Attempts to configure the system pipes for the attached Bluetooth adapter.
 *
 *  \param[in] DeviceDescriptor      Pointer to the Device Descriptor read from the attached device
 *  \param[in] ConfigDescriptorSize  Size of the retrieved Configuration Descriptor from the device
 *  \param[in] ConfigDescriptorData  Pointer to the Configuration Descriptor read from the attached device
 *
 *  \return  Boolean true if a valid Bluetooth interface was found, false otherwise.
 */
bool Bluetooth_ConfigurePipes(USB_Descriptor_Device_t* DeviceDescriptor,
                              uint16_t ConfigDescriptorSize,
                              void* ConfigDescriptorData)
{
	USB_Descriptor_Endpoint_t* DataINEndpoint  = NULL;
	USB_Descriptor_Endpoint_t* DataOUTEndpoint = NULL;
	USB_Descriptor_Endpoint_t* EventsEndpoint  = NULL;
	
	IsActive = false;

	/* Validate returned device Class, SubClass and Protocol values against the Bluetooth spec values */
	if ((DeviceDescriptor->Class    != BLUETOOTH_DEVICE_CLASS)    ||
	    (DeviceDescriptor->SubClass != BLUETOOTH_DEVICE_SUBCLASS) ||
	    (DeviceDescriptor->Protocol != BLUETOOTH_DEVICE_PROTOCOL))
	{
		return false;
	}

	/* The Bluetooth USB transport addendum mandates that the data (not streaming voice) endpoints
	   be in the first interface descriptor (interface 0) */
	USB_GetNextDescriptorOfType(&ConfigDescriptorSize, &ConfigDescriptorData, DTYPE_Interface);

	/* Ensure that an interface was found, and the end of the descriptor was not reached */
	if (!(ConfigDescriptorSize))
	  return false;

	while (!(DataINEndpoint) || !(DataOUTEndpoint))
	{
		/* Get the next Bluetooth interface's data endpoint descriptor */
		if (USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
		                              DComp_NextInterfaceBluetoothDataEndpoint) != DESCRIPTOR_SEARCH_COMP_Found)
		{
			/* Data endpoints not found within the first bluetooth device interface, error out */
			return false;
		}

		/* Retrieve the endpoint address from the endpoint descriptor */
		USB_Descriptor_Endpoint_t* EndpointData = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t);

		/* If the endpoint is a IN type endpoint */
		if ((EndpointData->EndpointAddress & ENDPOINT_DIR_MASK) == ENDPOINT_DIR_IN)
		{
			/* Check if the found endpoint is a interrupt or bulk type descriptor */
			if ((EndpointData->Attributes & EP_TYPE_MASK) == EP_TYPE_INTERRUPT)
			  EventsEndpoint = EndpointData;
			else
			  DataINEndpoint = EndpointData;
		}
		else
		{
			DataOUTEndpoint = EndpointData;
		}
	}

	/* Configure the Bluetooth data IN pipe */
	Pipe_ConfigurePipe(BLUETOOTH_DATA_IN_PIPE, EP_TYPE_BULK, PIPE_TOKEN_IN,
	                   DataINEndpoint->EndpointAddress, DataINEndpoint->EndpointSize, PIPE_BANK_SINGLE);

	/* Configure the Bluetooth data OUT pipe */
	Pipe_ConfigurePipe(BLUETOOTH_DATA_OUT_PIPE, EP_TYPE_BULK, PIPE_TOKEN_OUT,
					   DataOUTEndpoint->EndpointAddress, DataOUTEndpoint->EndpointSize, PIPE_BANK_SINGLE);

	/* Configure the Bluetooth events pipe */
	Pipe_ConfigurePipe(BLUETOOTH_EVENTS_PIPE, EP_TYPE_INTERRUPT, PIPE_TOKEN_IN,
					   EventsEndpoint->EndpointAddress, EventsEndpoint->EndpointSize, PIPE_BANK_SINGLE);
	Pipe_SetInterruptPeriod(EventsEndpoint->PollingIntervalMS);

	IsActive = true;
	return true;
}

/** Descriptor comparator function. This comparator function is can be called while processing an attached USB device's
 *  configuration descriptor, to search for a specific sub descriptor. It can also be used to abort the configuration
 *  descriptor processing if an incompatible descriptor configuration is found.
 *
 *  This comparator searches for the next Endpoint descriptor inside the current interface descriptor, aborting the
 *  search if another interface descriptor is found before the required endpoint.
 *
 *  \return A value from the DSEARCH_Return_ErrorCodes_t enum
 */
uint8_t DComp_NextInterfaceBluetoothDataEndpoint(void* CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	/* Determine the type of the current descriptor */
	if (Header->Type == DTYPE_Endpoint)
	  return DESCRIPTOR_SEARCH_Found;
	else if (Header->Type == DTYPE_Interface)
	  return DESCRIPTOR_SEARCH_Fail;
	else
	  return DESCRIPTOR_SEARCH_NotFound;
}

/** Performs any post configuration tasks after the attached Bluetooth adapter has been successfully enumerated.
 *
 *  \return Boolean true if no Bluetooth adapter attached or if all post-configuration tasks complete without error, false otherwise
 */
bool Bluetooth_PostConfiguration(void)
{
	if (!IsActive)
	  return true;

	// TODO, initialize stack
	return true;
}

/** Task to manage an enumerated USB Bluetooth adapter once connected, to display movement data as it is received. */
void Bluetooth_USBTask(void)
{
	if ((USB_HostState != HOST_STATE_Configured) || !IsActive)
	  return;
}

