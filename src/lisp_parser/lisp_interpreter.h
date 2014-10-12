//lisp_interpreter.h
#pragma once
#include <stdint.h>
#include "lisp_value.h"
#include "../ptree/ptree.h"

typedef struct Env Env;

LispVal expr(Ptree *t);

LispVal eval(LispVal expr, Env *env);

