/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 1 – Command Line Arguments
*
* File:: baxter_tyler_HW1_main.c
*
* Description:: Prints the number of CLI arguments and each argument string.
*
**************************************************************/

#include <stdio.h>

int main(int argc, char **argv)
{
	/* argc is the number of arguments. */
	printf("There were %d arguments on the command line.\n", argc);

	/* argv is the array of argument strings. */
	/* Since we're printing ALL, we'll use a for loop. */
	for (int i = 0; i < argc; ++i) {
		printf("Argument 0%d:\t%s\n", i, argv[i]);
	}

	return 0;
}

/* vim: filetype=c:noexpandtab:shiftwidth=8:tabstop=8:softtabstop=8:textwidth=80 */
