#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "timestamp_dataset.h"

/* Required Adjustable Parameters START **********************************************/

#define MAX_LENGTH_FILENAME_CSV 260
#define DIFFERENCE_FILENAME "difference.csv"

/* Required Adjustable Parameters END ************************************************/

/* Optional Adjustable Parameters START **********************************************/

/* If ASSUME_UNIX_TIME is defined, this assumes the input CSV file already has UNIX time (instead of camera time) for camera data. */
#define ASSUME_UNIX_TIME

/* Optional Adjustable Parameters END ************************************************/

void convert_data(timestamp_dataset* dataset, FILE* output_file, FILE* difference_file, time_t unix_time_data_start_day, time_t unix_time_data_start_week, time_t offset){
	const double*** data = get_data(dataset);
	int r = 0;
	for(; r < get_size(dataset); r++){
		double values[NUM_COLUMNS];
		int i = 0;
		for(; i < NUM_COLUMNS; i++){
			if(i >= 0 && i <= 2){
#ifdef ASSUME_UNIX_TIME
				values[i] = (*(*(*data+r)+i)) / 1000000000.0;
#else
				values[i] = (*(*(*data+r)+i)) / 1000000000.0 + (double) offset + (double) unix_time_data_start_day;
#endif
			}else if(i == 3){
				values[i] = (*(*(*data+r)+i)) / 1000000.0;
			}else if(i == 4){
				values[i] = fmod((*(*(*data+r)+i)), 604800.0) + (double) unix_time_data_start_week;
			}else if(i == 5){
				values[i] = (*(*(*data+r)+i));
			}
			fprintf(output_file, "%f", values[i]);
			if(i < NUM_COLUMNS - 1){
				fprintf(output_file, "%s", ",");
			}else{
				fprintf(output_file, "%s", "\n");
			}
		}
		for(i = 0; i < NUM_COLUMNS; i++){
			if(i < NUM_COLUMNS - 1){
				fprintf(difference_file, "%f,", values[i] - values[NUM_COLUMNS-1]);
			}else{
				fprintf(difference_file, "%f\n", values[i] - values[NUM_COLUMNS-1]);
			}
		}
	}
}

int main(){
	FILE* file;
	FILE* output_file;
	FILE* difference_file;
	char* output_filename;
	char* filename;
#ifndef ASSUME_UNIX_TIME
	double min;
	time_t unix_time_data_start = 0;
#endif
	time_t offset = 0;
	time_t unix_time_data_start_day = 0;
	time_t unix_time_data_start_week = 0;
	timestamp_dataset* dataset;

	/* Input/Output Prompting */
	filename = (char*) malloc(sizeof(char) * MAX_LENGTH_FILENAME_CSV);
	printf("%s", "Enter absolute or relative path and name of CSV file: ");
	fgets(filename, MAX_LENGTH_FILENAME_CSV, stdin);
	*(filename + strlen(filename) - 1) = '\0';
	file = fopen(filename, "r");
	free(filename);
	if(file == NULL){
		printf("%s\n", "Failed to open file. ");
		return 1;
	}
	output_filename = (char*) malloc(sizeof(char) * MAX_LENGTH_FILENAME_CSV);
	printf("%s", "Enter absolute or relative path and name of output file to create: ");
	fgets(output_filename, MAX_LENGTH_FILENAME_CSV, stdin);
	*(output_filename + strlen(output_filename) - 1) = '\0';
	output_file = fopen(output_filename, "w");
	if(output_file == NULL){
		printf("%s\n", "Failed to open output file. ");
		free(output_filename);
		return 1;
	}
	fclose(output_file);
	output_file = fopen(output_filename, "a");
	free(output_filename);
	if(output_file == NULL){
		printf("%s\n", "Failed to open output file. ");
		return 1;
	}
#ifndef ASSUME_UNIX_TIME
	printf("%s", "Enter Unix time at start of data collection: ");
	fscanf(stdin, "%ld", &unix_time_data_start);
#endif

	printf("%s", "Enter Unix time at start of the week (SUNDAY) of data collection: ");
	fscanf(stdin, "%ld", &unix_time_data_start_week);
#ifndef ASSUME_UNIX_TIME
	printf("%s", "Enter Unix time at start of the day of data collection: ");
	fscanf(stdin, "%ld", &unix_time_data_start_day);
#endif
	difference_file = fopen(DIFFERENCE_FILENAME, "w");
	if(difference_file == NULL){
		printf("%s\n", "Failed to open difference file. ");
		return 1;
	}
	fclose(difference_file);
	difference_file = fopen(DIFFERENCE_FILENAME, "a");
	if(difference_file == NULL){
		printf("%s\n", "Failed to open difference file. ");
		return 1;
	}

	/* Dataset reading and creation */
	dataset = timestamp_dataset_create(file);

#ifndef ASSUME_UNIX_TIME
	min = min_camera_start_time(dataset);
	offset = unix_time_data_start - unix_time_data_start_day - (min / 1000000000);
#endif
	
	/* Converting and writing output */
	convert_data(dataset, output_file, difference_file, unix_time_data_start_day, unix_time_data_start_week, offset);

	timestamp_dataset_destroy(dataset);	
	fclose(output_file);
	fclose(difference_file);
	fclose(file);
	return 0;
}
