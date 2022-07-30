/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * For printing errors in a consistent format.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef ERR_H
#define ERR_H

#include <stdio.h>
#include <stdarg.h>

/*
 * err_set_prgname - Set the program name so that it shows up
 *	in susbsequent print calls to err_print
 *
 * @name: program name
 */
void err_set_prgname(char *name);

/*
 * Print an error message to stderror using printf style formatting.
 */
void eprintf(const char *format, ...);

#endif
