#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "timestamp_dataset.h"

#define INITIAL_CAPACITY 100
#define MAX_LINE_LENGTH_CSV 256

struct timestamp_dataset{
	const double*** data;
	size_t size;
	size_t capacity;
	double min;
};

int read_data(FILE* file, timestamp_dataset* dataset);

timestamp_dataset* timestamp_dataset_create(FILE* file){
	timestamp_dataset* dataset = (timestamp_dataset*) malloc(sizeof(timestamp_dataset));
	dataset->data = (const double***) malloc(sizeof(double**));
	*(dataset->data) = (const double**) malloc(INITIAL_CAPACITY * sizeof(double*));
	dataset->size = 0;
	dataset->capacity = INITIAL_CAPACITY;
	dataset->min = DBL_MAX;
	if(read_data(file, dataset) == 0){
		return NULL;
	}
	return dataset;
}

const double*** get_data(timestamp_dataset* dataset){
	return dataset->data;
}

double min_camera_start_time(timestamp_dataset* dataset){
	return dataset->min;
}

size_t get_size(timestamp_dataset* dataset){
	return dataset->size;
}

void timestamp_dataset_destroy(timestamp_dataset* dataset){
	int r = 0;
	for(; r < dataset->size; r++){
		/* Free data for each row. */
		free((double*) *(*(dataset->data)+r));
	}
	/* Free all rows. */
	free((double**) *(dataset->data));
	/* Free data pointer. */
	free((double***) dataset->data);
	/* Free timestamp_dataset structure. */
	free(dataset);
}

int read_data(FILE* file, timestamp_dataset* dataset){
	char* line = (char*) malloc(sizeof(char) * MAX_LINE_LENGTH_CSV);
	int r = 0;
	while(fgets(line, MAX_LINE_LENGTH_CSV, file) != NULL){
		int i = 0;
		char* end_ptr = line;
		double* temp = (double*) malloc(NUM_COLUMNS * sizeof(double));
		for(; i < NUM_COLUMNS; i++){
			double value = strtod(end_ptr, &end_ptr);
			*(temp+i) = value;
			for(; *end_ptr != '\0' && (*end_ptr < '0' || *end_ptr > '9'); end_ptr++);
			/* Check if this is camera min or not. */
			if(i >= 0 && i <= 2 && value < dataset->min){
				dataset->min = value;
			}
		}
		*(*(dataset->data)+r) = (const double*) temp;
		dataset->size++;
		r++;
		/* Double table capacity when current capacity is around 70% filled. */
		if((double) r >= (double) dataset->capacity * (double) 0.7){
			const double** new_alloc_ptr = (const double**) realloc(*(dataset->data), dataset->capacity * 2 * sizeof(double));
			if(new_alloc_ptr == NULL){
				timestamp_dataset_destroy(dataset);
				return 0;
			}else{
				*(dataset->data) = new_alloc_ptr;
			}
			dataset->capacity *= 2;
		}
	}
	free(line);
	return 1;
}
