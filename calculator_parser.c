#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser_llk.h"


/*GRAMMAR:
start = additive

additive = multiplicative "+" additive | multiplicative

multiplicative = primary "*" multiplicative | primary

primary = integer | "(" additive ")"

integer = [0-9]+
*/


int start(Position *p, Ptree *t);
int additive(Position *p, Ptree *t);
int addition(Position *p, Ptree *t);
int multiplicative(Position *p, Ptree *t);
int multiplication(Position *p, Ptree *t);
int primary(Position *p, Ptree *t);
int integer(Position *p, Ptree *t);
int digit(Position *p, Ptree *t);


int start(Position *p, Ptree *t){
	if(!expect(p, t, additive)){
		logUnexpectedError(p, "start", "additive");
		return 0;
	}
	if(!expectEnd(p)){
		logUnexpectedError(p, "start", "end of input");
		return 0;
	}
	return 1;
}

int additive(Position *p, Ptree *t){
	return sepBy(p, t, multiplicative, addition);
}

int addition(Position *p, Ptree *t){
	return oneOf(p, t, "+-");
}

int multiplicative(Position *p, Ptree *t){
	return sepBy(p, t, primary, multiplication);
}

int multiplication(Position *p, Ptree *t){
	return oneOf(p, t, "*/%");
}

int primary(Position *p, Ptree *t){
	if(acceptString(p, t, "(")){
		if(!expect(p, t, additive)){
			logUnexpectedError(p, "primary", "additive");
			return 0;
		}
		if(!expectString(p, t, ")")){
			logUnexpectedError(p, "primary", ")");
			return 0;
		}
		return 1;
	}
	return expect(p, t, integer);
}

int integer(Position *p, Ptree *t){
	return repeatMinMax(p, t, digit, 1, -1);
}

int digit(Position *p, Ptree *t){
	return oneOf(p, t, "0123456789");
}


int main(int argc, char **argv){
	FILE *file;
	if(argc == 1){
		puts("No file specified.  Reading from stdin.");
		file = stdin;
	}else if(argc > 2){
		printf("Too many command line arguments.  Usage: %s <input file>\n", argv[0]);
		return 1;
	}else{
		file = fopen(argv[1], "r");
		if(!file){
			printf("Could not open file \"%s\"\n", argv[1]);
		}
		return 1;
	}
	printf("The different parsers are at locations:\n"
		   "start:          %p\n"
		   "additive:       %p\n"
		   "addition:       %p\n"
		   "multiplicative: %p\n"
		   "multiplication: %p\n"
		   "primary:        %p\n"
		   "integer:        %p\n"
		   "digit:          %p\n",
		   start, additive, addition, multiplicative, multiplication, primary, integer, digit);
	char *line = 0;
	size_t length = 0;
	int read = 0;
	Position *p;
	char success = 0;
	Ptree ptree;
	while((read = getline(&line, &length, file)) != -1){
		line[read - 1] = '\0';
		p = firstPosition(line);
		ptree = newPtree(NULL, "equation");
		success = start(p, &ptree);
		printf("string \"%s\" parsed %ssuccessfully.  Output: %d\n", line, success?"":"un", 10);
		printPtree(&ptree, 0);
		deletePtree(&ptree);
		free(p);
	}
	free(line);
	fclose(file);
}

