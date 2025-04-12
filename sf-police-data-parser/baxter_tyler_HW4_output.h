/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_output.h
*
* Description:: Final output interface. print_output produces the final output.
*
**************************************************************/

#ifndef OUTPUT_H
#define OUTPUT_H

#include "baxter_tyler_HW4_core.h"
#include "baxter_tyler_HW4_alloc.h"
#include "baxter_tyler_HW4_sort.h"
#include "baxter_tyler_HW4_thread.h"

#define CALL_MAP_SZ 1000
#define CALL_MAPS_SZ 3

/**
 * @struct Result
 * Holds final calculated Results.
 */
typedef struct Result {
	int count;
	int min;
	int LB;
	float Q1;
	int med;
	float mean;
	float Q3;
	int UB;
	int max;
	int IQR;
	float stddev;
} Result;

/**
 * @struct OutputMapEntry
 * Holds a copy of each Record subsection and the Results for each subsection.
 */
typedef struct OutputMapEntry
{
	Record **drrec;
	Result *drrec_res;
	Record **oerec;
	Result *oerec_res;
	Record **orrec;
	Result *orrec_res;
} OutputMapEntry;

/**
 * @struct OutputMap
 * Holds an OutputMapEntry for each field.
 */
typedef struct OutputMap
{
	OutputMapEntry **total;
	OutputMapEntry **arg1;
	OutputMapEntry **arg2;
} OutputMap;

/**
 * @struct MapPair
 * A struct for holding an OutputMap and a call type map as a pair.
 */
typedef struct MapPair {
	OutputMap **output_map;
	char ***call_map;
} MapPair;

/**
 * Destroy a map pair that holds an OutputMap and a call type map.
 */
void destroy_map_pair(MapPair *map_pair);

/**
 * Process a result for this subsection of Records.
 */
Result *process_result(Record **rec, int key, int sz);

/**
 * Create a map of each call type.
 */
char ***create_call_map(Output *output);

/**
 * Create a map of the expected output rows, from a map of call type.
 */
OutputMap **create_output_map(char ***call_map);

/**
 * Preprocess an output from parsing and prepare it for final output.
 */
MapPair *process_output(Output *output);

/**
 * Prints the header for each output section.
 */
void print_header();

/**
 * Prints a row of each output section.
 *
 * @param const char *call_type		The call type of this row.
 * @param Result *res				The results for that call type.
 */
void print_row(const char *call_type, Result *res);

/**
 * Prints the final output.
 */
void print_output(MapPair *map_pair, char *arg1, char *arg2);

#endif // OUTPUT_H

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
