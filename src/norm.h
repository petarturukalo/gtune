/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#ifndef NORM_H
#define NORM_H

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include "math.h"

typedef enum {
	SDTYPE_FLOAT,
	SDTYPE_DOUBLE,
	SDTYPE_SHORT,
	SDTYPE_INT,
	SDTYPE_USHORT,
	SDTYPE_UINT
} sdtype_number_type;

/*
 * Data type dependent data required to normalise samples.
 */
struct sample_data_type_metadata {
	sdtype_number_type number_type;
	uint samplesz;  // Size of a sample in bytes.
};

typedef struct sample_data_type_metadata sdtype_meta_t;

extern sdtype_meta_t sdtype_meta_float32;
extern sdtype_meta_t sdtype_meta_double64;
extern sdtype_meta_t sdtype_meta_int16;
extern sdtype_meta_t sdtype_meta_int32;
extern sdtype_meta_t sdtype_meta_uint16;
extern sdtype_meta_t sdtype_meta_uint32;

/*
 * Normalise audio samples into range -1 to 1 (both ends inclusive). 
 * The minimum of the samples becomes -1 and the max 1.
 * @samples: array of samples to normalise
 * @n: number of samples
 * @meta: metadata describing the data type of the samples. 
 * @norm: out-param array where to store normalised samples. This should be the 
 *	same length as the samples array.
 */
bool normalise_samples(char *samples, uint n, sdtype_meta_t *meta, double *norm);
/*
 * Copy the samples directly to the normalised array.
 */
void normalise_samples_copy(char *samples, uint n, sdtype_meta_t *meta, double *norm);

/*
 * Assert that the normalise functions can be used on this machine.
 */
void norm_assert(void);

#endif
