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

#include "BluetoothAdapter.h"

/** Bluetooth stack configuration and state stable, used to configure an instance of the Bluetooth stack. */
BT_StackConfig_t Bluetooth_Module =
	{
		.Config = 
			{
				.Class              = (DEVICE_CLASS_SERVICE_CAPTURING | DEVICE_CLASS_MAJOR_COMPUTER | DEVICE_CLASS_MINOR_COMPUTER_PALM),
				.Name               = "Bluetooth Robot",
				.PINCode            = "0000",
				.Discoverable       = true,
				.PacketBuffer       = EXTERNAL_MEMORY_START,
			}
	};

/** Indicates if the Bluetooth control mode is currently active or not */
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
	  
	Bluetooth_Init(&Bluetooth_Module);
	return true;
}

/** Task to manage an enumerated USB Bluetooth adapter once connected, to display movement data as it is received. */
void Bluetooth_USBTask(void)
{
	if ((USB_HostState != HOST_STATE_Configured) || !IsActive)
	  return;

	Pipe_SelectPipe(BLUETOOTH_DATA_IN_PIPE);
	Pipe_Unfreeze();
	
	if (Pipe_IsINReceived())
	{
		BT_L2CAP_Header_t* PacketHeader = (BT_L2CAP_Header_t*)Bluetooth_Module.Config.PacketBuffer;

		/* Read in the L2CAP packet data from the Data IN pipe */
		Pipe_Read_Stream_LE(PacketHeader, sizeof(BT_L2CAP_Header_t), NULL);
		Pipe_Read_Stream_LE(PacketHeader->Data, PacketHeader->DataLength, NULL);
		Pipe_ClearIN();
		Pipe_Freeze();

		RGB_SetColour(RGB_ALIAS_Busy);
		Bluetooth_ProcessPacket(&Bluetooth_Module, BLUETOOTH_PACKET_L2CAPData);
		RGB_SetColour(RGB_ALIAS_Connected);
	}
	
	Pipe_Freeze();
	
	Pipe_SelectPipe(BLUETOOTH_EVENTS_PIPE);
	Pipe_Unfreeze();
	
	if (Pipe_IsINReceived())
	{
		BT_HCIEvent_Header_t* EventHeader = (BT_HCIEvent_Header_t*)Bluetooth_Module.Config.PacketBuffer;

		/* Read in the Event packet data from the Event IN pipe */
		Pipe_Read_Stream_LE(EventHeader, sizeof(BT_HCIEvent_Header_t), NULL);
		Pipe_Read_Stream_LE(EventHeader->Parameters, EventHeader->ParameterLength, NULL);
		Pipe_ClearIN();
		Pipe_Freeze();

		RGB_SetColour(RGB_ALIAS_Busy);
		Bluetooth_ProcessPacket(&Bluetooth_Module, BLUETOOTH_PACKET_HCIEvent);
		RGB_SetColour(RGB_ALIAS_Connected);
	}
	
	Pipe_Freeze();
	
	/* Keep on running the management routine until all pending packets have been sent */	
	while (Bluetooth_ManageConnections(&Bluetooth_Module))
	  RGB_SetColour(RGB_ALIAS_Busy);

	RGB_SetColour(RGB_ALIAS_Connected);
}

void CALLBACK_Bluetooth_SendPacket(BT_StackConfig_t* const StackState,
                                   const uint8_t Type,
                                   const uint16_t Length)
{
	RGB_SetColour(RGB_ALIAS_Busy);

	/* Determine the type of packet being sent, use appropriate pipe */
	switch (Type)
	{
		case BLUETOOTH_PACKET_HCICommand:
			USB_ControlRequest = (USB_Request_Header_t)
				{
					.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_DEVICE),
					.bRequest      = 0,
					.wValue        = 0,
					.wIndex        = 0,
					.wLength       = Length
				};

			/* HCI commands must be sent over the Control pipe */
			Pipe_SelectPipe(PIPE_CONTROLPIPE);
			USB_Host_SendControlRequest(StackState->Config.PacketBuffer);
			break;
		default:
			Pipe_SelectPipe(BLUETOOTH_DATA_OUT_PIPE);
			Pipe_Unfreeze();

			/* L2CAP packets must be sent over the Data OUT pipe */
			Pipe_Write_Stream_LE(StackState->Config.PacketBuffer, Length, NULL);
			Pipe_ClearOUT();
			Pipe_Freeze();
			break;
	}

	RGB_SetColour(RGB_ALIAS_Connected);
}

bool CALLBACK_Bluetooth_ConnectionRequest(BT_StackConfig_t* const StackState,
                                          BT_HCI_Connection_t* const ConnectionHandle)
{
	LCD_Clear();
	LCD_WriteFormattedString("Conn Request:\n"
	                         "%02X%02X:%02X%02X:%02X%02X", ConnectionHandle->RemoteBDADDR[5], ConnectionHandle->RemoteBDADDR[4],
	                                                       ConnectionHandle->RemoteBDADDR[3], ConnectionHandle->RemoteBDADDR[2],
	                                                       ConnectionHandle->RemoteBDADDR[1], ConnectionHandle->RemoteBDADDR[0]);

	/* Accept all requests from all devices regardless of BDADDR */
	return true;
}

void EVENT_Bluetooth_ConnectionComplete(BT_StackConfig_t* const StackState,
                                        BT_HCI_Connection_t* const ConnectionHandle)
{	
	LCD_Clear();
	LCD_WriteFormattedString("Connected:\n"
	                         "%02X%02X:%02X%02X:%02X%02X", ConnectionHandle->RemoteBDADDR[5], ConnectionHandle->RemoteBDADDR[4],
	                                                       ConnectionHandle->RemoteBDADDR[3], ConnectionHandle->RemoteBDADDR[2],
	                                                       ConnectionHandle->RemoteBDADDR[1], ConnectionHandle->RemoteBDADDR[0]);
}

void EVENT_Bluetooth_DisconnectionComplete(BT_StackConfig_t* const StackState,
                                           BT_HCI_Connection_t* const ConnectionHandle)
{
	LCD_Clear();
	LCD_WriteFormattedString("Disconnected:\n"
	                         "%02X%02X:%02X%02X:%02X%02X", ConnectionHandle->RemoteBDADDR[5], ConnectionHandle->RemoteBDADDR[4],
	                                                       ConnectionHandle->RemoteBDADDR[3], ConnectionHandle->RemoteBDADDR[2],
	                                                       ConnectionHandle->RemoteBDADDR[1], ConnectionHandle->RemoteBDADDR[0]);
}

bool CALLBACK_Bluetooth_ChannelRequest(BT_StackConfig_t* const StackState,
                                       BT_HCI_Connection_t* const Connection,
                                       BT_L2CAP_Channel_t* const Channel)
{
	/* Accept all channel requests from all devices regardless of PSM */
	return true;
}

void EVENT_Bluetooth_ChannelOpened(BT_StackConfig_t* const StackState,
                                   BT_L2CAP_Channel_t* const Channel)
{
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_SDP:
			Bluetooth_SDP_ChannelOpened(StackState, Channel);			
			break;
		case CHANNEL_PSM_HIDCTL:
		case CHANNEL_PSM_HIDINT:
			Bluetooth_HID_ChannelOpened(StackState, Channel);
			break;
	}

	LCD_Clear();
	LCD_WriteFormattedString("L2CAP Opened\n"
	                         "L:%04X R:%04X", Channel->LocalNumber, Channel->RemoteNumber);
}

void EVENT_Bluetooth_ChannelClosed(BT_StackConfig_t* const StackState,
                                   BT_L2CAP_Channel_t* const Channel)
{
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_SDP:
			Bluetooth_SDP_ChannelClosed(StackState, Channel);			
			break;
		case CHANNEL_PSM_HIDCTL:
		case CHANNEL_PSM_HIDINT:
			Bluetooth_HID_ChannelClosed(StackState, Channel);
			break;
	}

	LCD_Clear();
	LCD_WriteFormattedString("L2CAP Closed\n"
	                         "L:%04X R:%04X", Channel->LocalNumber, Channel->RemoteNumber);
}

void EVENT_Bluetooth_DataReceived(BT_StackConfig_t* const StackState,
                                  BT_L2CAP_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_SDP:
			Bluetooth_SDP_ProcessPacket(StackState, Channel, Length, Data);
			break;
		case CHANNEL_PSM_HIDCTL:
		case CHANNEL_PSM_HIDINT:
			Bluetooth_HID_ProcessPacket(StackState, Channel, Length, Data);
			break;
		default:
			LCD_Clear();
			LCD_WriteFormattedString("P:%04X L:%04X\n"
			                         "LC:%04X RC:%04X", Channel->PSM, Length, Channel->LocalNumber, Channel->RemoteNumber);
			break;
	}	
}

void CALLBACK_Bluetooth_HID_ReportReceived(BT_StackConfig_t* const StackState,
                                           BT_L2CAP_Channel_t* const Channel,
                                           uint8_t ReportType,
                                           uint16_t Length,
                                           uint8_t* Data)
{
	/* Process output reports to look for key code changes */
	if (ReportType == HID_DATAT_Input)
	{
		// TODO: FIXME
		switch (*((uint16_t*)&Data[2]))
		{
			default:
				Motors_SetChannelSpeed(MOTOR_CHANNEL_All,    0);
				break;
			case 0xF600:
				Motors_SetChannelSpeed(MOTOR_CHANNEL_All,    MAX_MOTOR_POWER);
				break;
			case 0x00F6:
				Motors_SetChannelSpeed(MOTOR_CHANNEL_Left,   MAX_MOTOR_POWER);
				Motors_SetChannelSpeed(MOTOR_CHANNEL_Right, -MAX_MOTOR_POWER);					
				break;
			case 0x0A00:
				Motors_SetChannelSpeed(MOTOR_CHANNEL_All,   -MAX_MOTOR_POWER);					
				break;
			case 0x000A:
				Motors_SetChannelSpeed(MOTOR_CHANNEL_Left,  -MAX_MOTOR_POWER);
				Motors_SetChannelSpeed(MOTOR_CHANNEL_Right,  MAX_MOTOR_POWER);					
				break;
		}
	}
}

