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

short *fread_sint16(char *fpath, int *n)
{
	short *buf;
	int fsz, fd;

	fd = open(fpath, O_RDONLY);
	fsz = filesz(fd);

	// File is in bytes, and each buffer element is 2 bytes.
	// Assumes the file size is even.
	*n = fsz/2;  
	buf = malloc(*n*sizeof(short));
	read(fd, (void *)buf, fsz);

	close(fd);

	return buf;
}

