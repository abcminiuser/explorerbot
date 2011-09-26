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

#include "Headlights.h"

static bool Headlights_LatchedState = false;

/** Initializes the Headlights hardware driver ready for use. This must be called
 *  before any other functions in the Headlights driver.
 */
void Headlights_Init(void)
{
	DDRD |= (1 << 5);

	Headlights_SetState(false);
}

/** Turns on or off the robot Headlights.
 *
 *  \param[in] HeadlightsOn  Boolean \c true to turn the headlights on, \c false to turn headlights off.
 */
void Headlights_SetState(const bool HeadlightsOn)
{
	if (HeadlightsOn || Headlights_LatchedState)
	  PORTD |=  (1 << 5);
	else
	  PORTD &= ~(1 << 5);
}

/** Toggles on or off the robot Headlights.
 */
void Headlights_ToggleState(void)
{
	Headlights_LatchedState = !Headlights_LatchedState;
	PORTD ^= (1 << 5);
}
