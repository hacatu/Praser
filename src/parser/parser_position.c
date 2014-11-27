//parser_position.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ptree/_ptree.h"
#include "_parser.h"
#include "../util/debug.h"


struct Position{
	enum{FILE_POS, STRING_POS} type;
	union{
		struct{
			const char* string;
			size_t index;
		};
		FILE *file;
	};
};

void logUnexpectedError(Position *p, const char *name, const char *expected){
	//TODO: count lines and include the line/column number for errors, and print the line with the error highlighted.
	const char found[] = {currentChar(p), '\0'};
	printf("%s: expected %s but found \"%s\" at %li: \"%10.s\"\n", name, expected, found, p->index, p->string + p->index);
}


Position *firstPosition(const char *string){
	Position *start = malloc(1*sizeof(Position));
	*start = (Position){.type = STRING_POS, .string = string, .index = 0};
	return start;
}

Position *startPosition(FILE *file){
	Position *start = malloc(1*sizeof(Position));
	*start = (Position){.type = FILE_POS, .file = file};
	fseek(start->file, 0, SEEK_SET);
	return start;
}

size_t currentIndex(Position *p){
	switch(p->type){
		case STRING_POS:
		return p->index;
		case FILE_POS:
		return ftell(p->file);
	}
	//Not reachable:
	return 0;
}

void resetIndex(Position *p, size_t index){
	switch(p->type){
		case STRING_POS:
		p->index = index;
		break;
		case FILE_POS:
		fseek(p->file, index, SEEK_SET);
		break;
	}
}

char currentChar(Position *p){
	return nthChar(p, 0);
}

char getChar(Position *p){
	char temp;
	switch(p->type){
		case STRING_POS:
		return p->string[++p->index];
		case FILE_POS:
		if(!fread(&temp, sizeof(char), 1, p->file)){
			return 0;
		}
		return temp;
	}
	//Not reachable:
	return 0;
}

char nextChar(Position *p){
	return nthChar(p, 1);
}

char nthChar(Position *p, int n){
	size_t start;
	char temp;
	switch(p->type){
		case STRING_POS:
		return p->string[p->index + n];
		case FILE_POS:
		start = ftell(p->file);
		fseek(p->file, n*sizeof(char), SEEK_CUR);
		if(!fread(&temp, sizeof(char), 1, p->file)){
			return 0;
		}
		fseek(p->file, start, SEEK_SET);
		return temp;
	}
	//Not reachable
	return 0;
}

char acceptEnd(Position *p){
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
	//Not reachable
	return 0;
}

