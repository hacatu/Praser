//lisp_interpreter.h
#pragma once
#include <stdint.h>
#include "../ptree/ptree.h"

typedef enum{NAME, LIST, NUMBER, STRING, CHAR, BOOL} LispType;

typedef struct Env Env;

typedef struct LispVal{
	LispType type;
	union{
		char *name;
		char code;
		struct{
			struct LispVal *car;
			struct LispVal *cdr;
		};
		uint64_t number;
	};
} LispVal;

void deleteLispVal(LispVal *v);

LispVal expr(Ptree *t);

LispVal eval(LispVal expr, Env *env);

void printLispVal(const LispVal *v);

