/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 3 – Simple Shell with Pipes
*
* File:: baxter_tyler_H3_int_shell.h
*
* Description:: Internal shell API. This needs to be exposed mainly for test
* suite.
*
**************************************************************/

#ifndef INT_SHELL_H
#define INT_SHELL_H

#include <stdbool.h>
#include <stdlib.h>

// Internal state container for parsing comments.
// xxx to be implemented
void parse_comment(bool *comment, char *ch);

/**
 * Returns an argument vector from a character buffer. Heap allocates so argv 
 * can be shared with children processes.
 *
 * @pre		Expects space-separated arguments.
 * @post	Caller's responsibility to free with free_argv.
 */
char **process_argv(char *buf);

int check_exec(char *exec);
int cpipe(int pipefd[2]);
int ppipe(int pipefd[2]);
int shexec(char **argv);
int shpexec(char ***cargvs);
char ***process_op(char **shargv, const char *op, size_t *size);

#endif	// INT_SHELL_H

// vim: filetype=c:noexpandtab:shiftwidth=8:tabstop=8:softtabstop=8:textwidth=80
