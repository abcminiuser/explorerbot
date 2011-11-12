static bool MassStorage_OpenSensorLogFile(void)
{
	uint8_t ErrorCode;
	
	/* Create a new sensor log file on the disk, fail if one already exists */
	ErrorCode = f_open(&MassStorage_DiskLogFile, DATALOG_FILENAME,
	                   (FA_CREATE_NEW | FA_WRITE));

	/* See if the existing log was created sucessfully */
	if (ErrorCode == FR_OK)
	{
		/* Construct the sensor CSV header line(s) */
		char     LineBuffer[200];
		uint8_t  LineLength = Sensors_WriteSensorCSVHeader(LineBuffer);

		/* Write constructed CSV header to the attached mass storage disk */
		uint16_t BytesWritten = 0;
		f_write(&MassStorage_DiskLogFile, LineBuffer, LineLength, &BytesWritten);
	}
	else if (ErrorCode == FR_EXIST)
	{
		/* Open the already existing file on the disk */
		f_open(&MassStorage_DiskLogFile, DATALOG_FILENAME,
		       (FA_OPEN_EXISTING | FA_WRITE));
	
		/* Seek to the end of the existing log file */
		f_lseek(&MassStorage_DiskLogFile, MassStorage_DiskLogFile.fsize);	
	}
	else
	{
		/* Return disk error */
		return false;
	}
	
	return true;
}