# ****************************************************************************
#
# Name: VLSPQC
# Auth: Alessandra Dolmeta, Valeria Piscopo - Politecnico di Torino
# Date: September 2025
#
# Algorithm: ML-KEM-512 with Ascon (Kyber-Ascon, K=2)
#
# ****************************************************************************

# Compiled application has the same name of its folder
APPLI   = $(shell basename $(CURDIR))
SOURCES = src/ascon.c src/cbd.c src/indcpa.c src/kem.c src/ntt.c src/poly.c src/polyvec.c src/randombytes.c src/reduce.c src/symmetric-ascon.c src/verify.c
SRC 	= $(SOURCES) \
		  main.c

INC         = -I include/ -I
EXTRA_CFLAGS = -g -O2 -DKYBER_K=2

EXTRA_CFLAGS += -DTEST_VECT_INCLUDE="\"test_vectors_512_ascon.h\""

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
