//parser_combinators.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ptree/_ptree.h"
#include "_parser.h"
#include "../util/debug.h"

int PRA_repeat(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse, int min, int max){
	int count = 0;
	while(count < min){//make sure parse matches at least min times
		if(!PRA_accept(p, t, a, parse)){
			return 0;
		}
		++count;
	}
	if(max > 0){
		while(count < max){//accept up to max times
			if(!PRA_accept(p, t, a, parse)){
				break;
			}
			++count;
		}
	}else{//max <= 0 so there is no max
		while(1){//accept indefinitely
			if(!PRA_accept(p, t, a, parse)){
				break;
			}
			++count;
		}
	}
	return 1;
}

int PRA_sepBy(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_AppendMode aSeparator, PRA_parser parse, PRA_parser parseSeparator, int min, int max){
	int count = 1;//start the count at 1 because the next step ensures it will be.
	if(!PRA_accept(p, t, a, parse)){//try to accept parse
		return min <= 0;//if unsuccessful, return true if min <= 0, otherwise false
	}
	while(count < min){//make sure parse accepts at least min times
		if(!PRA_accept(p, t, aSeparator, parseSeparator)){
			return 0;
		}
		if(!PRA_accept(p, t, a, parse)){
			return 0;
		}
		++count;
	}
	if(max > min){//normal operation
		while(count < max){//continue accepting parse until max is reached
			if(!PRA_accept(p, t, aSeparator, parseSeparator)){
				return 1;
			}
			if(!PRA_accept(p, t, a, parse)){
				return 0;
			}
			++count;
		}
	}
	if(max < min || max == 0){//unbounded operation
		while(PRA_accept(p, t, aSeparator, parseSeparator)){
			if(!PRA_accept(p, t, a, parse)){
				return 0;
			}
		}
		return 1;
	}
	return 1;
}

int PRA_alternate(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode aA, PRA_AppendMode aB, PRA_parser parseA, PRA_parser parseB){
	while(PRA_accept(p, t, aA, parseA)){
		if(!PRA_accept(p, t, aB, parseB)){
			return 0;
		}
	}
	return 1;
}

int PRA_not(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse){
	//This function operates exactly the same way as PRA_try
	PRA_Position *startPos = copyPosition(p);
	if(!startPos){
		PRA_logMemoryError(__FUNCTION__);
		return 0;
	}
	PRA_Ptree* tempPtree = PRA_mallocPtree();
	if(!tempPtree){
		PRA_logMemoryError(__FUNCTION__);
		PRA_deletePosition(p);
		free(startPos);
		return 0;
	}
	if(parse(p, tempPtree)){
		PRA_deletePtree(tempPtree);
		free(tempPtree);
		resetIndex(p, startPos);
		PRA_deletePosition(p);
		free(startPos);
		return 0;
	}
	PRA_deletePtree(tempPtree);
	free(tempPtree);
	resetIndex(p, startPos);
	PRA_deletePosition(p);
	free(startPos);
	//until here where it adds a string constructed from the current char.
	//Note that string is automatically allocated and so is invalid when this function
	//goes out of scope, but it gets copied to dynamically allocated memory so it is ok.
	const char string[] = {PRA_getChar(p), '\0'};
	switch(a){
		case PRA_ADD:
			return appendNewPtree(t, string, 1);
		case PRA_PASS:
			PRA_appendString(t, string, 1);
			return 1;
		case PRA_SKIP:
			PRA_getChar(p);
			return 1;
		default:
			return 1;
	}
}

int PRA_oneOf(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *options){
	const char *c = options;
	while(*c != '\0'){//loop over all chars in the string options
		if(acceptChar(p, *c)){
			break;
		}
		++c;
	}
	if(*c == '\0'){//none of the chars matched.
		return 0;
	}
	switch(a){
		case PRA_ADD:
			return appendNewPtree(t, c, 1);
		case PRA_PASS:
			PRA_appendString(t, c, 1);
			return 1;
		case PRA_SKIP:
			return 1;
		default:
			return 1;
	}
}

int PRA_noneOf(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *options){
	const char *c = options;
	if(PRA_currentChar(p) == '\0'){//if the current char indicates end of input
		return 0;
	}
	while(*c != '\0'){//loop over every char as in PRA_oneOf
		if(PRA_currentChar(p) == *c){
			return 0;
		}
		++c;
	}
	//string is automatic memory but gets copied to dynamic memory.
	const char string[] = {PRA_currentChar(p), '\0'};
	PRA_getChar(p);
	switch(a){
		case PRA_ADD:
			return appendNewPtree(t, string, 1);
		case PRA_PASS:
			PRA_appendString(t, string, 1);
			return 1;
		case PRA_SKIP:
			return 1;
		default:
			return 0;
	}
}

