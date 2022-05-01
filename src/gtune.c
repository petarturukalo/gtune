/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "gtune.h"

/*
 * chunk_stepsz - Get the size of a chunk step
 * @chunksz: size of chunk
 * @nsteps: number of steps to pass a whole chunk of the given size
 *
 * Make sure chunksz is a multiple of nsteps.
 */
static int chunk_stepsz(int chunksz, int nsteps)
{
	return chunksz/nsteps;
}

bool gtune_init(gtune_t *g, int sample_rate, int chunksz, int chunk_nsteps, 
		double min_valid_freq, double max_valid_freq)
{
	norm_assert();
	fdata_init(&g->freq, sample_rate, chunksz);

	g->min_valid_freq = min_valid_freq;
	g->max_valid_freq = max_valid_freq;
	g->chunksz = chunksz;
	g->chunk_nsteps = chunk_nsteps;
	g->chunk_stepsz = chunk_stepsz(chunksz, chunk_nsteps);

	if (!mic_init(&g->mic, sample_rate, g->chunk_stepsz)) {
		fdata_free(&g->freq);
		return false;
	}
	// Fill with spaces so that its displayed width becomes MAX_NOTE_LEN, as otherwise 
	// without if it's filled with null-termination characters its display width is 0.
	memset(g->note, ' ', MAX_NOTE_LEN);  
	g->meta = &sdtype_meta_float32;
	g->samples = malloc(g->chunksz*sizeof(float));
	return true;
}

void gtune_cleanup(gtune_t *g)
{
	free(g->samples);
	mic_cleanup(&g->mic);
	fdata_free(&g->freq);
}

static void print_header(void)
{
	printf("LAST NOTE    CUR FREQ\n");
	printf("waiting for data...");
	fflush(stdout);
}

static void print_note(char *note, double freq)
{
	printf("\r%s          %07.3f", note, freq);
	fflush(stdout);
}

/*
 * gtune_freq - Calculate and print a frequency and its note
 * @samples: samples to process. Processes chunksz amount of samples as input
 *	at init-time of gtune
 */
static void gtune_freq(gtune_t *g, float *samples)
{
	double note_freq;

	note_freq = fdata_process_chunk(&g->freq, (char *)samples, g->meta, true);

	if (note_freq >= g->min_valid_freq && note_freq <= g->max_valid_freq)
		note_from_freq(note_freq, g->note);
	print_note(g->note, note_freq);
}

/*
 * gtune - Process samples without a step
 */
static void gtune(gtune_t *g)
{
	for (;;) {
		mic_read(&g->mic, g->samples, g->chunksz);
		gtune_freq(g, g->samples);
	}
}

/*
 * gtune_step - Process samples with the step provided by the user 
 *
 * This implementation does a bunch of copying, which is fine since it's O(n) and 
 * processing of the data is O(n*logn) by FFT's supposed complexity, but it could be avoided
 * entirely by implementing a circular buffer FFT operation.
 */
static void gtune_step(gtune_t *g)
{
	// Start index of last step of chunk.
	int chunk_last_step = (g->chunk_nsteps-1)*g->chunk_stepsz;

	// Read a whole chunk.
	mic_read(&g->mic, g->samples, g->chunksz);

	for (;;) {
		gtune_freq(g, g->samples);
		memcpy(g->samples, g->samples+g->chunk_stepsz, chunk_last_step*sizeof(float));
		// Read a step of a chunk (will block for chunksz/number of chunk steps amount of time).
		mic_read(&g->mic, g->samples+chunk_last_step, g->chunk_stepsz);
	}
}

void gtune_start(gtune_t *g)
{
	print_header();

	// Trusts the user not to enter bad values.
	if (g->chunk_nsteps == 1)
		gtune(g);
	else
		gtune_step(g);
}

