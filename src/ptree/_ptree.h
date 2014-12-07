//_ptree.h
/* DO not INCLUDE THIS FILE!  This header is for use by parser_llk.c and other non-PRA_Ptree parts of the library.
 */
#pragma once
#include "ptree.h"

/* Creates a new PRA_Ptree with a given string and appends it to a given parent.
 */
char appendNewPtree(PRA_Ptree *parent, const char *string, int length);


/* Resizes a PRA_Ptree.  Currently requires deleteChildrenAfter to be used if shrinking the PRA_Ptree
 * and updateGrandchildPointer to be used after if t->nodes moves.
 * TODO: call these functions internally
 */
char reallocPtree(PRA_Ptree *t, int size);

