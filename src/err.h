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

// Program name.
extern char *prgname;

/**
 * err_set_prgname - Set the program name so that it shows up
 *	in susbsequent print calls to err_print
 *
 * @name: program name
 */
void err_set_prgname(char *name);

/**
 * err_print - Print an error message to stderr with format 
 *	"prgname: description: source of error"
 * @description: description or error
 * @src: more direct source of error
 *
 * Call err_set_prgname before calling this.
 */
void err_print(const char *description, const char *src);

#endif
