//ptree_delete.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "../util/debug.h"

/*
static char isDeleted(const PRA_Ptree *t){
	debug_calls("called on: %p", t);
	debug_calls("returning: %i", t->PRA_parent);
	return !t->PRA_parent;
}
*/

/*
static PRA_Ptree* firstChild(PRA_Ptree *t){
	debug_calls("called on: %p", t);
	if(!t){
		debug_calls("returning: %p", 0);
		return 0;
	}
	if(PRA_isTerminal(t)){
		debug_calls("returning: %p", 0);
		return 0;
	}
	int i = 0;
	while(PRA_nthChild(t, i) && isDeleted(PRA_nthChild(t, i))){
		++i;
	}
	if(!PRA_nthChild(t, i)){//if all children were deleted, PRA_nthChild(t, i) will return 0 because there are no children.
		debug_calls("returning: %p", 0);
		return 0;
	}
	if(dbg){
		PRA_Ptree *temp = PRA_nthChild(t, i);
		debug_calls("returning: %p", temp);
		return temp;
	}
	return PRA_nthChild(t, i);
}
*/

//Functions to delete PRA_Ptrees.    =====================================
//note that PRA_mallocPtree'd PRA_Ptrees still need to be free'd:

/* Free a PRA_Ptree's node list and mark it deleted.
 */
static void freePRA_Ptree(PRA_Ptree *t){
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
void PRA_deletePtree(PRA_Ptree *t){
	const PRA_Ptree *top = t;
	//puts("PRA_deletePtree called");
	debug_calls("called on: %p", t);
	//PRA_printPtree(t, 0);
	while(1){
		//puts("looping");
		while(firstChild(t)){//while t has children, set t to its first child
			t = firstChild(t);
		}
		//t must be the root node and must have no children.
		//The above while loop ensures t has no children,
		//and if t's PRA_parent is null, t is the root.
		//if(!t->PRA_parent){
		if(t == top){
			//puts("deleting last node");
			freePRA_Ptree(t);
			//PRA_printPtree(t, 0);
			debug_calls("returning");
			return;
		}
		//t must have a PRA_parent because if it did PRA_not
		//the above if statement would have exited.
		//Set t equal to is PRA_parent and freePRA_Ptree its firstChild.
		t = t->PRA_parent;
		freePRA_Ptree(firstChild(t));
		//PRA_printPtree(t, 0);
	}
}
*/

//TODO: make iterative
void PRA_deletePtree(PRA_Ptree *t){
	for(int i = 0; i < PRA_getSize(t); ++i){
		PRA_deletePtree(PRA_nthChild(t, i));
	}
	freePRA_Ptree(t);
}

