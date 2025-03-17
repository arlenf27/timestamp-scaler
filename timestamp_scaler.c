#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <math.h>

/* Required Adjustable Parameters START **********************************************/

#define MAX_LENGTH_FILENAME_CSV 260
#define MAX_LINE_LENGTH_CSV 256

/* Required Adjustable Parameters END ************************************************/

/* Optional Adjustable Parameters START **********************************************/

#define UNIX_TIME_DATA_START 1741983780
#define UNIX_TIME_DATA_START_DAY 1741924800

/* Start of week is on SUNDAY in this case. */
#define UNIX_TIME_DATA_START_WEEK 1741496400

/* Can optionally add macros for input (INPUT_FILENAME) and output (OUTPUT_FILENAME) filenames here. */

/* Optional Adjustable Parameters END ************************************************/

void convert_line(FILE* output_file, char* line, const time_t* unix_time_data_start_day, const time_t* unix_time_data_start_week, const time_t* offset){
	char* end_ptr = line;
	const int NUM_COLUMNS = 6;
	int i = 0;
	for(; i < NUM_COLUMNS; i++){
		double value = strtod(end_ptr, &end_ptr);
		if(i >= 0 && i <= 2){
			value = value / 1000000000.0 + (double) (*offset) + (double) (*unix_time_data_start_day);
		}else if(i == 3){
			value /= 1000000;
		}else if(i == 4){
			value = fmod(value, 604800.0) + (double) (*unix_time_data_start_week);
		}
		fprintf(output_file, "%f", value);
		if(i < NUM_COLUMNS - 1){
			fprintf(output_file, "%s", ",");
		}else{
			fprintf(output_file, "%s", "\n");
		}
		for(; *end_ptr != '\0' && (*end_ptr < '0' || *end_ptr > '9'); end_ptr++);
	}	
}

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

int main(){
	FILE* file;
	FILE* output_file;
	char* output_filename;
	char* filename;
        char* line;
	double min;
	time_t offset;
	time_t unix_time_data_start = 0;
	time_t unix_time_data_start_day = 0;
	time_t unix_time_data_start_week = 0;
	filename = (char*) malloc(sizeof(char) * MAX_LENGTH_FILENAME_CSV);
#ifdef INPUT_FILENAME
	filename = INPUT_FILENAME;
#else
	printf("%s", "Enter absolute or relative path and name of CSV file: ");
	fgets(filename, MAX_LENGTH_FILENAME_CSV, stdin);
	*(filename + strlen(filename) - 1) = '\0';
#endif
	file = fopen(filename, "r");
	free(filename);
	if(file == NULL){
		printf("%s\n", "Failed to open file. ");
		return 1;
	}
	output_filename = (char*) malloc(sizeof(char) * MAX_LENGTH_FILENAME_CSV);
#ifdef OUTPUT_FILENAME
	output_filename = OUTPUT_FILENAME;
#else
	printf("%s", "Enter absolute or relative path and name of output file to create: ");
	fgets(output_filename, MAX_LENGTH_FILENAME_CSV, stdin);
	*(output_filename + strlen(output_filename) - 1) = '\0';
#endif
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
#ifdef UNIX_TIME_DATA_START
	unix_time_data_start = UNIX_TIME_DATA_START;
#else
	printf("%s", "Enter Unix time at start of data collection: ");
	fscanf(stdin, "%ld", &unix_time_data_start);
#endif

#ifdef UNIX_TIME_DATA_START_WEEK
	unix_time_data_start_week = UNIX_TIME_DATA_START_WEEK;
#else
	printf("%s", "Enter Unix time at start of the week (SUNDAY) of data collection: ");
	fscanf(stdin, "%ld", &unix_time_data_start_week);
#endif

#ifdef UNIX_TIME_DATA_START_DAY
	unix_time_data_start_day = UNIX_TIME_DATA_START_DAY;
#else
	printf("%s", "Enter Unix time at start of the day of data collection: ");
	fscanf(stdin, "%ld", &unix_time_data_start_day);
#endif
	min = min_camera_start_time(file);
	offset = unix_time_data_start - unix_time_data_start_day - (min / 1000000000);
	line = (char*) malloc(sizeof(char) * MAX_LINE_LENGTH_CSV);
	while(fgets(line, MAX_LINE_LENGTH_CSV, file) != NULL){
		convert_line(output_file, line, &unix_time_data_start_day, &unix_time_data_start_week, &offset);
	}
	free(line);
	fclose(output_file);
	fclose(file);
	return 0;
}
