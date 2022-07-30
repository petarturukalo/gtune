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

static void fdata_free_mallocs(fdata_t *f)
{
	free(f->hps);
	free(f->mag);
	free(f->c);
	free(f->norm);
}

void fdata_free(fdata_t *f)
{
	fftw_destroy_plan(f->p);
	fftw_cleanup();
	fdata_free_mallocs(f);
}

bool fdata_init(fdata_t *f, uint sample_rate, uint chunksz)
{
	// Zero the pointers set with malloc so that if one fails those that come
	// after it can safely be freed because they're already NULL pointers.
	bzero(f, sizeof(fdata_t));
	if ((f->norm = malloc(chunksz*sizeof(double))) == NULL ||
	    (f->c = malloc(chunksz*sizeof(fftw_complex))) == NULL ||
	    (f->mag = malloc(nmag(chunksz)*sizeof(double))) == NULL ||
	    (f->hps = malloc(nmag(chunksz)*sizeof(double))) == NULL) {
		fdata_free_mallocs(f);
		return false;
	}
	// Use measure option since it's expected that multiple chunks of samples
	// will be processed and not just one (otherwise estimate would be used).
	f->p = fftw_plan_dft_r2c_1d(chunksz, f->norm, f->c, FFTW_MEASURE);
	f->sample_rate = sample_rate;
	f->chunksz = chunksz;
	return true;
}

/*
 * maxi_dbl - Get the index of the maximum double value in an 
 *	array of doubles
 * @a: array of doubles
 * @n: length of array
 *
 * Assumes the array has at least 1 element.
 */
static uint maxi_dbl(double *a, uint n)
{
	uint mi = 0;

	while (--n) {
		if (a[n] > a[mi])
			mi = n;
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
	uint maxi;
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
	maxi = maxi_dbl(f->hps, m);

	return frequency(f->sample_rate, maxi, f->chunksz);
}

double fdata_process_chunk(fdata_t *f, char *samples, sdtype_meta_t *meta, bool skip_normalise)
{
	return process_chunk(f, f->p, samples, f->chunksz, meta, skip_normalise);
}
