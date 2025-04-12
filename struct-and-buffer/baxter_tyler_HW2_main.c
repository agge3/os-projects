/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 2 – Stucture in Memory and Buffering
*
* File:: baxter_tyler_H2_main.c
*
* Description:: This is a complicated and non-UNIX program. It does multiple
* disconnected things. The first function of this program is to populate my
* personal information for my instructor to parse and log notable information 
* about me. That function can be expanded to other students via arguments, but 
* as some of the information is hard-coded, reusability is questionable. The 
* second function of this program is to copy a string stream into 256 byte 
* blocks, presumably to write to 256 byte memory pages.
*
**************************************************************/

#include "assignment2.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	if (argc < 4) {
		fprintf(stdout,
		    "Usage: %s [FIRST_NAME] [LAST_NAME] [MESSAGE]\n", argv[0]);
		exit(1);
	}

	/* Message buffer's size; don't buffer overflow. */
	const size_t BUFFER_SIZE = 100;

	/* Instantiate personal info. */
	struct personalInfo *pi = malloc(sizeof(struct personalInfo));

	pi->firstName = malloc(strlen(argv[1]) + 1);
	strcpy(pi->firstName, argv[1]);

	pi->lastName = malloc(strlen(argv[2]) + 1);
	strcpy(pi->lastName, argv[2]);

	pi->studentID = 0;
	pi->level = JUNIOR;
	pi->languages = KNOWLEDGE_OF_C | KNOWLEDGE_OF_JAVA |
	    KNOWLEDGE_OF_JAVASCRIPT | KNOWLEDGE_OF_PYTHON |
	    KNOWLEDGE_OF_CPLUSPLUS | KNOWLEDGE_OF_LISP | KNOWLEDGE_OF_SQL |
	    KNOWLEDGE_OF_HTML | KNOWLEDGE_OF_INTEL_ASSEMBLER |
	    KNOWLEDGE_OF_MIPS_ASSEMBLER;

	/* Copy remaining argument into message buffer. */
	size_t end = strlen(argv[3]);
	if (end < BUFFER_SIZE) {
	    memcpy(pi->message, argv[3], end);
	/* xxx seems prone to errors to proceed anyway */
	#ifdef DEBUG
	} else {
		fprintf(stderr, "Argument three overflows buffer. "
		    "Unable to continue.");
		exit(1);
	}
	#else
	} else {
	    memcpy(pi->message, argv[3], BUFFER_SIZE);
	}
	#endif

	/* Write personal information and confirm success before continuing. */
	if (writePersonalInfo(pi) != 0) {
		fprintf(stderr, "Writing personal information failed. "
		    "Unable to continue.");
		exit(1);
	}

	/* Copy iterator strings into a block and commit the block when full.
	 * Any remaining characters of a string will be copied into the next
	 * block. Continue until there are no more iterator strings left. */
	char *buf = malloc(BLOCK_SIZE);	
	const char *s;
	size_t idx = 0;
	while (s = getNext()) {
		size_t len = strlen(s);
		if (len <= BLOCK_SIZE - idx) {
			memcpy(buf + idx, s, len);
			idx += len;
		} else {
			size_t remaining = BLOCK_SIZE - idx;
			memcpy(buf + idx, s, remaining);

			commitBlock(buf);
			idx = 0;

			memcpy(buf + idx, s + remaining, len - remaining);
			idx += len - remaining;
		}
	}

	/* After exhausting the `getNext()` iterator, commit the last block
	 * (even if it's half-full). */
	if (idx > 0) {
		commitBlock(buf);
	}

	/* Cleanup: */
	free(buf);
	buf = NULL;
	free(pi->firstName);
	pi->firstName = NULL;
	free(pi->lastName);
	pi->lastName = NULL;
	free(pi);
	pi = NULL;

	return checkIt();
}

// vim: filetype=c:noexpandtab:shiftwidth=8:tabstop=8:softtabstop=8:textwidth=80
