/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "hps.h"

void hps(double *mag, double *h, int len, int n)
{
	int ds, end, i;

	memcpy((void *)h, (void *)mag, len*sizeof(double));

	// Skip downsampling first since already done by memcpy.
	for (ds = 2; ds <= n; ++ds) {
		end = len/ds;
		
		for (i = 0; i < end; ++i)
			h[i] *= mag[i*ds];
	}
}
