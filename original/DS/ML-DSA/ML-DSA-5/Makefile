# ****************************************************************************
#
# Name: VLSPQC
# Auth: Alessandra Dolmeta, Valeria Piscopo - Politecnico di Torino
# Date: September 2025
#
# Algorithm: ML-DSA-5
#
# ****************************************************************************

# Compiled application has the same name of its folder
APPLI   = $(shell basename $(CURDIR))
SOURCES = src/ntt.c src/packing.c src/fips202.c src/randombytes.c src/poly.c src/polyvec.c src/reduce.c src/rounding.c src/sign.c src/symmetric-shake.c
SRC     = $(SOURCES) \
          main.c

INC         = -I include/ -I 
EXTRA_CFLAGS = -g -O2

EXTRA_CFLAGS += -DTEST_VECT_INCLUDE="\"test_vectors_5.h\""


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
