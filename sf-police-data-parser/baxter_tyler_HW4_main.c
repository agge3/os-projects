/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_main.c
*
* Description:: This program’s purpose is to analyze police response times in
* San Francisco neighborhoods obtained from parsing a fixed-length record
* dataset (Law500K.dat). This program uses multithreading implemented by POSIX
* pthreads to process data partitions concurrently and then merge the results
* into a final output. The final output displays tabularized statistical
* calculations of the dataset, with a total, a user-provided subfield, and two
* neighborhoods/districts respectively. Performance of this program is analyzed
* when executed with 1, 2, 4, and 8 threads.
*
**************************************************************/

#include "baxter_tyler_HW4_core.h"
#include "baxter_tyler_HW4_alloc.h"
#include "baxter_tyler_HW4_sort.h"
#include "baxter_tyler_HW4_thread.h"
#include "baxter_tyler_HW4_test.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#ifdef TEST
int main (int argc, char *argv[])
{
	//TEST_CreateFields();
	//TEST_ParseRecords();
	//TEST_MergeSort();
	
	//TEST_MultiMergeSort();
	//TEST_ThreadPart();
	//TEST_CopyRecords();
	
	//TEST_Threads();
	//TEST_FilterRecords();
	
	//TEST_Output();

	return 0;
}

#else
int main (int argc, char *argv[])
	{
	if (argc != 7) {
		fprintf(stdout, "Usage: %s <data_filename> <header_filename> <threads> "
			"<subfield> <subfield_value1> <subfield_value2>\n", argv[0]);
	}

	char *data_filename = argv[1];
	char *header_filename = argv[2];
	int threads = strtoindex(argv[3]);
	char *subfield = argv[4];
	char *subfield_value1 = argv[5];
	char *subfield_value2 = argv[6];

	Field **fields = create_fields(header_filename);

	//**************************************************************
	// DO NOT CHANGE THIS BLOCK
	//Time stamp start
	struct timespec startTime;
	struct timespec endTime;

	clock_gettime(CLOCK_REALTIME, &startTime);
	//**************************************************************
	
	Output *output = th_main(fields, data_filename, threads, subfield,
		subfield_value1, subfield_value2);

	MapPair *pair = process_output(output);

	print_output(pair, subfield_value1, subfield_value2);

	//**************************************************************
	// DO NOT CHANGE THIS BLOCK
	//Clock output
	clock_gettime(CLOCK_REALTIME, &endTime);
	time_t sec = endTime.tv_sec - startTime.tv_sec;
	long n_sec = endTime.tv_nsec - startTime.tv_nsec;
	if (endTime.tv_nsec < startTime.tv_nsec)
		{
		--sec;
		n_sec = n_sec + 1000000000L;
		}

	printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
	//**************************************************************
	destroy_output(output);
	}
#endif

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
