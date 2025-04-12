/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 3 – Simple Shell with Pipes
*
* File:: baxter_tyler_H3_shell.c
*
* Description:: Simple Shell public API implementation.
*
**************************************************************/

#include "baxter_tyler_HW3_shell.h"
#include "baxter_tyler_HW3_int_shell.h"
#include "baxter_tyler_HW3_global.h"

#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

// Thanks for the initial iterator blueprint.
// CREDIT: https://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Iterators.html
struct SH_Buf {
	char **bufs;
	size_t sz;
	size_t pos;
};

struct SH_Buf *SH_BufCreate(char *buf, size_t sz)
{
	if (buf == NULL) {
		return NULL;
	}

	struct SH_Buf *shbuf;
	shbuf = malloc(sizeof(*shbuf));
	if (shbuf == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	// xxx can be less than the buffer size, but OK for now.
	shbuf->sz = sz;
	shbuf->pos = 0;
	shbuf->bufs = malloc(shbuf->sz * sizeof(char *));
	if (shbuf->bufs == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	// Preprocess to construct the iterator elements.
	size_t bufidx = 0;
	size_t bidx = 0;
	size_t aidx = 0;
	while (aidx < sz) {
		#ifdef DEBUG
		if (buf[aidx] == '\n') {
			printf(
			    "SH_BufCreate: Found newline.\n"
			);
		} else if (buf[aidx] == '\0') {
			printf(
			    "SH_BufCreate: Found NULL-terminator.\n"
			);
		}
		#endif

		if (buf[aidx] == '\n') {
			#ifdef DEBUG
			if (buf[aidx] == '\n') {
				printf(
				    "SH_BufCreate: Found newline.\n"
				);
			} else if (buf[aidx] == '\0') {
				printf(
				    "SH_BufCreate: Found NULL-terminator.\n"
				);
			}
			#endif

			char *cbuf = malloc((aidx - bidx + 1) * sizeof(char));
			if (cbuf == NULL) {
				err(EXIT_FAILURE, "malloc");
			}

			memcpy(cbuf, &buf[bidx], aidx - bidx);

			// WARNING: If we newline-terminate we buffer overflow
			// in our test cases; if we NULL-terminate we buffer
			// overflow in our main driver code. What we should do
			// is newline-terminate then NULL-terminate, but main
			// driver code is prioritized for now.
			cbuf[aidx - bidx] = '\n';

			shbuf->bufs[bufidx++] = cbuf;

			bidx = aidx + 1;
		}
		aidx++;
	}

	// There may be a hanging command, if buffer did not end with a newline.
	if (bidx < aidx) {
		char *cbuf = malloc((aidx - bidx + 1) * sizeof(char));
		if (cbuf == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		memcpy(cbuf, &buf[bidx], aidx - bidx);

		// WARNING: If we newline-terminate we buffer overflow
		// in our test cases; if we NULL-terminate we buffer
		// overflow in our main driver code. What we should do
		// it newline-terminate then NULL-terminate, but main
		// driver code is prioritized for now.
		cbuf[aidx - bidx] = '\n';

		shbuf->bufs[bufidx++] = cbuf;
	}

	shbuf->bufs[bufidx] = NULL;
	return shbuf;
}

void SH_BufDestroy(struct SH_Buf **shbuf)
{
#ifdef DEBUG
	printf("SH_BufDestroy:\tEntered\n");
#endif

	// Avoid freeing an already NULL pointer.
	if (shbuf != NULL || *shbuf != NULL) {
		for (size_t i = 0; (*shbuf)->bufs[i] != NULL; i++) {
#ifdef DEBUG
			printf("SH_BufDestroy:\tfree:\t%s\n", (*shbuf)->bufs[i]);
#endif
			free((*shbuf)->bufs[i]);
			(*shbuf)->bufs[i] = NULL;
		}
		free((*shbuf)->bufs);
		(*shbuf)->bufs = NULL;
		free(*shbuf);
		*shbuf = NULL;

#ifdef DEBUG
		printf("SH_BufDestroy:\tDestroyed\n");
#endif
	}

}

char *SH_BufFirst(struct SH_Buf *shbuf)
{
	shbuf->pos = 0;
	return shbuf->bufs[shbuf->pos++];
}

int SH_BufDone(struct SH_Buf *shbuf)
{
	return shbuf->bufs[shbuf->pos] == NULL;
}

char *SH_BufNext(struct SH_Buf *shbuf)
{
	return shbuf->bufs[shbuf->pos++];
}

// Internal state container for parsing comments.
void parse_comment(bool *comment, char *ch)
{
	if (*comment == false && *ch == '#') {
		*comment = true;
	}
	// nop
}

// Sets the prompt for Simple Shell.
char *SH_SetPrompt(char **argv)
{
	#ifdef SIMPSH
	// Check for argument vector NULL, string NULL, and empty string.
	// Those are all cases where we don't want to proceed.
	if (argv != NULL && argv[1] != NULL && strlen(argv[1]) > 0) {
		// If there's not a space character after the prompt, then
		// append it to keep a uniform prompt style.
		size_t len = strlen(argv[1]);	
		if (argv[1][len - 1] != ' ') {
			char *prompt = malloc(len + 2);
			if (prompt == NULL) {
				err(EXIT_FAILURE, "malloc");
			}
			memcpy(prompt, argv[1], len);
			prompt[len] = ' ';
			prompt[len + 1] = '\0';
			return prompt;
		}

		char *prompt = malloc(len + 1);
		if (prompt == NULL) {
			err(EXIT_FAILURE, "malloc");
		}
		strcpy(prompt, argv[1]);
		return prompt;
	}

	const char *p = "> ";
	char *prompt = malloc(strlen(p) + 1);
	if (prompt == NULL) {
		err(EXIT_FAILURE, "malloc");
	}
	strcpy(prompt, p);

	#else
	char *rc = NULL;	// xxx look for a .rc file in path
	char *promptrc = "";	// xxx look for prompt in .rc file.
	char *prompt;
	const char *DFILE = "";
	if (rc != NULL && strcmp(promptrc, "") != 0) {
		// `+ 2` because we want a space separator.
		prompt = malloc(strlen(promptrc) + 2);
		strcpy(prompt, promptrc);
		strcat(prompt, " ");	// space separator
	} else {
		// xxx privileged vs. unprivileged?
		char *p = "$ ";
		prompt = malloac(strlen(p + 1));
		strcpy(prompt, p);
	}

	#ifdef DEBUG
	if (DFILE) {
		fprintf(DFILE, "Prompt: %s\n", prompt);
	}
	#endif
	#endif

	return prompt;
}

/**
 * Returns an argument vector from a character buffer. Heap allocates so argv 
 * can be shared with children processes.
 *
 * @pre		Expects space-separated arguments.
 * @post	Caller's responsibility to free with FREE_ARGV.
 */
char **process_argv(char *buf)
{
	if (buf == NULL) {
		return NULL;	// do nothing
	}

	// (n - 1) / 2; n = BUFSZ, 1 = NULL at end of argv, 2 = 1 string and 1
	// space for space-separated.
	size_t bufsz = (BUFSZ - 1) / 2;

	char **argv;
	ALLOC_ARGV(argv);
	
	char arg[BUFSZ];
	size_t idx = 0;
	size_t argv_idx = 0;
	size_t arg_idx = 0;

	// Parse the buffer one byte at a time.
	//bool comment = false;	// parse for comments for shell scripts.
	while (idx < BUFSZ) {
		#ifdef DEBUG
		printf("IDX: %d\n", buf[idx]);
		#endif

		//parse_comment(&comment, &buf[idx]);

		// Add argument to argv, otherwise parse for next byte.
		if (buf[idx] == ' ' || buf[idx] == '\n' || buf[idx] == EOF ||
		    buf[idx] == '\0') {
			// Store the last argument, if there is one.
			if (arg_idx > 0) {
				arg[arg_idx++] = '\0';
				argv[argv_idx] = malloc((arg_idx) * sizeof(char));
				if (argv[argv_idx] == NULL) {
					FREE_ARGV(argv);
					perror("malloc failed for arguments");
				}
				memcpy(argv[argv_idx++], arg, arg_idx);

				#ifdef DEBUG
				printf("Add argument path entered.\n");
				printf("XXX: %s\n", argv[argv_idx - 1]);
				#endif

				arg_idx = 0;
			}
			// EOL
			if (buf[idx] == '\n') {
				break;
			}
			//if (comment) {
			//	break;
			//}

			idx++;
		// Main execution path: parse argument.
		} else {
			arg[arg_idx++] = buf[idx++];
		}
	}

	// We have no more arguments, assign `last element + 1` NULL to 
	// terminate argv.
	argv[argv_idx] = NULL;

	#ifdef DEBUG
	printf("Processed argv:\n");
	int i = 0;
	while (argv[i] != NULL) {
		printf("arg%02d: %s\n", i, argv[i]);
		i++;
	}
	#endif

	return argv;
}

// xxx to be implemented with path
int check_exec(char *exec)
{
	// Error check to confirm with stat.
	struct stat sb;
	if (stat(exec, &sb) == -1) {
		switch (errno) {
		case EACCES:
			fprintf(stdout, "User permissions cannot access path.");
			return -1;
		case EBADF:
			fprintf(stdout, "Invalid executable file descriptor.");
			return -1;
		default:
			perror("Failed stat call");
			return -1;
		}
	}

	if (!(sb.st_mode & S_IXUSR) && !(sb.st_mode & S_IXGRP) && 
	    !(sb.st_mode & S_IXOTH)) {
		fprintf(stdout, "Not executable.");
		return -1;
	}

	return 0;
}

/*
 * Child pipe helper.
 */
int cpipe(int pipefd[2])
{
	// Close unused write end.
	if (close(pipefd[PIPE_WRITE]) == -1) {
		err(EXIT_FAILURE, "close");
	}

	if (dup2(pipefd[PIPE_READ], STDIN_FILENO) == -1) {
		err(EXIT_FAILURE, "dup2");
	}

	#ifdef DEBUG
	char buf[256];
	//while (fgets(buf, sizeof(buf), stdin) != NULL) {
	//	printf("Child pipe stdin: %s", buf);
	//}
	//printf("\n");
	#endif

	if (close(pipefd[PIPE_READ]) == -1) {
		err(EXIT_FAILURE, "close");
	}

	return 0;
}

/**
 * Parent pipe helper.
 */
int ppipe(int pipefd[2])
{
	// Close unused read end.
	if (close(pipefd[PIPE_READ]) == -1) {
		err(EXIT_FAILURE, "close");
	}

	#ifdef DEBUG
	char buf[256];
	//while (fgets(buf, sizeof(buf), stdout) != NULL) {
	//	printf("Parent pipe stdout: %s", buf);
	//}
	//printf("\n");
	#endif

	if (dup2(pipefd[PIPE_WRITE], STDOUT_FILENO) == -1) {
		err(EXIT_FAILURE, "dup2");
	}

	if (close(pipefd[PIPE_WRITE]) == -1) {
		err(EXIT_FAILURE, "close");
	}

	return 0;
}

/**
 * Recursive Simple Shell pipe execution helper. Recurse down the entire pipe 
 * tree, stringing together commands until the last pipe.
 *
 * @param char ***cargvs Vector of argument vectors. Call with `+ 1` until NULL
 * to recurse.
 *
 * @param pstate File descriptor for the previous pipe's read end, or -1 if 
 * none.
 */
int shrpexec(char ***cargvs, int pstate) {
	// Base case: No more children argument vectors.
	if (*cargvs == NULL) {
		return 0;
	}
	
	int pipefd[2];

	// There's another command.
	int has_next = (*(cargvs + 1) != NULL);

	// If there's another command, create a new pipe.
	if (has_next) {
		if (pipe(pipefd) == -1) {
			err(EXIT_FAILURE, "pipe");
		}
	}
	
	pid_t cpid = fork();
	if (cpid == -1) {
	    err(EXIT_FAILURE, "fork");
	}
	
	if (cpid == 0) {
		// If we recieved a previous pipe state, duplicate it onto STDIN.
		if (pstate != -1) {
			if (dup2(pstate, STDIN_FILENO) == -1) {
				err(EXIT_FAILURE, "dup2");
			}
			if (close(pstate) == -1) {
				err(EXIT_FAILURE, "close");
			}
		}

		// If there's another command, duplicate STDOUT onto our pipe.
		if (has_next) {
			// Behave like parent pipe.
			ppipe(pipefd);
		}

		int ret = execvp((*cargvs)[0], *cargvs);
		if (ret == -1) {
			err(EXIT_FAILURE, "execvp");
		}
	} else {  
		// If there's a previous pipe state, close the pipe's read end.
		if (pstate != -1) {
			if (close(pstate) == -1) {
				err(EXIT_FAILURE, "close");
			}
		}

		// If there's another command, pass the read end recursively as
		// previous pipe state.
		if (has_next) {
			if (close(pipefd[PIPE_WRITE]) == -1) {
				err(EXIT_FAILURE, "close");
			}
			if (shrpexec(cargvs + 1, pipefd[PIPE_READ]) != 0) {
				err(EXIT_FAILURE, "shrpexec");
			}
		}

		int wstatus;
		int ret = wait(&wstatus);
		if (ret == -1) {
			err(EXIT_FAILURE, "wait");
		}

		printf("Child %d, exited with %d\n", ret, WEXITSTATUS(wstatus));
	}
	
	return 0;
}

/**
 * Wrapper for the recursive pipe execution path, to allow an ergonomic internal
 * interface.
 */
int shpexec(char ***cargvs)
{
	if (shrpexec(cargvs, -1) != 0) {
		err(EXIT_FAILURE, "shrpexec");
	}
	return 0;
}

/**
 * Main internal execution path; for a simple command with no pipes.
 */
int shexec(char **argv) {
	pid_t cpid = fork();
	if (cpid == -1) {
		err(EXIT_FAILURE, "fork");
	}

	// In child process.
	if (cpid == 0) {
		#ifdef DEBUG
		printf("Child path entered\n");
		#endif

		int ret = execvp(argv[0], argv);
		if (ret == -1) {
			err(EXIT_FAILURE, "execvp");
		}
	// In parent process.
	} else {
		#ifdef DEBUG
		printf("Parent path entered\n");
		#endif

		int wstatus;
		int ret = wait(&wstatus);
		if (ret == -1) {
			err(EXIT_FAILURE, "wait");
		}

		printf("Child %d, exited with %d\n", ret, WEXITSTATUS(wstatus));
	}

	return 0;
}

/*
 * Process any reserved operators in an argument vector, splitting into children
 * argument vectors.
 *
 * @remark Designed generically to allow enforcing increasing operator 
 * precedence and increasingly nested argument vectors.
 *
 * @param char **shargv		The argument vector to be processed.
 * @param const char *op	The operator to split by.
 * @param size_t *size		Optional parameter for caller to track size.
 *
 * @return char***		A vector containing the children argument
 *				vectors, or NULL if no operators were found.
 *
 */
char ***process_op(char **shargv, const char *op, size_t *size)
{
	// We're going to have two separate trees in this function: One that
	// stays on the stack and one that allocates on the heap. Each tree is
	// mutually indepedent: Once we've start heap allocating, we'll continue
	// to heap allocate; if we've never entered the heap allocation tree,
	// then we stay on the stack.

	if (shargv == NULL || op == NULL) {
		return NULL;	// do nothing
	}

	// Preprocess first and construct a stack operator table. 
	// If no operator table is constructed, then we'll save allocations and 
	// simply return NULL.
	char **optbl[BUFSZ];
	char *cargv[BUFSZ];

	size_t shidx = 0;
	size_t cidx = 0;
	size_t oidx = 0;
	while (shargv[shidx] != NULL) {
		// Copy each string pointer head into our stack child argv.
		cargv[cidx++] = shargv[shidx++];

		// If an operator is found, then copy our current child argv 
		// into our operator table and start a new child argv.
		if (shargv[shidx] != NULL && strcmp(shargv[shidx], op) == 0) {
			cargv[cidx] = NULL;
			#ifdef DEBUG
			int j = 0;
			while (cargv[j] != NULL) {
				printf("process_op: Child argv string: %s\n",
				    cargv[j]);
				j++;
			}

			printf("process_op: Adding argv:\n");	
			#endif

			char **argv;
			ALLOC_ARGV(argv);
			size_t arg_idx = 0;
			for (size_t i = 0; cargv[i] != NULL; i++) {
				if (strcmp(cargv[i], op) != 0) {
					argv[arg_idx++] = strdup(cargv[i]);

					#ifdef DEBUG
					printf("Stack argument %s\n", cargv[i]);
					printf("Heap argument %s\n",
					    argv[arg_idx - 1]);
					#endif
				}
			}

			#ifdef DEBUG
			printf("process_op: End add argv.\n");
			#endif

			optbl[oidx++] = argv;

			cidx = 0;
		}
	}
	// Add hanging child argv (if there is one).
	if (cidx != shidx && cidx > 0) {
		cargv[cidx] = NULL;
		#ifdef DEBUG
		int k = 0;
		while (cargv[k] != NULL) {
			printf("process_op: Child argv string: %s\n",
			    cargv[k]);
			k++;
		}

		printf("process_op: Adding argv:\n");
		#endif

		char **argv;
		ALLOC_ARGV(argv);
		size_t arg_idx2 = 0;
		for (size_t i = 0; cargv[i] != NULL; i++) {
			if (strcmp(cargv[i], op) != 0) {
				argv[arg_idx2++] = strdup(cargv[i]);

				#ifdef DEBUG
				printf("Stack argument %s\n", cargv[i]);
				printf("Heap argument %s\n",
				    argv[arg_idx2 - 1]);
				#endif
			}
		}

		#ifdef DEBUG
		printf("process_op: End add argv.\n");
		#endif

		optbl[oidx++] = argv;
		optbl[oidx] = NULL;
	}

	// If we have an operator table, then allocate the operator table and 
	// return it, otherwise return NULL for Simple Shell to the use the
	// master argv.
	if (oidx == 0) {
		return NULL;
	}

	char ***tbl = malloc((oidx + 1) * sizeof(char **));	
	if (tbl == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	for (size_t i = 0; i < oidx; i++) {
		tbl[i] = optbl[i];

		#ifdef DEBUG
		printf("process_op: Adding pointer address: %p\n", optbl[i]);
		printf("process_op: Stored pointer address: %p\n", tbl[i]);
		#endif
	}
	tbl[oidx] = NULL;

	#ifdef DEBUG
	int x = 0;
	while (tbl[x] != NULL) {
		printf("process_op: Final table address %02d: %p\n",
		    x, tbl[x]);
		int y = 0;
		while (tbl[x][y] != NULL) {
			printf("Argument %02d: %s\n", y, tbl[x][y]);
			y++;
		}
		x++;
	}
	#endif

	*size = oidx;

	return tbl;
}

int SH_Print(char *msg)
{
	fflush(stdout);
	write(STDOUT_FILENO, msg, strlen(msg));
	return 0;
}

int SH_Exec(char **shargv)
{
	if (shargv == NULL) {
		return 0;	// do nothing
	}

	size_t size = 0;
	int ret = 0;
	char ***cargvs = process_op(shargv, "|", &size);

	if (cargvs == NULL) {
		shexec(shargv);
		return 0;
	}

	ret = shpexec(cargvs);

	#ifdef DEBUG
	printf("SH_Exec: Before cleanup.\n");
	for (int i = 0; cargvs[i] != NULL; i++) {
		printf("SH_Exec: Freeing pointer address: %p\n", cargvs[i]);
		for (int j = 0; cargvs[i][j] != NULL; j++) {
			printf("SH_Exec: Freeing string: %s\n", cargvs[i][j]);
			free(cargvs[i][j]);
		}
		free(cargvs[i]);
	}
	free(cargvs);
	cargvs = NULL;
	printf("SH_Exec: After cleanup.\n");
	#else
	// xxx allocator is double-freeing here.
	//for (int i = 0; cargvs[i] != NULL; i++) {
	//	FREE_ARGV(cargvs);
	//}
	//free(cargvs);
	//cargvs = NULL;
	for (int i = 0; cargvs[i] != NULL; i++) {
		for (int j = 0; cargvs[i][j] != NULL; j++) {
			free(cargvs[i][j]);
		}
		free(cargvs[i]);
	}
	free(cargvs);
	cargvs = NULL;	
	#endif
			
	return ret;
}

/*
 * Draws Simple Shell's prompt and populates the caller's buffer with a 
 * newline-delimited buffer.
 *
 * @remark buf is not cleared. Parse for the delimiter.
 *
 * @return Number of bytes read or EOF.
 */
ssize_t SH_Prompt(char *prompt, char *buf) {

	// Wait for stdin and draw prompt.
	fflush(stdout);
	write(STDOUT_FILENO, prompt, strlen(prompt));

	ssize_t bytes = read(STDIN_FILENO, buf, BUFSZ - 1);
	// Client filled entire buffer and we still need to delimitate. Default
	// delimiter is newline.
	if (bytes + 1 == BUFSZ) {
		buf[BUFSZ] = '\n';
	}
#ifdef DEBUG
	write(STDOUT_FILENO, buf, bytes);
#endif

	return bytes;
}

char **SH_ARGV(char *buf)
{
	char **shargv = process_argv(buf);
	if (shargv[0] == NULL) {
		// Make sure to free the allocation, because the caller only
		// sees NULL here and doesn't assume responsibility.
		FREE_ARGV(shargv);
		return NULL;
	}

	#ifdef DEBUG
	printf("Parsed argv.\n");
	#endif	

	return shargv;
}

char *SH_output() 
{
	return NULL;
}

int SH_Exit(char **shargv)
{
	if (shargv == NULL || shargv[0] == NULL) {
		return 0;	// do nothing
	}

	// Our only supported non-executable argument is `exit`.
	if (strcmp(shargv[0], "exit") == 0) {
		#ifdef DEBUG
		printf("argv[0]: %s\n", shargv[0]);
		printf("Exit path entered\n");
		#endif
		return -1;
	}
	return 0;
}

// vim: filetype=c:noexpandtab:shiftwidth=8:tabstop=8:softtabstop=8:textwidth=80
