/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "sig.h"

void sig_block(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
}

void sighandle(int sig)
{
	// Will call the cleanup function.
	exit(EXIT_SUCCESS);
}

/**
 * sig_setup - Set up the handler for a signal
 * @sig_handle: signal to handle
 * @sig_block: signal to block while handling
 */
void sig_setup(int sig_handle, int sig_block)
{
	struct sigaction sa;

	sa.sa_handler = sighandle;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, sig_block);

	sigaction(sig_handle, &sa, NULL);
}

void sig_handle(void)
{
	sig_setup(SIGINT, SIGTERM);
	sig_setup(SIGTERM, SIGINT);
}

