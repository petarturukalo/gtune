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
 * maxi_dbl - Get the index of the maximum double value in an 
 *	array of doubles
 * @a: array of doubles
 * @n: length of array
 *
 * Assumes the array has at least 1 element.
 */
static int maxi_dbl(double *a, int n)
{
	int mi = 0;

	for (int i = 1; i < n; ++i) {
		if (a[i] > a[mi]) 
			mi = i;
	}
	return mi;
}

/*
 * process_chunk - Process a chunk of samples into a frequency
 * @samples: samples to process 
 * @meta: metadata describing the numeric data type of a sample. 
 * @skip_normalise: whether to skip normalising the samples because they are already normalised
 *
 * Return the frequency of the samples.
 * Samples are converted to double as that's the required data type input to the implementation 
 * of FFT in use, but the user could have read a different data type, such as signed 16-bit integers, 
 * or 32-bit floats, etc.
 */
static double process_chunk(fdata_t *f, fftw_plan p, char *samples, int n, sdtype_meta_t *meta,
			    bool skip_normalise)
{
	int m = nmag(f->chunksz);

	// Generate normalised values, floating point numbers in range -1 to 1, 
	// which are input for FFT.
	normalise_samples_copy(samples, f->chunksz, meta, f->norm, skip_normalise);
	// Preprocess the values further for better and more accurate frequency results.
	hanning_window(f->norm, f->chunksz);
	fftw_execute(p);
	// Use output of FFT to prepare for calculating frequency on a subsequent frequency.
	magnitudes(f->c, f->mag, m);
	hps(f->mag, f->hps, m, 5);
	f->maxi = maxi_dbl(f->hps, m);

	return frequency(f->sample_rate, f->maxi, f->chunksz);
}

double fdata_process_chunk(fdata_t *f, char *samples, sdtype_meta_t *meta, bool skip_normalise)
{
	return process_chunk(f, f->p, samples, f->chunksz, meta, skip_normalise);
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

