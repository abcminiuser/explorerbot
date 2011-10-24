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
 *  Driver for the robot's RGB status LED.c.
 */

#include "RGB.h"


/** Initializes the RGB hardware driver ready for use. This must be called
 *  before any other functions in the RGB driver.
 */
void RGB_Init(void)
{
	DDRF |= RGB_COLOUR_White;

	RGB_SetColour(RGB_COLOUR_Off);
}

/** Sets the colour of the RGB status LED mounted on the robot.
 *
 *  \param[in] Colour  New colour to set the RGB LED to, a value from the \ref RGB_Colour_t or \ref RGB_Colour_Aliases_t enums.
 */
void RGB_SetColour(const uint8_t Colour)
{
	PORTF = (PORTF | RGB_COLOUR_White) & ~Colour;
}
