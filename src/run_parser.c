//run_parser.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "get_line.h"
#include "ptree.h"
#include "parser_llk.h"
#include "tree_parser.h"
#include "debug.h"


#define START start


int main(int argc, char **argv){
	FILE *file = stdin;
	if(argc < 2){
		puts("No file specified, reading from stdin:");
	}else{
		file = fopen(argv[1], "r");
		if(!file){
			printf("Could not open \"%s\"!  Reading from stdin:\n", argv[1]);
			file = stdin;
		}
	}
	size_t length = 0;
	char *line = NULL;
	int read = 0;
	Position *p;
	Ptree *t;
	while((read = getLine(&line, &length, file)) > 0){
		line[read - 1] = '\0';
		printf("\"%s\"\n", line);
		t = mallocPtree();
		p = firstPosition(line);
		if(START(p, t)){
			puts("Parsed successfully.  Output:");
			flattenTagged(t);
			printPtree(t, 0);
		}else{
			puts("Parsed unsuccessfully");
		}
		deletePtree(t);
		free(t);
		free(p);
	}
	free(line);
	fclose(file);
}

