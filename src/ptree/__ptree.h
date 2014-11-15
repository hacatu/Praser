//__ptree.h
/* DO NOT INCLUDE THIS FILE!  This header is for ptree related parts of the library only.
 */
#pragma once
#include "_ptree.h"

//Types:    ==========================================================
struct Ptree{//an n-ary string tagged tree to store parse data for postprocessing
	struct Ptree *parent;//the parent node.
	int nodec;//the number of nodes.
	struct Ptree *nodes;//the list of nodes
	int length;//the length of the string (only needed when it is a non null terminated substring of the whole string being parsed)
	char *string;//the string tag
};

