/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "test-math.h"

void assert_nr_new_range(int n, int start, int end, 
			 int new_start, int new_end, double sln)
{
	double ans = nr_new_range(n, start, end, new_start, new_end);

	/*printf("i=%d,ans=%f,sln=%f\n", n, ans, sln);*/

	assert(ans == sln);
}

void test_nr_new_range(void)
{
	for (int i = 0; i < 10; ++i)
		assert_nr_new_range(i, 0, 10, 0, 1, i/(double)10);

	assert_nr_new_range(5, 0, 10, -1, 1, 0);
	assert_nr_new_range(0, -2, 2, -1, 1, 0);
	assert_nr_new_range(1, -2, 2, -1, 1, 0.5);
}

void test_math_entry(void)
{
	test_nr_new_range();
	printf("foo\n");
}
