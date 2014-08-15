//parser_llk.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptree.h"
#include "parser_llk.h"

struct Position{
	const char* string;
	const char* current;
};


void logUnexpectedError(Position *p, const char *name, const char *expected){
	//TODO: count lines and include the line/column number for errors, and print the line with the error highlighted.
	const char found[] = {currentChar(p), '\0'};
	printf("%s: expected %s but found %s\n", name, expected, found);
}


Position* firstPosition(const char *string){
	Position *start = malloc(1*sizeof(Position));
	*start = (Position){string, string};
	return start;
}


char currentChar(Position *p){
	return *p->current;
}

char getChar(Position *p){
	return *(++p->current);
}

char nextChar(Position *p){
	return p->current[1];
}

char nthChar(Position *p, int n){
	return p->current[n];
}

/* Local function not in header:
 * Attempts to parse the char c.
 * If successfull, get the next char and return 1.
 * Otherwise return 0.
 */
int acceptChar(Position *p, char c){
	if(currentChar(p) == c){
		getChar(p);
		return 1;
	}
	return 0;
}

/* Local function not in header:
 * If the char c is not acceptChar'd, call logUnexpectedError and return 0.
 * Otherwise return 1.
 */
int expectChar(Position *p, char c){
	if(acceptChar(p, c)){
		return 1;
	}
	const char expected[] = {c, '\0'};
	logUnexpectedError(p, "expectChar", expected);
	return 0;
}


int acceptString(Position *p, Ptree *t, const char *s){
	int i = 0;
	while(s[i]){
		if(!acceptChar(p, s[i])){
			return 0;
		}
			++i;
	}
	appendPtree(t, newPtree(t, s));
	return 1;
}

int expectString(Position *p, Ptree *t, const char *s){
	if(acceptString(p, t, s)){
		return 1;
	}
	logUnexpectedError(p, "expectString",  s);
	return 0;
}


char expectEnd(Position *p){
	return expectChar(p, '\0');
}


int try(Position *p, Ptree *t, parser parse){
	Position current = *p;
	if(accept(&current, t, parse)){
		*p = current;
		return 1;
	}
	return 0;
}


int accept(Position *p, Ptree *t, parser parse){
	Ptree temp = newPtree(t, NULL);
	if(!parse(p, &temp)){
		return 0;
	}
	if(!appendPtree(t, temp)){
		return 0;
	}
	return 1;
}

int expect(Position *p, Ptree *t, parser parse){
	if(accept(p, t, parse)){
		return 1;
	}
	char expected[9];
	snprintf(expected, 9, "%p", parse);
	logUnexpectedError(p, "expect", expected);
	return 0;
}


int repeatMinMax(Position *p, Ptree *t, parser parse, int min, int max){
	//t->string = __func__;
	int count = 0;
	while(count < min){
		if(!expect(p, t, parse)){
			return 0;
		}
		++count;
	}
	if(max > 0){
		while(count < max){
			if(!accept(p, t, parse)){
				break;
			}
			++count;
		}
	}else{
		while(1){
			if(!accept(p, t, parse)){
				break;
			}
			++count;
		}
	}
	return 1;
}

int sepBy(Position *p, Ptree *t, parser parse, parser parseSeperator){
	//t->string = __func__;
	if(!accept(p, t, parse)){
		return 0;
	}
	while(accept(p, t, parseSeperator)){
		if(!expect(p, t, parse)){
			return 0;
		}
	}
	return 1;
}

int alternate(Position *p, Ptree *t, parser parseA, parser parseB){
	//t->string = __func__;
	while(accept(p, t, parseA)){
		if(!expect(p, t, parseB)){
			return 0;
		}
	}
	return 1;
}

int takeWhileNot(Position *p, Ptree *t, parser parse){
	//t->string = __func__;
	Position start = *p;
	int c = 0;
	Ptree temp = newPtree(t, NULL);
	while(!parse(p, &temp)){
		if(!getChar(p)){
			return 0;
		}
		++c;
	}
	if(!appendPtree(t, (Ptree){.parent = t, .length = c, .string = start.current})){
		logMemoryError(__func__);
		return 0;
	}
	if(!appendPtree(t, temp)){
		logMemoryError(__func__);
		return 0;
	}
	return 1;
}

int oneOf(Position *p, Ptree *t, const char *options){
	t->length = 1;
	const char *c = options;
	while(*c){
		if(acceptChar(p, *c)){
			t->string = c;
			return 1;
		}
		++c;
	}
	return 0;
}

