#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <math.h>

/* Required Adjustable Parameters START **********************************************/

#define MAX_LENGTH_FILENAME_CSV 260
#define MAX_LINE_LENGTH_CSV 256
#define DIFFERENCE_FILENAME "difference.csv"
#define NUM_COLUMNS 6

/* Required Adjustable Parameters END ************************************************/

/* Optional Adjustable Parameters START **********************************************/

/* If ASSUME_UNIX_TIME is defined, this assumes the input CSV file already has UNIX time (instead of camera time) for camera data. */
#define ASSUME_UNIX_TIME

/* Optional Adjustable Parameters END ************************************************/

void convert_line(FILE* output_file, FILE* difference_file, char* line, const time_t* unix_time_data_start_day, const time_t* unix_time_data_start_week, const time_t* offset){
	char* end_ptr = line;
	double data[NUM_COLUMNS];
	int i = 0;
	for(; i < NUM_COLUMNS; i++){
		double value = strtod(end_ptr, &end_ptr);
		if(i >= 0 && i <= 2){
#ifdef ASSUME_UNIX_TIME
			value /= 1000000000.0;
#else
			value = value / 1000000000.0 + (double) (*offset) + (double) (*unix_time_data_start_day);
#endif
		}else if(i == 3){
			value /= 1000000;
		}else if(i == 4){
			value = fmod(value, 604800.0) + (double) (*unix_time_data_start_week);
		}
		data[i] = value;
		fprintf(output_file, "%f", value);
		if(i < NUM_COLUMNS - 1){
			fprintf(output_file, "%s", ",");
		}else{
			fprintf(output_file, "%s", "\n");
		}
		for(; *end_ptr != '\0' && (*end_ptr < '0' || *end_ptr > '9'); end_ptr++);
	}
	fprintf(difference_file, "%f,%f,%f,%f,%f,%f\n", data[0] - data[NUM_COLUMNS-1], data[1] - data[NUM_COLUMNS-1], data[2] - data[NUM_COLUMNS-1], 
			data[3] - data[NUM_COLUMNS-1], data[4] - data[NUM_COLUMNS-1], 0.0);
}

#ifndef ASSUME_UNIX_TIME
double min_camera_start_time(FILE* input_file){
	double min = DBL_MAX;
	char* end_ptr;
	char* line = (char*) malloc(sizeof(char) * MAX_LINE_LENGTH_CSV);
	while(fgets(line, MAX_LINE_LENGTH_CSV, input_file) != NULL){
		int i;
		double values[3];
		values[0] = strtod(line, &end_ptr);
		for(; *end_ptr < '0' || *end_ptr > '9'; end_ptr++);
		values[1] = strtod(end_ptr, &end_ptr);
		for(; *end_ptr < '0' || *end_ptr > '9'; end_ptr++);
		values[2] = strtod(end_ptr, &end_ptr);
		for(i = 0; i < 3; i++){
			if(values[i] < min){
				min = values[i];
			}
		}
	}
	free(line);
	rewind(input_file);
	return min;
}
#endif

int main(){
	FILE* file;
	FILE* output_file;
	FILE* difference_file;
	char* output_filename;
	char* filename;
        char* line;
#ifndef ASSUME_UNIX_TIME
	double min;
	time_t unix_time_data_start = 0;
#endif
	time_t offset = 0;
	time_t unix_time_data_start_day = 0;
	time_t unix_time_data_start_week = 0;
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
#ifndef ASSUME_UNIX_TIME
	min = min_camera_start_time(file);
	offset = unix_time_data_start - unix_time_data_start_day - (min / 1000000000);
#endif
	line = (char*) malloc(sizeof(char) * MAX_LINE_LENGTH_CSV);
	while(fgets(line, MAX_LINE_LENGTH_CSV, file) != NULL){
		convert_line(output_file, difference_file, line, &unix_time_data_start_day, &unix_time_data_start_week, &offset);
	}
	free(line);
	fclose(output_file);
	fclose(difference_file);
	fclose(file);
	return 0;
}
