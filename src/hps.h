/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Harmonic product spectrum.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef HPS_H
#define HPS_H

#include <string.h>
#include "math.h"

/**
 * hps - Proecss a harmonic product spectrum (HPS) array for finding the fundamental frequency from
 *	a monophonic audio signal with harmonics, such as from playing a single guitar string
 *	note
 * @mag: array of magnitudes 
 * @h: empty harmonic product spectrum array 
 * @len: length of both arrays
 * @n: number of times to downsample and largest downsample integer
 */
void hps(double *mag, double *h, int len, int n);

#endif
