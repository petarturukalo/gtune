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

void eprintf(const char *format, ...)
{
	static char output_msg[512];
	va_list ap;

	va_start(ap, format);
	vsnprintf(output_msg, sizeof(output_msg), format, ap);
	va_end(ap);
	
	fprintf(stderr, "ERROR: %s: %s\n", prgname, output_msg);
}

