CC = gcc
EXE = tree.parser
CFLAGS = -std=c11 -lm -Wall -Werror
DBGFLAGS = DEBUG_LOG

.DEFAULT_GOAL = parser

debug: CFLAGS += $(addprefix -D, $(DBGFLAGS))
debug: test
test: CFLAGS += -g
test: parser

parser: obj ../../bin/$(EXE)

obj:
	mkdir $@

../../bin/$(EXE): obj/tree_parser.o ../util/obj/get_line.o ../../lib/parser.a
	$(CC) -o $@ $^ $(CFLAGS)

obj/%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	-rm obj/* ../../bin/$(EXE)

