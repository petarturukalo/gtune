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
#include <errno.h>
#include "math.h"
#include "err.h"
#include "norm.h"

struct frequency_data {
	uint sample_rate;
	uint chunksz;  // Size of a chunk to process in samples.
	fftw_plan p;  // Data required by FFT operation.
	double *norm;  // Normalised array of data between -1 and 1 (input to FFT).
	fftw_complex *c;  // Complex number output of FFT operation.
	double *mag;  // Magnitude frequency outputs of complex data.
	double *hps;  // Harmonic product spectrum array.
	// (The mag and hps arrays could be combined to save space since they're used 
	// sequentially and not at the same time, but it would make the code harder to read.)
};

typedef struct frequency_data fdata_t;


/*
 * fdata_init - Initialise a new frequency data
 * @sample_rate: sample rate in Hz
 * @chunksz: size of chunk which restricts the samples being processed. Also
 *	the number of samples processed each execution
 *
 * Return whether the initialisation was successful. Free with with fdata_free.
 */
bool fdata_init(fdata_t *f, uint sample_rate, uint chunksz);

/*
 * fdata_free - Free a frequency data initialised with fdata_init
 */
void fdata_free(fdata_t *f);


/*
 * fdata_process_chunk - Process a chunk of samples into a frequency
 * @samples: samples to process 
 * @meta: metadata describing the numeric data type of a sample. 
 * @skip_normalise: whether to skip normalising the samples because they are already normalised
 *
 * Return the frequency of the samples.
 * Samples are converted to double as that's the required data type input to the implementation 
 * of FFT in use, but the user could have read a different data type, such as signed 16-bit integers, 
 * or 32-bit floats, etc.
 */
double fdata_process_chunk(fdata_t *f, char *samples, sdtype_meta_t *meta, bool skip_normalise);

#endif
