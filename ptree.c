//ptree.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	Ptree ptree = {.parent = parent};
	setString(&ptree, string, length);
	return ptree;
}

Ptree* mallocPtree(){
	return calloc(1, sizeof(Ptree));
}

char appendNewPtree(Ptree *parent, const char *string, int length){
	switch(reallocPtree(parent, parent->nodec + 1)){
		case 0:
		logMemoryError("appendNewPtree");
		return 0;
		case 1:
		*lastChild(parent) = newPtree(parent, string, length);
		return 1;
		case 2:
		*lastChild(parent) = newPtree(parent, string, length);
		updateGrandchildPointers(parent);
		return 1;
	}
	return 0;
}

char appendPtree(Ptree *parent, Ptree node){
	switch(reallocPtree(parent, parent->nodec + 1)){
		case 0:
		logMemoryError("appendPtree");
		return 0;
		case 1:
		*lastChild(parent) = node;
		return 1;
		case 2:
		*lastChild(parent) = node;
		updateGrandchildPointers(parent);
		return 1;
	}
	return 0;
}


//Fuctions to change a Ptree's string tag:    ========================
void setString(Ptree *p, const char *string, int length){
	free(p->string);
	p->string = malloc(length*sizeof(char));
	if(!p->string){
		logMemoryError(__func__);
		p->length = 0;
		return;
	}
	p->length = length;
	strncpy(p->string, string, length);
}

void appendString(Ptree *p, const char *string, int length){
	const char *temp;
	temp = realloc(p->string, (p->length + length)*sizeof(char));
	if(!temp){
		logMemoryError(__func__);
		return;
	}
	p->length += length;
	strncpy(p->string + p->length, string, length);
}


//getters:    ========================================================
char isTerminal(const Ptree *t){
	return !t->nodec;
}

int size(Ptree *t){
	return t->nodec;
}

/* Local function not in header:
 * Returns 1 if the parent is null, 0 otherwise.
 */
char deleted(const Ptree *t){
	return !t->parent;
}


//Functions to manipulate tree structure (in progress)    ============
//TODO: make iterative
void flatten(Ptree *t){
	Ptree *child, *parent = t->parent;
	while(size(t) == 1){
		child = nthChild(t, 0);
		free(t->string);
		memcpy(t, child, sizeof(Ptree));
		free(child);
	}
	t->parent = parent;
	for(int i = 0; i < size(t); ++i){
		flatten(nthChild(t, i));
	}
}


//Functions to access child nodes:    ================================
Ptree* nthChild(Ptree *t, int n){
	if(n < 0){
		n += t->nodec;
		if(n < 0){
			return 0;
		}
	}else if(n >= t->nodec){//if n is out of bounds, return a null pointer.
		return 0;
	}
	return t->nodes + n;
}

Ptree* lastChild(Ptree *t){
	return nthChild(t, -1);
}

/* Local function not in header:
 * Returns the first non deleted child of Ptree *t.
 * Ptrees only have deleted children when they are being deleted.
 */
Ptree* firstChild(Ptree *t){
	if(isTerminal(t)){
		return 0;
	}
	int i = 0;
	while(nthChild(t, i) && deleted(nthChild(t, i))){
		++i;
	}
	if(!nthChild(t, i)){//if all children were deleted, nthChild(t, i) will return 0 because there are no children.
		return 0;
	}
	return nthChild(t, i);
}


//Functions to reallocate Ptrees:     ================================
char reallocPtree(Ptree *t, int size){
	debug("called");
	Ptree *temp = realloc(t->nodes, size*sizeof(Ptree));
	if(!temp){
		logMemoryError("reallocPtree");
		return 0;
	}
	if(temp != t->nodes){
		t->nodes = temp;
		t->nodec = size;
		debug("moved pointer");
		return 2;
	}
	t->nodec = size;
	debug("all is well");
	return 1;
}

void updateGrandchildPointers(Ptree *t){
	int i = 0, j;
	Ptree *child, *grandchild;
	while((child = nthChild(t, i))){
		j = 0;
		while((grandchild = nthChild(child, j))){
			grandchild->parent = child;
			++j;
		}
		++i;
	}
}

void deleteChildrenAfter(Ptree *t, int n){
	Ptree *current = nthChild(t, n + 1), *last = lastChild(t);
	while(current <= last){
		deletePtree(current);
		++current;
	}
}


//Functions to delete Ptrees.    =====================================
//Note that mallocPtree'd Ptrees still need to be free'd:

/* Local function not in header:
 * Free a Ptree's node list and mark it deleted.
 */
void freePtree(Ptree *t){
	free(t->nodes);
	free(t->string);
	t->length = 0;
	t->nodec = 0;
	t->parent = 0;
}

void deletePtree(Ptree *t){
	const Ptree *top = t;
	debug("called on:");
	ifdebug printPtree(t, 0);
	while(1){
		debug(" deleting t...");
		while(firstChild(t)){//while t has children, set t to its first child
			debug("  t has children, set t = firstChild(t)");
			t = firstChild(t);
			debug("  t is now:");
			ifdebug printPtree(t, 0);
		}
		//t must be the root node and must have no children.
		//The above while loop ensures t has no children,
		//and if t's parent is null, t is the root.
		debug(" t has been reduced to a leaf node");
		//if(!t->parent){
		if(t == top){
			debug("  t is the root node; freePtree'ing t");
			freePtree(t);
			return;
		}
		//t must have a parent because if it did not
		//the above if statement would have exited.
		//Set t equal to is parent and freePtree its firstChild.
		debug(" t has a parent; setting t = t->parent");
		t = t->parent;
		debug(" deleting first child of:");
		ifdebug printPtree(t, 0);
		freePtree(firstChild(t));
	}
}


//Functions to print Ptrees:    ======================================
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

