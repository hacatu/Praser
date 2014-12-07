//_parser.h
#pragma once
#include "parser.h"
#include "../ptree/ptree.h"

size_t currentIndex(PRA_Position *p);

void resetIndex(PRA_Position *p, size_t index);

int acceptChar(PRA_Position *p, char c);

