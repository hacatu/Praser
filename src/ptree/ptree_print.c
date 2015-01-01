//ptree_print.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "../util/debug.h"


//Functions to print PRA_Ptrees:    ======================================

static void printString(const PRA_Ptree *t){
	printf("%s", t->string);
}

//*
void PRA_printPtree(const PRA_Ptree *t, int indent){
	if(!t){
		puts("t is null! WHY!?");
		return;
	}
	if(PRA_isTerminal(t)){
		printf("%*s\"", indent, "");
		printString(t);
		printf("\"\n");
		return;
	}
	printf("%*s", indent, "");
	printString(t);
	printf(//"%*sparent:%p\n"
		   //"%*snodec:%d\n"
		   ":\n%*s{\n",
		   //indent, "", t->PRA_parent,
		   //indent, "", t->nodec,
		   indent, "");
	int i = 0;
	PRA_Ptree *sub;
	while((sub = PRA_nthChild((PRA_Ptree*)t, i))){
		PRA_printPtree(sub, indent + 1);
		++i;
	}
	printf("%*s}\n", indent, "");
}
//*/

//debug version that prints all data
/*
void PRA_printPtree(const PRA_Ptree *t, int indent){
	if(!t){
		puts("t is null! WHY!?");
		return;
	}
	printf("%*sPRA_ADDr:   %p\n%*sstring: ", indent, "", t, indent, "");
	printString(t);
	printf("\n"
		   "%*slength: %d\n"
		   "%*sPRA_parent: %p\n"
		   "%*snodec:  %d\n"
		   "%*snodes:  %p\n"
		   "%*s{\n",
		   indent, "", t->length,
		   indent, "", t->PRA_parent,
		   indent, "", t->nodec,
		   indent, "", t->nodes,
		   indent, "");
	int i = 0;
	PRA_Ptree *sub;
	while((sub = PRA_nthChild((PRA_Ptree*)t, i))){
		PRA_printPtree(sub, indent + 1);
		++i;
	}
	printf("%*s}\n", indent, "");
}
*/

