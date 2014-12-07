//__ptree.h
/* DO not INCLUDE THIS FILE!  This header is for PRA_Ptree related parts of the library only.
 */
#pragma once
#include "_ptree.h"

//Types:    ==========================================================
struct PRA_Ptree{//an n-ary string tagged tree to store parse data for postprocessing
	struct PRA_Ptree *parent;//the parent node.
	int nodec;//the number of nodes.
	struct PRA_Ptree *nodes;//the list of nodes
	int length;//the length of the string (only needed when it is a non null terminated substring of the whole string being parsed)
	char *string;//the string tag
};

