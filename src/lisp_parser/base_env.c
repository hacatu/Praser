//base_env.c
#include <stdio.h>
#include <stdlib.h>
#include "lisp_interpreter.h"
#include "lisp_value.h"
#include "lisp_env.h"
#include "base_env.h"

#define lengthof(list) (sizeof(list)/sizeof(list[0]))

//BUILTIN FUNCTIONS: + - * / magnitude quotient modulo remainder = eqv? < > integer? string? char?
//list? bool? quote car cdr cons if display exit define lambda set? let let* letrec unquote
//quasiquote unquote-splicing

Env *baseEnv;
LispVal BASE_nil;

char initBaseEnv(){
	BASE_nil = BASE_NIL;
	Attribute baseAttrs[] = {
		{"nil", &BASE_nil},
		/*
		{"min", BASE_min},
		{"max", BASE_max},
		{"magnitude", BASE_magnitude},
		{"remainder", BASE_remainder},
		{">=", BASE_gte},
		{"<=", BASE_lte},
		{"zero?", BASE_zero},
		{"positive?", BASE_positive},
		{"negative?", BASE_negative},
		{"list", BASE_list},
		{"length", BASE_length},
		{"append", BASE_append},
		{"reverse", BASE_reverse},
		{"null?", BASE_null},
		{"apply", BASE_apply},
		{"map", BASE_map},
		{"do", BASE_do},
		{"cond", BASE_cond},
		{"case", BASE_case},
		{"begin", BASE_begin}
		*/
	};
	baseEnv = createEnvFrom(lengthof(baseAttrs), baseAttrs);
	return 1;
}

