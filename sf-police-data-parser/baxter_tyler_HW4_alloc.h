/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_alloc.h
*
* Description:: Allocators and deallocators interface.
*
**************************************************************/

#ifndef ALLOC_H
#define ALLOC_H

#include "baxter_tyler_HW4_core.h"

/**
 * For destroying temporary Records and not breaking the existing API.
 * @todo Should be refactored into a better main destroy_records routine.
 */
int destroy_tmp_records(Record **records);

/**
 * Creates an OutputSection for Output.
 */
OutputSection *create_output_section(int size);

/**
 * Creates the final Output struct.
 */
Output *create_output(int size);

/**
 * Destroys the final Output struct.
 */
void destroy_output(Output *output);

/**
 * Returns an array of ProcessEntries.
 */
ProcessEntry **create_process_entries();

/**
 * Construct and return a vector of PostEntry objects.
 */
PostEntry **create_post_entries();

/**
 * Destroy a vector of ProcessEntry objects.
 */
int destroy_process_entries(ProcessEntry ***entries);

/**
 * Destroy a vector of PostEntry objects.
 */
int destroy_post_entries(PostEntry ***entries);

/**
 * Allocate a vector of lines split by newline.
 */
char **alloc_lines();

/**
 * Free an allocated vector of newline-split lines.
 */
int free_lines(char ***lines);

/**
 * Constructor for a Record.
 *
 * @return Record *			A pointer to the created Record.
 *
 * @param Field **fields	A Field vector.
 */
Record *create_record(Field **fields);

/**
 * Destructor for a Record.
 *
 * @param Record **record	The address of the Record pointer to destroy.
 */
int destroy_record(Record **record);

/**
 * Creates a vector of Records.
 */
Record **create_records(int begin, int end);

/**
 * Destroys a vector of Records.
 */
int destroy_records(Record ***records);

/**
 * Make a heap allocated copy of a Record.
 */
Record *copy_record(Record *record, Field **fields);

/**
 * Returns a vector of Fields parsed from `headers.txt`.
 *
 * Ownership model: Caller must call destroy_fields to deallocate.
 */
Field **create_fields(const char *fh);

/**
 * Destroy a vector of Field vectors.
 */
int destroy_fields(Field ***fields);

#endif

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
