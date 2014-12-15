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


int start(PRA_Position *p, PRA_Ptree *t);
int sxpr(PRA_Position *p, PRA_Ptree *t);
int atom(PRA_Position *p, PRA_Ptree *t);
int alist(PRA_Position *p, PRA_Ptree *t);
int aname(PRA_Position *p, PRA_Ptree *t);
int abool(PRA_Position *p, PRA_Ptree *t);
int name(PRA_Position *p, PRA_Ptree *t);
int nchar(PRA_Position *p, PRA_Ptree *t);

int start(PRA_Position *p, PRA_Ptree *t){
	if(!PRA_accept(p, t, PRA_PASS, sxpr)){
		PRA_logUnexpectedError(p, __FUNCTION__, "s-expression");
		return 0;
	}
	if(!PRA_acceptEnd(p)){
		PRA_logUnexpectedError(p, __FUNCTION__, "end of input");
		return 0;
	}
	return 1;
}

int sxpr(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, "(sxpr)", 6);
	if(!PRA_acceptString(p, t, PRA_SKIP, "(")){
		return 0;
	}
	if(!PRA_sepBy(p, t, PRA_ADD, PRA_SKIP, atom, PRA_spaces, 0, 0)){
		puts("did not find atoms seperated by spaces");
		return 0;
	}
	if(!PRA_acceptString(p, t, PRA_SKIP, ")")){
		PRA_logUnexpectedError(p, __FUNCTION__, ")");
		return 0;
	}
	return 1;
}

int atom(PRA_Position *p, PRA_Ptree *t){
	if(PRA_accept(p, t, PRA_PASS, sxpr)){
		puts("found sxpr");
		return 1;
	}
	if(PRA_accept(p, t, PRA_ADD, PRA_cstring)){
		puts("found string");
		PRA_setString(t, "(string)", 8);
		return 1;
	}
	if(PRA_accept(p, t, PRA_ADD, abool)){
		puts("found bool");
		PRA_setString(t, "(bool)", 6);
		return 1;
	}
	if(PRA_accept(p, t, PRA_PASS, alist)){
		puts("found list");
		return 1;
	}
	if(PRA_accept(p, t, PRA_ADD, PRA_integer)){//never freed (ibid.) x 2
		puts("found number");
		PRA_setString(t, "(number)", 8);//never freed x 2
		return 1;
	}
	if(PRA_accept(p, t, PRA_ADD, aname)){//never freed (appendNewPRA_Ptree (PRA_Ptree_create.c:38), reallocPRA_Ptree (PRA_Ptree_util.c:157))
		PRA_setString(t, "(name)", 6);//never freed
		return 1;
	}
	return 0;
}

int abool(PRA_Position *p, PRA_Ptree *t){
	if(!PRA_acceptString(p, t, PRA_SKIP, "#")){
		return 0;
	}
	if(PRA_acceptString(p, t, PRA_SKIP, "t")){
		PRA_setString(t, "t", 1);
		return 1;
	}
	if(PRA_acceptString(p, t, PRA_SKIP, "f")){
		PRA_setString(t, "f", 1);
		return 1;
	}
	PRA_logUnexpectedError(p, __FUNCTION__, "t or f");
	return 0;
}

int aname(PRA_Position *p, PRA_Ptree *t){
	return PRA_accept(p, t, PRA_PASS, name);
}

int name(PRA_Position *p, PRA_Ptree *t){
	if(!PRA_noneOf(p, t, PRA_PASS, ")")){
		return 0;
	}
	PRA_repeat(p, t, PRA_PASS, nchar, 0, 0);
	return 1;
}

int nchar(PRA_Position *p, PRA_Ptree *t){
	return PRA_noneOf(p, t, PRA_PASS, ") \n\t\v\f\r");
}

int alist(PRA_Position *p, PRA_Ptree *t){
	if(PRA_acceptString(p, t, PRA_SKIP, "'")){
		PRA_setString(t, "(quote)", 7);
	}else if(PRA_acceptString(p, t, PRA_SKIP, "`")){
		PRA_setString(t, "(quasiquote)", 12);
	}else if(PRA_acceptString(p, t, PRA_SKIP, ",")){
		if(PRA_acceptString(p, t, PRA_SKIP, "@")){
			PRA_setString(t, "(unquote-splicing)", 18);
		}else{
			PRA_setString(t, "(unquote)", 9);
		}
	}else{
		return 0;
	}
	if(!PRA_accept(p, t, PRA_ADD, sxpr)){
		PRA_logUnexpectedError(p, __FUNCTION__, "s-expr");
		return 0;
	}
	return 1;
}


int main(){
	initBaseEnv();
	PRA_Ptree *t;
	LispVal v, r;
	size_t size = 0;
	char *line = 0;
	int read;
	Env *e = copyEnv(baseEnv);
	while((read = getLine(&line, &size, stdin)) > 0){
		line[read - 1] = '\0';
		t = PRA_parseString(line, start);
		if(t){
			//PRA_printPtree(t, 0);
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
		PRA_deletePtree(t);
		free(t);
	}
	deleteEnvData(e);
	deleteEnvData(baseEnv);
	free(line);
}

