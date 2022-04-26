/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "win.h"

/*
 * hann - Compute the Hanning function for a number i in range 0 <= i < n.
 */
double hann(int i, int n)
{
	return 0.5*(1-cos((2*M_PI*i)/(n-1)));
}

void hanning_window(double *a, int n)
{
	for (int i = 0; i < n; ++i)
		a[i] *= hann(i, n);
}
