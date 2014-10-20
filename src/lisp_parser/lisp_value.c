//lisp_value.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lisp_value.h"

LispVal BASE_NYI = {.type = NYI};

LispVal BASE_NIL = {.type = LIST};

LispVal BASE_TRUE = {.type = BOOL, .code = 1};

LispVal BASE_FALSE = {.type = BOOL};

void deleteLispVal(LispVal v){
	if(!isLIST(v)){
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
	LispVal cell = {.type = LIST, .car = malloc(1*sizeof(LispVal)), .cdr = malloc(1*sizeof(LispVal))};
	if(!cell.car || !cell.cdr){
		return BASE_NYI;
	}
	*cell.car = a;
	*cell.cdr = d;
	return cell;
}

LispVal liftLAMBDA(LispVal params, LispVal body){
	LispVal lambda = {.type = LAMBDA};
	lambda.car = malloc(1*sizeof(LispVal));
	if(!lambda.car){
		return BASE_NYI;
	}
	lambda.cdr = malloc(1*sizeof(LispVal));
	if(!lambda.cdr){
		free(lambda.car);
		return BASE_NYI;
	}
	*lambda.car = params;
	*lambda.cdr = body;
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
	LispVal cpy = {.type = LIST};
	switch(v.type){
		case NAME:
		case NUMBER:
		case BOOL:
			return v;
		case LIST:
			cpy.type = LIST;
			cpy.car = malloc(1*sizeof(LispVal));
			if(!cpy.car){
				return BASE_NIL;
			}
			*cpy.car = copyLispVal(*v.car);
			cpy.cdr = malloc(1*sizeof(LispVal));
			if(!cpy.cdr){
				deleteLispVal(cpy);
				return BASE_NIL;
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
			printLispVal(car(v));
			printf(")->(");
			printLispVal(cdr(v));
			printf(")");
		default:
			printf("NYI");
	}
}

