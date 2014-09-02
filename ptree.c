//ptree.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptree.h"
#include "debug.h"

void logMemoryError(const char *name){
	printf("%s: out of memory.\n", name);
}

Ptree newPtree(Ptree *parent, const char *string){
	return (Ptree){.parent = parent, .string = string};
}

char isTerminal(const Ptree *t){
	return !t->nodec;
}

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

/*
char appendPtree(Ptree *parent, Ptree node){
	Ptree *temp = realloc(parent->nodes, (parent->nodec + 1)*sizeof(Ptree));//expand parent's node list
	if(!temp){//if realloc failed, call logMemoryError to report the error and then return 0.
		logMemoryError("appendPtree");
		return 0;
	}
	if(temp != parent->nodes){
		parent->nodes = temp;//reassign the node list pointer to the new address from realloc.
		++parent->nodec;//increase the node count by one.
		*lastChild(parent) = node;
		int i = 0, j;
		Ptree *grandchild;
		while((temp = nthChild(parent, i))){
			j = 0;
			while((grandchild = nthChild(temp, j))){
				grandchild->parent = temp;
				++j;
			}
			++i;
		}
		return 1;
	}
	++parent->nodec;//increase the node count by one.
	*lastChild(parent) = node;
	return 1;
}
*/

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

/* Local function not in header:
 * Returns 1 if the parent is null, 0 otherwise.
 */
char deleted(const Ptree *t){
	return !t->parent;
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

/* Local function not in header:
 * Free a Ptree's node list and mark it deleted.
 */
void freePtree(Ptree *t){
	free(t->nodes);
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
	printf("%*s%s:\n"
		   //"%*sparent:%p\n"
		   //"%*snodec:%d\n"
		   "%*s{\n",
		   indent, "", t->string,
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

