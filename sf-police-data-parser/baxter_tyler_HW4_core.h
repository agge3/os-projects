/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_core.h
*
* Description:: Core interface. Provides main data structures and globals.
* Provides record parsing and storing API.
*
**************************************************************/

#ifndef CORE_H
#define CORE_H

#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>
#include <err.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

//#define _XOPEN_SOURCE // See feature_test_macros(7)

//#define DEBUG
//#define TEST

/**
 * Maximum amount of lines for allocator.
 */
#define MAX_LINES 100

#define MAX_LINE 81	// 80 columns terminal width should do.
#define MAX_INDEX 3	// Two digits max, and the NULL-terminator.

/**
 * Maximum amount of fields for allocator.
 */
#define MAX_FIELDS 100

// `header.txt` is 768 bytes, so 4KB should be fine for our read buffer.
#define HEADER_TXT_BUFFER 4096

// Temporary macro for number of Record keys.
#define NUM_KEYS 37

/**
 * Generic buffer size for allocating character buffers.
 */
#define BUFSZ 32

#define TOTAL_TIME 36

/**
 * Amount of post-processed RecordEntries.
 */
#define PROC_RECORDS 4

/**
 * @struct Field
 * Structure for holding Field information from `header.txt`.
 */
typedef struct Field {
	int index;
	char *entry;
} Field;

/**
 * @enum RecordValueType
 * Type enum for RecordValue.
 */
typedef enum RecordValueType {
	STRING,
	SECOND
} RecordValueType;

/**
 * @struct RecordValue
 * RecordValue for a Record map that can store either a string or a time_t.
 */
typedef struct RecordValue {
	RecordValueType type;
	union {
		char *str;
		time_t sec;
	} data;
} RecordValue;

/**
 * @struct RecordEntry
 * a RecordEntry of a Record map.
 */
typedef struct RecordEntry {
	char *key;
	int len;
	RecordValue *val;
} RecordEntry;

/**
 * @struct Record
 * A Record stores each field value as a RecordEntry. It additionally stores any
 * post-processed data to be indexed by a key from the PostKeys lookup table.
 */
typedef struct Record {
	RecordEntry **entries;
} Record;

/**
 * @struct OutputSection
 * An OutputSection for Output storing a vector of Records for each respective
 * data processing subset.
 */
typedef struct OutputSection {
	Record **dispatch_received_diff;
	Record **onscene_enroute_diff;
	Record **onscene_received_diff;
} OutputSection;

/**
 * @struct Output
 * Final Output. For each argument subfield, stores each RecordSection (which
 * holds each data processing subset) and the total size of the respective
 * Records.
 */
typedef struct Output {
	OutputSection *total;
	size_t totalsz;
	OutputSection *arg1;
	size_t arg1sz;
	OutputSection *arg2;
	size_t arg2sz;

	Field **fields;
} Output;

/**
 * Constructor for final Output.
 */
Output *create_output();

/**
 * @enum ProcessKeys
 * Named constants for keys we need to process. To be used as indices into an
 * array of ProcessEntries to store their respective value.
 */
enum ProcessKeys {
	CALL_TYPE_FINAL_DESC,
	CALL_TYPE_ORIGINAL_DESC,
	DISPATCH_DATETIME,
	RECEIVED_DATETIME,
	ONSCENE_DATETIME,
	ENROUTE_DATETIME,
	ANALYSIS_NEIGHBORHOOD,
	POLICE_DISTRICT,
	PROCESS_KEYS_TOTAL
};

/**
 * @struct ProcessEntry
 * Temporary lookup table for processing RecordEntries.
 */
typedef struct ProcessEntry {
	int key;
	RecordValue *val;
} ProcessEntry;

/**
 * @enum PostKeys
 * Lookup table keys for the final post-processed Record map.
 */
enum PostKeys {
	FINAL_DESC,
	DISPATCH_RECEIVED_DIFF,
	ONSCENE_ENROUTE_DIFF,
	ONSCENE_RECEIVED_DIFF,
	POST_ANALYSIS_NEIGHBORHOOD,
	POST_POLICE_DISTRICT,
	POST_KEYS_TOTAL
};

/**
 * @struct PostEntry
 * Temporary lookup table for storing post-processed entries of the Record map,
 * before finally inserting into the Record map itself.
 */
typedef struct PostEntry
{
	int key;
	RecordValue *val;
} PostEntry;

/**
 * DSL wrapper for strtol(3).
 */
int strtoindex(char *strindex);

/**
 * @pre Caller allocates lines and passes as a mutator argument.
 * @pre Opens `header.txt` with relative paths. `header.txt` must be in
 * application root.
 *
 * @post Caller deallocates lines.
 */
int split_lines(const char *fh, char ***lines);

/**
 * Wrapper for string datetime to seconds conversion.
 */
time_t strdate_to_seconds(char *str);

/**
 * @post Allocates an array of pointers to PostEntries. Call
 * destroy_post_entries to deallocate.
 */
PostEntry **process_data(ProcessEntry **entries);

/**
 * Based on a Fields manifest, populates a Record.
 *
 * @pre Caller allocates Record.
 * @post Caller deallocates Record.
 */
void populate_record(Record **record, char *buf, int begin, int len, const int MAX);

/**
 * Get the length of a Record.
 *
 * @param Field **fields	A Field vector to get the length from.
 */
int get_record_len(Field **fields);

/**
 * Get the amount of Fields.
 *
 * @param Field **fields	A Field vector to get the amount from.
 */
int get_field_amt(Field **fields);

/**
 * Main driver to parse a data set and populate a vector of Records.
 */
void parse_records2(Record **records, Field **fields, int len, int fd,
	int begin, int end);

/**
 * Look up the Field index for this key.
 */
int lookup_field_index(Field **fields, char *key);

/**
 * @remark Can also be used without a filter, to create a copy of the Records
 * passed. Set index to -1 to simply create a copy.
 *
 * @param int *newsz	Pass an optional size parameter to get the new size of
 * 						the Records.
 */
Record **filter_records(Record **records, Field **fields, char *match, 
	int index, int begin, int end, int *newsz);

/**
 * Print the provided Records.
 */
void print_records(Record **records, const int MAX);

/**
 * Sort routine to call as a thread job.
 * @warning Deprecated
 */
Record **thsort(Record **records, Field **fields, int begin, int end);

/**
 * Get the size of the file descriptor on disk.
 */
off_t fdsize(int fd);

#endif	// CORE_H

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
