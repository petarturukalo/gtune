/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "gtune.h"

/*
 * chunk_stepsz - Get the size of a chunk step in samples
 * @chunksz: size of chunk in  samples
 * @nsteps: number of steps to pass a whole chunk of the given size
 */
static uint chunk_stepsz(uint chunksz, uint nsteps)
{
	return chunksz/nsteps;
}

static bool no_stepping(uint chunk_nsteps)
{
	return chunk_nsteps == 1;
}

static bool stepping(uint chunk_nsteps)
{
	return no_stepping(chunk_nsteps) == false;
}

static bool nsteps_valid(uint nsteps)
{
	if (nsteps == 0) {
		eprintf("number of steps of 0 will produce unexpected behaviour");
		return false;
	}
	return true;
}

static bool stepping_valid(uint chunk_nsteps, uint chunksz, uint chunk_stepsz)
{
	if (stepping(chunk_nsteps)) {
		if (chunk_stepsz > chunksz) {
			// This shouldn't be reachable anyway since stepsz is calculated by chunksz/nsteps, so
			// stepsz > chunksz only when 0 < nsteps < 1, but nsteps is an unsigned int and can't
			// be in this range.
			eprintf("stepping with step size %d greater than chunk size %d will produce unexpected behaviour",
				chunk_stepsz, chunksz);
			return false;
		}
		// Calculating stepsz = chunksz/nsteps when nsteps > chunksz will floor stepsz to 0 anyway since they're
		// all ints.
		if (chunk_nsteps > chunksz || chunk_stepsz == 0) {
			eprintf("number of steps %d can't be greater than chunk size %d as the calculated step size will be "
				"less than 1 and a fraction of a sample can't be stepped over", chunk_nsteps, chunksz);
			return false;
		}
	}
	return true;
}

static bool frequency_valid(double freq)
{
	if (freq <= 0) {
		eprintf("frequency %f can't be <= 0", freq);
		return false;
	}
	return true;
}

static bool frequencies_valid(double min_valid_freq, double max_valid_freq)
{
	if (frequency_valid(min_valid_freq) == false || frequency_valid(max_valid_freq) == false)
		return false;
	// Because both frequency endpoints are inclusive they can be the same (==), otherwise this
	// check would be >=.
	if (min_valid_freq > max_valid_freq) {
		eprintf("min valid frequency %f must be less than or equal to max valid frequency %f", 
			min_valid_freq, max_valid_freq);
		return false;
	}
	return true;
}

static bool sample_rate_valid(uint sample_rate)
{
	if (sample_rate == 0) {
		eprintf("sample rate can't be 0 as no samples will be read");
		return false;
	}
	return true;
}

static bool chunksz_valid(uint chunksz)
{
	if (chunksz == 0) {
		eprintf("chunk size can't be 0 as no samples will be read");
		return false;
	}
	return true;
}

/*
 * Convert a pulse audio sample format to a sample data type metadata.
 * In future if another method of audio input is used then that other method can
 * be converted to a sample data type metadata too.
 */
static sdtype_meta_t *pasamplefmt_to_sdtype_meta(PaSampleFormat fmt)
{
	switch (fmt) {
		case paFloat32:
			return &sdtype_meta_float32;
		case paInt32:
			return &sdtype_meta_int32;
		case paInt16:
			return &sdtype_meta_int16;
		// TODO handle other cases. Will need to open up sdtype_meta_t to handle 
		// other data formats and sizes.
		default:
			PaError samplesz = Pa_GetSampleSize(fmt);

			if (samplesz == paSampleFormatNotSupported)
				eprintf("pulse audio sample format %d: %s", fmt, Pa_GetErrorText(paSampleFormatNotSupported));
			else
				eprintf("pulse audio sample format %d with sample size %d bytes not currently supported", fmt, samplesz);
			return NULL;
	}
}

bool gtune_init(gtune_t *g, uint sample_rate, uint chunksz, uint chunk_nsteps, 
		double min_valid_freq, double max_valid_freq, PaSampleFormat fmt)
{
	uint stepsz;

	norm_assert();

	if (!nsteps_valid(chunk_nsteps) || !chunksz_valid(chunksz) ||
	    !frequencies_valid(min_valid_freq, max_valid_freq) ||
	    !sample_rate_valid(sample_rate))
		return false;
	stepsz = chunk_stepsz(chunksz, chunk_nsteps);
	if (stepping_valid(chunk_nsteps, chunksz, stepsz) == false)
		return false;
	bzero(g, sizeof(gtune_t));
	g->chunksz = chunksz;
	g->chunk_nsteps = chunk_nsteps;
	g->chunk_stepsz = stepsz;
	g->min_valid_freq = min_valid_freq;
	g->max_valid_freq = max_valid_freq;

	if (fdata_init(&g->freq, sample_rate, chunksz) == false)
		return false;
	// Set up sample data type before initialising mic since it uses the sample data type.
	if ((g->meta = pasamplefmt_to_sdtype_meta(fmt)) == NULL ||
	    (g->samples = malloc(g->chunksz*g->meta->samplesz)) == NULL) {
		fdata_free(&g->freq);
		return false;
	}
	g->pafmt = fmt;
	if (mic_init(&g->mic, sample_rate, g->chunk_stepsz, fmt) == false) {
		fdata_free(&g->freq);
		free(g->samples);
		return false;
	}
	// Fill with spaces so that its displayed width becomes MAX_NOTE_LEN, as otherwise 
	// without if it's filled with null-termination characters its display width is 0.
	memset(g->note, ' ', MAX_NOTE_LEN);  
	return true;
}

void gtune_cleanup(gtune_t *g)
{
	if (g) {
		free(g->samples);
		mic_cleanup(&g->mic);
		fdata_free(&g->freq);
	}
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
 * @samples: samples to process. Processes chunksz amount of input samples as 
 *	specified at init-time of gtune
 */
static void gtune_freq(gtune_t *g, char *samples)
{
	double note_freq;

	// TODO should only be skipping normalisation for paFloat32 since it's already normalised, but
	// the not already normalised int types seem to work better without it
	note_freq = fdata_process_chunk(&g->freq, samples, g->meta, true);

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
		mic_read_until_success(&g->mic, g->samples, g->chunksz);
		gtune_freq(g, g->samples);
	}
}

/*
 * gtune_step - Process samples with the step provided by the user 
 *
 * This implementation does a bunch of copying, which is fine since it's O(n) and 
 * processing of the data is O(n*logn) by FFT's supposed complexity, but it could be avoided
 * entirely by implementing a circular buffer FFT operation.
 *
 * Example: in the below ____ denotes the samples array and | identifies an index start of a step in the samples array. 
 * Here nstep=4, so the samples array is broken up into 4 sections (0-indexed).
 *
 * 1. The samples array is filled completely.
 *
 *                      * samples+bytes_in_chunk_step     * samples+bytes_to_chunk_last_step
 *     0                1                2                3
 *     |________________|________________|________________|________________
 *
 * 2. The whole samples array is processed for a frequency.
 * 3. Samples are copied from range index 1 (inclusive) to the end of the array, to range index 0 (inclusive)
 * to index 3 (exclusive). Characters other than _ are used to show whether step sections have the same samples.
 *
 *                               * copy from range---------------------------------*
 *		* copy to range ----------------------------------*       
 *              0                1                2                3
 * before copy: |________________|xxxxxxxxxxxxxxxx|oooooooooooooooo|OOOOOOOOOOOOOOOO
 * after copy:  |xxxxxxxxxxxxxxxx|oooooooooooooooo|OOOOOOOOOOOOOOOO|OOOOOOOOOOOOOOOO
 *								   
 * Notice that after copying, section at index 0 lost its ____ samples, and sections 
 * at indices 2 and 3 now share the same OOOO samples. 
 *					   
 * 4. Read new samples starting at index 3, overwriting now redundant samples. Characters nnnn 
 * denote newly read in samples.
 *
 *                                                                 * read into here
 * before read: |xxxxxxxxxxxxxxxx|oooooooooooooooo|OOOOOOOOOOOOOOOO|OOOOOOOOOOOOOOOO
 * after read:  |xxxxxxxxxxxxxxxx|oooooooooooooooo|OOOOOOOOOOOOOOOO|nnnnnnnnnnnnnnnn
 *
 * 5. A "step" has been done. Jump to 2. to process the frequency and step again (loop).
 */
static void gtune_step(gtune_t *g)
{
	uint chunk_last_step_index;
	off_t bytes_to_chunk_last_step, bytes_in_chunk_step;

	chunk_last_step_index = (g->chunk_nsteps-1)*g->chunk_stepsz;
	// Convert from number of samples to a byte offset.
	bytes_to_chunk_last_step = chunk_last_step_index*g->meta->samplesz;
	bytes_in_chunk_step = g->chunk_stepsz*g->meta->samplesz;

	// Read a whole chunk.
	mic_read_until_success(&g->mic, g->samples, g->chunksz);
	for (;;) {
		gtune_freq(g, g->samples);
		// Use memmove() over memcpy() because source and dest arrays overlap.
		memmove(g->samples, g->samples+bytes_in_chunk_step, bytes_to_chunk_last_step);
		// Read a step of a chunk (will block for chunksz/number of chunk steps amount of time).
		mic_read_until_success(&g->mic, g->samples+bytes_to_chunk_last_step, g->chunk_stepsz);
	}
}

void gtune_start(gtune_t *g)
{
	print_header();
	if (no_stepping(g->chunk_nsteps))
		gtune(g);
	else
		gtune_step(g);
}

