/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Musical notes and calculation of them from frequencies (in Hz).
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef NOTE_H
#define NOTE_H

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Highest note allowed is something around E6, so not bothering with handling
// notes with numbers >= 10.
#define MAX_NOTE_LEN 3
#define NNOTES 12  

/*
 * Initialise a string storing a musical note by filling it with spaces so that
 * its displayed width becomes MAX_NOTE_LEN. If it were filled with null termination characters
 * then its display width would be 0. This makes "A0" and "A#0" (etc.) the same width.
 */
void init_note(char *note);

/*
 * note_from_freq - Get the name of a note from its frequency
 * @freq: frequency of note to get name of
 * @s: string to store note name in. Must have length of at least MAX_NOTE_LEN 
 */
void note_from_freq(double freq, char *s);

#endif
