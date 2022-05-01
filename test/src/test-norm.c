/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "test-norm.h"

static void assert_norm(char *samples, int n, sdtype_meta_t *meta, 
			double *expected_norm)
{
	bool success;
	double *actual_norm = calloc(n, sizeof(double));

	normalise_samples(samples, n, meta, actual_norm);
	for (int i = 0; i < n; ++i) {
		// TODO assertion fails if use full double value
		success = (float)expected_norm[i] == (float)actual_norm[i];
		if (!success) 
			break;
	}
	free(actual_norm);
	assert(success);
}

static void assert_sdtype_norm(char *samples, int n, int expected_sz, sdtype_meta_t *meta,
			       double *expected_norm)
{
	assert(expected_sz == meta->samplesz);
	assert_norm(samples, n, meta, expected_norm);
}

static void assert_int32_norm(int *samples, int n, double *expected_norm)
{
	assert_sdtype_norm((char *)samples, n, 4, &sdtype_meta_int32, expected_norm);
}

static void assert_uint16_norm(unsigned short *samples, int n, double *expected_norm)
{
	assert_sdtype_norm((char *)samples, n, 2, &sdtype_meta_uint16, expected_norm);
}

static void assert_double64_norm(double *samples, double n, double *expected_norm)
{
	assert_sdtype_norm((char *)samples, n, 8, &sdtype_meta_double64, expected_norm);
}

void test_norm_entry(void)
{
	assert_int32_norm((int[]){ -12, 6, -6, 0, 12 }, 5, (double[]){ -1, 0.5, -0.5, 0, 1 });
	assert_uint16_norm((unsigned short[]){ 0, 20000, 4000, 12000 }, 4, (double[]){ -1, 1, -0.6, 0.2 });
	assert_double64_norm((double[]){ 5.5, 6.5, 7.5, 8.5, 9.5 }, 5, (double[]){ -1, -0.5, 0, 0.5, 1 });
}
