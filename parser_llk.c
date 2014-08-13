//parser_llk.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser_llk.h"
#include "debug.log"
/* TODO:
 * Make the standard parser combinators not leave useless Ptree*'s at
 * the end of every nodes.
 */


struct Position{
	const char* string;
	const char* current;
};


void logUnexpectedError(Position *p, const char *name, const char *expected){
	//TODO: count lines and include the line/column number for errors, and print the line with the error highlighted.
	const char found[] = {currentChar(p), '\0'};
	printf("%s: expected %s but found %s\n", name, expected, found);
}

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
	debug("called on:");
	printPtree(t, 0);
	while(1){
		debug(" deleting t...");
		while(firstChild(t)){//while t has children, set t to its first child
			debug("  t has children, set t = firstChild(t)");
			t = firstChild(t);
			debug("  t is now:");
			printPtree(t, 0);
		}
		//t must be the root node and must have no children.
		//The above while loop ensures t has no children,
		//and if t's parent is null, t is the root.
		debug(" t has been reduced to a leaf node");
		if(!t->parent){
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
		printPtree(t, 0);
		freePtree(firstChild(t));
	}
}

void printPtree(const Ptree *t, int indent){
	if(!t){
		puts("t is null! WHY!?");
		return;
	}
	if(isTerminal(t)){
		printf("%*s\"%.*s\"\n%*s%p\n", indent, "", t->length, t->string, indent, "", t->parent);
		return;
	}
	printf("%*s%s:\n"
		   "%*sparent:%p\n"
		   //"%*snodec:%d\n"
		   "%*snodes:{\n",
		   indent, "", t->string,
		   indent, "", t->parent,
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


Position* firstPosition(const char *string){
	Position *start = malloc(1*sizeof(Position));
	*start = (Position){string, string};
	return start;
}


char currentChar(Position *p){
	return *p->current;
}

char getChar(Position *p){
	return *(++p->current);
}

char nextChar(Position *p){
	return p->current[1];
}

char nthChar(Position *p, int n){
	return p->current[n];
}

/* Local function not in header:
 * Attempts to parse the char c.
 * If successfull, get the next char and return 1.
 * Otherwise return 0.
 */
int acceptChar(Position *p, char c){
	if(currentChar(p) == c){
		getChar(p);
		return 1;
	}
	return 0;
}

/* Local function not in header:
 * If the char c is not acceptChar'd, call logUnexpectedError and return 0.
 * Otherwise return 1.
 */
int expectChar(Position *p, char c){
	if(acceptChar(p, c)){
		return 1;
	}
	const char expected[] = {c, '\0'};
	logUnexpectedError(p, "expectChar", expected);
	return 0;
}


int acceptString(Position *p, Ptree *t, const char *s){
	int i = 0;
	while(s[i]){
		if(!acceptChar(p, s[i])){
			return 0;
		}
			++i;
	}
	appendPtree(t, newPtree(t, s));
	return 1;
}

int expectString(Position *p, Ptree *t, const char *s){
	if(acceptString(p, t, s)){
		return 1;
	}
	logUnexpectedError(p, "expectString",  s);
	return 0;
}


char expectEnd(Position *p){
	return expectChar(p, '\0');
}


int try(Position *p, Ptree *t, parser parse){
	Position current = *p;
	if(accept(&current, t, parse)){
		*p = current;
		return 1;
	}
	return 0;
}


int accept(Position *p, Ptree *t, parser parse){
	Ptree temp = newPtree(t, NULL);
	if(!parse(p, &temp)){
		return 0;
	}
	if(!appendPtree(t, temp)){
		return 0;
	}
	return 1;
}

int expect(Position *p, Ptree *t, parser parse){
	if(accept(p, t, parse)){
		return 1;
	}
	char expected[9];
	snprintf(expected, 9, "%p", parse);
	logUnexpectedError(p, "expect", expected);
	return 0;
}


int repeatMinMax(Position *p, Ptree *t, parser parse, int min, int max){
	//t->string = __func__;
	int count = 0;
	while(count < min){
		if(!expect(p, t, parse)){
			return 0;
		}
		++count;
	}
	if(max > 0){
		while(count < max){
			if(!accept(p, t, parse)){
				break;
			}
			++count;
		}
	}else{
		while(1){
			if(!accept(p, t, parse)){
				break;
			}
			++count;
		}
	}
	return 1;
}

int sepBy(Position *p, Ptree *t, parser parse, parser parseSeperator){
	//t->string = __func__;
	if(!accept(p, t, parse)){
		return 0;
	}
	while(accept(p, t, parseSeperator)){
		if(!expect(p, t, parse)){
			return 0;
		}
	}
	return 1;
}

int alternate(Position *p, Ptree *t, parser parseA, parser parseB){
	//t->string = __func__;
	while(accept(p, t, parseA)){
		if(!expect(p, t, parseB)){
			return 0;
		}
	}
	return 1;
}

int takeWhileNot(Position *p, Ptree *t, parser parse){
	//t->string = __func__;
	Position start = *p;
	int c = 0;
	Ptree temp = newPtree(t, NULL);
	while(!parse(p, &temp)){
		if(!getChar(p)){
			return 0;
		}
		++c;
	}
	if(!appendPtree(t, (Ptree){.parent = t, .length = c, .string = start.current})){
		logMemoryError(__func__);
		return 0;
	}
	if(!appendPtree(t, temp)){
		logMemoryError(__func__);
		return 0;
	}
	return 1;
}

int oneOf(Position *p, Ptree *t, const char *options){
	t->length = 1;
	const char *c = options;
	while(*c){
		if(acceptChar(p, *c)){
			t->string = c;
			return 1;
		}
		++c;
	}
	return 0;
}

