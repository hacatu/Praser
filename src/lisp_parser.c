//lisp_parser.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "get_line.h"
#include "ptree.h"
#include "parser_llk.h"
#include "debug.h"


/* GRAMMAR:
start: sxpr
sxpr: atom | "(" sxpr* ")"
atom: 
 */

typedef enum{NAME, LIST, NUMBER, STRING, BOOL} LispType;

typedef struct LispVal{
	LispType type;
	union{
		const char *string;
		struct LispVal *list;
		uint64_t number;
		int boolean;
	};
} LispVal;


int start(Position *p, Ptree *t);
int sxpr(Position *p, Ptree *t);
int atom(Position *p, Ptree *t);
int alist(Position *p, Ptree *t);
int aname(Position *p, Ptree *t);
int abool(Position *p, Ptree *t);
int name(Position *p, Ptree *t);
int nchar(Position *p, Ptree *t);

int start(Position *p, Ptree *t){
	if(!expect(p, t, PASS, sxpr)){
		logUnexpectedError(p, __func__, "s-expression");
		return 0;
	}
	if(!expectEnd(p)){
		logUnexpectedError(p, __func__, "end of input");
		return 0;
	}
	return 1;
}

int sxpr(Position *p, Ptree *t){
	setString(t, __func__, strlen(__func__));
	if(!acceptString(p, t, SKIP, "(")){
		return 0;
	}
	if(!sepBy(p, t, ADD, SKIP, atom, spaces, 0, 0)){
		logUnexpectedError(p, __func__, "atom");
		return 0;
	}
	if(!expectString(p, t, SKIP, ")")){
		logUnexpectedError(p, __func__, ")");
		return 0;
	}
	return 1;
}

int atom(Position *p, Ptree *t){
	if(accept(p, t, PASS, sxpr)){
		return 1;
	}
	if(accept(p, t, ADD, cstring)){
		setString(t, "string", 6);
		return 1;
	}
	if(accept(p, t, ADD, abool)){
		setString(t, "bool", 4);
		return 1;
	}
	if(accept(p, t, PASS, alist)){
		return 1;
	}
	if(accept(p, t, ADD, integer)){
		setString(t, "number", 6);
		return 1;
	}
	if(accept(p, t, ADD, aname)){
		setString(t, "name", 4);
		return 1;
	}
	return 0;
}

int abool(Position *p, Ptree *t){
	if(!acceptString(p, t, SKIP, "#")){
		return 0;
	}
	if(acceptString(p, t, SKIP, "t")){
		setString(t, "true", 4);
		return 1;
	}
	if(acceptString(p, t, SKIP, "f")){
		setString(t, "false", 5);
		return 1;
	}
	logUnexpectedError(p, __func__, "t or f");
	return 0;
}

int aname(Position *p, Ptree *t){
	return accept(p, t, ADD, name);
}

int name(Position *p, Ptree *t){
	if(!noneOf(p, t, PASS, ")")){
		return 0;
	}
	repeat(p, t, PASS, nchar, 0, 0);
	return 1;
}

int nchar(Position *p, Ptree *t){
	return noneOf(p, t, PASS, ") \n\t\v\f\r");
}

int alist(Position *p, Ptree *t){
	if(acceptString(p, t, SKIP, "'")){
		setString(t, "quote", 5);
	}else if(acceptString(p, t, SKIP, "`")){
		setString(t, "quasiquote", 5);
	}else if(acceptString(p, t, SKIP, ",")){
		if(acceptString(p, t, SKIP, "@")){
			setString(t, "unquote-splicing", 16);
		}else{
			setString(t, "unquote", 7);
		}
	}else{
		return 0;
	}
	if(!expect(p, t, ADD, sxpr)){
		logUnexpectedError(p, __func__, "s-expr");
		return 0;
	}
	return 1;
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
		free(p);
	}
	free(line);
	return 1;
}

