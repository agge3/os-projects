/**************************************************************
* Class::  CSC-415-01 Spring 2025
* Name:: Tyler Baxter
* GitHub-Name:: agge3
* Project:: Assignment 4 – Processing FLR Data with Threads
*
* File:: baxter_tyler_HW4_output.c
*
* Description:: Final output implementation. print_output produces the final
* output.
*
**************************************************************/

#include "baxter_tyler_HW4_output.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CUSTOM_DEALLOC

void destroy_map_pair(MapPair *map_pair)
{
	OutputMap **output_map = map_pair->output_map;
	char ***call_map = map_pair->call_map;

	#ifdef CUSTOM_DEALLOC
	size_t i = 0;
	while (i < CALL_MAPS_SZ) {
		size_t j = 0;
		while (call_map[i][j] != NULL ) {
			const int ARRSZ = 9;
			Record **recarr[ARRSZ];

			recarr[0] = output_map[i]->total[j]->drrec;
			recarr[1] = output_map[i]->total[j]->oerec;
			recarr[2] = output_map[i]->total[j]->orrec;

			recarr[3] = output_map[i]->arg1[j]->drrec;
			recarr[4] = output_map[i]->arg1[j]->oerec;
			recarr[5] = output_map[i]->arg1[j]->orrec;

			recarr[6] = output_map[i]->arg2[j]->drrec;
			recarr[7] = output_map[i]->arg2[j]->oerec;
			recarr[8] = output_map[i]->arg2[j]->orrec;

			size_t k = 0;
			while (k < ARRSZ) {
				if (recarr[k] == NULL) {
					k++;
					continue;
				}

				destroy_records(&recarr[i]);
				k++;
			}
			j++;
		}
		i++;
	}


	//while ((*record)->entries[i] != NULL) {
	//	if ((*record)->entries[i]->key != NULL) {
	//		free((*record)->entries[i]->key);
	//		(*record)->entries[i]->key = NULL;
	//	}
	//	
	//	if ((*record)->entries[i]->val != NULL) {
	//		if ((*record)->entries[i]->val->type == STRING &&
	//			(*record)->entries[i]->val->data.str != NULL) {
	//			free((*record)->entries[i]->val->data.str);
	//			(*record)->entries[i]->val->data.str = NULL;
	//		}
	//	
	//		free((*record)->entries[i]->val);
	//		(*record)->entries[i]->val = NULL;
	//	}

	//	free((*record)->entries[i]);
	//	(*record)->entries[i] = NULL;

	//	i++;
	//}

	//free((*record)->entries);
	//(*record)->entries = NULL;

	//free(*record);
	//*record = NULL;

	//return 0;


	#else

	size_t i = 0;
	while (i < CALL_MAPS_SZ) {
		size_t j = 0;
		while (call_map[i][j] != NULL) {
			// Could be NULL.
			if (output_map[i]->total[j]->drrec != NULL) {
				destroy_records(&output_map[i]->total[j]->drrec);
			}

			destroy_records(&output_map[i]->total[j]->oerec);
			destroy_records(&output_map[i]->total[j]->orrec);

			#ifdef DEBUG
			if (&output_map[i]->arg1[j]->drrec[0] == NULL) {
				printf("destroy_map_pair:\tdestroy_records invalid destroy\n");
			}
			#endif
				
			destroy_records(&output_map[i]->arg1[j]->drrec);
			destroy_records(&output_map[i]->arg1[j]->oerec);
			destroy_records(&output_map[i]->arg1[j]->orrec);

			destroy_records(&output_map[i]->arg2[j]->drrec);
			destroy_records(&output_map[i]->arg2[j]->oerec);
			destroy_records(&output_map[i]->arg2[j]->orrec);

			free(output_map[i]->total[j]->drrec_res);
			free(output_map[i]->arg1[j]->drrec_res);
			free(output_map[i]->arg2[j]->drrec_res);

			free(output_map[i]->total[j]->oerec_res);
			free(output_map[i]->arg1[j]->oerec_res);
			free(output_map[i]->arg2[j]->oerec_res);

			free(output_map[i]->total[j]->orrec_res);
			free(output_map[i]->arg1[j]->oerec_res);
			free(output_map[i]->arg2[j]->orrec_res);

			output_map[i]->total[j]->drrec_res	= NULL;
			output_map[i]->total[j]->oerec_res	= NULL;
			output_map[i]->total[j]->orrec_res	= NULL;

			output_map[i]->arg1[j]->drrec_res	= NULL;
			output_map[i]->arg1[j]->oerec_res	= NULL;
			output_map[i]->arg1[j]->orrec_res	= NULL;

			output_map[i]->arg2[j]->drrec_res	= NULL;
			output_map[i]->arg2[j]->oerec_res	= NULL;
			output_map[i]->arg2[j]->orrec_res	= NULL;

			j++;
		}
		i++;
	}
	#endif

	i = 0;
	while (i < CALL_MAPS_SZ) {
		size_t j = 0;
		while (call_map[i][j] != NULL) {
			free(call_map[i][j]);
			call_map[i][j] = NULL;
			j++;
		}

		free(call_map[i]);
		call_map[i] = NULL;
		i++;
	}
}

Result *process_result(Record **rec, int key, int sz)
{
	Result *res = malloc(sizeof(Result));
	if (res == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	// Default initialize everything as zero. We can also return out early if
	// there's no elements to perform calculations on, and trust everything is
	// zeroed out correctly.
	res->count = 0;
	res->min = 0;
	res->LB = 0;
	res->Q1 = 0.0f;
	res->med = 0;
	res->mean = 0.0f;
	res->Q3 = 0.0f;
	res->UB = 0;
	res->max = 0;
	res->IQR = 0;
	res->stddev = 0.0f;

	// Empty Records vector, return default initialized Result.
	if (rec == NULL || sz == 0 || *rec == NULL) {
		return res;
	}

	time_t orig_secs[sz];
	float sum = 0.0f;

	size_t i = 0;
	while (i < (size_t)sz && rec[i] != NULL) {
		time_t sec = rec[i]->entries[key]->val->data.sec;
		// Filter out invalid times.
		if (sec < 0) {
			i++;
			continue;
		}

		orig_secs[res->count] = sec;
		sum += (float)sec;

		res->count++;
		i++;
	}

	// i is less verbose than res->count; i will be our count from here on out.
	i = res->count;

	// We could potentially filter out invalid times earlier, but this works for
	// now. We need a correct size for our secs array. This is an insanely
	// optimized integer register op anyway.
	time_t secs[i];
	size_t j = 0;
	while (j < i) {
		secs[j] = orig_secs[j];
		j++;
	}

	// Zero elements, perform no calculations and leave everything default
	// initialized as zero.
	if (i < 1) {
		return res;
	}

	// One element, set everything to that one element and return res.
	if (i < 2) {
		res->min = secs[0];
		res->LB = secs[0];
		res->Q1 = (float)secs[0];
		res->med = secs[0];
		res->mean = (float)secs[0];
		res->Q3 = (float)secs[0];
		res->UB = secs[0];
		res->max = secs[0];
		res->IQR = secs[0];
		res->stddev = (float)secs[0];

		return res;
	}

	res->min = secs[0];
	res->max = secs[i - 1];
	res->mean = sum / i;

	// even median calculation:
	if (i % 2 == 0) {
		res->med = ((float)secs[i / 2 - 1] + (float)secs[i / 2]) / 2.0f;
	// odd median calculation:
	} else {
		res->med = (float)secs[i / 2];
	}

	// Q1 and Q3:
	int mid = i / 2;
	// even number of elements
	if (i % 2 == 0) {
		if (mid % 2 == 0) {
			res->Q1 = ((float)secs[mid / 2 - 1] + (float)secs[mid / 2]) / 2.0f;
		} else {
			res->Q1 = (float)secs[mid / 2];
		}
		
		int upper = i - mid;
		if (upper % 2 == 0) {
			res->Q3 = ((float)secs[upper / 2 + mid - 1] +
				(float)secs[upper / 2 + mid]) / 2.0f;
		} else {
			res->Q3 = (float)secs[upper / 2 + mid];
		}
	// odd number of elements
	} else {
		if (mid % 2 == 0) {
			#ifdef DEBUG
			printf("process_result:\tmid:\t%d\n", mid);
			#endif

			res->Q1 = ((float)secs[mid / 2 - 1] + (float)secs[mid / 2]) / 2.0f;
		} else {
			res->Q1 = (float)secs[mid / 2];
		}

		int upper = i - mid - 1;
		if (upper % 2 == 0) {
			res->Q3 = ((float)secs[upper / 2 + mid] +
				(float)secs[upper / 2 + mid + 1]) / 2.0f;
		} else {
			res->Q3 = (float)secs[upper / 2 + mid + 1];
		}
	}

	res->IQR = res->Q3 - res->Q1;
	res->LB = res->Q1 - (int)(1.5f * res->IQR);
	res->UB = res->Q3 + (int)(1.5f * res->IQR);

	float var = 0.0f;
	for (size_t j = 0; j < i; j++) {
		float diff = (float)secs[j] - res->mean;
		var += diff * diff;
	}
	var /= i;
	res->stddev = sqrtf(var);

	return res;
}

Result *process_result2(Record **rec, int key, int sz)
{
	Result *res = malloc(sizeof(Result));
	if (res == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	// Default initialize everything as zero.
	res->count = 0;
	res->min = 0;
	res->LB = 0;
	res->Q1 = 0.0f;
	res->med = 0;
	res->mean = 0.0f;
	res->Q3 = 0.0f;
	res->UB = 0;
	res->max = 0;
	res->IQR = 0;
	res->stddev = 0.0f;

	// Empty Records vector, return default initialized Result.
	if (rec == NULL || sz == 0 || *rec == NULL) {
		return res;
	}

	time_t orig_secs[sz];
	float sum = 0.0f;

	size_t i = 0;
	while (i < (size_t)sz && rec[i] != NULL) {
		time_t sec = rec[i]->entries[key]->val->data.sec;
		// Filter out invalid times.
		if (sec < 0) {
			i++;
			continue;
		}

		orig_secs[res->count] = sec;
		sum += (float)sec;

		res->count++;
		i++;
	}

	// Use res->count as our new i.
	i = res->count;

	// Create a correctly sized array.
	time_t secs[i];
	size_t j = 0;
	while (j < i) {
		secs[j] = orig_secs[j];
		j++;
	}

	// Zero elements: nothing to calculate.
	if (i < 1) {
		return res;
	}

	// One element: all statistics equal that one element.
	if (i < 2) {
		res->min = secs[0];
		res->LB = secs[0];
		res->Q1 = (float)secs[0];
		res->med = secs[0];
		res->mean = (float)secs[0];
		res->Q3 = (float)secs[0];
		res->UB = secs[0];
		res->max = secs[0];
		res->IQR = 0;
		res->stddev = 0.0f;
		return res;
	}

	// Calculate min, max, and mean.
	res->min = secs[0];
	res->max = secs[i - 1];
	res->mean = sum / i;

	// Median calculation.
	if (i % 2 == 0) {
		res->med = ((float)secs[i / 2 - 1] + (float)secs[i / 2]) / 2.0f;
	} else {
		res->med = (float)secs[i / 2];
	}

	// Q1 and Q3 calculation.
	int mid = i / 2;
	if (i % 2 == 0) {  // Even number of elements.
		if (mid % 2 == 0) {
			res->Q1 = ((float)secs[mid / 2 - 1] + (float)secs[mid / 2]) / 2.0f;
		} else {
			res->Q1 = (float)secs[mid / 2];
		}
		
		int upper = i - mid;
		if (upper % 2 == 0) {
			res->Q3 = ((float)secs[upper / 2 + mid - 1] +
					   (float)secs[upper / 2 + mid]) / 2.0f;
		} else {
			res->Q3 = (float)secs[upper / 2 + mid];
		}
	} else {  // Odd number of elements.
		if (mid % 2 == 0) {
#ifdef DEBUG
			printf("process_result:\tmid:\t%d\n", mid);
#endif
			res->Q1 = ((float)secs[mid / 2 - 1] + (float)secs[mid / 2]) / 2.0f;
		} else {
			res->Q1 = (float)secs[mid / 2];
		}

		int upper = i - mid - 1;
		if (upper % 2 == 0) {
			res->Q3 = ((float)secs[upper / 2 + mid] +
					   (float)secs[upper / 2 + mid + 1]) / 2.0f;
		} else {
			res->Q3 = (float)secs[upper / 2 + mid + 1];
		}
	}

	res->IQR = res->Q3 - res->Q1;
	res->LB = res->Q1 - (int)(1.5f * res->IQR);
	res->UB = res->Q3 + (int)(1.5f * res->IQR);

	// Clamp LB and UB so they are not negative.
	if (res->LB < 0) {
		res->LB = 0;
	}
	if (res->UB < 0) {
		res->UB = 0;
	}

	// Standard deviation calculation.
	float var = 0.0f;
	for (size_t j = 0; j < i; j++) {
		float diff = (float)secs[j] - res->mean;
		var += diff * diff;
	}
	var /= i;
	res->stddev = sqrtf(var);

	return res;
}

char ***create_call_map(Output *output)
{
	const int INDEX = get_field_amt(output->fields);
	int desc_idx = INDEX + FINAL_DESC;
	int dridx = INDEX + DISPATCH_RECEIVED_DIFF;
	int oeidx = INDEX + ONSCENE_ENROUTE_DIFF;
	int oridx = INDEX + ONSCENE_RECEIVED_DIFF;

	char **map = calloc(CALL_MAP_SZ, sizeof(char *));
	if (map == NULL) {
		err(EXIT_FAILURE, "calloc");
	}

	char **map1 = calloc(CALL_MAP_SZ, sizeof(char *));
	if (map1 == NULL) {
		err(EXIT_FAILURE, "calloc");
	}

	char **map2 = calloc(CALL_MAP_SZ, sizeof(char *));
	if (map2 == NULL) {
		err(EXIT_FAILURE, "calloc");
	}

	// We only need one of the sorted records for each section.
	Record **drrec = output->total->dispatch_received_diff;
	Record **drrec1 = output->arg1->dispatch_received_diff;
	Record **drrec2 = output->arg2->dispatch_received_diff;

	// Create a map of all the call types for each section of Records.
	size_t i = 0;
	size_t max = 0;
	while (drrec[i] != NULL) {
		char *s = drrec[i]->entries[desc_idx]->val->data.str;

		// Look if we already have this key in our call type map.
		size_t j = 0;
		int found = 0;
		while (map[j] != NULL && !found) {
			if (strcmp(s, map[j]) == 0) {
				found = 1;
			}

			// Keep track of max, so we can set the final size of this map.
			j++;
			if (j > max) {
				max = j;
			}
		}

		#ifdef DEBUG
		printf("create_call_map:\tcall type:\t%s\n", s);
		printf("create_call_map:\tfound:\t%d\n", found);
		printf("create_call_map:\tmap index:\t%zu\n", j);
		#endif

		// If we don't put it in the map.
		if (!found) {
			if (j >= CALL_MAP_SZ) {
				err(EXIT_FAILURE, "buffer overflow: call map bounds exceeded");
			}

			// xxx we're losing a free here
			map[j] = malloc((strlen(s) + 1) * sizeof(char));
			if (map[j] == NULL) {
				err(EXIT_FAILURE, "malloc");
			}
			strcpy(map[j], s);
		}

		i++;
	}

	// realloc(3) to the final size and NULL-terminate the end.
	map = realloc(map, (max + 1) * sizeof(char *));
	if (map == NULL) {
		err(EXIT_FAILURE, "realloc");
	}
	map[max] = NULL;

	#ifdef DEBUG
	printf("create_call_map:\tdrrec call type max:\t%zu\n", max);
	#endif

	i = 0;
	max = 0;
	while (drrec1[i] != NULL) {
		char *s = drrec1[i]->entries[desc_idx]->val->data.str;

		size_t j = 0;
		int found = 0;
		while (map1[j] != NULL && !found) {
			if (strcmp(s, map1[j]) == 0) {
				found = 1;
			}

			j++;
			if (j > max) {
				max = j;
			}
		}

		if (!found) {
			map1[j] = malloc((strlen(s) + 1) * sizeof(char));
			if (map1[j] == NULL) {
				err(EXIT_FAILURE, "malloc");
			}
			strcpy(map1[j], s);
		}

		i++;
	}

	map1 = realloc(map1, (max + 1) * sizeof(char *));
	if (map1 == NULL) {
		err(EXIT_FAILURE, "realloc");
	}
	map1[max] = NULL;

	#ifdef DEBUG
	printf("create_call_map:\tdrrec1 call type max:\t%zu\n", max);
	#endif

	i = 0;
	max = 0;
	while (drrec2[i] != NULL) {
		char *s = drrec2[i]->entries[desc_idx]->val->data.str;

		size_t j = 0;
		int found = 0;
		while (map2[j] != NULL && !found) {
			if (strcmp(s, map2[j]) == 0) {
				found = 1;
			}

			j++;
			if (j > max) {
				max = j;
			}
		}

		if (!found) {
			map2[j] = malloc((strlen(s) + 1) * sizeof(char));
			if (map2[j] == NULL) {
				err(EXIT_FAILURE, "malloc");
			}
			strcpy(map2[j], s);
		}

		i++;
	}

	map2 = realloc(map2, (max + 1) * sizeof(char *));
	if (map2 == NULL) {
		err(EXIT_FAILURE, "realloc");
	}
	map2[max] = NULL;

	#ifdef DEBUG
	printf("create_call_map:\tdrrec2 call type max:\t%zu\n", max);
	#endif

	char ***maps = malloc(3 * sizeof(maps));
	if (maps == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	maps[0] = map;
	maps[1] = map1;
	maps[2] = map2;

	return maps;
}

OutputMap **create_output_map(char ***call_map)
{
	OutputMap **output_map = malloc(CALL_MAPS_SZ * sizeof(OutputMap *));
	if (output_map == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	size_t j = 0;
	while (j < CALL_MAPS_SZ) {
		output_map[j] = malloc(sizeof(OutputMap));
		if (output_map[j] == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		output_map[j]->total = malloc(CALL_MAP_SZ * sizeof(OutputMapEntry *));
		output_map[j]->arg1 = malloc(CALL_MAP_SZ * sizeof(OutputMapEntry *));
		output_map[j]->arg2 = malloc(CALL_MAP_SZ * sizeof(OutputMapEntry *));

		if (output_map[j]->total == NULL || output_map[j]->arg1 == NULL || 
			output_map[j]->arg2 == NULL) {
			err(EXIT_FAILURE, "malloc");
		}

		size_t i = 0;
		while (call_map[j][i] != NULL) {
			output_map[j]->total[i] = malloc(sizeof(OutputMapEntry));
			output_map[j]->arg1[i] = malloc(sizeof(OutputMapEntry));
			output_map[j]->arg2[i] = malloc(sizeof(OutputMapEntry));

			if (output_map[j]->total[i] == NULL ||
				output_map[j]->arg1[i] == NULL ||
				output_map[j]->arg2[i] == NULL) {
				err(EXIT_FAILURE, "malloc");
			}

			i++;
		}

		j++;
	}

	return output_map;
}

MapPair *process_output(Output *output)
{
	const int INDEX = get_field_amt(output->fields);
	int desc_idx = INDEX + FINAL_DESC;
	int dridx = INDEX + DISPATCH_RECEIVED_DIFF;
	int oeidx = INDEX + ONSCENE_ENROUTE_DIFF;
	int oridx = INDEX + ONSCENE_RECEIVED_DIFF;

	char ***map = create_call_map(output);
	OutputMap **output_map = create_output_map(map);

	Record **drrec = output->total->dispatch_received_diff;
	Record **oerec = output->total->onscene_enroute_diff;
	Record **orrec = output->total->onscene_received_diff;

	Record **drrec1 = output->arg1->dispatch_received_diff;
	Record **oerec1 = output->arg1->onscene_enroute_diff;
	Record **orrec1 = output->arg1->onscene_received_diff;

	Record **drrec2 = output->arg2->dispatch_received_diff;
	Record **oerec2 = output->arg2->onscene_enroute_diff;
	Record **orrec2 = output->arg2->onscene_received_diff;

	size_t j = 0;
	while(map[0][j] != NULL) {
		int size = 0;
		output_map[0]->total[j]->drrec = filter_records(drrec, 
			output->fields, map[0][j], desc_idx, 0, output->totalsz, &size);
		output_map[0]->total[j]->drrec_res =
			process_result(output_map[0]->total[j]->drrec, dridx, size);

		// xxx we don't actually need the records after this moment, and we're
		// segfaulting on our other destruction routines. destroying here is a
		// choice made for locality of behavior
		/// @warning Record **records are NULL after this function call.
		if (size > 0) {
			destroy_records(&output_map[0]->total[j]->drrec);
		}
		j++;
	}

	j = 0;
	while(map[0][j] != NULL) {
		int size = 0;
		output_map[0]->total[j]->oerec = filter_records(oerec, 
			output->fields, map[0][j], desc_idx, 0, output->totalsz, &size);
		output_map[0]->total[j]->oerec_res =
			process_result(output_map[0]->total[j]->oerec, oeidx, size);
		if (size > 0) {
			destroy_records(&output_map[0]->total[j]->oerec);
		}
		j++;
	}

	j = 0;
	while(map[0][j] != NULL) {
		int size = 0;
		output_map[0]->total[j]->orrec = filter_records(orrec, 
			output->fields, map[0][j], desc_idx, 0, output->totalsz, &size);
		output_map[0]->total[j]->orrec_res =
			process_result(output_map[0]->total[j]->orrec, oridx, size);
		if (size > 0) {
			destroy_records(&output_map[0]->total[j]->orrec);
		}
		j++;
	}

	j = 0;
	while(map[1][j] != NULL) {
		int size = 0;
		output_map[1]->arg1[j]->drrec = filter_records(drrec1, 
			output->fields, map[1][j], desc_idx, 0, output->arg1sz, &size);
		output_map[1]->arg1[j]->drrec_res =
			process_result(output_map[1]->arg1[j]->drrec, dridx, size);
		if (size > 0) {
			destroy_records(&output_map[1]->arg1[j]->drrec);
		}
		j++;
	//if (rec != NULL || size 0 || *rec == NULL) {
	//	return res;
	//}
	}

	j = 0;
	while(map[1][j] != NULL) {
		int size = 0;
		output_map[1]->arg1[j]->oerec = filter_records(oerec1, 
			output->fields, map[1][j], desc_idx, 0, output->arg1sz, &size);
		output_map[1]->arg1[j]->oerec_res =
			process_result(output_map[1]->arg1[j]->oerec, oeidx, size);
		if (size > 0) {
			destroy_records(&output_map[1]->arg1[j]->oerec);
		}
		j++;

	}

	j = 0;
	while(map[1][j] != NULL) {
		int size = 0;
		output_map[1]->arg1[j]->orrec = filter_records(orrec1, 
			output->fields, map[1][j], desc_idx, 0, output->arg1sz, &size);
		output_map[1]->arg1[j]->orrec_res =
			process_result(output_map[1]->arg1[j]->orrec, oridx, size);
		if (size > 0) {
			destroy_records(&output_map[1]->arg1[j]->orrec);
		}
		j++;

	}

	j = 0;
	while(map[2][j] != NULL) {
		int size = 0;
		output_map[2]->arg2[j]->drrec = filter_records(drrec2, 
			output->fields, map[2][j], desc_idx, 0, output->arg2sz, &size);
		output_map[2]->arg2[j]->drrec_res =
			process_result(output_map[2]->arg2[j]->drrec, dridx, size);
		if (size > 0) {
			destroy_records(&output_map[2]->arg2[j]->drrec);
		}
		j++;
	}

	j = 0;
	while(map[2][j] != NULL) {
		int size = 0;
		output_map[2]->arg2[j]->oerec = filter_records(oerec2,
			output->fields, map[2][j], desc_idx, 0, output->arg2sz, &size);
		output_map[2]->arg2[j]->oerec_res =
			process_result(output_map[2]->arg2[j]->oerec, oeidx, size);
		if (size > 0) {
			destroy_records(&output_map[2]->arg2[j]->oerec);
		}
		j++;
	}

	j = 0;
	while(map[2][j] != NULL) {
		int size = 0;
		output_map[2]->arg2[j]->orrec = filter_records(orrec2,
			output->fields, map[2][j], desc_idx, 0, output->arg2sz, &size);
		output_map[2]->arg2[j]->orrec_res =
			process_result(output_map[2]->arg2[j]->orrec, oridx, size);
		if (size > 0) {
			destroy_records(&output_map[2]->arg2[j]->orrec);
		}
		j++;
	}

	// Create a pair to return both output_map and call_map as a pair.
	MapPair *map_pair = malloc(sizeof(MapPair));
	if (map_pair == NULL) {
		err(EXIT_FAILURE, "malloc");
	}

	map_pair->call_map = map;
	map_pair->output_map = output_map;

	return map_pair;
}

void print_header() {
	printf("%-20s | %7s | %6s | %6s | %6s | %6s | %8s | %6s | %6s | %6s | %6s | %8s\n",
		"Call Type", "count", "min", "LB", "Q1", "med", "mean", "Q3", "UB", "max", "IQR", "stddev");
}

void print_row(const char *call_type, Result *res) {
	printf("%-20s | %7d | %6d | %6d | %6.0f | %6d | %8.2f | %6.0f | %6d | %6d | %6d | %8.2f\n",
		call_type, res->count, res->min, res->LB, res->Q1, res->med, res->mean,
		res->Q3, res->UB, res->max, res->IQR, res->stddev);
}

void print_output(MapPair *map_pair, char *arg1, char *arg2)
{
	OutputMap **output_map = map_pair->output_map;
	char ***call_map = map_pair->call_map;

	printf("Total\t-\tDispatch - Received Time\n\n");
	print_header();

	size_t i = 0;
	// Call map and output map have perfectly aligned indices; output map was
	// constructed from call map.
	// xxx calculate total
	while (call_map[0][i] != NULL) {
		print_row(call_map[0][i], output_map[0]->total[i]->drrec_res);
		i++;
	}

	// To match expected output spacing.
	printf("\n\n\n");

	printf("Total\t-\tOn-Scene - Enroute Time\n\n");
	print_header();
	i = 0;
	while (call_map[0][i] != NULL) {
		print_row(call_map[0][i], output_map[0]->total[i]->oerec_res);
		i++;
	}
	printf("\n\n\n");

	printf("Total\t-\tOn-Scene - Received Time\n\n");
	print_header();
	i = 0;
	while (call_map[0][i] != NULL) {
		print_row(call_map[0][i], output_map[0]->total[i]->orrec_res);
		i++;
	}
	printf("\n\n\n");


	printf("%s\t-\tDispatch - Received Time\n\n", arg1);
	i = 0;
	while (call_map[1][i] != NULL) {
		print_row(call_map[1][i], output_map[1]->arg1[i]->drrec_res);
		i++;
	}
	printf("\n\n\n");

	printf("%s\t-\tOn-Scene - Enroute Time\n\n", arg1);
	print_header();
	i = 0;
	while (call_map[1][i] != NULL) {
		print_row(call_map[1][i], output_map[1]->arg1[i]->oerec_res);
		i++;
	}
	printf("\n\n\n");

	printf("%s\t-\tOn-Scene - Received Time\n\n", arg1);
	print_header();
	i = 0;
	while (call_map[1][i] != NULL) {
		print_row(call_map[1][i], output_map[1]->arg1[i]->orrec_res);
		i++;
	}
	printf("\n\n\n");

	printf("%s\t-\tDispatch - Received Time\n\n", arg2);
	i = 0;
	while (call_map[2][i] != NULL) {
		print_row(call_map[2][i], output_map[2]->arg2[i]->drrec_res);
		i++;
	}
	printf("\n\n\n");

	printf("%s\t-\tOn-Scene - Enroute Time\n\n", arg2);
	print_header();
	i = 0;
	while (call_map[2][i] != NULL) {
		print_row(call_map[2][i], output_map[2]->arg2[i]->oerec_res);
		i++;
	}
	printf("\n\n\n");

	printf("%s\t-\tOn-Scene - Received Time\n\n", arg2);
	print_header();
	i = 0;
	while (call_map[2][i] != NULL) {
		print_row(call_map[2][i], output_map[2]->arg2[i]->orrec_res);
		i++;
	}

	// Cleanup:
	// done here for locality of behavior. could be refactored into a
	// destruction routine.

	// xxx recall we already freed the records
	// xxx we miss indexed when we added our results, that's why this is done
	// this way.
	i = 0;
	while (call_map[0][i] != NULL) {
		free(output_map[0]->total[i]->drrec_res);
		free(output_map[0]->total[i]->oerec_res);
		free(output_map[0]->total[i]->orrec_res);

		output_map[0]->total[i]->drrec_res = NULL;
		output_map[0]->total[i]->oerec_res = NULL;
		output_map[0]->total[i]->orrec_res = NULL;

		i++;
	}

	i = 0;
	while (call_map[1][i] != NULL) {
		free(output_map[1]->arg1[i]->drrec_res);
		free(output_map[1]->arg1[i]->oerec_res);
		free(output_map[1]->arg1[i]->orrec_res);

		output_map[1]->arg1[i]->drrec_res = NULL;
		output_map[1]->arg1[i]->oerec_res = NULL;
		output_map[1]->arg1[i]->orrec_res = NULL;

		i++;
	}

	i = 0;
	while (call_map[2][i] != NULL) {
		free(output_map[2]->arg2[i]->drrec_res);
		free(output_map[2]->arg2[i]->oerec_res);
		free(output_map[2]->arg2[i]->orrec_res);

		output_map[2]->arg2[i]->drrec_res = NULL;
		output_map[2]->arg2[i]->oerec_res = NULL;
		output_map[2]->arg2[i]->orrec_res = NULL;

		i++;
	}

	i = 0;
	while (i < CALL_MAPS_SZ) {
		size_t j = 0;
		while (call_map[i][j] != NULL) {
			free(output_map[i]->total[j]);
			free(output_map[i]->arg1[j]);
			free(output_map[i]->arg2[j]);

			output_map[i]->total[j] = NULL;
			output_map[i]->arg1[j] = NULL;
			output_map[i]->arg2[j] = NULL;

			free(call_map[i][j]);
			call_map[i][j] = NULL;

			j++;
		}

		free(output_map[i]->total);
		output_map[i]->total = NULL;
		free(output_map[i]->arg1);
		output_map[i]->arg1 = NULL;
		free(output_map[i]->arg2);
		output_map[i]->arg2 = NULL;

		free(output_map[i]);
		output_map[i] = NULL;

		free(call_map[i]);
		call_map[i] = NULL;

		i++;
	}

	free(output_map);
	free(call_map);
	free(map_pair);
	output_map = NULL;
	call_map = NULL;
	map_pair = NULL;
}

// vim: filetype=c:noexpandtab:shiftwidth=4:tabstop=4:softtabstop=4:textwidth=80
