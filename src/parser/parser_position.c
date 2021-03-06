//parser_position.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ptree/_ptree.h"
#include "_parser.h"
#include "../util/debug.h"


typedef struct{
	size_t size;
	void *state;
} PRA_userState;

struct PRA_Position{
	enum{FILE_POS, STRING_POS} type;
	union{
		struct{
			const char* string;
		};
		struct{
			FILE *file;
			size_t line;
			size_t column;
		};
	};
	size_t index;
	PRA_userState state;
};

void PRA_logUnexpectedError(PRA_Position *p, const char *name, const char *expected){
	const char found[] = {PRA_currentChar(p), '\0'};
	printf("%s: expected %s but found \"%s\" at (%li:%li/%li): \"%10.s\"\n",
	       name,
	       expected,
	       found,
	       PRA_currentLine(p),
	       PRA_currentColumn(p),
	       p->index,
	       p->string + p->index
	);
}

PRA_Position *firstPosition(const char *string){
	PRA_Position *start = malloc(1*sizeof(PRA_Position));
	*start = (PRA_Position){.type = STRING_POS, .string = string, .index = 0};
	return start;
}

PRA_Position *startPosition(FILE *file){
	PRA_Position *start = malloc(1*sizeof(PRA_Position));
	*start = (PRA_Position){.type = FILE_POS, .file = file, .line = 1, .column = 1};
	fseek(start->file, 0, SEEK_SET);
	return start;
}

void *PRA_getState(PRA_Position *p){
	return p->state.state;
}

void *PRA_allocState(PRA_Position *p, size_t size){
	p->state.size = size;
	return p->state.state = malloc(size);
}

void PRA_deletePosition(PRA_Position *p){
	free(PRA_getState(p));
}

static void copyState(PRA_Position *src, PRA_Position *dst){
	memcpy(PRA_getState(dst), PRA_getState(src), src->state.size);
}

PRA_Position *copyPosition(PRA_Position *p){
	PRA_Position *t = malloc(1*sizeof(PRA_Position));
	if(!t){
		return NULL;
	}
	if(p->type == FILE_POS){
		p->index = ftell(p->file);
	}
	*t = *p;
	if(!PRA_allocState(t, p->state.size)){
		free(t);
		return NULL;
	}
	copyState(p, t);
	return t;
}

size_t PRA_currentLine(PRA_Position *p){
	switch(p->type){
		case STRING_POS:
			return 1;
		case FILE_POS:
			return p->line;
		default:
			return 0;
	}
}

size_t PRA_currentColumn(PRA_Position *p){
	switch(p->type){
		case STRING_POS:
			return p->index + 1;
		case FILE_POS:
			return p->column;
		default:
			return 0;
	}
}

void resetIndex(PRA_Position *p, PRA_Position *b){
	PRA_deletePosition(p);
	*p = *b;
	if(p->type == FILE_POS){
		fseek(p->file, b->index, SEEK_SET);
	}
}

char PRA_currentChar(PRA_Position *p){
	return PRA_nthChar(p, 0);
}

char PRA_getChar(PRA_Position *p){
	char temp;
	switch(p->type){
		case STRING_POS:
			return p->string[++p->index];
		case FILE_POS:
			if(!fread(&temp, sizeof(char), 1, p->file)){
				return '\0';
			}
			++p->column;
			if(temp == '\n'){
				++p->line;
				p->column = 1;
			}
			return temp;
		default:
			return '\0';
	}
}

char PRA_nextChar(PRA_Position *p){
	return PRA_nthChar(p, 1);
}

char PRA_nthChar(PRA_Position *p, int n){
	size_t start;
	char temp;
	switch(p->type){
		case STRING_POS:
		return p->string[p->index + n];
		case FILE_POS:
		start = ftell(p->file);
		if(fseek(p->file, n*sizeof(char), SEEK_CUR)){
			return '\0';
		}
		if(!fread(&temp, sizeof(char), 1, p->file)){
			return '\0';
		}
		fseek(p->file, start, SEEK_SET);
		return temp;
	}
	//not reachable
	return '\0';
}

char PRA_acceptEnd(PRA_Position *p){
	size_t start;
	char temp;
	switch(p->type){
		case STRING_POS:
		return acceptChar(p, '\0');
		case FILE_POS:
		start = ftell(p->file);
		if(!fread(&temp, sizeof(char), 1, p->file)){
			return 1;
		}
		fseek(p->file, start, SEEK_SET);
		return 0;
	}
	//not reachable
	return 0;
}

