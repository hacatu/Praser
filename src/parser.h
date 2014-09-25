//parser_llk.h
#pragma once
#include "ptree.h"

typedef struct Position Position;//holds the string being parsed and a pointer to the current char
typedef int (*parser)(Position*, Ptree*);//a function pointer to a parsing function


/* Logs errors caused by the string being parsed having improper formatting.
 */
void logUnexpectedError(Position *p, const char *name, const char *expected);


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
 * Do not try to use this except for the end of input, thus do not make the starting parser recursive.
 * Returns 0 on failure, nonzero otherwise.
 */
char acceptEnd(Position *p);


/* A parser that accepts a string and adds it to the parse tree if found.
 * Returns 0 on failure, nonzero otherwise.
 */
int acceptString(Position *p, Ptree *t, AppendMode a, const char *s);


/* Trys a parser and resets Position p if the parser fails.
 * This is vital if a parser may consume input but fail, but makes the parser not ll[k]
 * Returns 0 on failure, nonzero otherwise.
 */
int try(Position *p, Ptree *t, AppendMode a, parser parse);


/* Runs a parser.
 * Returns 0 on failure, nonzero otherwise
 */
int accept(Position *p, Ptree *t, AppendMode a, parser parse);


/* Run a parser from min to max times, inclusive.
 * If max is <= 0, there is no upper bound.
 * Returns 0 on failure, 1 otherwise.
 */
int repeat(Position *p, Ptree *t, AppendMode a, parser parse, int min, int max);

/* Takes two parsers, parse and parseSeperetor, and looks for them to alternate with parse matching on the outside.
 * Example: parse accepts numbers and parseSeperator accepts commas.  sepBy will accept "123,456,62", "0", or "1,2",
 * but not "123,456,", ",234", or ",0,".
 */
int sepBy(Position *p, Ptree *t, AppendMode a, AppendMode aSeperator, parser parse, parser parseSeperator, int min, int max);

/* Takes two parsers, A and B, and looks for ABABABAB any number of times.
 */
int alternate(Position *p, Ptree *t, AppendMode aA, AppendMode aB, parser parseA, parser parseB);

/* Consumes a single character if a given parser does not match.
 * Good for ignoring comments or whitespace or strings.
 */
int not(Position *p, Ptree *t, AppendMode a, parser parse);

/* Accepts any char in the string options given.
 */
int oneOf(Position *p, Ptree *t, AppendMode a, const char *options);

/* Accepts any char not in the string options given.
 */
int noneOf(Position *p, Ptree *t, AppendMode a, const char *options);

/* Accepts a string literal.
 */
int cstring(Position *p, Ptree *t);

int cchar(Position *p, Ptree *t);

/* Accepts and discards whitespace.
 */
int spaces(Position *p, Ptree *t);

int letter(Position *p, Ptree *t);

int digit(Position *p, Ptree *t);

int integer(Position *p, Ptree *t);
