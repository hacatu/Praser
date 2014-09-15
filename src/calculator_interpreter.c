//calculator_interpreter.c
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "calculator_interpreter.h"
#include "stack.h"
#include "ptree.h"
#include "debug.h"

DEFINE_STACK(Stack, double)

/* This is possibly the simplest possible useful application of a parser.
 * The calculator interpreter reduces a ptree to a double, only remembering
 * tree values and only using nextPtree to walk the parse tree.  I think I'll
 * need more walking functions to build a real interpreter (you have to WALK
 * before you can RUN hehehe).
 */


double eval(Ptree *t){
	Stack s = CREATE_STACK(double);
	const char *tag;
	const char *tok;
	for(Ptree *c = firstPostorder(t); c; c = nextPostorder(c)){
		if(isTerminal(c)){
			continue;
		}
		if(dbg_log){
			printPtree(c, 0);
		}
		tag = getString(c);
		if(!strcmp(tag, "primary")){
			tok = getString(nthChild(c, 0));
			debug_log("primary");
			PUSH(double, s, atof(tok));
		}else if(!strcmp(tag, "additive")){
			tok = getString(nthChild(c, 1));
			double a = POP(s);
			double b = POP(s);
			if(!strcmp(tok, "+")){
				debug_log("addition");
				PUSH(double, s, b + a);
			}else if(!strcmp(tok, "-")){
				debug_log("subtraction");
				PUSH(double, s, b - a);
			}else{
				//there is some kind of error.
			}
		}else if(!strcmp(tag, "multiplicative")){
			tok = getString(nthChild(c, 1));
			double a = POP(s);
			double b = POP(s);
			if(!strcmp(tok, "*")){
				debug_log("multiplition");
				PUSH(double, s, b*a);
			}else if(!strcmp(tok, "/")){
				debug_log("division");
				PUSH(double, s, b/a);
			}else if(!strcmp(tok, "%")){
				debug_log("modulus");
				PUSH(double, s, fmod(b,a));
			}else{
				//there is some kind of error.
			}
		}
	}
	double result = POP(s);
	DELETE_STACK(s);
	return result;
}

