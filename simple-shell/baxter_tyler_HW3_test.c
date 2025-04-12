/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 3 – Simple Shell with Pipes
*
* File:: baxter_tyler_H3_test.c
*
* Description:: Simple Shell test suite implementation.
*
**************************************************************/

#include "baxter_tyler_HW3_test.h"
#include "baxter_tyler_HW3_global.h"
#include "baxter_tyler_HW3_shell.h"
#include "baxter_tyler_HW3_int_shell.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>

int TEST_ARGV() 
{
	char buf[BUFSZ];
	size_t idx = 0;
	while (idx < 10) {
		buf[idx] = 'a';
		idx++;
	}
	buf[idx++] = ' ';
	buf[idx++] = ' ';
	while (idx < 20) {
		buf[idx] = 'b';
		idx++;
	}
	buf[idx++] = ' ';
	while (idx < 30) {
		buf[idx] = 'c';
		idx++;
	}
	buf[idx++] = ' ';
	buf[idx++] = ' ';
	buf[idx++] = ' ';
	while (idx < 40) {
		buf[idx] = 'd';
		idx++;
	}
	while (idx < 50) {
		buf[idx] = ' ';
		idx++;
	}
	buf[idx] = '\n';

	char **int_argv = process_argv(buf);
	printf("Processed argv:\n");
	int i = 0;
	while (int_argv[i] != NULL) {
		printf("int_arg %02d: %s\n", i, int_argv[i]);
		i++;
	}
	FREE_ARGV(int_argv);

	char **ext_argv = SH_ARGV(buf);
	printf("Processed argv:\n");
	i = 0;
	while (ext_argv[i] != NULL) {
		printf("ext_arg %02d: %s\n", i, ext_argv[i]);
		i++;
	}
	FREE_ARGV(ext_argv);

	/* Mimic a stdin stream. */
	// Done this way to chop off NULL-terminator.
	char *s = "hello world hi world ello world x   xx     xxx";
	char buf2[BUFSZ];
	memcpy(buf2, s, strlen(s));	// no `+ 1` for no NULL-terminator
	// Append newline, like we would have for a stdin stream.
	buf2[strlen(s)] = '\n';
	printf("TEST_ARGV: buf2 string: %s\n", buf2);

	char **int_argv2 = process_argv(buf2);
	printf("Processed int_argv2:\n");
	i = 0;
	while (int_argv2[i] != NULL) {
		printf("int_arg2 %02d: %s\n", i, int_argv2[i]);
		i++;
	}
	FREE_ARGV(int_argv2);

	char **ext_argv2 = SH_ARGV(buf2);
	printf("Processed ext_argv2:\n");
	i = 0;
	while (ext_argv2[i] != NULL) {
		printf("ext_arg2 %02d: %s\n", i, ext_argv2[i]);
		i++;
	}
	FREE_ARGV(ext_argv2);

	return 0;
}

int TEST_Allocators()
{
	char **argv;
	ALLOC_ARGV(argv);
	assert(argv != NULL);
	for (int i = 0; i < 10; i++) {
		char *s = "hello";
		argv[i] = malloc((strlen(s) + 1) * sizeof(char));
		if (argv[i] == NULL) {
			FREE_ARGV(argv);
			perror("malloc failed for arguments");
		}
		strcpy(argv[i], s);
		assert(strcmp(argv[i], s) == 0);
	}
	FREE_ARGV(argv);
	assert(argv == NULL);
	
	ALLOC_ARGV(argv);
	assert(argv != NULL);
	for (int i = 0; i < 10; i++) {
		char *s = "hello";
		argv[i] = malloc((strlen(s) + 1) * sizeof(char));
		if (argv[i] == NULL) {
			FREE_ARGV(argv);
			perror("malloc failed for arguments");
		}
		strcpy(argv[i], s);
		assert(strcmp(argv[i], s) == 0);
	}
	FREE_ARGV(argv);
	assert(argv == NULL);

	ALLOC_ARGV(argv);
	assert(argv != NULL);
	FREE_ARGV(argv);
	assert(argv == NULL);

	return 0;
}

int TEST_StartTest(const char* name)
{
	char header[81];
	for (size_t i = 0; i < 80; i++) {
		header[i] = '#';
	}
	header[80] = '\0';

	printf("%s\n## TEST %s ##\n", header, name);

	return 0;
}

int TEST_ProcessOp()
{
	TEST_StartTest("ProcessOp");

	/* Mimic a stdin stream. */
	// Done this way to chop off NULL-terminator.
	char *s = "cat Makefile | wc -l -w | grep 313 | echo hello";
	char buf2[BUFSZ];
	memcpy(buf2, s, strlen(s));	// no `+ 1` for no NULL-terminator
	// Append newline, like we would have for a stdin stream.
	buf2[strlen(s)] = '\n';
	printf("TEST_ARGV: buf2 string: %s\n", buf2);

	char **int_argv2 = process_argv(buf2);
	printf("Processed int_argv2:\n");
	int i = 0;
	while (int_argv2[i] != NULL) {
		printf("int_arg2 %02d: %s\n", i, int_argv2[i]);
		i++;
	}

	size_t size = 0;
	char ***optest = process_op(int_argv2, "|", &size);
	assert(optest != NULL);
	printf("Exited process_op.\n");

	assert(size == 4);
	assert(strcmp(optest[0][0], "cat") == 0);
	assert(strcmp(optest[0][1], "Makefile") == 0);
	assert(strcmp(optest[1][0], "wc") == 0);
	assert(strcmp(optest[1][1], "-l") == 0);
	assert(strcmp(optest[1][2], "-w") == 0);
	assert(strcmp(optest[2][0], "grep") == 0);
	assert(strcmp(optest[2][1], "313") == 0);
	assert(strcmp(optest[3][0], "echo") == 0);
	assert(strcmp(optest[3][1], "hello") == 0);
	
	for (int j = 0; optest[j] != NULL; j++) {
		printf("Child argv %02d:\n", j);
		for (int k = 0; optest[j][k] != NULL; k++) {
			printf("Child argv index %02d:\n", k);
			printf("String: %s\n", optest[j][k]);
		}
	}

	for (int j = 0; optest[j] != NULL; j++) {
		printf("Freeing pointer address: %p\n", optest[j]);
		FREE_ARGV(optest[j]);
	}
	free(optest);
	optest = NULL;

	/* Mimic a stdin stream. */
	// Done this way to chop off NULL-terminator.
	char *s2 = "echo hello world | grep hello";
	char buf[BUFSZ];
	memcpy(buf, s2, strlen(s2));	// no `+ 1` for no NULL-terminator
	// Append newline, like we would have for a stdin stream.
	buf[strlen(s2)] = '\n';
	printf("TEST_ARGV: buf string: %s\n", buf);

	char **int_argv = process_argv(buf);
	printf("Processed int_argv2:\n");
	i = 0;
	while (int_argv[i] != NULL) {
		printf("int_arg %02d: %s\n", i, int_argv[i]);
		i++;
	}

	size = 0;
	char ***optest2 = process_op(int_argv, "|", &size);
	assert(optest2 != NULL);
	printf("Exited process_op.\n");

	assert(size == 2);
	assert(strcmp(optest2[0][0], "echo") == 0);
	assert(strcmp(optest2[0][1], "hello") == 0);
	assert(strcmp(optest2[0][2], "world") == 0);
	assert(strcmp(optest2[1][0], "grep") == 0);
	assert(strcmp(optest2[1][1], "hello") == 0);
	
	for (int j = 0; optest2[j] != NULL; j++) {
		printf("Child argv %02d:\n", j);
		for (int k = 0; optest2[j][k] != NULL; k++) {
			printf("Child argv index %02d:\n", k);
			printf("String: %s\n", optest2[j][k]);
		}
	}

	for (int j = 0; optest2[j] != NULL; j++) {
		printf("Freeing pointer address: %p\n", optest2[j]);
		FREE_ARGV(optest2[j]);
	}
	free(optest2);
	optest2 = NULL;

	FREE_ARGV(int_argv);
	FREE_ARGV(int_argv2);
	
	return 0;
}

void TEST_SHBufBasic()
{
	TEST_StartTest("SHBufBasic");

	/* Mimic a stdin stream. */
	// Done this way to chop off NULL-terminator.
	char *s1 = "ls    ps";
	char cmd1[BUFSZ];
	memcpy(cmd1, s1, strlen(s1));	// no `+ 1` for no NULL-terminator
	// Append newline, like we would have for a stdin stream.
	cmd1[strlen(s1)] = '\n';
	printf("TEST_SHBufBasic1: cmd1 string: %s\n", cmd1);

	char *s2 = "ls";
	char cmd2[BUFSZ];
	memcpy(cmd2, s2, strlen(s2));	// no `+ 1` for no NULL-terminator
	// Append newline, like we would have for a stdin stream.
	cmd2[strlen(s2)] = '\n';
	printf("TEST_SHBufBasic2: cmd2 string: %s\n", cmd2);

	struct SH_Buf *shbuf1 = SH_BufCreate(cmd1, BUFSZ);
	assert(shbuf1 != NULL);

	for (char *i = SH_BufFirst(shbuf1);
	    !SH_BufDone(shbuf1);
	    i = SH_BufNext(shbuf1)) {
		assert(i != NULL);
		printf("TEST_SHBufBasic1: Successful iteration loop.\n");
		printf("TEST_SHBufBasic1: Buffer line: %s\n", i);
	}

	// xxx pass an address? But that makes the interface messy.
	SH_BufDestroy(&shbuf1);
	assert(shbuf1 == NULL);


	struct SH_Buf *shbuf2 = SH_BufCreate(cmd2, BUFSZ);
	assert(shbuf2 != NULL);

	for (char *i = SH_BufFirst(shbuf2);
	    !SH_BufDone(shbuf2);
	    i = SH_BufNext(shbuf2)) {
		assert(i != NULL);
		printf("TEST_SHBufBasic2: Successful iteration loop.\n");
		printf("TEST_SHBufBasic2: Buffer line: %s\n", i);
	}

	SH_BufDestroy(&shbuf2);
	assert(shbuf2 == NULL);
}

int TEST_SHBuf()
{
	TEST_StartTest("SHBuf");

	/* Test with newlines. */
	/* Mimic a stdin stream. */
	// Done this way to chop off NULL-terminator.
	char *s = "ls\necho Hello World\nls -l -a\nps\ncat Makefile | "
	    "wc -l -w\nls foo";
	char buf[BUFSZ];
	memcpy(buf, s, strlen(s));	// no `+ 1` for no NULL-terminator
	// Append newline, like we would have for a stdin stream.
	buf[strlen(s)] = '\n';
	printf("TEST_SHBuf: buf string: %s\n", buf);

	struct SH_Buf *shbuf = SH_BufCreate(buf, BUFSZ);
	assert(shbuf != NULL);

	for (char *i = SH_BufFirst(shbuf);
	    !SH_BufDone(shbuf);
	    i = SH_BufNext(shbuf)) {
		assert(i != NULL);
		printf("TEST_SHBuf: Successful iteration loop.\n");
		printf("TEST_SHBuf: Buffer line: %s\n", i);
	}

	// xxx pass an address? But that makes the interface messy.
	SH_BufDestroy(&shbuf);
	assert(shbuf == NULL);

	/* Test with NO newlines, except at end. */
	/* Mimic a stdin stream. */
	// Done this way to chop off NULL-terminator.
	char *s2 = "ls echo Hello World ls -l -a ps cat Makefile | "
	    "wc -l -w ls foo";
	char buf2[BUFSZ];
	memcpy(buf2, s2, strlen(s2));	// no `+ 1` for no NULL-terminator
	// Append newline, like we would have for a stdin stream.
	buf2[strlen(s2)] = '\n';
	printf("TEST_SHBuf: buf2 string: %s\n", buf2);

	struct SH_Buf *shbuf2 = SH_BufCreate(buf2, BUFSZ);
	assert(shbuf2 != NULL);

	for (char *i = SH_BufFirst(shbuf2);
	    !SH_BufDone(shbuf2);
	    i = SH_BufNext(shbuf2)) {
		assert(i != NULL);
		printf("TEST_SHBuf: Successful iteration loop.\n");
		printf("TEST_SHBuf: Buffer line: %s\n", i);
	}

	SH_BufDestroy(&shbuf2);
	assert(shbuf2 == NULL);
}

int TEST_SetPrompt()
{
	char *p_no_yes = "MyPromptNoSpaceEnd ";
	char *p_yes = "MyPromptYesSpaceEnd ";


	char *argv_no[] = { "main", "MyPromptNoSpaceEnd", NULL };
	char *argv_yes[] = {  "main", "MyPromptYesSpaceEnd ", NULL };

	char *prompt_no = SH_SetPrompt(argv_no);
	char *prompt_yes = SH_SetPrompt(argv_yes);

	printf("TEST_SetPrompt: Prompt no space-end: \'%s\'\n", prompt_no);
	printf("TEST_SetPrompt: Prompt yes space-end: \'%s\'\n", prompt_yes);

	assert(strcmp(prompt_no, p_no_yes) == 0);
	assert(strcmp(prompt_yes, p_yes) == 0);

	char **argv_null = NULL;
	char *prompt = SH_SetPrompt(argv_null);
	char *p = "> ";
	assert(strcmp(prompt, p) == 0);
	printf("TEST_SetPrompt: Prompt default: \'%s\'\n", prompt);

	free(prompt_yes);
	free(prompt_no);
	free(prompt);

	return 0;
}

void TEST_TwoCommands()
{
	TEST_StartTest("TwoCommands");

	/* Mimic a stdin stream. */
	// Done this way to chop off NULL-terminator.
	char *s1 = "ls    ps";
	char cmd1[BUFSZ];
	memcpy(cmd1, s1, strlen(s1));	// no `+ 1` for no NULL-terminator
	// Append newline, like we would have for a stdin stream.
	cmd1[strlen(s1)] = '\n';
	printf("TEST_TwoCommands: cmd1 string: %s\n", cmd1);

	char *s2 = "ls";
	char cmd2[BUFSZ];
	memcpy(cmd2, s2, strlen(s2));	// no `+ 1` for no NULL-terminator
	// Append newline, like we would have for a stdin stream.
	cmd2[strlen(s2)] = '\n';
	printf("TEST_TwoCommands: cmd2 string: %s\n", cmd2);

	char **int_argv1 = process_argv(cmd1);
	printf("Processed int_argv1:\n");
	int i = 0;
	while (int_argv1[i] != NULL) {
		printf("int_argv1[%d]:\t%s\n", i, int_argv1[i]);
		i++;
	}

	char **int_argv2 = process_argv(cmd2);
	printf("Processed int_argv2:\n");
	i = 0;
	while (int_argv2[i] != NULL) {
		printf("int_argv2[%d]:\t%s\n", i, int_argv2[i]);
		i++;
	}
	
	char **ext_argv1 = SH_ARGV(cmd1);
	printf("Processed ext_argv1:\n");
	i = 0;
	while (ext_argv1[i] != NULL) {
		printf("ext_argv1[%d]:\t%s\n", i, ext_argv1[i]);
		i++;
	}

	char **ext_argv2 = SH_ARGV(cmd2);
	printf("Processed ext_argv2:\n");
	i = 0;
	while (ext_argv2[i] != NULL) {
		printf("ext_argv2[%d]:\t%s\n", i, ext_argv2[i]);
		i++;
	}

	SH_Exec(ext_argv1);
	SH_Exec(ext_argv2);
}

// vim: filetype=c:noexpandtab:shiftwidth=8:tabstop=8:softtabstop=8:textwidth=80
