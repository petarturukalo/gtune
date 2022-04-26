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
#include "hps.h"

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
 * Return NULL on error. Dynamically allocated, must be freed with fdata_free.
 */
void fdata_init(fdata_t *f, int sample_rate, int chunksz);

/*
 * fdata_free - Free a frequency data initialised with fdata_init
 */
void fdata_free(fdata_t *f);


/*
 * fdata_process_chunk - Process a chunk of 32-bit float samples where each float has
 *	already been preprocessed to be in range -1 to 1 (endpoints inclusive)
 * @samples: samples to process
 */
void fdata_process_chunk_float32_preprocd(fdata_t *f, float *samples);

/*
 * fdata_process_chunk_sint16 - Process a chunk of signed 16-bit integer samples
 */
void fdata_process_chunk_sint16(fdata_t *f, short *samples);

/*
 * fdata_process_all_sint16 - Process all signed 16-bit integer samples
 * @nsamps: number of samples
 * @stepsz: number of samples to move forward after each processing of a chunk of samples
 * @n: out-parameter to store length of returned frequencies array
 *
 * Return array of frequencies, each element being a frequency for a chunk of chunk size
 * matching the init-time chunk size specification.
 */
double *fdata_process_all_sint16(fdata_t *f, short *samples, int nsamps, int stepsz, int *n);

/*
 * fdata_frequency - Get the frequency from processed samples
 *
 * Gets the frequency of the samples last processed by function fdata_process.
 * Assumes fdata_process has been called at least once (after initialisation, also).
 */
double fdata_frequency(fdata_t *f);

#endif
