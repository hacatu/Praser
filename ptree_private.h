//ptree_private.h
#pragma once
#include "ptree.h"

/* Creates a new Ptree with a given string and appends it to a given parent.
 */
char appendNewPtree(Ptree *parent, const char *string, int length);

/* Gets the number of children a Ptree has (t->nodec).
 */
int getSize(Ptree *t);

/* Resizes a Ptree.  Currently requires deleteChildrenAfter to be used if shrinking the Ptree
 * and updateGrandchildPointer to be used after if t->nodes moves.
 * TODO: call these functions internally
 */
char reallocPtree(Ptree *t, int size);

