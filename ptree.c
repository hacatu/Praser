//ptree.c
#include <stdio.h>
#include <stdlib.h>
#include "ptree.h"
#include "ptree_private.h"
#include "debug.h"

//Types:    ==========================================================
struct Ptree{//an n-ary string tagged tree to store parse data for postprocessing
	struct Ptree *parent;//the parent node.
	int nodec;//the number of nodes.
	struct Ptree *nodes;//the list of nodes
	int length;//the length of the string (only needed when it is a non null terminated substring of the whole string being parsed)
	char *string;//the string tag
};


//Error functions, do not invlove Ptrees:    =========================
void logMemoryError(const char *name){
	printf("%s: out of memory.\n", name);
}


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

/* Local function not in header:
 * Returns 1 if the parent is null, 0 otherwise.
 */
char isDeleted(const Ptree *t){
	debug("called on: %p", t);
	debug("returning: %i", t->parent);
	return !t->parent;
}


//Declaration:
void updateChildPointers(Ptree *t);


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

/* Local function not in header:
 * Returns the first non deleted child of Ptree *t.
 * Ptrees only have deleted children when they are being deleted.
 */
Ptree* firstChild(Ptree *t){
	debug("called on: %p", t);
	if(!t){
		debug("returning: %p", 0);
		return 0;
	}
	if(isTerminal(t)){
		debug("returning: %p", 0);
		return 0;
	}
	int i = 0;
	while(nthChild(t, i) && isDeleted(nthChild(t, i))){
		++i;
	}
	if(!nthChild(t, i)){//if all children were deleted, nthChild(t, i) will return 0 because there are no children.
		debug("returning: %p", 0);
		return 0;
	}
	if(dbg){
		Ptree *temp = nthChild(t, i);
		debug("returning: %p", temp);
		return temp;
	}
	return nthChild(t, i);
}


/* Local function not in header:
 * updates child pointers to point to their new parent if moved
 */
void updateChildPointers(Ptree *t){
	debug("called on: %p", t);
	Ptree *child;
	for(int i = 0; (child = nthChild(t, i)); ++i){
		child->parent = t;
	}
	debug("returning");
}

/* Local function not in header:
 * updates grandchild pointers to point to their new parent if it is moved by realloc
 */
void updateGrandchildPointers(Ptree *t){
	debug("called on: %p", t);
	Ptree *child;
	for(int i = 0; (child = nthChild(t, i)); ++i){
		updateChildPointers(child);
	}
	debug("returning");
}

/* Local function not in header:
 * delete children after an index
 */
void deleteChildrenAfter(Ptree *t, int n){
	debug("called on: %p, %d", t, n);
	Ptree *current = nthChild(t, n + 1), *last = lastChild(t);
	while(current <= last){
		deletePtree(current);
		++current;
	}
	debug("returning");
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


//Functions to delete Ptrees.    =====================================
//Note that mallocPtree'd Ptrees still need to be free'd:

/* Local function not in header:
 * Free a Ptree's node list and mark it deleted.
 */
void freePtree(Ptree *t){
	debug("called on: %p", t);
	if(!t){
		debug("returning");
		return;
	}
	free(t->nodes);
	free(t->string);
	t->length = 0;
	t->nodec = 0;
	t->parent = 0;
	debug("returning");
}

void deletePtree(Ptree *t){
	const Ptree *top = t;
	debug("called on: %p", t);
	while(1){
		while(firstChild(t)){//while t has children, set t to its first child
			t = firstChild(t);
		}
		//t must be the root node and must have no children.
		//The above while loop ensures t has no children,
		//and if t's parent is null, t is the root.
		//if(!t->parent){
		if(t == top){
			freePtree(t);
			debug("returning");
			return;
		}
		//t must have a parent because if it did not
		//the above if statement would have exited.
		//Set t equal to is parent and freePtree its firstChild.
		t = t->parent;
		freePtree(firstChild(t));
	}
	debug("returning");
}


//Functions to print Ptrees:    ======================================

void printString(const Ptree *t){
	fwrite(t->string, sizeof(char), t->length, stdout);
}

//*
void printPtree(const Ptree *t, int indent){
	if(!t){
		puts("t is null! WHY!?");
		return;
	}
	if(isTerminal(t)){
		if(t->length){
			printf("%*s\"%.*s\"\n", indent, "", t->length, t->string);
			return;
		}
		printf("%*s\"%s\"\n", indent, "", t->string);
		return;
	}
	printf("%*s%*s:\n"
		   //"%*sparent:%p\n"
		   //"%*snodec:%d\n"
		   "%*s{\n",
		   indent, "", t->length, t->string,
		   //indent, "", t->parent,
		   //indent, "", t->nodec,
		   indent, "");
	int i = 0;
	Ptree *sub;
	while((sub = nthChild((Ptree*)t, i))){
		printPtree(sub, indent + 1);
		++i;
	}
	printf("%*s}\n", indent, "");
}
//*/

//debug version that prints all data
/*
void printPtree(const Ptree *t, int indent){
	if(!t){
		puts("t is null! WHY!?");
		return;
	}
	printf("%*saddr:   %p\n%*sstring: ", indent, "", t, indent, "");
	printString(t);
	printf("\n"
		   "%*slength: %d\n"
		   "%*sparent: %p\n"
		   "%*snodec:  %d\n"
		   "%*snodes:  %p\n"
		   "%*s{\n",
		   indent, "", t->length,
		   indent, "", t->parent,
		   indent, "", t->nodec,
		   indent, "", t->nodes,
		   indent, "");
	int i = 0;
	Ptree *sub;
	while((sub = nthChild((Ptree*)t, i))){
		printPtree(sub, indent + 1);
		++i;
	}
	printf("%*s}\n", indent, "");
}
*/

