/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_test.c
*
* Description:: Test suite implementation.
*
**************************************************************/

#include "baxter_tyler_HW4_test.h"
#include "baxter_tyler_HW4_core.h"
#include "baxter_tyler_HW4_alloc.h"
#include "baxter_tyler_HW4_sort.h"
#include "baxter_tyler_HW4_thread.h"
#include "baxter_tyler_HW4_output.h"

void TEST_Output()
{
	printf("TEST_Output:\tBEGIN\n");

	Field **fields = create_fields("header.txt");

	Output *output = th_main(fields, "Law50K.dat", 2, "analysis_neighborhood",
		"Tenderloin", "Nob Hill");

	printf("TEST_Output:\tprocess_output:\tBEGIN\n");
	MapPair *pair = process_output(output);
	printf("TEST_Output:\tprocess_output:\tEND\n");


	printf("TEST_Output:\tprint_output:\tBEGIN\n");
	print_output(pair, "Tenderloin", "Nob Hill");
	printf("TEST_Output:\tprint_output:\tEND\n");

	destroy_output(output);
	destroy_fields(&fields);

	printf("TEST_Output:\tEND\n");
}

/**
 * Test internal API create_fields.
 */
void TEST_CreateFields()
{
	printf("TEST_CreateFields:\tBEGIN\n");

	Field **fields = create_fields("header.txt");

	size_t i = 0;
	while (fields[i] != NULL) {
		printf("fields[%zu] index:\t%d\n", i,
			fields[i]->index);
		printf("fields[%zu] entry:\t%s\n", i,
			fields[i]->entry);
		i++;
	}

	assert(destroy_fields(&fields) == 0);
	assert(fields == NULL);

	printf("TEST_CreateFields:\tEND\n");
}

/**
 * @warning Deprecated due to API change. Must be refactored to follow new API.
 */
void TEST_ParseRecords()
{
	printf("TEST_ParseRecords:\tBEGIN\n");

	int fd = open("Law50K.dat", O_RDONLY | O_NOATIME);
	if (fd == -1) {
		err(EXIT_FAILURE, "open");	// fatal
	}

	Field **fields = create_fields("header.txt");
	int len = get_record_len(fields);

	Record **records = create_records(0, 50000);	
	parse_records2(records, fields, len, fd, 0, 50000);

	const int MAX = get_field_amt(fields);
	print_records(records, MAX + 6);

	assert(destroy_records(&records) == 0);
	assert(records == NULL);

	assert(destroy_fields(&fields) == 0);
	assert(fields == NULL);

	printf("TEST_ParseRecords:\tEND\n");
}

void TEST_Threads()
{
	const int LAW5K_SZ = 3425000;
	const int LAW50K_SZ = 34250000;

	Field **fields = create_fields("header.txt");

	th_main(fields, "Law5K.dat", 2, "analysis_neighborhood", "Tenderloin",
		"Nob Hill");
}

/**
 * @warning Deprecated due to API change. Must be refactored to follow new API.
 */
void TEST_MergeSort()
{
	printf("TEST_MergeSort:\tBEGIN\n");

	int fd = open("Law50K.dat", O_RDONLY | O_NOATIME);
	if (fd == -1) {
		err(EXIT_FAILURE, "open");
	}

	int fdsz = fdsize(fd);

	Field **fields = create_fields("header.txt");
	int len = get_record_len(fields);
	int amt = fdsz / len;

	Record **records = create_records(0, amt);
	parse_records2(records, fields, len, fd, 0, amt);

	int index = get_field_amt(fields);
	index += DISPATCH_RECEIVED_DIFF;
	printf("TEST_MergeSort:\tindex:\t%d\n", index);

	Record **tmp_records = create_records(0, fdsz);
	merge_sort(tmp_records, records, index, amt, 0, amt - 1);

	size_t i = 0;
	//Record **test_records = filter_records(fin_records, fields, NULL, -1, 0, 10);
	while (records[i] != NULL) {
		printf("TEST_MergeSort:\ti:\t%zu\n", i);
		printf("TEST_MergeSort:\t%s:\t%ld\n", records[i]->entries[index]->key,
			records[i]->entries[index]->val->data.sec);
		i++;
	}

	assert(destroy_fields(&fields) == 0);
	assert(fields == NULL);

	assert(destroy_records(&records) == 0);
	assert(records == NULL);

	// xxx fin_records is not freeing correctly, review destruction routine.
	//assert(destroy_records(&fin_records) == 0);
	//assert(fin_records == NULL);

	printf("TEST_MergeSort:\tEND\n");
}

void TEST_MultiMergeSort()
{
	printf("TEST_MultiMergeSort:\tBEGIN\n");

	int fd = open("Law5K.dat", O_RDONLY | O_NOATIME);
	if (fd == -1) {
		err(EXIT_FAILURE, "open");
	}

	Field **fields = create_fields("header.txt");
	int len = get_record_len(fields);

	int index = get_field_amt(fields);
	index += DISPATCH_RECEIVED_DIFF;
	printf("TEST_MergeSort:\tindex:\t%d\n", index);

	const int FHSZ = 342500;
	printf("TEST_MultiMergeSort:\tFHSZ:\t%d\n", FHSZ);

	const int RECORDSZ = 685;

	int num_records = FHSZ / RECORDSZ;

	int th1 = num_records / 4;
	int th2 = 2 * th1;
	int th3 = 3 * th1;
	int th4 = num_records;

	printf("TEST_MultiMergeSort:\tth1:\t%d\n", th1);
	printf("TEST_MultiMergeSort:\tth2:\t%d\n", th2);
	printf("TEST_MultiMergeSort:\tth3:\t%d\n", th3);
	printf("TEST_MultiMergeSort:\tth4:\t%d\n", th4);

	int th1_begin = 0;
	int th1_end = th1 - 1;
	int th1sz = th1_end - th1_begin + 1;

	int	th2_begin = th1;
	int	th2_end = th2 - 1;
	int	th2sz = th2_end - th2_begin + 1;

	int th3_begin = th2;
	int th3_end = th3 - 1;
	int th3sz = th3_end - th3_begin + 1;

	int th4_begin = th3;
	int th4_end = th4 - 1;
	int th4sz = th4_end - th4_begin + 1;

	printf("TEST_MultiMergeSort:\tth1_begin:\t%d\n", th1_begin);
	printf("TEST_MultiMergeSort:\tth2_begin:\t%d\n", th2_begin);
	printf("TEST_MultiMergeSort:\tth3_begin:\t%d\n", th3_begin);
	printf("TEST_MultiMergeSort:\tth4_begin:\t%d\n", th4_begin);

	printf("TEST_MultiMergeSort:\tth1_end:\t%d\n", th1_end);
	printf("TEST_MultiMergeSort:\tth2_end:\t%d\n", th2_end);
	printf("TEST_MultiMergeSort:\tth3_end:\t%d\n", th3_end);
	printf("TEST_MultiMergeSort:\tth4_end:\t%d\n", th4_end);

	printf("TEST_MultiMergeSort:\tth1sz:\t%d\n", th1sz);
	printf("TEST_MultiMergeSort:\tth2sz:\t%d\n", th2sz);
	printf("TEST_MultiMergeSort:\tth3sz:\t%d\n", th3sz);
	printf("TEST_MultiMergeSort:\tth4sz:\t%d\n", th4sz);

	Record **th1_records = create_records(0, th1sz);
	Record **th1_tmp_records = create_records(0, num_records);

	parse_records2(th1_records, fields, len, fd, th1_begin, th1_end + 1);

	printf("TEST_MultiMergeSort:\ti:\t%d\n", 0);
	printf("TEST_MultiMergeSort:\t%s:\t%ld\n",
		th1_records[0]->entries[index]->key,
		th1_records[0]->entries[index]->val->data.sec);

	printf("TEST_MultiMergeSort:\ti:\t%d\n", 1);
	printf("TEST_MultiMergeSort:\t%s:\t%ld\n",
		th1_records[1]->entries[index]->key,
		th1_records[1]->entries[index]->val->data.sec);

	merge_sort(th1_tmp_records, th1_records, index, th1sz, th1_begin, th1_end);

	printf("TEST_MultiMergeSort:\ti:\t%d\n", 20);
	printf("TEST_MultiMergeSort:\t%s:\t%ld\n",
		th1_records[20]->entries[index]->key,
		th1_records[20]->entries[index]->val->data.sec);

	printf("TEST_MultiMergeSort:\ti:\t%d\n", 21);
	printf("TEST_MultiMergeSort:\t%s:\t%ld\n",
		th1_records[21]->entries[index]->key,
		th1_records[21]->entries[index]->val->data.sec);

	Record **th2_records = create_records(0, th2sz);
	Record **th2_tmp_records = create_records(0, num_records);

	parse_records2(th2_records, fields, len, fd, th2_begin, th2_end + 1);
	merge_sort(th2_tmp_records, th2_records, index, th2sz, 0, th1_end);

	Record **th3_records = create_records(0, th1sz);
	Record **th3_tmp_records = create_records(0, num_records);

	parse_records2(th3_records, fields, len, fd, th3_begin, th3_end + 1);
	merge_sort(th3_tmp_records, th3_records, index, th3sz, 0, th1_end);

	Record **th4_records = create_records(0, th1sz);
	Record **th4_tmp_records = create_records(0, num_records);

	parse_records2(th4_records, fields, len, fd, th4_begin, th4_end + 1);
	merge_sort(th4_tmp_records, th4_records, index, th4sz, 0, th1_end);

	size_t i = 0;
	while (th1_records[i] != NULL) {
		printf("TEST_MultiMergeSort:\ti:\t%zu\n", i);
		printf("TEST_MultiMergeSort:\t%s:\t%ld\n",
			th1_records[i]->entries[index]->key,
			th1_records[i]->entries[index]->val->data.sec);
		i++;
	}

	i = 0;
	while (th2_records[i] != NULL) {
		printf("TEST_MultiMergeSort:\ti:\t%zu\n", i);
		printf("TEST_MultiMergeSort:\t%s:\t%ld\n",
			th2_records[i]->entries[index]->key,
			th2_records[i]->entries[index]->val->data.sec);
		i++;
	}

	i = 0;
	while (th3_records[i] != NULL) {
		printf("TEST_MultiMergeSort:\ti:\t%zu\n", i);
		printf("TEST_MultiMergeSort:\t%s:\t%ld\n",
			th3_records[i]->entries[index]->key,
			th3_records[i]->entries[index]->val->data.sec);
		i++;
	}

	i = 0;
	while (th4_records[i] != NULL) {
		printf("TEST_MultiMergeSort:\ti:\t%zu\n", i);
		printf("TEST_MultiMergeSort:\t%s:\t%ld\n",
			th4_records[i]->entries[index]->key,
			th4_records[i]->entries[index]->val->data.sec);
		i++;
	}

	assert(destroy_fields(&fields) == 0);
	assert(fields == NULL);

	assert(destroy_records(&th1_records) == 0);
	assert(destroy_records(&th2_records) == 0);
	assert(destroy_records(&th3_records) == 0);
	assert(destroy_records(&th4_records) == 0);

	assert(th1_records == NULL);
	assert(th2_records == NULL);
	assert(th3_records == NULL);
	assert(th4_records == NULL);

	// xxx fin_records is not freeing correctly, review destruction routine.
	//assert(destroy_records(&fin_records) == 0);
	//assert(fin_records == NULL);

	printf("TEST_MultiMergeSort:\tEND\n");
}

void TEST_CopyRecords()
{
	printf("TEST_MultiMergeSort:\tBEGIN\n");

	int fd = open("Law5K.dat", O_RDONLY | O_NOATIME);
	if (fd == -1) {
		err(EXIT_FAILURE, "open");
	}

	Field **fields = create_fields("header.txt");
	int len = get_record_len(fields);

	int index = get_field_amt(fields);
	index += DISPATCH_RECEIVED_DIFF;
	printf("TEST_CopyRecords:\tindex:\t%d\n", index);

	const int FHSZ = 342500;
	printf("TEST_CopyRecords:\tFHSZ:\t%d\n", FHSZ);

	const int RECORDSZ = 685;

	int num_records = (FHSZ / RECORDSZ) / 2;

	Record **records = create_records(0, num_records);
	Record **tmp_records = create_records(0, num_records * 2);

	parse_records2(records, fields, len, fd, 0, num_records);

	merge_sort(tmp_records, records, index, num_records, 0, num_records - 1);

	Record **new_records = create_records(0, num_records);
	
	size_t i = 0;
	while (records[i] != NULL) {
		new_records[i] = copy_record(records[i], fields);
		i++;
	}

	i = 0;
	while (new_records[i] != NULL) {
		assert(
			new_records[i]->entries[index]->val->data.sec ==
			records[i]->entries[index]->val->data.sec
		);
		i++;
	}

	Record **records2 = create_records(0, num_records);
	parse_records2(records2, fields, len, fd, num_records, num_records * 2);
	merge_sort(tmp_records, records, index, num_records, 0, num_records - 1);

	Record **out_records = create_records(0, num_records * 2);
	i = 0;
	while (new_records[i] != NULL) {
		out_records[i] = copy_record(new_records[i], fields);
		i++;
	}

	size_t totalsz = i;

	i = 0;
	while (records2[i] != NULL) {
		out_records[totalsz + i] = copy_record(records2[i], fields);
		i++;
	}

	merge(tmp_records, out_records, index, num_records * 2, 0, num_records - 1,
		num_records * 2 - 1);

	i = 0;
	while (out_records[i] != NULL) {
		// xxx merge is failing here.
		printf("TEST_CopyRecords:\tout_records[%zu]:\t%ld\n",
			i, out_records[i]->entries[index]->val->data.sec);
		if (i > 0) {
			assert(
				out_records[i]->entries[index]->val->data.sec >=
				out_records[i - 1]->entries[index]->val->data.sec
			);
		}
		i++;
	}

	destroy_fields(&fields);

	destroy_records(&records);
	destroy_records(&new_records);
	destroy_records(&records2);
	destroy_records(&out_records);
}

/**
 * @warning Deprecated due to API change. Must be refactored to follow new API.
 */
void TEST_FilterRecords()
{
	printf("TEST_FilterRecords:\tBEGIN\n");

	int fd = open("Law5K.dat", O_RDONLY | O_NOATIME);
	if (fd == -1) {
		err(EXIT_FAILURE, "open");
	}

	Field **fields = create_fields("header.txt");
	int len = get_record_len(fields);

	Record **records = create_records(0, 5000);
	parse_records2(records, fields, len, fd, 0, 5000);

	const int INDEX = get_field_amt(fields);
	int time_idx = INDEX + DISPATCH_RECEIVED_DIFF;
	printf("TEST_FilterRecords:\ttime_idx:\t%d\n", time_idx);

	Record **fin_records = create_records(0, 5000);
	// xxx we don't need to sort here, just takes extra time.
	//merge_sort(fin_records, records, time_idx, 5000, 0, 4999);

	size_t i = 0;
	while (records[i] != NULL) {
		//printf("TEST_FilterRecords:\ti:\t%zu\n", i);
		//printf("TEST_FilterRecords:\t%s:\t%ld\n",
		//	records[i]->entries[time_idx]->key,
		//	records[i]->entries[time_idx]->val->data.sec);
		i++;
	}

	printf("TEST_FilterRecords:\tfilter_records:\tBEGIN\n");
	int fil_idx = INDEX + POST_ANALYSIS_NEIGHBORHOOD;
	Record **filrecords =
		filter_records(records, fields, "Tenderloin", fil_idx, 0, 5000, NULL);
	printf("TEST_FilterRecords:\tfilter_records:\tEND\n");

	printf("TEST_FilterRecords:\trecords[i] != NULL:\tBEGIN\n");
	i = 0;
	while (records[i] != NULL) {
		if (strcmp(records[i]->entries[fil_idx]->val->data.str,
			"Tenderloin") == 0) {
			printf("TEST_FilterRecords:\trecords[%zu] key:\t%s\n", i,
				records[i]->entries[fil_idx]->val->data.str);
		}
		i++;
	}
	printf("TEST_FilterRecords:\trecords[i] != NULL:\tEND\n");

	i = 0;
	while (records[i] != NULL) {
		//printf("TEST_FilterRecords:\trecords[%zu] key:\t%s\n", i,
		//	records[i]->entries[fil_idx]->val->data.str);
		i++;
	}

	printf("TEST_FilterRecords:\tfilrecords[i] != NULL:\tBEGIN\n");
	i = 0;
	while (filrecords[i] != NULL) {
		printf("TEST_FilterRecords:\tfilrecords[%zu] key:\t%s\n", i,
			filrecords[i]->entries[fil_idx]->val->data.str);
		i++;
	}
	printf("TEST_FilterRecords:\tfilrecords[i] != NULL:\tEND\n");

	assert(destroy_fields(&fields) == 0);
	assert(fields == NULL);

	assert(destroy_records(&records) == 0);
	assert(records == NULL);

	assert(destroy_records(&filrecords) == 0);
	assert(filrecords == NULL);

	printf("TEST_FilterRecords:\tEND\n");
}

void TEST_ThreadPart()
{
	const int FHSZ = 342500;
	const int RECORDSZ = 685;
	const int thnum = 4;

	int recnum = FHSZ / RECORDSZ;

	ThreadPart **parts = create_thparts(FHSZ, thnum, RECORDSZ);

	assert(parts[0]->sz == 125);
	assert(parts[1]->sz == 125);
	assert(parts[2]->sz == 125);
	assert(parts[3]->sz == 125);

	assert(parts[0]->begin == 0);
	assert(parts[1]->begin == 125);
	assert(parts[2]->begin == 250);
	assert(parts[3]->begin == 375);

	assert(parts[0]->end == 124);
	assert(parts[1]->end == 249);
	assert(parts[2]->end == 374);
	assert(parts[3]->end == 499);

	assert(parts[0]->recnum == 500);
	assert(parts[1]->recnum == 500);
	assert(parts[2]->recnum == 500);
	assert(parts[3]->recnum == 500);
}

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
