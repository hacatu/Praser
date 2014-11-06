//lisp_env.h
#pragma once
#include "lisp_all_types.h"
#include "lisp_value.h"

struct Attribute{
	char *name;
	LispVal value;
};

Env* createOneAttrEnv(Attribute attr);

char addAttr(Env *e, Attribute attr);

char addName(Env *e, const char *name, LispVal val);

LispVal getName(Env *e, const char *name);

char setName(Env *e, const char *name, LispVal val);

void deleteEnv(Env *e);

void deleteEnvData(Env *e);

Env* createEnvFrom(Attribute attrv[]);

Env* copyEnv(const Env *e);

