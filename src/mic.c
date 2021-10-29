/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "mic.h"

/**
 * mic_set_params - Set default parameters for the microphone
 *
 * Return -1 on couldn't get the default device. Return 0 on success.
 */
int mic_set_params(PaStreamParameters *p)
{
	const PaDeviceInfo *info;

	p->device = Pa_GetDefaultInputDevice();

	if (p->device == paNoDevice)
		return -1;
	info = Pa_GetDeviceInfo(p->device);
	// Not a very helpful name since it's just "default",
	// but print it anyway.
	printf("using audio input device '%s'\n", info->name);

	p->channelCount = 1;
	p->sampleFormat = paFloat32;
	p->suggestedLatency = info->defaultLowInputLatency;
	p->hostApiSpecificStreamInfo = NULL;
	return 0;
}

mic_t *mic_init(int sample_rate, int readsz)
{
	PaError err;
	mic_t *m;
	PaStreamParameters mic_params;

	err = Pa_Initialize();

	if (err != paNoError) {
		err_print("couldn't init portaudio", Pa_GetErrorText(err));
		return NULL;
	}

	m = malloc(sizeof(mic_t));
	err = mic_set_params(&mic_params);

	if (err == -1) {
		err_print("couldn't get default input device", Pa_GetErrorText(err));
		free(m);
		Pa_Terminate();
		return NULL;
	}

	err = Pa_OpenStream(&m->stream, &mic_params, NULL, sample_rate, readsz, 
			    paClipOff, NULL, NULL);

	if (err != paNoError) {
		err_print("couldn't open audio input stream", Pa_GetErrorText(err));
		Pa_Terminate();
		free(m);
		return NULL;
	}

	err = Pa_StartStream(m->stream);

	if (err != paNoError) {
		err_print("couldn't start audio input stream", Pa_GetErrorText(err));
		Pa_CloseStream(m->stream);
		Pa_Terminate();
		free(m);
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

