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
int acceptChar(PRA_Position *p, char c){
	if(PRA_currentChar(p) == c){
		PRA_getChar(p);
		return 1;
	}
	return 0;
}

int PRA_acceptString(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *s){
	int i = 0;
	while(s[i]){
		if(!acceptChar(p, s[i])){
			return 0;
		}
			++i;
	}
	switch(a){
		case PRA_ADD:
		return appendNewPtree(t, s, strlen(s));
		case PRA_PASS:
		PRA_appendString(t, s, strlen(s));
		return 1;
		case PRA_SKIP:
		return 1;
	}
	//not reachable
	return 0;
}

int PRA_try(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse){
	PRA_Position *b = copyPosition(p);
	if(!b){
		return 0;
	}
	int children = PRA_getSize(t);
	if(PRA_accept(p, t, a, parse)){
		PRA_deletePosition(b);
		free(b);
		return 1;
	}
	if(a != PRA_SKIP){
		reallocPtree(t, children);
	}
	resetIndex(p, b);
	//PRA_deletePosition(b);
	free(b);
	return 0;
}

int PRA_accept(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse){
	PRA_Ptree *temp;
	switch(a){
		case PRA_ADD:
		if(!appendNewPtree(t, NULL, 0)){
			debug_log("appendNewPtree failed");
			return 0;
		}
		if(!parse(p, PRA_lastChild(t))){
			debug_log("parsing failed");
			reallocPtree(t, PRA_getSize(t) - 1);
			return 0;
		}
		return 1;
		case PRA_PASS:
		if(!parse(p, t)){
			debug_log("parsing failed");
			return 0;
		}
		return 1;
		case PRA_SKIP:
		temp = PRA_mallocPtree();
		if(!temp){
			debug_log("tempPRA_Ptree failed");
			return 0;
		}
		if(!parse(p, temp)){
			debug_log("parsing failed");
			PRA_deletePtree(temp);
			free(temp);
			return 0;
		}
		PRA_deletePtree(temp);
		free(temp);
		return 1;
	}
	//needed to supress erronious warning from gcc.  The above switch can't fall through because a is an enum.
	return 1;
}

PRA_Ptree *PRA_parseFile(FILE *file, PRA_parser parse){
	PRA_Position *p = startPosition(file);
	if(!p){
		return NULL;
	}
	PRA_Ptree *t = PRA_mallocPtree();
	if(!t){
		PRA_deletePosition(p);
		free(p);
		return NULL;
	}
	if(!parse(p, t)){
		free(t);
		t = NULL;
	}
	PRA_deletePosition(p);
	free(p);
	return t;
}

PRA_Ptree *PRA_parseString(char *string, PRA_parser parse){
	PRA_Position *p = firstPosition(string);
	if(!p){
		return NULL;
	}
	PRA_Ptree *t = PRA_mallocPtree();
	if(!t){
		PRA_deletePosition(p);
		free(p);
		return NULL;
	}
	if(!parse(p, t)){
		free(t);
		t = NULL;
	}
	PRA_deletePosition(p);
	free(p);
	return t;
}

