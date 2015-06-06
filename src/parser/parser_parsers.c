//parser_parsers.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ptree/_ptree.h"
#include "_parser.h"
#include "../util/debug.h"


static int cschar(PRA_Position *p, PRA_Ptree *t){
	if(PRA_currentChar(p) == '\\'){
		PRA_getChar(p);
		PRA_appendString(t, (const char[]){PRA_currentChar(p), '\0'}, 1);
		PRA_getChar(p);
		return 1;
	}
	return PRA_noneOf(p, t, PRA_PASS, "\"");
}

int PRA_cstring(PRA_Position *p, PRA_Ptree *t){
	if(!PRA_acceptString(p, t, PRA_SKIP, "\"")){
		return 0;
	}
	if(!PRA_repeat(p, t, PRA_PASS, cschar, 0, 0)){
		return 0;
	}
	if(!PRA_acceptString(p, t, PRA_SKIP, "\"")){
		return 0;
	}
	return 1;
}

int cchar(PRA_Position *p, PRA_Ptree *t){
	if(!PRA_acceptString(p, t, PRA_SKIP, "'")){
		return 0;
	}
	if(PRA_currentChar(p) == '\\'){
		PRA_getChar(p);
		PRA_appendString(t, (const char[]){PRA_currentChar(p), '\0'}, 1);
		PRA_getChar(p);
		return PRA_acceptString(p, t, PRA_SKIP, "'");
	}
	if(!PRA_noneOf(p, t, PRA_PASS, "'")){
		return 0;
	}
	return PRA_acceptString(p, t, PRA_SKIP, "'");
}

static int space(PRA_Position *p, PRA_Ptree *t){
	return PRA_oneOf(p, t, PRA_SKIP, " \t\n\r\v\f");
}

int PRA_spaces(PRA_Position *p, PRA_Ptree *t){
	return PRA_repeat(p, t, PRA_SKIP, space, 1, 0);
}

int PRA_newline(PRA_Position *p, PRA_Ptree *t){
	return PRA_acceptString(p, t, PRA_SKIP, "\n");
}

int PRA_letter(PRA_Position *p, PRA_Ptree *t){
	return PRA_oneOf(p, t, PRA_PASS, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

int PRA_digit(PRA_Position *p, PRA_Ptree *t){
	return PRA_oneOf(p, t, PRA_PASS, "0123456789");
}

int PRA_alphanum(PRA_Position *p, PRA_Ptree *t){
	return PRA_oneOf(p, t, PRA_PASS, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

int PRA_integer(PRA_Position *p, PRA_Ptree *t){
	return PRA_repeat(p, t, PRA_PASS, PRA_digit, 1, 0);
}

