//ptree_private.h
#pragma once
#include "ptree.h"

char appendPtree(Ptree *parent, Ptree node);

Ptree* lastChild(Ptree *t);

char reallocPtree(Ptree *t, int size);

void updateGrandchildPointers(Ptree *t);

void deleteChildrenAfter(Ptree *t, int n);

