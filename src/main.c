/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Entry point to program.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include <stdlib.h>
#include "gtune.h"
#include "sig.h"

gtune_t *g;

/**
 * cleanup - Clean up the program at exit
 */
void cleanup(void)
{
	gtune_cleanup(g);
}

int main(void)
{
	sig_block();

	g = gtune_init(44100, 32768, 1, 20, 1500);

	atexit(cleanup);
	sig_handle();

	gtune_start(g);

	return 0;
}
