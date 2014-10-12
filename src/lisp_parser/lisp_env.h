//lisp_env.h
#pragma once
#include "lisp_interpreter.h"
#include "stdlib.h"

typedef struct Attribute{
	const char *name;
	LispVal value;
} Attribute;

Env* createOneAttrEnv(Attribute attr);

char addAttr(Env *e, Attribute attr);

char addName(Env *e, const char *name, LispVal val);

LispVal getName(Env *e, const char *name);

char setName(Env *e, const char *name, LispVal val);

void deleteEnv(Env *e);

Env* createEnvFrom(size_t attrc, Attribute attrv[]);

Env* copyEnv(const Env *e);

