//calculator_parser.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <get_line.h>
#include <parser.h>
#include <ptree.h>
#include "calculator_interpreter.h"
#include <debug.h>


/*GRAMMAR:
start = additive

additive = multiplicative "+" additive | multiplicative

multiplicative = primary "*" multiplicative | primary

primary = integer | "(" additive ")"

integer = [0-9]+
*/


int start(PRA_Position *p, PRA_Ptree *t);
int additive(PRA_Position *p, PRA_Ptree *t);
int addition(PRA_Position *p, PRA_Ptree *t);
int multiplicative(PRA_Position *p, PRA_Ptree *t);
int multiplication(PRA_Position *p, PRA_Ptree *t);
int primary(PRA_Position *p, PRA_Ptree *t);


int start(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	if(!PRA_accept(p, t, PRA_PASS, additive)){
		PRA_logUnexpectedError(p, "start", "additive");
		return 0;
	}
	if(!PRA_acceptEnd(p)){
		PRA_logUnexpectedError(p, "start", "end of input");
		return 0;
	}
	return 1;
}

int additive(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	return PRA_sepBy(p, t, PRA_ADD, PRA_ADD, multiplicative, addition, 1, 0);
}

int addition(PRA_Position *p, PRA_Ptree *t){
	//PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	return PRA_oneOf(p, t, PRA_PASS, "+-");
}

int multiplicative(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	return PRA_sepBy(p, t, PRA_ADD, PRA_ADD, primary, multiplication, 1, 0);
}

int multiplication(PRA_Position *p, PRA_Ptree *t){
	//PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	return PRA_oneOf(p, t, PRA_PASS, "*/%");
}

int primary(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	if(PRA_acceptString(p, t, PRA_SKIP, "(")){
		if(!PRA_accept(p, t, PRA_PASS, additive)){
			PRA_logUnexpectedError(p, "primary", "additive");
			return 0;
		}
		if(!PRA_acceptString(p, t, PRA_SKIP, ")")){
			PRA_logUnexpectedError(p, "primary", ")");
			return 0;
		}
		return 1;
	}
	return PRA_accept(p, t, PRA_ADD, PRA_integer);
}

int main(){
	PRA_Ptree *t;
	PRA_Position *p;
	size_t size = 0;
	char *line = 0;
	int read;
	while((read = getLine(&line, &size, stdin)) > 0){
		line[read - 1] = '\0';
		printf("\"%s\"\n", line);
		p = PRA_firstPosition(line);
		t = PRA_mallocPtree();
		if(start(p, t)){
			puts("String parsed successfully!  Output:");
			PRA_flattenTagged(t);
			PRA_printPtree(t, 0);
			printf("Evaluated: %f\n", eval(t));
		}else{
			puts("String parsed unsuccessfully!");
		}
		PRA_deletePtree(t);
		free(t);
		free(p);
	}
	free(line);
}

