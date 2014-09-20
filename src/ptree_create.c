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
	debug_calls("called on: %p, %*s, %d", parent, length, string, length);
	Ptree ptree = {.parent = parent,
				   .nodec = 0,
				   .nodes = 0,
				   .length = 0,
				   .string = NULL
				  };
	setString(&ptree, string, length);
	debug_calls("returning: %p", ptree);
	return ptree;
}

Ptree* mallocPtree(){
	debug_calls("called");
	if(dbg){
		Ptree *t = calloc(1, sizeof(Ptree));
		debug_calls("returning: %p", t);
		return t;
	}else{
		return calloc(1, sizeof(Ptree));
	}
}

char appendNewPtree(Ptree *parent, const char *string, int length){
	debug_calls("called on: %p, %*s, %d", parent, length, string, length);
	if(!reallocPtree(parent, parent->nodec + 1)){
		logMemoryError(__func__);
		debug_calls("returning: 0");
		return 0;
	}
	*lastChild(parent) = newPtree(parent, string, length);
	debug_calls("returning: 1");
	return 1;
}


//Fuctions to change a Ptree's string tag:    ========================
void setString(Ptree *p, const char *string, int length){
	debug_calls("called on: %p, %*s, %d", p, length, string, length);
	free(p->string);
	p->string = malloc((length + 1)*sizeof(char));
	if(!p->string){
		logMemoryError(__func__);
		p->length = 0;
		debug_calls("returning");
		return;
	}
	p->length = length;
	strncpy(p->string, string, length);
	p->string[length] = '\0';
	debug_calls("returning");
}


void appendString(Ptree *p, const char *string, int length){
	debug_calls("called on: %p, %*s, %d", p, length, string, length);
	char *temp;
	temp = realloc(p->string, (p->length + length + 1)*sizeof(char));
	if(!temp){
		logMemoryError(__func__);
		debug_calls("returning");
		return;
	}
	p->string = temp;
	strncpy(p->string + p->length, string, length);
	p->length += length;
	p->string[p->length] = '\0';
	debug_calls("returning");
}

//TODO: make iterative

char copyChildren(Ptree *dest, const Ptree *src){
	Ptree *current;
	for(int i = 0; i < getSize(src); ++i){
		current = nthChild(src, i);
		if(!appendNewPtree(dest, current->string, current->length)){
			return 0;
		}
		if(!copyChildren(nthChild(dest, i), current)){
			return 0;
		}
	}
	return 1;
}

Ptree* copyPtree(const Ptree *t){
	Ptree *cpy = mallocPtree();
	*cpy = newPtree(NULL, t->string, t->length);
	if(!copyChildren(cpy, t)){
		puts("Failed to copy");
	}
	return cpy;
}

