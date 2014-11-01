//lisp_value.h
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include "lisp_all_types.h"

enum LispType{NYI, NAME, LIST, NUMBER, CHAR, BOOL, LAMBDA};

//TODO: Implement CHAR

struct LispVal{
	LispType type;
	union{
		char *name;//NAME
		char code;//CHAR, BOOL
		struct{//LIST
			struct LispVal *car;
			struct LispVal *cdr;
		};
		struct{//LAMBDA
			struct LispVal *params;
			struct LispVal *body;
			Env *closure;
		};
		uint64_t number;//NUMBER
	};
};

void deleteLispVal(LispVal v);

size_t lengthLIST(LispVal list);

char isLIST(LispVal v);

char isNUMBER(LispVal v);

char isCHAR(LispVal v);

char isBOOL(LispVal v);

char isNIL(LispVal v);

char isNYI(LispVal v);

char isNAME(LispVal v);

char isLAMBDA(LispVal v);

LispVal liftBOOL(char b);

LispVal liftAND(LispVal a, LispVal b);

LispVal liftCONS(LispVal a, LispVal d);

LispVal liftLAMBDA(LispVal params, LispVal body, Env *closure);

LispVal liftEQV(LispVal a, LispVal b);

LispVal car(LispVal v);

LispVal cdr(LispVal v);

//TODO: Make iterative
LispVal copyLispVal(LispVal v);

void printLispVal(LispVal v);

extern LispVal BASE_NYI;

extern LispVal BASE_NIL;

extern LispVal BASE_TRUE;

extern LispVal BASE_FALSE;

