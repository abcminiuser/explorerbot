/*
             Bluetooth Robot
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.com
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef _SENSORS_ITG3200_H_
#define _SENSORS_ITG3200_H_

	/* Includes: */
		#include "SensorsCommon.h"

	/* Macros: */
		#define ITG3200_CHIP_ID       0x69
		
		#define ITG3200_STA_RAWRDY    (1 << 0)
		#define ITG3200_STA_PLLRDY    (1 << 2)
	
		#define ITG3200_WHOAMI_REG    0x00
		#define	ITG3200_SMPLRT_REG    0x15
		#define ITG3200_DLPF_FS_REG   0x16
		#define ITG3200_INT_CFG_REG   0x17
		#define ITG3200_INT_STA_REG   0x1A
		#define	ITG3200_TMP_H_REG     0x1B
		#define	ITG3200_TMP_L_REG     0x1C
		#define	ITG3200_GX_H_REG      0x1D
		#define	ITG3200_GX_L_REG      0x1E
		#define	ITG3200_GY_H_REG      0x1F
		#define	ITG3200_GY_L_REG      0x20
		#define ITG3200_GZ_H_REG      0x21
		#define ITG3200_GZ_L_REG      0x22
		#define ITG3200_PWR_M_REG     0x3E

	/* Function Prototypes: */
		void ITG3200_Init(SensorData_t* const SensorInfo);
		void ITG3200_Update(SensorData_t* const SensorInfo);

#endif

