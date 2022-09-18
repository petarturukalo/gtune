/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "mic.h"

/*
 * Set default parameters for the microphone.
 * Return -1 on couldn't get the default device. Return 0 on success.
 */
int mic_set_params(PaStreamParameters *p, PaSampleFormat fmt)
{
	const PaDeviceInfo *dev;
	const PaHostApiInfo *host;

	p->device = Pa_GetDefaultInputDevice();

	if (p->device == paNoDevice) {
		eprintf("couldn't get default input device");
		return -1;
	}
	dev = Pa_GetDeviceInfo(p->device);
	host = Pa_GetHostApiInfo(dev->hostApi);
	printf("using %s %s audio input device\n", host->name, dev->name);

	// Use 1 channel for now since multiple will only help with redundancy (as in all channels
	// producing the same note) and not with getting a note reading faster unless gtune is refactored 
	// to work in parallel, e.g. with threads handling separate channels.
	p->channelCount = 1;
	p->sampleFormat = fmt;
	p->suggestedLatency = dev->defaultLowInputLatency;
	p->hostApiSpecificStreamInfo = NULL;
	return 0;
}

bool mic_init(mic_t *m, uint sample_rate, uint readsz, PaSampleFormat fmt)
{
	PaError err;
	PaStreamParameters mic_params;

	err = Pa_Initialize();

	if (err != paNoError) {
		eprintf("couldn't init portaudio: %s", Pa_GetErrorText(err));
		return false;
	}
	err = mic_set_params(&mic_params, fmt);

	if (err == -1) {
		Pa_Terminate();
		return false;
	}
	err = Pa_OpenStream(&m->stream, &mic_params, NULL, sample_rate, readsz, 
			    paClipOff, NULL, NULL);
	if (err != paNoError) {
		eprintf("couldn't open audio input stream: %s", Pa_GetErrorText(err));
		Pa_Terminate();
		return false;
	}
	err = Pa_StartStream(m->stream);

	if (err != paNoError) {
		eprintf("couldn't start audio input stream: %s", Pa_GetErrorText(err));
		Pa_CloseStream(m->stream);
		Pa_Terminate();
		return false;
	}
	return true;
}

void mic_read_until_success(mic_t *m, char *samples, uint readsz)
{
	PaError err;

	while ((err = Pa_ReadStream(m->stream, (void *)samples, readsz)) != paNoError)
		eprintf("failed to read audio input samples: %s", Pa_GetErrorText(err));
}

void mic_cleanup(mic_t *m)
{
	if (m) {
		Pa_StopStream(m->stream);
		Pa_CloseStream(m->stream);
		Pa_Terminate();
	}
}

