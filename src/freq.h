/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Data structure and funcs/algorithms for getting the frequency
 * of an audio signal.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef FREQ_H
#define FREQ_H

#include <stdlib.h>
#include <fftw3.h>
#include "math.h"
#include "win.h"
#include "norm.h"

struct frequency_data {
	int sample_rate;
	int chunksz;  // Size of a chunk to process in samples.
	int maxi;  // Index of max.
	fftw_plan p;  // Data required by FFT operation.
	double *norm;  // Normalised array of data between -1 and 1 (input to FFT).
	fftw_complex *c;  // Complex number output of FFT operation.
	double *mag;  // Magnitude frequency outputs of complex data.
	double *hps;  // Harmonic product spectrum array.
};

typedef struct frequency_data fdata_t;


/*
 * fdata_init - Initialise a new frequency data
 * @sample_rate: sample rate in Hz
 * @chunksz: size of chunk which restricts the samples being processed. Also
 *	the number of samples processed each execution
 *
 * Free with with fdata_free.
 */
void fdata_init(fdata_t *f, int sample_rate, int chunksz);

/*
 * fdata_free - Free a frequency data initialised with fdata_init
 */
void fdata_free(fdata_t *f);


/*
 * fdata_process_chunk - Process a chunk of samples for a frequency
 * See process_chunk for more info.
 */
double fdata_process_chunk(fdata_t *f, char *samples, sdtype_meta_t *meta, bool skip_normalise);

#endif
