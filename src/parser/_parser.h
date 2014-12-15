//_parser.h
#pragma once
#include "parser.h"
#include "../ptree/ptree.h"

size_t currentIndex(PRA_Position *p);

void resetIndex(PRA_Position *p, size_t index);

int acceptChar(PRA_Position *p, char c);

/* Returns a pointer to the initial position in a string.
 * If the string is deleted, this position is invalid.
 * This pointer must be free'd.
 */
PRA_Position *PRA_firstPosition(const char *string);

/* Returns a pointer to the initial position in a file.
 * If the file is closed, this position is invalid.
 * This pointer must be free'd.
 */
PRA_Position *PRA_startPosition(FILE *file);

