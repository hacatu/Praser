DEMO_DIR = $(wildcard */)
_DEMO_EXE = $(addsuffix .parser,$(DEMO_DIR))
DEMO_EXE = $(addprefix $(BASE)/$(BIN)/,$(_DEMO_EXE))

all: $(DEMO_EXE)

$(BASE)/$(BIN)/%.parser: $($(wildcard %/*.c):.c=.o)
	$(LD) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<
