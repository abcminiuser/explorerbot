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

#include "Buttons.h"

/** Initializes the Buttons hardware driver, ready for use. This must be called prior to
 *  any other functions in this driver.
 */
void Buttons_Init(void)
{
	DDRD  &= ~(BUTTON1_MASK | BUTTON2_MASK);
	PORTD |=  (BUTTON1_MASK | BUTTON2_MASK);
}

/** Retrieves the state of all the buttons in the device.
 *
 *  \return A \c BUTTON*_MASK mask of pressed buttons.
 */
uint8_t Buttons_GetStateMask(void)
{
	return (~PIND & (BUTTON1_MASK | BUTTON2_MASK));
}
