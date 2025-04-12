/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_core.
*
* Description:: Core implementation.
*
**************************************************************/

#include "baxter_tyler_HW4_core.h"
#include "baxter_tyler_HW4_alloc.h"
#include "baxter_tyler_HW4_sort.h"

#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/stat.h>

//#define LAW500K

off_t fdsize(int fd)
{
	struct stat st;
	if (fstat(fd, &st) == -1) {
		err(EXIT_FAILURE, "fstat");		
	}
	return st.st_size;
}

/**
 * DSL wrapper for strtol(3).
 */
int strtoindex(char *strindex)
{
	char *endptr;
	int index = (int)strtol(strindex, &endptr, 10);
	
	#ifdef DEBUG
	//printf("create_fields:\tField.index:\t%s\n", endptr);
	#endif

	if (*endptr != '\0') {
		err(EXIT_FAILURE, "strtol");	// xxx better error
	}

	return index;
}

/**
 * @pre Caller allocates lines and passes as a mutator argument.
 * @pre Opens `header.txt` with relative paths. `header.txt` must be in
 * application root.
 *
 * @post Caller deallocates lines.
 */
int split_lines(const char *fh, char ***lines)
{
	int fd = open(fh, O_RDONLY | O_NOATIME);
	if (fd == -1) {
		err(EXIT_FAILURE, "open");	// fatal
	}

	// Format follows: `%d: %s\n`. Parse based on that.
	char buf[HEADER_TXT_BUFFER];
	ssize_t bytes;

	// we could either read(2) byte-wise, or read(2) the entire block. We're 
	// going to read(2) the entire block front-load the memory chunk all at
	// once.
	bytes = read(fd,  buf, HEADER_TXT_BUFFER);
	if (bytes < 1) {
		err(EXIT_FAILURE, "read");	// xxx better error condition
	}

	char word[MAX_LINE];

	size_t lidx = 0;
	size_t widx = 0;
	for (ssize_t i = 0; i < bytes; i++) {
		if (buf[i] == '\n') {
			word[widx] = '\0';

			(*lines)[lidx] = malloc((widx + 1) * sizeof(char));
			if ((*lines)[lidx] == NULL) {
				err(EXIT_FAILURE, "malloc");
			}
			strncpy((*lines)[lidx], word, widx + 1);

			lidx++;
			widx = 0;
		} else {
			// Guard an invalid state.
			if (widx >= sizeof(word) - 1) {
				err(EXIT_FAILURE, "split_lines buffer overflow");
			}

			// Parse line: Main execution path.
			word[widx] = buf[i];
			widx++;
		}
	}
	(*lines)[lidx] = NULL;

	#ifdef DEBUG
	size_t i = 0;
	while ((*lines)[i] != NULL) {
		printf("split_lines:\tlines[%zu]:\t%s\n", i, (*lines)[i]);
		i++;
	}
	#endif

	return 1;
}

time_t strdate_to_seconds(char *str)
{
	struct tm tm = {0};
	char *fmt = "%m/%d/%Y %I:%M:%S %p";

	if (strptime(str, fmt, &tm) == NULL) {
		return -1;	// zero seconds if parsing fails
	}

	tm.tm_isdst = -1;	// let mktime handle DST
	time_t time = mktime(&tm);

	#ifdef DEBUG
	printf("strdate_to_seconds:\tstr:\t%s\n", str);
	printf("strdate_to_seconds:\ttime:\t%ld\n", time);
	#endif
	
	return time;
}


/**
 * @post Allocates an array of pointers to PostEntries. Call
 * destroy_post_entries to deallocate.
 */
PostEntry **process_data(ProcessEntry **entries)
{
	PostEntry **post_entries = create_post_entries();

	if (entries[CALL_TYPE_FINAL_DESC]->val->data.str == NULL ||
	 	strcmp(entries[CALL_TYPE_FINAL_DESC]->val->data.str, "") == 0) {
		size_t len = strlen(entries[CALL_TYPE_FINAL_DESC]->val->data.str);

		post_entries[FINAL_DESC]->val->type = STRING;
		post_entries[FINAL_DESC]->val->data.str =
			malloc((len + 1) * sizeof(char));
		if (post_entries[FINAL_DESC]->val->data.str == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		strcpy(post_entries[FINAL_DESC]->val->data.str,
			entries[CALL_TYPE_ORIGINAL_DESC]->val->data.str);
	} else {
		size_t len = strlen(entries[CALL_TYPE_ORIGINAL_DESC]->val->data.str);

		post_entries[FINAL_DESC]->val->type = STRING;
		post_entries[FINAL_DESC]->val->data.str =
			malloc((len + 1) * sizeof(char));
		if (post_entries[FINAL_DESC]->val->data.str == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		strcpy(post_entries[FINAL_DESC]->val->data.str,
			entries[CALL_TYPE_ORIGINAL_DESC]->val->data.str);
	}

	// Record the subfield for the final lookup table.
	size_t len = strlen(entries[ANALYSIS_NEIGHBORHOOD]->val->data.str);

	post_entries[POST_ANALYSIS_NEIGHBORHOOD]->val->type = STRING;
	post_entries[POST_ANALYSIS_NEIGHBORHOOD]->val->data.str =
		malloc((len + 1) * sizeof(char));
	if (post_entries[POST_ANALYSIS_NEIGHBORHOOD]->val->data.str == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	strcpy(post_entries[POST_ANALYSIS_NEIGHBORHOOD]->val->data.str,
		entries[ANALYSIS_NEIGHBORHOOD]->val->data.str);

	len = strlen(entries[POLICE_DISTRICT]->val->data.str);

	post_entries[POST_POLICE_DISTRICT]->val->type = STRING;
	post_entries[POST_POLICE_DISTRICT]->val->data.str =
		malloc((len + 1) * sizeof(char));
	if (post_entries[POST_POLICE_DISTRICT]->val->data.str == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	strcpy(post_entries[POST_POLICE_DISTRICT]->val->data.str,
		entries[POLICE_DISTRICT]->val->data.str);

	// Calculate difference and normalize to -1 if an invalid number.
	time_t dispatch_received_diff =
		(entries[DISPATCH_DATETIME]->val->data.sec != -1 &&
		entries[RECEIVED_DATETIME]->val->data.sec != -1)
			? entries[DISPATCH_DATETIME]->val->data.sec -
			entries[RECEIVED_DATETIME]->val->data.sec
			: -1;
	post_entries[DISPATCH_RECEIVED_DIFF]->val->data.sec =
		dispatch_received_diff;

	time_t onscene_enroute_diff =
		(entries[ONSCENE_DATETIME]->val->data.sec != -1 &&
		entries[ENROUTE_DATETIME]->val->data.sec != -1)
			? entries[ONSCENE_DATETIME]->val->data.sec -
			entries[ENROUTE_DATETIME]->val->data.sec
			: -1;
	post_entries[ONSCENE_ENROUTE_DIFF]->val->data.sec = onscene_enroute_diff;
	
	time_t onscene_received_diff =
		(entries[ONSCENE_DATETIME]->val->data.sec != -1 &&
		entries[RECEIVED_DATETIME]->val->data.sec != -1)
			? entries[ONSCENE_DATETIME]->val->data.sec -
			entries[RECEIVED_DATETIME]->val->data.sec
			: -1;
	post_entries[ONSCENE_RECEIVED_DIFF]->val->data.sec = onscene_received_diff;

	#ifdef DEBUG
	printf(
		"process_data:\tdispatch_received_diff:\t%ld\n"
		"process_data:\tentries[DISPATCH_DATETIME]->val->data.sec:\t%ld\n"
		"process_data:\tentries[RECEIVED_DATETIME]->val->data.sec:\t%ld\n"
		"process_data:\tpost_entries[DISPATCH_RECEIVED_DIFF]->val->data.sec:\t%ld\n"
		"process_data:\tonscene_enroute_diff:\t%ld\n"
		"process_data:\tentries[ONSCENE_DATETIME]->val->data.sec:\t%ld\n"
		"process_data:\tentries[ENROUTE_DATETIME]->val->data.sec:\t%ld\n"
		"process_data:\tpost_entries[ONSCENE_ENROUTE_DIFF]->val->data.sec:\t%ld\n"
		"process_data:\tonscene_received_diff:\t%ld\n"
		"process_data:\tentries[ONSCENE_DATETIME]->val->data.sec:\t%ld\n"
		"process_data:\tentries[RECEIVED_DATETIME]->val->data.sec:\t%ld\n"

		"process_data:\tentries[ANALYSIS_NEIGHBORHOOD]->val->data.str:\t%s\n"
		"process_data:\tentries[POLICE_DISTRICT]->val->data.str:\t%s\n",
		dispatch_received_diff,
		entries[DISPATCH_DATETIME]->val->data.sec,
		entries[RECEIVED_DATETIME]->val->data.sec,
		post_entries[DISPATCH_RECEIVED_DIFF]->val->data.sec,
		onscene_enroute_diff,
		entries[ONSCENE_DATETIME]->val->data.sec,
		entries[ENROUTE_DATETIME]->val->data.sec,
		post_entries[ONSCENE_ENROUTE_DIFF]->val->data.sec,
		onscene_received_diff,
		entries[ONSCENE_DATETIME]->val->data.sec,
		entries[RECEIVED_DATETIME]->val->data.sec,

		entries[ANALYSIS_NEIGHBORHOOD]->val->data.str,
		entries[POLICE_DISTRICT]->val->data.str
	);
	#endif

	return post_entries;
}

/**
 * Based on a Fields manifest, populates a Record.
 *
 * @pre Caller allocates Record.
 * @post Caller deallocates Record.
 */
void populate_record(Record **record, char *buf, int begin, int len, const int MAX)
{
	ProcessEntry **entries = create_process_entries();

	#ifdef DEBUG
		printf("populate_record:\tlen:\t%d\n", len);
	#endif

	size_t i = 0;
	size_t offset = 0;

	// Recall that create_record allocates extra space for additional Record
	// information outside of parsing. We need to subtract for our bounds any
	// additional allocations.
	while ((*record)->entries[i] != NULL && i < MAX) {
		char *v = buf + begin + offset;
		
		#ifdef DEBUG
		printf("populate_record:\tvalue:\t%s\n", v);

		if (v == NULL) {
			printf("populate_record:\tvalue:\tNULL\n");
		}
		if (begin + offset == len) {
			printf("populate_record:\tbegin + offset == len:\t%ld\n",
		  		begin + offset);
		}
		#endif

		if (strstr((*record)->entries[i]->key, "datetime") != NULL) {
			#ifdef DEBUG
			printf("populate_record:\tsec value:\t%s\n", v);
			#endif

			time_t sec = strdate_to_seconds(v);
			(*record)->entries[i]->val->type = SECOND;
			(*record)->entries[i]->val->data.sec = sec;

			// Record the index of any entries we need to process.
			if (strstr((*record)->entries[i]->key,
				"dispatch_datetime") != NULL) {
				entries[DISPATCH_DATETIME]->key = i;
				entries[DISPATCH_DATETIME]->val->type = SECOND;
				entries[DISPATCH_DATETIME]->val->data.sec = sec;

			} else if (strstr((*record)->entries[i]->key,
				"received_datetime") != NULL) {
				entries[RECEIVED_DATETIME]->key = i;
				entries[RECEIVED_DATETIME]->val->type = SECOND;
				entries[RECEIVED_DATETIME]->val->data.sec = sec;

			} else if (strstr((*record)->entries[i]->key,
				"onscene_datetime") != NULL) {
				entries[ONSCENE_DATETIME]->key = i;
				entries[ONSCENE_DATETIME]->val->type = SECOND;
				entries[ONSCENE_DATETIME]->val->data.sec = sec;

			} else if (strstr((*record)->entries[i]->key,
				"enroute_datetime") != NULL) {
				entries[ENROUTE_DATETIME]->key = i;
				entries[ENROUTE_DATETIME]->val->type = SECOND;
				entries[ENROUTE_DATETIME]->val->data.sec = sec;
			}
		} else {
			#ifdef DEBUG
			//printf("populate_record:\tstr value:\t%s\n", v);
			#endif

			assert((*record)->entries[i]->val != NULL);

			(*record)->entries[i]->val->type = STRING;

			(*record)->entries[i]->val->data.str =
				malloc((strlen(v) + 1) * sizeof(char));
			if ((*record)->entries[i]->val->data.str == NULL) {
				err(EXIT_FAILURE, "malloc");
			}

			strcpy((*record)->entries[i]->val->data.str, v);

			// Record the index of any entries we need to process.
			if (strstr((*record)->entries[i]->key,
				"call_type_final_desc") != NULL) {
				entries[CALL_TYPE_FINAL_DESC]->val->type = STRING;

				entries[CALL_TYPE_FINAL_DESC]->val->data.str =
					malloc((strlen(v) + 1) * sizeof(char));
				if (entries[CALL_TYPE_FINAL_DESC]->val->data.str == NULL) {
					err(EXIT_FAILURE, "malloc");
				}

				strcpy(entries[CALL_TYPE_FINAL_DESC]->val->data.str, v);

			} else if (strstr((*record)->entries[i]->key,
				"call_type_original_desc") != NULL) {
				entries[CALL_TYPE_ORIGINAL_DESC]->val->type = STRING;

				entries[CALL_TYPE_ORIGINAL_DESC]->val->data.str =
					malloc((strlen(v) + 1) * sizeof(char));
				if (entries[CALL_TYPE_ORIGINAL_DESC]->val->data.str == NULL) {
					err(EXIT_FAILURE, "malloc");
				}

				strcpy(entries[CALL_TYPE_ORIGINAL_DESC]->val->data.str, v);

			} else if (strstr((*record)->entries[i]->key,
				"analysis_neighborhood") != NULL) {
				#ifdef DEBUG
				printf("populate_record:\tanalysis_neighborhood:\t%s\n", v);
				#endif

				entries[ANALYSIS_NEIGHBORHOOD]->val->type = STRING;

				entries[ANALYSIS_NEIGHBORHOOD]->val->data.str =
					malloc((strlen(v) + 1) * sizeof(char));
				if (entries[ANALYSIS_NEIGHBORHOOD]->val->data.str == NULL) {
					err(EXIT_FAILURE, "malloc");
				}

				strcpy(entries[ANALYSIS_NEIGHBORHOOD]->val->data.str, v);

			} else if (strstr((*record)->entries[i]->key,
				"police_district") != NULL) {
				entries[POLICE_DISTRICT]->val->type = STRING;

				entries[POLICE_DISTRICT]->val->data.str =
					malloc((strlen(v) + 1) * sizeof(char));
				if (entries[POLICE_DISTRICT]->val->data.str == NULL) {
					err(EXIT_FAILURE, "malloc");
				}

				strcpy(entries[POLICE_DISTRICT]->val->data.str, v);
			}
		}

		offset += (*record)->entries[i]->len;
		i++;
	}

	#ifdef DEBUG
	printf("populate_record:\toffset:\t%ld\n", offset);
	#endif

	PostEntry **post_entries = process_data(entries);

	// Postprocess the Record to normalize our final important data points.
	char *s = post_entries[FINAL_DESC]->val->data.str;

	(*record)->entries[i]->val->data.str =
		malloc((strlen(s) + 1) * sizeof(char));
	if ((*record)->entries[i]->val->data.str == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	strcpy((*record)->entries[i]->val->data.str, s);
	(*record)->entries[i]->val->type = STRING;

	i++;

	(*record)->entries[i]->val->data.sec =
		post_entries[DISPATCH_RECEIVED_DIFF]->val->data.sec;
	(*record)->entries[i]->val->type = SECOND;
	i++;

	(*record)->entries[i]->val->data.sec =
		post_entries[ONSCENE_ENROUTE_DIFF]->val->data.sec;
	(*record)->entries[i]->val->type = SECOND;
	i++;

	(*record)->entries[i]->val->data.sec =
		post_entries[ONSCENE_RECEIVED_DIFF]->val->data.sec;
	(*record)->entries[i]->val->type = SECOND;
	i++;

	char *s2 = post_entries[POST_ANALYSIS_NEIGHBORHOOD]->val->data.str;

	(*record)->entries[i]->val->data.str =
		malloc((strlen(s2) + 1) * sizeof(char));
	if ((*record)->entries[i]->val->data.str == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	strcpy((*record)->entries[i]->val->data.str, s2);
	(*record)->entries[i]->val->type = STRING;

	i++;

	char *s3 = post_entries[POST_POLICE_DISTRICT]->val->data.str;

	(*record)->entries[i]->val->data.str =
		malloc((strlen(s3) + 1) * sizeof(char));
	if ((*record)->entries[i]->val->data.str == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	strcpy((*record)->entries[i]->val->data.str, s3);
	(*record)->entries[i]->val->type = STRING;

	i++;

	destroy_process_entries(&entries);
	destroy_post_entries(&post_entries);

}

int get_record_len(Field **fields)
{
	int len = 0;
	size_t i = 0;
	while (fields[i] != NULL) {
		len += fields[i]->index;
		i++;
	}
	
	#ifdef DEBUG
	printf("get_record_len:\tlen:\t%d\n", len);
	#endif

	return len;
}

int get_field_amt(Field **fields)
{
	int i = 0;
	while (fields[i] != NULL) {
		i++;
	}
	return i;
}

void parse_records2(Record **records, Field **fields, int len, int fd, int begin, int end)
{
	const int MAX = get_field_amt(fields);
	const int CHUNKSZ = 2000;
	int size = (end - begin) * len;

	#ifdef DEBUG
	printf("parse_records:\tsize:\t%d\n", size);
	#endif

	// upper record limit
	if (end - begin > CHUNKSZ) {
		int reclen = 0;

		// split into multiple sizes for pread to read one chunk at a time. we
		// don't want to overload our stack buffer if the amount of Records is
		// over 2000.
		int split = (end - begin) / 2000;
		int rem = (end - begin) % 2000;

		// leave one size for overflow
		int sizes[split + 1];
		for (size_t i = 0; i < split; i++) {
			sizes[i] = 2000;
		}

		// if no overflow, set to -1 and we'll break out of our loop
		sizes[split] = (rem > 0) ? rem : -1;

		size_t i = 0;
		while (i < split + 1) {
			if (sizes[i] == -1) {
				break;
			}

			// We need to add an offset to the begin we were given. If we're on
			// the first index of sizes, then that offset is zero (use begin);
			// else, that offset is the previous record chunk we parsed.
			int offset = (i != 0) ? 2000 : 0;

			#ifdef DEBUG
			printf("parse_records:\tend - begin > CHUNKSZ:\tENTERED\n");
			printf("parse_records:\tsize:\t%d\n", sizes[i]);
			printf("parse_records:\trem:\t%d\n", rem);
			printf("parse_records:\toffset:\t%d\n", begin + offset);
			#endif

			char buf[sizes[i] * len];
			if (pread(fd, buf, sizes[i] * len, (begin + offset) * len) < 1) {
				err(EXIT_FAILURE, "pread");
			}
			int amt = sizes[i];

			size_t j = 0;
			size_t k = 0;
			while (j < amt) {
				Record *record = create_record(fields);
				populate_record(&record, buf, k, len, MAX);
				records[reclen] = record;

				j++;
				k += len;
				reclen++;
			}
			i++;
		}

		records[reclen] = NULL;
	} else {
		#ifdef DEBUG
		printf("parse_records:\tsize:\t%d\n", size);
		printf("parse_records:\tbegin offset:\t%d\n", begin * len);
		printf("parse_records:\tlen:\t%d\n", len);
		printf("parse_records:\tbegin:\t%d\n", begin);
		printf("parse_records:\tend:\t%d\n", end);
		#endif

		char buf[size];

		// Location of any record (n) is simply (n - 1) * RECORD_SIZE.
		// Safe to parallelize.
		if (pread(fd, buf, size, begin * len) < 1) {
			err(EXIT_FAILURE, "pread");
		}

		int amt = end - begin;
		#ifdef DEBUG
		printf("parse_records:\tamt:\t%d\n", amt);
		#endif

		size_t i = 0;
		size_t j = 0;
		while (i < amt) {
			Record *record = create_record(fields);
			populate_record(&record, buf, j, len, MAX);
			records[i] = record;

			i++;
			j += len;
		}

		records[i] = NULL;
	}
}

int lookup_field_index(Field **fields, char *key)
{
	int i = 0;
	while (fields[i] != NULL) {
		if (strcmp(fields[i]->entry, key) == 0) {
			return i;
		}
		i++;
	}
	return -1;
}

/**
 * @remark Can also be used without a filter, to create a copy of the Records
 * passed. Set index to -1 to simply create a copy.
 */
Record **filter_records(Record **records, Field **fields, char *match,
	int index, int begin, int end, int *newsz)
{
	if (records == NULL) {
		return NULL;
	}

	Record **filrecords = create_records(begin, end);

	size_t i = 0;
	size_t j = 0;

	#ifdef DEBUG
	printf("filter_records:\trecords[i] != NULL:\tBEGIN\n");
	#endif
	while (records[i] != NULL) {
		if (index > 0 &&
			strcmp(records[i]->entries[index]->val->data.str, match) != 0) {
			i++;
			continue;
		}

		filrecords[j] = copy_record(records[i], fields);

		#ifdef DEPR
		Record *record = create_record(fields);
		size_t j = 0;
		while (records[i]->entries[j] != NULL) {
			if (records[i]->entries[j]->val->type == SECOND) {
				record->entries[j]->val->type =
					records[i]->entries[j]->val->type;

				record->entries[j]->val->data.sec =
					records[i]->entries[j]->val->data.sec;
			} else {
				record->entries[j]->val->type =
					records[i]->entries[j]->val->type;
					
				size_t len = strlen(records[i]->entries[j]->val->data.str);
				record->entries[j]->val->data.str =
					malloc((len + 1) * sizeof(char));
				if (record->entries[j]->val->data.str == NULL) {
					err(EXIT_FAILURE, "malloc");
				}

				strcpy(record->entries[j]->val->data.str,
		   			records[i]->entries[j]->val->data.str);
			}		
			j++;
		}	
		#endif

		i++;
		j++;

		// We've added a Record to the return Records - increment optional
		// newsz.
		if (newsz != NULL) {
			(*newsz)++;
		}
	}
	#ifdef DEBUG
	printf("filter_records:\trecords[i] != NULL:\tEND\n");
	#endif

	if (j == 0) {
		/// @warning When filter_records returns no records we have an invalid
		/// state that breaks our destroy_records.
		#ifdef DEBUG
		printf("filter_records:\tno records allocated\n");
		#endif
	}

	filrecords[j] = NULL;
	return filrecords;
}

void print_records(Record **records, const int MAX)
{
	printf("print_records:\tBEGIN\n");

	printf("print_records:\tMAX:\t%d\n", MAX);

	size_t i = 0;
	size_t j = 0;
	while (records[i] != NULL) {
		while (records[i]->entries[j] != NULL && j < MAX) {
			printf("records[%zu]->entries[%zu]->key:\t%s\n", i, j,
		  		records[i]->entries[j]->key);
			printf("records[%zu]->entries[%zu]->len:\t%d\n", i, j,
		  		records[i]->entries[j]->len);

			if (records[i]->entries[j]->val->type == STRING) {
				printf("records[%zu]->entries[%zu]->data.str:\t%s\n", i, j,
					records[i]->entries[j]->val->data.str);
			} else {
				printf("records[%zu]->entries[%zu]->data.sec:\t%ld\n", i, j,
					records[i]->entries[j]->val->data.sec);
			}

			j++;
		}

		j = 0;
		i++;
		printf("print_records:\ti:\t%zu\n", i);
	}

	printf("print_records:\tEND\n");
}

Record **thsort(Record **records, Field **fields, int begin, int end)
{
	int size = end - begin;
	int amt = get_field_amt(fields);

	Record **fin_records = create_records(begin, end);
	merge_sort(fin_records, records, amt + PROC_RECORDS, size, begin, end);

	return 0;
}

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
