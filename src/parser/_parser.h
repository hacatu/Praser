//_parser.h
#pragma once
#include "parser.h"
#include "../ptree/ptree.h"

struct Position{
	const char* string;
	const char* current;
};

int acceptChar(Position *p, char c);

