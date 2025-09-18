#ifndef HQC_H
#define HQC_H

/**
 * @file hqc.h
 * @brief Functions of the HQC_PKE IND_CPA scheme
 */

#include <stdint.h>

void hqc_pke_keygen(unsigned char* pk, unsigned char* sk, const uint8_t *sk_seed, const uint8_t *pk_seed);
void hqc_pke_encrypt(uint64_t *u, uint64_t *v, const uint64_t *m, const unsigned char *theta, const unsigned char *pk);
void hqc_pke_decrypt(uint64_t *m, const uint64_t *u, const uint64_t *v, const unsigned char *sk);

#endif