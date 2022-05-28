/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Operations on files.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef FILE_H
#define FILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * filesz - Get the size of a file in bytes
 * @fd: file descriptor to open file
 */
int filesz(int fd);

#endif
