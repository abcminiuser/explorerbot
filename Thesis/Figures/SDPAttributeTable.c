static const SDP_ServiceAttributeTable_t PROGMEM SerialPort_Attribute_Table[] =
	{
		{ SDP_ATTRIBUTE_ID_SERVICERECORDHANDLE,
		  &SerialPort_Attr_ServiceHandle        },
		{ SDP_ATTRIBUTE_ID_SERVICECLASSIDS,
		  &SerialPort_Attr_ServiceClassIDs      },
		{ SDP_ATTRIBUTE_ID_PROTOCOLDESCRIPTORLIST,
		  &SerialPort_Attr_ProtocolDescriptor   },
		{ SDP_ATTRIBUTE_ID_BROWSEGROUPLIST,
		  &SerialPort_Attr_BrowseGroupList      },
		{ SDP_ATTRIBUTE_ID_LANGUAGEBASEATTROFFSET,
		  &SerialPort_Attr_LanguageBaseIDOffset },
		{ SDP_ATTRIBUTE_ID_SERVICENAME,
		  &SerialPort_Attr_ServiceName          },
		{ SDP_ATTRIBUTE_ID_SERVICEDESCRIPTION,
		  &SerialPort_Attr_ServiceDescription   },
	};