#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "timestamp_dataset.h"

#define INITIAL_CAPACITY 100
#define MAX_LINE_LENGTH_CSV 256

struct timestamp_dataset{
	double*** data;
	size_t size;
	size_t capacity;
	double min;
};

int read_data(FILE* file, timestamp_dataset* dataset);

timestamp_dataset* timestamp_dataset_create(FILE* file){
	timestamp_dataset* dataset = (timestamp_dataset*) malloc(sizeof(timestamp_dataset));
	dataset->data = (double***) malloc(sizeof(double**));
	*(dataset->data) = (double**) malloc(INITIAL_CAPACITY * sizeof(double*));
	dataset->size = 0;
	dataset->capacity = INITIAL_CAPACITY;
	dataset->min = DBL_MAX;
	if(read_data(file, dataset) == 0){
		return NULL;
	}
	return dataset;
}

const double*** get_data(timestamp_dataset* dataset){
	return (const double***) dataset->data;
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
		free(*(*(dataset->data)+r));
	}
	/* Free all rows. */
	free(*(dataset->data));
	/* Free data pointer. */
	free(dataset->data);
	/* Free timestamp_dataset structure. */
	free(dataset);
}

int read_data(FILE* file, timestamp_dataset* dataset){
	char* line = (char*) malloc(sizeof(char) * MAX_LINE_LENGTH_CSV);
	int r = 0;
	while(fgets(line, MAX_LINE_LENGTH_CSV, file) != NULL){
		int i = 0;
		char* end_ptr = line;
		*(*(dataset->data)+r) = (double*) malloc(NUM_COLUMNS * sizeof(double));
		for(; i < NUM_COLUMNS; i++){
			double value = strtod(end_ptr, &end_ptr);
			*(*(*(dataset->data)+r)+i) = value;
			for(; *end_ptr != '\0' && (*end_ptr < '0' || *end_ptr > '9'); end_ptr++);
			/* Check if this is camera min or not. */
			if(i >= 0 && i <= 2 && value < dataset->min){
				dataset->min = value;
			}
		}
		r++;
		/* Double table capacity when current capacity is around 70% filled. */
		if((double) r >= (double) dataset->capacity * (double) 0.7){
			double** new_alloc_ptr = (double**) realloc(*(dataset->data), dataset->capacity * 2 * sizeof(double));
			if(new_alloc_ptr == NULL){
				free(*(dataset->data));
				return 0;
			}else{
				*(dataset->data) = new_alloc_ptr;
			}
			dataset->capacity *= 2;
		}
		dataset->size++;
	}
	free(line);
	return 1;
}
