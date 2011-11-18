static const struct
{
	SDP_ItemSequence8Bit_t UUIDList_Header;
	struct
	{
		SDP_ItemUUID_t SerialPortUUID;
	} UUIDList;
} ATTR_PACKED PROGMEM SerialPort_Attr_ServiceClassIDs =
	{
		SDP_ITEMSEQUENCE8BIT(sizeof(SerialPort_Attr_ServiceClassIDs.UUIDList)),
		{
			SDP_ITEMUUID(SP_CLASS_UUID),
		}
	};