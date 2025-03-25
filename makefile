build: timestamp_scaler.c
	gcc -ansi -pedantic -Wall -o timestamp_scaler timestamp_scaler.c timestamp_dataset.c -lm

clean: 
	rm -f timestamp_scaler
