/**
 * @file api.h
 * @brief NIST KEM API used by the HQC_KEM IND-CCA2 scheme
 */

#ifndef API_H
#define API_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define CRYPTO_ALGNAME                      "HQC-192"

#define CRYPTO_SECRETKEYBYTES               4562
#define CRYPTO_PUBLICKEYBYTES               4522
#define CRYPTO_BYTES                        64
#define CRYPTO_CIPHERTEXTBYTES              9026

// As a technicality, the public key is appended to the secret key in order to respect the NIST API.
// Without this constraint, CRYPTO_SECRETKEYBYTES would be defined as 32

int crypto_kem_keypair(unsigned char* pk, unsigned char* sk, const uint8_t *sk_seed, const uint8_t *pk_seed);
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk, const uint64_t *m);
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk);

#endif
