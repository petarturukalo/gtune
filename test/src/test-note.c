/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "test-note.h"

// Maximum note number (a note's number suffix) in NOTE_FREQ_PAIRS.
#define MAX_NOTE_NR 5

struct note_freq_pair {
	char note[MAX_NOTE_LEN];
	double freq;
};

// All notes must be the same length, length MAX_NOTE_LEN.
static struct note_freq_pair NOTE_FREQ_PAIRS[] = {
	{ "C0 ", 16.35 },
	{ "C#0", 17.32 },
	{ "D0 ", 18.35 },
	{ "D#0", 19.45 },
	{ "E0 ", 20.60 },
	{ "F0 ", 21.83 },
	{ "F#0", 23.12 },
	{ "G0 ", 24.50 },
	{ "G#0", 25.96 },
	{ "A0 ", 27.50 },
	{ "A#0", 29.14 },
	{ "B0 ", 30.87 },

	{ "C1 ", 32.70 },
	{ "C#1", 34.65 },
	{ "D1 ", 36.71 },
	{ "D#1", 38.89 },
	{ "E1 ", 41.20 },
	{ "F1 ", 43.65 },
	{ "F#1", 46.25 },
	{ "G1 ", 49.00 },
	{ "G#1", 51.91 },
	{ "A1 ", 55.00 },
	{ "A#1", 58.27 },
	{ "B1 ", 61.74 },

	{ "C2 ", 65.41 },
	{ "C#2", 69.30 },
	{ "D2 ", 73.42 },
	{ "D#2", 77.78 },
	{ "E2 ", 82.41 },
	{ "F2 ", 87.31 },
	{ "F#2", 92.50 },
	{ "G2 ", 98.00 },
	{ "G#2", 103.83 },
	{ "A2 ", 110.00 },
	{ "A#2", 116.54 },
	{ "B2 ", 123.47 },

	{ "C3 ", 130.81 },
	{ "C#3", 138.59 },
	{ "D3 ", 146.83 },
	{ "D#3", 155.56 },
	{ "E3 ", 164.81 },
	{ "F3 ", 174.61 },
	{ "F#3", 185.00 },
	{ "G3 ", 196.00 },
	{ "G#3", 207.65 },
	{ "A3 ", 220.00 },
	{ "A#3", 233.08 },
	{ "B3 ", 246.94 },

	{ "C4 ", 261.63 },
	{ "C#4", 277.18 },
	{ "D4 ", 293.66 },
	{ "D#4", 311.13 },
	{ "E4 ", 329.63 },
	{ "F4 ", 349.23 },
	{ "F#4", 369.99 },
	{ "G4 ", 392.00 },
	{ "G#4", 415.30 },
	{ "A4 ", 440.00 },
	{ "A#4", 466.16 },
	{ "B4 ", 493.88 },

	{ "C5 ", 523.25 },
	{ "C#5", 554.37 },
	{ "D5 ", 587.33 },
	{ "D#5", 622.25 },
	{ "E5 ", 659.25 },
	{ "F5 ", 698.46 },
	{ "F#5", 739.99 },
	{ "G5 ", 783.99 },
	{ "G#5", 830.61 },
	{ "A5 ", 880.00 },
	{ "A#5", 932.33 },
	{ "B5 ", 987.77 }
};

static void assert_freq_to_note(double freq, char *sln_note)
{
	char ans[MAX_NOTE_LEN];

	note_from_freq(freq, ans);
	/*printf("freq=%f, ans=[%s], sln=[%s]\n", freq, ans, sln_note);*/
	assert(strcmp(ans, sln_note) == 0);
}

static void test_freq_to_note(void)
{
	struct note_freq_pair *p;
	int n;

	p = NOTE_FREQ_PAIRS;
	n = (MAX_NOTE_NR+1)*NNOTES; 

	for (int i = 0; i < n; ++i, ++p) {
		assert_freq_to_note(p->freq, p->note);
	}
}

void test_note_entry(void)
{
	test_freq_to_note();
}
