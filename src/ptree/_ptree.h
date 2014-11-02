//_ptree.h
/* DO NOT INCLUDE THIS FILE!  This header is for use by parser_llk.c and other non-ptree parts of the library.
 */
#pragma once
#include "ptree.h"

/* Creates a new Ptree with a given string and appends it to a given parent.
 */
char appendNewPtree(Ptree *parent, const char *string, int length);


/* Resizes a Ptree.  Currently requires deleteChildrenAfter to be used if shrinking the Ptree
 * and updateGrandchildPointer to be used after if t->nodes moves.
 * TODO: call these functions internally
 */
char reallocPtree(Ptree *t, int size);
