//lisp_parser.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <get_line.h>
#include <ptree.h>
#include <parser.h>
#include "lisp_interpreter.h"
#include "lisp_value.h"
#include "lisp_env.h"
#include "base_env.h"
#include <debug.h>


/* GRAMMAR:
start: sxpr
sxpr: atom | "(" sxpr* ")"
atom: 
 */


int start(Position *p, Ptree *t);
int sxpr(Position *p, Ptree *t);
int atom(Position *p, Ptree *t);
int alist(Position *p, Ptree *t);
int aname(Position *p, Ptree *t);
int abool(Position *p, Ptree *t);
int name(Position *p, Ptree *t);
int nchar(Position *p, Ptree *t);

int start(Position *p, Ptree *t){
	if(!accept(p, t, PASS, sxpr)){
		logUnexpectedError(p, __FUNCTION__, "s-expression");
		return 0;
	}
	if(!acceptEnd(p)){
		logUnexpectedError(p, __FUNCTION__, "end of input");
		return 0;
	}
	return 1;
}

int sxpr(Position *p, Ptree *t){
	setString(t, "(sxpr)", 6);
	if(!acceptString(p, t, SKIP, "(")){
		return 0;
	}
	if(!sepBy(p, t, ADD, SKIP, atom, spaces, 0, 0)){
		puts("did not find atoms seperated by spaces");
		return 0;
	}
	if(!acceptString(p, t, SKIP, ")")){
		logUnexpectedError(p, __FUNCTION__, ")");
		return 0;
	}
	return 1;
}

int atom(Position *p, Ptree *t){
	if(accept(p, t, PASS, sxpr)){
		puts("found sxpr");
		return 1;
	}
	if(accept(p, t, ADD, cstring)){
		puts("found string");
		setString(t, "(string)", 8);
		return 1;
	}
	if(accept(p, t, ADD, abool)){
		puts("found bool");
		setString(t, "(bool)", 6);
		return 1;
	}
	if(accept(p, t, PASS, alist)){
		puts("found list");
		return 1;
	}
	if(accept(p, t, ADD, integer)){//never freed (ibid.) x 2
		puts("found number");
		setString(t, "(number)", 8);//never freed x 2
		return 1;
	}
	if(accept(p, t, ADD, aname)){//never freed (appendNewPtree (ptree_create.c:38), reallocPtree (ptree_util.c:157))
		setString(t, "(name)", 6);//never freed
		return 1;
	}
	return 0;
}

int abool(Position *p, Ptree *t){
	if(!acceptString(p, t, SKIP, "#")){
		return 0;
	}
	if(acceptString(p, t, SKIP, "t")){
		setString(t, "t", 1);
		return 1;
	}
	if(acceptString(p, t, SKIP, "f")){
		setString(t, "f", 1);
		return 1;
	}
	logUnexpectedError(p, __FUNCTION__, "t or f");
	return 0;
}

int aname(Position *p, Ptree *t){
	return accept(p, t, PASS, name);
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
		setString(t, "(quote)", 7);
	}else if(acceptString(p, t, SKIP, "`")){
		setString(t, "(quasiquote)", 12);
	}else if(acceptString(p, t, SKIP, ",")){
		if(acceptString(p, t, SKIP, "@")){
			setString(t, "(unquote-splicing)", 18);
		}else{
			setString(t, "(unquote)", 9);
		}
	}else{
		return 0;
	}
	if(!accept(p, t, ADD, sxpr)){
		logUnexpectedError(p, __FUNCTION__, "s-expr");
		return 0;
	}
	return 1;
}


int main(){
	initBaseEnv();
	Ptree *t;
	Position *p;
	LispVal v, r;
	size_t size = 0;
	char *line = 0;
	int read;
	Env *e = copyEnv(baseEnv);
	while((read = getLine(&line, &size, stdin)) > 0){
		line[read - 1] = '\0';
		p = firstPosition(line);
		t = mallocPtree();
		if(start(p, t)){
			//printPtree(t, 0);
			v = expr(t);
			puts("S-expression:");
			printLispVal(v);
			puts("");
			r = eval(v, e);
			deleteLispVal(v);
			puts("Evaluated:");
			printLispVal(r);
			puts("");
			deleteLispVal(r);
		}else{
			puts("String parsed unsuccessfully!");
		}
		deletePtree(t);
		free(t);
		free(p);
	}
	deleteEnvData(e);
	deleteEnvData(baseEnv);
	free(line);
}

