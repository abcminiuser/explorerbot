if (Joystick_IsPS3Controller)
{
	/* Read out the latest inserted bluetooth adapter address stored in EEPROM */
	BDADDR_t TempAddress;
	eeprom_read_block(TempAddress, BluetoothAdapter_LastLocalBDADDR, sizeof(BDADDR_t));

	/* Send PS3 bluetooth host pair request report to the adapter */
	uint8_t AdapterPairRequest[] = {0x01, 0x00, TempAddress[5], TempAddress[4],
	                                            TempAddress[3], TempAddress[2],
	                                            TempAddress[1], TempAddress[0]};
	HID_Host_SendReportByID(&Joystick_HID_Interface,
	                        0xF5, HID_REPORT_ITEM_Feature,
	                        AdapterPairRequest, sizeof(PS3AdapterPairRequest));

	/* Instruct the PS3 controller to send reports via the HID data IN endpoint */
	uint8_t StartReportingRequest[] = {0x42, 0x0C, 0x00, 0x00};
	HID_Host_SendReportByID(&Joystick_HID_Interface,
	                        0xF4, HID_REPORT_ITEM_Feature,
	                        StartReportingRequest, sizeof(PS3StartReportingRequest));
}