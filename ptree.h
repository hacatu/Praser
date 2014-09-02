//ptree.h
#pragma once

typedef struct Ptree{//an n-ary string tagged tree to store parse data for postprocessing
	struct Ptree *parent;//the parent node.
	int nodec;//the number of nodes.
	struct Ptree *nodes;//the list of nodes
	int length;//the length of the string (only needed when it is a non null terminated substring of the whole string being parsed)
	const char *string;//the string tag
} Ptree;

typedef enum{
	ADD,
	PASS,
	SKIP
} AppendMode;

/* Logs errors caused by malloc, realloc, etc failing.
 */
void logMemoryError(const char *name);


/* Returns a Ptree with a given parent and tag, string.
 * This is how you create a Ptree.  Generally this is handled automatically
 * by accept(), so it should only be called explicitly to create the root.
 * This Ptree is assigned to allocated memory in another Ptree's nodes field.
 * If I have a parser `parenGrp` that uses the parsers `openParen`, `inner`, and `closeParen`,
 * I would make parenGrp set t->string = "parenGrp" and then accept/ expect the other parsers.
 * accept() will automatically create Ptrees and pass them to the parsers you call with it.
 * eg:
 * int parenGrp(Position *p, Ptree *t){
 *     t->string = "parenGrp";
 *     if(!accept(p, lastChild(t), openParen)){
 *         return 0;
 *     }if(!expect(p, lastChild(t), inner)){
 *         logUnexpectedError(p, "parenGrp", "inner");
 *     }
 *     if(!expect(p, lastChild(t), closeParen)){
 *         logUnexpectedError(p, "parenGrp", "closeParen");
 *     }
 *     return 0;
 * }
 */
Ptree newPtree(Ptree *parent, const char *string);

/* Returns 1 if Ptree *t has no children (represents a terminal symbol).
 */
char isTerminal(const Ptree *t);

/* Returns the nth Child of a Ptree *t.
 * Negative wraparound indicies may e used.
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
