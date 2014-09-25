//lisp_interpreter.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "lisp_interpreter.h"
#include "ptree.h"
#include "debug.h"

struct Env{
	struct Env *parent;
	struct Env *left;
	struct Env *right;
	const char *name;
	LispVal value;
};

void deleteLispVal(LispVal *v){
	if(v->type != LIST){
		return;
	}
	if(v->car){
		deleteLispVal(v->car);
		free(v->car);
		v->car = NULL;
	}
	if(v->cdr){
		deleteLispVal(v->cdr);
		free(v->cdr);
		v->cdr = NULL;
	}
}

LispVal exprNAME(Ptree *t){
	LispVal temp = {.type = NAME, .name = malloc(getLength(t)*sizeof(char))};
	if(temp.name){
		strcpy(temp.name, getString(t));
	}
	return temp;
}

LispVal exprNUMBER(Ptree *t){
	return (LispVal){.type = NUMBER, .number = atoll(getString(t))};
}

LispVal exprBOOL(Ptree *t){
	return (LispVal){.type = BOOL, .code = getString(t)[0]=='t'?1:0};
}

LispVal exprSTRING(Ptree *t){
	LispVal s = {.type = LIST};
	LispVal *cdr = &s;
	const char *string = getString(t);
	for(int i = 0;;){
		cdr->car = malloc(1*sizeof(LispVal));
		if(!cdr->car){
			deleteLispVal(&s);
			return s;
		}
		*(cdr->car) = (LispVal){.type = CHAR, .code = string[i]};
		++i;
		if(i == getSize(t)){
			break;
		}
		cdr->cdr = malloc(1*sizeof(LispVal));
		if(!cdr->cdr){
			deleteLispVal(&s);
			return s;
		}
		cdr = cdr->cdr;
		cdr->type = LIST;
	}
	return s;
}

LispVal exprLIST(Ptree *t){
	LispVal s = {.type = LIST};
	LispVal *cdr = &s;
	for(int i = 0;;){
		cdr->car = malloc(1*sizeof(LispVal));
		if(!cdr->car){
			deleteLispVal(&s);
			return s;
		}
		*(cdr->car) = expr(nthChild(t, i));
		++i;
		if(i == getSize(t)){
			break;
		}
		cdr->cdr = malloc(1*sizeof(LispVal));
		if(!cdr->cdr){
			deleteLispVal(&s);
			return s;
		}
		cdr = cdr->cdr;
		cdr->type = LIST;
	}
	return s;
}

LispVal exprQUOTE(const char *string, Ptree *t){
	LispVal s = {.type = LIST};
	s.car = malloc(1*sizeof(LispVal));
	if(!s.car){
		deleteLispVal(&s);
		return s;
	}
	*(s.car) = (LispVal){.type = NAME, .name = malloc(getLength(t)*sizeof(char))};
	if(!s.car->name){
		deleteLispVal(&s);
		return s;
	}
	strcpy(s.car->name, getString(t));
	s.cdr = malloc(1*sizeof(LispVal));
	if(!s.cdr){
		deleteLispVal(&s);
		return s;
	}
	*(s.cdr) = expr(t);
	return s;
}

LispVal expr(Ptree *t){
	if(!t){
		return (LispVal){.type = LIST};
	}
	const char *string = getString(t);
	if(!strcmp(string, "(name)")){
		return exprNAME(nthChild(t, 0));
	}
	if(!strcmp(string, "(number)")){
		return exprNUMBER(nthChild(t, 0));
	}
	if(!strcmp(string, "(bool)")){
		return exprBOOL(nthChild(t, 0));
	}
	if(!strcmp(string, "(string)")){
		return exprSTRING(nthChild(t, 0));
	}
	if(!strcmp(string, "(sxpr)")){
		return exprLIST(t);
	}
	if(!strcmp(string, "(quote)")){
		return exprQUOTE("quote", nthChild(t, 0));
	}
	if(!strcmp(string, "(quasiquote)")){
		return exprQUOTE("quasiquote", nthChild(t, 0));
	}
	if(!strcmp(string, "(unquote)")){
		return exprQUOTE("unquote", nthChild(t, 0));
	}
	if(!strcmp(string, "(unquote-splicing)")){
		return exprQUOTE("unquote-splicing", nthChild(t, 0));
	}
	//NYI
	return (LispVal){.type = LIST};
}

LispVal eval(LispVal expr, Env *env);

void printLispVal(const LispVal *v){
	switch(v->type){
		case NAME:
			printf("NAME:%s ", v->name);
			break;
		case LIST:
			printf("LIST:(");
			if(v->car){
				printLispVal(v->car);
			}
			if(v->cdr){
				printLispVal(v->cdr);
			}
			printf(")");
			break;
		case NUMBER:
			printf("NUMBER:%li ", v->number);
			break;
		case BOOL:
			printf(v->code?"BOOL:true ":"BOOL:false ");
			break;
		default:
			printf("NYI");
	}
}

