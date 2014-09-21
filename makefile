CC = gcc
EXE = parser_repl
LIB = parser.a
_LOBJS = parser_llk.o ptree_create.o ptree_util.o ptree_delete.o ptree_print.o
_OBJS = get_line.o spawn_process.o
_PARSERS = calculator.parser tree.parser lisp.parser
CFLAGS = -std=c11 -lm -Wall -Werror
LIBDIR = lib
SRCDIR = src
BINDIR = bin
OBJDIR = obj
DBGFLAGS = DEBUG_LOG
.DEFAULT_GOAL = all

OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))
LOBJS = $(patsubst %,$(OBJDIR)/%,$(_LOBJS))
PARSERS = $(patsubst %,$(BINDIR)/%,$(_PARSERS))

$(LIBDIR)/$(LIB): $(LOBJS)
	ar rcs $@ $^

debug: CFLAGS += -g $(addprefix -D, $(DBGFLAGS))
debug: all
test: CFLAGS += -g
test: all

all: $(BINDIR)/$(EXE) $(PARSERS)

$(BINDIR)/$(EXE): $(OBJDIR)/$(EXE).o $(OBJS) $(LIBDIR)/$(LIB)
	$(CC) -o $@ $^ $(CFLAGS)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/%.parser: $(OBJDIR)/%_parser.o $(OBJDIR)/%_interpreter.o $(OBJS) $(LIBDIR)/$(LIB)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm $(OBJDIR)/*.o $(BINDIR)/$(EXE) $(LIBDIR)/$(LIB) $(BINDIR)/*.parser
	
rebuild: clean all
rebug: clean debug
retest: clean test

