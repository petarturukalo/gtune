/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Signal handling for cleaning up the program at exit time.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef SIG_H
#define SIG_H

#include <signal.h>
#include <stdlib.h>

/*
 * sig_block - Block signals so that initialisation of the program
 *	isn't interrupted
 */
void sig_block(void);

/*
 * sig_handle - Set up signal handlers for exiting and cleaning up the program
 */
void sig_handle(void);

#endif
