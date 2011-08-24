/*
             Bluetooth Robot
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
        www.fourwalledcubicle.org
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

#ifndef _SENSORS_BMP085_H_
#define _SENSORS_BMP085_H_

	/* Macros: */
		#define BMP085_CAL_AC1             0xAA
		#define BMP085_CAL_AC2             0xAC
		#define BMP085_CAL_AC3             0xAE
		#define BMP085_CAL_AC4             0xB0
		#define BMP085_CAL_AC5             0xB2
		#define BMP085_CAL_AC6             0xB4
		#define BMP085_CAL_B1              0xB6
		#define BMP085_CAL_B2              0xB8
		#define BMP085_CAL_MB              0xBA
		#define BMP085_CAL_MC              0xBC
		#define BMP085_CAL_MD              0xBE

		#define BMP085_CONTROL             0xF4
		#define BMP085_TEMPDATA            0xF6
		#define BMP085_PRESSUREDATA        0xF6
		#define BMP085_READTEMPCMD         0x2E
		#define BMP085_READPRESSURECMD     0x34

		#define BMP085_ULTRALOWPOWER       0
		#define BMP085_STANDARD            1
		#define BMP085_HIGHRES             2
		#define BMP085_ULTRAHIGHRES        3

#endif

