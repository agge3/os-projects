/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 3 – Simple Shell with Pipes
*
* File:: baxter_tyler_H3_main.c
*
* Description:: Simple Shell globals and macros.
*
**************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdlib.h>
#include <err.h>

// We're going to wrapper all the idiosyncratic "school assignment" parts in
// `SIMPSH` for later processing out. I'm excited to use this as a template for
// my own shell, following "Linux Application Development", by Michael K.
// Johnson.
#define SIMPSH
//#define ADVSH

// Simple Shell has debug and test run options.
//#define DEBUG
//#define TEST

// We define an always TRUE running state here.
#define RUNNING 1

// Public API supports 103 bytes. The `+ 1` is in the case the client fills the
// entire 103 bytes and we still need to delimitate.
#define BUFSZ (103 + 1)

// Simple Shell's UNIX pipe(2) internal macros.
#define PIPE_READ 0
#define PIPE_WRITE 1

// Simple Shell's parsed argv allocator.
#define ALLOC_ARGV(argv)						\
	do {								\
		size_t bufsz = (BUFSZ - 1) / 2;				\
		(argv) = calloc(bufsz + 1, sizeof(char *));		\
		if ((argv) == NULL) {					\
			err(EXIT_FAILURE, "calloc");			\
		}							\
	} while (0)

// Simple Shell's parsed argv deallocator.
#define FREE_ARGV(argv)							\
	do {								\
		if ((argv) != NULL) {					\
			for (size_t i = 0; (argv)[i] != NULL; i++) {	\
				free((argv)[i]);			\
				(argv)[i] = NULL;			\
			}						\
			free((argv));					\
			(argv) = NULL;					\
		}							\
	} while (0)

#endif // GLOBAL_H

// vim: filetype=c:noexpandtab:shiftwidth=8:tabstop=8:softtabstop=8:textwidth=80
