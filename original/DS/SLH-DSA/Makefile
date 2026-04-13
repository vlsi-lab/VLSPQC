# ****************************************************************************
#
# Auth: Alessandra Dolmeta - Politecnico di Torino
# Date: August 2024
# ****************************************************************************

# Compiled application has the same name of its folder
APPLI   = $(shell basename $(CURDIR))
SOURCES = src/address.c src/fips202.c src/wots.c src/utils.c src/fors.c src/sign.c src/hash_shake256.c src/thash_shake256_robust.c
SRC     = $(SOURCES) \
          main.c


INC         = -I include/ -I
EXTRA_CFLAGS = -g -O2


# Add the test parameters to EXTRA_CFLAGS
EXTRA_CFLAGS += -DTEST_KEY=$(TEST_KEY) -DTEST_ENC=$(TEST_ENC) -DTEST_DEC=$(TEST_DEC)
EXTRA_CFLAGS += -DPRINT_VECT=$(PRINT_VECT)
EXTRA_CFLAGS += -DTEST_VECT_INCLUDE="\"test_vectors_shake256_128s.h\""

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
