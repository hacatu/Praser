//ptree_util.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "debug.h"

//Error functions, do not invlove Ptrees:    =========================
void logMemoryError(const char *name){
	printf("%s: out of memory.\n", name);
}


void updateChildPointers(Ptree *t){
	debug("called on: %p", t);
	Ptree *child;
	for(int i = 0; (child = nthChild(t, i)); ++i){
		child->parent = t;
	}
	debug("returning");
}

void updateGrandchildPointers(Ptree *t){
	debug("called on: %p", t);
	Ptree *child;
	for(int i = 0; (child = nthChild(t, i)); ++i){
		updateChildPointers(child);
	}
	debug("returning");
}

void deleteChildrenAfter(Ptree *t, int n){
	debug("called on: %p, %d", t, n);
	Ptree *current = nthChild(t, n + 1), *last = lastChild(t);
	while(current <= last){
		deletePtree(current);
		++current;
	}
	debug("returning");
}


//getters:    ========================================================
char isTerminal(const Ptree *t){
	debug("called on: %p", t);
	debug("returning: %i", !t->nodec);
	return !t->nodec;
}

int getSize(Ptree *t){
	debug("called on: %p", t);
	debug("returning: %i", t->nodec);
	return t->nodec;
}


//Functions to manipulate tree structure (TODO: add more)    =========
//TODO: make iterative
void flatten(Ptree *t){
	debug("called on: %p", t);
	Ptree *child, *parent = t->parent;
	while(getSize(t) == 1){
		child = nthChild(t, 0);
		free(t->string);
		t->length = child->length;
		t->nodec = child->nodec;
		t->nodes = child->nodes;
		t->string = child->string;
		free(child);
	}
	t->parent = parent;
	updateChildPointers(t);
	for(int i = 0; i < getSize(t); ++i){
		flatten(nthChild(t, i));
	}
	debug("returning");
}

//TODO: make iterative
void flattenTagged(Ptree *t){
	debug("called on: %p", t);
	Ptree *child, *parent = t->parent;
	while(getSize(t) == 1){
		child = nthChild(t, 0);
		if(isTerminal(child)){
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
	for(int i = 0; i < getSize(t); ++i){
		flattenTagged(nthChild(t, i));
	}
	debug("returning");
}


//Functions to access child nodes:    ================================
Ptree* nthChild(Ptree *t, int n){
	debug("called on: %p, %d", t, n);
	if(n < 0){
		n += t->nodec;
		if(n < 0){
			debug("returning: %p", 0);
			return 0;
		}
	}else if(n >= t->nodec){//if n is out of bounds, return a null pointer.
		debug("returning: %p", 0);
		return 0;
	}
	debug("returning: %p", t->nodes + n);
	return t->nodes + n;
}

Ptree* lastChild(Ptree *t){
	debug("called on: %p", t);
	if(dbg){
		Ptree *temp = nthChild(t, -1);
		debug("returning: %p", temp);
		return temp;
	}else{
		return nthChild(t, -1);
	}
}


//Functions to reallocate Ptrees:     ================================
char reallocPtree(Ptree *t, int size){
	debug("called on: %p, %d", t, size);
	if(size < getSize(t)){
		deleteChildrenAfter(t, size - 1);
	}
	Ptree *temp = realloc(t->nodes, size*sizeof(Ptree));
	if(!temp){
		logMemoryError("reallocPtree");
		debug("returning: 0");
		return 0;
	}
	if(temp != t->nodes){
		t->nodes = temp;
		t->nodec = size;
		updateGrandchildPointers(t);
		debug("returning: 2");
		return 2;
	}
	t->nodec = size;
	debug("returning: 1");
	return 1;
}

