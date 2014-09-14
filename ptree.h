//ptree.h
#pragma once

typedef struct Ptree Ptree;

typedef enum{
	ADD,
	PASS,//Note: PASS is supposed to allow Ptrees to be flattened,
	//but this way of doing that is pointless, so I am replacing it with flatten().
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


/* Removes all Ptrees with only one child by their child.
 */
void flatten(Ptree *t);


/* Returns the last child of a ptree
 */
Ptree* lastChild(Ptree *t);

/* Returns the nth Child of a Ptree *t.
 * Negative wraparound indicies may be used.
 * Returns a null pointer if n is out of bounds.
 */
Ptree* nthChild(Ptree *t, int n);


/* Deletes all nodes in a Ptree except the root, regardless of what node is actually given.
 * If the root was malloc'd, it still has to be free'd elsewhere. 
 */
void deletePtree(Ptree *t);

/* Prints a Ptree *t.  Indent is the amount of spaces to indent each line.
 */
void printPtree(const Ptree *t, int indent);
