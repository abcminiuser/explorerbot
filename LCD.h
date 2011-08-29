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

#ifndef _LCD_H_
#define _LCD_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#include <stdbool.h>
		#include <stdarg.h>
		#include <stdio.h>

		#include <LUFA/Common/Common.h>
		
	/* Macros: */
		#define LCD_E      (1 << 3)
		#define LCD_RW     (1 << 4)
		#define LCD_RS     (1 << 5)
		#define LCD_DATA7  (1 << 0)
		#define LCD_DATA6  (1 << 1)
		#define LCD_DATA5  (1 << 2)
		#define LCD_DATA4  (1 << 3)
		
	/* Function Prototypes: */
		void    LCD_Init(void);
		void    LCD_SetBacklight(const uint8_t Intensity);
		void    LCD_Clear(void);
		void    LCD_SetCursor(const uint8_t Y,
		                      const uint8_t X);
		void    LCD_SetCustomChar(const uint8_t Index,
		                          const uint8_t* CharacterData);
		void    LCD_WriteByte(const uint8_t Byte);
		uint8_t LCD_ReadByte(void);
		void    LCD_WriteString(const char* String);
		void    LCD_WriteFormattedString(const char* FormatString, ...);
		void    LCD_WriteString_P(const char* String);
		
#endif
