//parser_llk.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptree.h"
#include "ptree_private.h"
#include "parser_llk.h"
#include "debug.h"


//TODO: Fix all string assignments to malloc (use helper function).  Fix PASS so that it is useful.


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


int acceptString(Position *p, Ptree *t, AppendMode a, const char *s){
	int i = 0;
	while(s[i]){
		if(!acceptChar(p, s[i])){
			return 0;
		}
			++i;
	}
	if(a == ADD || a == PASS){
		appendPtree(t, newPtree(t, s));
	}
	return 1;
}

int expectString(Position *p, Ptree *t, AppendMode a, const char *s){
	if(acceptString(p, t, a, s)){
		return 1;
	}
	logUnexpectedError(p, "expectString",  s);
	return 0;
}


char expectEnd(Position *p){
	return expectChar(p, '\0');
}


int try(Position *p, Ptree *t, AppendMode a, parser parse){
	Position current = *p;
	int size = t->nodec;
	if(accept(&current, t, a, parse)){
		*p = current;
		return 1;
	}
	if(a != SKIP){
		deleteChildrenAfter(t, size);
		reallocPtree(t, size);
	}
	return 0;
}


int accept(Position *p, Ptree *t, AppendMode a, parser parse){
	Ptree ptree;
	Ptree *temp = &ptree;
	if(a == PASS){
		temp = t;
	}else{
		ptree = newPtree(t, NULL);
	}
	if(!parse(p, temp)){
		debug("parsing failed");
		return 0;
	}
	if(a == ADD && !appendPtree(t, ptree)){
		debug("appendPtree failed");
		return 0;
	}
	return 1;
}

int expect(Position *p, Ptree *t, AppendMode a, parser parse){
	if(accept(p, t, a, parse)){
		return 1;
	}
	char expected[9];
	snprintf(expected, 9, "%p", parse);
	logUnexpectedError(p, "expect", expected);
	return 0;
}


int repeatMinMax(Position *p, Ptree *t, AppendMode a, parser parse, int min, int max){
	//t->string = __func__;
	int count = 0;
	while(count < min){
		if(!expect(p, t, a, parse)){
			debug("not enough matches");
			return 0;
		}
		++count;
	}
	if(max > 0){
		while(count < max){
			if(!accept(p, t, a, parse)){
				break;
			}
			++count;
		}
	}else{
		while(1){
			if(!accept(p, t, a, parse)){
				break;
			}
			++count;
		}
	}
	debug("successful");
	return 1;
}

int sepBy(Position *p, Ptree *t, AppendMode a, AppendMode aSeperator, parser parse, parser parseSeperator){
	
	//t->string = __func__;
	if(!accept(p, t, a, parse)){
		return 0;
	}
	while(accept(p, t, aSeperator, parseSeperator)){
		if(!expect(p, t, a, parse)){
			return 0;
		}
	}
	return 1;
}

int alternate(Position *p, Ptree *t, AppendMode aA, AppendMode aB, parser parseA, parser parseB){
	
	//t->string = __func__;
	while(accept(p, t, aA, parseA)){
		if(!expect(p, t, aB, parseB)){
			return 0;
		}
	}
	return 1;
}

int not(Position *p, Ptree *t, AppendMode a, parser parse){
	Position start = *p;
	Ptree temp = newPtree(t, NULL);
	if(parse(p, &temp)){
		deletePtree(&temp);
		return 0;
	}
	if(!getChar(p)){
		return 0;
	}
	if(a == SKIP){
		return 1;
	}
	temp = (Ptree){.parent = t, .length = 1, .string = start.current};
	if(!appendPtree(t, temp)){
		logMemoryError(__func__);
		return 0;
	}
	return 1;
}

int oneOf(Position *p, Ptree *t, AppendMode a, const char *options){
	Ptree *temp;
	Ptree ptree;
	if(a == PASS){
		temp = t;
	}else if(a == ADD){
		ptree = newPtree(t, NULL);
		temp = &ptree;
	}
	const char *c = options;
	while(*c){
		if(acceptChar(p, *c)){
			if(a == PASS){
				//you are here
				temp->string = c;
				temp->length +=1;
			}
			if(a == ADD && !appendPtree(t, ptree)){
				tem->length = 1;
				temp->string = c;
				logMemoryError("oneOf");
			}
			debug("successful");
			return 1;
		}
		++c;
	}
	return 0;
}

