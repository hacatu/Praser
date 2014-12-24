//ptree.h
/* #include THIS FILE and parser_llk.h, never _ptree.h or __ptree.h.  These are for the library.
 */
#pragma once

typedef struct PRA_Ptree PRA_Ptree;

typedef enum{
	PRA_ADD,
	PRA_PASS,
	PRA_SKIP
} PRA_AppendMode;


/* Logs errors caused by malloc, realloc, etc failing.
 */
void PRA_logMemoryError(const char *name);


/* Sets the string of a PRA_Ptree
 * */
void PRA_setString(PRA_Ptree *p, const char *string, int length);

/* Appends a string to a PRA_Ptree
 */
void PRA_appendString(PRA_Ptree *p, const char *string, int length);


/* Returns 1 if PRA_Ptree *t has no children (represents a terminal symbol).
 */
char PRA_isTerminal(const PRA_Ptree *t);

const char* PRA_getString(PRA_Ptree *t);

int PRA_getSize(const PRA_Ptree *t);

int PRA_getLength(const PRA_Ptree *t);


/* Replaces all PRA_Ptrees with only one child by their child.
 */
void PRA_flatten(PRA_Ptree *t);

/* Like PRA_flatten, but leaves PRA_Ptrees with terminal children
 */
void PRA_flattenTagged(PRA_Ptree *t);

/* Replaces a ptree with its child if it only has one.
 */
void PRA_flattenSelf(PRA_Ptree *t);

PRA_Ptree* PRA_parent(PRA_Ptree *t);

/* Returns the last child of a PRA_Ptree
 */
PRA_Ptree* PRA_lastChild(PRA_Ptree *t);

/* Returns the nth Child of a PRA_Ptree *t.
 * Negative wraparound indicies may be used.
 * Returns a null pointer if n is out of bounds.
 */
PRA_Ptree* PRA_nthChild(const PRA_Ptree *t, int n);

/* Returns the first node of a PRA_Ptree in postorder.
 */
PRA_Ptree* PRA_firstPostorder(PRA_Ptree *root);

/* Returns the next node from a current node in postorder, or null
 */
PRA_Ptree* PRA_nextPostorder(PRA_Ptree *current);

/* Creates a copy of the given PRA_Ptree.
 */
PRA_Ptree* PRA_copyPtree(const PRA_Ptree *t);

/* Deletes all nodes in a PRA_Ptree except the root, regardless of what node is actually given.
 * If the root was malloc'd, it still has to be free'd elsewhere. 
 */
void PRA_deletePtree(PRA_Ptree *t);

/* Prints a PRA_Ptree *t.  Indent is the amount of spaces to indent each line.
 */
void PRA_printPtree(const PRA_Ptree *t, int indent);
