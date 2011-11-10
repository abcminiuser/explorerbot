typedef struct
{
	/* Indicates if the current sensor is connected or not. */
	bool  Connected;
	
	/* Human-readable name of the sensor, stored in SRAM. */
	const char* Name;
	
	/* Indicates if the sensor data is represented as a single value,
	   or a triplicate of three axis values. */
	bool SingleAxis;
	
	/* Last retrieved data from the sensor. */	
	union
	{
		/* Sensor data if the sensor outputs a single axis value. */
		int32_t Single;
	
		/* Sensor data if the sensor outputs a triplicate of three axis values. */
		struct
		{
			/* X axis value of the triplicate. */
			int16_t X;

			/* Y axis value of the triplicate. */
			int16_t Y;

			/* Z axis value of the triplicate. */			
			int16_t Z;
		} Triplicate; 
	} Data;
} SensorData_t;