//lisp_value.h
#pragma once
#include <stdlib.h>
#include <stdint.h>

typedef enum{NYI, NAME, LIST, NUMBER, CHAR, BOOL, LAMBDA} LispType;

//TODO: Implement CHAR

typedef struct LispVal{
	LispType type;
	union{
		char *name;
		char code;
		struct{
			struct LispVal *car;//params for lambda
			struct LispVal *cdr;//body for lambda
		};
		uint64_t number;
	};
} LispVal;

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

LispVal liftLAMBDA(LispVal params, LispVal body);

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

