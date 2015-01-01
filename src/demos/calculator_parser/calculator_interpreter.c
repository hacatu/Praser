//calculator_interpreter.c
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "calculator_interpreter.h"
#include <ptree.h>
#include <debug.h>


/* This is possibly the simplest possible useful application of a parser.
 * The calculator interpreter reduces a PRA_Ptree to a double.
 */


double eval(PRA_Ptree *t){
	const char *tag;
	double val, cur;
	if(PRA_isTerminal(t)){//something went wrong; eval should only be called on tagged Ptrees
		return NAN;
	}
	tag = PRA_getString(t);
	if(!strcmp(tag, "primary")){//tag == "primary"
		return (double)atoi(PRA_getString(PRA_nthChild(t, 0)));
	}
	if(!strcmp(tag, "additive")){//tag == "additive"
		val = eval(PRA_nthChild(t, 0));
		for(int i = 1; i < PRA_getSize(t); i += 2){//loop over all of the operators, which are every other node.
			cur = eval(PRA_nthChild(t, i + 1));
			switch(PRA_getString(PRA_nthChild(t, i))[0]){
				case '+':
					val += cur;
					break;
				case '-':
					val -= cur;
					break;
				default://something went wrong; additive means the operators should all be + or -
					return NAN;
			}
		}
		return val;
	}
	if(!strcmp(tag, "multiplicative")){//tag == "multiplicative"
		val = eval(PRA_nthChild(t, 0));
		for(int i = 1; i < PRA_getSize(t); i += 2){//loop over all of the operators, which are every other node.
			cur = eval(PRA_nthChild(t, i + 1));
			switch(PRA_getString(PRA_nthChild(t, i))[0]){
				case '*':
					val *= cur;
					break;
				case '/':
					val /= cur;
					break;
				case '%':
					val = fmod(val, cur);
					break;
				default://something went wrong; multiplicative means the operators should all be *, / or %
					return NAN;
			}
		}
		return val;
	}
}

