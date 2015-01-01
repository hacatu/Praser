//ptree_util.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "../util/debug.h"

//Error functions, do not invlove PRA_Ptrees:    =========================
void PRA_logMemoryError(const char *name){
	printf("%s: out of memory.\n", name);
}


static void updateChildPointers(PRA_Ptree *t){
	PRA_Ptree *child;
	for(int i = 0; (child = PRA_nthChild(t, i)); ++i){
		child->parent = t;
	}
}

static void updateGrandchildPointers(PRA_Ptree *t){
	PRA_Ptree *child;
	for(int i = 0; (child = PRA_nthChild(t, i)); ++i){
		updateChildPointers(child);
	}
}

static void deleteChildrenAfter(PRA_Ptree *t, int n){
	PRA_Ptree *current = PRA_nthChild(t, n + 1), *last = PRA_lastChild(t);
	if(!current){
		return;
	}
	while(current <= last){
		PRA_deletePtree(current);
		++current;
	}
}


//getters:    ========================================================
char PRA_isTerminal(const PRA_Ptree *t){
	return !t->nodec;
}

int PRA_getSize(const PRA_Ptree *t){
	return t->nodec;
}

int PRA_getLength(const PRA_Ptree *t){
	return t->length;
}

const char* PRA_getString(PRA_Ptree *t){
	return t->string;
}

//Functions to manipulate tree structure (TODO: Add more)    =========
//TODO: make iterative
void PRA_flatten(PRA_Ptree *t){
	PRA_Ptree *child, *parent = t->parent;
	while(PRA_getSize(t) == 1){
		child = PRA_nthChild(t, 0);
		free(t->string);
		t->length = child->length;
		t->nodec = child->nodec;
		t->nodes = child->nodes;
		t->string = child->string;
		free(child);
	}
	t->parent = parent;
	updateChildPointers(t);
	for(int i = 0; i < PRA_getSize(t); ++i){
		PRA_flatten(PRA_nthChild(t, i));
	}
}

//TODO: make iterative
void PRA_flattenTagged(PRA_Ptree *t){
	PRA_Ptree *child, *parent = t->parent;
	while(PRA_getSize(t) == 1){
		child = PRA_nthChild(t, 0);
		if(PRA_isTerminal(child)){
			break;
		}
		free(t->string);
		t->length = child->length;
		t->nodec = child->nodec;
		t->nodes = child->nodes;
		t->string = child->string;
		free(child);
	}
	t->parent = parent;
	updateChildPointers(t);
	for(int i = 0; i < PRA_getSize(t); ++i){
		PRA_flattenTagged(PRA_nthChild(t, i));
	}
}

void PRA_flattenSelf(PRA_Ptree *t){
	PRA_Ptree *child, *parent = t->parent;
	if(PRA_getSize(t) == 1){
		child = PRA_nthChild(t, 0);
		free(t->string);
		t->length = child->length;
		t->nodec = child->nodec;
		t->nodes = child->nodes;
		t->string = child->string;
		free(child);
	}
	t->parent = parent;
	updateChildPointers(t);
}


//Get the parent
PRA_Ptree* PRA_parent(PRA_Ptree *t){
	return t->parent;
}


//Functions to access child nodes:    ================================
PRA_Ptree* PRA_nthChild(const PRA_Ptree *t, int n){
	if(n < 0){
		n += t->nodec;
		if(n < 0){
			return NULL;
		}
	}else if(n >= t->nodec){//if n is out of bounds, return a null pointer.
		return NULL;
	}
	return t->nodes + n;
}

PRA_Ptree* PRA_lastChild(PRA_Ptree *t){
	if(dbg){
		PRA_Ptree *temp = PRA_nthChild(t, -1);
		return temp;
	}else{
		return PRA_nthChild(t, -1);
	}
}


//Functions to reallocate PRA_Ptrees:     ================================
char reallocPtree(PRA_Ptree *t, int size){
	if(size == 0){
		deleteChildrenAfter(t, -1);
		free(t->nodes);
		t->nodes = 0;
		t->nodec = 0;
		return 3;
	}
	if(size < PRA_getSize(t)){
		deleteChildrenAfter(t, size - 1);
	}
	PRA_Ptree *temp = realloc(t->nodes, size*sizeof(PRA_Ptree));
	if(!temp){
		PRA_logMemoryError(__FUNCTION__);
		return 0;
	}
	if(size > t->nodec){
		memset(temp + t->nodec, 0, (size - t->nodec)*sizeof(PRA_Ptree));//temp:[0,1,...,t->nodec-1,t->nodec,...,size-1]
	}
	if(temp != t->nodes){
		t->nodes = temp;
		t->nodec = size;
		updateGrandchildPointers(t);
		return 2;
	}
	t->nodec = size;
	return 1;
}


//Functions to iterate over PRA_Ptrees:
PRA_Ptree* PRA_firstPostorder(PRA_Ptree *root){
	while(!PRA_isTerminal(root)){
		root = PRA_nthChild(root, 0);
	}
	return root;
}

PRA_Ptree* PRA_nextPostorder(PRA_Ptree *current){
	PRA_Ptree *parent = current->parent;
	if(!parent){
		return NULL;
	}
	++current;
	if(current - parent->nodes == parent->nodec){
		return parent;
	}
	return PRA_firstPostorder(current);
}

