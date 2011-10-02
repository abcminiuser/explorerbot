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

#ifndef _SPEAKER_H_
#define _SPEAKER_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/pgmspace.h>
		#include <stdbool.h>
		#include <stddef.h>
		
		#include <LUFA/Common/Common.h>
	
	/* Macros: */
		/** Update interval that \ref Speaker_TickElapsed() will be called at, used to calculate the tick duration of notes
		 *  in a sequence constructed with \ref SPEAKER_NOTE.
		 */
		#define SPEAKER_UPDATE_TICK_MS     10
	
		/** Converts a given frequency (expressed as a floating point number, in Hz) to the PWM reload value needed by
		 *  \ref Speaker_Tone().
		 *
		 *  \param[in] Freq  Frequency to convert, in Hz.
		 *
		 *  \return PWM reload value suitable for \ref Speaker_Tone().
		 */
		#define SPEAKER_HZ(Freq)           (uint8_t)(Freq ? ((F_CPU / 1024) / Freq) : 0)
		
		/** Converts the given number of milliseconds into a number of update ticks, needed to play a note sequence.
		 *
		 *  \param[in] MS  Milliseconds the note should play for.
		 *
		 *  \return Number of ticks the note should play for.
		 */
		#define SPEAKER_DURATION(MS)       (uint8_t)(MS ? ((MS) / SPEAKER_UPDATE_TICK_MS) : 0)
		
		/** Constructs a note sequence element value from the given note frequency and duration. This may be placed into
		 *  a \c uint16_t array to define a note sequence which can be played via \ref Speaker_PlaySequence().
		 *
		 *  \param[in] Freq  Note frequency to play, in Hz.
		 *  \param[in] MS    Note duration to play for, in milliseconds.
		 *
		 *  \return Note element array value for a note sequence.
		 */
		#define SPEAKER_NOTE(Freq, MS)     (((uint16_t)SPEAKER_HZ(Freq) << 8) | SPEAKER_DURATION(MS))
	
	/* Enums: */
		/** Enum for the possible note sequences which can be played via \ref Speaker_PlaySequence(). */
		enum Speaker_SequenceIDs_t
		{
			SPEAKER_SEQUENCE_Connecting    = 0,
			SPEAKER_SEQUENCE_Connected     = 1,
			SPEAKER_SEQUENCE_Disconnected  = 2,
			SPEAKER_SEQUENCE_ConnectFailed = 3,
			SPEAKER_SEQUENCE_LaCucaracha   = 4,
		};
		
	/* Inline Functions: */
		/** Sets the tone generated from the speaker. This tone value does not have a defined frequency correspondance.
		 *
		 *  \param[in] PWMValue  PWM value to load into the speaker PWM timer (8-bit).
		 */
		static inline void Speaker_SetPWM(const uint8_t PWMValue)
		{
			if (PWMValue)
			{
				#if !defined(DISABLE_SPEAKER)
				DDRB  |= (1 << 7);
				OCR0A  = PWMValue;
				TCCR0B = ((1 << CS02) | (1 << CS00));
				#endif
			}
			else
			{
				TCCR0B = 0;
				DDRB  &= ~(1 << 7);
			}
		}

	/* Function Prototypes: */
		void Speaker_Init(void);
		void Speaker_TickElapsed(void);
		void Speaker_PlaySequence(const uint8_t SequenceID);
		void Speaker_Tone(const uint8_t PWMValue);

#endif

