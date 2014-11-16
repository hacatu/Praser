//parser_combinators.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ptree/_ptree.h"
#include "_parser.h"
#include "../util/debug.h"

int repeat(Position *p, Ptree *t, AppendMode a, parser parse, int min, int max){
	//t->string = __func__;
	int count = 0;
	while(count < min){
		if(!accept(p, t, a, parse)){
			debug_log("not enough matches");
			return 0;
		}
		++count;
	}
	if(max > 0){
		while(count < max){
			if(!accept(p, t, a, parse)){
				break;
			}
			++count;
		}
	}else{
		while(1){
			if(!accept(p, t, a, parse)){
				break;
			}
			++count;
		}
	}
	debug_log("successful");
	return 1;
}

int sepBy(Position *p, Ptree *t, AppendMode a, AppendMode aSeperator, parser parse, parser parseSeperator, int min, int max){
	int count = 1;
	//t->string = __func__;
	if(!accept(p, t, a, parse)){
		return min <= 0;
	}
	while(count < min){
		if(!accept(p, t, aSeperator, parseSeperator)){
			return 0;
		}
		if(!accept(p, t, a, parse)){
			return 0;
		}
		++count;
	}
	if(max > min){
		while(count < max){
			if(!accept(p, t, aSeperator, parseSeperator)){
				return 1;
			}
			if(!accept(p, t, a, parse)){
				return 0;
			}
			++count;
		}
	}if(max < min || max == 0){
		while(accept(p, t, aSeperator, parseSeperator)){
			if(!accept(p, t, a, parse)){
				return 0;
			}
		}
		return 1;
	}
	return 1;
}

int alternate(Position *p, Ptree *t, AppendMode aA, AppendMode aB, parser parseA, parser parseB){
	
	//t->string = __func__;
	while(accept(p, t, aA, parseA)){
		if(!accept(p, t, aB, parseB)){
			return 0;
		}
	}
	return 1;
}

int not(Position *p, Ptree *t, AppendMode a, parser parse){
	Position start = *p;
	Ptree* temp = mallocPtree();
	if(!temp){
		logMemoryError(__func__);
		return 0;
	}
	if(parse(&start, temp)){
		deletePtree(temp);
		free(temp);
		return 0;
	}
	deletePtree(temp);
	free(temp);
	switch(a){
		case ADD:
			return appendNewPtree(t, (const char[]){getChar(p), '\0'}, 1);
		case PASS:
			appendString(t, (const char[]){getChar(p), '\0'}, 1);
			return 1;
		case SKIP:
			getChar(p);
			return 1;
	}
	//needed to supress erronious warning from gcc.  The above switch can't fall through bcause a is an enum.
	return 1;
}

int oneOf(Position *p, Ptree *t, AppendMode a, const char *options){
	const char *c = options;
	while(*c){
		if(acceptChar(p, *c)){
			break;
		}
		++c;
	}
	if(!*c){
		return 0;
	}
	switch(a){
		case ADD:
			return appendNewPtree(t, c, 1);
		case PASS:
			appendString(t, c, 1);
			return 1;
		case SKIP:
			return 1;
	}
	//needed to supress erronious warning from gcc.  The above switch can't fall through bcause a is an enum.
	return 1;
}

int noneOf(Position *p, Ptree *t, AppendMode a, const char *options){
	const char *c = options;
	while(*c){
		if(currentChar(p) == *c){
			return 0;
		}
		++c;
	}
	c = p->current;
	getChar(p);
	switch(a){
		case ADD:
			return appendNewPtree(t, c, 1);
		case PASS:
			appendString(t, c, 1);
			return 1;
		case SKIP:
			return 1;
	}
	//needed to supress erronious warning from gcc.  The above switch can't fall through bcause a is an enum.
	return 1;
}

