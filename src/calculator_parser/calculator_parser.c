//calculator_parser.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../util/get_line.h"
#include "../ptree/ptree.h"
#include "../parser/parser.h"
#include "calculator_interpreter.h"
#include "../util/debug.h"


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


int start(Position *p, Ptree *t){
	setString(t, __func__, strlen(__func__));
	if(!accept(p, t, PASS, additive)){
		logUnexpectedError(p, "start", "additive");
		return 0;
	}
	if(!acceptEnd(p)){
		logUnexpectedError(p, "start", "end of input");
		return 0;
	}
	return 1;
}

int additive(Position *p, Ptree *t){
	setString(t, __func__, strlen(__func__));
	return sepBy(p, t, ADD, ADD, multiplicative, addition, 1, 0);
}

int addition(Position *p, Ptree *t){
	//setString(t, __func__, strlen(__func__));
	return oneOf(p, t, PASS, "+-");
}

int multiplicative(Position *p, Ptree *t){
	setString(t, __func__, strlen(__func__));
	return sepBy(p, t, ADD, ADD, primary, multiplication, 1, 0);
}

int multiplication(Position *p, Ptree *t){
	//setString(t, __func__, strlen(__func__));
	return oneOf(p, t, PASS, "*/%");
}

int primary(Position *p, Ptree *t){
	setString(t, __func__, strlen(__func__));
	if(acceptString(p, t, SKIP, "(")){
		if(!accept(p, t, PASS, additive)){
			logUnexpectedError(p, "primary", "additive");
			return 0;
		}
		if(!acceptString(p, t, SKIP, ")")){
			logUnexpectedError(p, "primary", ")");
			return 0;
		}
		return 1;
	}
	return accept(p, t, ADD, integer);
}

int main(){
	Ptree *t;
	Position *p;
	size_t size = 0;
	char *line = 0;
	int read;
	while((read = getLine(&line, &size, stdin)) > 0){
		line[read - 1] = '\0';
		printf("\"%s\"\n", line);
		p = firstPosition(line);
		t = mallocPtree();
		if(start(p, t)){
			puts("String parsed successfully!  Output:");
			flattenTagged(t);
			printPtree(t, 0);
			printf("Evaluated: %f\n", eval(t));
		}else{
			puts("String parsed unsuccessfully!");
		}
		deletePtree(t);
		free(t);
		free(p);
	}
	free(line);
}
