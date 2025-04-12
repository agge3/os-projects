/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_thread.c
*
* Description:: Thread driver code implementation. th_main is an aliased main
* entry point for multithreading.
*
**************************************************************/

#include "baxter_tyler_HW4_thread.h"
#include "baxter_tyler_HW4_core.h"
#include "baxter_tyler_HW4_alloc.h"
#include "baxter_tyler_HW4_sort.h"

#include <pthread.h>
#include <stdlib.h>

#define FULL

ThreadPart **create_thparts(int fhsz, int thnum, int recsz)
{
	int recnum = fhsz / recsz;
	int sz = recnum / thnum;

	ThreadPart **parts = malloc(thnum * sizeof(ThreadPart *));
	if (parts == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	for (size_t i = 0; i < thnum; i++) {
		parts[i] = malloc(sizeof(ThreadPart));
		if (parts[i] == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		parts[i]->sz = sz;
		parts[i]->recnum = recnum;
		parts[i]->begin = sz * i;
		parts[i]->end = sz * (i + 1) - 1;
		parts[i]->recsz = recsz;
	}

	return parts;
}

int destroy_thparts(ThreadPart ***parts, int thnum)
{
	for (size_t i = 0; i < thnum; i++) {
		free((*parts)[i]);
		(*parts)[i] = NULL;
	}

	free(*parts);
	*parts = NULL;

	return 0;
}

void *th_job(void *data)
{
	ThreadData *thdata = (ThreadData *)data;

	Output *output = thdata->output;
	pthread_mutex_t *mutex = thdata->mutex;
	Field **fields = thdata->fields;
	ThreadPart *part = thdata->part;
	int fd = thdata->fd;
	char *subfield = thdata->subfield;
	char *arg1 = thdata->arg1;
	char *arg2 = thdata->arg2;

	const int INDEX = get_field_amt(fields);

	int filoffset;
	if (strcmp(subfield, "analysis_neighborhood") == 0) {
		filoffset = POST_ANALYSIS_NEIGHBORHOOD;
	} else {
		filoffset = POST_POLICE_DISTRICT;
	}

	int filidx = INDEX + filoffset;

	Record **records = create_records(0, part->sz);

	parse_records2(records, fields, part->recsz, fd, part->begin,
		part->end + 1);

	Record **dispatch_received_records =
		filter_records(records, fields, NULL, -1, 0, part->sz, NULL);
	//assert(dispatch_received_records[0] != NULL);

	Record **onscene_enroute_records =
		filter_records(records, fields, NULL, -1, 0, part->sz, NULL);
	Record **onscene_received_records =
		filter_records(records, fields, NULL, -1, 0, part->sz, NULL);

	Record **arg1_records =
		filter_records(records, fields, arg1, filidx, 0, part->sz, NULL);
	//assert(arg1_records[0] != NULL);

	Record **arg1_dispatch_received_records =
		filter_records(arg1_records, fields, NULL, -1, 0, part->sz, NULL);
	//assert(arg1_dispatch_received_records[0] != NULL);

	Record **arg1_onscene_enroute_records =
		filter_records(arg1_records, fields, NULL, -1, 0, part->sz, NULL);
	Record **arg1_onscene_received_records =
		filter_records(arg1_records, fields, NULL, -1, 0, part->sz, NULL);

	Record **arg2_records =
		filter_records(records, fields, arg2, filidx, 0, part->sz, NULL);

	Record **arg2_dispatch_received_records =
		filter_records(arg2_records, fields, NULL, -1, 0, part->sz, NULL);
	Record **arg2_onscene_enroute_records =
		filter_records(arg2_records, fields, NULL, -1, 0, part->sz, NULL);
	Record **arg2_onscene_received_records =
		filter_records(arg2_records, fields, NULL, -1, 0, part->sz, NULL);

	#ifdef DEBUG
	size_t j = 0;
	while (arg1_dispatch_received_records[j] != NULL) {
		char *s =
			arg1_dispatch_received_records[j]->entries[filidx]->val->data.str;

		printf("th_job:\targ1 filrec:\t%s\n", s);
		j++;
	}

	j = 0;
	while (arg2_dispatch_received_records[j] != NULL) {
		char *s =
			arg2_dispatch_received_records[j]->entries[filidx]->val->data.str;

		printf("th_job:\targ2 filrec:\t%s\n", s);
		j++;
	}

	printf("th_job:\tfilter_records:\tEND\n");
	#endif

	// tmp records for merge_sort. Create here so we only have to deallocate
	// once.
	Record **tmp_records =
		create_records(0, part->recnum);

	// xxx broken, merging after
	#ifdef MERGE_BEFORE
	merge_sort(tmp_records, dispatch_received_records,
		field_amt + DISPATCH_RECEIVED_DIFF, part->sz, 0, part->sz - 1);

	merge_sort(tmp_records, onscene_enroute_records,
		field_amt + ONSCENE_ENROUTE_DIFF, part->sz, 0, part->sz - 1);

	merge_sort(tmp_records, onscene_received_records,
		field_amt + ONSCENE_RECEIVED_DIFF, part->sz, 0, part->sz - 1);

	merge_sort(tmp_records, arg1_dispatch_received_records,
		field_len + DISPATCH_RECEIVED_DIFF, size, begin, end);

	merge_sort(tmp_records, arg1_onscene_enroute_records,
		field_len + ONSCENE_ENROUTE_DIFF, size, begin, end);

	merge_sort(tmp_records, arg1_onscene_received_records,
		field_len + ONSCENE_RECEIVED_DIFF, size, begin, end);

	merge_sort(tmp_records, arg2_dispatch_received_records,
		field_len + DISPATCH_RECEIVED_DIFF, size, begin, end);

	merge_sort(tmp_records, arg2_onscene_enroute_records,
		field_len + ONSCENE_ENROUTE_DIFF, size, begin, end);

	merge_sort(tmp_records, arg2_onscene_received_records,
		field_len + ONSCENE_RECEIVED_DIFF, size, begin, end);
	#endif 

	// Lock critical section and merge into master output records.
	pthread_mutex_lock(mutex);

	size_t i = 0;
	while (dispatch_received_records[i] != NULL) {
		#ifdef DEBUG
		//printf("th_job:\toutput->totalsz + i:\t%zu\n", output->totalsz + i);
		#endif

		output->total->dispatch_received_diff[output->totalsz + i] =
			copy_record(dispatch_received_records[i], fields);
		output->total->onscene_enroute_diff[output->totalsz + i] =
			copy_record(onscene_enroute_records[i], fields);
		output->total->onscene_received_diff[output->totalsz + i] =
			copy_record(onscene_received_records[i], fields);

		i++;
	}

	output->totalsz += i;

	int drdiff = INDEX + DISPATCH_RECEIVED_DIFF;
	int oediff = INDEX + ONSCENE_ENROUTE_DIFF;
	int ordiff = INDEX + ONSCENE_RECEIVED_DIFF;

	#ifdef DEBUG
	//printf("th_job:\toutput->totalsz + i:\t%zu\n", output->totalsz + i);
	//printf("th_job:\t%zu\n", i);
	//printf("th_job:\toutput data.sec:\t%ld\n",
	//	output->total->dispatch_received_diff[output->totalsz + i - 1]->
	//	entries[drdiff]->val->data.sec);
	//printf("th_job:\tthread data.sec:\t%ld\n", 
	//	onscene_received_records[i - 1]->
	//	entries[drdiff]->val->data.sec);
	#endif

	assert(dispatch_received_records[0] != NULL);
	assert(output->total->dispatch_received_diff[0] != NULL);

	#ifdef MERGE_BEFORE
	assert(
		output->total->dispatch_received_diff[output->totalsz + i - 1]->
		entries[field_amt + DISPATCH_RECEIVED_DIFF]->val->data.sec
		==
		onscene_received_records[i - 1]->
		entries[field_amt + DISPATCH_RECEIVED_DIFF]->val->data.sec
	);

	int mid = (output->totalsz < 1)
		? (part->begin + part->end) / 2
		: output->totalsz - 1;

	merge(tmp_records, output->total->dispatch_received_diff,
		field_amt + DISPATCH_RECEIVED_DIFF, output->totalsz + part->sz, 0,
		mid, output->totalsz + part->sz - 1);

	merge(tmp_records, output->total->onscene_enroute_diff,
		field_amt + ONSCENE_ENROUTE_DIFF, output->totalsz + part->sz, 0,
		mid, output->totalsz + part->sz - 1);

	merge(tmp_records, output->total->onscene_received_diff,
		field_amt + ONSCENE_RECEIVED_DIFF, output->totalsz + part->sz, 0,
		mid, output->totalsz + part->sz - 1);

	output->totalsz += i;
	#endif

	#ifdef DEBUG
	printf("th_job:\targ1_dispatch_received_records:\tBEGIN\n");
	#endif

	#ifdef FULL
	i = 0;
	while (arg1_dispatch_received_records[i] != NULL) {
		#ifdef DEBUG
		//printf("th_job:\targ1_dispatch_received_records[%zu] sec:\t%ld\n", i,
		//	arg1_dispatch_received_records[i]->
		//	entries[drdiff]->val->data.sec);
		#endif

		output->arg1->dispatch_received_diff[output->arg1sz + i] = 
			copy_record(arg1_dispatch_received_records[i], fields);
		output->arg1->onscene_enroute_diff[output->arg1sz + i] = 
			copy_record(arg1_onscene_enroute_records[i], fields);
		output->arg1->onscene_received_diff[output->arg1sz + i] = 
			copy_record(arg1_onscene_received_records[i], fields);
		i++;
	}

	output->arg1sz += i;
	#endif

	#ifdef DEBUG
	printf("th_job:\targ1_dispatch_received_records:\tEND\n");
	#endif

	#ifdef MERGE_BEFORE
	merge(tmp_records, output->arg1->dispatch_received_diff,
		field_len + DISPATCH_RECEIVED_DIFF, output->arg1sz + end, 0,
		output->arg1sz - 1, output->arg1sz + end - 1);

	merge(tmp_records, output->arg1->onscene_enroute_diff,
		field_len + ONSCENE_ENROUTE_DIFF, output->arg1sz + end, 0,
		output->arg1sz - 1, output->arg1sz + end - 1);

	merge(tmp_records, output->arg1->onscene_received_diff,
		field_len + ONSCENE_RECEIVED_DIFF, output->arg1sz + end, 0,
		output->arg1sz - 1, output->arg1sz + end - 1);

	output->arg1sz += i;
	#endif

	#ifdef FULL
	i = 0;
	while (arg2_dispatch_received_records[i] != NULL) {
		output->arg2->dispatch_received_diff[output->arg2sz + i] =
			copy_record(arg2_dispatch_received_records[i], fields);
		output->arg2->onscene_enroute_diff[output->arg2sz + i] =
			copy_record(arg2_onscene_enroute_records[i], fields);
		output->arg2->onscene_received_diff[output->arg2sz + i] =
			copy_record(arg2_onscene_received_records[i], fields);
		i++;
	}

	output->arg2sz += i;
	#endif

	#ifdef MERGE_BEFORE
	merge(tmp_records, output->arg2->dispatch_received_diff,
		field_len + DISPATCH_RECEIVED_DIFF, output->arg2sz + end, 0,
		output->arg2sz - 1, output->arg2sz + end - 1);

	merge(tmp_records, output->arg2->onscene_enroute_diff,
		field_len + ONSCENE_ENROUTE_DIFF, output->arg2sz + end, 0,
		output->arg2sz - 1, output->arg2sz + end - 1);

	merge(tmp_records, output->arg2->onscene_received_diff,
		field_len + ONSCENE_RECEIVED_DIFF, output->arg2sz + end, 0,
		output->arg2sz - 1, output->arg2sz + end - 1);

	output->arg2sz += i;
	#endif

	pthread_mutex_unlock(mutex);

	destroy_records(&records);
	destroy_records(&dispatch_received_records);
	destroy_records(&onscene_enroute_records);
	destroy_records(&onscene_received_records);

	destroy_records(&arg1_records);
	destroy_records(&arg1_dispatch_received_records);
	destroy_records(&arg1_onscene_enroute_records);
	destroy_records(&arg1_onscene_received_records);

	destroy_records(&arg2_records);
	destroy_records(&arg2_dispatch_received_records);
	destroy_records(&arg2_onscene_enroute_records);
	destroy_records(&arg2_onscene_received_records);

	destroy_tmp_records(tmp_records);
	tmp_records = NULL;
}

pthread_t th_create(Output *output, pthread_mutex_t *mutex, Field **fields,
	ThreadPart *part, int fd, char *subfield, char *arg1, char *arg2)
{
	pthread_t thid;

	ThreadData *thdata = malloc(sizeof(ThreadData));
	if (thdata == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	thdata->output = output;
	thdata->mutex = mutex;
	thdata->fields = fields;
	thdata->part = part;
	thdata->fd = fd;
	thdata->subfield = subfield;
	thdata->arg1 = arg1;
	thdata->arg2 = arg2;

	// We use a lot of stack space for our routines.
	size_t stacksz = 8192 * 1024;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, stacksz);

	pthread_create(&thid, &attr, th_job, thdata);

	// don't free: `thdata->output`
	// don't free: pthread_mutex_t *mutex;
	// don't free: &thdata->fields

	// xxx segfault on these, free after the resources are done - NOT here.
	/*
	free(thdata->part);
	thdata->part = NULL;

	free(thdata->subfield);
	free(thdata->arg1);
	free(thdata->arg2);

	thdata->subfield = NULL;
	thdata->arg1 = NULL;
	thdata->arg2 = NULL;

	free(thdata);
	thdata = NULL;
	*/

	return thid;
}

Output *th_main(Field **fields, char *fh, int num_threads, char *subfield,
	char *arg1, char *arg2)
{
	// Open the `.dat` file in the main thread and get the fd.
	int fd = open(fh, O_RDONLY | O_NOATIME);
	if (fd == -1) {
		err(EXIT_FAILURE, "open");	// fatal
	}

	int fsize = fdsize(fd);

	int field_len = get_record_len(fields);
	ThreadPart **parts = create_thparts(fsize, num_threads, field_len);

	// we're losing a free here, free in output.c
	Output *output = create_output(parts[0]->recnum);

	output->fields = fields;

	// recnum is the same for each index of parts.
	//output->total->dispatch_received_diff = create_records(0, parts[0]->recnum);
	//output->total->onscene_enroute_diff = create_records(0, parts[0]->recnum);
	//output->total->onscene_received_diff = create_records(0, parts[0]->recnum);

	//output->arg1->dispatch_received_diff = create_records(0, parts[0]->recnum);
	//output->arg1->onscene_enroute_diff = create_records(0, parts[0]->recnum);
	//output->arg1->onscene_received_diff  = create_records(0, parts[0]->recnum);

	//output->arg2->dispatch_received_diff = create_records(0, parts[0]->recnum);
	//output->arg2->onscene_enroute_diff = create_records(0, parts[0]->recnum);
	//output->arg2->onscene_received_diff = create_records(0, parts[0]->recnum);

	//output->totalsz = 0;
	//output->arg1sz = 0;
	//output->arg2sz = 0;

	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	pthread_t thids[num_threads];
	for (int i = 0; i < num_threads; i++) {
		thids[i] = th_create(output, &mutex, fields, parts[i], fd, subfield,
			arg1, arg2);
	}

	for (int i = 0; i < num_threads; ++i) {
		pthread_join(thids[i], NULL);
	}

	#ifdef DEBUG
	size_t i = 0;
	int amt = get_field_amt(fields);
	int index = amt + DISPATCH_RECEIVED_DIFF;
	while (output->total->dispatch_received_diff[i] != NULL) {
		printf("th_main:\toutput->total->dispatch_received_diff[%zu]:\t%ld\n",
			i,
			output->total->dispatch_received_diff[i]->
			entries[index]->val->data.sec);
		i++;
	}
	#endif

	const int INDEX = get_field_amt(fields);

	int drdiff = INDEX + DISPATCH_RECEIVED_DIFF;

	Record **tmp_records = create_records(0, parts[0]->recnum);
	merge_sort(tmp_records, output->total->dispatch_received_diff, drdiff,
		parts[0]->recnum, 0, parts[0]->recnum - 1);

	destroy_thparts(&parts, num_threads);

	return output;
}

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
