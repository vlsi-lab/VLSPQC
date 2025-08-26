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
SOURCES = CROSS.c csprng_hash.c fips202.c keccakf1600.c merkle.c pack_unpack.c seedtree.c sign.c
SRC     = $(SOURCES) \
          main.c

INC         = -I include/ -I 
EXTRA_CFLAGS = -g -O2


EXTRA_CFLAGS += -DTEST_VECT_INCLUDE="\"CROSS_KAT_5_RSDPG_s.h\""

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
