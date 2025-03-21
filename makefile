build: timestamp_scaler.c
	gcc -ansi -pedantic -Wall -o timestamp_scaler.exe timestamp_scaler.c -lm

clean: 
	rm -f timestamp_scaler.exe
