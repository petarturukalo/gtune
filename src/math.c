/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "math.h"

double magnitude(fftw_complex c)
{
	// Element at index 0 is real part, element at index 1 is complex part.
	return sqrt(c[0]*c[0] + c[1]*c[1]);
}

void magnitudes(fftw_complex *c, double *mags, int n)
{
	for (int i = 0; i < n; ++i) 
		mags[i] = magnitude(c[i]);
}

double frequency(int sample_rate, int bin_index, int nbins)
{
	return (sample_rate*bin_index)/(double)nbins;
}

/*
 * dist_double - Get the distance between two numbers
 * @lt: the lesser of the two numbers
 * @gt: the greater of the two numbers
 */
static double dist(double lt, double gt)
{
	return gt-lt;
}

/*
 * nr_prcnt_over_range - Get a number's percentage across a range
 * @n: number in range between start and end
 * @start: number start of range (inclusive)
 * @end: number end of range (inclusive)
 *
 * Number 5 in range 0 to 10 is 50% (would return 0.5) across the range. 
 * Number 4 in the same range is 40% (0.4) across the range, etc.
 */
static double nr_prcnt_over_range(double n, double start, double end)
{
	double nlen, rangelen;

	nlen = dist(start, n);
	rangelen = dist(start, end);

	return nlen/rangelen;
}

double nr_new_range(double n, double start, double end, double new_start, double new_end)
{
	// Length of new range.
	double rangelen = dist(new_start, new_end);  
	return new_start+nr_prcnt_over_range(n, start, end)*rangelen;
}

void hps(double *mag, double *h, int len, int n)
{
	int ds, end, i;

	memcpy((void *)h, (void *)mag, len*sizeof(double));

	// Skip downsampling first since already done by memcpy.
	for (ds = 2; ds <= n; ++ds) {
		end = len/ds;
		for (i = 0; i < end; ++i)
			h[i] *= mag[i*ds];
	}
}

/*
 * hann - Compute the Hanning function for a number i in range 0 <= i < n.
 */
double hann(int i, int n)
{
	return 0.5*(1-cos((2*M_PI*i)/(n-1)));
}

void hanning_window(double *a, int n)
{
	for (int i = 0; i < n; ++i)
		a[i] *= hann(i, n);
}
