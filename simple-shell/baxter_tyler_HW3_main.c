/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 3 – Simple Shell with Pipes
*
* File:: baxter_tyler_H3_main.c
*
* Description:: Simple Shell. Supports both fully qualified paths and $PATH
* environment variable for executables. Supports pipes and multiple
* newline-separated commands. May have a user-specified prompt as the first
* argument. Try it like bash and see what Simple Shell supports! `exit` or EOF
* to quit. Purposely is built like a library to be a platform for Advanced 
* Shell.
*
**************************************************************/

#include "baxter_tyler_HW3_global.h"
#include "baxter_tyler_HW3_shell.h"
#include "baxter_tyler_HW3_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TEST
int main(int argc, char **argv)
{
	//TEST_Allocators();
	//TEST_ARGV();
	//TEST_ProcessOp();
	//TEST_SHBuf();
	//TEST_SetPrompt();
	//TEST_TwoCommands();
	TEST_SHBufBasic();
}
#else
int main(int argc, char **argv)
{
	char *prompt = SH_SetPrompt(argv);

	char *buf = malloc(BUFSZ * sizeof(char));

	while (RUNNING) {
		// Draw prompt and hang until we have stdin.
		ssize_t ret = SH_Prompt(prompt, buf);
		if (ret == -1) {
			SH_Print("Error: Read error.\n");
			continue;
		} else if (ret == 0) {
			// Reached end of file.
			goto cleanup;
		}

		// Iterate for each newline in the command.
		struct SH_Buf *shbuf = SH_BufCreate(buf, BUFSZ);
		for (char *i = SH_BufFirst(shbuf);
		    !SH_BufDone(shbuf);
		    i = SH_BufNext(shbuf)) {

			char **shargv = SH_ARGV(i);

#ifdef DEBUG
			printf("main:\tSH_Buf iterator:\t%s\n", i);
			int idx = 0;
			while (shargv[idx] != NULL) {
				printf("main:\tshargv[%d]:\t%s\n", idx, shargv[idx]);
				idx++;
			}
#endif


			if (shargv == NULL) {
				SH_BufDestroy(&shbuf);
				memset(buf, 0, BUFSZ);
				FREE_ARGV(shargv);
				continue;
			}

			if (SH_Exit(shargv) != 0) {
				SH_BufDestroy(&shbuf);
				memset(buf, 0, BUFSZ);
				FREE_ARGV(shargv);
				goto cleanup;
			}

			if (SH_Exec(shargv) != 0) {
				SH_BufDestroy(&shbuf);
				memset(buf, 0, BUFSZ);
				FREE_ARGV(shargv);
				goto cleanup;
			}

			FREE_ARGV(shargv);
		}
#ifdef DEBUG
		printf("main:\tSH_Buf iterator:\tExited\n");
#endif
		SH_BufDestroy(&shbuf);
		memset(buf, 0, BUFSZ);
	}

cleanup:
	#ifdef DEBUG
	printf("Reached cleanup\n");
	#endif

	free(prompt);
	free(buf);
	prompt = NULL;

	return 0;
}
#endif

// vim: filetype=c:noexpandtab:shiftwidth=8:tabstop=8:softtabstop=8:textwidth=80
