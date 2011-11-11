DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
	uint8_t ErrorCode = RES_OK;

	if (USB_HostState != HOST_STATE_Configured)
	{
		ErrorCode = RES_NOTRDY;
	}
	else if (MS_Host_ReadDeviceBlocks(&Disk_MS_Interface, 0,
	                                  sector, count, 512, buff))
	{
		MS_Host_ResetMSInterface(&Disk_MS_Interface);
		ErrorCode = RES_ERROR;
	}

	return ErrorCode;
}

DRESULT disk_write(BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
	uint8_t ErrorCode = RES_OK;

	if (USB_HostState != HOST_STATE_Configured)
	{
		ErrorCode = RES_NOTRDY;
	}
	else if (MS_Host_WriteDeviceBlocks(&Disk_MS_Interface, 0,
		                               sector, count, 512, buff))
	{
		MS_Host_ResetMSInterface(&Disk_MS_Interface);
		ErrorCode = RES_ERROR;
	}
	
	return ErrorCode;
}
