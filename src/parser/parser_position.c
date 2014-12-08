//parser_position.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ptree/_ptree.h"
#include "_parser.h"
#include "../util/debug.h"


struct PRA_Position{
	enum{PRA_FILE_POS, STRING_POS} type;
	union{
		struct{
			const char* string;
			size_t index;
		};
		FILE *file;
	};
};

void PRA_logUnexpectedError(PRA_Position *p, const char *name, const char *expected){
	//TODO: count lines and include the line/column number for errors, and print the line with the error highlighted.
	const char found[] = {PRA_currentChar(p), '\0'};
	printf("%s: expected %s but found \"%s\" at %li: \"%10.s\"\n", name, expected, found, p->index, p->string + p->index);
}


PRA_Position *PRA_firstPosition(const char *string){
	PRA_Position *start = malloc(1*sizeof(PRA_Position));
	*start = (PRA_Position){.type = STRING_POS, .string = string, .index = 0};
	return start;
}

PRA_Position *PRA_startPosition(FILE *file){
	PRA_Position *start = malloc(1*sizeof(PRA_Position));
	*start = (PRA_Position){.type = PRA_FILE_POS, .file = file};
	fseek(start->file, 0, SEEK_SET);
	return start;
}

size_t currentIndex(PRA_Position *p){
	switch(p->type){
		case STRING_POS:
		return p->index;
		case PRA_FILE_POS:
		return ftell(p->file);
	}
	//not reachable:
	return 0;
}

void resetIndex(PRA_Position *p, size_t index){
	switch(p->type){
		case STRING_POS:
		p->index = index;
		break;
		case PRA_FILE_POS:
		fseek(p->file, index, SEEK_SET);
		break;
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
		case PRA_FILE_POS:
		if(!fread(&temp, sizeof(char), 1, p->file)){
			return 0;
		}
		return temp;
	}
	//not reachable:
	return 0;
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
		case PRA_FILE_POS:
		start = ftell(p->file);
		fseek(p->file, n*sizeof(char), SEEK_CUR);
		if(!fread(&temp, sizeof(char), 1, p->file)){
			return 0;
		}
		fseek(p->file, start, SEEK_SET);
		return temp;
	}
	//not reachable
	return 0;
}

char PRA_acceptEnd(PRA_Position *p){
	size_t start;
	char temp;
	switch(p->type){
		case STRING_POS:
		return acceptChar(p, '\0');
		case PRA_FILE_POS:
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

