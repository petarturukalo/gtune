/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "freq.h"

/*
 * nmag - Get the number of magnitudes that need to be processed
 */
uint nmag(uint chunksz)
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
	if (f) {
		fftw_destroy_plan(f->p);
		fftw_cleanup();
		fdata_free_mallocs(f);
	}
}

bool fdata_init(fdata_t *f, uint sample_rate, uint chunksz)
{
	// Zero the pointers set with malloc so that if one fails those that come
	// after it can safely be freed because they're already NULL pointers.
	bzero(f, sizeof(fdata_t));
	if (!(f->norm = malloc(chunksz*sizeof(double))) ||
	    !(f->c = malloc(chunksz*sizeof(fftw_complex))) ||
	    !(f->mag = malloc(nmag(chunksz)*sizeof(double))) ||
	    !(f->hps = malloc(nmag(chunksz)*sizeof(double)))) {
		eprintf("failed to init frequency data: %s", strerror(errno));
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

double fdata_process_chunk(fdata_t *f, char *samples, sdtype_meta_t *meta, bool skip_normalise)
{
	uint maxi;
	uint m = nmag(f->chunksz);
	
	// Generate normalised values, floating point numbers in range -1 to 1, 
	// which are input for FFT.
	if (skip_normalise)
		normalise_samples_copy(samples, f->chunksz, meta, f->norm);
	else
		normalise_samples(samples, f->chunksz, meta, f->norm);
	// Preprocess the values further for better and more accurate frequency results.
	hanning_window(f->norm, f->chunksz);
	fftw_execute(f->p);
	// Use output of FFT to prepare for calculating frequency.
	magnitudes(f->c, f->mag, m);
	hps(f->mag, f->hps, m, 5);
	maxi = maxi_dbl(f->hps, m);

	return frequency(f->sample_rate, maxi, f->chunksz);
}
