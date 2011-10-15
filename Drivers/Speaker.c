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
 *  Driver for the robot's PWM controlled speaker.
 */

#include "Speaker.h"

/** Note sequence for to signal a connection in progress. */
static const uint16_t Sequence_Connecting[]    PROGMEM = {SPEAKER_NOTE(440.00, 120), 0};

/** Note sequence for to signal a connection has completed sucessfully. */
static const uint16_t Sequence_Connected[]     PROGMEM = {SPEAKER_NOTE(349.23,  80), SPEAKER_NOTE(440.00, 120), 0};

/** Note sequence for to signal an established connection has disconnected. */
static const uint16_t Sequence_Disconnected[]  PROGMEM = {SPEAKER_NOTE(440.00,  80), SPEAKER_NOTE(349.23, 120), 0};

/** Note sequence for to signal a connection has failed to complete. */
static const uint16_t Sequence_ConnectFailed[] PROGMEM = {SPEAKER_NOTE(349.23, 150), SPEAKER_NOTE(349.23, 150), 0};

/** Note sequence for the robot's novelty horn. */
static const uint16_t Sequence_LaCucaracha[]   PROGMEM = {SPEAKER_NOTE(261.63,  50), SPEAKER_NOTE(0, 50),
                                                          SPEAKER_NOTE(261.63,  50), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(261.63,  50), SPEAKER_NOTE(0, 50),
                                                          SPEAKER_NOTE(349.23, 100), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(440.00, 100), SPEAKER_NOTE(0, 80),
                                                          SPEAKER_NOTE(261.63,  50), SPEAKER_NOTE(0, 50),
                                                          SPEAKER_NOTE(261.63,  50), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(261.63,  50), SPEAKER_NOTE(0, 50),
                                                          SPEAKER_NOTE(349.23, 100), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(440.00, 100), SPEAKER_NOTE(0, 80),
                                                          SPEAKER_NOTE(349.23,  50), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(349.23,  50), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(329.63,  50), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(329.63,  50), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(293.66,  50), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(293.66,  50), SPEAKER_NOTE(0, 50), 
                                                          SPEAKER_NOTE(261.63, 100), SPEAKER_NOTE(0, 50),
                                                          0};

/** Table of note sequences which can be played via \ref Speaker_PlaySequence(). */
static const uint16_t* const Sequence_Table[]  PROGMEM = {Sequence_Connecting,
                                                          Sequence_Connected,
                                                          Sequence_Disconnected,
                                                          Sequence_ConnectFailed,
                                                          Sequence_LaCucaracha};

/** Pointer to the current note being played in the selected note sequence. */
static const uint16_t* SequencePosition = NULL;

/** Number of ticks elapsed for the currently playing note in the sequence */
static uint8_t SequenceTicksElapsed     = 0;

/** Number of ticks the currently playing note should continue for. */
static uint8_t SequenceTickDuration     = 0;


/** Initializes the Speaker hardware driver ready for use. This must be called before any other
 *  functions in the Speaker hardware driver.
 */
void Speaker_Init(void)
{
	DDRB |= (1 << 7);

	TCCR0A = ((1 << COM0A0) | (1 << WGM01));

	Speaker_SetPWM(0);
}

/** Updates the currently playing note sequence (if any), progressing to the next note if needed. This function should be called at
 *  \ref SPEAKER_UPDATE_TICK_MS to obtain the correct note timings.
 */
void Speaker_TickElapsed(void)
{
	if (!(SequencePosition))
	  return;
	
	if (SequenceTicksElapsed++ >= SequenceTickDuration)
	{
		uint16_t NextSequenceValue = pgm_read_word(SequencePosition++);
		
		if (!(NextSequenceValue))
		  SequencePosition = NULL;

		SequenceTicksElapsed = 0;
		SequenceTickDuration = (NextSequenceValue & 0xFF);
		Speaker_SetPWM(NextSequenceValue >> 8);
	}
}

/** Plays the requested note sequence through the robot's speaker, updating the current tone as \ref Speaker_TickElapsed() is called.
 *  While playing, calls to \ref Speaker_Tone() are ignored.
 *
 *  \param[in] SequenceID  Note sequence to play, a value from \ref Speaker_SequenceIDs_t.
 */
void Speaker_PlaySequence(const uint8_t SequenceID)
{
	#if !defined(DISABLE_SPEAKER_SEQUENCES)
	SequencePosition     = pgm_read_ptr(&Sequence_Table[SequenceID]);
	SequenceTicksElapsed = 0;
	SequenceTickDuration = 10;
	#else
	(void)SequenceID;
	#endif
}

/** Plays the given tone through the speaker, if a note sequence is not currently playing.
 *
 *  \note The given PWM value will continue to play until overridden by a note sequence, or a subsequent call to \ref Speaker_Tone().
 *
 *  \param[in] PWMValue  PWM value to play through the speaker; use \ref SPEAKER_DURATION() to obtain a value from a specific frequency.
 */
void Speaker_Tone(const uint8_t PWMValue)
{
	if (!(SequencePosition))
	  Speaker_SetPWM(PWMValue);
}
