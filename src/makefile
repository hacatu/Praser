CC = gcc
MAKE = make
EXE = parser_repl
LIB = parser.a
_LOBJS = parser/parser_position.o parser/parser_accept.o parser/parser_combinators.o parser/parser_parsers.o ptree/ptree_create.o ptree/ptree_util.o ptree/ptree_delete.o ptree/ptree_print.o
_PARSERS = calculator.parser tree.parser lisp.parser
CFLAGS = -std=c11 -lm -Wall -Werror
DBGFLAGS = DEBUG_LOG
.DEFALT_GOAL = all

LOBJS = $(patsubst %,obj/%,$(_LOBJS))
PARSERS = $(patsubst %,bin/%,$(_PARSERS))

debug: CFLAGS += $(addprefix -D, $(DBGFLAGS))
debug: test
test: CFLAGS += -g
test: all
all: bin lib obj UTIL lib/$(LIB) bin/$(EXE) $(PARSERS)

bin:
	mkdir bin

lib:
	mkdir lib

obj:
	mkdir obj

UTIL:
	$(MAKE) -C util

lib/$(LIB): $(LOBJS)
	ar rcs $2 $^

bin/$(EXE): obj/
