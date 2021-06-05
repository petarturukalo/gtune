/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "mic.h"

/**
 * mic_set_params - Set default parameters for the microphone
 */
void mic_set_params(PaStreamParameters *p)
{
	p->device = Pa_GetDefaultInputDevice();
	p->channelCount = 1;
	p->sampleFormat = paFloat32;
	p->suggestedLatency = Pa_GetDeviceInfo(p->device)->defaultLowInputLatency;
	p->hostApiSpecificStreamInfo = NULL;
}

mic_t *mic_init(int sample_rate, int readsz)
{
	PaError err;
	mic_t *m;
	PaStreamParameters mic_params;

	err = Pa_Initialize();

	if (err != paNoError) {
		fprintf(stderr, "couldn't init portaudio\n");
		return NULL;
	}

	m = malloc(sizeof(mic_t));
	mic_set_params(&mic_params);

	err = Pa_OpenStream(&m->stream, &mic_params, NULL, sample_rate, readsz, 
			    paClipOff, NULL, NULL);

	if (err != paNoError) {
		Pa_Terminate();
		free(m);
		fprintf(stderr, "couldn't open audio input stream\n");
		return NULL;
	}

	err = Pa_StartStream(m->stream);

	if (err != paNoError) {
		Pa_CloseStream(m->stream);
		Pa_Terminate();
		free(m);
		fprintf(stderr, "couldn't start audio input stream\n");
		return NULL;
	}

	return m;
}

void mic_read(mic_t *m, float *samples, int readsz)
{
	Pa_ReadStream(m->stream, (void *)samples, readsz);
}

void mic_cleanup(mic_t *m)
{
	Pa_StopStream(m->stream);
	Pa_CloseStream(m->stream);
	Pa_Terminate();
	free(m);
}

