//lisp_value.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lisp_env.h"
#include "lisp_value.h"

LispVal BASE_NYI = {.type = NYI};

LispVal BASE_NIL = {.type = LIST};

LispVal BASE_TRUE = {.type = BOOL, .code = 1};

LispVal BASE_FALSE = {.type = BOOL};

void deleteLispVal(LispVal v){
	//puts("deleteLispVal called");
	//printLispVal(v);
	//puts("");
	if(!isLIST(v)){
		if(isNAME(v)){
			free(v.name);
			return;
		}
		if(isLAMBDA(v)){
			LispVal *l = v.params;
			Env *e;
			if(l){
				deleteLispVal(*l);
				free(l);
				v.params = NULL;
			}
			l = v.body;
			if(l){
				deleteLispVal(*l);
				free(l);
				v.body = NULL;
			}
			e = v.closure;
			if(e){
				deleteEnv(e);
				e = NULL;
			}
		}
		return;
	}
	if(isNIL(v)){
		return;
	}
	deleteLispVal(car(v));
	free(v.car);
	v.car = NULL;
	deleteLispVal(cdr(v));
	free(v.cdr);
	v.cdr = NULL;
}

size_t lengthLIST(LispVal list){
	if(isNYI(list)){
		return 0;
	}
	if(!isLIST(list)){
		return 1;
	}
	if(isNIL(list)){
		return 0;
	}
	return 1 + lengthLIST(cdr(list));
}

char isLIST(LispVal v){
	return v.type == LIST;
}

char isNUMBER(LispVal v){
	return v.type == NUMBER;
}

char isCHAR(LispVal v){
	return v.type == CHAR;
}

char isBOOL(LispVal v){
	return v.type == BOOL;
}

char isNIL(LispVal v){
	if(isLIST(v)){
		return !v.cdr;
	}
	return 0;
}

char isNYI(LispVal v){
	return v.type == NYI;
}

char isNAME(LispVal v){
	return v.type == NAME;
}

char isLAMBDA(LispVal v){
	return v.type == LAMBDA;
}

LispVal liftBOOL(char b){
	return (LispVal){.type = BOOL, .code = b};
}

LispVal liftAND(LispVal a, LispVal b){
	if(!isBOOL(a) || !isBOOL(b)){
		return BASE_NYI;
	}
	return liftBOOL(a.code && b.code);
}

LispVal liftCONS(LispVal a, LispVal d){
	LispVal cell = {.type = LIST};
	cell.car = malloc(1*sizeof(LispVal));
	if(!cell.car){
		return BASE_NYI;
	}
	cell.cdr = malloc(1*sizeof(LispVal));
	if(!cell.cdr){
		free(cell.car);
		return BASE_NYI;
	}
	*cell.car = a;
	*cell.cdr = d;
	return cell;
}

LispVal liftLAMBDA(LispVal params, LispVal body, Env *closure){
	LispVal lambda = {.type = LAMBDA};
	lambda.params = malloc(1*sizeof(LispVal));
	if(!lambda.params){
		return BASE_NYI;
	}
	lambda.body = malloc(1*sizeof(LispVal));
	if(!lambda.body){
		free(lambda.params);
		return BASE_NYI;
	}
	lambda.closure = copyEnv(closure);
	if(!lambda.closure){
		free(lambda.params);
		free(lambda.body);
		return BASE_NYI;
	}
	//TODO: possibly initialize parameters to BASE_NIL here
	*lambda.params = copyLispVal(params);
	*lambda.body = copyLispVal(body);
	return lambda;
}

LispVal liftEQV(LispVal a, LispVal b){
	if(a.type != b.type){
		return BASE_FALSE;
	}
	switch(a.type){
		case CHAR:
		case BOOL:
			return liftBOOL(a.code == b.code);
		case NUMBER:
			return liftBOOL(a.number == b.number);
		case LIST:
			if(isNIL(a) && isNIL(b)){
				return BASE_TRUE;
			}
			return liftAND(liftEQV(car(a), car(b)), liftEQV(cdr(a), cdr(b)));
		default:
			return BASE_FALSE;
	}
}

LispVal car(LispVal v){
	if(!isLIST(v)){
		return BASE_NYI;
	}
	if(isNIL(v)){
		return BASE_NYI;
	}
	return *v.car;
}

LispVal cdr(LispVal v){
	if(!isLIST(v)){
		return BASE_NYI;
	}
	if(isNIL(v)){
		return BASE_NYI;
	}
	return *v.cdr;
}

//TODO: Make iterative
LispVal copyLispVal(LispVal v){
	LispVal cpy = {0};
	switch(v.type){
		case NUMBER:
		case BOOL:
			return v;
		case NAME:
			cpy.type = NAME;
			cpy.name = malloc((strlen(v.name) + 1)*sizeof(char));//TODO: not use strlen
			if(!cpy.name){
				return BASE_NYI;
			}
			strcpy(cpy.name, v.name);
			return cpy;
		case LIST:
			if(isNIL(v)){
				return BASE_NIL;
			}
			cpy.type = LIST;
			cpy.car = malloc(1*sizeof(LispVal));
			if(!cpy.car){
				return BASE_NYI;
			}
			*cpy.car = copyLispVal(*v.car);
			cpy.cdr = malloc(1*sizeof(LispVal));
			if(!cpy.cdr){
				deleteLispVal(cpy);
				return BASE_NYI;
			}
			*cpy.cdr = copyLispVal(*v.cdr);
			return cpy;
		default:
			//NYI
			return BASE_NYI;
	}
}

void printLispVal(const LispVal v){
	switch(v.type){
		case NAME:
			printf("NAME:%s", v.name);
			break;
		case LIST:
			if(isNIL(v)){
				printf("NIL");
				break;
			}
			printf("LIST:(");
			printLispVal(car(v));
			printf(" ");
			printLispVal(cdr(v));
			printf(")");
			break;
		case NUMBER:
			printf("NUMBER:%li", v.number);
			break;
		case BOOL:
			printf(v.code?"BOOL:true":"BOOL:false");
			break;
		case CHAR:
			printf("'%c'", v.code);
			break;
		case LAMBDA:
			printf("LAMBDA:(");
			printLispVal(*v.params);
			printf(")->(");
			printLispVal(*v.body);
			printf("))");
			break;
		default:
			printf("NYI");
	}
}

