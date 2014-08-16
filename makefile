CC = gcc
EXE = calc_parser
CFLAGS = -std=c11
debug: CFLAGS += -g -DDEBUG
debug: all
all: $(EXE)
$(EXE): parser_llk.o calculator_parser.o ptree.o
	$(CC) $(CFLAGS) -o $@ $^
.c.o:
	$(CC) $(CFLAGS) -c $<
clean:
	rm *.o $(EXE)
rebuild: clean all
rebug: clean debug
