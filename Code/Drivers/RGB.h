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

/** \file
 *
 *  Header file for RGB.c.
 */
 
#ifndef _RGB_H_
#define _RGB_H_

	/* Includes: */
		#include <avr/io.h>

	/* Enums: */
		/** Enum for the possible RGB LED colours, used by \ref RGB_SetColour(). */
		enum RGB_Colour_t
		{
			RGB_COLOUR_Off          = 0,
			RGB_COLOUR_Red          = (1 << 4),
			RGB_COLOUR_Green        = (1 << 5),
			RGB_COLOUR_Blue         = (1 << 6),
			RGB_COLOUR_Yellow       = (RGB_COLOUR_Red  | RGB_COLOUR_Green),
			RGB_COLOUR_Cyan         = (RGB_COLOUR_Blue | RGB_COLOUR_Green),			
			RGB_COLOUR_Magenta      = (RGB_COLOUR_Red  | RGB_COLOUR_Blue),
			RGB_COLOUR_White        = (RGB_COLOUR_Red  | RGB_COLOUR_Green | RGB_COLOUR_Blue),
		};
		
		/** Enum for the possible status, which can be used by \ref RGB_SetColour() in favour of a direct colour value. */
		enum RGB_Colour_Aliases_t
		{
			RGB_ALIAS_Disconnected  = RGB_COLOUR_White,
			RGB_ALIAS_Enumerating   = RGB_COLOUR_Yellow,
			RGB_ALIAS_Error         = RGB_COLOUR_Red,
			RGB_ALIAS_Ready         = RGB_COLOUR_Green,
			RGB_ALIAS_Connected     = RGB_COLOUR_Blue,
			RGB_ALIAS_Busy          = RGB_COLOUR_Magenta,
		};

	/* Function Prototypes: */
		void RGB_Init(void);
		void RGB_SetColour(const uint8_t Colour);

#endif

