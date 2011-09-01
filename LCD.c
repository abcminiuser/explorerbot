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

#include "LCD.h"

/** Latches a 4-bit data word onto the LCD bus.
 *
 *  \param[in] Data  Data word (4-bits) to latch onto the LCD bus.
 */
static void LCD_SetDataLines(const uint8_t Data)
{
	uint8_t NewData = 0;
				
	if (Data & 0x01)
	  NewData |= LCD_DATA4;
	  
	if (Data & 0x02)
	  NewData |= LCD_DATA5;

	if (Data & 0x04)
	  NewData |= LCD_DATA6;

	if (Data & 0x08)
	  NewData |= LCD_DATA7;

	PORTF = (PORTF & ~(LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4)) | NewData;
	
	Delay_MS(1);
	PORTE |= LCD_E;
	Delay_MS(1);
	PORTE &= ~LCD_E;
}

/** Retrieves the next 4-bits of data from the LCD bus.
 *
 *  \return Next 4-bit word of data from the LCD bus.
 */
static uint8_t LCD_GetDataLines(void)
{
	uint8_t CurrDataLines;
	uint8_t NewData = 0;
	
	PORTE |= LCD_E;
	Delay_MS(1);
	PORTE &= ~LCD_E;
	Delay_MS(1);
	
	CurrDataLines = PINF;
				
	if (CurrDataLines & LCD_DATA4)
	  NewData |= 0x01;
	  
	if (CurrDataLines & LCD_DATA5)
	  NewData |= 0x02;
	
	if (CurrDataLines & LCD_DATA6)
	  NewData |= 0x04;

	if (CurrDataLines & LCD_DATA7)
	  NewData |= 0x08;

	return NewData;
}

/** Intializes the LCD driver ready for use. This must be called before any other
 *  functions in the LCD driver.
 */
void LCD_Init(void)
{
	DDRE |= (LCD_E | LCD_RS | LCD_RW);
	DDRF |= (LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4);
	DDRB |= (1 << 4);
	
	TCCR2A = ((1 << COM2A1) | (1 << WGM20));

	LCD_SetBacklight(0);

	PORTE &= ~LCD_RS;
	LCD_SetDataLines(0b0011);
	Delay_MS(5);
	LCD_SetDataLines(0b0011);
	Delay_MS(1);
	LCD_SetDataLines(0b0011);
	LCD_SetDataLines(0b0010);
	
	LCD_WriteByte(0x88);
	LCD_WriteByte(0x08);
	LCD_WriteByte(0x01);
	LCD_WriteByte(0x04);
	LCD_WriteByte(0x0C);
	PORTE |=  LCD_RS;
	
	LCD_Clear();
}

/** Sets the LCD backlight intensity level.
 *
 *  \param[in] Intensity  Intensity of the backlight, a value between 0 (off) and 255 (maximum brightness).
 */
void LCD_SetBacklight(const uint8_t Intensity)
{	
	if (!(Intensity))
	{
		TCCR2B = 0;
		PORTB &= ~(1 << 4);
	}
	else
	{
		OCR2A  = Intensity;
		TCCR2B = (1 << CS21);
	}
}

/** Clears all text on the LCD display, moving the cursor back to the (1, 0) home position. */
void LCD_Clear(void)
{
	PORTE &= ~LCD_RS;
	LCD_WriteByte(0x01);
	PORTE |=  LCD_RS;
}

/** Sets the LCD cursor position to the nominated position on the LCD display.
 *
 *  \param[in] Y  Line number of the cursor, either 1 or 2.
 *  \param[in] X  Character index, from 0 to 15.
 */
void LCD_SetCursor(const uint8_t Y,
                   const uint8_t X)
{
	PORTE &= ~LCD_RS;
	LCD_WriteByte(0x80 | (((Y - 1) * 64) + X));
	PORTE |=  LCD_RS;
}

/** Sets the given LCD custom character index to the given character data.
 *
 *  \param[in] Index          Index of the custom character to set, between 1 and 8.
 *  \param[in] CharacterData  Pointer to the character data stored in FLASH.
 */
void LCD_SetCustomChar(const uint8_t Index,
                       const uint8_t* CharacterData)
{
	PORTE &= ~LCD_RS;
	LCD_WriteByte(0x40 | (Index << 3));
	PORTE |=  LCD_RS;
	
	for (uint8_t i = 0; i < 8; i++)
	  LCD_WriteByte(pgm_read_byte(CharacterData++));
}

/** Reads a byte of data from the LCD bus, at the current LCD DRAM address.
 *
 *  \return Read byte of data from the LCD
 */
uint8_t LCD_ReadByte(void)
{
	uint8_t Byte = 0;

	PORTF &= ~(LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4);
	DDRF  &= ~(LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4);
	PORTE |= LCD_RW;

	Byte |= (LCD_GetDataLines() << 4);
	Byte |= LCD_GetDataLines();
	
	return Byte;
}

/** Writes a byte of data to the LCD bus, at the current LCD DRAM address.
 *
 *  \param[in] Byte  Byte of data to write to the LCD.
 */
void LCD_WriteByte(const uint8_t Byte)
{
	PORTE &= ~LCD_RW;
	DDRF  |= LCD_DATA7 | LCD_DATA6 | LCD_DATA5 | LCD_DATA4;
	
	LCD_SetDataLines(Byte >> 4);
	LCD_SetDataLines(Byte & 0x0F);
}

/** Writes a NUL terminated string to the LCD bus, starting from the current LCD DRAM address.
 *
 *  \param[in] String  Pointer to the start of a string to write to the LCD, stored in SRAM.
 */
void LCD_WriteString(const char* String)
{
	while (*String != 0x00)
	{
		if (*String == '\n')
		  LCD_SetCursor(2, 0);
		else
		  LCD_WriteByte(*String);

		String++;
	}
}

/** Writes a formatted string to the LCD display, starting from the current LCD DRAM address.
 *
 *  \param[in] FormatString  Pointer to the start of a \c printf() format string, stored in SRAM.
 */
void LCD_WriteFormattedString(const char* FormatString, ...)
{
	char LineBuffer[50];

    va_list va;
    va_start(va, FormatString);
    vsprintf(LineBuffer, FormatString, va);
    va_end(va);

	LCD_WriteString(LineBuffer);
}

/** Writes a NUL terminated string to the LCD bus, starting from the current LCD DRAM address.
 *
 *  \param[in] String  Pointer to the start of a string to write to the LCD, stored in FLASH.
 */
void LCD_WriteString_P(const char* String)
{
	uint8_t CurrByte;

	while ((CurrByte = pgm_read_byte(String)) != 0x00)
	{
		if (CurrByte == '\n')
		  LCD_SetCursor(2, 0);
		else
		  LCD_WriteByte(CurrByte);

		String++;
	}
}
