//calculator_parser.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "get_line.h"
#include "ptree.h"
#include "parser_llk.h"
#include "calculator_interpreter.h"
#include "debug.h"


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
	setString(t, __func__, strlen(__func__));
	if(!expect(p, t, PASS, additive)){
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
	setString(t, __func__, strlen(__func__));
	return sepBy(p, t, ADD, ADD, multiplicative, addition);
}

int addition(Position *p, Ptree *t){
	//setString(t, __func__, strlen(__func__));
	return oneOf(p, t, PASS, "+-");
}

int multiplicative(Position *p, Ptree *t){
	setString(t, __func__, strlen(__func__));
	return sepBy(p, t, ADD, ADD, primary, multiplication);
}

int multiplication(Position *p, Ptree *t){
	//setString(t, __func__, strlen(__func__));
	return oneOf(p, t, PASS, "*/%");
}

int primary(Position *p, Ptree *t){
	setString(t, __func__, strlen(__func__));
	if(acceptString(p, t, SKIP, "(")){
		if(!expect(p, t, PASS, additive)){
			logUnexpectedError(p, "primary", "additive");
			return 0;
		}
		if(!expectString(p, t, SKIP, ")")){
			logUnexpectedError(p, "primary", ")");
			return 0;
		}
		return 1;
	}
	return expect(p, t, ADD, integer);
}

int integer(Position *p, Ptree *t){
	//setString(t, __func__, strlen(__func__));
	return repeatMinMax(p, t, PASS, digit, 1, -1);
}

int digit(Position *p, Ptree *t){
	//setString(t, __func__, strlen(__func__));
	return oneOf(p, t, PASS, "0123456789");
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
	Ptree *root;	
	while((read = getLine(&line, &length, file)) != -1){
		root = mallocPtree();
		line[read - 1] = '\0';
		p = firstPosition(line);
		success = start(p, root);
		printf("string \"%s\" parsed %ssuccessfully.  Output:\n", line, success?"":"un");
		flattenTagged(root);
		puts("flattened:");
		printPtree(root, 0);
		puts("evaluated:");
		printf("%f\n", eval(root));
		deletePtree(root);
		free(root);
		free(p);
	}
	free(line);
	fclose(file);
}

