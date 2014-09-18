//get_line.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "get_line.h"


#define SIZE 4
#define MULT 2


int getLine(char **line, size_t *size, FILE *file){
	if(!line || !size || !file){
		return -1;
	}
	if(!*line ^ !*size){
		return -1;
	}
	if(*size < 3){
		free(*line);
		*size = 0;
	}
	if(!*size){
		*line = malloc(SIZE*sizeof(char));
		if(!*line){
			return -1;
		}
		*size = SIZE;
	}
	char *temp;
	size_t curr = 0;
	while(1){
		(*line)[*size - 2] = '\0';//if this is '\0' or '\n' when done with fgets, the string is done.
		if(!fgets(*line + curr, *size - curr, file)){
			return -1;
		}
		switch((*line)[*size - 2]){
			case '\0':
				for(size_t i = *size / MULT; i < *size; ++i){
					if((*line)[i] == '\0'){
						return i;
					}
				}
				return *size - 1;
			case '\n':
				return *size;
		}
		temp = realloc(*line, MULT**size*sizeof(char));//that ** is a rare operator.  Actually its x*(*y).
		if(!temp){
			return -1;
		}
		curr = *size - 1;
		*line = temp;
		*size *= MULT;
	}
}

