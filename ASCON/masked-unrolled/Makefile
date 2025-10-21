# Compiled application has the same name of its folder
APPLI   = $(shell basename $(CURDIR))
SOURCES = src/constants.c src/interleave.c src/printstate.c src/randombytes.c src/shares.c
SRC     = $(SOURCES) \
		main.c
          

INC         = -I ./include/
EXTRA_CFLAGS = -g -O2 

DEBUG_CODE ?= 1
EXTRA_CFLAGS += -DDEBUG_CODE=$(DEBUG_CODE)

# Debug print statements
$(info APPLI: $(APPLI))
$(info EXTRA_CFLAGS: $(EXTRA_CFLAGS))
# Compiler
CC = gcc

# Targets
all: $(APPLI)

$(APPLI): $(SRC)
	$(CC) $(SRC) $(INC) $(EXTRA_CFLAGS) -o $(APPLI)

clean:
	rm -f $(APPLI)

.PHONY: all clean



