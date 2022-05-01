/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#include "norm.h"

// Maximum size of a sample is 8 bytes, i.e. the
// typical size of a double.
#define MAX_SAMPLE_SZ 8

/*
 * Get the max sample in an array of samples.
 * @samples: the array of samples
 * @samplesz: the size of a sample in bytes
 * @n: the number of samples in the array
 * @out_max: where to store the max sample. This should be of size samplesz
 * @gt: function to compare whether a sample is greater than another sample
 */
static bool max_sample(char *samples, uint samplesz, uint n, char *out_max, 
		       bool (*gt)(void *, void *))
{
	if (n) {
		memcpy(out_max, samples, samplesz);
		samples += samplesz;
		for (int i = 1; i < n; ++i) {
			if (gt(samples, out_max))
				memcpy(out_max, samples, samplesz);
			samples += samplesz;
		}
	} 
	return n;
}

/*
 * Get the min sample in an array of samples.
 * @samples: the array of samples
 * @samplesz: the size of a sample in bytes
 * @n: the number of samples in the array
 * @out_min: where to store the min sample. This should be of size samplesz
 * @lt: function to compare whether a sample is less than another sample
 */
static bool min_sample(char *samples, uint samplesz, uint n, char *out_min, 
		       bool (*lt)(void *, void *))
{
	if (n) {
		memcpy(out_min, samples, samplesz);
		samples += samplesz;
		for (int i = 1; i < n; ++i) {
			if (lt(samples, out_min))  {
				memcpy(out_min, samples, samplesz);
			}
			samples += samplesz;
		}
	} 
	return n;
}

static bool lt_float(void *a, void *b)  { return *(float *)a < *(float *)b; }
static bool gt_float(void *a, void *b)  { return *(float *)a > *(float *)b; }
static bool lt_double(void *a, void *b) { return *(double *)a < *(double *)b; }
static bool gt_double(void *a, void *b) { return *(double *)a > *(double *)b; }
static bool lt_short(void *a, void *b)  { return *(short *)a < *(short *)b; }
static bool gt_short(void *a, void *b)  { return *(short *)a > *(short *)b; }
static bool lt_int(void *a, void *b)    { return *(int *)a < *(int *)b; }
static bool gt_int(void *a, void *b)    { return *(int *)a > *(int *)b; }
static bool lt_ushort(void *a, void *b) { return *(unsigned short *)a < *(unsigned short *)b; }
static bool gt_ushort(void *a, void *b) { return *(unsigned short *)a > *(unsigned short *)b; }
static bool lt_uint(void *a, void *b)   { return *(unsigned int *)a < *(unsigned int *)b; }
static bool gt_uint(void *a, void *b)   { return *(unsigned int *)a > *(unsigned int *)b; }

static double ftod(void *p)  { return *(float *)p; }
static double dtod(void *p)  { return *(double *)p; }
static double stod(void *p)  { return *(short *)p; }
static double itod(void *p)  { return *(int *)p; }
static double ustod(void *p) { return *(unsigned short *)p; }
static double uitod(void *p) { return *(unsigned int *)p; }

struct sdtype_fns {
	bool (*lt)(void *, void *);  // Less than.
	bool (*gt)(void *, void *);  // Greater than.
	double (*xtod)(void *);  // Convert a numeric type to a double.
};

/*
 * Select comparison and conversion functions dependent on a sample's numeric data type.
 */
static struct sdtype_fns select_sdtype_fns(sdtype_number_type type)
{
	switch (type) {
		case SDTYPE_FLOAT:  return (struct sdtype_fns){ lt_float,  gt_float,  ftod };
		case SDTYPE_DOUBLE: return (struct sdtype_fns){ lt_double, gt_double, dtod };
		case SDTYPE_SHORT:  return (struct sdtype_fns){ lt_short,  gt_short,  stod };
		case SDTYPE_INT:    return (struct sdtype_fns){ lt_int,    gt_int,    itod };
		case SDTYPE_USHORT: return (struct sdtype_fns){ lt_ushort, gt_ushort, ustod };
		case SDTYPE_UINT:   return (struct sdtype_fns){ lt_uint,   gt_uint,   uitod };
	}
	return (struct sdtype_fns){ NULL, NULL, NULL };
}

/*
 * normalise - Move a number from its current min-max range into a range with min -1 and
 *	max 1 (range of the new normalised number)
 * @n: number to normalise
 * @min: minimum in range number is a part of
 * @max: maximum in range number is a part of
 *
 * The min number in the original range gets normalised value -1, and the max number
 * gets 1.
 */
static double normalise(double n, double min, double max)
{
	return nr_new_range(n, min, max, -1, 1);
}

bool normalise_samples(char *samples, uint n, sdtype_meta_t *meta, double *norm)
{
	char min_samp[MAX_SAMPLE_SZ];
	char max_samp[MAX_SAMPLE_SZ];
	struct sdtype_fns fns = select_sdtype_fns(meta->number_type);

	bzero(min_samp, sizeof(min_samp));
	bzero(max_samp, sizeof(max_samp));

	if (!min_sample(samples, meta->samplesz, n, min_samp, fns.lt))
		return false;
	if (!max_sample(samples, meta->samplesz, n, max_samp, fns.gt))
		return false;

	for (int i = 0; i < n; ++i) {
		norm[i] = normalise(fns.xtod(samples), fns.xtod(min_samp), 
				    fns.xtod(max_samp));
		samples += meta->samplesz;
	}
	return true;
}

bool normalise_samples_copy(char *samples, uint n, sdtype_meta_t *meta, double *norm, bool copy)
{
	if (copy) {
		struct sdtype_fns fns = select_sdtype_fns(meta->number_type);

		for (int i = 0; i < n; ++i) {
			norm[i] = fns.xtod(samples);
			samples += meta->samplesz;
		}
		return true;
	} else
		return normalise_samples(samples, n, meta, norm);
}

sdtype_meta_t sdtype_meta_float32  = { SDTYPE_FLOAT,  sizeof(float) };
sdtype_meta_t sdtype_meta_double64 = { SDTYPE_DOUBLE, sizeof(double) };
sdtype_meta_t sdtype_meta_int16    = { SDTYPE_SHORT,  sizeof(short) };
sdtype_meta_t sdtype_meta_int32    = { SDTYPE_INT,    sizeof(int) };
sdtype_meta_t sdtype_meta_uint16   = { SDTYPE_USHORT, sizeof(unsigned short) };
sdtype_meta_t sdtype_meta_uint32   = { SDTYPE_UINT,   sizeof(unsigned int) };

void norm_assert(void)
{
	// Make sure all ints fit in a double since they're converted to double.
	assert((int)INT_MAX == (double)INT_MAX);
	assert((uint)UINT_MAX == (double)UINT_MAX);

	assert(sdtype_meta_float32.samplesz  == 4);
	assert(sdtype_meta_double64.samplesz == 8);
	assert(sdtype_meta_int16.samplesz    == 2);
	assert(sdtype_meta_int32.samplesz    == 4);
	assert(sdtype_meta_uint16.samplesz   == 2);
	assert(sdtype_meta_uint32.samplesz   == 4);

	assert(MAX_SAMPLE_SZ == sizeof(double));
	assert(MAX_SAMPLE_SZ == sizeof(long));
	assert(MAX_SAMPLE_SZ == sizeof(unsigned long));
}

