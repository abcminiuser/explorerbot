void EVENT_Bluetooth_InitServices(BT_StackConfig_t* const StackState)
{
	SDP_Init(StackState);
	RFCOMM_Init(StackState);
}

void EVENT_Bluetooth_ManageServices(BT_StackConfig_t* const StackState)
{
	SDP_Manage(StackState);
	RFCOMM_Manage(StackState);
}

bool CALLBACK_Bluetooth_ConnectionRequest(BT_StackConfig_t* const StackState,
                                          BT_HCI_Connection_t* const Connection)
{
	/* Accept all requests from all devices regardless of BDADDR */
	return true;
}

void EVENT_Bluetooth_ConnectionStateChange(BT_StackConfig_t* const StackState,
                                           BT_HCI_Connection_t* const Connection)
{	
	/* Ignore state changes to the HCI connections */
}

bool CALLBACK_Bluetooth_ChannelRequest(BT_StackConfig_t* const StackState,
                                       BT_HCI_Connection_t* const Connection,
                                       BT_L2CAP_Channel_t* const Channel)
{
	/* Accept all channel requests from all devices */
	return true;
}

void EVENT_Bluetooth_ChannelStateChange(BT_StackConfig_t* const StackState,
                                        BT_L2CAP_Channel_t* const Channel)
{
	/* Determine the channel event that has occurred */
	if (Channel->State == L2CAP_CHANSTATE_Open)
	{
		SDP_ChannelOpened(StackState, Channel);			
		RFCOMM_ChannelOpened(StackState, Channel);	
	}
	else if (Channel->State == L2CAP_CHANSTATE_Closed)
	{
		SDP_ChannelClosed(StackState, Channel);
		RFCOMM_ChannelClosed(StackState, Channel);	
	}
}

void EVENT_Bluetooth_DataReceived(BT_StackConfig_t* const StackState,
                                  BT_L2CAP_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{
	SDP_ProcessPacket(StackState, Channel, Length, Data);
	RFCOMM_ProcessPacket(StackState, Channel, Length, Data);
}

void CALLBACK_RFCOMM_DataReceived(BT_StackConfig_t* const StackState,
                                  RFCOMM_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{
	/* Process received RFCOMM data on the virtual serial port here */
}