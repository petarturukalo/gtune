/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Guitar tuner.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef GTUNE_H
#define GTUNE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "note.h"
#include "freq.h"
#include "mic.h"
#include "math.h"

// All the data required by the guitar tuner.
struct guitar_tuner {
	fdata_t freq;  // For converting audio input into frequencies.
	mic_t mic;  // For audio input.
	char note[MAX_NOTE_LEN];  // Frequency converted to a musical note.
	sdtype_meta_t *meta;  // Metadata describing the data type of the samples for normalising them.
	PaSampleFormat pafmt;
	char *samples;  // Array to store read samples in. The size of a sample is described in meta.
	double min_valid_freq;
	double max_valid_freq;
	uint chunksz;
	uint chunk_nsteps;
	uint chunk_stepsz;
};

typedef struct guitar_tuner gtune_t;

/*
 * gtune_init - Initialise guitar tuner data
 * @sample_rate: number of samples recorded per second (Hz)
 * @chunksz: number of samples to both read and process per frequency calculation
 * @chunk_nsteps: a number >= 1 that determines the step size between frequency processes. 2 steps means
 *	that the whole of a chunk will be passed in 2 steps. A chunk size of 8192 and chunk steps of 2 gives
 *	a chunk step size of 8192/2 = 4096. This step size of half the chunk size will, on reading a chunk, first
 *	have the whole chunk processed, then on the next process will be the second half of the just
 *	processed chunk and the first half of the next read chunk, and again on the next next process will be
 *	the whole of the next chunk. This is to allow for a faster refresh rate, since, for example with
 *	a step of 2 there will be 2 prints per chunk process, using overlapping parts of the chunks.
 * @min_valid_freq: minimum valid frequency (inclusive) that is considered a note. 
 * @max_valid_freq: maximum valid frequency (inclusive) that is considered a note
 *
 * The accuracy of a reading is calculated by sample_rate/chunksz. This means that for a sample rate of
 * 44100 Hz and chunk size of 8192, the accuracy is 44100/8192 ~= 5.38, which has that while tuning up from
 * 110 Hz, for example, a next higher Hz will only appear when tuned up to 110+5.38 = 115.38 Hz.
 * Again, for a sample rate of 8000 and chunk size of 8192, the accuracy is 8000/8192 ~= 0.98, so
 * the next up from 110 Hz is 110.98, thus providing improved accuracy.
 *
 * The tradeoff here is refresh rate, as the better the accuracy, the worse the refresh rate. The
 * calculation for refresh rate is 1/accuracy, so for an accuracy of 5.38 Hz this is a 1/5.38 ~= 0.19 second
 * refresh rate, but for an improvement in accuracy going to 0.98 Hz, the refresh rate becomes 1/0.98 ~= 
 * 1.02 seconds. This can be "offset" by making chunk_nsteps greater than 1 - making it 2 with an initial
 * refresh rate of 1.02 seconds will have it refresh at 1.02/2 = 0.51 seconds.
 *
 * Return whether initialisation was successful.
 */
bool gtune_init(gtune_t *g, uint sample_rate, uint chunksz, uint chunk_nsteps, 
		double min_valid_freq, double max_valid_freq, PaSampleFormat fmt);

/*
 * gtune_free - Clean up and free a guiter tuner allocated with gtune_init
 */
void gtune_cleanup(gtune_t *g);

/*
 * gtune_start - Start recording and displaying frequencies and notes
 */
void gtune_start(gtune_t *g);

#endif
