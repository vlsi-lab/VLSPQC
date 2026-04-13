#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>

// Helper for results
int compare_coeffs(const char* test_name, int idx, int32_t got, int32_t exp);

// Software Test Runners
int test_kyber_ntt_sw();
int test_dilithium_ntt_sw();
int test_kyber_intt_sw();
int test_dilithium_intt_sw();
int test_mq_montymul_sw();
int test_karats_sw();
int test_gf_carryless_sw();

int test_fqmul_sw();
int test_dilithium_reduce32();
int test_dilithium_caddq();

// BFU Hardware Emulator Test Runners
int test_kyber_ntt_bfu();
int test_dilithium_ntt_bfu();
int test_kyber_intt_bfu();
int test_dilithium_intt_bfu();
int test_mq_montymul_bfu();
int test_karats_bfu();
int test_gf_carryless_bfu();

int test_fqmul_bfu();
int test_dilithium_reduce32_bfu();
int test_dilithium_caddq_bfu();

#endif