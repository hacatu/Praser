//lisp_interpreter.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "lisp_interpreter.h"
#include "lisp_env.h"
#include "base_eval.h"
#include "lisp_value.h"
#include "../ptree/ptree.h"
#include "../util/debug.h"

//TODO: Implement CHAR

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

//TODO: Create some kind of malloc LispVal function to malloc and assign a LispVal in one go
LispVal exprSTRING(Ptree *t){
	LispVal s = {.type = LIST};
	LispVal *cdr = &s;
	const char *string = getString(t);
	for(int i = 0;;){
		cdr->car = malloc(1*sizeof(LispVal));
		if(!cdr->car){
			deleteLispVal(s);
			return s;
		}
		*(cdr->car) = (LispVal){.type = CHAR, .code = string[i]};
		++i;
		if(i == getSize(t)){
			break;
		}
		cdr->cdr = malloc(1*sizeof(LispVal));
		if(!cdr->cdr){
			deleteLispVal(s);
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
			deleteLispVal(s);
			return s;
		}
		*(cdr->car) = expr(nthChild(t, i));
		++i;
		if(i == getSize(t)){
			break;
		}
		cdr->cdr = malloc(1*sizeof(LispVal));
		if(!cdr->cdr){
			deleteLispVal(s);
			return s;
		}
		cdr = cdr->cdr;
		cdr->type = LIST;
	}
	return s;
}

LispVal exprQUOTE(const char *string, Ptree *t){
	LispVal s = BASE_NIL;
	s.car = malloc(1*sizeof(LispVal));
	if(!s.car){
		deleteLispVal(s);
		return s;
	}
	*(s.car) = (LispVal){.type = NAME, .name = malloc(getLength(t)*sizeof(char))};
	if(!s.car->name){
		deleteLispVal(s);
		return s;
	}
	strcpy(s.car->name, getString(t));
	s.cdr = malloc(1*sizeof(LispVal));
	if(!s.cdr){
		deleteLispVal(s);
		return s;
	}
	*(s.cdr) = expr(t);
	return s;
}

LispVal expr(Ptree *t){
	if(!t){
		return BASE_NIL;
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
	return BASE_NIL;
}

LispVal evalLAMBDA(LispVal lambda, LispVal args, Env *env){
	LispVal a = car(lambda);
	if(lengthLIST(a) > lengthLIST(args)){
		return BASE_NYI;
	}
	Env *new = copyEnv(env);
	while(!isNIL(a)){
		if(!addName(new, car(a).name, car(args))){
			deleteEnv(new);
			return BASE_NYI;
		}
		a = cdr(a);
		args = cdr(args);
	}
	a = eval(cdr(lambda), new);
	deleteEnv(new);
	return a;
}

LispVal evalLIST(LispVal expr, Env *env){
	LispVal a = car(expr);
	if(isLIST(a)){
		a = evalLIST(a, env);
	}else if(isNAME(a)){
		a = getName(env, car(expr).name);
		if(isNYI(a)){
			return evalBASE(expr, env);
		}
	}
	if(isLAMBDA(a)){
		return evalLAMBDA(a, cdr(expr), env);
	}
	return expr;
}

LispVal eval(LispVal expr, Env *env){
	switch(expr.type){
		case NAME:
			return getName(env, expr.name);
		case LIST:
			return evalLIST(expr, env);
		default:
			return expr;
	}
}

