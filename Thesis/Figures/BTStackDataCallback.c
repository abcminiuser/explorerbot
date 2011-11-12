void EVENT_Bluetooth_DataReceived(BT_StackConfig_t* const StackState,
                                  BT_L2CAP_Channel_t* const Channel,
                                  uint16_t Length,
                                  uint8_t* Data)
{
	/* Dispatch packets with a known protocol to the integrated services - for unknown packets, display a message to the LCD */
	switch (Channel->PSM)
	{
		case CHANNEL_PSM_SDP:
		case CHANNEL_PSM_HIDCTL:
		case CHANNEL_PSM_HIDINT:
		case CHANNEL_PSM_RFCOMM:
			SDP_ProcessPacket(StackState, Channel, Length, Data);
			HID_ProcessPacket(StackState, Channel, Length, Data);
			RFCOMM_ProcessPacket(StackState, Channel, Length, Data);
			break;		
		default:
			LCD_WriteFormattedString_P(PSTR("\fL2CAP Recv:%04X\n"
			                                  "PSM:%04X C:%04X"),
			                           Length, Channel->PSM, Channel->LocalNumber);
			break;
	}
}