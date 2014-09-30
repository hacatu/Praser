CC = gcc
EXE = calculator.parser
CFLAGS = -std=c11 -lm -Wall -Werror

.DEFAULT_GOAL = parser

parser: obj ../../bin/$(EXE)

obj:
	mkdir $@

../../bin/$(EXE): obj/calculator_parser.o obj/calculator_interpreter.o ../util/obj/get_line.o ../../lib/parser.a
	$(CC) -o $@ $^ $(CFLAGS)

obj/%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm obj/*
	rm ../../bin/$(EXE)
