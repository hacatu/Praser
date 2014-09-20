//ptree_print.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "debug.h"


//Functions to print Ptrees:    ======================================

void printString(const Ptree *t){
	printf("%s", t->string);
}

//*
void printPtree(const Ptree *t, int indent){
	if(!t){
		puts("t is null! WHY!?");
		return;
	}
	if(isTerminal(t)){
		printf("%*s\"", indent, "");
		printString(t);
		printf("\"\n");
		return;
		return;
	}
	printf("%*s", indent, "");
	printString(t);
	printf(//"%*sparent:%p\n"
		   //"%*snodec:%d\n"
		   ":\n%*s{\n",
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

