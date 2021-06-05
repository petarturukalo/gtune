/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * My own "math" related functions.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef MATH_H
#define MATH_H

#include <math.h>
#include <fftw3.h>

/**
 * magnitude - Get the magnitude of a complex number
 */
double magnitude(fftw_complex c);

/**
 * magnitudes - Get the magnitudes of multiple complex numbers
 * @c: array of complex numbers
 * @mags: array of magnitudes
 * @n: number of magnitudes to calculate 
 */
void magnitudes(fftw_complex *c, double *mags, int n);

/**
 * nr_new_range - Convert a number in a current range to a new range
 * @n: number
 * @start: start (inclusive) of number's current range 
 * @end: end (inclusive) of number's current range 
 * @new_start: new start (inclusive) of number's new range
 * @new_end: new end (inclusive) of number's new range
 *
 * Number 5 in range 0 to 10 converted to new range 10 to 20 is 15 (halfway through
 * the range).
 */
double nr_new_range(int n, int start, int end, int new_start, int new_end);

/**
 * max_sint16 - Get the max value in an array of signed 16-bit integer
 * @a: array
 * @n: length of array
 *
 * Assumes the array has at least 1 element.
 */
short max_sint16(short *a, int n);

/**
 * max_sint16 - Get the min value in an array of signed 16-bit integers
 * @a: array
 * @n: length of array
 *
 * Assumes the array has at least 1 element.
 */
short min_sint16(short *a, int n);

/**
 * maxi_dbl - Get the index of the maximum double value in an 
 *	array of doubles
 * @a: array of doubles
 * @n: length of array
 *
 * Assumes the array has at least 1 element.
 */
int maxi_dbl(double *a, int n);

/**
 * frequency - Calculate the frequency of an index in the frequency domain
 * @sample_rate: sample rate from time domain before conversion to frequency domain
 * @bin_index: index of frequency bin in frequency domain
 * @nbins: number of bins in the frequency domain
 */
double frequency(int sample_rate, int bin_index, int nbins);

#endif
