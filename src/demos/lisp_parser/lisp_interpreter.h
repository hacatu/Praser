//lisp_interpreter.h
#pragma once
#include <stdint.h>
#include "lisp_all_types.h"
#include <ptree.h>

LispVal expr(PRA_Ptree *t);

LispVal eval(LispVal expr, Env *env);

