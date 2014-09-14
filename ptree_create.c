//ptree_create.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "debug.h"


//Functions to create Ptrees:    =====================================
/* Local function not in header
 * Create an empty ptree.
 */
Ptree newPtree(Ptree *parent, const char *string, int length){
	debug("called on: %p, %*s, %d", parent, length, string, length);
	Ptree ptree = {.parent = parent,
				   .nodec = 0,
				   .nodes = 0,
				   .length = 0,
				   .string = NULL
				  };
	setString(&ptree, string, length);
	debug("returning: %p", ptree);
	return ptree;
}

Ptree* mallocPtree(){
	debug("called");
	if(dbg){
		Ptree *t = calloc(1, sizeof(Ptree));
		debug("returning: %p", t);
		return t;
	}else{
		return calloc(1, sizeof(Ptree));
	}
}

char appendNewPtree(Ptree *parent, const char *string, int length){
	debug("called on: %p, %*s, %d", parent, length, string, length);
	if(!reallocPtree(parent, parent->nodec + 1)){
		logMemoryError(__func__);
		debug("returning: 0");
		return 0;
	}
	*lastChild(parent) = newPtree(parent, string, length);
	debug("returning: 1");
	return 1;
}


//Fuctions to change a Ptree's string tag:    ========================
void setString(Ptree *p, const char *string, int length){
	debug("called on: %p, %*s, %d", p, length, string, length);
	free(p->string);
	p->string = malloc(length*sizeof(char));
	if(!p->string){
		logMemoryError(__func__);
		p->length = 0;
		debug("returning");
		return;
	}
	p->length = length;
	strncpy(p->string, string, length);
	debug("returning");
}

void appendString(Ptree *p, const char *string, int length){
	debug("called on: %p, %*s, %d", p, length, string, length);
	const char *temp;
	temp = realloc(p->string, (p->length + length)*sizeof(char));
	if(!temp){
		logMemoryError(__func__);
		debug("returning");
		return;
	}
	p->length += length;
	strncpy(p->string + p->length, string, length);
	debug("returning");
}

