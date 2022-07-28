/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "err.h"

static char *prgname = NULL;

void err_set_prgname(char *name)
{
	prgname = name;
}

void err_print(const char *description, const char *src)
{
	fprintf(stderr, "%s: %s: %s\n", prgname, description, src);
}

