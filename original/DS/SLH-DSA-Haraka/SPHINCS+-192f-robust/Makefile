# ****************************************************************************
#
# Auth: Alessandra Dolmeta - Politecnico di Torino
# Date: August 2024
# ****************************************************************************

# Compiled application has the same name of its folder
APPLI   = $(shell basename $(CURDIR))
MODE    = $(shell basename $(CURDIR) | sed 's/.*-//')
SOURCES = src/address.c src/fors.c src/haraka.c src/hash_haraka.c src/merkle.c src/randombytes.c src/sign.c src/wots.c src/wotsx1.c src/thash_haraka_$(MODE).c src/utils.c src/utilsx1.c
SRC     = $(SOURCES) \
          main.c


INC         = -I include/ -I
EXTRA_CFLAGS = -g -O2
EXTRA_CFLAGS += -DAPPLI_NAME="\"$(APPLI)\""

# Debug print statements
$(info APPLI: $(APPLI))
$(info EXTRA_CFLAGS: $(EXTRA_CFLAGS))
$(info PRINT_VECT: $(PRINT_VECT))
# Compiler
CC = gcc

# Targets
all: $(APPLI)

$(APPLI): $(SRC)
	$(CC) $(SRC) $(INC) $(EXTRA_CFLAGS) -o $(APPLI)

clean:
	rm -f $(APPLI)

.PHONY: all clean

