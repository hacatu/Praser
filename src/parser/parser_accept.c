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
		//Move to the next char.
		PRA_getChar(p);
		return 1;
	}
	return 0;
}

int PRA_acceptString(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *s){
	for(const char *c = s; *c != '\0'; ++c){//loop over every char in the string s.
		if(!acceptChar(p, *c)){
			return 0;
		}
	}
	switch(a){
		case PRA_ADD:
			return appendNewPtree(t, s, strlen(s));
		case PRA_PASS:
			PRA_appendString(t, s, strlen(s));
			return 1;
		case PRA_SKIP:
			return 1;
		default:
			return 0;
	}
}

int PRA_try(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse){
	PRA_Position *startPos = copyPosition(p);//create a copy of the position to reset to if parsing fails
	if(!startPos){//copyPosition failed
		return 0;
	}
	int children = PRA_getSize(t);
	if(PRA_accept(p, t, a, parse)){
		PRA_deletePosition(startPos);
		free(startPos);
		return 1;
	}
	if(a != PRA_SKIP){
		reallocPtree(t, children);
	}
	resetIndex(p, startPos);
	//do not PRA_deletePosition startPos because it is a copy of p so their user state points to the same memory.
	free(startPos);
	return 0;
}

int PRA_accept(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse){
	PRA_Ptree *tempPtree;
	switch(a){
		case PRA_ADD:
			//Append an empty Ptree to add to to the input ptree.
			if(!appendNewPtree(t, NULL, 0)){
				return 0;
			}
			//call the parse function on the current position and the empty Ptree
			if(!parse(p, PRA_lastChild(t))){
				reallocPtree(t, PRA_getSize(t) - 1);
				return 0;
			}
			return 1;
		case PRA_PASS:
			//call the parse function on the current position and Ptree.
			if(!parse(p, t)){
				return 0;
			}
			return 1;
		case PRA_SKIP:
			//create a temporary Ptree.
			tempPtree = PRA_mallocPtree();
			if(!tempPtree){
				return 0;
			}
			//call the parse function on the current position and the temporary Ptree.
			if(!parse(p, tempPtree)){
				PRA_deletePtree(tempPtree);
				free(tempPtree);
				return 0;
			}
			//delete the temporary Ptree.
			PRA_deletePtree(tempPtree);
			free(tempPtree);
			return 1;
		default:
			return 0;
	}
}

PRA_Ptree *PRA_parseFile(FILE *file, PRA_parser parse){
	//Initialize a PRA_Position to use for parsing.
	PRA_Position *p = startPosition(file);
	if(!p){
		return NULL;
	}
	//Initialize a PRA_Ptree to use for parsing.
	PRA_Ptree *t = PRA_mallocPtree();
	if(!t){
		PRA_deletePosition(p);
		free(p);
		return NULL;
	}
	//try the parser
	if(!PRA_accept(p, t, PRA_PASS, parse)){
		free(t);
		t = NULL;
	}
	PRA_deletePosition(p);
	free(p);
	return t;
}

PRA_Ptree *PRA_parseString(char *string, PRA_parser parse){
	//Initialize a PRA_Position to use for parsing.
	PRA_Position *p = firstPosition(string);
	if(!p){
		return NULL;
	}
	//Initialize a PRA_Ptree to use for parsing.
	PRA_Ptree *t = PRA_mallocPtree();
	if(!t){
		PRA_deletePosition(p);
		free(p);
		return NULL;
	}
	//try the parser
	if(!PRA_accept(p, t, PRA_PASS, parse)){
		free(t);
		t = NULL;
	}
	PRA_deletePosition(p);
	free(p);
	return t;
}

