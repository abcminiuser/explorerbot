static BT_StackConfig_t BluetoothAdapter_Stack =
	{
		.Config = 
			{
				.Class              = (DEVICE_CLASS_SERVICE_CAPTURING   |
				                       DEVICE_CLASS_MAJOR_COMPUTER      |
				                       DEVICE_CLASS_MINOR_COMPUTER_PALM),
				.Name               = "Bluetooth Robot",
				.PINCode            = "0000",
				.Discoverable       = true,
				.PacketBuffer       = EXTERNAL_MEMORY_START,
			}
	};