/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "file.h"

int filesz(int fd)
{
	struct stat s;

	if (fstat(fd, &s) == -1)
		return -1;
	return s.st_size;
}

