//parser_parser.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <get_line.h>
#include <parser.h>
#include <ptree.h>
#include "parser_interpreter.h"
#include <debug.h>

/* GRAMMAR
 * "string"        string //escape sequences nyi in praser lib
 * symbol          symbol
 * (group)         group
 * (name: group)   named group
 * expr?           optional expression
 * expr+           repeat 1 or more times
 * expr*           repeat 0 or more times
 * expr{n,m}       repeat n to m times
 * [charset]       charset
 * [^charset]      inverted charset
 * ^expr           add in current ptree instead of adding a new one
 * /expr           skip over text
 * name            must have only letters and numbers
 * expr expr       just put expressions after each other
 * name = expr     define a rule
 */

int start(PRA_Position *p, PRA_Ptree *t);//done
int rule(PRA_Position *p, PRA_Ptree *t);//done
int name(PRA_Position *p, PRA_Ptree *t);//done
int charset(PRA_Position *p, PRA_Ptree *t);//done
int group(PRA_Position *p, PRA_Ptree *t);
int repeat(PRA_Position *p, PRA_Ptree *t);//done
int appendMode(PRA_Position *p, PRA_Ptree *t);//done
int expr(PRA_Position *p, PRA_Ptree *t);//done
int symbol(PRA_Position *p, PRA_Ptree *t);//done
int gname(PRA_Position *p, PRA_Ptree *t);//done
int charsetchar(PRA_Position *p, PRA_Ptree *t);//done

int start(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	if(!PRA_alternate(p, t, PRA_ADD, PRA_SKIP, rule, PRA_newline, 0, 0)){
		PRA_logUnexpectedError(p, "start", "rule");
		return 0;
	}
	if(!PRA_acceptEnd(p)){
		PRA_logUnexpectedError(p, "start", "end of input");
		return 0;
	}
	return 1;
}

int rule(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	if(!PRA_accept(p, t, PRA_ADD, name)){
		PRA_logUnexpectedError(p, "rule", "name");
		return 0;
	}
	PRA_accept(p, t, PRA_SKIP, PRA_spaces);
	if(!PRA_acceptString(p, t, PRA_SKIP, "=")){
		PRA_logUnexpectedError(p, "rule", "=");
		return 0;
	}
	PRA_accept(p, t, PRA_SKIP, PRA_spaces);
	if(!PRA_accept(p, t, PRA_ADD, expr)){
		PRA_logUnexpectedError(p, "rule", "=");
		return 0;
	}
	return 1;
}

int name(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	return PRA_repeat(p, t, PRA_ADD, PRA_alphanum, 1, 0);
}

int appendMode(PRA_Position *p, PRA_Ptree *t){
	if(PRA_acceptString(p, t, PRA_SKIP, "^")){
		PRA_setString(t, "PRA_PASS", 8);
		return 1;
	}
	if(PRA_acceptString(p, t, PRA_SKIP, "/")){
		PRA_setString(t, "PRA_SKIP", 8);
		return 1;
	}
	PRA_setString(t, "PRA_ADD", 7);
	return 1;
}

int repeat(PRA_Position *p, PRA_Ptree *t){
	if(PRA_acceptString(p, t, PRA_SKIP, "?")){
		PRA_setString(t, "?", 1);
		return 1;
	}
	if(PRA_acceptString(p, t, PRA_SKIP, "+")){
		PRA_setString(t, "+", 1);
		return 1;
	}
	if(PRA_acceptString(p, t, PRA_SKIP, "*")){
		PRA_setString(t, "*", 1);
		return 1;
	}
	if(PRA_acceptString(p, t, PRA_SKIP, "{")){
		if(!PRA_accept(p, t, PRA_ADD, PRA_integer)){
			PRA_logUnexpectedError(p, "repeat", "integer");
			return 0;
		}
		if(!PRA_acceptString(p, t, PRA_SKIP, ",")){
			PRA_logUnexpectedError(p, "repeat", ",");
			return 0;
		}
		if(!PRA_accept(p, t, PRA_ADD, PRA_integer)){
			PRA_logUnexpectedError(p, "repeat", "integer");
			return 0;
		}
		if(!PRA_acceptString(p, t, PRA_SKIP, "}")){
			PRA_logUnexpectedError(p, "repeat", "}");
			return 0;
		}
		PRA_setString(t, ",", 1);
		return 1;
	}
	PRA_setString(t, "1", 1);
	return 1;
}

int expr(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	if(!PRA_sepBy(p, t, PRA_ADD, PRA_SKIP, group, PRA_spaces, 1, 0)){
		PRA_logUnexpectedError(p, "expr", "group");
		return 0;
	}
	return 1;
}

int group(PRA_Position *p, PRA_Ptree *t){//fix
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	if(!PRA_accept(p, t, PRA_ADD, appendMode)){
		return 0;
	}
	if(PRA_acceptString(p, t, PRA_SKIP, "(")){
		if(!PRA_accept(p, t, PRA_ADD, symbol)){
			PRA_logUnexpectedError(p, "group", "symbol");
			return 0;
		}
		
	}else{
		PRA_accept(p, t, PRA_ADD, gname);
		PRA_accept(p, t, PRA_SKIP, PRA_spaces);
		if(!PRA_accept(p, t, PRA_ADD, expr)){
			PRA_logUnexpectedError(p, "group", "expr");
			return 0;
		}
	}
	if(!PRA_accept(p, t, PRA_ADD, repeat)){
		PRA_logUnexpectedError(p, "group", "repeat");
		return 0;
	}
	return 1;
}

int gname(PRA_Position *p, PRA_Ptree *t){
	PRA_setString(t, __FUNCTION__, strlen(__FUNCTION__));
	if(!PRA_accept(p, t, PRA_ADD, name)){
		PRA_logUnexpectedError(p, "gname", "name");
		return 0;
	}
	if(!PRA_acceptString(p, t, PRA_SKIP, ":")){
		PRA_logUnexpectedError(p, "gname", ":");
		return 0;
	}
	PRA_accept(p, t, PRA_SKIP, PRA_spaces);
	return 1;
}

int charset(PRA_Position *p, PRA_Ptree *t){
	if(!PRA_acceptString(p, t, PRA_SKIP, "[")){
		return 0;
	}
	if(PRA_acceptString(p, t, PRA_SKIP, "^")){
		PRA_setString(t, "invcharset", 10);
	}else{
		PRA_setString(t, "charset", 7);
	}
	PRA_repeat(p, t, PRA_ADD, charsetchar, 0, 0);
	if(!PRA_acceptString(p, t, PRA_SKIP, "]")){
		PRA_logUnexpectedError(p, "charset", "]");
		return 0;
	}
	return 1;
}

int charsetchar(PRA_Position *p, PRA_Ptree *t){
	if(PRA_noneOf(p, t, PRA_PASS, "\\]")){
		return 1;
	}
	if(PRA_acceptString(p, t, PRA_SKIP, "\\")){
		return PRA_acceptString(p, t, PRA_PASS, "]");
	}
	return 0;
}

int symbol(PRA_Position *p, PRA_Ptree *t){
	if(PRA_accept(p, t, PRA_PASS, charset)){
		return 1;
	}
	if(PRA_accept(p, t, PRA_PASS, PRA_cstring)){
		PRA_setString(t, "string", 6);
		return 1;
	}
	return PRA_accept(p, t, PRA_PASS, name);
}

