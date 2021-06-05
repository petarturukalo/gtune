/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Window functions for FFT preprocessing.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef WIN_H
#define WIN_H

#include <math.h>

/**
 * hanning_window - Run a hanning window on an array (in-place)
 * @a: array to run Hanning window on
 * @n: length of array
 */
void hanning_window(double *a, int n);

#endif
