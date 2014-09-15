CC = gcc
EXE = calc_parser
_OBJS = parser_llk.o calculator_parser.o ptree_create.o ptree_util.o ptree_delete.o ptree_print.o calculator_interpreter.o
CFLAGS = -std=c11 -lm
OBJDIR = obj
SRCDIR = src
BINDIR = bin
DBGFLAGS = DEBUG_LOG

OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

debug: CFLAGS += -g $(addprefix -D, $(DBGFLAGS))
debug: all
test: CFLAGS += -g
test: all
all: $(BINDIR)/$(EXE)
$(BINDIR)/$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)
clean:
	rm $(OBJDIR)/*.o $(BINDIR)/$(EXE)
rebuild: clean all
rebug: clean debug
retest: clean test
