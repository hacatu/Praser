//tree_parser.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "get_line.h"
#include "ptree.h"
#include "parser_llk.h"
#include "debug.h"


/* GRAMMAR:
tree = name "(" (tree(","tree)*)? ")"
name = string | [^"\\(), ]+
string = "\"" ([^"\\]|("\\".))+ "\""
 */


int tree(Position *p, Ptree *t);
int schar(Position *p, Ptree *t);
int nchar(Position *p, Ptree *t);
int comma(Position *p, Ptree *t);


int start(Position *p, Ptree *t){
	expect(p, t, PASS, tree);
	expectEnd(p);
}

int tree(Position *p, Ptree *t){
	debug_gets("%c", currentChar(p));
	if(currentChar(p) == '"'){
		if(!expectString(p, t, SKIP, "\"")){
			logUnexpectedError(p, __func__, "\"");
			return 0;
		}
		repeat(p, t, PASS, schar, 0, 0);
		if(!expectString(p, t, SKIP, "\"")){
			logUnexpectedError(p, __func__, "\"");
			return 0;
		}
	}else{
		repeat(p, t, PASS, nchar, 0, 0);
	}
	if(!expectString(p, t, SKIP, "(")){
		logUnexpectedError(p, __func__, "(");
		return 0;
	}
	if(!sepBy(p, t, ADD, SKIP, tree, comma, 0, 0)){
		return 0;
	}
	if(!expectString(p, t, SKIP, ")")){
		logUnexpectedError(p, __func__, ")");
		return 0;
	}
	return 1;
}

int schar(Position *p, Ptree *t){
	if(acceptString(p, t, SKIP, "\\")){
		noneOf(p, t, PASS, "");
		return 1;
	}
	return noneOf(p, t, PASS, "\\\"");
}

int nchar(Position *p, Ptree *t){
	return noneOf(p, t, PASS, "\"(),");
}

int comma(Position *p, Ptree *t){
	return acceptString(p, t, ADD, ",");
}

int main(){
	Ptree *t;
	Position *p;
	size_t size = 0;
	char *line = 0;
	int read;
	while((read = getLine(&line, &size, stdin)) > 0){
		line[read - 1] = '\0';
		p = firstPosition(line);
		t = mallocPtree();
		if(start(p, t)){
			puts("String parsed successfully!  Output:");
			flattenTagged(t);
			printPtree(t, 0);
		}else{
			puts("String parsed unsuccessfully!");
		}
		deletePtree(t);
		free(t);
	}
	free(line);
	_exit(0);
}

