//_parser.h
#pragma once
#include "parser.h"
#include "../ptree/ptree.h"

size_t currentIndex(Position *p);

void resetIndex(Position *p, size_t index);

int acceptChar(Position *p, char c);

