//ptree_create.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "../util/debug.h"


//Functions to create PRA_Ptrees:    =====================================
/* Local function not in header
 * Create an empty PRA_Ptree.
 */
static PRA_Ptree newPtree(PRA_Ptree *parent, const char *string, int length){
	debug_calls("called on: %p, %*s, %d", parent, length, string, length);
	PRA_Ptree PRA_Ptree = {.parent = parent,
				   .nodec = 0,
				   .nodes = 0,
				   .length = 0,
				   .string = NULL
				  };
	PRA_setString(&PRA_Ptree, string, length);
	debug_calls("returning: %p", PRA_Ptree);
	return PRA_Ptree;
}

PRA_Ptree* PRA_mallocPtree(){
	debug_calls("called");
	if(dbg){
		PRA_Ptree *t = calloc(1, sizeof(PRA_Ptree));
		debug_calls("returning: %p", t);
		return t;
	}else{
		return calloc(1, sizeof(PRA_Ptree));
	}
}

char appendNewPtree(PRA_Ptree *parent, const char *string, int length){
	debug_calls("called on: %p, %*s, %d", parent, length, string, length);
	if(!reallocPtree(parent, parent->nodec + 1)){
		PRA_logMemoryError(__FUNCTION__);
		debug_calls("returning: 0");
		return 0;
	}
	PRA_setString(PRA_lastChild(parent), string, length);
	debug_calls("returning: 1");
	return 1;
}


//Fuctions to change a PRA_Ptree's string tag:    ========================
void PRA_setString(PRA_Ptree *p, const char *string, int length){
	debug_calls("called on: %p, %*s, %d", p, length, string, length);
	free(p->string);
	p->string = malloc((length + 1)*sizeof(char));
	if(!p->string){
		PRA_logMemoryError(__FUNCTION__);
		p->length = 0;
		debug_calls("returning");
		return;
	}
	p->length = length;
	strncpy(p->string, string, length);
	p->string[length] = '\0';
	debug_calls("returning");
}


void PRA_appendString(PRA_Ptree *p, const char *string, int length){
	debug_calls("called on: %p, %*s, %d", p, length, string, length);
	char *temp;
	temp = realloc(p->string, (p->length + length + 1)*sizeof(char));
	if(!temp){
		PRA_logMemoryError(__FUNCTION__);
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

static char copyChildren(PRA_Ptree *dest, const PRA_Ptree *src){
	PRA_Ptree *current;
	for(int i = 0; i < PRA_getSize(src); ++i){
		current = PRA_nthChild(src, i);
		if(!appendNewPtree(dest, current->string, current->length)){
			return 0;
		}
		if(!copyChildren(PRA_nthChild(dest, i), current)){
			return 0;
		}
	}
	return 1;
}

PRA_Ptree* PRA_copyPtree(const PRA_Ptree *t){
	PRA_Ptree *cpy = PRA_mallocPtree();
	*cpy = newPtree(NULL, t->string, t->length);
	if(!copyChildren(cpy, t)){
		puts("Failed to copy");
	}
	return cpy;
}

