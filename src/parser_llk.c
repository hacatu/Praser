//parser_llk.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "_ptree.h"
#include "parser_llk.h"
#include "debug.h"


//TODO: replace AppendMode with three functions: add, skip, and pass, and make parsers return ptrees instead of taking them

struct Position{
	const char* string;
	const char* current;
};


void logUnexpectedError(Position *p, const char *name, const char *expected){
	//TODO: count lines and include the line/column number for errors, and print the line with the error highlighted.
	const char found[] = {currentChar(p), '\0'};
	printf("%s: expected %s but found \"%s\" at %li: \"%10.s\"\n", name, expected, found, p->current - p->string, p->current);
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
	debug_gets("%c",p->current[0]);
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


char acceptEnd(Position *p){
	return acceptChar(p, '\0');
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
			debug_log("appendNewPtree failed");
			return 0;
		}
		if(!parse(p, lastChild(t))){
			debug_log("parsing failed");
			reallocPtree(t, getSize(t) - 1);
			return 0;
		}
		return 1;
		case PASS:
		if(!parse(p, t)){
			debug_log("parsing failed");
			return 0;
		}
		return 1;
		case SKIP:
		temp = mallocPtree();
		if(!temp){
			debug_log("tempPtree failed");
			return 0;
		}
		if(!parse(p, temp)){
			debug_log("parsing failed");
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


int repeat(Position *p, Ptree *t, AppendMode a, parser parse, int min, int max){
	//t->string = __func__;
	int count = 0;
	while(count < min){
		if(!accept(p, t, a, parse)){
			debug_log("not enough matches");
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
	debug_log("successful");
	return 1;
}

int sepBy(Position *p, Ptree *t, AppendMode a, AppendMode aSeperator, parser parse, parser parseSeperator, int min, int max){
	int count = 1;
	//t->string = __func__;
	if(!accept(p, t, a, parse)){
		return min <= 0;
	}
	while(count < min){
		if(!accept(p, t, aSeperator, parseSeperator)){
			return 0;
		}
		if(!accept(p, t, a, parse)){
			return 0;
		}
		++count;
	}
	if(max > min){
		while(count < max){
			if(!accept(p, t, aSeperator, parseSeperator)){
				return 1;
			}
			if(!accept(p, t, a, parse)){
				return 0;
			}
			++count;
		}
	}if(max < min || max == 0){
		while(accept(p, t, aSeperator, parseSeperator)){
			if(!accept(p, t, a, parse)){
				return 0;
			}
		}
		return 1;
	}
	return 1;
}

int alternate(Position *p, Ptree *t, AppendMode aA, AppendMode aB, parser parseA, parser parseB){
	
	//t->string = __func__;
	while(accept(p, t, aA, parseA)){
		if(!accept(p, t, aB, parseB)){
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

int noneOf(Position *p, Ptree *t, AppendMode a, const char *options){
	const char *c = options;
	while(*c){
		if(currentChar(p) == *c){
			return 0;
		}
		++c;
	}
	c = p->current;
	getChar(p);
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

int cchar(Position *p, Ptree *t){
	if(currentChar(p) == '\\'){
		getChar(p);
		appendString(t, p->current, 1);
		getChar(p);
		return 1;
	}
	return noneOf(p, t, PASS, "\"");
}

int cstring(Position *p, Ptree *t){
	if(!acceptString(p, t, SKIP, "\"")){
		return 0;
	}
	if(!repeat(p, t, PASS, cchar, 0, 0)){
		logUnexpectedError(p, __func__, "a string");
		return 0;
	}
	if(!acceptString(p, t, SKIP, "\""));
	return 1;
}

int space(Position *p, Ptree *t){
	return oneOf(p, t, SKIP, " \t\n\r\v\f");
}

int spaces(Position *p, Ptree *t){
	return repeat(p, t, SKIP, space, 0, 0);
}

int letter(Position *p, Ptree *t){
	return oneOf(p, t, PASS, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

int digit(Position *p, Ptree *t){
	return oneOf(p, t, PASS, "0123456789");
}

int integer(Position *p, Ptree *t){
	return repeat(p, t, PASS, digit, 1, 0);
}



