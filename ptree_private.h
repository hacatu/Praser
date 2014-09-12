//ptree_private.h
#pragma once
#include "ptree.h"

char appendNewPtree(Ptree *parent, const char *string, int length);

char appendPtree(Ptree *parent, Ptree node);

int size(Ptree *t);

char reallocPtree(Ptree *t, int size);

void updateGrandchildPointers(Ptree *t);

void deleteChildrenAfter(Ptree *t, int n);
