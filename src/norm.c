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
 * Get the best sample in an array of samples, where the "best" sample is determined
 * by the comparison function.
 * @samples: the array of samples
 * @samplesz: the size of a sample in bytes
 * @n: the number of samples in the array
 * @out_max: where to store the max sample. This should be of size samplesz
 * @cmp: comparison function comparing whether its first param is better than
 *	its second
 */
static bool best_sample(char *samples, uint samplesz, uint n, char *out_best,
			bool (*cmp)(void *, void *))
{
	if (!samples || !samplesz || !n || !out_best || !cmp)
		return false;

	memcpy(out_best, samples, samplesz);
	samples += samplesz;
	while (--n) {
		if (cmp(samples, out_best))
			memcpy(out_best, samples, samplesz);
		samples += samplesz;
	}
	return true;
}

/*
 * Get the max sample in an array of samples.
 * @lt: function to compare whether a sample is greater than another sample
 */
static bool max_sample(char *samples, uint samplesz, uint n, char *out_max, 
		       bool (*gt)(void *, void *))
{
	return best_sample(samples, samplesz, n, out_max, gt);
}

/*
 * Get the min sample in an array of samples.
 * @lt: function to compare whether a sample is less than another sample
 */
static bool min_sample(char *samples, uint samplesz, uint n, char *out_min, 
		       bool (*lt)(void *, void *))
{
	return best_sample(samples, samplesz, n, out_min, lt);
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

static struct sdtype_fns float_fns  = { lt_float,  gt_float,  ftod };
static struct sdtype_fns double_fns = { lt_double, gt_double, dtod };
static struct sdtype_fns short_fns  = { lt_short,  gt_short,  stod };
static struct sdtype_fns int_fns    = { lt_int,    gt_int,    itod };
static struct sdtype_fns ushort_fns = { lt_ushort, gt_ushort, ustod };
static struct sdtype_fns uint_fns   = { lt_uint,   gt_uint,   uitod };

/*
 * Select comparison and conversion functions dependent on a sample's numeric data type.
 */
static struct sdtype_fns *select_sdtype_fns(sdtype_number_type type)
{
	switch (type) {
		case SDTYPE_FLOAT:  return &float_fns;
		case SDTYPE_DOUBLE: return &double_fns;
		case SDTYPE_SHORT:  return &short_fns;
		case SDTYPE_INT:    return &int_fns;
		case SDTYPE_USHORT: return &ushort_fns;
		case SDTYPE_UINT:   return &uint_fns;
	}
	return NULL;
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
	char min_samp_bytes[MAX_SAMPLE_SZ];
	char max_samp_bytes[MAX_SAMPLE_SZ];
	struct sdtype_fns *fns = select_sdtype_fns(meta->number_type);
	double min_samp, max_samp;

	bzero(min_samp_bytes, sizeof(min_samp_bytes));
	bzero(max_samp_bytes, sizeof(max_samp_bytes));

	if (min_sample(samples, meta->samplesz, n, min_samp_bytes, fns->lt) == false)
		return false;
	if (max_sample(samples, meta->samplesz, n, max_samp_bytes, fns->gt) == false)
		return false;
	min_samp = fns->xtod(min_samp_bytes);
	max_samp = fns->xtod(max_samp_bytes);

	for (; n--; ++norm) {
		*norm = normalise(fns->xtod(samples), min_samp, max_samp);
		samples += meta->samplesz;
	}
	return true;
}

void normalise_samples_copy(char *samples, uint n, sdtype_meta_t *meta, double *norm)
{
	struct sdtype_fns *fns = select_sdtype_fns(meta->number_type);

	for (; n--; ++norm) {
		*norm = fns->xtod(samples);
		samples += meta->samplesz;
	}
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

