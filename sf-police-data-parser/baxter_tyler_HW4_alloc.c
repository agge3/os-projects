/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_alloc.c
*
* Description:: Allocators and deallocators implementation.
*
**************************************************************/

#include "baxter_tyler_HW4_alloc.h"
#include <stdlib.h>

int destroy_tmp_records(Record **records)
{
	if (records == NULL) {
		return 0;
	}
	if (*records = NULL) {
		free(records);
		return 0;
	}

	size_t i = 0;
	while (records[i] != NULL) {
		free(records[i]);
		i++;
	}

	free(records);
	return 0;
}

OutputSection *create_output_section(int size)
{
	OutputSection *output_section = malloc(sizeof(OutputSection));
	if (output_section == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	output_section->dispatch_received_diff = create_records(0, size);

	output_section->onscene_enroute_diff = create_records(0, size);

	output_section->onscene_received_diff = create_records(0, size);

	return output_section;
}

Output *create_output(int size)
{
	Output *output = malloc(sizeof(Output));
	if (output == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	output->total = malloc(sizeof(OutputSection));
	if (output->total == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	output->total->dispatch_received_diff = create_records(0, size);
	output->total->onscene_enroute_diff = create_records(0, size);
	output->total->onscene_received_diff  = create_records(0, size);

	output->arg1 = malloc(sizeof(OutputSection));
	if (output->arg1 == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	output->arg1->dispatch_received_diff = create_records(0, size);
	output->arg1->onscene_enroute_diff = create_records(0, size);
	output->arg1->onscene_received_diff  = create_records(0, size);

	output->arg2 = malloc(sizeof(OutputSection));
	if (output->arg2 == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	output->arg2->dispatch_received_diff = create_records(0, size);
	output->arg2->onscene_enroute_diff = create_records(0, size);
	output->arg2->onscene_received_diff  = create_records(0, size);

	// Default initialize sizes to zero.
	output->totalsz = 0;
	output->arg1sz = 0;
	output->arg2sz = 0;

	output->fields = malloc(sizeof(Field *));
	if (output->fields == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	return output;
}

void destroy_output(Output *output)
{
	destroy_records(&output->total->dispatch_received_diff);
	destroy_records(&output->total->onscene_enroute_diff);
	destroy_records(&output->total->onscene_received_diff);

	destroy_records(&output->arg1->dispatch_received_diff);
	destroy_records(&output->arg1->onscene_enroute_diff);
	destroy_records(&output->arg1->onscene_received_diff);

	destroy_records(&output->arg2->dispatch_received_diff);
	destroy_records(&output->arg2->onscene_enroute_diff);
	destroy_records(&output->arg2->onscene_received_diff);

	destroy_fields(&output->fields);

	// xxx destroy_records frees these pointers.
	//free(output->total);
	//output->total = NULL;

	//free(output->arg1);
	//output->arg1 = NULL;

	//free(output->arg2);
	//output->arg2 = NULL;

	//free(*output);
	//output = NULL;
}

/**
 * Returns an array of ProcessEntries.
 *
 * @todo Only used on the stack by populate_record; reconsider if this needs a
 * dynamic memory allocation.
 */
ProcessEntry **create_process_entries()
{
	ProcessEntry **entries =
		malloc(PROCESS_KEYS_TOTAL * sizeof(ProcessEntry *));
	if (entries == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	for (size_t i = 0; i < PROCESS_KEYS_TOTAL; i++) {
		entries[i] = malloc(sizeof(ProcessEntry));
		if (entries[i] == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		entries[i]->val = malloc(sizeof(RecordValue));
		if (entries[i]->val == NULL) {
			err(EXIT_FAILURE, "malloc");
		}
	}

	return entries;
}

PostEntry **create_post_entries()
{
	PostEntry **entries =
		malloc(POST_KEYS_TOTAL * sizeof(PostEntry *));
	if (entries == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	for (size_t i = 0; i < POST_KEYS_TOTAL; i++) {
		entries[i] = malloc(sizeof(PostEntry));
		if (entries[i] == NULL) {
	  		err(EXIT_FAILURE, "malloc");
		}

		entries[i]->val = malloc(sizeof(RecordValue));
		if (entries[i]->val == NULL) {
	  		err(EXIT_FAILURE, "malloc");
		}
	}

	return entries;
}

int destroy_process_entries(ProcessEntry ***entries)
{
	if (entries == NULL || *entries == NULL) {
		return 1;
	}

	for (size_t i = 0; i < PROCESS_KEYS_TOTAL; i++) {
		if ((*entries)[i]->val->type == STRING) {
			free((*entries)[i]->val->data.str);
			(*entries)[i]->val->data.str = NULL;
		}

		free((*entries)[i]->val);
		(*entries)[i]->val = NULL;

		free((*entries)[i]);
		(*entries)[i] = NULL;
	}

	free(*entries);
	*entries = NULL;

	return 1;
}

int destroy_post_entries(PostEntry ***entries)
{
	if (entries == NULL || *entries == NULL) {
		return 1;
	}

	for (size_t i = 0; i < POST_KEYS_TOTAL; i++) {
		if ((*entries)[i]->val->type == STRING) {
			// xxx gdb backtrace here
			free((*entries)[i]->val->data.str);
			(*entries)[i]->val->data.str = NULL;
		}

		free((*entries)[i]->val);
		(*entries)[i]->val = NULL;

		free((*entries)[i]);
		(*entries)[i] = NULL;
	}

	free(*entries);
	*entries = NULL;

	return 1;
}

/**
 * Allocate a vector of lines split by newline.
 */
char **alloc_lines()
{
	char **lines = malloc(MAX_LINES * sizeof(char *));
	if (lines == NULL) {
		err(EXIT_FAILURE, "malloc");
	}
	return lines;
}

/**
 * Free an allocated vector of newline-split lines.
 */
int free_lines(char ***lines)
{
	if (*lines == NULL) {
		return 0;
	}

	size_t i = 0;
	while ((*lines)[i] != NULL) {
		free((*lines)[i]);
		(*lines)[i] = NULL;
		i++;
	}
	free(*lines);
	*lines = NULL;
	return 1;
}

/**
 * Returns a vector of Fields parsed from `headers.txt`.
 *
 * Ownership model: Caller must call destroy_fields to deallocate.
 */
Field **create_fields(const char *fh)
{
	Field **fields = malloc(MAX_FIELDS * sizeof(Field *));
	if (fields == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	char **lines = alloc_lines();
	if (!split_lines(fh, &lines)) {
		err(EXIT_FAILURE, "split_lines");
	}

	// Indices are prefixed with the first letter of what they refer to.
	size_t fidx = 0;
	size_t sidx = 0;
	size_t bidx = 0;

	// Buffers for storing the entry and index while we parse.
	char buf[MAX_LINE];
	char sindex[3];

	// Field.index always comes first in the line.
	int parse_idx = 1;

	size_t i = 0;
	size_t j = 0;
	while (lines[i] != NULL && fidx < MAX_FIELDS - 1) {
		// Skip malformed lines.
		if (strchr(lines[i], ':') == NULL) {
			i++;
			continue;
		}

		// Always ignore whitespace.
		if (lines[i][j] == ' ') {
			j++;
			continue;
		}

		// This is the first entry point into the loop. We always parse for the
		// index first.
		if (parse_idx) {
			// Stop parsing for index if we reach a colon.
			if (lines[i][j] == ':') {
				sindex[sidx] = '\0';
				int index = strtoindex(sindex);

				// Since index is our first entry point for parsing the line, at 
				// this point we allocate the Field struct for this index (and
				// its subsequent entry).
				fields[fidx] = malloc(sizeof(Field));
				if (fields[fidx] == NULL) {
					err(EXIT_FAILURE, "malloc");
				}

				fields[fidx]->index = index;

				parse_idx = 0;
				sidx = 0;
				j++;

				continue;	// proceed next to parse the entry
			}

			// Guard an invalid state.
			if (sidx >= sizeof(sindex) - 1) {
				err(EXIT_FAILURE, "Field.index > 2");
			}

			// Parse index: Main execution path.
			sindex[sidx++] = lines[i][j++];

			continue;	// skip rest of execution until done parsing index
		}
		if (lines[i][j] == '\0') {
			#ifdef DEBUG
			printf("create_fields:\tif lines[%zu][%zu] == '\\0':\tENTERED\n",
		  		i, j);
			#endif

			buf[bidx] = '\0';

			fields[fidx]->entry = malloc((bidx + 1) * sizeof(char));
			if (fields[fidx]->entry == NULL) {
				err(EXIT_FAILURE, "malloc");
			}
			strncpy(fields[fidx]->entry, buf, bidx + 1);

			#ifdef DEBUG
			printf("create_fields:\tstack buffer:\t%s\n", buf);
			printf("create_fields:\tField.index:\t%d\n", fields[fidx]->index);
			printf("create_fields:\tField.entry:\t%s\n", fields[fidx]->entry);
			#endif

			fidx++;
			i++;	// we only advance lines here

			j = 0;	// reset to start of line
			bidx = 0;	// reset to start of buffer
			parse_idx = 1;	// prepare to parse for index

			continue;
		}

		// Guard an invalid state.
		if (bidx >= sizeof(buf) - 1) {
			err(EXIT_FAILURE, "Field.entry buffer overflow");
		}

		// Parse entry: Main execution path.
		buf[bidx++] = lines[i][j++];
	}
	fields[fidx] = NULL;

	if (lines[i] != NULL) {
		err(EXIT_FAILURE, "lines exceeds MAX_FIELDS");
	}

	free_lines(&lines);

	return fields;
}

int destroy_fields(Field ***fields)
{
	if (fields == NULL || *fields == NULL) {
		return 0;	// success, already destroyed.
	}

	size_t i = 0;
	while ((*fields)[i] != NULL) {
		if ((*fields)[i]->entry != NULL) {
			free((*fields)[i]->entry);
			(*fields)[i]->entry = NULL;
		}
		free((*fields)[i]);
		(*fields)[i] = NULL;
		i++;
	}
	free(*fields);
	*fields = NULL;

	return 0;	// success, destroyed
}

Record *create_record(Field **fields)
{
	Record *record = malloc(sizeof(Record));
	if (record == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	record->entries = malloc(MAX_FIELDS * sizeof(RecordEntry *));
	if (record->entries == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	size_t i = 0;
	while (fields[i] != NULL) {
		record->entries[i] = malloc(sizeof(RecordEntry));
		if (record->entries[i] == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		size_t len = strlen(fields[i]->entry);
		record->entries[i]->key = malloc((len + 1) * sizeof(char));
		if (record->entries[i]->key == NULL) {
	  		err(EXIT_FAILURE, "malloc");
		}
		strcpy(record->entries[i]->key, fields[i]->entry);

		record->entries[i]->len = fields[i]->index;

		record->entries[i]->val = malloc(sizeof(RecordValue));
		if (record->entries[i]->val == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		i++;
	}

	record->entries[i] = malloc(sizeof(RecordEntry));
	if (record->entries[i] == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	record->entries[i]->key = malloc(BUFSZ * sizeof(char));
	if (record->entries[i]->key == NULL) {
		err(EXIT_FAILURE, "malloc");
	}
	strcpy(record->entries[i]->key, "final_desc");

	// Sentinel value signifies user-data outside of record parsing.
	record->entries[i]->len = -1;

	record->entries[i]->val = malloc(sizeof(RecordValue));
	if (record->entries[i]->val == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	i++;

	record->entries[i] = malloc(sizeof(RecordEntry));
	if (record->entries[i] == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	record->entries[i]->key = malloc(BUFSZ * sizeof(char));
	if (record->entries[i]->key == NULL) {
		err(EXIT_FAILURE, "malloc");
	}
	strcpy(record->entries[i]->key, "dispatch_received_diff");

	// Sentinel value signifies user-data outside of record parsing.
	record->entries[i]->len = -1;

	record->entries[i]->val = malloc(sizeof(RecordValue));
	if (record->entries[i]->val == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	i++;

	record->entries[i] = malloc(sizeof(RecordEntry));
	if (record->entries[i] == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	record->entries[i]->key = malloc(BUFSZ * sizeof(char));
	if (record->entries[i]->key == NULL) {
		err(EXIT_FAILURE, "malloc");
	}
	strcpy(record->entries[i]->key, "onscene_enroute_diff");

	// Sentinel value signifies user-data outside of record parsing.
	record->entries[i]->len = -1;

	record->entries[i]->val = malloc(sizeof(RecordValue));
	if (record->entries[i]->val == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	i++;

	record->entries[i] = malloc(sizeof(RecordEntry));
	if (record->entries[i] == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	record->entries[i]->key = malloc(BUFSZ * sizeof(char));
	if (record->entries[i]->key == NULL) {
		err(EXIT_FAILURE, "malloc");
	}
	strcpy(record->entries[i]->key, "onscene_received_diff");

	// Sentinel value signifies user-data outside of record parsing.
	record->entries[i]->len = -1;

	record->entries[i]->val = malloc(sizeof(RecordValue));
	if (record->entries[i]->val == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	i++;

	record->entries[i] = malloc(sizeof(RecordEntry));
	if (record->entries[i] == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	record->entries[i]->key = malloc(BUFSZ * sizeof(char));
	if (record->entries[i]->key == NULL) {
		err(EXIT_FAILURE, "malloc");
	}
	strcpy(record->entries[i]->key, "post_analysis_neighborhood");

	// Sentinel value signifies user-data outside of record parsing.
	record->entries[i]->len = -1;

	record->entries[i]->val = malloc(sizeof(RecordValue));
	if (record->entries[i]->val == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	i++;

	record->entries[i] = malloc(sizeof(RecordEntry));
	if (record->entries[i] == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	record->entries[i]->key = malloc(BUFSZ * sizeof(char));
	if (record->entries[i]->key == NULL) {
		err(EXIT_FAILURE, "malloc");
	}
	strcpy(record->entries[i]->key, "post_police_district");

	// Sentinel value signifies user-data outside of record parsing.
	record->entries[i]->len = -1;

	record->entries[i]->val = malloc(sizeof(RecordValue));
	if (record->entries[i]->val == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	i++;

	record->entries[i] = NULL;

	return record;
}

int destroy_record(Record **record)
{
	if (record == NULL || *record == NULL) {
		return 0;	// success, nothing to destroy
	}

	// xxx do we need?
	if ((*record)->entries == NULL) {
		free((*record)->entries);
		(*record)->entries = NULL;
		
		free(*record);
		*record = NULL;

		return 0;
	}

	size_t i = 0;
	while ((*record)->entries[i] != NULL) {
		if ((*record)->entries[i]->key != NULL) {
			free((*record)->entries[i]->key);
			(*record)->entries[i]->key = NULL;
		}
		
		if ((*record)->entries[i]->val != NULL) {
			if ((*record)->entries[i]->val->type == STRING &&
				(*record)->entries[i]->val->data.str != NULL) {
				free((*record)->entries[i]->val->data.str);
				(*record)->entries[i]->val->data.str = NULL;
			}
		
			free((*record)->entries[i]->val);
			(*record)->entries[i]->val = NULL;
		}

		free((*record)->entries[i]);
		(*record)->entries[i] = NULL;

		i++;
	}

	free((*record)->entries);
	(*record)->entries = NULL;

	free(*record);
	*record = NULL;

	return 0;
}
/**
 * Creates a vector of Records.
 */
Record **create_records(int begin, int end)
{
	int amt = end - begin;

	Record **records = calloc((amt + 1), sizeof(Record *));
	if (records == NULL) {
		err(EXIT_FAILURE, "calloc");
	}

	// xxx shouldn't break things
	records[amt] = NULL;

	return records;
}

/**
 * Destroys a vector of Records.
 */
int destroy_records(Record ***records)
{
	if (records == NULL) {
		return 0;	// nothing to destroy
	}

	if (*records == NULL) {
		return 0;
	}

	if (*records != NULL && (*records)[0] == NULL) {
		#ifdef DEBUG
		printf("destroy_records:\tedge case:\tENTERED");
		//free(*records);
		//*records = NULL;
		#endif
		return 0;
	}

	size_t i = 0;
	while ((*records)[i] != NULL) {
		if (destroy_record(&(*records)[i]) != 0) {
			err(EXIT_FAILURE, "destroy_record");
		}
		i++;
	}

	free(*records);
	*records = NULL;

	return 0;
}

/**
 * Make a heap allocated copy of a Record.
 */
Record *copy_record(Record *record, Field **fields)
{
	Record *new_record = create_record(fields);
	
	size_t i = 0;
	while (record->entries[i] != NULL) {
		if (record->entries[i]->val->type == STRING) {
			new_record->entries[i]->val->type =
				record->entries[i]->val->type;
				
			size_t len = strlen(record->entries[i]->val->data.str);
			new_record->entries[i]->val->data.str =
				malloc((len + 1) * sizeof(char));
			if (new_record->entries[i]->val->data.str == NULL) {
				err(EXIT_FAILURE, "malloc");
			}

			strcpy(new_record->entries[i]->val->data.str,
				record->entries[i]->val->data.str);
		} else {
			new_record->entries[i]->val->type =
				record->entries[i]->val->type;

			new_record->entries[i]->val->data.sec =
				record->entries[i]->val->data.sec;

		}		
		i++;
	}

	return new_record;
}

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
