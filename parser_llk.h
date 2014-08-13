//parser_llk.h
#pragma once

typedef struct Position Position;//holds the string being parsed and a pointer to the current char
typedef struct Ptree{//an n-ary string tagged tree to store parse data for postprocessing
	struct Ptree *parent;//the parent node.
	int nodec;//the number of nodes.
	struct Ptree *nodes;//the list of nodes
	int length;//the length of the string (only needed when it is a non null terminated substring of the whole string being parsed)
	const char *string;//the string tag
} Ptree;
typedef int (*parser)(Position*, Ptree*);//a function pointer to a parsing function


/* Logs errors caused by the string being parsed having improper formatting.
 */
void logUnexpectedError(Position *p, const char *name, const char *expected);

/* Logs errors caused by malloc, realloc, etc failing.
 */
void logMemoryError(const char *name);


/* Returns a Ptree with a given parent and tag, string.
 * This is how you create a Ptree.  Learn it use it LOVE it.
 * This Ptree should be assigned to allocated memory in another Ptree's nodes field.
 * If I have a parser `parenGrp` that uses the parsers `openParen`, `inner`, and `closeParen`,
 * I would make parenGrp appendPtree a newPtree to the Ptree *t I gave it and then
 * give a pointer to this appended Ptree to openParen, and repeat for my other functions.
 * eg:
 * int parenGrp(Position *p, Ptree *t){
 *     t->string = "parenGrp";
 *     Ptree temp = newPtree(t, NULL);
 *     if(!appendPtree(t, temp)){
 *         logMemoryError("parenGrp");
 *         return 0;
 *     }
 *     if(!accept(p, lastChild(t), openParen)){
 *         return 0;
 *     }
 *     temp = newPtree(t, NULL);
 *     if(!appendPtree(t, temp)){
 *         logMemoryError("parenGrp");
 *         return 0;
 *     }
 *     if(!expect(p, lastChild(t), inner)){
 *         logUnexpectedError(p, "parenGrp", "inner");
 *     }
 *     temp = newPtree(t, NULL);
 *     if(!appendPtree(t, temp)){
 *         logMemoryError("parenGrp");
 *         return 0;
 *     }
 *     if(!expect(p, lastChild(t), closeParen)){
 *         logUnexpectedError(p, "parenGrp", "closeParen");
 *     }
 *     return 0;
 * }
 */
Ptree newPtree(Ptree *parent, const char *string);

/* Adds a Ptree node to the nodes list of a Ptree *parent.
 * This is how you build up a Ptree.  Learn it use it LOVE it.
 * Calls logMemoryError if realloc fails
 * Returns 1 on success, 0 on failure.
 * See newPtree for an example.
 */
char appendPtree(Ptree *parent, Ptree node);

/* Returns 1 if Ptree *t has no children (represents a terminal symbol).
 */
char isTerminal(const Ptree *t);

/* Returns the nth Child of a Ptree *t.
 * Negative wraparound indicies may e used.
 * Returns a null pointer if n is out of bounds.
 */
Ptree* nthChild(Ptree *t, int n);

/* Returns the last Child of a Ptree *t.
 */
Ptree* lastChild(Ptree *t);

/* Deletes all nodes in a Ptree except the root, regardless of what node is actually given.
 * If the root was malloc'd, it still has to be free'd elsewhere. 
 */
void deletePtree(Ptree *t);

/* Prints a Ptree *t.  Indent is the amount of spaces to indent each line.
 */
void printPtree(const Ptree *t, int indent);


/* Returns a pointer to the initial position in a string.
 * If the string is deleted, this position is invalid.
 * This pointer must be free'd.
 */
Position* firstPosition(const char *string);


/* Returns the current character in a Position.
 */
char currentChar(Position *p);

/* Increments a Position's position and returns the new character.
 */
char getChar(Position *p);

/* Returns the next character without incrementing the position.
 * This makes the parser ll[1].
 */
char nextChar(Position *p);

/* Returns the nth character in a Position.
 * nthChar(p, 0) == currentChar(p)
 * nthChar(p, 1) == nextChar(p)
 * This makes the parser ll[k].
 */
char nthChar(Position *p, int n);


/* A parser that expects the end of input ('\0').
 * Calls logUnexpectedError if the input does not end.
 * Do not try to use this except for the end of input, thus do not make the starting parser recursive.
 * Returns 0 on failure, nonzero otherwise.
 */
char expectEnd(Position *p);


/* A parser that accepts a string and adds it to the parse tree if found.
 * Returns 0 on failure, nonzero otherwise.
 */
int acceptString(Position *p, Ptree *t, const char *s);

/* A parser that expects a string, adding it to the parse tree if found, otherwise calling logUnexpectedError and returning 0.
 */
int expectString(Position *p, Ptree *t, const char *s);


/* Trys a parser and resets Position p if the parser fails.
 * This is vital if a parser may consume input but fail, but makes the parser not ll[k]
 * Returns 0 on failure, nonzero otherwise.
 */
int try(Position *p, Ptree *t, parser parse);


/* Runs a parser.
 * Returns 0 on failure, nonzero otherwise
 */
int accept(Position *p, Ptree *t, parser parse);

/* Runs a parser and calls logUnexpectedError if it fails.
 * Returns 0 on failure, nonzero otherwise.
 */
int expect(Position *p, Ptree *t, parser parse);


/* Run a parser from min to max times, inclusive.
 * If max is <= 0, there is no upper bound.
 * Returns 0 on failure, 1 otherwise.
 */
int repeatMinMax(Position *p, Ptree *t, parser parse, int min, int max);

/* Takes two parsers, parse and parseSeperetor, and looks for them to alternate with parse matching on the outside.
 * Example: parse accepts numbers and parseSeperator accepts commas.  sepBy will accept "123,456,62", "0", or "1,2",
 * but not "123,456", ",234", or ",0,".
 */
int sepBy(Position *p, Ptree *t, parser parse, parser parseSeperator);

/* Takes two parsers, A and B, and looks for ABABABAB any number of times.
 */
int alternate(Position *p, Ptree *t, parser parseA, parser parseB);

/* Consumes input until a given parser matches.
 * Good for ignoring comments or whitespace or strings.
 */
int takeWhileNot(Position *p, Ptree *t, parser parse);

/* Accepts any char in the string options given.
 */
int oneOf(Position *p, Ptree *t, const char *options);
