//base_eval.c
#include <stdio.h>
#include <string.h>
#include "lisp_interpreter.h"
#include "lisp_value.h"
#include "lisp_env.h"

LispVal BASE_add(LispVal args, Env *env){
	LispVal v, r;
	if(isNIL(args)){
		return (LispVal){.type = NUMBER};
	}
	v = eval(car(args), env);
	if(!isNUMBER(v)){
		return BASE_NYI;
	}
	r = BASE_add(cdr(args), env);
	if(isNYI(r)){
		return BASE_NYI;
	}
	return (LispVal){.type = NUMBER, .number = v.number + r.number};
}

LispVal BASE_sub(LispVal args, Env *env){
	LispVal v, r;
	if(isNIL(args)){
		return BASE_NYI;
	}
	v = eval(car(args), env);
	if(!isNUMBER(v)){
		return BASE_NYI;
	}
	if(lengthLIST(args) == 1){
		return (LispVal){.type = NUMBER, .number = -v.number};
	}
	r = BASE_add(cdr(args), env);
	if(isNYI(r)){
		return BASE_NYI;
	}
	return (LispVal){.type = NUMBER, .number = v.number - r.number};
}

LispVal BASE_mul(LispVal args, Env *env){
	LispVal v, r;
	if(isNIL(args)){
		return (LispVal){.type = NUMBER, .number = 1};
	}
	v = eval(car(args), env);
	if(!isNUMBER(v)){
		return BASE_NYI;
	}
	r = BASE_mul(cdr(args), env);
	if(isNYI(r)){
		return BASE_NYI;
	}
	return (LispVal){.type = NUMBER, .number = v.number*r.number};
}

LispVal BASE_div(LispVal args, Env *env){
	LispVal v, r;
	if(isNIL(args)){
		return BASE_NYI;
	}
	v = eval(car(args), env);
	if(!isNUMBER(v)){
		return BASE_NYI;
	}
	if(lengthLIST(args) == 1){
		return (LispVal){.type = NUMBER, .number = 1/v.number};
	}
	r = BASE_mul(cdr(args), env);
	if(!isNUMBER(r)){
		return BASE_NYI;
	}
	return (LispVal){.type = NUMBER, .number = v.number/r.number};
}

LispVal BASE_quotient(LispVal args, Env *env){
	LispVal v, r;
	if(isNIL(args)){
		return BASE_NYI;
	}
	v = eval(car(args), env);
	if(!isNUMBER(v)){
		return BASE_NYI;
	}
	if(lengthLIST(args) == 1){
		return (LispVal){.type = NUMBER, .number = 1/v.number};
	}
	r = BASE_mul(cdr(args), env);
	if(!isNUMBER(r)){
		return BASE_NYI;
	}
	return (LispVal){.type = NUMBER, .number = v.number/r.number};
}

LispVal BASE_modulo(LispVal args, Env *env){
	LispVal v, r;
	if(isNIL(args)){
		return BASE_NYI;
	}
	v = eval(car(args), env);
	if(!isNUMBER(v)){
		return BASE_NYI;
	}
	if(lengthLIST(args) == 1){
		return (LispVal){.type = NUMBER, .number = v.number};
	}
	r = BASE_mul(cdr(args), env);
	if(isNYI(r)){
		return BASE_NYI;
	}
	return (LispVal){.type = NUMBER, .number = v.number%r.number};
}

LispVal BASE_lt(LispVal args, Env *env){
	LispVal a, b;
	if(lengthLIST(args) < 2){
		return BASE_NYI;
	}
	a = eval(car(args), env);
	if(!isNUMBER(a)){
		return BASE_NYI;
	}
	b = eval(car(cdr(args)), env);
	if(!isNUMBER(b)){
		return BASE_NYI;
	}
	return liftBOOL(a.number < b.number);
}

LispVal BASE_gt(LispVal args, Env *env){
	LispVal a, b;
	if(lengthLIST(args) < 2){
		return BASE_NYI;
	}
	a = eval(car(args), env);
	if(!isNUMBER(a)){
		return BASE_NYI;
	}
	b = eval(car(cdr(args)), env);
	if(!isNUMBER(b)){
		return BASE_NYI;
	}
	return liftBOOL(a.number > b.number);
}

LispVal BASE_integer(LispVal args, Env *env){
	if(isNIL(args)){
		return BASE_NYI;
	}
	LispVal v = eval(car(args), env);
	return liftBOOL(isNUMBER(v));
}

LispVal BASE_char(LispVal args, Env *env){
	if(isNIL(args)){
		return BASE_NYI;
	}
	LispVal v = eval(car(args), env);
	return liftBOOL(isCHAR(v));
}

LispVal BASE_bool(LispVal args, Env *env){
	if(isNIL(args)){
		return BASE_NYI;
	}
	LispVal v = eval(car(args), env);
	return liftBOOL(isBOOL(v));
}

LispVal BASE_list(LispVal args, Env *env){
	if(isNIL(args)){
		return BASE_NYI;
	}
	LispVal v = eval(car(args), env);
	return liftBOOL(isLIST(v));
}

LispVal BASE_quote(LispVal args, Env *env){//surprisingly, does not work at all (72 errors)
	LispVal copy = copyLispVal(args);
	if(isNYI(copy)){
		return BASE_NYI;
	}
	return copy;
}

LispVal BASE_car(LispVal args, Env *env){
	return car(eval(car(args), env));
}

LispVal BASE_cdr(LispVal args, Env *env){
	return cdr(eval(car(args), env));
}

LispVal BASE_cons(LispVal args, Env *env){//can leak memory
	LispVal a, d;
	if(lengthLIST(args) < 2){
		return BASE_NYI;
	}
	a = eval(car(args), env);
	d = eval(car(cdr(args)), env);
	return liftCONS(a, d);
}

LispVal BASE_if(LispVal args, Env *env){
	LispVal condition;
	if(lengthLIST(args) < 3){
		return BASE_NYI;
	}
	condition = eval(car(args), env);
	if(isNYI(condition) || isNIL(condition) || (isBOOL(condition) && condition.code)){
		return eval(car(cdr(args)), env);
	}
	return eval(car(cdr(cdr(args))), env);
}

//or and not

LispVal BASE_display(LispVal args, Env *env){
	if(isNIL(args)){
		puts("Nothing to display");
		return BASE_NYI;
	}
	printLispVal(eval(car(args), env));
	puts("");
	return BASE_NIL;
}

LispVal BASE_define(LispVal args, Env *env){
	LispVal k, v, c;
	if(lengthLIST(args) < 2){
		return BASE_NYI;
	}
	k = car(args);
	if(!isNAME(k)){
		return BASE_NYI;
	}
	v = eval(car(cdr(args)), env);
	if(isNYI(v)){
		return BASE_NYI;
	}
	c = copyLispVal(v);
	if(isNYI(c)){
		deleteLispVal(v);
		return BASE_NYI;
	}
	if(!addName(env, k.name, c)){
		return BASE_NYI;
	}
	return v;
}

LispVal BASE_eql(LispVal args, Env *env){
	LispVal a, b;
	if(lengthLIST(args) < 2){
		return BASE_TRUE;
	}
	a = eval(car(args), env);
	if(isNYI(a)){
		return BASE_NYI;
	}
	b = eval(car(cdr(args)), env);
	if(isNYI(b)){
		return BASE_NYI;
	}
	if(a.type != b.type){
		return BASE_FALSE;
	}
	switch(a.type){
		case NUMBER:
			return liftBOOL(a.number == b.number);
		case CHAR:
		case BOOL:
			return liftBOOL(a.code == b.code);
		default:
			return BASE_FALSE;
	}
}

LispVal BASE_eqv(LispVal args, Env *env){
	LispVal a, b;
	if(lengthLIST(args) < 2){
		return BASE_TRUE;
	}
	a = eval(car(args), env);
	if(isNYI(a)){
		return BASE_NYI;
	}
	b = eval(car(cdr(args)), env);
	if(isNYI(b)){
		return BASE_NYI;
	}
	return liftEQV(a, b);
}

LispVal BASE_set(LispVal args, Env *env){
	LispVal k, v;
	if(lengthLIST(args) < 2){
		return BASE_NYI;
	}
	k = car(args);
	if(!isNAME(k)){
		return BASE_NYI;
	}
	v = eval(car(cdr(args)), env);
	if(isNYI(v)){
		return BASE_NYI;
	}
	if(!setName(env, k.name, v)){
		return BASE_NYI;
	}
	return v;
}

LispVal BASE_let(LispVal args, Env *env){
	//(let ((name value) ...) (block))
	LispVal v, b;
	Env *new;
	if(lengthLIST(args) < 2){
		puts("let called on too few arguments, expected (let ((name value) ...) (block))");
		return BASE_NYI;
	}
	v = car(args);
	if(!isLIST(v)){
		puts("first argument to let needs to be a list, (let ((name value) ...) (block))");
		return BASE_NYI;
	}
	new = copyEnv(env);
	while(!isNIL(v)){
		b = car(v);
		if(lengthLIST(b) < 2 || !isNAME(car(b))){
			deleteEnv(new);
			puts("let expects a list of name value cells, ((name value) ...)");
			return BASE_NYI;
		}
		if(!addName(new, car(b).name, eval(car(cdr(b)), env))){
			deleteEnv(new);
			puts("addName failed in let");
			return BASE_NYI;
		}
		v = cdr(v);
	}
	b = eval(car(cdr(args)), new);
	deleteEnv(new);
	return b;
}

LispVal BASE_lambda(LispVal args, Env *env){
	if(lengthLIST(args) < 2){
		return BASE_NYI;
	}
	return liftLAMBDA(car(args), car(cdr(args)), env);
}

//TODO: Put BASE functions in search tree
LispVal evalBASE(LispVal expr, Env *env){
	const char *name = car(expr).name;
	if(!strcmp(name, "+")){
		return BASE_add(cdr(expr), env);
	}
	if(!strcmp(name, "-")){
		return BASE_sub(cdr(expr), env);
	}
	if(!strcmp(name, "*")){
		return BASE_mul(cdr(expr), env);
	}
	if(!strcmp(name, "/")){
		return BASE_div(cdr(expr), env);
	}
	if(!strcmp(name, "quotient")){
		return BASE_quotient(cdr(expr), env);
	}
	if(!strcmp(name, "modulo")){
		return BASE_modulo(cdr(expr), env);
	}
	if(!strcmp(name, "=")){
		return BASE_eql(cdr(expr), env);
	}
	if(!strcmp(name, "eqv?")){
		return BASE_eqv(cdr(expr), env);
	}
	if(!strcmp(name, "<")){
		return BASE_lt(cdr(expr), env);
	}
	if(!strcmp(name, ">")){
		return BASE_gt(cdr(expr), env);
	}
	if(!strcmp(name, "integer?")){
		return BASE_integer(cdr(expr), env);
	}
	if(!strcmp(name, "char?")){
		return BASE_char(cdr(expr), env);
	}
	if(!strcmp(name, "list?")){
		return BASE_list(cdr(expr), env);
	}
	if(!strcmp(name, "bool?")){
		return BASE_bool(cdr(expr), env);
	}
	if(!strcmp(name, "quote")){
		return BASE_quote(cdr(expr), env);
	}
	if(!strcmp(name, "car")){
		return BASE_car(cdr(expr), env);
	}
	if(!strcmp(name, "cdr")){
		return BASE_cdr(cdr(expr), env);
	}
	if(!strcmp(name, "cons")){
		return BASE_cons(cdr(expr), env);
	}
	if(!strcmp(name, "if")){
		return BASE_if(cdr(expr), env);
	}
	if(!strcmp(name, "display")){
		return BASE_display(cdr(expr), env);
	}
	if(!strcmp(name, "exit")){
		return BASE_NYI;
	}
	if(!strcmp(name, "define")){
		return BASE_define(cdr(expr), env);
	}
	if(!strcmp(name, "lambda")){
		return BASE_lambda(cdr(expr), env);
	}
	if(!strcmp(name, "set!")){
		return BASE_set(cdr(expr), env);
	}
	if(!strcmp(name, "let")){
		return BASE_let(cdr(expr), env);
	}
	/*
	if(!strcmp(name, "let*")){
		return BASE_let_(expr.cdr, env);
	}
	if(!strcmp(name, "letrec")){
		return BASE_letrec(expr.cdr, env);
	}
	*/
	return BASE_NYI;
}

