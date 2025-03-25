#ifndef TIMESTAMP_DATASET_H
#define TIMESTAMP_DATASET_H

#include <stdio.h>
#include <time.h>

/* Number of columns in CSV file */
#define NUM_COLUMNS 6

/* Opaque pointer to timestamp_dataset. */
typedef struct timestamp_dataset timestamp_dataset;

/* Creates a dataset and return a pointer to it. 
 * file: input CSV file, required to be empty and open for appending */ 
timestamp_dataset* timestamp_dataset_create(FILE* file);

/* Returns data as const pointer to matrix (dereferencing once gets the row pointer).
 * The matrix is not modifiable at any level.  
 * Requires timestamp_dataset_create() to already have been called for dataset
 * dataset: dataset to get data from */
const double* const* const* get_data(timestamp_dataset* dataset);

/* Returns earliest camera time as start time. 
 * Requires timestamp_dataset_create() to already have been called for dataset
 * dataset: dataset to get min from */
double min_camera_start_time(timestamp_dataset* dataset);

/* Returns number of rows in dataset. 
 * Requires timestamp_dataset_create() to already have been called for dataset
 * dataset: dataset to get size from */
size_t get_size(timestamp_dataset* dataset);

/* Destroy dataset. 
 * Requires timestamp_dataset_create() to already have been called for dataset
 * dataset: dataset to destroy */
void timestamp_dataset_destroy(timestamp_dataset* dataset);

#endif
