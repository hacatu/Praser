//parser_llk.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "_ptree.h"
#include "parser_llk.h"
#include "debug.h"


//TODO: Fix PASS so that it is useful.


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
		appendNewPtree(t, s, strlen(s));
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
	int children = getSize(t);
	if(accept(&current, t, a, parse)){
		*p = current;
		return 1;
	}
	if(a != SKIP){
		reallocPtree(t, children);
	}
	return 0;
}


int accept(Position *p, Ptree *t, AppendMode a, parser parse){
	Ptree *temp;
	switch(a){
		case ADD:
		if(!appendNewPtree(t, NULL, 0)){
			debug("appendNewPtree failed");
			return 0;
		}
		if(!parse(p, lastChild(t))){
			debug("parsing failed");
			reallocPtree(t, getSize(t) - 1);
			return 0;
		}
		return 1;
		case PASS:
		if(!parse(p, t)){
			debug("parsing failed");
			return 0;
		}
		return 1;
		case SKIP:
		temp = mallocPtree();
		if(!temp){
			debug("tempPtree failed");
			return 0;
		}
		if(!parse(p, temp)){
			debug("parsing failed");
			deletePtree(temp);
			free(temp);
			return 0;
		}
		deletePtree(temp);
		free(temp);
		return 1;
	}
	//needed to supress erronious warning from gcc.  The above switch can't fall through because a is an enum.
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
	Ptree* temp = mallocPtree();
	if(!temp){
		logMemoryError(__func__);
		return 0;
	}
	if(parse(p, temp)){
		deletePtree(temp);
		free(temp);
		return 0;
	}
	deletePtree(temp);
	free(temp);
	switch(a){
		case ADD:
			return appendNewPtree(t, start.current, 1);
		case PASS:
			appendString(t, start.current, 1);
			return 1;
		case SKIP:
			return 1;
	}
	//needed to supress erronious warning from gcc.  The above switch can't fall through bcause a is an enum.
	return 1;
}

int oneOf(Position *p, Ptree *t, AppendMode a, const char *options){
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
		case ADD:
		return appendNewPtree(t, c, 1);
		case PASS:
		appendString(t, c, 1);
		return 1;
		case SKIP:
		return 1;
	}
	//needed to supress erronious warning from gcc.  The above switch can't fall through bcause a is an enum.
	return 1;
}


