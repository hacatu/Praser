//ptree_delete.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "debug.h"


char isDeleted(const Ptree *t){
	debug("called on: %p", t);
	debug("returning: %i", t->parent);
	return !t->parent;
}


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


//Functions to delete Ptrees.    =====================================
//Note that mallocPtree'd Ptrees still need to be free'd:

/* Free a Ptree's node list and mark it deleted.
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

