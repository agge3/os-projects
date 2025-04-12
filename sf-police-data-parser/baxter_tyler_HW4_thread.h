/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_thread.h
*
* Description:: Thread driver code interface. th_main is an aliased main entry
* point for multithreading.
*
**************************************************************/

#ifndef THREAD_H
#define THREAD_H

#include "baxter_tyler_HW4_core.h"
#include "baxter_tyler_HW4_alloc.h"
#include "baxter_tyler_HW4_sort.h"
#include "baxter_tyler_HW4_output.h"

#include <pthread.h>
#include <stdlib.h>

/**
 * @struct ThreadPart
 * For a thread to know how to partition its jobs.
 */
typedef struct ThreadPart {
	int sz;
	int begin;
	int end;
	int recnum;
	int recsz;
} ThreadPart;

/**
 * @struct ThreadContext
 * Context struct for passing around threads.
 */
typedef struct ThreadContext {
	pthread_mutex_t *mutex;
	Field **fields;
	int fd;
	char *subfield;
	char *arg1;
	char *arg2;
} ThreadContext;

/**
 * @struct ThreadData
 * A struct for collapsing all ThreadData into a single `void *` for
 * pthread_create(3).
 */
typedef struct ThreadData {
	Output *output;
	pthread_mutex_t *mutex;
	Field **fields;
	ThreadPart *part;
	int fd;
	char *subfield;
	char *arg1;
	char *arg2;
} ThreadData;

/**
 * Creates a vector of ThreadParts for each thread to know how to partition its
 * jobs.
 */
ThreadPart **create_thparts(int fhsz, int thnum, int recsz);

/**
 * Destroys a vector of ThreadParts.
 *
 * @param ThreadPart ***parts	A vector of ThreadParts.
 * @param int thnum				The number of threads that were partitioned.
 */
int destroy_thparts(ThreadPart ***parts, int thnum);

/**
 * A thread job for pthread_create(3).
 */
void *th_job(void *data);

/**
 * Wrapper for pthread_create(3) that handles our DSL preprocessing.
 */
pthread_t th_create(Output *output, pthread_mutex_t *mutex, Field **fields,
	ThreadPart *part, int fd, char *subfield, char *arg1, char *arg2);

/**
 * Main routine for multithreading. Parses the record database, sorts the
 * records, and returns the final Output.
 *
 * @return Output *output	The final Output.
 */
Output *th_main(Field **fields, char *fh, int num_threads, char *subfield,
	char *arg1, char *arg2);

#endif // THREAD_H

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
