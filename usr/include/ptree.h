//ptree.h
/* #include THIS FILE and parser_llk.h, never _ptree.h or __ptree.h.  These are for the library.
 */
#pragma once

typedef struct Ptree Ptree;

typedef enum{
	ADD,
	PASS,
	SKIP
} AppendMode;


/* Logs errors caused by malloc, realloc, etc failing.
 */
void logMemoryError(const char *name);


/* Create a root ptree
 */
Ptree* mallocPtree();


/* Sets the string of a ptree
 * */
void setString(Ptree *p, const char *string, int length);

/* Appends a string to a ptree
 */
void appendString(Ptree *p, const char *string, int length);


/* Returns 1 if Ptree *t has no children (represents a terminal symbol).
 */
char isTerminal(const Ptree *t);

const char* getString(Ptree *t);

int getSize(const Ptree *t);

int getLength(const Ptree *t);


/* Replaces all Ptrees with only one child by their child.
 */
void flatten(Ptree *t);

/* Like flatten, but leaves Ptrees with terminal children
 */
void flattenTagged(Ptree *t);


Ptree* parent(Ptree *t);

/* Returns the last child of a ptree
 */
Ptree* lastChild(Ptree *t);

/* Returns the nth Child of a Ptree *t.
 * Negative wraparound indicies may be used.
 * Returns a null pointer if n is out of bounds.
 */
Ptree* nthChild(const Ptree *t, int n);

/* Returns the first node of a ptree in postorder.
 */
Ptree* firstPostorder(Ptree *root);

/* Returns the next node from a current node in postorder, or null
 */
Ptree* nextPostorder(Ptree *current);

/* Creates a copy of the given Ptree.
 */
Ptree* copyPtree(const Ptree *t);

/* Deletes all nodes in a Ptree except the root, regardless of what node is actually given.
 * If the root was malloc'd, it still has to be free'd elsewhere. 
 */
void deletePtree(Ptree *t);

/* Prints a Ptree *t.  Indent is the amount of spaces to indent each line.
 */
void printPtree(const Ptree *t, int indent);
