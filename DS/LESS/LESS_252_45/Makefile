# ****************************************************************************
#
# Name: VLSPQC
# Auth: Alessandra Dolmeta, Valeria Piscopo - Politecnico di Torino
# Date: September 2025
#
# Algorithm: CROSS
#
# ****************************************************************************

# Compiled application has the same name of its folder
APPLI   = $(shell basename $(CURDIR))
SOURCES = src/canonical.c src/codes.c src/fips202.c src/keccakf1600.c src/LESS.c src/monomial.c src/rng.c src/seedtree.c src/sign.c src/sort.c src/transpose.c src/utils.c
SRC     = $(SOURCES) \
          main.c

INC         = -I include/ -I 
EXTRA_CFLAGS = -g -O2

EXTRA_CFLAGS += -DTEST_VECT_INCLUDE="\"LESS_KAT_252_45.h\""

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
