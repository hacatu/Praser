//ptree_util.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "../util/debug.h"

//Error functions, do not invlove Ptrees:    =========================
void logMemoryError(const char *name){
	printf("%s: out of memory.\n", name);
}


void updateChildPointers(Ptree *t){
	debug_calls("called on: %p", t);
	Ptree *child;
	for(int i = 0; (child = nthChild(t, i)); ++i){
		child->parent = t;
	}
	debug_calls("returning");
}

void updateGrandchildPointers(Ptree *t){
	debug_calls("called on: %p", t);
	Ptree *child;
	for(int i = 0; (child = nthChild(t, i)); ++i){
		updateChildPointers(child);
	}
	debug_calls("returning");
}

void deleteChildrenAfter(Ptree *t, int n){
	debug_calls("called on: %p, %d", t, n);
	Ptree *current = nthChild(t, n + 1), *last = lastChild(t);
	while(current <= last){
		deletePtree(current);
		++current;
	}
	debug_calls("returning");
}


//getters:    ========================================================
char isTerminal(const Ptree *t){
	debug_calls("called on: %p", t);
	debug_calls("returning: %i", !t->nodec);
	return !t->nodec;
}

int getSize(const Ptree *t){
	debug_calls("called on: %p", t);
	debug_calls("returning: %i", t->nodec);
	return t->nodec;
}

int getLength(const Ptree *t){
	return t->length;
}

const char* getString(Ptree *t){
	return t->string;
}

//Functions to manipulate tree structure (TODO: add more)    =========
//TODO: make iterative
void flatten(Ptree *t){
	debug_calls("called on: %p", t);
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
	debug_calls("returning");
}

//TODO: make iterative
void flattenTagged(Ptree *t){
	debug_calls("called on: %p", t);
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
	debug_calls("returning");
}


//Get the parent
Ptree* parent(Ptree *t){
	return t->parent;
}


//Functions to access child nodes:    ================================
Ptree* nthChild(const Ptree *t, int n){
	debug_calls("called on: %p, %d", t, n);
	if(n < 0){
		n += t->nodec;
		if(n < 0){
			debug_calls("returning: %p", 0);
			return 0;
		}
	}else if(n >= t->nodec){//if n is out of bounds, return a null pointer.
		debug_calls("returning: %p", 0);
		return 0;
	}
	debug_calls("returning: %p", t->nodes + n);
	return t->nodes + n;
}

Ptree* lastChild(Ptree *t){
	debug_calls("called on: %p", t);
	if(dbg){
		Ptree *temp = nthChild(t, -1);
		debug_calls("returning: %p", temp);
		return temp;
	}else{
		return nthChild(t, -1);
	}
}


//Functions to reallocate Ptrees:     ================================
char reallocPtree(Ptree *t, int size){
	if(size == 0){
		deleteChildrenAfter(t, -1);
		free(t->nodes);
		t->nodes = 0;
		t->nodec = 0;
		return 3;
	}
	if(size < getSize(t)){
		deleteChildrenAfter(t, size - 1);
	}
	Ptree *temp = realloc(t->nodes, size*sizeof(Ptree));
	if(!temp){
		logMemoryError("reallocPtree");
		debug_calls("returning: 0");
		return 0;
	}
	if(size > t->nodec){
		memset(nthChild(t, -1), 0, (size - t->nodec)*sizeof(Ptree));
	}
	if(temp != t->nodes){
		t->nodes = temp;
		t->nodec = size;
		updateGrandchildPointers(t);
		debug_calls("returning: 2");
		return 2;
	}
	t->nodec = size;
	debug_calls("returning: 1");
	return 1;
}


//Functions to iterate over Ptrees:
Ptree* firstPostorder(Ptree *root){
	while(!isTerminal(root)){
		root = nthChild(root, 0);
	}
	return root;
}

Ptree* nextPostorder(Ptree *current){
	Ptree *parent = current->parent;
	if(!parent){
		return NULL;
	}
	++current;
	if(current - parent->nodes == parent->nodec){
		return parent;
	}
	return firstPostorder(current);
}

