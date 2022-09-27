/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "note.h"

// Frequency (in Hz) of base note A4.
#define BASE_NOTE_FREQ 440 
#define BASE_NOTE_NR 4  // The 4 in A4.
// Frequency (in Hz) of the first note C0.
#define FIRST_NOTE_FREQ 16.35

static const char *NOTES[] = { 
	"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"
};

/*
 * semitones_from_note - Get the number of semitones a note is away from another note
 * @freq: frequency of note
 * @target_freq: frequency of the other note
 */
static double semitones_from_note(double freq, double target_freq)
{
       return NNOTES*log2(freq/target_freq);
}

/*
 * semitones_from_base - Get the number of semitones a note is away from the base note
 * @freq: frequency of note to get number of semitones away from base note of
 */
static double semitones_from_base(double freq)
{
       return semitones_from_note(freq, BASE_NOTE_FREQ);
}

/*
 * note_index - Get the index of a note in the notes array
 *
 * Gets the closest note.
 */
static int note_index(double freq)
{
	int i = (int)round(semitones_from_base(freq))%NNOTES;
	return i < 0 ? NNOTES+i : i;
}

/*
 * note_number - Get the number of the note, such as 4 for note A4 with 440 Hz frequency
 * @note: note name
 * @n: length of note name
 */
static int note_number(double freq, const char *note, int n)
{
	double nr = log2(freq/FIRST_NOTE_FREQ);
	// Special case a new occurrence of C might have number 0.98 or something if the frequency is
	// closest to a C but not exactly a C, so need to round up to the next number, C1
	// in this intance, instead of flooring to a C0 that already exists at a lower frequency.
	// This special case only holds for the first note C, since the note corresponding to 
	// FIRST_NOTE_FREQ is C0.
	return (note[0] == 'C' && n == 1) ? round(nr) : floor(nr);
}

void init_note(char *note)
{
	memset(note, ' ', MAX_NOTE_LEN);
}

void note_from_freq(double freq, char *s)
{
	int note_ind, note_nr;
	const char *note;

	note_ind = note_index(freq);
	note = NOTES[note_ind];
	note_nr = note_number(freq, note, strlen(note));

	init_note(s);
	strcpy(s, note);  // Append note letter.
	s += strlen(note);
	*s = '0'+note_nr;  // Append its number.
}

