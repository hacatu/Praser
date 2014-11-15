//parser_llk.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ptree/_ptree.h"
#include "_parser.h"
#include "../util/debug.h"


//TODO: replace AppendMode with three functions: add, skip, and pass, and make parsers return ptrees instead of taking them

static int cschar(Position *p, Ptree *t){
	if(currentChar(p) == '\\'){
		getChar(p);
		appendString(t, p->current, 1);
		getChar(p);
		return 1;
	}
	return noneOf(p, t, PASS, "\"");
}

int cstring(Position *p, Ptree *t){
	if(!acceptString(p, t, SKIP, "\"")){
		return 0;
	}
	if(!repeat(p, t, PASS, cschar, 0, 0)){
		return 0;
	}
	if(!acceptString(p, t, SKIP, "\""));
	return 1;
}

int cchar(Position *p, Ptree *t){
	if(!acceptString(p, t, SKIP, "'")){
		return 0;
	}
	if(currentChar(p) == '\\'){
		getChar(p);
		appendString(t, p->current, 1);
		getChar(p);
		return acceptString(p, t, SKIP, "'");
	}
	if(!noneOf(p, t, PASS, "'")){
		return 0;
	}
	return acceptString(p, t, SKIP, "'");
}

static int space(Position *p, Ptree *t){
	return oneOf(p, t, SKIP, " \t\n\r\v\f");
}

int spaces(Position *p, Ptree *t){
	return repeat(p, t, SKIP, space, 1, 0);
}

int letter(Position *p, Ptree *t){
	return oneOf(p, t, PASS, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

int digit(Position *p, Ptree *t){
	return oneOf(p, t, PASS, "0123456789");
}

int integer(Position *p, Ptree *t){
	return repeat(p, t, PASS, digit, 1, 0);
}

