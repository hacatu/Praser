//parser_llk.h
#pragma once
#include <ptree.h>

typedef struct PRA_Position PRA_Position;//holds the string being parsed and a pointer to the current char
typedef int (*PRA_parser)(PRA_Position*, PRA_Ptree*);//a function pointer to a parsing function


/* Logs errors caused by the string being parsed having improper formatting.
 */
void PRA_logUnexpectedError(PRA_Position *p, const char *name, const char *expected);


/* Returns a pointer to the initial position in a string.
 * If the string is deleted, this position is invalid.
 * This pointer must be free'd.
 */
PRA_Position *PRA_firstPosition(const char *string);

/* Returns a pointer to the initial position in a file.
 * If the file is closed, this position is invalid.
 * This pointer must be free'd.
 */
PRA_Position *PRA_startPosition(FILE *file);

/* Returns the current character in a PRA_Position.
 */
char PRA_currentChar(PRA_Position *p);

/* Increments a PRA_Position's position and returns the new character.
 */
char PRA_getChar(PRA_Position *p);

/* Returns the next character without incrementing the position.
 * This makes the parser ll[1].
 */
char PRA_nextChar(PRA_Position *p);

/* Returns the nth character in a PRA_Position.
 * PRA_nthChar(p, 0) == PRA_currentChar(p)
 * PRA_nthChar(p, 1) == PRA_nextChar(p)
 * This makes the parser ll[k].
 */
char PRA_nthChar(PRA_Position *p, int n);


/* A parser that expects the end of input ('\0').
 * Do PRA_not PRA_try to use this except for the end of input, thus do PRA_not make the starting parser recursive.
 * Returns 0 on failure, nonzero otherwise.
 */
char PRA_acceptEnd(PRA_Position *p);


/* A parser that PRA_accepts a string and PRA_ADDs it to the parse tree if found.
 * Returns 0 on failure, nonzero otherwise.
 */
int PRA_acceptString(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *s);


/* PRA_trys a parser and resets PRA_Position p if the parser fails.
 * This is vital if a parser may consume input but fail, but makes the parser PRA_not ll[k]
 * Returns 0 on failure, nonzero otherwise.
 */
int PRA_try(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse);


/* Runs a parser.
 * Returns 0 on failure, nonzero otherwise
 */
int PRA_accept(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse);


/* Run a parser from min to max times, inclusive.
 * If max is <= 0, there is no upper bound.
 * Returns 0 on failure, 1 otherwise.
 */
int PRA_repeat(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse, int min, int max);

/* Takes two parsers, parse and parseSeperetor, and looks for them to alternate with parse matching on the outside.
 * Example: parse accepts numbers and parseSeperator accepts commas.  PRA_sepBy will accept "123,456,62", "0", or "1,2",
 * but PRA_not "123,456,", ",234", or ",0,".
 */
int PRA_sepBy(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_AppendMode aSeperator, PRA_parser parse, PRA_parser parseSeperator, int min, int max);

/* Takes two parsers, A and B, and looks for ABABABAB any number of times.
 */
int PRA_alternate(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode aA, PRA_AppendMode aB, PRA_parser parseA, PRA_parser parseB);

/* Consumes a single character if a given parser does PRA_not match.
 * Good for ignoring comments or whitespace or strings.
 */
int PRA_not(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse);

/* PRA_accepts any char in the string options given.
 */
int PRA_oneOf(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *options);

/* PRA_accepts any char PRA_not in the string options given.
 */
int PRA_noneOf(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *options);

/* PRA_accepts and discards whitespace.
 */
int PRA_spaces(PRA_Position *p, PRA_Ptree *t);

int PRA_cstring(PRA_Position *p, PRA_Ptree *t);

int PRA_letter(PRA_Position *p, PRA_Ptree *t);

int PRA_digit(PRA_Position *p, PRA_Ptree *t);

int PRA_integer(PRA_Position *p, PRA_Ptree *t);
