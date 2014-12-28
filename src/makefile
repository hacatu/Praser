PARSER_SRC = $(wildcard parser/*.c)
PTREE_SRC = $(wildcard ptree/*.c)
UTIL_SRC = $(wildcard util/*.c)
REPL_SRC = $(wildcard *.c)
PARSER_LIB = $(BASE)/$(LIB)/libparser.a
PARSER_UTIL_LIB = $(BASE)/$(LIB)/libparser_util.a
PARSER_EXE = $(BASE)/$(BIN)/parser_repl

PARSER_OBJ = $(PARSER_SRC:.c=.o)
PTREE_OBJ = $(PTREE_SRC:.c=.o)
UTIL_OBJ = $(UTIL_SRC:.c=.o)
REPL_OBJ = $(REPL_SRC:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(PARSER_LIB): $(PARSER_OBJ) $(PTREE_OBJ)
	$(AR) $@ $^

$(PARSER_UTIL_LIB): $(UTIL_OBJ)
	$(AR) $@ $^

$(PARSER_EXE): $(PARSER_LIB) $(PARSER_UTIL_LIB) $(REPL_OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)
