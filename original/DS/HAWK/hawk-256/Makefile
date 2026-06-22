# ****************************************************************************
#
# Auth: Alessandra Dolmeta - Politecnico di Torino
# Date: June 2025
# Desc: Build the HAWK-256 KAT test binary
#
# ****************************************************************************

# Compiled application has the same name as its folder
APPLI = $(shell basename $(CURDIR))

SOURCES = src/hawk_kgen.c src/hawk_sign.c src/hawk_vrfy.c  \
          src/ng_fxp.c src/ng_hawk.c src/ng_mp31.c src/ng_ntru.c \
          src/ng_poly.c src/ng_zint31.c src/sha3.c            \
          src/api.c src/rng.c

SRC = $(SOURCES) main.c

INC = -I include/

EXTRA_CFLAGS = -g -O2

# Number of test vectors to run (must be <= N_TVEC defined in the header)
ifdef NTESTS
EXTRA_CFLAGS += -DNTESTS=$(NTESTS)
endif

# Selectively enable/disable test phases (default: all enabled)
TEST_KEY       ?= 1
TEST_SIGN      ?= 1
TEST_SIGN_OPEN ?= 1

# Print intermediate vectors (default: off)
PRINT_VECT ?= 0

EXTRA_CFLAGS += -DTEST_KEY=$(TEST_KEY)
EXTRA_CFLAGS += -DTEST_SIGN=$(TEST_SIGN)
EXTRA_CFLAGS += -DTEST_SIGN_OPEN=$(TEST_SIGN_OPEN)
EXTRA_CFLAGS += -DPRINT_VECT=$(PRINT_VECT)

$(info APPLI:       $(APPLI))
$(info EXTRA_CFLAGS: $(EXTRA_CFLAGS))

CC = gcc

all: $(APPLI)

$(APPLI): $(SRC)
	$(CC) $(SRC) $(INC) $(EXTRA_CFLAGS) -o $(APPLI)

clean:
	rm -f $(APPLI)

.PHONY: all clean
