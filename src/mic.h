/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Reading from microphone input device. 
 * Wrapper for portaudio input.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef MIC_H
#define MIC_H

#include <portaudio.h>
#include <stdlib.h>
#include <stdio.h>
#include "err.h"

struct microphone {
	PaStream *stream;
	float *samples;
};

typedef struct microphone mic_t;

/**
 * mic_init - Initialise a new microphone for audio input
 * @sample_rate: samples per second (Hz)
 * @readsz: number of samples per read (although a read doesn't have to match this, but
 *	it should for performance reasons)
 * 
 * Uses the default microphone. Return NULL on error. Starts the underlying
 * stream which can be stopped with mic_free (which also cleans up the data structure).
 */
mic_t *mic_init(int sample_rate, int readsz);

/**
 * mic_read - Read samples from a microphone
 * @samples: array to store read samples into
 * @readsz: number of samples to read (preferrably the same as sthe init-time read size)
 *
 * Microphone must have been initialised with call to mic_init before calling this.
 * Blocks until underlying samples buffer is filled with the read size amount of samples.
 */
void mic_read(mic_t *m, float *samples, int readsz);

/**
 * mic_cleanup - Clean up a microphone data structure initialised with mic_init
 */
void mic_cleanup(mic_t *m);

#endif
