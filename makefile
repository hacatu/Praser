CC = gcc
MAKE = make
EXE = parser_repl
LIB = parser.a
_PARSER_LOBJS = parser_position.o parser_accept.o parser_combinators.o parser_parsers.o
_PTREE_LOBJS = ptree_create.o ptree_util.o ptree_delete.o ptree_print.o
_OBJS = get_line.o spawn_process.o
_PARSERS = calculator.parser tree.parser lisp.parser
CFLAGS = -std=c1x -lm -Wall -Werror
LIBDIR = lib
SRCDIR = src
BINDIR = bin
OBJDIR = obj
DBGFLAGS = DEBUG_LOG
MAKE_SUBTARGET =
.DEFAULT_GOAL = all

_LOBJS = $(patsubst %,parser/%,$(_PARSER_LOBJS))
_LOBJS += $(patsubst %,ptree/%,$(_PTREE_LOBJS))
OBJS = $(patsubst %,$(SRCDIR)/util/obj/%,$(_OBJS))
LOBJS = $(patsubst %,$(SRCDIR)/%,$(_LOBJS))
PARSERS = $(patsubst %,$(BINDIR)/%,$(_PARSERS))

$(LIBDIR)/$(LIB): $(LOBJS)
	ar rcs $@ $^

debug: CFLAGS += $(addprefix -D, $(DBGFLAGS))
debug: MAKE_SUBTARGET = debug
debug: test
test: CFLAGS += -g
test: MAKE_SUBTARGET = test
test: all

all: dirs $(BINDIR)/$(EXE) $(LIBDIR)/$(LIB) $(PARSERS)

dirs: $(LIBDIR) $(BINDIR) $(OBJDIR) $(SRCDIR)/util/obj

$(LIBDIR):
	mkdir $@

$(BINDIR):
	mkdir $@

$(OBJDIR):
	mkdir $@

$(SRCDIR)/util/obj:
	mkdir $@

$(BINDIR)/$(EXE): $(OBJDIR)/$(EXE).o $(OBJS) $(LIBDIR)/$(LIB)
	$(CC) -o $@ $^ $(CFLAGS)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(SRCDIR)/util/obj/%.o: $(SRCDIR)/util/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/%.parser:
	$(MAKE) -C $(SRCDIR)/$*_parser $(MAKE_SUBTARGET)

clean: MAKE_SUBTARGET = clean
clean:
	-rm $(OBJDIR)/*.o $(BINDIR)/$(EXE) $(LIBDIR)/$(LIB) $(BINDIR)/*.parser

rebuild: clean all
rebug: clean debug
retest: clean test

