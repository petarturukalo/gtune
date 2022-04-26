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
 * dist - Get the distance between two numbers
 * @lt: the lesser of the two numbers
 * @gt: the greater of the two numbers
 */
static int dist(int lt, int gt)
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
double nr_prcnt_over_range(int n, int start, int end)
{
	int nlen, rangelen;

	nlen = dist(start, n);
	rangelen = dist(start, end);

	return nlen/(double)rangelen;
}

double nr_new_range(int n, int start, int end, int new_start, int new_end)
{
	// Length of new range.
	int rangelen = dist(new_start, new_end);  
	return new_start+nr_prcnt_over_range(n, start, end)*rangelen;
}

short max_sint16(short *a, int n)
{
	short max = a[0];

	for (int i = 1; i < n; ++i) {
		if (a[i] > max)
			max = a[i];
	}
	return max;
}

short min_sint16(short *a, int n)
{
	short min = a[0];

	for (int i = 1; i < n; ++i) {
		if (a[i] < min)
			min = a[i];
	}
	return min;
}

int maxi_dbl(double *a, int n)
{
	int mi = 0;

	for (int i = 1; i < n; ++i) {
		if (a[i] > a[mi]) 
			mi = i;
	}
	return mi;
}

