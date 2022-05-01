/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "test-note.h"
#include "test-math.h"
#include "test-norm.h"

int main(void)
{
	test_note_entry();
	test_math_entry();
	test_norm_entry();
	return 0;
}
