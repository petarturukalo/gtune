/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "freq.h"

/*
 * nmag - Get the number of magnitudes that need to be processed
 */
int nmag(int chunksz)
{
	// Half because the frequency domain output from FFT is mirrored or something,
	// so only the first half is actually needed for calculations.
	return chunksz/2;
}

void fdata_init(fdata_t *f, int sample_rate, int chunksz)
{
	f->norm = malloc(chunksz*sizeof(double));
	f->c = malloc(chunksz*sizeof(fftw_complex));
	// This array could be combined with the normalised array to save space
	// since they're used sequentially and not at the same time, but it would
	// make this code hard to read.
	f->mag = malloc(nmag(chunksz)*sizeof(double));
	f->hps = malloc(nmag(chunksz)*sizeof(double));

	// Use measure option since it's expected that multiple chunks of samples
	// will be processed and not just one (otherwise estimate would be used).
	f->p = fftw_plan_dft_r2c_1d(chunksz, f->norm, f->c, FFTW_MEASURE);
	f->sample_rate = sample_rate;
	f->chunksz = chunksz;
}

/*
 * process_chunk - Process a chunk of samples
 * @samples: samples to process cast to void pointer (can be any data type)
 * @conv: function which takes param samples and norm array and is expected to convert
 *	all samples to between -1 and 1 and store this in the norm array (samples and norm
 *	should be the same length, the size of a chunk). Last parameter is chunk size
 *
 * Samples are converted to double as that's the required data type input to the implementation of FFT
 * in use, and the user could have read a different data type, such as signed 16-bit integers, or 32-bit
 * floats, etc.
 */
static int process_chunk(fdata_t *f, fftw_plan p, void *samples, void (*conv)(void *, double *, int))
{
	int m = nmag(f->chunksz);

	// Generate normalised values, floating point numbers in range -1 to 1, 
	// which are input for FFT.
	conv(samples, f->norm, f->chunksz);
	// Preprocess the values further for better and more accurate frequency results.
	hanning_window(f->norm, f->chunksz);
	fftw_execute(p);
	// Use output of FFT to prepare for calculating frequency on a subsequent 
	// call to fdata_frequency.
	magnitudes(f->c, f->mag, m);
	hps(f->mag, f->hps, m, 5);
	f->maxi = maxi_dbl(f->hps, m);
}

/*
 * conv_samples_float32 - "Convert" float 32-bit samples already preprocessed to be in range 
 *	-1 to 1 into the double array
 */
static void conv_samples_float32_preprocd(void *samples, double *norm, int chunksz)
{
	float *fsamps = (float *)samples;

	for (int i = 0; i < chunksz; ++i)
		norm[i] = fsamps[i];
}

void fdata_process_chunk_float32_preprocd(fdata_t *f, float *samples)
{
	process_chunk(f, f->p, (void *)samples, conv_samples_float32_preprocd);
}

/*
 * normalise - Move an int from its current min-max range into a range with min -1 and
 *	max 1 (range of the new normalised number)
 * @n: number to normalise
 * @min: minimum in range number is a part of
 * @max: maximum in range number is a part of
 *
 * The min number in the original range gets normalised value -1, and the max number
 * gets 1.
 */
static double normalise(int n, int min, int max)
{
	return nr_new_range(n, min, max, -1, 1);
}

/*
 * conv_samples_sint16 - Convert signed 16-bit integer samples into normalised doubles
 *	in range -1 and 1
 */
static void conv_samples_sint16(void *samples, double *norm, int chunksz)
{
	int min, max;
	short *ssamps = (short *)samples;

	min = min_sint16(ssamps, chunksz);
	max = max_sint16(ssamps, chunksz);

	for (int i = 0; i < chunksz; ++i) 
		norm[i] = normalise(ssamps[i], min, max);
}

void fdata_process_chunk_sint16(fdata_t *f, short *samples)
{
	process_chunk(f, f->p, (void *)samples, conv_samples_sint16);
}

double fdata_frequency(fdata_t *f)
{
	return frequency(f->sample_rate, f->maxi, f->chunksz);
}

void fdata_free(fdata_t *f)
{
	fftw_destroy_plan(f->p);
	fftw_cleanup();

	free(f->hps);
	free(f->mag);
	free(f->c);
	free(f->norm);
}

