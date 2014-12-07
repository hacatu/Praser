//tree_parser.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <get_line.h>
#include <ptree.h>
#include <parser.h>
#include <debug.h>


/* GRAMMAR:
tree = name "(" (tree(","tree)*)? ")"
name = string | [^"\\(), ]+
string = "\"" ([^"\\]|("\\".))+ "\""
 */


int tree(PRA_Position *p, PRA_Ptree *t);
int schar(PRA_Position *p, PRA_Ptree *t);
int nchar(PRA_Position *p, PRA_Ptree *t);
int comma(PRA_Position *p, PRA_Ptree *t);


int start(PRA_Position *p, PRA_Ptree *t){
	if(!PRA_accept(p, t, PRA_PASS, tree)){
		return 0;
	}
	if(!PRA_acceptEnd(p)){
		return 0;
	}
	return 1;
}

int tree(PRA_Position *p, PRA_Ptree *t){
	debug_gets("%c", PRA_currentChar(p));
	if(PRA_currentChar(p) == '"'){
		if(!PRA_acceptString(p, t, PRA_SKIP, "\"")){
			PRA_logUnexpectedError(p, __FUNCTION__, "\"");
			return 0;
		}
		PRA_repeat(p, t, PRA_PASS, schar, 0, 0);
		if(!PRA_acceptString(p, t, PRA_SKIP, "\"")){
			PRA_logUnexpectedError(p, __FUNCTION__, "\"");
			return 0;
		}
	}else{
		PRA_repeat(p, t, PRA_PASS, nchar, 0, 0);
	}
	if(!PRA_acceptString(p, t, PRA_SKIP, "(")){
		PRA_logUnexpectedError(p, __FUNCTION__, "(");
		return 0;
	}
	if(!PRA_sepBy(p, t, PRA_ADD, PRA_SKIP, tree, comma, 0, 0)){
		return 0;
	}
	if(!PRA_acceptString(p, t, PRA_SKIP, ")")){
		PRA_logUnexpectedError(p, __FUNCTION__, ")");
		return 0;
	}
	return 1;
}

int schar(PRA_Position *p, PRA_Ptree *t){
	if(PRA_acceptString(p, t, PRA_SKIP, "\\")){
		PRA_noneOf(p, t, PRA_PASS, "");
		return 1;
	}
	return PRA_noneOf(p, t, PRA_PASS, "\\\"");
}

int nchar(PRA_Position *p, PRA_Ptree *t){
	return PRA_noneOf(p, t, PRA_PASS, "\"(),");
}

int comma(PRA_Position *p, PRA_Ptree *t){
	return PRA_acceptString(p, t, PRA_ADD, ",");
}

int main(){
	PRA_Ptree *t;
	PRA_Position *p;
	size_t size = 0;
	char *line = 0;
	int read;
	while((read = getLine(&line, &size, stdin)) > 0){
		line[read - 1] = '\0';
		p = PRA_firstPosition(line);
		t = PRA_mallocPtree();
		if(start(p, t)){
			puts("String parsed successfully!  Output:");
			PRA_flattenTagged(t);
			PRA_printPtree(t, 0);
		}else{
			puts("String parsed unsuccessfully!");
		}
		PRA_deletePtree(t);
		free(t);
		free(p);
	}
	free(line);
}

