CC = gcc
EXE = lisp.parser
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

../../bin/$(EXE): obj/lisp_parser.o obj/lisp_interpreter.o obj/lisp_env.o obj/lisp_value.o obj/base_eval.o obj/base_env.o ../util/obj/get_line.o ../../lib/parser.a
	$(CC) -o $@ $^ $(CFLAGS)

obj/%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	-rm obj/* ../../bin/$(EXE)

