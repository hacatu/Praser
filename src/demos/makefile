DEMO_DIR = $(wildcard */)
_DEMO_DIR = $(subst _parser/,,$(DEMO_DIR))
_DEMO_EXE = $(addsuffix .parser,$(_DEMO_DIR))
DEMO_EXE = $(addprefix $(BASE)/$(BIN)/,$(_DEMO_EXE))
.SECONDEXPANSION:

all: $(DEMO_EXE)

$(DEMO_EXE): $(BASE)/$(BIN)/%.parser: $$(subst .c,.o,$$(wildcard $(BASE)/src/demos/$$*_parser/*.c))
	@echo $^
	$(LK) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<
