//ptree_private.h
#pragma once
#include "ptree.h"

/* Creates a new Ptree with a given string and appends it to a given parent.
 */
char appendNewPtree(Ptree *parent, const char *string, int length);

/* Appends a Ptree to a parent Ptree.
 */
char appendPtree(Ptree *parent, Ptree node);

/* Gets the number of children a Ptree has (t->nodec).
 */
int size(Ptree *t);

/* Resizes a Ptree.  Currently requires deleteChildrenAfter to be used if shrinking the Ptree
 * and updateGrandchildPointer to be used after if t->nodes moves.
 * TODO: call these functions internally
 */
char reallocPtree(Ptree *t, int size);

/* Uptates the grandchild pointers of a Ptree so that their parent pointers point to the new,
 * realloc'd location of t->nodes+n, where n is the index of their parent.
 */
//void updateGrandchildPointers(Ptree *t);

/* Deletes a Ptree's children after an index.
 */
//void deleteChildrenAfter(Ptree *t, int n);
