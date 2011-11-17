SDP_ServiceEntry_t ServiceEntry_RFCOMMSerialPort =
	{
		.Stack                = NULL,
		.TotalTableAttributes = (sizeof(SerialPort_Attribute_Table) /
		                         sizeof(SerialPort_Attribute_Table[0])),
		.AttributeTable       = SerialPort_Attribute_Table,
	};