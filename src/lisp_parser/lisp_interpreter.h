//lisp_interpreter.h
#pragma once
#include <stdint.h>
#include "lisp_all_types.h"
#include "../ptree/ptree.h"

LispVal expr(Ptree *t);

LispVal eval(LispVal expr, Env *env);

