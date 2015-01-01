//ptree_delete.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "../util/debug.h"

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

//TODO: make iterative
void PRA_deletePtree(PRA_Ptree *t){
	for(int i = 0; i < PRA_getSize(t); ++i){
		PRA_deletePtree(PRA_nthChild(t, i));
	}
	freePRA_Ptree(t);
}

