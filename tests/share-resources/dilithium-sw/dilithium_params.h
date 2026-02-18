#ifndef DILITHIUM_PARAMS_H
#define DILITHIUM_PARAMS_H

#include <stdint.h>
#include <stddef.h>

#define DILITHIUM_N 256
#define DILITHIUM_Q 8380417
#define DILITHIUM_ETA 2
#define DILITHIUM_GAMMA1 (1 << 17)

#define SEEDBYTES 32
#define CRHBYTES 64
#define POLY_UNIFORM_NBLOCKS 5

typedef struct {
    int32_t coeffs[DILITHIUM_N];
} poly;

/* Function Prototypes - FIXES THE IMPLICIT DECLARATION ERROR */
unsigned int poly_uniform(poly *a, const uint8_t seed[SEEDBYTES], uint16_t nonce);
unsigned int poly_uniform_eta(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce);
void poly_uniform_gamma1(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce);

/* Golden Reference Values (Seed: 000102...1F, Nonce: 0x0000) */
static const int32_t GOLDEN_UNIFORM[4] = {736173, 563212, 591605, 172935}; 
static const int32_t GOLDEN_ETA[4]     = {1, 0, -2, 2};

#endif