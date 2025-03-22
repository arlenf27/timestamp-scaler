# Timestamp Scaler
This program takes input from a CSV file for the timestamps of cameras, a LiDAR, a GPS, and the local PC time of each data point. It then converts the data to standard Unix time and measures the difference between each timestamp relative to the local time. 

## Dependencies
This program requires no dependencies other than a C compiler and library that is compliant with any ISO C standard (ANSI/C89/C90, C99, C11, C17, C23, etc.). It is only dependent on the ANSI/C89/C90 versions of standard C libraries such as stdio.h, stdlib.h, string.h, time.h, math.h, and float.h. ***For guaranteed results, compile on a Linux/MacOS distribution. Please note that using a Windows C library (e.g. MinGW-w64 libraries) may result in undefined behavior or unexpected warnings/errors! For instance, Windows libraries often use the LLP64 data model (where long int is 32 bits) instead of the LP64 data model. Also, time_t may be defined differently in ANSI C Windows libraries when compared to ANSI C MacOS or Linux libraries. ***

## Usage
To compile on Linux/MacOS, any ISO-compliant C compiler should work without any errors or warnings. For instance, `gcc -ansi -pedantic -Wall -o timestamp_scaler timestamp_scaler.c -lm` will work on most Linux/MacOS distributions.   
Just make sure that libraries including the following standard ISO ANSI/C89/C90/C95/C99/C11/C17/C23 headers are included: 
- stdio.h
- stdlib.h
- string.h
- math.h
- float.h (On many Linux distributions, this is included in libm instead of libc)
- time.h

A makefile has been included that provides an example compilation command for GCC that works on most Linux/MacOS distributions. 


## Input Format
- The input file must be a CSV file with ***no header***. 
- Columns 1 to 3 are for the 3 Teledyne Blackfly GigE cameras; input is in Unix time (nanoseconds) OR camera time already multiplied by 8. For more details about timestamp scaling if the data is in camera time, see https://www.teledynevisionsolutions.com/support/support-center/application-note/iis/synchronizing-a-blackfly-or-grasshopper3-gige-cameras-time-to-pc-time/. By default, this program takes in the Unix time. ***To handle parameters and options required if camera time is used, `ASSUME_UNIX_TIME` must not be defined and the unix time (seconds) for the start of the day of data collection and start of data collection must be specified through user input with appropriate values.*** 
- Column 4 is for the Cepton LiDAR; input is in Unix time (microseconds). 
- Column 5 is for the GPS; input is in the form of amount of seconds elapsed since the start of the week (Sunday). 
- Column 6 is the local time; input is in Unix time (seconds). 

## Parameters
- There are 4 required adjustable parameters: `MAX_LENGTH_FILENAME_CSV`, `MAX_LINE_LENGTH_CSV`, `DIFFERENCE_FILENAME`, and `NUM_COLUMNS`. 
- There is 1 optional adjustable parameter: `ASSUME_UNIX_TIME` (No corresponding value required or used) - See Input Format Columns 1 to 3 for more details on this parameter. 

## Outputs
- A CSV file (with user-specified name by default) with scaled outputs, all in Unix time (seconds). 
- A CSV file ("difference.csv" by default) with the difference (seconds) for each timestamp relative to the local time in the same row. 
