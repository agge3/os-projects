/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 3 – Simple Shell with Pipes
*
* File:: baxter_tyler_H3_shell.h
*
* Description:: Simple Shell public API.
*
**************************************************************/

#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>

// Buffer iterator.
struct SH_Buf;
struct SH_Buf *SH_BufCreate(char *buf, size_t sz);
void SH_BufDestroy(struct SH_Buf **shbuf);
char *SH_BufFirst(struct SH_Buf *shbuf);
int SH_BufDone(struct SH_Buf *shbuf);
char *SH_BufNext(struct SH_Buf *shbuf);

// Sets the prompt for Simple Shell.
char *SH_SetPrompt(char **argv);

/*
 * Draws Simple Shell's prompt and populates the caller's buffer with either a 
 * newline-delimited or EOF-delimited standard input stream.
 *
 * @remark buf is not cleared. Parse for the delimiter.
 *
 * @return Number of bytes read.
 */
ssize_t SH_Prompt(char *prompt, char *buf);

int SH_Exec(char **shargv);
char **SH_ARGV(char *buf);
int SH_Exit(char **shargv);
int SH_Print(char *msg);

#endif

// vim: filetype=c:noexpandtab:shiftwidth=8:tabstop=8:softtabstop=8:textwidth=80
