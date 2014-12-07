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
#include <ptree.h>
#include <debug.h>

//TODO: Implement CHAR

LispVal exprNAME(PRA_Ptree *t){
	LispVal temp = {.type = NAME, .name = malloc((PRA_getLength(t) + 1)*sizeof(char))};
	if(temp.name){
		strcpy(temp.name, PRA_getString(t));
	}
	return temp;
}

LispVal exprNUMBER(PRA_Ptree *t){
	return (LispVal){.type = NUMBER, .number = atoll(PRA_getString(t))};
}

LispVal exprBOOL(PRA_Ptree *t){
	return liftBOOL(PRA_getString(t)[0] == 't');
}

//TODO: Create some kind of malloc LispVal function to malloc and assign a LispVal in one go
LispVal exprSTRING(PRA_Ptree *t){
	LispVal s = BASE_NIL;
	LispVal *cdr = &s;
	const char *string = PRA_getString(t);
	for(int i = 0; i < PRA_getLength(t); ++i){
		cdr->car = malloc(1*sizeof(LispVal));
		if(!cdr->car){
			puts("exprLIST error: malloc failed");
			deleteLispVal(s);
			break;
		}
		cdr->cdr = malloc(1*sizeof(LispVal));
		if(!cdr->cdr){
			puts("exprLIST error: malloc failed");
			deleteLispVal(s);
			break;
		}
		*(cdr->car) = (LispVal){.type = CHAR, .code = string[i]};
		 cdr = cdr->cdr;
		 *cdr = BASE_NIL;
	}
	return s;
}

LispVal exprLIST(PRA_Ptree *t){
	LispVal s = BASE_NIL;
	LispVal *cdr = &s;
	for(int i = 0; i < PRA_getSize(t); ++i){
		cdr->car = malloc(1*sizeof(LispVal));
		if(!cdr->car){
			puts("exprLIST error: malloc failed");
			deleteLispVal(s);
			break;
		}
		cdr->cdr = malloc(1*sizeof(LispVal));
		if(!cdr->cdr){
			puts("exprLIST error: malloc failed");
			deleteLispVal(s);
			break;
		}
		*(cdr->car) = expr(PRA_nthChild(t, i));
		 cdr = cdr->cdr;
		 *cdr = BASE_NIL;
	}
	return s;
}

LispVal exprQUOTE(const char *string, PRA_Ptree *t){
	LispVal s = BASE_NIL;
	s.car = malloc(1*sizeof(LispVal));
	if(!s.car){
		deleteLispVal(s);
		return s;
	}
	*(s.car) = (LispVal){.type = NAME, .name = malloc(PRA_getLength(t)*sizeof(char))};
	if(!s.car->name){
		deleteLispVal(s);
		return s;
	}
	strcpy(s.car->name, PRA_getString(t));
	s.cdr = malloc(1*sizeof(LispVal));
	if(!s.cdr){
		deleteLispVal(s);
		return s;
	}
	*(s.cdr) = expr(t);
	return s;
}

LispVal expr(PRA_Ptree *t){
	if(!t){
		puts("expr error: null PRA_Ptree");
		return BASE_NYI;
	}
	const char *string = PRA_getString(t);
	if(!strcmp(string, "(name)")){
		return exprNAME(PRA_nthChild(t, 0));
	}
	if(!strcmp(string, "(number)")){
		return exprNUMBER(PRA_nthChild(t, 0));
	}
	if(!strcmp(string, "(bool)")){
		return exprBOOL(PRA_nthChild(t, 0));
	}
	if(!strcmp(string, "(string)")){
		return exprSTRING(PRA_nthChild(t, 0));
	}
	if(!strcmp(string, "(sxpr)")){
		return exprLIST(t);
	}
	if(!strcmp(string, "(quote)")){
		return exprQUOTE("quote", PRA_nthChild(t, 0));
	}
	if(!strcmp(string, "(quasiquote)")){
		return exprQUOTE("quasiquote", PRA_nthChild(t, 0));
	}
	if(!strcmp(string, "(unquote)")){
		return exprQUOTE("unquote", PRA_nthChild(t, 0));
	}
	if(!strcmp(string, "(unquote-splicing)")){
		return exprQUOTE("unquote-splicing", PRA_nthChild(t, 0));
	}
	//NYI
	printf("expr error: invalid string %s\n", string);
	return BASE_NYI;
}

LispVal evalLAMBDA(LispVal lambda, LispVal args, Env *env){//TODO: fix
	LispVal a = *lambda.params;
	if(lengthLIST(a) > lengthLIST(args)){
		return BASE_NYI;
	}
	Env *new = copyEnv(lambda.closure);
	if(!new){
		return BASE_NYI;
	}
	while(!isNIL(a)){
		if(!addName(new, car(a).name, car(args))){
			deleteEnv(new);
			return BASE_NYI;
		}
		a = cdr(a);
		args = cdr(args);
	}
	printLispVal(*lambda.body);
	puts("");
	a = eval(*lambda.body, new);
	deleteEnv(new);
	return a;
}

LispVal evalLIST(LispVal expr, Env *env){
	LispVal a = car(expr);
	if(isLIST(a)){
		a = evalLIST(a, env);
	}else if(isNAME(a)){
		a = getName(env, a.name);
		if(isNYI(a)){
			return evalBASE(expr, env);
		}
	}
	if(lengthLIST(expr) == 1){
		return a;
	}
	if(isLAMBDA(a)){
		return evalLAMBDA(a, cdr(expr), env);
	}
	return liftCONS(a, cdr(expr));
}

LispVal eval(LispVal expr, Env *env){
	switch(expr.type){
		case NAME:
			return getName(env, expr.name);
		case LIST:
			return evalLIST(expr, env);
		default:
			return copyLispVal(expr);
	}
}

