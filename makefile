CC = gcc
EXE = calc_parser
CFLAGS = -std=c11
debug: CFLAGS += -g -DDEBUG
debug: all
test: CFLAGS += -g
test: all
all: $(EXE)
$(EXE): parser_llk.o calculator_parser.o ptree_create.o ptree_util.o ptree_delete.o ptree_print.o
	$(CC) $(CFLAGS) -o $@ $^
.c.o:
	$(CC) $(CFLAGS) -c $<
clean:
	rm *.o $(EXE)
rebuild: clean all
rebug: clean debug
retest: clean test
