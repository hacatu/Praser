//ptree_delete.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "../util/debug.h"

/*
static char isDeleted(const Ptree *t){
	debug_calls("called on: %p", t);
	debug_calls("returning: %i", t->parent);
	return !t->parent;
}
*/

/*
static Ptree* firstChild(Ptree *t){
	debug_calls("called on: %p", t);
	if(!t){
		debug_calls("returning: %p", 0);
		return 0;
	}
	if(isTerminal(t)){
		debug_calls("returning: %p", 0);
		return 0;
	}
	int i = 0;
	while(nthChild(t, i) && isDeleted(nthChild(t, i))){
		++i;
	}
	if(!nthChild(t, i)){//if all children were deleted, nthChild(t, i) will return 0 because there are no children.
		debug_calls("returning: %p", 0);
		return 0;
	}
	if(dbg){
		Ptree *temp = nthChild(t, i);
		debug_calls("returning: %p", temp);
		return temp;
	}
	return nthChild(t, i);
}
*/

//Functions to delete Ptrees.    =====================================
//Note that mallocPtree'd Ptrees still need to be free'd:

/* Free a Ptree's node list and mark it deleted.
 */
static void freePtree(Ptree *t){
	debug_calls("called on: %p", t);
	if(!t){
		debug_calls("returning");
		return;
	}
	free(t->nodes);
	free(t->string);
	t->length = 0;
	t->nodec = 0;
	t->parent = NULL;
	t->nodes = NULL;
	t->string = NULL;
	debug_calls("returning");
}

/*
void deletePtree(Ptree *t){
	const Ptree *top = t;
	//puts("deletePtree called");
	debug_calls("called on: %p", t);
	//printPtree(t, 0);
	while(1){
		//puts("looping");
		while(firstChild(t)){//while t has children, set t to its first child
			t = firstChild(t);
		}
		//t must be the root node and must have no children.
		//The above while loop ensures t has no children,
		//and if t's parent is null, t is the root.
		//if(!t->parent){
		if(t == top){
			//puts("deleting last node");
			freePtree(t);
			//printPtree(t, 0);
			debug_calls("returning");
			return;
		}
		//t must have a parent because if it did not
		//the above if statement would have exited.
		//Set t equal to is parent and freePtree its firstChild.
		t = t->parent;
		freePtree(firstChild(t));
		//printPtree(t, 0);
	}
}
*/

//TODO: make iterative
void deletePtree(Ptree *t){
	for(int i = 0; i < getSize(t); ++i){
		deletePtree(nthChild(t, i));
	}
	freePtree(t);
}

