/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_sort.c
*
* Description:: Sorting procedures implementation.
*
**************************************************************/

#include "baxter_tyler_HW4_sort.h"

void merge(Record **tmp_records, Record **records, int key, int size, int begin, int mid, int end)
{
	int begin_side = begin;
	int begin_mid = mid;
	int end_mid = mid + 1;
	int end_side = end;

	int idx = begin_side;
	while (begin_side <= begin_mid && end_mid <= end_side) {
		#ifdef DEBUG
		printf("merge:\tFirst loop:\tBEGIN\n");
		printf("merge:\tkey\t%d\n", key);
		printf("merge:\tidx:\t%d\n", idx);
		printf("merge:\tbegin_side:\t%d\n", begin_side);
		printf("merge:\tend_side:\t%d\n", end_side);
		printf("merge:\tbegin_mid:\t%d\n", begin_mid);
		printf("merge:\tend_mid:\t%d\n", end_mid);
		printf("merge:\trecords[%d]->entries[%d]->val->data.sec:\t%ld\n",
			begin_side, key,
			records[begin_side]->entries[key]->val->data.sec
		);
		printf("merge:\trecords[%d]->entries[%d]->val->data.sec:\t%ld\n",
			end_side, key,
			records[end_side]->entries[key]->val->data.sec
		);
		printf("merge:\tFirst loop:\tEND\n");
		#endif

		if (records[begin_side]->entries[key]->val->data.sec <=
			records[end_mid]->entries[key]->val->data.sec) {
			tmp_records[idx] = records[begin_side];
			idx++;
			begin_side++;
		} else {
			tmp_records[idx] = records[end_mid];
			idx++;
			end_mid++;
		}
	}
	
	while (begin_side <= begin_mid) {
		#ifdef DEBUG
		printf("merge:\tSecond loop:\tBEGIN\n");
		printf("merge:\tkey\t%d\n", key);
		printf("merge:\tidx:\t%d\n", idx);
		printf("merge:\tbegin_side:\t%d\n", begin_side);
		printf("merge:\tend_side:\t%d\n", end_side);
		printf("merge:\tbegin_mid:\t%d\n", begin_mid);
		printf("merge:\tend_mid:\t%d\n", end_mid);
		printf("merge:\trecords[%d]->entries[%d]->val->data.sec:\t%ld\n",
			begin_side, key,
			records[begin_side]->entries[key]->val->data.sec
		);
		printf("merge:\trecords[%d]->entries[%d]->val->data.sec:\t%ld\n",
			end_side, key,
			records[end_side]->entries[key]->val->data.sec
		);
		printf("merge:\tSecond loop:\tEND\n");
		#endif

		tmp_records[idx] = records[begin_side];
		idx++;
		begin_side++;
	}

	while (end_mid <= end_side) {
		#ifdef DEBUG
		printf("merge:\tThird loop:\tBEGIN\n");
		printf("merge:\tkey\t%d\n", key);
		printf("merge:\tidx:\t%d\n", idx);
		printf("merge:\tbegin_side:\t%d\n", begin_side);
		printf("merge:\tend_side:\t%d\n", end_side);
		printf("merge:\tbegin_mid:\t%d\n", begin_mid);
		printf("merge:\tend_mid:\t%d\n", end_mid);
		printf("merge:\trecords[%d]->entries[%d]->val->data.sec:\t%ld\n",
			begin_side, key,
			records[begin_side]->entries[key]->val->data.sec
		);
		printf("merge:\trecords[%d]->entries[%d]->val->data.sec:\t%ld\n",
			end_side, key,
			records[end_side]->entries[key]->val->data.sec
		);
		printf("merge:\tThird loop:\tEND\n");
		#endif

		tmp_records[idx] = records[end_mid];
		idx++;
		end_mid++;
	}

	for (int i = 0; i < size; ++i) {
		#ifdef DEBUG
		printf("merge:\ttmp_records[%d]->entries[%d]->val->data.sec:\t%ld\n",
			i, key, tmp_records[i]->entries[key]->val->data.sec);
		#endif

		records[i] = tmp_records[i];
	}
}

void merge_sort(Record **tmp_records, Record **records, int key, int size, int begin, int end)
{
	if (begin < end) {
		int mid = (begin + end) / 2;
		merge_sort(tmp_records, records, key, size, begin, mid);
		merge_sort(tmp_records, records, key, size, mid + 1, end);

		merge(tmp_records, records, key, size, begin, mid, end);
	}

	// xxx doesn't need to repopulate fin_records.
	for (int i = 0; i < size; ++i) {
		tmp_records[i] = records[i];
	}
}

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
