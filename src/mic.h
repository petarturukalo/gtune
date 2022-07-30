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
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "err.h"

struct microphone {
	PaStream *stream;
};

typedef struct microphone mic_t;

/*
 * mic_init - Initialise a new microphone for audio input
 * @sample_rate: samples per second (Hz)
 * @readsz: number of samples per read (although a read doesn't have to match this, but
 *	it should for performance reasons)
 * @fmt: pulse audio's description of a sample's data type
 * 
 * Uses the default microphone. Return whether the intialisation was successful. Starts the underlying
 * stream which can be stopped with mic_free (which also cleans up the data structure).
 */
bool mic_init(mic_t *m, uint sample_rate, uint readsz, PaSampleFormat fmt);

/*
 * mic_read_until_success - Read samples from a microphone. If a read fails it keeps retrying 
 *	until it succeeds.
 * @samples: array to store read samples into
 * @readsz: number of samples to read (preferrably the same as the init-time read size)
 *
 * Microphone must have been initialised with call to mic_init before calling this.
 * Blocks until underlying samples buffer is filled with the read size amount of samples.
 */
void mic_read_until_success(mic_t *m, char *samples, uint readsz);

/*
 * mic_cleanup - Clean up a microphone data structure initialised with mic_init
 */
void mic_cleanup(mic_t *m);

#endif
