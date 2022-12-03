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
#include <string.h>
#include <fftw3.h>

/*
 * magnitude - Get the magnitude of a complex number
 */
double magnitude(fftw_complex c);

/*
 * magnitudes - Get the magnitudes of multiple complex numbers
 * @c: array of complex numbers
 * @n: number of magnitudes to calculate 
 */
void magnitudes(fftw_complex *c, double *out_magnitudes, int n);

/*
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
double nr_new_range(double n, double start, double end, double new_start, double new_end);

/*
 * frequency - Calculate the frequency of an index in the frequency domain
 * @sample_rate: sample rate from time domain before conversion to frequency domain
 * @bin_index: index of frequency bin in frequency domain
 * @nbins: number of bins in the frequency domain
 */
double frequency(int sample_rate, int bin_index, int nbins);

/*
 * Run a harmonic product spectrum (HPS) on magnitudes to ignore harmonics and find the fundamental
 * frequency of a note.
 * @out_hps: out-param where hps-processed magnitudes are stored
 * @len: length of both arrays
 * @n: number of times to downsample and largest downsample integer
 */
void hps(double *magnitudes, double *out_hps, int len, int n);

/*
 * hanning_window - Run a hanning window on an array (in-place)
 * @a: array to run Hanning window on
 * @n: length of array
 */
void hanning_window(double *a, int n);

#endif
