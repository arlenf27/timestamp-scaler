build: timestamp_scaler.c
	gcc -ansi -pedantic -Wall -o timestamp_scaler timestamp_scaler.c -lm

clean: 
	rm -f timestamp_scaler
