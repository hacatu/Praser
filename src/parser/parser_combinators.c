//parser_combinators.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ptree/_ptree.h"
#include "_parser.h"
#include "../util/debug.h"

int PRA_repeat(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse, int min, int max){
	//t->string = __FUNCTION__;
	int count = 0;
	while(count < min){
		if(!PRA_accept(p, t, a, parse)){
			debug_log("PRA_not enough matches");
			return 0;
		}
		++count;
	}
	if(max > 0){
		while(count < max){
			if(!PRA_accept(p, t, a, parse)){
				break;
			}
			++count;
		}
	}else{
		while(1){
			if(!PRA_accept(p, t, a, parse)){
				break;
			}
			++count;
		}
	}
	debug_log("successful");
	return 1;
}

int PRA_sepBy(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_AppendMode aSeperator, PRA_parser parse, PRA_parser parseSeperator, int min, int max){
	int count = 1;
	//t->string = __FUNCTION__;
	if(!PRA_accept(p, t, a, parse)){
		return min <= 0;
	}
	while(count < min){
		if(!PRA_accept(p, t, aSeperator, parseSeperator)){
			return 0;
		}
		if(!PRA_accept(p, t, a, parse)){
			return 0;
		}
		++count;
	}
	if(max > min){
		while(count < max){
			if(!PRA_accept(p, t, aSeperator, parseSeperator)){
				return 1;
			}
			if(!PRA_accept(p, t, a, parse)){
				return 0;
			}
			++count;
		}
	}if(max < min || max == 0){
		while(PRA_accept(p, t, aSeperator, parseSeperator)){
			if(!PRA_accept(p, t, a, parse)){
				return 0;
			}
		}
		return 1;
	}
	return 1;
}

int PRA_alternate(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode aA, PRA_AppendMode aB, PRA_parser parseA, PRA_parser parseB){
	
	//t->string = __FUNCTION__;
	while(PRA_accept(p, t, aA, parseA)){
		if(!PRA_accept(p, t, aB, parseB)){
			return 0;
		}
	}
	return 1;
}

int PRA_not(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse){
	size_t start = currentIndex(p);
	PRA_Ptree* temp = PRA_mallocPtree();
	if(!temp){
		PRA_logMemoryError(__FUNCTION__);
		return 0;
	}
	if(parse(p, temp)){
		PRA_deletePtree(temp);
		free(temp);
		resetIndex(p, start);
		return 0;
	}
	PRA_deletePtree(temp);
	free(temp);
	resetIndex(p, start);
	switch(a){
		case PRA_ADD:
			return appendNewPtree(t, (const char[]){PRA_getChar(p), '\0'}, 1);
		case PRA_PASS:
			PRA_appendString(t, (const char[]){PRA_getChar(p), '\0'}, 1);
			return 1;
		case PRA_SKIP:
			PRA_getChar(p);
			return 1;
	}
	//needed to supress erronious warning from gcc.  The above switch can't fall through bcause a is an enum.
	return 1;
}

int PRA_oneOf(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *options){
	const char *c = options;
	while(*c){
		if(acceptChar(p, *c)){
			break;
		}
		++c;
	}
	if(!*c){
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
	}
	//needed to supress erronious warning from gcc.  The above switch can't fall through bcause a is an enum.
	return 1;
}

int PRA_noneOf(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *options){
	const char *c = options;
	while(*c){
		if(PRA_currentChar(p) == *c){
			return 0;
		}
		++c;
	}
	switch(a){
		case PRA_ADD:
			if(!appendNewPtree(t, (const char[]){PRA_currentChar(p), '\0'}, 1)){
				return 0;
			}
		case PRA_PASS:
			PRA_appendString(t, (const char[]){PRA_currentChar(p), '\0'}, 1);
		case PRA_SKIP:;
	}
	PRA_getChar(p);
	return 1;
}

