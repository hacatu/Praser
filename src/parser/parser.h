//parser.h
#pragma once

#include <stdio.h>
#include <ptree.h>

typedef struct PRA_Position PRA_Position;//Holds the string or file being parsed and column, line, position, and user data.
typedef int (*PRA_parser)(PRA_Position*, PRA_Ptree*);//a function pointer to a parsing function


/* Logs errors caused by the string being parsed having improper formatting.
 */
void PRA_logUnexpectedError(PRA_Position *p, const char *name, const char *expected);


/* Returns the pointer to the user-defined state of a position.
 */
void *PRA_getState(PRA_Position *p);

/* Allocates and returns a pointer to user state of a given size in a position.
 */
void *PRA_allocState(PRA_Position *p, size_t s);

/* Delete a position (frees the user state).
 * Always call this before freeing a position.
 */
void PRA_deletePosition(PRA_Position *p);

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
 * Do not try to use this except for the end of input, thus do not make the starting parser recursive.
 * Returns 0 on failure, nonzero otherwise.
 */
char PRA_acceptEnd(PRA_Position *p);


/* A parser that PRA_accepts a string and PRA_ADDs it to the parse tree if found.
 * Returns 0 on failure, nonzero otherwise.
 */
int PRA_acceptString(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *s);


/* Trys a parser and resets PRA_Position p if the parser fails.
 * This is vital if a parser may consume input but fail, but makes the parser not ll[k].
 * With this function, context sensitive parsers are possible.
 * Returns 0 on failure, nonzero otherwise.
 */
int PRA_try(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse);


/* Runs a parser.
 * Append mode is one of PRA_SKIP, PRA_PASS, or PRA_ADD, which skip the text parsed,
 * append it to the current PRA_Ptree *t, or add it as a child node respectively.
 * Returns 0 on failure, nonzero otherwise
 */
int PRA_accept(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse);

/* Returns the current line number.  This starts at 1 and is always 1 for a string.
 */
size_t PRA_currentLine(PRA_Position *p);

/* Return the current column number.  This starts at 1.
 */
size_t PRA_currentColumn(PRA_Position *p);

/* Parse a file.
 */
PRA_Ptree *PRA_parseFile(FILE *file, PRA_parser parse);

/* Parse a string.
 */
PRA_Ptree *PRA_parseString(char *string, PRA_parser parse);


/* Run a parser from min to max times, inclusive.
 * If max is <= 0, there is no upper bound.
 * Returns 0 on failure, 1 otherwise.
 */
int PRA_repeat(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse, int min, int max);

/* Takes two parsers, parse and parseSeperetor, and looks for them to alternate with parse matching on the outside.
 * Example: parse accepts numbers and parseSeparator accepts commas.  PRA_sepBy will accept "123,456,62", "0", or "1,2",
 * but not "123,456,", ",234", or ",0,".
 */
int PRA_sepBy(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_AppendMode aSeparator, PRA_parser parse, PRA_parser parseSeparator, int min, int max);

/* Takes two parsers, A and B, and looks for ABABABAB any number of times.
 */
int PRA_alternate(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode aA, PRA_AppendMode aB, PRA_parser parseA, PRA_parser parseB);

/* Consumes a single character if a given parser does not match.
 * Good for ignoring comments or whitespace or strings.
 */
int PRA_not(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, PRA_parser parse);

/* Accepts any char in the string options given.
 */
int PRA_oneOf(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *options);

/* Accepts any char not in the string options given.
 */
int PRA_noneOf(PRA_Position *p, PRA_Ptree *t, PRA_AppendMode a, const char *options);

/* Accepts and discards whitespace.
 */
int PRA_spaces(PRA_Position *p, PRA_Ptree *t);

/* Accepts and discards newlines.
 */
int PRA_newline(PRA_Position *p, PRA_Ptree *t);

/* Accepts a string literal.  Escape sequences (backslashes) are not supported.
 */
int PRA_cstring(PRA_Position *p, PRA_Ptree *t);

/* Accepts any letter of the English alphabet (A-Z or a-z).
 */
int PRA_letter(PRA_Position *p, PRA_Ptree *t);

/* Accepts any decimal digit (0-9).
 */
int PRA_digit(PRA_Position *p, PRA_Ptree *t);

/* Accepts any alphanumeric (A-Z, a-z, or 0-9).
 */
int PRA_alphanum(PRA_Position *p, PRA_Ptree *t);

/* Accepts an integer, which is a sequence of digits.
 */
int PRA_integer(PRA_Position *p, PRA_Ptree *t);
