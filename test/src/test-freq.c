/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "test-freq.h"

/**
 * process_all_sint16 - Test processing the whole of a signed 16-bit integer file
 */
void process_all_sint16(void)
{
	short *samples;
	int nsamps, nfreqs;
	double *freqs;
	fdata_t *f = fdata_init(44100, 8192);

	samples = fread_sint16("data/e.wav", &nsamps);
	printf("read %d samples\n", nsamps);
	freqs = fdata_process_all_sint16(f, samples, nsamps, 8192, &nfreqs);

	printf("%d FREQS\n", nfreqs);
	for (int i = 0; i < nfreqs; ++i)
		printf("freq[%d]=%f\n", i, freqs[i]);

	free(samples);
	free(freqs);
	fdata_free(f);
}

void test_freq_entry(void)
{
	process_all_sint16();
}
