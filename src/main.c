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
#include "err.h"

gtune_t g = { 0 };

/*
 * cleanup - Clean up the program at exit
 */
void cleanup(void)
{
	gtune_cleanup(&g);
}

int main(int argc, char *argv[])
{
	err_set_prgname(argv[0]);
	sig_block();

	if (gtune_init(&g, 44100, 32768, 4, 20, 1500, paFloat32) == false) {
		eprintf("failed to init gtune");
		return EXIT_FAILURE;
	}
	atexit(cleanup);
	sig_handle();

	gtune_start(&g);
	// This shouldn't be reached.
	return EXIT_FAILURE;
}
