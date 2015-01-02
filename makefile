CC ?= gcc
LK ?= gcc
AR ?= "ar rcs"
MAKE ?= make
MKDIR ?= mkdir
INSTALLDIR ?= /usr

LIBS = parser parser_util m
INCLUDES = $(BASE)/src/parser $(BASE)/src/ptree $(BASE)/src/util
CFLAGS = -c -std=c1x $(addprefix -I,$(INCLUDES))
LDFLAGS = -L$(BASE)/$(LIB) $(addprefix -l,$(LIBS))
LIB = lib
BIN = bin
BASE = $(shell pwd)

.DEFAULT_GOAL = all

dirs:
	[ -d $(LIB) ] || $(MKDIR) $(LIB)
	[ -d $(BIN) ] || $(MKDIR) $(BIN) 

debug: CFLAGS += -g
debug: all

export

clean:
	-rm -r $(BIN)
	-rm -r $(LIB)
	-rm src/*.o
	-rm src/*/*.o
	-rm src/demos/*/*.o

all: dirs lib
	$(MAKE) -C src/demos

.PHONY: lib
lib:
	$(MAKE) -C src

install:
	cp lib/* $(INSTALLDIR)/lib/
	cp src/ptree/ptree.h src/parser/parser.h $(INSTALLDIR)/include/


