//parser_accept.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ptree/_ptree.h"
#include "_parser.h"
#include "../util/debug.h"

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

int try(Position *p, Ptree *t, AppendMode a, parser parse){
	size_t start = currentIndex(p);
	int children = getSize(t);
	if(accept(p, t, a, parse)){
		return 1;
	}
	if(a != SKIP){
		reallocPtree(t, children);
	}
	resetIndex(p, start);
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

