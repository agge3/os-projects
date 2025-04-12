/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_sort.h
*
* Description:: Sorting procedures for sorting record data structures.
*
**************************************************************/

#ifndef SORT_H
#define sORT_H

#include "baxter_tyler_HW4_core.h"

/**
 * merge_sort a Record vector.
 */
void merge_sort(Record **tmp_records, Record **records, int key, int size, int begin, int end);

/**
 * merge for merge_sort and merging on thread join.
 */
void merge(Record **tmp_records, Record **records, int key, int size, int begin, int mid, int end);

#endif

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
