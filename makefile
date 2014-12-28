CC = gcc
LD = gcc
AR = ar rcs
MAKE = make
MKDIR = mkdir

LIBS = parser parser_util
INCLUDES = src/parser src/ptree src/util
CFLAGS = -c $(addprefix -I,$(INCLUDES))
LDFLAGS = $(addprefix -l,$(LIBS))
LIB = lib
BIN = bin
BASE = `pwd`

.DEFAULT_GOAL = all

dirs:
	[ -d $(LIB) ] || $(MKDIR) $(LIB)
	[ -d $(BIN) ] || $(MKDIR) $(BIN) 

debug: CFLAGS += -g
debug: all

all: dirs lib
	$(MAKE) -C src/demos

lib:
	$(MAKE) -C src
