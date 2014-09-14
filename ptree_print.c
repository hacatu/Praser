//ptree_print.c
#include <stdio.h>
#include <stdlib.h>
#include "__ptree.h"
#include "debug.h"


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

